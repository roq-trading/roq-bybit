/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/parser_2.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("success", "[json_auth_2]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"auth",)"
                 R"("connId":"d4al82f88smcpctamlf0-b5jk")"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::Auth2 obj{message, buffers};
  CHECK(obj.ret_code == 0);
  // parser
  struct Handler final : public json::Parser2::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth2> const &event) override {
      found = true;
      auto &[trace_info, auth] = event;
      CHECK(auth.ret_code == 0);
    }
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::PlaceOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::AmendOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::CancelOrder2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
