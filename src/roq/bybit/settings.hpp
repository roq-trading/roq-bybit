/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <fmt/compile.h>
#include <fmt/format.h>

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
struct WS final {
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
  detail::WS ws;
};

}  // namespace bybit
}  // namespace roq

template <>
struct fmt::formatter<roq::bybit::detail::Common> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::bybit::detail::Common const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(request_limit={}, )"
        R"(request_limit_interval={}, )"
        R"(encode_buffer_size={}, )"
        R"(decode_buffer_size={}, )"
        R"(mbp_allow_price_inversion={}, )"
        R"(execution_lookback={})"
        R"(}})"_cf,
        value.request_limit,
        value.request_limit_interval,
        value.encode_buffer_size,
        value.decode_buffer_size,
        value.mbp_allow_price_inversion,
        value.execution_lookback);
  }
};

template <>
struct fmt::formatter<roq::bybit::detail::REST> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::bybit::detail::REST const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(uri={}, )"
        R"(proxy={}, )"
        R"(ping_freq={}, )"
        R"(ping_path="{}", )"
        R"(request_timeout={}, )"
        R"(recv_window={}, )"
        R"(terminate_on_403={}, )"
        R"(back_off_delay={})"
        R"(}})"_cf,
        value.uri,
        value.proxy,
        value.ping_freq,
        value.ping_path,
        value.request_timeout,
        value.recv_window,
        value.terminate_on_403,
        value.back_off_delay);
  }
};

template <>
struct fmt::formatter<roq::bybit::detail::WS> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::bybit::detail::WS const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(public_uri={}, )"
        R"(private_uri={}, )"
        R"(ping_freq={}, )"
        R"(request_timeout={}, )"
        R"(max_subscriptions_per_stream={}, )"
        R"(mbp_depth={})"
        R"(}})"_cf,
        value.public_uri,
        value.private_uri,
        value.ping_freq,
        value.request_timeout,
        value.max_subscriptions_per_stream,
        value.mbp_depth);
  }
};

template <>
struct fmt::formatter<roq::bybit::Settings> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::bybit::Settings const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(exchange="{}", )"
        R"(common={}, )"
        R"(rest={}, )"
        R"(ws={}, )"
        R"(server={})"
        R"(}})"_cf,
        value.exchange,
        value.common,
        value.rest,
        value.ws,
        static_cast<roq::server::Settings const &>(value));
  }
};
