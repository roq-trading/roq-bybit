/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("simple", "[json_error]") {
  auto message = R"({)"
                 R"("retCode":10004,)"
                 R"("retMsg":"error sign! origin_string[1682321990138dBW7d0xxu8cbXUl5vq5000category=linear]",)"
                 R"("result":{},)"
                 R"("retExtInfo":{},)"
                 R"("time":1682321990235)"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::Error obj{message, buffers};
  CHECK(obj.ret_code == 10004);
  /*
  // parser
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    void operator()(Trace<json::Error> const &event) override {
      found = true;
      auto &[trace_info, error] = event;
      CHECK(error.ret_code == 10004);
    }
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::PublicTrade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    void operator()(Trace<json::Kline> const &) override { FAIL(); }
    void operator()(Trace<json::Wallet> const &) override { FAIL(); }
    void operator()(Trace<json::Position> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::Execution2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
  */
}
