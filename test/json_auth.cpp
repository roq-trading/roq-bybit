/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::Auth;

TEST_CASE("success", "[json_auth]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("ret_msg":"",)"
                 R"("conn_id":"2aec98fffedccb36-0000000e-00070dd5-439bc55e423b896e-73088429",)"
                 R"("req_id":"1",)"
                 R"("op":"auth")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.ret_msg == ""sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
