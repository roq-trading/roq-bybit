/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/parser.hpp"
#include "roq/bybit/json/pong.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("op":"pong",)"
                     R"("args":[1671422637043],)"
                     R"("req_id":"312243545551713",)"
                     R"("conn_id":"cfe843f5")"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_pong_simple", "[json_pong]") {
  core::Buffer buffer(8192);
  json::Pong obj{MESSAGE, buffer};
}

TEST_CASE("json_pong_parser", "[json_pong]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { found = true; }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::BookTicker> const &) override { FAIL(); }
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
