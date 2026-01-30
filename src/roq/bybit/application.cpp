/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/application.hpp"

#include "roq/bybit/api.hpp"
#include "roq/bybit/config.hpp"
#include "roq/bybit/gateway.hpp"
#include "roq/bybit/settings.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
uint8_t const API_SPOT = 0x0;
uint8_t const API_LINEAR = 0x1;
uint8_t const API_INVERSE = 0x2;
uint8_t const API_OPTION = 0x3;
}  // namespace

// === HELPERS ===

namespace {
auto parse_api(auto &settings) {
  auto api = API::parse_api(settings.app.api);
  switch (api) {
    using enum tools::API;
    case UNDEFINED:
      break;
    case SPOT:
      return API_SPOT;
    case LINEAR:
      return API_LINEAR;
    case INVERSE:
      return API_INVERSE;
    case OPTION:
      return API_OPTION;
  }
  log::fatal(R"(Unexpected: api="{}")"sv, settings.app.api);
}
}  // namespace

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  Settings settings{args};
  auto api = parse_api(settings);
  Config config{settings};
  auto context = server::create_io_context(settings);
  server::Trading<Gateway>{settings, config, *context, api}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bybit
}  // namespace roq
