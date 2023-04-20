/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/cancel_order.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("json_cancel_order_simple", "[json_cancel_order]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("orderId":"1311065929670571776",)"
                 R"("orderLinkId":"cAAC7AMAAQAAyzMH7f43",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("cancelTime":"1671027238030",)"
                 R"("orderPrice":"16939.93",)"
                 R"("orderQty":"0.0001",)"
                 R"("orderType":"LIMIT",)"
                 R"("side":"BUY",)"
                 R"("status":"NEW",)"
                 R"("timeInForce":"GTC",)"
                 R"("accountId":"44484788",)"
                 R"("execQty":"0",)"
                 R"("orderCategory":0)"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1671027238033)"
                 R"(})";
  core::Buffer buffer(8192);
  json::CancelOrder obj{message, buffer};
}
