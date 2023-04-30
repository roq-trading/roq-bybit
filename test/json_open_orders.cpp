/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/open_orders.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const EMPTY = R"({)"
                   R"("retCode":0,)"
                   R"("retMsg":"OK",)"
                   R"("result":{)"
                   R"("list":[],)"
                   R"("nextPageCursor":"",)"
                   R"("category":"linear")"
                   R"(},)"
                   R"("retExtInfo":{},)"
                   R"("time":1682689733889)"
                   R"(})"sv;
}  // namespace

TEST_CASE("json_open_orders_empty", "[json_open_orders]") {
  core::Buffer buffer(8192);
  json::OpenOrders obj{EMPTY, buffer};
}

namespace {
auto const MESSAGE = R"({)"
                     R"("retCode":0,)"
                     R"("retMsg":"OK",)"
                     R"("result":{)"
                     R"("nextPageCursor":"1410314510738196224",)"
                     R"("category":"spot",)"
                     R"("list":[{)"
                     R"("orderLinkId":"iQAC6QMAAQAAz1OYoMFC",)"
                     R"("orderId":"1410314510738196224",)"
                     R"("blockTradeId":"",)"
                     R"("symbol":"BTCUSDT",)"
                     R"("price":"28909.99",)"
                     R"("isLeverage":"0",)"
                     R"("positionIdx":0,)"
                     R"("qty":"0.000040",)"
                     R"("side":"Buy",)"
                     R"("orderStatus":"New",)"
                     R"("cancelType":"UNKNOWN",)"
                     R"("rejectReason":"",)"
                     R"("avgPrice":"0",)"
                     R"("leavesQty":"",)"
                     R"("leavesValue":"",)"
                     R"("cumExecQty":"0",)"
                     R"("cumExecValue":"",)"
                     R"("cumExecFee":"",)"
                     R"("timeInForce":"GTC",)"
                     R"("orderType":"Limit",)"
                     R"("stopOrderType":"",)"
                     R"("orderIv":"",)"
                     R"("triggerPrice":"",)"
                     R"("takeProfit":"",)"
                     R"("stopLoss":"",)"
                     R"("tpTriggerBy":"",)"
                     R"("slTriggerBy":"",)"
                     R"("triggerDirection":0,)"
                     R"("triggerBy":"",)"
                     R"("lastPriceOnCreated":"",)"
                     R"("reduceOnly":false,)"
                     R"("closeOnTrigger":false,)"
                     R"("createdTime":"1682858590868",)"
                     R"("updatedTime":"1682858590879",)"
                     R"("smpType":"None",)"
                     R"("smpGroup":0,)"
                     R"("smpOrderId":"")"
                     R"(})"
                     R"(])"
                     R"(})"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_open_orders_simple", "[json_open_orders]") {
  core::Buffer buffer(8192);
  json::OpenOrders obj{MESSAGE, buffer};
}
