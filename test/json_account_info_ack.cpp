/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/protocol/json/account_info_ack.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = protocol::json::AccountInfoAck;

TEST_CASE("simple", "[json_account_info_ack]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("marginMode":"REGULAR_MARGIN",)"
                 R"("updatedTime":"1644843230000",)"
                 R"("unifiedMarginStatus":1,)"
                 R"("dcpStatus":"OFF",)"
                 R"("timeWindow":10,)"
                 R"("smpGroup":0)"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 0);
    CHECK(obj.ret_msg == "OK"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
