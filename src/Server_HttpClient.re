open Prom;

/*
 You can get documentation for superagent here:
 https://visionmedia.github.io/superagent
  */
type request;
/* TODO: If you're in the browser, binary content will be decoded as a UTF-8 string
   and placed in the "text" attribute, and "body" will be null. That's not great. */
type response = {
  .
  "status": int,
  "body": Js.Json.t,
  "headers": Js.Dict.t(string),
};
type binaryResponse = {
  .
  "status": int,
  "text": string,
  "body": Node.Buffer.t,
  "headers": Js.Dict.t(string),
};

type rawError = {
  .
  "status": option(int),
  /* Use this attribute in the case of an error that doesn't yield
     a status or a response, like a network error */
  "message": string,
};

type error = {
  status: option(int),
  message: string,
};

[@bs.module "superagent"] external get: string => request = "";
[@bs.module "superagent"] external post: string => request = "";
[@bs.module "superagent"]
external postJson: (string, Js.Json.t) => request = "post";
[@bs.module "superagent"]
external postBuffer: (string, Node.Buffer.t) => request = "post";
[@bs.module "superagent"] external put: string => request = "";
[@bs.module "superagent"]
external putJson: (string, Js.Json.t) => request = "put";
[@bs.module "superagent"]
external putBuffer: (string, Node.Buffer.t) => request = "put";
[@bs.module "superagent"] external delete: string => request = "";
[@bs.send] external retry: (request, int) => request = "";

/* Attaches binary data as the body of the request. */
[@bs.send] external sendBuffer: (request, Node.Buffer.t) => request = "send";

let sendBuffer = sendBuffer;

[@bs.send]
external stringField: (request, string, string) => request = "field";

let stringField = stringField;

[@bs.send]
external attachBuffer:
  (
    request,
    string,
    Node.buffer,
    {
      .
      "filename": string,
      "contentType": string,
    }
  ) =>
  request =
  "attach";

let attachBuffer =
    (builder: request, name: string, buff: Node.buffer, contentType: string)
    : request =>
  builder->attachBuffer(
    name,
    buff,
    {"filename": name, "contentType": contentType},
  );

let attachJsonAsFile =
    (builder: request, name: string, payload: Js.Json.t): request =>
  builder->attachBuffer(
    name,
    Node.Buffer.fromString(Js.Json.stringify(payload)),
    "application/json",
  );

module Browser = {
  module Blob = {
    type t;
    [@bs.new] external fromStrings: Js.Array.t(string) => t = "Blob";
    [@bs.new]
    external fromUint8Array:
      (
        Js.Array.t(Js.Typed_array.Uint8Array.t),
        [@bs.as {json|{"type": "vnd/day-one-encrypted"}|json}] _
      ) =>
      t =
      "Blob";
  };

  [@bs.send.pipe: request]
  external attachBlob: (string, Blob.t, {. "filename": string}) => request =
    "attach";

  [@bs.send.pipe: request]
  external attachBlobWithType:
    (
      string,
      Blob.t,
      {
        .
        "filename": string,
        "contentType": string,
      }
    ) =>
    request =
    "attach";

  let attachJsonAsFile =
      (builder: request, name: string, payload: Js.Json.t): request =>
    builder
    |> attachBlobWithType(
         name,
         Blob.fromStrings([|Js.Json.stringify(payload)|]),
         {"filename": name, "contentType": "application/json"},
       );
};

/* Set a header on the request */
[@bs.send] external set: (request, string, string) => request = "";
/* Use this to modify which kinds of responses count as errors. By default, all non-200 responses
   get treated as errors, and show up on the error side of the pom. */
[@bs.send] external ok: (request, response => bool) => request = "";

/* Supports basic auth */
[@bs.send]
external auth: (request, ~username: string, ~password: string) => request = "";

/* "response" sets the amount of time the client will wait for the first byte of the request before abortind. "deadline" sets the total time allowed for the entire request to finish. */
[@bs.send]
external timeout:
  (
    request,
    {
      .
      "response": option(int),
      "deadline": option(int),
    }
  ) =>
  request =
  "";

let toPromise = (r: request): promWithError(response, error) =>
  /* The request object implements Promise. So we can just magic it. */
  Obj.magic(r)
  ->mapError(e => {
      let rawError: rawError = Obj.magic(e);
      {status: rawError##status, message: rawError##message};
    });

let toPromiseExpectBinary =
    (r: request): promWithError(binaryResponse, error) =>
  /* The request object implements Promise. So we can just magic it. */
  Obj.magic(r)
  ->mapError(e => {
      let rawError: rawError = Obj.magic(e);
      {status: rawError##status, message: rawError##message};
    });

let finish = (request: request): promWithError(response, error) =>
  request->toPromise;

let finishBinary = (request: request): promWithError(binaryResponse, error) =>
  request->toPromiseExpectBinary;