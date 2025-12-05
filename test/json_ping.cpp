/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::Ping;

TEST_CASE("simple", "[json_ping]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("ret_msg":"pong",)"
                 R"("conn_id":"6356e46e-283a-46ab-aa57-4cabd05176ff",)"
                 R"("req_id":"78630463144691",)"
                 R"("op":"ping")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.ret_msg == "pong"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
