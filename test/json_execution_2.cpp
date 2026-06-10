/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::Execution;

TEST_CASE("parser", "[json_execution]") {
  auto message = R"({)"
                 R"("topic":"execution",)"
                 R"("id":"6e2ab9ee56f4c6bb3dc788ba004a5054:663a60ba7623dfef:0:01",)"
                 R"("creationTime":1682924344208,)"
                 R"("data":[{)"
                 R"("blockTradeId":"",)"
                 R"("category":"linear",)"
                 R"("execFee":"0.01714062",)"
                 R"("execId":"a3333bcf-4d02-55f1-a9d2-08a5149064f4",)"
                 R"("execPrice":"28567.70",)"
                 R"("execQty":"0.001",)"
                 R"("execTime":"1682924344203",)"
                 R"("execType":"Trade",)"
                 R"("execValue":"28.5677",)"
                 R"("feeRate":"0.0006",)"
                 R"("indexPrice":"0.00",)"
                 R"("isLeverage":"",)"
                 R"("isMaker":false,)"
                 R"("leavesQty":"0",)"
                 R"("markIv":"",)"
                 R"("markPrice":"28576.05",)"
                 R"("orderId":"787dcaf8-84b4-44b7-b57c-71bdf1bd5420",)"
                 R"("orderLinkId":"nQAC6QMAAQAAnUSX79BC",)"
                 R"("orderPrice":"28567.70",)"
                 R"("orderQty":"0.001",)"
                 R"("orderType":"Limit",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("stopOrderType":"UNKNOWN",)"
                 R"("side":"Buy",)"
                 R"("tradeIv":"",)"
                 R"("underlyingPrice":"",)"
                 R"("closedSize":"0")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "execution"sv);
    CHECK(obj.id == "6e2ab9ee56f4c6bb3dc788ba004a5054:663a60ba7623dfef:0:01"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
