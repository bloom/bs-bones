type verb =
  | GET
  | POST
  | PUT
  | DELETE;

module type HandlerConfig = {
  type context;

  let path: string;
  let verb: verb;

  let middleware: Server_Middleware.t(unit, context);
  let handler: Server_Middleware.handler(context);
};

module Make = (Cfg: HandlerConfig) => {
  let use = (router: Express.Router.t) => {
    let fn =
      switch (Cfg.verb) {
      | GET => Express.Router.get
      | POST => Express.Router.post
      | PUT => Express.Router.put
      | DELETE => Express.Router.delete
      };

    router->fn(
      ~path=Cfg.path,
      Cfg.middleware->Server_Middleware.andThenHandle(Cfg.handler),
    );
  };
};