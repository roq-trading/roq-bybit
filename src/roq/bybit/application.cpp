/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/application.hpp"

#include "roq/bybit/config.hpp"
#include "roq/bybit/gateway.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
auto const SETTINGS = server::Settings{
    .package_name = ROQ_PACKAGE_NAME,
    .build_number = ROQ_BUILD_NUMBER,
    .api = {},
    .type = server::Type::ORDER_MANAGEMENT,
};
}  // namespace

// === IMPLEMENTATION ===

int Application::main(int, char **) {
  Config config;
  auto context = server::create_io_context();
  server::Trading<Gateway>{SETTINGS, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bybit
}  // namespace roq
