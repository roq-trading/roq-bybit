/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::Auth2;

TEST_CASE("success", "[json_auth_2]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"auth",)"
                 R"("connId":"d4al82f88smcpctamlf0-b5jk")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 0);
    CHECK(obj.ret_msg == "OK"sv);
    CHECK(obj.conn_id == "d4al82f88smcpctamlf0-b5jk"sv);
  };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}
