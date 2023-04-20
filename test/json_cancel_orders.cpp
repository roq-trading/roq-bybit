/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/cancel_orders.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("json_cancel_orders_success", "[json_cancel_orders]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("success":"1")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1669269074353)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::CancelOrders>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(obj.result.success == 1);
  CHECK(obj.time == 1669269074353ms);
}
