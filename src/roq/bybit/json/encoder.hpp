/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/cancel_all_orders.hpp"
#include "roq/cancel_order.hpp"
#include "roq/create_order.hpp"
#include "roq/modify_order.hpp"

#include "roq/server/oms/order.hpp"

#include "roq/bybit/json/category.hpp"

namespace roq {
namespace bybit {
namespace json {

struct Encoder final {
  static std::string_view place_order(std::string &buffer, roq::CreateOrder const &, server::oms::Order const &, std::string_view const &request_id, Category);

  static std::string_view amend_order(
      std::string &buffer,
      roq::ModifyOrder const &,
      server::oms::Order const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id,
      Category);

  static std::string_view cancel_order(
      std::string &buffer,
      roq::CancelOrder const &,
      server::oms::Order const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id,
      Category);

  static std::string_view cancel_all_orders(
      std::string &buffer, roq::CancelAllOrders const &, std::string_view const &request_id, std::string_view const &symbol, Category);
};

}  // namespace json
}  // namespace bybit
}  // namespace roq
