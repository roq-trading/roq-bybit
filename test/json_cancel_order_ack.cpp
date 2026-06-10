/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/protocol/json/cancel_order_ack.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::CancelOrderAck;

TEST_CASE("response", "[json_cancel_order_ack]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("orderId":"b8614cb3-9ddb-460b-9282-a994528eac87",)"
                 R"("orderLinkId":"ZwAC7nLlHkQAAQAAAAAA")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1764947226743)"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 0);
    CHECK(obj.ret_msg == "OK"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
