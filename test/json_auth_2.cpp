/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/auth2.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("success", "[json_auth_2]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"auth",)"
                 R"("connId":"d4al82f88smcpctamlf0-b5jk")"
                 R"(})"sv;
  core::json::BufferStack buffer{8192, 1};
  json::Auth2 obj{message, buffer};
}
