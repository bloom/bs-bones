open Prom.Infix;
open Belt;

module type TableConfig = {
  let name: string;
  let pool: Server_Pg.pool;
};

module Utils = {
  let getPgPath = (path: list(string)) => {
    "{" ++ (path |> List.toArray |> Js.Array.joinWith(",")) ++ "}";
  };

  let getStack = () => {
    let error: Js.Exn.t = [%bs.raw "new Error()"];
    Js.Exn.stack(error)->Option.getWithDefault("No trace could be provided");
  };
};

module Table = (CFG: TableConfig) => {
  module Bricks = Server_DBBricks;
  include Bricks.Raw;
  include Bricks.Types;
  include Bricks.Where;
  include Bricks.Filters;
  include Bricks.Transformers;
  include Bricks.OnConflict;
  include Bricks.To;

  let select = (columns: string): Bricks.builder => {
    Bricks.(select(columns)->from(CFG.name));
  };

  let selectIn = (~column: string, path: list(string)): Bricks.builder => {
    Bricks.select(column ++ "#>" ++ Utils.getPgPath(path))
    ->Bricks.from(CFG.name);
  };

  let update = (vs: Js.t('a)): Bricks.builder => {
    Bricks.(update(~table=CFG.name, vs));
  };
  let updateJson = (vs: Js.Json.t): Bricks.builder => {
    Bricks.(updateJson(~table=CFG.name, vs));
  };

  let setIn =
      (~column: string, ~path: list(string), value: 'a): Bricks.builder => {
    Bricks.updateTableOnly(CFG.name)
    ->Bricks.set(
        ~column,
        Bricks.sqlWithValues(
          ~text="jsonb_set($1, $2, $3)",
          ~values=`Three((column, Utils.getPgPath(path), value)),
        ),
      );
  };

  let _insert = (vs: 'a): Bricks.builder => {
    Bricks.(insertInto(CFG.name)->anyValue(vs));
  };
  let insert: Js.t('a) => Bricks.builder = _insert;
  let insertJson: Js.Json.t => Bricks.builder = _insert;

  let _upsert = (vs: 'a): Bricks.builder => {
    Bricks.(insertInto(CFG.name)->anyValue(vs))->onConflict("id")->doUpdate;
  };
  let upsert: Js.t('a) => Bricks.builder = _upsert;
  let upsertJson: Js.Json.t => Bricks.builder = _upsert;

  let delete = (): Bricks.builder => {
    Bricks.(delete()->from(CFG.name));
  };

  let perform = (builder, decoder) => {
    Server_DBAbstract.perform(CFG.pool, builder)
    <!> (
      err => {
        Server_Log.error3("Failed DB request", Utils.getStack(), err);
      }
    )
    <$> Server_Decode.decode(decoder)
    <!> (
      err => {
        Server_Log.error3(
          "Failed to decode from database",
          Utils.getStack(),
          err,
        );
      }
    );
  };

  let maybeOne = (builder, decoder) => {
    Server_DBAbstract.perform(CFG.pool, builder)
    <!> (
      err => {
        Server_Log.error3("Failed DB request", Utils.getStack(), err);
      }
    )
    <$> Server_Decode.(decode(maybeFirst(decoder)))
    <!> (
      err => {
        Server_Log.error3(
          "Failed to decode from database",
          Utils.getStack(),
          err,
        );
      }
    );
  };

  let one = (builder, decoder) => {
    Server_DBAbstract.perform(CFG.pool, builder)
    <!> (
      err => {
        Server_Log.error3("Failed DB request", Utils.getStack(), err);
      }
    )
    <$> Server_Decode.(decode(first(decoder)))
    <!> (
      err => {
        Server_Log.error3(
          "Failed to decode from database",
          Utils.getStack(),
          err,
        );
      }
    );
  };
};