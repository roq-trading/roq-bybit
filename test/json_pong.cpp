/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::Pong;

TEST_CASE("simple", "[json_pong]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"pong",)"
                 R"("data":["1795052029661"],)"
                 R"("connId":"dbms19mqc71cqqh3mlkg-1dwm")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 0);
    CHECK(obj.ret_msg == "OK"sv);
  };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}
