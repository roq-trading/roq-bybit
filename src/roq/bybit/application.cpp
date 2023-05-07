/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/application.hpp"

#include "roq/bybit/config.hpp"
#include "roq/bybit/gateway.hpp"
#include "roq/bybit/settings.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === HELPERS ===

namespace {
auto const TYPE = server::Type::ORDER_MANAGEMENT;
}

// === IMPLEMENTATION ===

int Application::main(int, char **) {
  auto settings = Settings::create(TYPE);
  Config config{settings};
  auto context = server::create_io_context();
  server::Trading<Gateway>{settings, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bybit
}  // namespace roq
