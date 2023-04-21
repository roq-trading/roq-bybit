/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/order.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("type":"snapshot",)"
                     R"("topic":"order",)"
                     R"("ts":"1671026168138",)"
                     R"("data":[{)"
                     R"("e":"order",)"
                     R"("E":"1671026168138",)"
                     R"("s":"BTCUSDT",)"
                     R"("c":"rQAC6wMAAQAA9tJBrf43",)"
                     R"("S":"BUY",)"
                     R"("o":"LIMIT",)"
                     R"("f":"GTC",)"
                     R"("q":"0.0001",)"
                     R"("p":"16890.74",)"
                     R"("X":"NEW",)"
                     R"("i":"1311056954682008064",)"
                     R"("M":"0",)"
                     R"("l":"0",)"
                     R"("z":"0",)"
                     R"("L":"0",)"
                     R"("n":"0",)"
                     R"("N":"",)"
                     R"("u":true,)"
                     R"("w":true,)"
                     R"("m":false,)"
                     R"("O":"1671026168128",)"
                     R"("Z":"0",)"
                     R"("A":"0",)"
                     R"("C":false,)"
                     R"("v":"0",)"
                     R"("d":"NO_LIQ")"
                     R"(})"
                     R"(])"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_order_simple", "[json_order]") {
  core::Buffer buffer(8192);
  json::Order obj{MESSAGE, buffer};
}

TEST_CASE("json_order_parser", "[json_order]") {
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
    void operator()(Trace<json::Order> const &) override { found = true; }
    void operator()(Trace<json::TicketInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.found == true);
}

TEST_CASE("json_order_new", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671026168138",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671026168138",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"rQAC6wMAAQAA9tJBrf43",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"16890.74",)"
                 R"("X":"NEW",)"
                 R"("i":"1311056954682008064",)"
                 R"("M":"0",)"
                 R"("l":"0",)"
                 R"("z":"0",)"
                 R"("L":"0",)"
                 R"("n":"0",)"
                 R"("N":"",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671026168128",)"
                 R"("Z":"0",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671026168138ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671026168138ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "rQAC6wMAAQAA9tJBrf43"sv);
  CHECK(d0.side == json::Side::BUY);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 16890.74_a);
  CHECK(d0.order_status == json::OrderStatus::NEW);
  CHECK(d0.order_id == "1311056954682008064"sv);
  CHECK(d0.opponent_order_id == "0");
  CHECK(d0.last_filled_quantity == 0.0_a);
  CHECK(d0.total_filled_quantity == 0.0_a);
  CHECK(d0.last_filled_price == 0.0_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == ""sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);
  CHECK(d0.order_creation_time == 1671026168128ms);
  CHECK(d0.total_filled_value == 0.0_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
}

TEST_CASE("json_order_filled", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671084988205",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671084988205",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"sAAC6wMAAQAAuTr1Xgw4",)"
                 R"("S":"SELL",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"17722.37",)"
                 R"("X":"FILLED",)"
                 R"("i":"1311550338286841088",)"
                 R"("M":"1311550372613019648",)"
                 R"("l":"0.0001",)"
                 R"("z":"0.0001",)"
                 R"("L":"17722.37",)"
                 R"("n":"0",)"
                 R"("N":"USDT",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671084984037",)"
                 R"("Z":"1.772237",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ",)"
                 R"("t":"2290000000032911710")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671084988205ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671084988205ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "sAAC6wMAAQAAuTr1Xgw4"sv);
  CHECK(d0.side == json::Side::SELL);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 17722.37_a);
  CHECK(d0.order_status == json::OrderStatus::FILLED);
  CHECK(d0.order_id == "1311550338286841088"sv);
  CHECK(d0.opponent_order_id == "1311550372613019648");
  CHECK(d0.last_filled_quantity == 0.0001_a);
  CHECK(d0.total_filled_quantity == 0.0001_a);
  CHECK(d0.last_filled_price == 17722.37_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == "USDT"sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);
  CHECK(d0.order_creation_time == 1671084984037ms);
  CHECK(d0.total_filled_value == 1.772237_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
  CHECK(d0.trade_id == "2290000000032911710"sv);
}

// === parital fills ===

TEST_CASE("json_order_partially_filled_1", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671088952783",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671088952783",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"vwAC6gMAAQAAPyh7Sw04",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"17713.79",)"
                 R"("X":"PARTIALLY_FILLED",)"
                 R"("i":"1311583625600415488",)"
                 R"("M":"1311583630147024128",)"
                 R"("l":"0.000048",)"
                 R"("z":"0.000048",)"
                 R"("L":"17713.79",)"
                 R"("n":"0",)"
                 R"("N":"BTC",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671088952194",)"
                 R"("Z":"0.85026192",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ",)"
                 R"("t":"2290000000032917430")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671088952783ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671088952783ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "vwAC6gMAAQAAPyh7Sw04"sv);
  CHECK(d0.side == json::Side::BUY);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 17713.79_a);
  CHECK(d0.order_status == json::OrderStatus::PARTIALLY_FILLED);
  CHECK(d0.order_id == "1311583625600415488"sv);
  CHECK(d0.opponent_order_id == "1311583630147024128");
  CHECK(d0.last_filled_quantity == 0.000048_a);
  CHECK(d0.total_filled_quantity == 0.000048_a);
  CHECK(d0.last_filled_price == 17713.79_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == "BTC"sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);
  CHECK(d0.order_creation_time == 1671088952194ms);
  CHECK(d0.total_filled_value == 0.85026192_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
  CHECK(d0.trade_id == "2290000000032917430"sv);
}

TEST_CASE("json_order_partially_filled_2", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671088953011",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671088953011",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"vwAC6gMAAQAAPyh7Sw04",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"17713.79",)"
                 R"("X":"PARTIALLY_FILLED",)"
                 R"("i":"1311583625600415488",)"
                 R"("M":"1311583632093209088",)"
                 R"("l":"0.00005",)"
                 R"("z":"0.000098",)"
                 R"("L":"17713.79",)"
                 R"("n":"0",)"
                 R"("N":"BTC",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671088952194",)"
                 R"("Z":"1.73595142",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ",)"
                 R"("t":"2290000000032917434")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671088953011ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671088953011ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "vwAC6gMAAQAAPyh7Sw04"sv);
  CHECK(d0.side == json::Side::BUY);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 17713.79_a);
  CHECK(d0.order_status == json::OrderStatus::PARTIALLY_FILLED);
  CHECK(d0.order_id == "1311583625600415488"sv);
  CHECK(d0.opponent_order_id == "1311583632093209088");
  CHECK(d0.last_filled_quantity == 0.00005_a);
  CHECK(d0.total_filled_quantity == 0.000098_a);
  CHECK(d0.last_filled_price == 17713.79_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == "BTC"sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);
  CHECK(d0.order_creation_time == 1671088952194ms);
  CHECK(d0.total_filled_value == 1.73595142_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
  CHECK(d0.trade_id == "2290000000032917434"sv);
}

TEST_CASE("json_order_filled_3", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671088955209",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671088955209",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"vwAC6gMAAQAAPyh7Sw04",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"17713.79",)"
                 R"("X":"FILLED",)"
                 R"("i":"1311583625600415488",)"
                 R"("M":"1311583650539758080",)"
                 R"("l":"0.000002",)"
                 R"("z":"0.0001",)"
                 R"("L":"17713.79",)"
                 R"("n":"0",)"
                 R"("N":"BTC",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671088952194",)"
                 R"("Z":"1.771379",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ",)"
                 R"("t":"2290000000032917450")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671088955209ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671088955209ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "vwAC6gMAAQAAPyh7Sw04"sv);
  CHECK(d0.side == json::Side::BUY);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 17713.79_a);
  CHECK(d0.order_status == json::OrderStatus::FILLED);
  CHECK(d0.order_id == "1311583625600415488"sv);
  CHECK(d0.opponent_order_id == "1311583650539758080");
  CHECK(d0.last_filled_quantity == 0.000002_a);
  CHECK(d0.total_filled_quantity == 0.0001_a);
  CHECK(d0.last_filled_price == 17713.79_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == "BTC"sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);
  CHECK(d0.order_creation_time == 1671088952194ms);
  CHECK(d0.total_filled_value == 1.771379_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
}

TEST_CASE("json_order_taker", "[json_order]") {
  auto message = R"({)"
                 R"("type":"snapshot",)"
                 R"("topic":"order",)"
                 R"("ts":"1671090927089",)"
                 R"("data":[{)"
                 R"("e":"order",)"
                 R"("E":"1671090927089",)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"WgAC6QMAAQAABzcwwQ04",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.0001",)"
                 R"("p":"17715.7",)"
                 R"("X":"FILLED",)"
                 R"("i":"1311600191800917504",)"
                 R"("M":"1311600186348290560",)"
                 R"("l":"0.0001",)"
                 R"("z":"0.0001",)"
                 R"("L":"17707.79",)"
                 R"("n":"0",)"
                 R"("N":"BTC",)"
                 R"("u":true,)"
                 R"("w":true,)"
                 R"("m":false,)"
                 R"("O":"1671090927039",)"
                 R"("Z":"1.770779",)"
                 R"("A":"0",)"
                 R"("C":false,)"
                 R"("v":"0",)"
                 R"("d":"NO_LIQ",)"
                 R"("t":"2290000000032919989")"
                 R"(})"
                 R"(])"
                 R"(})";
  core::Buffer buffer(8192);
  json::Order order{message, buffer};
  CHECK(order.type == json::EventType::SNAPSHOT);
  CHECK(order.topic == "order"sv);
  CHECK(order.timestamp == 1671090927089ms);
  auto &data = order.data;
  REQUIRE(std::size(data) == 1);
  auto &d0 = data[0];
  CHECK(d0.event_type == "order"sv);
  CHECK(d0.event_time == 1671090927089ms);
  CHECK(d0.symbol == "BTCUSDT"sv);
  CHECK(d0.order_link_id == "WgAC6QMAAQAABzcwwQ04"sv);
  CHECK(d0.side == json::Side::BUY);
  CHECK(d0.order_type == json::OrderType::LIMIT);
  CHECK(d0.time_in_force == json::TimeInForce::GTC);
  CHECK(d0.quantity == 0.0001_a);
  CHECK(d0.price == 17715.7_a);
  CHECK(d0.order_status == json::OrderStatus::FILLED);
  CHECK(d0.order_id == "1311600191800917504"sv);
  CHECK(d0.opponent_order_id == "1311600186348290560");
  CHECK(d0.last_filled_quantity == 0.0001_a);
  CHECK(d0.total_filled_quantity == 0.0001_a);
  CHECK(d0.last_filled_price == 17707.79_a);
  CHECK(d0.trading_fee == 0.0_a);
  CHECK(d0.fee_asset_type == "BTC"sv);
  CHECK(d0.normal_trade == true);
  CHECK(d0.working == true);
  CHECK(d0.limit_maker == false);  // ???
  CHECK(d0.order_creation_time == 1671090927039ms);
  CHECK(d0.total_filled_value == 1.770779_a);
  CHECK(d0.opponent_account_id == "0"sv);
  CHECK(d0.is_close == false);
  CHECK(d0.leverage == 0.0_a);
  CHECK(d0.liquidation == json::Liquidation::NO_LIQ);
  CHECK(d0.trade_id == "2290000000032919989"sv);
}
