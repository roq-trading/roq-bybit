/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/server.hpp"

namespace roq {
namespace bybit {

namespace detail {
struct Common final {
  uint32_t request_limit = {};
  std::chrono::nanoseconds request_limit_interval = {};
  uint32_t encode_buffer_size = {};
  uint32_t decode_buffer_size = {};
  bool mbp_allow_price_inversion = {};
  std::chrono::milliseconds execution_lookback = {};
};
struct REST final {
  roq::io::web::URI const &uri;
  roq::io::web::URI const &proxy;
  std::chrono::nanoseconds ping_freq = {};
  std::string_view ping_path;
  std::chrono::nanoseconds request_timeout = {};
  std::chrono::milliseconds recv_window = {};
  bool terminate_on_403 = {};
  std::chrono::nanoseconds back_off_delay = {};
};
struct WebSocket final {
  roq::io::web::URI const &public_uri;
  roq::io::web::URI const &private_uri;
  std::chrono::nanoseconds ping_freq = {};
  std::chrono::nanoseconds request_timeout = {};
  uint32_t max_subscriptions_per_stream = {};
  uint32_t mbp_depth = {};
};
}  // namespace detail

struct Settings final : public server::Settings {
  explicit Settings(server::Type);

  std::string_view exchange;
  detail::Common common;
  detail::REST rest;
  detail::WebSocket ws;
};

}  // namespace bybit
}  // namespace roq
