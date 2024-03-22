/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/bybit/shared.hpp"

#include <algorithm>
#include <cctype>

#include <magic_enum.hpp>

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === HELPERS ===

namespace {
auto create_api(auto &settings) {
  std::string value{settings.app.api};
  std::transform(std::begin(value), std::end(value), std::begin(value), ::toupper);
  auto result = magic_enum::enum_cast<tools::API>(value);
  if (!result.has_value())
    log::fatal(R"(Unexpected: api="{}")"sv, value);
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

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, rate_limiter{settings.request.limit, settings.request.limit_interval},
      api{create_api(settings)}, category{create_category(api)}, symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace bybit
}  // namespace roq
