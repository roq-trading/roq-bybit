/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::Subscribe;

TEST_CASE("spot", "[json_subscribe]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("ret_msg":"subscribe",)"
                 R"("conn_id":"13ac3a8f-52dd-411a-81c1-7644f6a1ef8b",)"
                 R"("req_id":"4000001",)"
                 R"("op":"subscribe")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.ret_msg == "subscribe"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("linear", "[json_subscribe]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("ret_msg":"",)"
                 R"("conn_id":"2ef1dd44-e9e0-4b3a-8852-292afa6ae416",)"
                 R"("req_id":"4000001",)"
                 R"("op":"subscribe")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.ret_msg == ""sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("inverse", "[json_subscribe]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("ret_msg":"",)"
                 R"("conn_id":"7d269cfe-ad45-40f0-8fd4-7f42fc58c2c3",)"
                 R"("req_id":"4000001",)"
                 R"("op":"subscribe")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.ret_msg == ""sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("option", "[json_subscribe]") {
  auto message = R"({)"
                 R"("success":true,)"
                 R"("conn_id":"461834fffe84142f-00000001-000c2ffe-1fd4e17e7fe53c8b-2c14741e",)"
                 R"("data":{)"
                 R"("failTopics":[],)"
                 R"("successTopics":[)"
                 R"("orderbook.25.BTC-21APR23-28500-P",)"
                 R"("orderbook.25.BTC-21APR23-27500-C",)"
                 R"("orderbook.25.BTC-21APR23-31500-C")"
                 R"(])"
                 R"(},)"
                 R"("type":"COMMAND_RESP")"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.success == true);
    CHECK(obj.conn_id == "461834fffe84142f-00000001-000c2ffe-1fd4e17e7fe53c8b-2c14741e"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
