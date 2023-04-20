/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/book_ticker.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
// note! reduced
auto const MESSAGE = R"({)"
                     R"("data":{)"
                     R"("s":"ETHUSDT",)"
                     R"("bp":"1194.42",)"
                     R"("bq":"1.41539",)"
                     R"("ap":"1194.76",)"
                     R"("aq":"1.0997",)"
                     R"("t":1669300792787)"
                     R"(},)"
                     R"("type":"delta",)"
                     R"("topic":"bookticker.ETHUSDT",)"
                     R"("ts":1669300792788)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_book_ticker_simple", "[json_book_ticker]") {
  core::Buffer buffer(8192);
  json::BookTicker book_ticker{MESSAGE, buffer};
}

TEST_CASE("json_book_ticker_parser", "[json_book_ticker]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::BookTicker> const &) override { found = true; }
    void operator()(Trace<json::OrderBook> const &) override { FAIL(); }
    void operator()(Trace<json::Trade> const &) override { FAIL(); }
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
