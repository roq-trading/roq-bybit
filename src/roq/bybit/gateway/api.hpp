/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <string_view>

#include "roq/bybit/gateway/settings.hpp"

#include "roq/bybit/tools/api.hpp"

#include "roq/bybit/protocol/json/category.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct ROQ_PUBLIC API final {
  struct {
    std::string_view instruments_info;
    std::string_view kline;
  } market_data;

  struct {
    std::string_view account_info;
    std::string_view account_wallet_balance;
    std::string_view position_list;
    std::string_view order_realtime;
    std::string_view execution_list;
    std::string_view order_create;
    std::string_view order_amend;
    std::string_view order_cancel;
    std::string_view order_cancel_all;
  } simple;

  tools::API api;
  protocol::json::Category category;

  // factory
  static API create(Settings const &);

  static tools::API parse_api(std::string_view const &api);
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
