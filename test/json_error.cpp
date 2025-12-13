/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_2_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::Error;
/*
TEST_CASE("simple", "[json_error]") {
  auto message = R"({)"
                 R"("retCode":10004,)"
                 R"("retMsg":"error sign! origin_string[1682321990138dBW7d0xxu8cbXUl5vq5000category=linear]",)"
                 R"("result":{},)"
                 R"("retExtInfo":{},)"
                 R"("time":1682321990235)"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 10004);
    CHECK(obj.ret_msg == "error sign! origin_string[1682321990138dBW7d0xxu8cbXUl5vq5000category=linear]"sv);
  };
  Parser2Tester<value_type>::dispatch(helper, message, 8192, 1);
}
*/
