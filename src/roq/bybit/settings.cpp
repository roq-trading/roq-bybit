/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/settings.hpp"

#include "roq/logging.hpp"

#include "roq/server/flags/settings.hpp"

#include "roq/bybit/flags/flags.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

Settings::Settings(server::Type type)
    : server::Settings{server::flags::create_settings(type, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER, flags::Flags::api())},
      exchange{flags::Flags::exchange()} {
  log::debug("settings={}"sv, *this);
}

}  // namespace bybit
}  // namespace roq
