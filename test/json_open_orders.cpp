/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/open_orders.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_open_orders_spot", "[json_open_orders]") {
  core::Buffer buffer(8192);
  json::OpenOrders obj{MESSAGE, buffer};
}
