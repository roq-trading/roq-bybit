/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/parser.hpp"
#include "roq/bybit/json/tickers.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("data":{)"
                     R"("t":1669300793081,)"
                     R"("s":"ETHUSDT",)"
                     R"("o":"1161.66",)"
                     R"("h":"1217.54",)"
                     R"("l":"1150.51",)"
                     R"("c":"1194.43",)"
                     R"("v":"112924.12393",)"
                     R"("qv":"133609050.5631047",)"
                     R"("m":"0.0282")"
                     R"(},)"
                     R"("type":"delta",)"
                     R"("topic":"tickers.ETHUSDT",)"
                     R"("ts":1669300793085)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_tickers_simple", "[json_tickers]") {
  core::Buffer buffer(8192);
  json::Tickers ticks{MESSAGE, buffer};
}

TEST_CASE("json_tickers_parser", "[json_tickers]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::Trade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { found = true; }
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
