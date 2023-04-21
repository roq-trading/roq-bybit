/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/outbound_account_info.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("type":"snapshot",)"
                     R"("topic":"outboundAccountInfo",)"
                     R"("ts":"1671025418117",)"
                     R"("data":[{)"
                     R"("e":"outboundAccountInfo",)"
                     R"("E":"1671025418117",)"
                     R"("T":true,)"
                     R"("W":true,)"
                     R"("D":true,)"
                     R"("B":[{)"
                     R"("a":"USDT",)"
                     R"("f":"176.26940354",)"
                     R"("l":"19.413852")"
                     R"(})"
                     R"(])"
                     R"(})"
                     R"(])"
                     R"(})"sv;
}  // namespace

// note! simple parsing not supported due to "data" being array

TEST_CASE("json_outbound_account_info_parser", "[json_outbound_account_info]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::Trade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::OutboundAccountInfo> const &event) override {
      ++count;
      auto &outbound_account_info = event.value;
      CHECK(outbound_account_info.type == json::EventType::SNAPSHOT);
      CHECK(outbound_account_info.topic == "outboundAccountInfo"sv);
      CHECK(outbound_account_info.ts == 1671025418117ms);
      auto &data = outbound_account_info.data;
      CHECK(data.type == "outboundAccountInfo"sv);
      CHECK(data.timestamp == 1671025418117ms);
      CHECK(data.allow_trade == true);
      CHECK(data.allow_withdraw == true);
      CHECK(data.allow_deposit == true);
      CHECK(std::size(data.list) == 1);
      auto &d0 = data.list[0];
      CHECK(d0.asset == "USDT"sv);
      CHECK(d0.available == 176.26940354_a);
      CHECK(d0.locked == 19.413852_a);
    }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::TicketInfo> const &) override { FAIL(); }

    size_t count = 0;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.count == 1);
}
