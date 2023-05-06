/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/settings.hpp"

#include "roq/bybit/flags/flags.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

Settings Settings::create(server::Type type) {
  auto api = flags::Flags::api();
  auto settings = server::create_settings(type, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER, api);
  return {settings};
}

}  // namespace bybit
}  // namespace roq
