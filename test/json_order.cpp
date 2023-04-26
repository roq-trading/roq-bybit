/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/order.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

// note! from websocket

namespace {
auto const MESSAGE = R"({)"
                     R"("id":"460579-1405980802291926784-ff9c1866-f679-4fde-9e43-833c96d09967",)"
                     R"("topic":"order",)"
                     R"("creationTime":1682341972648,)"
                     R"("data":[{)"
                     R"("symbol":"BTCUSDT",)"
                     R"("orderId":"1405980802291926784",)"
                     R"("side":"Buy",)"
                     R"("orderType":"Limit",)"
                     R"("cancelType":"UNKNOWN",)"
                     R"("price":"27061.65",)"
                     R"("qty":"0.001000",)"
                     R"("orderIv":"",)"
                     R"("timeInForce":"GTC",)"
                     R"("orderStatus":"New",)"
                     R"("orderLinkId":"1682341972120",)"
                     R"("lastPriceOnCreated":"",)"
                     R"("reduceOnly":false,)"
                     R"("leavesQty":"",)"
                     R"("leavesValue":"",)"
                     R"("cumExecQty":"0.000000",)"
                     R"("cumExecValue":"0.00000000",)"
                     R"("avgPrice":"",)"
                     R"("blockTradeId":"",)"
                     R"("positionIdx":0,)"
                     R"("cumExecFee":"0",)"
                     R"("createdTime":"1682341972563",)"
                     R"("updatedTime":"",)"
                     R"("rejectReason":"",)"
                     R"("stopOrderType":"",)"
                     R"("triggerPrice":"",)"
                     R"("takeProfit":"",)"
                     R"("stopLoss":"",)"
                     R"("tpTriggerBy":"",)"
                     R"("slTriggerBy":"",)"
                     R"("triggerDirection":0,)"
                     R"("triggerBy":"",)"
                     R"("closeOnTrigger":false,)"
                     R"("category":"spot",)"
                     R"("isLeverage":"0",)"
                     R"("smpType":"None",)"
                     R"("smpGroup":0,)"
                     R"("smpOrderId":"")"
                     R"(})"
                     R"(])"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_order_spot", "[json_order]") {
  core::Buffer buffer(8192);
  json::Order obj{MESSAGE, buffer};
}

TEST_CASE("json_order_parser", "[json_order]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Ping> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { FAIL(); }
    void operator()(Trace<json::PublicTrade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::WalletBalance2> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { found = true; }
    void operator()(Trace<json::TicketInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.found == true);
}
