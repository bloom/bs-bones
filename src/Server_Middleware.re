open Prom;
type res('ok, 'err) = Belt.Result.t('ok, 'err);

module Types = {
  type t('context, 'newContext) =
    (Express.Request.t, Express.Response.t, 'context) =>
    promWithError('newContext, Express.complete);

  type dataAdder('data, 'context) = t('data => 'context, 'context);

  type handler('context) =
    (Express.Request.t, Express.Response.t, 'context) =>
    prom(Express.complete);
};
include Types;

module Private = {
  [@bs.module "util"]
  external util_inspect: ('a, {. "depth": int}) => string = "inspect";

  let toString = whatever => util_inspect(whatever, {"depth": 100});

  let _middlewareToExpressMiddleware =
      (handler: t(unit, Express.complete)): Express.Middleware.t =>
    Express.PromiseMiddleware.from((_, req, res) =>
      handler(req, res, ())
      ->map(
          fun
          | Belt.Result.Ok(resp) => resp
          | Belt.Result.Error(resp) => resp,
        )
      ->catch(err => {
          Js.log2(
            "An unexpected error was thrown inside of the handler",
            err,
          );
          Express.Response.(res |> sendStatus(StatusCode.InternalServerError))
          ->wrap;
        })
    );

  let _handlerToMiddleware: handler('a) => t('a, Express.complete) =
    (handler, req, res, ctx) =>
      handler(req, res, ctx)->map(a => Belt.Result.Ok(a));

  type _callableExpressMiddleware =
    (Express.Request.t, Express.Response.t, option(exn) => unit) => unit;
  external _callExpressMiddleware:
    Express.Middleware.t => _callableExpressMiddleware =
    "%identity";
};
open Private;

module Compat = {
  let middlewareFromExpress = (em: Express.Middleware.t): t('a, 'a) =>
    (req, res, a) => {
      let (p, resolve) = Prom.make();
      let next =
        fun
        | Some(err) => {
            Js.log2("Failure from wrapped express middleware", err);
            resolve(
              Express.Response.(
                res |> sendStatus(StatusCode.InternalServerError)
              )
              ->Belt.Result.Error,
            );
          }
        | None => resolve(a->Belt.Result.Ok);
      _callExpressMiddleware(em, req, res, next);
      p;
    };
};
include Compat;

module JsonMiddleware = {
  /* By default bs-express only parses json if the headers state that the content type is applicaiton/json.
     Also, the wrapping for bs-express doesn't allow us to specify that the parser should always run.
     So we're going to patch it up a bit here.*/

  module Options = {
    [@bs.deriving abstract]
    type t = {
      [@bs.optional] [@bs.as "type"]
      type_: Express.Request.t => bool,
      [@bs.optional]
      limit: int,
    };
  };

  [@bs.module "express"] [@bs.val]
  external json: Options.t => Express.Middleware.t = "json";
};

let bodyAsJson = size => {
  JsonMiddleware.(json(Options.t(~type_=_ => true, ~limit=size, ())))
  ->middlewareFromExpress;
};

let safeJson: t(unit, unit) = bodyAsJson(5 * 1024 * 1024 /*mb*/);
let bigJson: t(unit, unit) = bodyAsJson(100 * 1024 * 1024 /*mb*/);

let requireQuery: Json.Decode.decoder('a) => dataAdder('query, 'context) =
  (decoder, req, res, ctxBuilder) => {
    switch (decoder(Server_ExpressExt.queryJson(req))) {
    | exception e =>
      Express.Response.(
        res
        |> status(StatusCode.BadRequest)
        |> sendString(
             "Could not decode expected params from query string:"
             ++ toString(e),
           )
      )
      ->wrapError
    | thing => ctxBuilder(thing)->wrapOk
    };
  };

let requireParams: Json.Decode.decoder('a) => dataAdder('params, 'context) =
  (decoder, req, res, ctxBuilder) => {
    let paramsAsJson: Js.Json.t = Obj.magic(req |> Express.Request.params);
    switch (decoder(paramsAsJson)) {
    | exception e =>
      Express.Response.(
        res
        |> status(StatusCode.BadRequest)
        |> sendString(
             "Could not decode expected params from the URL path:"
             ++ toString(e),
           )
      )
      ->wrapError
    | thing => ctxBuilder(thing)->wrapOk
    };
  };

let requireBody: Json.Decode.decoder('b) => dataAdder('b, 'context) =
  (decoder, req, res, ctxBuilder) =>
    switch (req |> Express.Request.bodyJSON) {
    | Some(rawBodyJson) =>
      switch (decoder(rawBodyJson)) {
      | exception (Json.Decode.DecodeError(err)) =>
        Js.log2("Error decoding expected body", rawBodyJson);
        wrapError(
          Express.Response.(
            res
            |> status(StatusCode.BadRequest)
            |> sendString("Error decoding body: " ++ err)
          ),
        );

      | decodedBody => wrapOk(decodedBody->ctxBuilder)
      }
    | None =>
      wrapError(
        Express.Response.(
          res |> status(StatusCode.BadRequest) |> sendString("Body Required")
        ),
      )
    };

let requireToken: dataAdder(string, 'context) =
  (req, res, ctxBuilder) => {
    let header = req |> Express.Request.get("Authorization");
    switch (header) {
    | Some(token) => token->ctxBuilder |> wrapOk
    | None =>
      wrapError(
        Express.Response.(
          res
          |> status(StatusCode.Unauthorized)
          |> sendString("Must include an Authorization header.")
        ),
      )
    };
  };

module Composers = {
  let startWith = (ctxBuilder: 'a): t(unit, 'a) => {
    (_req, _res, _) => {
      ctxBuilder->Belt.Result.Ok->wrap;
    };
  };

  // Use this if your context type is just one value, and not a
  // function with multiple arguments that can be built up through
  // multiple calls.
  let just = (m: t('a, 'b)): t(unit, 'b) =>
    (req, res, _) => {
      m(req, res, a => a);
    };

  let andThen = (m: t('a, 'b), m2: t('b, 'c)): t('a, 'c) =>
    (req, res, a) =>
      m(req, res, a)
      ->flatMap(
          fun
          | Belt.Result.Ok(context) => m2(req, res, context)
          | Belt.Result.Error(response) => Belt.Result.Error(response)->wrap,
        );

  let andThenHandle =
      (middleware: t(unit, 'a), handler: handler('a)): Express.Middleware.t =>
    middleware
    ->andThen(_handlerToMiddleware(handler))
    ->_middlewareToExpressMiddleware;

  let handle = (handler: handler(unit)): Express.Middleware.t =>
    _handlerToMiddleware(handler)->_middlewareToExpressMiddleware;
};
include Composers;

let none = startWith();