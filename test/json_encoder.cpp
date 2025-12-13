/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/encoder.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

// === CONSTANTS ===

namespace {
auto SYMBOL = "BTCUSDT"sv;
}

// === HELPERS ===

namespace {
auto create_order_helper() {
  auto order = server::oms::Order{};
  order.symbol = SYMBOL;
  order.price_precision.precision = Precision::_2;
  order.quantity_precision.precision = Precision::_4;
  return order;
}
}  // namespace

// create-order

TEST_CASE("create_order", "[json_encoder]") {
  std::string buffer;
  auto create_order = CreateOrder{
      .account = "A1"sv,
      .order_id = 1000,
      .exchange = "bybit",
      .symbol = SYMBOL,
      .side = Side::BUY,
      .position_effect = {},
      .margin_mode = {},
      .quantity_type = {},
      .max_show_quantity = NaN,
      .order_type = OrderType::LIMIT,
      .time_in_force = TimeInForce::GTC,
      .execution_instructions = {},
      .request_template = {},
      .quantity = 1.2345,
      .price = 23456.78,
      .stop_price = NaN,
      .leverage = NaN,
      .routing_id = {},
      .strategy_id = {},
  };
  auto order = create_order_helper();
  auto request_id = "1234"sv;
  json::Encoder::place_order(buffer, create_order, order, request_id, json::Category::SPOT);
  auto expected = R"({)"
                  R"("category":"spot",)"
                  R"("symbol":"BTCUSDT",)"
                  R"("side":"Buy",)"
                  R"("orderType":"Limit",)"
                  R"("qty":"1.2345",)"
                  R"("timeInForce":"GTC",)"
                  R"("reduceOnly":false,)"
                  R"("price":"23456.78",)"
                  R"("orderLinkId":"1234")"
                  R"(})";
  CHECK(buffer == expected);
}

// modify-order

TEST_CASE("modify_order_price", "[json_encoder]") {
  std::string buffer;
  auto modify_order = ModifyOrder{
      .account = "A1"sv,
      .order_id = 1000,
      .request_template = {},
      .quantity = NaN,
      .price = 23456.78,
      .routing_id = {},
      .version = {},
      .conditional_on_version = {},
  };
  auto order = create_order_helper();
  auto request_id = "2345"sv;
  auto previous_request_id = "1234"sv;
  json::Encoder::amend_order(buffer, modify_order, order, request_id, previous_request_id, json::Category::SPOT);
  auto expected = R"({)"
                  R"("category":"spot",)"
                  R"("symbol":"BTCUSDT",)"
                  R"("price":"23456.78",)"
                  R"("orderLinkId":"1234")"
                  R"(})";
  CHECK(buffer == expected);
}

TEST_CASE("modify_order_quantity", "[json_encoder]") {
  std::string buffer;
  auto modify_order = ModifyOrder{
      .account = "A1"sv,
      .order_id = 1000,
      .request_template = {},
      .quantity = 1.2345,
      .price = NaN,
      .routing_id = {},
      .version = {},
      .conditional_on_version = {},
  };
  auto order = create_order_helper();
  auto request_id = "2345"sv;
  auto previous_request_id = "1234"sv;
  json::Encoder::amend_order(buffer, modify_order, order, request_id, previous_request_id, json::Category::SPOT);
  auto expected = R"({)"
                  R"("category":"spot",)"
                  R"("symbol":"BTCUSDT",)"
                  R"("qty":"1.2345",)"
                  R"("orderLinkId":"1234")"
                  R"(})";
  CHECK(buffer == expected);
}

TEST_CASE("modify_order_both", "[json_encoder]") {
  std::string buffer;
  auto modify_order = ModifyOrder{
      .account = "A1"sv,
      .order_id = 1000,
      .request_template = {},
      .quantity = 1.2345,
      .price = 23456.78,
      .routing_id = {},
      .version = {},
      .conditional_on_version = {},
  };
  auto order = create_order_helper();
  auto request_id = "2345"sv;
  auto previous_request_id = "1234"sv;
  json::Encoder::amend_order(buffer, modify_order, order, request_id, previous_request_id, json::Category::SPOT);
  auto expected = R"({)"
                  R"("category":"spot",)"
                  R"("symbol":"BTCUSDT",)"
                  R"("price":"23456.78",)"
                  R"("qty":"1.2345",)"
                  R"("orderLinkId":"1234")"
                  R"(})";
  CHECK(buffer == expected);
}

// cancel-order

TEST_CASE("cancel_order", "[json_encoder]") {
  std::string buffer;
  auto cancel_order = CancelOrder{
      .account = "A1"sv,
      .order_id = 1000,
      .request_template = {},
      .routing_id = {},
      .version = {},
      .conditional_on_version = {},
  };
  auto order = create_order_helper();
  auto request_id = "2345"sv;
  auto previous_request_id = "1234"sv;
  json::Encoder::cancel_order(buffer, cancel_order, order, request_id, previous_request_id, json::Category::SPOT);
  auto expected = R"({)"
                  R"("category":"spot",)"
                  R"("symbol":"BTCUSDT",)"
                  R"("orderLinkId":"1234")"
                  R"(})";
  CHECK(buffer == expected);
}
