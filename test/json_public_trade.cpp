/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

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
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::PublicTrade obj{message, buffers};
  CHECK(obj.topic == "publicTrade.BTCUSDT"sv);
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::PublicTrade> const &event) override {
      found = true;
      auto &[trace_info, public_trade] = event;
      CHECK(public_trade.topic == "publicTrade.BTCUSDT"sv);
    }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::Wallet> const &) override { FAIL(); }
    void operator()(Trace<json::Position> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::Execution2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
