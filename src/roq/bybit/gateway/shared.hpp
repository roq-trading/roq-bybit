/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <vector>

#include "roq/api.hpp"

#include "roq/server.hpp"

#include "roq/core/symbols.hpp"
#include "roq/core/timer_queue.hpp"

#include "roq/core/limit/rate_limiter.hpp"

#include "roq/bybit/gateway/api.hpp"
#include "roq/bybit/gateway/settings.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct Shared final {
  Shared(server::Dispatcher &, Settings const &);

  Shared(Shared const &) = delete;

  server::Dispatcher &dispatcher;

  Settings const &settings;
  API const api;

  core::limit::RateLimiter rate_limiter;

  core::Symbols symbols;
  utils::unordered_set<std::string> all_symbols;

  core::TimerQueue<std::string> time_series_request_queue;

  std::vector<MBPUpdate> bids, asks, final_bids, final_asks;
  std::vector<Trade> trades;
  std::vector<Bar> bars;
  std::vector<Fill> fills;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
