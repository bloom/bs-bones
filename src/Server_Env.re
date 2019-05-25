let fromEnv = (key: string, default: string) => {
  let env = Node.Process.process##env;
  switch (env->(Js.Dict.get(key))) {
  | Some(v) => v
  | None => default
  };
};

let envName = fromEnv("NODE_ENV", "development");
let isProduction = envName == "production";
let isDev = envName == "dev" || envName == "development";
let logLevel = fromEnv("LOG_LEVEL", isProduction ? "info" : "silly");
let databaseUrl =
  fromEnv(
    "DATABASE_URL",
    "postgres://postgres:password@localhost:5432/postgres",
  );
let port = fromEnv("PORT", "3000") |> int_of_string;