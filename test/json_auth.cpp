/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"
#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/auth.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("success":true,)"
                     R"("ret_msg":"",)"
                     R"("conn_id":"2aec98fffedccb36-0000000e-00070dd5-439bc55e423b896e-73088429",)"
                     R"("req_id":"1",)"
                     R"("op":"auth")"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_auth_simple", "[json_auth]") {
  core::json::BufferStack buffer{8192, 1};
  json::Auth obj{MESSAGE, buffer};
  CHECK(obj.success == true);
}

TEST_CASE("json_auth_parser", "[json_auth]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::PublicTrade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { found = true; }
    void operator()(Trace<json::Wallet> const &) override { FAIL(); }
    void operator()(Trace<json::Position> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::Execution2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  core::json::BufferStack buffer{8192, 1};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
