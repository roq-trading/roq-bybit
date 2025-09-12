/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bybit/json/encoder.hpp"

#include <fmt/format.h>

#include "roq/decimal.hpp"

#include "roq/bybit/json/map.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {
namespace json {

std::string_view Encoder::place_order(
    std::string &buffer, roq::CreateOrder const &create_order, server::oms::Order const &order, std::string_view const &request_id, Category category) {
  buffer.clear();
  auto side = map(create_order.side).template get<Side>();
  auto order_type = map(create_order.order_type).template get<OrderType>();
  auto time_in_force = static_cast<TimeInForce>(Map{create_order.time_in_force});
  auto reduce_only = create_order.execution_instructions.has(ExecutionInstruction::DO_NOT_INCREASE);
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("category":"{}",)"
      R"("symbol":"{}",)"
      R"("side":"{}",)"
      R"("orderType":"{}",)"
      R"("qty":"{}",)"
      R"("timeInForce":"{}",)"
      R"("reduceOnly":{})"sv,
      category.as_raw_text(),
      create_order.symbol,
      side.as_raw_text(),
      order_type.as_raw_text(),
      Decimal{create_order.quantity, order.quantity_precision.precision},
      time_in_force.as_raw_text(),
      reduce_only);
  if (!std::isnan(create_order.price)) {
    fmt::format_to(std::back_inserter(buffer), R"(,"price":"{}")"sv, Decimal{create_order.price, order.price_precision.precision});
  }
  fmt::format_to(
      std::back_inserter(buffer),
      R"(,"orderLinkId":"{}")"
      R"(}})"sv,
      request_id);
  return buffer;
}

std::string_view Encoder::amend_order(
    std::string &buffer,
    roq::ModifyOrder const &modify_order,
    server::oms::Order const &order,
    [[maybe_unused]] std::string_view const &request_id,
    std::string_view const &previous_request_id,
    Category category) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("category":"{}",)"
      R"("symbol":"{}")"sv,
      category.as_raw_text(),
      order.symbol);
  if (!std::isnan(modify_order.price)) {
    fmt::format_to(std::back_inserter(buffer), R"(,"price":"{}")"sv, Decimal{modify_order.price, order.price_precision.precision});
  }
  if (!std::isnan(modify_order.quantity)) {
    fmt::format_to(std::back_inserter(buffer), R"(,"qty":"{}")"sv, Decimal{modify_order.quantity, order.quantity_precision.precision});
  }
  if (!std::empty(order.external_order_id)) {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"orderId":"{}")"
        R"(}})"sv,
        order.external_order_id);
  } else {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"orderLinkId":"{}")"
        R"(}})"sv,
        previous_request_id);  // XXX not correct -- we need original request_id
  }
  return buffer;
}

std::string_view Encoder::cancel_order(
    std::string &buffer,
    roq::CancelOrder const &,
    server::oms::Order const &order,
    [[maybe_unused]] std::string_view const &request_id,
    std::string_view const &previous_request_id,
    Category category) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("category":"{}",)"
      R"("symbol":"{}")"sv,
      category.as_raw_text(),
      order.symbol);
  if (!std::empty(order.external_order_id)) {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"orderId":"{}")"
        R"(}})"sv,
        order.external_order_id);
  } else {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"orderLinkId":"{}")"
        R"(}})"sv,
        previous_request_id);  // XXX not correct -- we need original request_id
  }
  return buffer;
}

std::string_view Encoder::cancel_all_orders(
    std::string &buffer, roq::CancelAllOrders const &, [[maybe_unused]] std::string_view const &request_id, std::string_view const &symbol, Category category) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("category":"{}",)"
      R"("symbol":"{}")"
      R"(}})"sv,
      category.as_raw_text(),
      symbol);
  return buffer;
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
