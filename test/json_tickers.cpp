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
                     R"("topic":"tickers.BTCUSDT",)"
                     R"("ts":1682083233096,)"
                     R"("type":"snapshot",)"
                     R"("cs":792472763,)"
                     R"("data":{)"
                     R"("symbol":"BTCUSDT",)"
                     R"("lastPrice":"27579.99",)"
                     R"("highPrice24h":"35000",)"
                     R"("lowPrice24h":"26900",)"
                     R"("prevPrice24h":"29005.97",)"
                     R"("volume24h":"240.62801",)"
                     R"("turnover24h":"6905439.32558533",)"
                     R"("price24hPcnt":"-0.0492",)"
                     R"("usdIndexPrice":"28190.58008011")"
                     R"(})"
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
    void operator()(Trace<json::PublicTrade> const &) override { FAIL(); }
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
