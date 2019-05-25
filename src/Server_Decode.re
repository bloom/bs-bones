let first = (decoder: Json.Decode.decoder('a), json: Js.Json.t): 'a => {
  open Json.Decode;
  let decoded = json |> list(decoder);
  switch (decoded->Belt.List.get(0)) {
  | Some(item) => item
  | None =>
    raise(
      Json.Decode.DecodeError(
        "Expected at least one element in list but got" ++ Obj.magic(json),
      ),
    )
  };
};

// Meant specifically for query strings. Allows the user to pass in
// booleans like this: ?foo | ?foo=1 | ?foo=true | ?foo=false | ?foo=0 etc...
let boolFromQuery = json => {
  switch (Json.Decode.string(json)) {
  | "" => true
  | "1" => true
  | "true" => true
  | "yes" => true
  | "0" => false
  | "false" => false
  | _ => false
  };
};

// Meant specifically for query strings. Allows the user to pass in
// strings like so: ?excludeJournals=c1g132g,c73aw93,d902ufa
let stringArrayFromQuery = json => {
  Json.Decode.string(json) |> Js.String.split(",");
};

let base64 = (json: Js.Json.t): Node.Buffer.t => {
  json |> Json.Decode.string |> Node.Buffer.fromStringWithEncoding(_, `base64);
};

let buffer = (json: Js.Json.t): Node.Buffer.t => {
  let isBuffer: bool = [%bs.raw "json instanceof Buffer"];
  isBuffer
    ? Obj.magic(json)
    : raise(
        Json.Decode.DecodeError(
          "Expected a buffer, but got "
          ++ Js.Json.stringifyAny(json)->Belt.Option.getExn,
        ),
      );
};

// Returns a Some('a) if the json is a non-empty array.
// Fails if the first item of the array is not 'a.
// Returns a None if the array is empty.
//
// This is distinct from Json.Decode.optional, which returns
// None if anything fails at all.
let maybeFirst = (decoder, json) =>
  Json.Decode.(
    json
    |> (
      array(a => a)
      |> map(arr =>
           switch (arr->Belt.Array.get(0)) {
           | Some(j) => Some(decoder(j))
           | None => None
           }
         )
    )
  );

// Decodes a string number into a javascript number.
// Many database drivers return int8 values as strings since
// Javascript's number isn't big enough to hold the top
// possible values in int8.
let bigint = (json: Js.Json.t): float =>
  Json.Decode.oneOf(
    [Json.Decode.float, Json.Decode.(map(float_of_string, string))],
    json,
  );

// The standard Json.Decode library throws an OCaml exception if a decoding
// error is met. OCaml exceptions do not provide stack traces. So this function
// will raise a JS error instead.
let decode = (decoder: Json.Decode.decoder('a), json: Js.Json.t): 'a => {
  switch (decoder(json)) {
  | exception (Json.Decode.DecodeError(e)) => Js.Exn.raiseError(e)
  | v => v
  };
};