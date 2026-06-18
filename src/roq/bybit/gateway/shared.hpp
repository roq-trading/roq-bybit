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

  auto discard_symbol(std::string_view const &name) const { return dispatcher.discard_symbol(name); }

  template <typename... Args>
  auto operator()(Args &&...args) {
    return dispatcher(std::forward<Args>(args)...);
  }

  template <typename... Args>
  auto get_ref_data(Args &&...args) {
    return dispatcher.get_ref_data(std::forward<Args>(args)...);
  }

 public:
  server::Dispatcher &dispatcher;
  Settings const &settings;
  API const api;

  core::limit::RateLimiter rate_limiter;

  core::Symbols symbols;
  utils::unordered_set<std::string> all_symbols;

  core::TimerQueue<std::string> time_series_request_queue;

  std::vector<MBPUpdate> bids, asks, bids_2, asks_2;
  std::vector<Trade> trades;
  std::vector<Bar> bars;
  std::vector<Fill> fills;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
