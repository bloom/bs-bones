open Prom;
module Bricks = Server_DBBricks;

let _connect = (pool: Server_Pg.pool): prom(Server_Pg.Client.t) => {
  Server_Pg.Client.connect(pool);
};

let perform = (pool: Server_Pg.pool, bricks: Bricks.builder): prom(Js.Json.t) =>
  pool->Server_Pg.Pool.query(bricks->Bricks.toPayload)->map(a => a##rows);

let performRaw = (pool: Server_Pg.pool, ~values=?, text): prom(Js.Json.t) => {
  let fragment =
    switch (values) {
    | Some(v) => Bricks.sqlWithValues(~text, ~values=v)
    | None => Bricks.sql(text)
    };
  pool
  ->Server_Pg.Pool.query({"text": fragment##str, "values": fragment##vals})
  ->map(a => a##rows);
};