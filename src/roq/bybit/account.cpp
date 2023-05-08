/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/account.hpp"

#include "roq/utils/safe_cast.hpp"

#include "roq/clock.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace bybit {

// === IMPLEMENTATION ===

Account::Account(Settings const &settings, Config const &config, std::string_view const &name)
    : name_{name}, crypto_{config.get_api_key(name_), config.get_secret(name_), settings.rest.recv_window},
      rate_limiter{settings.common.request_limit, settings.common.request_limit_interval}, request_queue{rate_limiter} {
}

std::string Account::create_signature(std::chrono::milliseconds expires) {
  return crypto_.create_signature_v2(expires);
}

std::string Account::create_headers(
    std::string_view const &path, std::string_view const &query, std::string_view const &body) {
  auto now = clock::get_realtime();
  return crypto_.create_headers_v2(path, query, body, utils::safe_cast(now));
}

}  // namespace bybit
}  // namespace roq
