/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/gateway/shared.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {
namespace gateway {

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, api{API::create(settings)}, rate_limiter{settings.request.limit, settings.request.limit_interval},
      symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
