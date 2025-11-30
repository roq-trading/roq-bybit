/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/parser_2.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("success", "[json_cancel_order_2]") {
  auto message = R"({)"
                 R"("reqId":"gwAC7yKS2UIAAgAAAAAA",)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"order.cancel",)"
                 R"("data":{)"
                 R"("orderId":"15141634-9867-4d9f-9159-ba37ae9b5fca",)"
                 R"("orderLinkId":"gAAC7yKS2UIAAQAAAAAA")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("X-Bapi-Limit":"10",)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764403074614",)"
                 R"("Traceid":"267c391aa81624976096894ff201abef",)"
                 R"("Timenow":"1764403074616")"
                 R"(},)"
                 R"("connId":"d4al7m6c0hv96a0jimg0-b9pw")"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::CancelOrder2 obj{message, buffers};
  CHECK(obj.ret_code == 0);
  // parser
  struct Handler final : public json::Parser2::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth2> const &) override { FAIL(); }
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::PlaceOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::AmendOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::CancelOrder2> const &event) override {
      found = true;
      auto &[trace_info, cancel_order] = event;
      CHECK(cancel_order.ret_code == 0);
    }

    bool found = false;
  } handler;
  auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}

TEST_CASE("failure", "[json_cancel_order_2]") {
  auto message = R"({)"
                 R"("reqId":"RAACPVr82kIAAgAAAAAA",)"
                 R"("retCode":110001,)"
                 R"("retMsg":"order not exists or too late to cancel",)"
                 R"("op":"order.cancel",)"
                 R"("data":{},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("Traceid":"705ae0668ba526507879e7a5ab488f15",)"
                 R"("Timenow":"1764403799483",)"
                 R"("X-Bapi-Limit":"10",)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764403799481")"
                 R"(},)"
                 R"("connId":"d4al7m6c0hv96a0jimg0-b9wz")"
                 R"(})"sv;
  core::json::BufferStack buffers{8192, 1};
  // simple
  json::CancelOrder2 obj{message, buffers};
  CHECK(obj.ret_code == 110001);
  // parser
  struct Handler final : public json::Parser2::Handler {
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Auth2> const &) override { FAIL(); }
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::PlaceOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::AmendOrder2> const &) override { FAIL(); }
    void operator()(Trace<json::CancelOrder2> const &event) override {
      found = true;
      auto &[trace_info, cancel_order] = event;
      CHECK(cancel_order.ret_code == 110001);
    }

    bool found = false;
  } handler;
  auto res = json::Parser2::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
