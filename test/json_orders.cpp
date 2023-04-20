/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/orders.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("json_orders_empty", "[json_orders]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1669268683285)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::Orders>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(std::size(obj.result.list) == 0);
  CHECK(obj.time == 1669268683285ms);
}

TEST_CASE("json_orders_simple", "[json_orders]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[{)"
                 R"("accountId":"44484788",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("orderLinkId":"BAAC6QMAAQAA3V7XNfw3",)"
                 R"("orderId":"1310968091129244928",)"
                 R"("orderPrice":"17726.68",)"
                 R"("orderQty":"0.001",)"
                 R"("execQty":"0",)"
                 R"("cummulativeQuoteQty":"0",)"
                 R"("avgPrice":"0",)"
                 R"("status":"NEW",)"
                 R"("timeInForce":"GTC",)"
                 R"("orderType":"LIMIT",)"
                 R"("side":"BUY",)"
                 R"("stopPrice":"0.0",)"
                 R"("icebergQty":"0.0",)"
                 R"("createTime":1671015574767,)"
                 R"("updateTime":1671015574774,)"
                 R"("isWorking":"1",)"
                 R"("orderCategory":0)"
                 R"(})"
                 R"(])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1671023493481)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::Orders>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(std::size(obj.result.list) == 1);
  auto &item = obj.result.list[0];
  CHECK(item.account_id == "44484788"sv);
  CHECK(item.symbol == "BTCUSDT"sv);
  CHECK(item.order_link_id == "BAAC6QMAAQAA3V7XNfw3"sv);
  CHECK(item.order_id == "1310968091129244928"sv);
  CHECK(item.order_price == 17726.68_a);
  CHECK(item.order_qty == 0.001_a);
  CHECK(item.exec_qty == 0.0_a);
  CHECK(item.cummulative_quote_qty == 0.0_a);
  CHECK(item.avg_price == 0.0_a);
  CHECK(item.status == json::OrderStatus::NEW);
  CHECK(item.time_in_force == json::TimeInForce::GTC);
  CHECK(item.order_type == json::OrderType::LIMIT);
  CHECK(item.side == json::Side::BUY);
  CHECK(item.stop_price == 0.0_a);
  CHECK(item.iceberg_qty == 0.0_a);
  CHECK(item.create_time == 1671015574767ms);
  CHECK(item.update_time == 1671015574774ms);
  CHECK(item.is_working == 1);
  CHECK(item.order_category == 0);
  CHECK(obj.time == 1671023493481ms);
}
