[@bs.module "./logger.js"] external debug: 'a => unit = "";
[@bs.module "./logger.js"] external debug2: ('a, 'b) => unit = "";
[@bs.module "./logger.js"] external debug3: ('a, 'b, 'c) => unit = "";

[@bs.module "./logger.js"] external info: 'a => unit = "";
[@bs.module "./logger.js"] external info2: ('a, 'b) => unit = "";
[@bs.module "./logger.js"] external info3: ('a, 'b, 'c) => unit = "";

[@bs.module "./logger.js"] external warn: 'a => unit = "";
[@bs.module "./logger.js"] external warn2: ('a, 'b) => unit = "";
[@bs.module "./logger.js"] external warn3: ('a, 'b, 'c) => unit = "";

[@bs.module "./logger.js"] external error: 'a => unit = "";
[@bs.module "./logger.js"] external error2: ('a, 'b) => unit = "";
[@bs.module "./logger.js"] external error3: ('a, 'b, 'c) => unit = "";