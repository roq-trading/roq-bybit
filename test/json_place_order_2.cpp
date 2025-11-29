/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/place_order2.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

TEST_CASE("success", "[json_place_order_2]") {
  auto message = R"({)"
                 R"("reqId":"gAAC7yKS2UIAAQAAAAAA",)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("op":"order.create",)"
                 R"("data":{)"
                 R"("orderId":"15141634-9867-4d9f-9159-ba37ae9b5fca",)"
                 R"("orderLinkId":"gAAC7yKS2UIAAQAAAAAA")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("X-Bapi-Limit":"10",)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764401421682",)"
                 R"("Traceid":"7731f013426f418c395ff4787e693f5c",)"
                 R"("Timenow":"1764401421683")"
                 R"(},)"
                 R"("connId":"d4al7m6c0hv96a0jimg0-b9a1")"
                 R"(})"sv;
  core::json::BufferStack buffer{8192, 1};
  json::PlaceOrder2 obj{message, buffer};
}

TEST_CASE("failure", "[json_place_order_2]") {
  auto message = R"({)"
                 R"("reqId":"RAACPlr82kIAAQAAAAAA",)"
                 R"("retCode":110003,)"
                 R"("retMsg":"price is invalid",)"
                 R"("op":"order.create",)"
                 R"("data":{},)"
                 R"("retExtInfo":{},)"
                 R"("header":{)"
                 R"("X-Bapi-Limit-Status":"9",)"
                 R"("X-Bapi-Limit-Reset-Timestamp":"1764403934980",)"
                 R"("Traceid":"48c68086615e9eca379ccf7cece503d5",)"
                 R"("Timenow":"1764403934982",)"
                 R"("X-Bapi-Limit":"10")"
                 R"(},)"
                 R"("connId":"d4al7m6c0hv96a0jimg0-b9wz")"
                 R"(})"sv;
  core::json::BufferStack buffer{8192, 1};
  json::PlaceOrder2 obj{message, buffer};
}
