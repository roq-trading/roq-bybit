/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/parser.hpp"
#include "roq/bybit/json/trade.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("data":{)"
                     R"("v":"2280000000027001097",)"
                     R"("t":1669300793081,)"
                     R"("p":"1194.43",)"
                     R"("q":"1.2558",)"
                     R"("m":false)"
                     R"(},)"
                     R"("type":"delta",)"
                     R"("topic":"trade.ETHUSDT",)"
                     R"("ts":1669300793086)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_trade_simple", "[json_trade]") {
  core::Buffer buffer(8192);
  json::Trade trade{MESSAGE, buffer};
}

TEST_CASE("json_trade_parser", "[json_trade]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::BookTicker> const &) override { FAIL(); }
    void operator()(Trace<json::OrderBook> const &) override { FAIL(); }
    void operator()(Trace<json::Trade> const &) override { found = true; }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::OutboundAccountInfo> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::TicketInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.found == true);
}

// "type" was added
TEST_CASE("json_trade_20230221", "[json_trade]") {
  auto const message = R"({)"
                       R"("topic":"trade.ETHUSDT",)"
                       R"("ts":1676948743908,)"
                       R"("type":"snapshot",)"
                       R"("data":{)"
                       R"("v":"2280000000035612654",)"
                       R"("t":1676948743906,)"
                       R"("p":"1701.2",)"
                       R"("q":"3.72",)"
                       R"("m":true,)"
                       R"("type":"1")"
                       R"(})"
                       R"(})"sv;
  core::Buffer buffer(8192);
  json::Trade trade{message, buffer};
}
