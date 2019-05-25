let queryJson = (req: Express.Request.t): Js.Json.t =>
  Obj.magic(Express.Request.query(req));

[@bs.module]
external middlewareAsComplete:
  (Express.Middleware.t, Express.Request.t, Express.Response.t) =>
  Js.Promise.t(Express.complete) =
  "./middlewareAsComplete.js";