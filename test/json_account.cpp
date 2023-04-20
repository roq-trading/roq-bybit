/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/account.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("json_account_simple", "[json_account]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("balances":[{)"
                 R"("coin":"USDT",)"
                 R"("coinId":"USDT",)"
                 R"("total":"200.01",)"
                 R"("free":"200.01",)"
                 R"("locked":"0")"
                 R"(})"
                 R"(])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1669209528407)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::Account>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(std::size(obj.result.balances) == 1);
  auto &b0 = obj.result.balances[0];
  CHECK(b0.coin == "USDT"sv);
  CHECK(b0.coin_id == "USDT"sv);
  CHECK(b0.total == 200.01_a);
  CHECK(b0.free == 200.01_a);
  CHECK(b0.locked == 0.0_a);
  CHECK(obj.time == 1669209528407ms);
}
