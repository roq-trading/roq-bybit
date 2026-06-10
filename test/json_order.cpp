/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::Order;

// note! from websocket

TEST_CASE("spot", "[json_order]") {
  auto message = R"({)"
                 R"("id":"460579-1405980802291926784-ff9c1866-f679-4fde-9e43-833c96d09967",)"
                 R"("topic":"order",)"
                 R"("creationTime":1682341972648,)"
                 R"("data":[{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("orderId":"1405980802291926784",)"
                 R"("side":"Buy",)"
                 R"("orderType":"Limit",)"
                 R"("cancelType":"UNKNOWN",)"
                 R"("price":"27061.65",)"
                 R"("qty":"0.001000",)"
                 R"("orderIv":"",)"
                 R"("timeInForce":"GTC",)"
                 R"("orderStatus":"New",)"
                 R"("orderLinkId":"1682341972120",)"
                 R"("lastPriceOnCreated":"",)"
                 R"("reduceOnly":false,)"
                 R"("leavesQty":"",)"
                 R"("leavesValue":"",)"
                 R"("cumExecQty":"0.000000",)"
                 R"("cumExecValue":"0.00000000",)"
                 R"("avgPrice":"",)"
                 R"("blockTradeId":"",)"
                 R"("positionIdx":0,)"
                 R"("cumExecFee":"0",)"
                 R"("createdTime":"1682341972563",)"
                 R"("updatedTime":"",)"
                 R"("rejectReason":"",)"
                 R"("stopOrderType":"",)"
                 R"("triggerPrice":"",)"
                 R"("takeProfit":"",)"
                 R"("stopLoss":"",)"
                 R"("tpTriggerBy":"",)"
                 R"("slTriggerBy":"",)"
                 R"("triggerDirection":0,)"
                 R"("triggerBy":"",)"
                 R"("closeOnTrigger":false,)"
                 R"("category":"spot",)"
                 R"("isLeverage":"0",)"
                 R"("smpType":"None",)"
                 R"("smpGroup":0,)"
                 R"("smpOrderId":"")"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.id == "460579-1405980802291926784-ff9c1866-f679-4fde-9e43-833c96d09967"sv);
    CHECK(obj.topic == "order"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("linear", "[json_order]") {
  auto message = R"({)"
                 R"("topic":"order",)"
                 R"("id":"11eb7e6c3bbe1d1c7ce51f9b84047b3f:fb62413e3563d3b3:0:01",)"
                 R"("creationTime":1682917170027,)"
                 R"("data":[{)"
                 R"("avgPrice":"",)"
                 R"("blockTradeId":"",)"
                 R"("cancelType":"UNKNOWN",)"
                 R"("category":"linear",)"
                 R"("closeOnTrigger":false,)"
                 R"("createdTime":"1682917170023",)"
                 R"("cumExecFee":"0",)"
                 R"("cumExecQty":"0",)"
                 R"("cumExecValue":"0",)"
                 R"("leavesQty":"0.001",)"
                 R"("leavesValue":"27.6734",)"
                 R"("orderId":"af6512ea-74b0-4ac5-959a-9a52d4208b08",)"
                 R"("orderIv":"",)"
                 R"("isLeverage":"",)"
                 R"("lastPriceOnCreated":"28674.10",)"
                 R"("orderStatus":"New",)"
                 R"("orderLinkId":"VQAC6QMAAQAARFkvRM9C",)"
                 R"("orderType":"Limit",)"
                 R"("positionIdx":0,)"
                 R"("price":"27673.40",)"
                 R"("qty":"0.001",)"
                 R"("reduceOnly":false,)"
                 R"("rejectReason":"EC_NoError",)"
                 R"("side":"Buy",)"
                 R"("slTriggerBy":"UNKNOWN",)"
                 R"("stopLoss":"0.00",)"
                 R"("stopOrderType":"UNKNOWN",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("takeProfit":"0.00",)"
                 R"("timeInForce":"GTC",)"
                 R"("tpTriggerBy":"UNKNOWN",)"
                 R"("triggerBy":"UNKNOWN",)"
                 R"("triggerDirection":0,)"
                 R"("triggerPrice":"0.00",)"
                 R"("updatedTime":"1682917170026",)"
                 R"("placeType":"",)"
                 R"("smpType":"None",)"
                 R"("smpGroup":0,)"
                 R"("smpOrderId":"",)"
                 R"("tpSlMode":"UNKNOWN",)"
                 R"("tpLimitPrice":"",)"
                 R"("slLimitPrice":"")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "order"sv);
    CHECK(obj.id == "11eb7e6c3bbe1d1c7ce51f9b84047b3f:fb62413e3563d3b3:0:01"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
