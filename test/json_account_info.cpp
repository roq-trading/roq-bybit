/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/account_info.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("simple", "[json_account_info]") {
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
  core::json::BufferStack buffers{8192, 1};
  json::AccountInfo obj{message, buffers};
  CHECK(obj.ret_code == 0);
}
