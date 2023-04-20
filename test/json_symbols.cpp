/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/symbols.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

// note! reduced
TEST_CASE("json_symbols_item", "[json_symbols]") {
  auto message = R"({)"
                 R"("ret_code":0,)"
                 R"("ret_msg":"",)"
                 R"("ext_code":null,)"
                 R"("ext_info":null,)"
                 R"("result":[{)"
                 R"("name":"BTCUSDT",)"
                 R"("alias":"BTCUSDT",)"
                 R"("baseCurrency":"BTC",)"
                 R"("quoteCurrency":"USDT",)"
                 R"("basePrecision":"0.000001",)"
                 R"("quotePrecision":"0.00000001",)"
                 R"("minTradeQuantity":"0.000158",)"
                 R"("minTradeAmount":"10",)"
                 R"("maxTradeQuantity":"4",)"
                 R"("maxTradeAmount":"100000",)"
                 R"("minPricePrecision":"0.01",)"
                 R"("category":1,)"
                 R"("showStatus":true)"
                 R"(},{)"
                 R"("name":"ETHUSDT",)"
                 R"("alias":"ETHUSDT",)"
                 R"("baseCurrency":"ETH",)"
                 R"("quoteCurrency":"USDT",)"
                 R"("basePrecision":"0.00001",)"
                 R"("quotePrecision":"0.0000001",)"
                 R"("minTradeQuantity":"0.00224",)"
                 R"("minTradeAmount":"10",)"
                 R"("maxTradeQuantity":"50",)"
                 R"("maxTradeAmount":"100000",)"
                 R"("minPricePrecision":"0.01",)"
                 R"("category":1,)"
                 R"("showStatus":true)"
                 R"(},{)"
                 R"("name":"SOSUSDT",)"
                 R"("alias":"SOSUSDT",)"
                 R"("baseCurrency":"SOS",)"
                 R"("quoteCurrency":"USDT",)"
                 R"("basePrecision":"1",)"
                 R"("quotePrecision":"0.00000001",)"
                 R"("minTradeQuantity":"154321",)"
                 R"("minTradeAmount":"1",)"
                 R"("maxTradeQuantity":"1666666667",)"
                 R"("maxTradeAmount":"10000",)"
                 R"("minPricePrecision":"0.00000001",)"
                 R"("category":1,)"
                 R"("showStatus":true)"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  [[maybe_unused]] auto obj = core::json::Parser::create<json::Symbols>(message, buffer_);
}
