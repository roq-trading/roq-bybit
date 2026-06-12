/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/application.hpp"

#include "roq/bybit/flags/settings.hpp"

#include "roq/bybit/gateway/config.hpp"
#include "roq/bybit/gateway/controller.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  flags::Settings settings{args};
  gateway::Config config{settings};
  auto context = server::create_io_context(settings);
  server::Trading<gateway::Controller>{settings, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bybit
}  // namespace roq
