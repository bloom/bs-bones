// Generated by BUCKLESCRIPT VERSION 5.0.4, PLEASE EDIT WITH CARE
'use strict';

var Pg = require("pg");

function query(pool, payload) {
  return pool.query(payload);
}

var Pool = /* module */[/* query */query];

function query$1(client, payload) {
  return client.query(payload);
}

var Client = /* module */[/* query */query$1];

function poolStats(pool) {
  return "totalCount=" + (String(pool.totalCount) + (",idleCount=" + (String(pool.idleCount) + (",waitingCount=" + (String(pool.waitingCount) + ",")))));
}

var Events = /* module */[/* poolStats */poolStats];

function parseDate(a) {
  return a.toString();
}

Pg.types.setTypeParser(20, (function (prim) {
        return parseInt(prim);
      }));

Pg.types.setTypeParser(1184, parseDate);

var DbTypes = /* module */[/* parseDate */parseDate];

exports.Pool = Pool;
exports.Client = Client;
exports.Events = Events;
exports.DbTypes = DbTypes;
/*  Not a pure module */
