/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::Position;

// note! from websocket

TEST_CASE("linear", "[json_position]") {
  auto message = R"({)"
                 R"("topic":"position",)"
                 R"("id":"5fa7fca971e8f4c432d9a35e8e30ac54:6f1e96ddc1d9062d:0:01",)"
                 R"("creationTime":1682919303680,)"
                 R"("data":[{)"
                 R"("bustPrice":"0.10",)"
                 R"("category":"linear",)"
                 R"("createdTime":"1644842313438",)"
                 R"("cumRealisedPnl":"-3.63826074",)"
                 R"("entryPrice":"32685.25",)"
                 R"("leverage":"10",)"
                 R"("liqPrice":"0.10",)"
                 R"("markPrice":"28502.11",)"
                 R"("positionBalance":"14.93863007",)"
                 R"("positionIdx":0,)"
                 R"("positionMM":"0.000001",)"
                 R"("positionIM":"0.653705",)"
                 R"("positionStatus":"Normal",)"
                 R"("positionValue":"65.3705",)"
                 R"("riskId":1,)"
                 R"("riskLimitValue":"2000000",)"
                 R"("side":"Buy",)"
                 R"("size":"0.002",)"
                 R"("stopLoss":"0.00",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("takeProfit":"0.00",)"
                 R"("tpSlMode":"Full",)"
                 R"("tradeMode":0,)"
                 R"("autoAddMargin":0,)"
                 R"("trailingStop":"0.00",)"
                 R"("unrealisedPnl":"-8.36628",)"
                 R"("updatedTime":"1682919303679",)"
                 R"("adlRankIndicator":2)"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "position"sv);
    CHECK(obj.id == "5fa7fca971e8f4c432d9a35e8e30ac54:6f1e96ddc1d9062d:0:01"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("none", "[json_position]") {
  auto message = R"({)"
                 R"("topic":"position",)"
                 R"("id":"f127201edb617010e1ab3e10b12db776:404381ec7fcd40ee:0:01",)"
                 R"("creationTime":1683003561820,)"
                 R"("data":[{)"
                 R"("bustPrice":"0.00",)"
                 R"("category":"linear",)"
                 R"("createdTime":"1644842313438",)"
                 R"("cumRealisedPnl":"-12.59652097",)"
                 R"("entryPrice":"0",)"
                 R"("leverage":"10",)"
                 R"("liqPrice":"0.00",)"
                 R"("markPrice":"27930.39",)"
                 R"("positionBalance":"0",)"
                 R"("positionIdx":0,)"
                 R"("positionMM":"0",)"
                 R"("positionIM":"0",)"
                 R"("positionStatus":"Normal",)"
                 R"("positionValue":"0",)"
                 R"("riskId":1,)"
                 R"("riskLimitValue":"2000000",)"
                 R"("side":"None",)"
                 R"("size":"0",)"
                 R"("stopLoss":"0.00",)"
                 R"("symbol":"BTCUSDT",)"
                 R"("takeProfit":"0.00",)"
                 R"("tpSlMode":"Full",)"
                 R"("tradeMode":0,)"
                 R"("autoAddMargin":0,)"
                 R"("trailingStop":"0.00",)"
                 R"("unrealisedPnl":"0",)"
                 R"("updatedTime":"1683003561818",)"
                 R"("adlRankIndicator":0)"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "position"sv);
    CHECK(obj.id == "f127201edb617010e1ab3e10b12db776:404381ec7fcd40ee:0:01"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
