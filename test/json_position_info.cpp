/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/position_info.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_position_info_spot", "[json_position_info]") {
  core::Buffer buffer(8192);
  json::PositionInfo obj{MESSAGE, buffer};
}
