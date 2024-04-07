/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/bybit/shared.hpp"

#include <algorithm>
#include <cctype>

#include <magic_enum.hpp>

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, api{API::create(settings)},
      rate_limiter{settings.request.limit, settings.request.limit_interval},
      symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace bybit
}  // namespace roq
