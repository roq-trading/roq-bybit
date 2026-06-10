/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::PublicTrade;

TEST_CASE("simple", "[json_public_trade]") {
  auto message = R"({)"
                 R"("topic":"publicTrade.BTCUSDT",)"
                 R"("ts":1682084122523,)"
                 R"("type":"snapshot",)"
                 R"("data":[{)"
                 R"("i":"2100000000019296063",)"
                 R"("T":1682084122521,)"
                 R"("p":"27200",)"
                 R"("v":"0.000221",)"
                 R"("S":"Buy",)"
                 R"("s":"BTCUSDT",)"
                 R"("BT":false)"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "publicTrade.BTCUSDT"sv);
    CHECK(obj.timestamp == 1682084122523ms);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
