const cls = require("cls-hooked");
const LogFmtr = require("logfmtr");

const requestContext = cls.createNamespace("request");

module.exports.middleware = getContext => (req, res, next) => {
  // GetContext should be a function that, given a request, returns
  // an object of information to add to any log statement after this.
  const log = LogFmtr.default({}, getContext ? getContext(req) : {});
  req.log = log;

  requestContext.run(() => {
    // Set the logger on the async context, so that the global logging
    // function can use it when available.
    requestContext.set("logger", log);
    LogFmtr.middleware(req, res, next);
  });
};

let defaultLogger = LogFmtr.default();
let getLogger = () => {
  const maybeLogger = requestContext.get("logger");
  maybeLogger ? maybeLogger : defaultLogger;
};

module.exports.debug = (...args) => {
  getLogger().debug(...args);
};

module.exports.info = (...args) => {
  getLogger().info(...args);
};

module.exports.warn = (...args) => {
  getLogger().warn(...args);
};

module.exports.error = (...args) => {
  getLogger().error(...args);
};
