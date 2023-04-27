/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/authenticator.hpp"

#include "roq/utils/safe_cast.hpp"

#include "roq/clock.hpp"

#include "roq/bybit/flags/flags.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace bybit {

// === IMPLEMENTATION ===

Authenticator::Authenticator(Config const &config, std::string_view const &account)
    : account_{account},
      crypto_{config.get_api_key(account_), config.get_secret(account_), flags::Flags::rest_recv_window()},
      rate_limiter{flags::Flags::request_limit(), flags::Flags::request_limit_interval()} {
}

std::string Authenticator::create_signature(std::chrono::milliseconds expires) {
  return crypto_.create_signature_v2(expires);
}

std::string Authenticator::create_headers(
    std::string_view const &path, std::string_view const &query, std::string_view const &body) {
  auto now = clock::get_realtime();
  return crypto_.create_headers_v2(path, query, body, utils::safe_cast(now));
}

}  // namespace bybit
}  // namespace roq
