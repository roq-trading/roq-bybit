/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/amend_order_ack.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::AmendOrderAck;

TEST_CASE("error", "[json_amend_order_ack]") {
  auto message = R"({)"
                 R"("retCode":10001,)"
                 R"("retMsg":"Illegal category",)"
                 R"("result":{},)"
                 R"("retExtInfo":{},)"
                 R"("time":1682911123716)"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.ret_code == 10001); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
