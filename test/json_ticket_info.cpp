/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/parser.hpp"
#include "roq/bybit/json/ticket_info.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("type":"snapshot",)"
                     R"("topic":"ticketInfo",)"
                     R"("ts":"1671084198535",)"
                     R"("data":[{)"
                     R"("e":"ticketInfo",)"
                     R"("E":"1671084198535",)"
                     R"("s":"BTCUSDT",)"
                     R"("q":"0.0001",)"
                     R"("t":"1671084198521",)"
                     R"("p":"17711.03",)"
                     R"("T":"2290000000032909993",)"
                     R"("o":"1311543650485845504",)"
                     R"("c":"NAAC6QMAAQAAHO1wLww4",)"
                     R"("O":"1311543748884281088",)"
                     R"("a":"44484788",)"
                     R"("A":"9948018",)"
                     R"("m":true,)"
                     R"("S":"BUY")"
                     R"(})"
                     R"(])"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_ticket_info_simple", "[json_ticket_info]") {
  core::Buffer buffer(8192);
  json::TicketInfo obj{MESSAGE, buffer};
}

TEST_CASE("json_ticket_info_parser", "[json_ticket_info]") {
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
    void operator()(Trace<json::OutboundAccountInfo> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::TicketInfo> const &) override { found = true; }

    bool found = false;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.found == true);
}

TEST_CASE("json_ticket_info_maker_1", "[json_ticket_info]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"ticketInfo",)"
                 R"("ts":"1671084198535",)"
                 R"("data":[{)"
                 R"("e":"ticketInfo",)"
                 R"("E":"1671084198535",)"
                 R"("s":"BTCUSDT",)"
                 R"("q":"0.0001",)"
                 R"("t":"1671084198521",)"
                 R"("p":"17711.03",)"
                 R"("T":"2290000000032909993",)"
                 R"("o":"1311543650485845504",)"
                 R"("c":"NAAC6QMAAQAAHO1wLww4",)"
                 R"("O":"1311543748884281088",)"
                 R"("a":"44484788",)"
                 R"("A":"9948018",)"
                 R"("m":true,)"
                 R"("S":"BUY")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::TicketInfo obj{message, buffer};
  CHECK(obj.type == json::EventType::SNAPSHOT);
  CHECK(obj.topic == "ticketInfo"sv);
  CHECK(obj.timestamp == 1671084198535ms);
  auto &data = obj.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "ticketInfo"sv);
  CHECK(d0.event_time == 1671084198535ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.timestamp == 1671084198521ms);
  CHECK(d0.price == 17711.03_a);
  CHECK(d0.trade_id == "2290000000032909993"sv);
  CHECK(d0.order_id == "1311543650485845504"sv);
  CHECK(d0.order_link_id == "NAAC6QMAAQAAHO1wLww4"sv);
  CHECK(d0.opponent_order_id == "1311543748884281088"sv);
  CHECK(d0.account_id == "44484788"sv);
  CHECK(d0.opponent_account_id == "9948018"sv);
  CHECK(d0.is_maker == true);
  CHECK(d0.side == json::Side::BUY);
}

TEST_CASE("json_ticket_info_maker_2", "[json_ticket_info]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"ticketInfo",)"
                 R"("ts":"1671084988155",)"
                 R"("data":[{)"
                 R"("e":"ticketInfo",)"
                 R"("E":"1671084988155",)"
                 R"("s":"BTCUSDT",)"
                 R"("q":"0.0001",)"
                 R"("t":"1671084988131",)"
                 R"("p":"17722.37",)"
                 R"("T":"2290000000032911710",)"
                 R"("o":"1311550338286841088",)"
                 R"("c":"sAAC6wMAAQAAuTr1Xgw4",)"
                 R"("O":"1311550372613019648",)"
                 R"("a":"44484788",)"
                 R"("A":"7067024",)"
                 R"("m":true,)"
                 R"("S":"SELL")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::TicketInfo obj{message, buffer};
  CHECK(obj.type == json::EventType::SNAPSHOT);
  CHECK(obj.topic == "ticketInfo"sv);
  CHECK(obj.timestamp == 1671084988155ms);
  auto &data = obj.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "ticketInfo"sv);
  CHECK(d0.event_time == 1671084988155ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.timestamp == 1671084988131ms);
  CHECK(d0.price == 17722.37_a);
  CHECK(d0.trade_id == "2290000000032911710"sv);
  CHECK(d0.order_id == "1311550338286841088"sv);
  CHECK(d0.order_link_id == "sAAC6wMAAQAAuTr1Xgw4"sv);
  CHECK(d0.opponent_order_id == "1311550372613019648"sv);
  CHECK(d0.account_id == "44484788"sv);
  CHECK(d0.opponent_account_id == "7067024"sv);
  CHECK(d0.is_maker == true);
  CHECK(d0.side == json::Side::SELL);
}

TEST_CASE("json_ticket_info_taker", "[json_ticket_info]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"ticketInfo",)"
                 R"("ts":"1671090927053",)"
                 R"("data":[{)"
                 R"("e":"ticketInfo",)"
                 R"("E":"1671090927053",)"
                 R"("s":"BTCUSDT",)"
                 R"("q":"0.0001",)"
                 R"("t":"1671090927043",)"
                 R"("p":"17707.79",)"
                 R"("T":"2290000000032919989",)"
                 R"("o":"1311600191800917504",)"
                 R"("c":"WgAC6QMAAQAABzcwwQ04",)"
                 R"("O":"1311600186348290560",)"
                 R"("a":"44484788",)"
                 R"("A":"6069086",)"
                 R"("m":false,)"
                 R"("S":"BUY")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::TicketInfo obj{message, buffer};
  CHECK(obj.type == json::EventType::SNAPSHOT);
  CHECK(obj.topic == "ticketInfo"sv);
  CHECK(obj.timestamp == 1671090927053ms);
  auto &data = obj.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "ticketInfo"sv);
  CHECK(d0.event_time == 1671090927053ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.timestamp == 1671090927043ms);
  CHECK(d0.price == 17707.79_a);
  CHECK(d0.trade_id == "2290000000032919989"sv);
  CHECK(d0.order_id == "1311600191800917504"sv);
  CHECK(d0.order_link_id == "WgAC6QMAAQAABzcwwQ04"sv);
  CHECK(d0.opponent_order_id == "1311600186348290560"sv);
  CHECK(d0.account_id == "44484788"sv);
  CHECK(d0.opponent_account_id == "6069086"sv);
  CHECK(d0.is_maker == false);  // here
  CHECK(d0.side == json::Side::BUY);
}
