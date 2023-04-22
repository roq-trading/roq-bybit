/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/shared.hpp"

#include <algorithm>
#include <cctype>

#include <magic_enum.hpp>

#include "roq/logging.hpp"

#include "roq/bybit/flags/flags.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === HELPERS ===

namespace {
auto create_api() {
  std::string value{flags::Flags::api()};
  std::transform(std::begin(value), std::end(value), std::begin(value), ::toupper);
  auto result = magic_enum::enum_cast<API>(value);
  if (!result.has_value())
    log::fatal(R"(Unexpected: api="{}")"sv, value);
  return *result;
}
}  // namespace

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher)
    : dispatcher_{dispatcher}, rate_limiter{flags::Flags::request_limit(), flags::Flags::request_limit_interval()},
      api{create_api()}, symbols{
                             .spot = core::Symbols{flags::Flags::ws_max_subscriptions_per_stream()},
                             .linear = core::Symbols{flags::Flags::ws_max_subscriptions_per_stream()},
                             .inverse = core::Symbols{flags::Flags::ws_max_subscriptions_per_stream()},
                             .option = core::Symbols{flags::Flags::ws_max_subscriptions_per_stream()},
                         } {
  log::debug("api={}"sv, api);
}

}  // namespace bybit
}  // namespace roq
