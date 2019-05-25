open Prom;
open Server;

let app = App.make();

app->App.use(Log.middleware());

module Health =
  Handler.Make({
    type context = unit;
    let path = "/";
    let verb = Handler.GET;

    let middleware = Middleware.none;
    let handler = (_req, res, _) => {
      Log.warn("Inside a handler");
      Response.(
        res
        |> status(StatusCode.Ok)
        |> sendString("Heeellllllloooooooooooooo")
      )
      ->wrap;
    };
  });

let router = Router.make();
router->Health.use;
app->App.useRouter(router);
app->App.listen(~port=3000, ~onListen=_ => Log.info("server started"), ());