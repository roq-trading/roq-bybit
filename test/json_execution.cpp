/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/execution.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_execution_spot", "[json_execution]") {
  core::Buffer buffer(8192);
  json::Execution obj{MESSAGE, buffer};
}
