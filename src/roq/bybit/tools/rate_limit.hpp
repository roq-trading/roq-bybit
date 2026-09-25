/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>

#include "roq/web/rest/client.hpp"
#include "roq/web/rest/response.hpp"

namespace roq {
namespace bybit {
namespace tools {

struct RateLimit final {
  RateLimit() = default;

  RateLimit(RateLimit &&) = default;
  RateLimit(RateLimit const &) = delete;

  operator std::chrono::nanoseconds() const { return suspend_until_; }

  void operator()(Trace<web::rest::Client::Header> const &);
  void operator()(Trace<web::rest::Response> const &);

  void suspend();

 private:
  int32_t limit_ = {};
  int32_t limit_status_ = {};
  int64_t limit_reset_timestamp_ = {};  // msec

  std::chrono::nanoseconds suspend_until_ = {};
};

}  // namespace tools
}  // namespace bybit
}  // namespace roq
