/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/parser_2.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("success", "[json_amend_order_2]") {
  auto message = R"({)"
                 R"("reqId":"1AACjm7r2kIAAgAAAAAA",)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"order.amend",)"
                 R"("data":{)"
                 R"("orderId":"ab99c212-48ce-4cd1-baf6-591fdd559ea5",)"
                 R"("orderLinkId":"1wACjm7r2kIAAQAAAAAA")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("X-Bapi-Limit":"10",)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764403686937",)"
                 R"("Traceid":"4132f73eda96f268031eca6d04bae324",)"
                 R"("Timenow":"1764403686938")"
                 R"(},)"
                 R"("connId":"d4al82f88smcpctamlf0-b9ry")"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AmendOrder2 obj{message, buffers};
  CHECK(obj.ret_code == 0);
  // parser
  struct Handler final : public json::Parser2::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth2> const &) override { FAIL(); }
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::PlaceOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::AmendOrder2> const &event) override {
      found = true;
      auto &[trace_info, amend_order] = event;
      CHECK(amend_order.ret_code == 0);
    }
    void operator()(Trace<json::CancelOrder2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}

TEST_CASE("failure", "[json_amend_order_2]") {
  auto message = R"({)"
                 R"("reqId":"RQACPVr82kIAAwAAAAAA",)"
                 R"("retCode":110001,)"
                 R"("retMsg":"order not exists or too late to replace",)"
                 R"("op":"order.amend",)"
                 R"("data":{},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("Traceid":"73a4b4c565baf0dc76cdbc1d6dffe1a8",)"
                 R"("Timenow":"1764403866842",)"
                 R"("X-Bapi-Limit":"10",)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764403866841")"
                 R"(},)"
                 R"("connId":"d4al7m6c0hv96a0jimg0-b9wz")"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::AmendOrder2 obj{message, buffers};
  CHECK(obj.ret_code == 110001);
  // parser
  struct Handler final : public json::Parser2::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth2> const &) override { FAIL(); }
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::PlaceOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::AmendOrder2> const &event) override {
      found = true;
      auto &[trace_info, amend_order] = event;
      CHECK(amend_order.ret_code == 110001);
    }
    void operator()(Trace<json::CancelOrder2> const &) override { FAIL(); }

    bool found = false;
  } handler;
  auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
