/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bybit/shared.hpp"

#include "roq/utils/common.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === IMPLEMENTATION ===

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, settings_time_series_interval{utils::to_interval(settings.time_series.interval)}, api{API::create(settings)},
      rate_limiter{settings.request.limit, settings.request.limit_interval}, symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace bybit
}  // namespace roq
