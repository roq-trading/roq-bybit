/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/settings.hpp"

#include "roq/bybit/flags/flags.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

Settings::Settings(server::Type type)
    : server::Settings{server::create_settings(type, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER, flags::Flags::api())},
      exchange{flags::Flags::exchange()},
      common{
          .request_limit = flags::Flags::request_limit(),
          .request_limit_interval = flags::Flags::request_limit_interval(),
          .encode_buffer_size = flags::Flags::encode_buffer_size(),
          .decode_buffer_size = flags::Flags::decode_buffer_size(),
          .mbp_allow_price_inversion = flags::Flags::mbp_allow_price_inversion(),
          .execution_lookback = flags::Flags::execution_lookback(),
      },
      rest{
          .uri = flags::Flags::rest_uri(),
          .proxy = flags::Flags::rest_proxy(),
          .ping_freq = flags::Flags::rest_ping_freq(),
          .ping_path = flags::Flags::rest_ping_path(),
          .request_timeout = flags::Flags::rest_request_timeout(),
          .recv_window = flags::Flags::rest_recv_window(),
          .terminate_on_403 = flags::Flags::rest_terminate_on_403(),
          .back_off_delay = flags::Flags::rest_back_off_delay(),
      },
      ws{
          .public_uri = flags::Flags::ws_public_uri(),
          .private_uri = flags::Flags::ws_private_uri(),
          .ping_freq = flags::Flags::ws_ping_freq(),
          .request_timeout = flags::Flags::ws_request_timeout(),
          .max_subscriptions_per_stream = flags::Flags::ws_max_subscriptions_per_stream(),
          .mbp_depth = flags::Flags::ws_mbp_depth(),
      } {
}

}  // namespace bybit
}  // namespace roq
