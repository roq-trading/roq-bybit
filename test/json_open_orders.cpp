/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/open_orders.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const EMPTY = R"({)"
                   R"("retCode":0,)"
                   R"("retMsg":"OK",)"
                   R"("result":{)"
                   R"("list":[],)"
                   R"("nextPageCursor":"",)"
                   R"("category":"linear")"
                   R"(},)"
                   R"("retExtInfo":{},)"
                   R"("time":1682689733889)"
                   R"(})"sv;
}  // namespace

TEST_CASE("empty", "[json_open_orders]") {
  core::Buffer buffer(8192);
  json::OpenOrders obj{EMPTY, buffer};
}
