/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/trades.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("json_trades_empty", "[json_trades]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1669268683470)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::Trades>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(std::size(obj.result.list) == 0);
  CHECK(obj.time == 1669268683470ms);
}

TEST_CASE("json_trades_simple", "[json_trades]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311543749010091264",)"
                 R"("orderId":"1311543650485845504",)"
                 R"("tradeId":"2290000000032909993",)"
                 R"("orderPrice":"17711.03",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671084198521",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311543748884281088",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671084198534")"
                 R"(})"
                 R"(])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1671084425949)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  auto obj = core::json::Parser::create<json::Trades>(message, buffer_);
  CHECK(obj.ret_code == 0);
  CHECK(obj.ret_msg == "OK"sv);
  CHECK(std::size(obj.result.list) == 1);
  CHECK(obj.time == 1671084425949ms);
}

TEST_CASE("json_trades_multiple", "[json_trade]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311600191935198976",)"
                 R"("orderId":"1311600191800917504",)"
                 R"("tradeId":"2290000000032919989",)"
                 R"("orderPrice":"17707.79",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671090927043",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"1",)"
                 R"("matchOrderId":"1311600186348290560",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671090927055")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311596262702759424",)"
                 R"("orderId":"1311596262358871296",)"
                 R"("tradeId":"2290000000032919128",)"
                 R"("orderPrice":"17731.31",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"USDT",)"
                 R"("creatTime":"1671090458641",)"
                 R"("isBuyer":"1",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311596262568552448",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671090458654")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311583650623633152",)"
                 R"("orderId":"1311583625600415488",)"
                 R"("tradeId":"2290000000032917450",)"
                 R"("orderPrice":"17713.79",)"
                 R"("orderQty":"0.000002",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671088955169",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311583650539758080",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671088955177")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311583632193872384",)"
                 R"("orderId":"1311583625600415488",)"
                 R"("tradeId":"2290000000032917434",)"
                 R"("orderPrice":"17713.79",)"
                 R"("orderQty":"0.00005",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671088952970",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311583632093209088",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671088952980")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311583630247715328",)"
                 R"("orderId":"1311583625600415488",)"
                 R"("tradeId":"2290000000032917430",)"
                 R"("orderPrice":"17713.79",)"
                 R"("orderQty":"0.000048",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671088952740",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311583630147024128",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671088952748")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311576552300896768",)"
                 R"("orderId":"1311576551856368896",)"
                 R"("tradeId":"2290000000032916487",)"
                 R"("orderPrice":"17724.9",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"USDT",)"
                 R"("creatTime":"1671088108971",)"
                 R"("isBuyer":"1",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311576552108046080",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671088108991")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311550372948500992",)"
                 R"("orderId":"1311550338286841088",)"
                 R"("tradeId":"2290000000032911710",)"
                 R"("orderPrice":"17722.37",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"USDT",)"
                 R"("creatTime":"1671084988131",)"
                 R"("isBuyer":"1",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311550372613019648",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671084988169")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("id":"1311543749010091264",)"
                 R"("orderId":"1311543650485845504",)"
                 R"("tradeId":"2290000000032909993",)"
                 R"("orderPrice":"17711.03",)"
                 R"("orderQty":"0.0001",)"
                 R"("execFee":"0",)"
                 R"("feeTokenId":"BTC",)"
                 R"("creatTime":"1671084198521",)"
                 R"("isBuyer":"0",)"
                 R"("isMaker":"0",)"
                 R"("matchOrderId":"1311543748884281088",)"
                 R"("makerRebate":"0",)"
                 R"("executionTime":"1671084198534")"
                 R"(})"
                 R"(])"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1671095054754)"
                 R"(})";
  core::Buffer buffer(8192);
  core::json::Buffer buffer_(buffer);
  [[maybe_unused]] auto obj = core::json::Parser::create<json::Trades>(message, buffer_);
}
