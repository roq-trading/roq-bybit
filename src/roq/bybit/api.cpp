/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/api.hpp"

#include "roq/exceptions.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === HELPERS ===

namespace {
auto create_api(auto &settings) {
  std::string value{settings.app.api};
  std::ranges::transform(value, std::begin(value), ::toupper);
  auto result = magic_enum::enum_cast<tools::API>(value);
  if (!result.has_value()) {
    log::fatal(R"(Unexpected: api="{}")"sv, value);
  }
  return *result;
}

auto create_category(auto api) -> json::Category {
  switch (api) {
    using enum tools::API;
    case UNDEFINED:
      break;
    case SPOT:
      return json::Category::SPOT;
    case LINEAR:
      return json::Category::LINEAR;
    case INVERSE:
      return json::Category::INVERSE;
    case OPTION:
      return json::Category::OPTION;
  }
  log::fatal("Unexpected"sv);
}
}  // namespace

// === IMPLEMENTATION ===

API API::create(Settings const &settings) {
  auto api = create_api(settings);
  auto category = create_category(api);
  return {
      .market_data{
          .instruments_info = "/v5/market/instruments-info"sv,
          .kline = "/v5/market/kline"sv,
      },
      .simple{
          .account_info = "/v5/account/info"sv,
          .account_wallet_balance = "/v5/account/wallet-balance"sv,
          .position_list = "/v5/position/list"sv,
          .order_realtime = "/v5/order/realtime"sv,
          .execution_list = "/v5/execution/list"sv,
          .order_create = "/v5/order/create"sv,
          .order_amend = "/v5/order/amend"sv,
          .order_cancel = "/v5/order/cancel"sv,
          .order_cancel_all = "/v5/order/cancel-all"sv,
      },
      .api = api,
      .category = category,
  };
}

}  // namespace bybit
}  // namespace roq
