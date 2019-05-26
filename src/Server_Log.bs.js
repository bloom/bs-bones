// Generated by BUCKLESCRIPT VERSION 5.0.4, PLEASE EDIT WITH CARE
'use strict';

var LoggerJs = require("./logger.js");
var Caml_option = require("bs-platform/lib/js/caml_option.js");

function debug(prim) {
  LoggerJs.debug(prim);
  return /* () */0;
}

function debug2(prim, prim$1) {
  LoggerJs.debug(prim, prim$1);
  return /* () */0;
}

function debug3(prim, prim$1, prim$2) {
  LoggerJs.debug(prim, prim$1, prim$2);
  return /* () */0;
}

function info(prim) {
  LoggerJs.info(prim);
  return /* () */0;
}

function info2(prim, prim$1) {
  LoggerJs.info(prim, prim$1);
  return /* () */0;
}

function info3(prim, prim$1, prim$2) {
  LoggerJs.info(prim, prim$1, prim$2);
  return /* () */0;
}

function warn(prim) {
  LoggerJs.warn(prim);
  return /* () */0;
}

function warn2(prim, prim$1) {
  LoggerJs.warn(prim, prim$1);
  return /* () */0;
}

function warn3(prim, prim$1, prim$2) {
  LoggerJs.warn(prim, prim$1, prim$2);
  return /* () */0;
}

function error(prim) {
  LoggerJs.error(prim);
  return /* () */0;
}

function error2(prim, prim$1) {
  LoggerJs.error(prim, prim$1);
  return /* () */0;
}

function error3(prim, prim$1, prim$2) {
  LoggerJs.error(prim, prim$1, prim$2);
  return /* () */0;
}

function middleware(prim, prim$1) {
  return LoggerJs.middleware(prim !== undefined ? Caml_option.valFromOption(prim) : undefined);
}

exports.debug = debug;
exports.debug2 = debug2;
exports.debug3 = debug3;
exports.info = info;
exports.info2 = info2;
exports.info3 = info3;
exports.warn = warn;
exports.warn2 = warn2;
exports.warn3 = warn3;
exports.error = error;
exports.error2 = error2;
exports.error3 = error3;
exports.middleware = middleware;
/* ./logger.js Not a pure module */
