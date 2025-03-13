/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bybit/application.hpp"

#include "roq/bybit/config.hpp"
#include "roq/bybit/gateway.hpp"
#include "roq/bybit/settings.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
uint8_t const API_2 = {};
}

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  Settings settings{args};
  Config config{settings};
  auto context = server::create_io_context(settings);
  server::Trading<Gateway>{settings, config, *context, API_2}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bybit
}  // namespace roq
