/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <benchmark/benchmark.h>

#include "roq/bybit/json/encoder.hpp"

#include "roq/bybit/tools/crypto.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;

// === CONSTANTS ===

namespace {
auto const PATH = "/spot/v3/private/order"sv;
auto const CREATE_ORDER = CreateOrder{
    .account = "A1"sv,
    .order_id = 1001,
    .exchange = "bybit"sv,
    .symbol = "BTCUSDT"sv,
    .side = Side::BUY,
    .position_effect = {},
    .margin_mode = {},
    .quantity_type = {},
    .max_show_quantity = NaN,
    .order_type = OrderType::LIMIT,
    .time_in_force = TimeInForce::GTC,
    .execution_instructions = {},
    .request_template = {},
    .quantity = 0.001,
    .price = 17123.45,
    .stop_price = NaN,
    .leverage = NaN,
    .routing_id = {},
    .strategy_id = {},
};
auto const ORDER = []() {
  server::oms::Order result;
  result.price_precision.precision = Precision::_2;
  result.quantity_precision.precision = Precision::_5;
  return result;
}();
auto const REQUEST_ID = "rQAC6wMAAQAA9tJBrf43"sv;
auto const LOGIN = "iAj2shx6x6iIb6f0up"sv;
auto const SECRET = "3qFD9aBSKCX6IqgBy4WIAFn0uvE2j3XuI6GP"sv;
}  // namespace

// === IMPLEMENTATION ===

void BM_create_order(benchmark::State &state) {
  std::string buffer;
  server::oms::Order order;
  for (auto _ : state) {
    json::Encoder::place_order(buffer, CREATE_ORDER, order, REQUEST_ID, json::Category::SPOT);
  }
}

BENCHMARK(BM_create_order);

void BM_create_order_and_sign(benchmark::State &state) {
  std::string buffer;
  tools::Crypto crypto{LOGIN, SECRET, 1s};
  for (auto _ : state) {
    auto body = json::Encoder::place_order(buffer, CREATE_ORDER, ORDER, REQUEST_ID, json::Category::SPOT);
    crypto.create_headers_v2(PATH, {}, body, 1671026168138ms);
  }
}

BENCHMARK(BM_create_order_and_sign);
