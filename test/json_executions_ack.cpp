/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/executions_ack.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

using value_type = json::ExecutionsAck;

TEST_CASE("simple", "[json_executions_ack]") {
  auto message = R"({)"
                 R"("retCode":0,)"
                 R"("retMsg":"OK",)"
                 R"("result":{)"
                 R"("list":[{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("orderId":"1682668800-BTCUSDT-460579-Buy",)"
                 R"("orderLinkId":"",)"
                 R"("side":"Buy",)"
                 R"("orderPrice":"0.00",)"
                 R"("orderQty":"0.000",)"
                 R"("leavesQty":"0.000",)"
                 R"("orderType":"UNKNOWN",)"
                 R"("stopOrderType":"UNKNOWN",)"
                 R"("execFee":"0.04486547",)"
                 R"("execId":"7eeb8e40-fde4-4be1-9fa1-191081c49d8e",)"
                 R"("execPrice":"29476.42",)"
                 R"("execQty":"0.002",)"
                 R"("execType":"Funding",)"
                 R"("execValue":"58.95284",)"
                 R"("execTime":"1682668800000",)"
                 R"("isMaker":false,)"
                 R"("feeRate":"0.00076104",)"
                 R"("tradeIv":"",)"
                 R"("markIv":"",)"
                 R"("markPrice":"29476.42",)"
                 R"("indexPrice":"",)"
                 R"("underlyingPrice":"",)"
                 R"("blockTradeId":"",)"
                 R"("closedSize":"0.000")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("orderId":"1682640000-BTCUSDT-460579-Buy",)"
                 R"("orderLinkId":"",)"
                 R"("side":"Buy",)"
                 R"("orderPrice":"0.00",)"
                 R"("orderQty":"0.000",)"
                 R"("leavesQty":"0.000",)"
                 R"("orderType":"UNKNOWN",)"
                 R"("stopOrderType":"UNKNOWN",)"
                 R"("execFee":"-0.06101433",)"
                 R"("execId":"c140901a-2a06-4f3b-bd40-3db229e95c7b",)"
                 R"("execPrice":"29474.10",)"
                 R"("execQty":"0.002",)"
                 R"("execType":"Funding",)"
                 R"("execValue":"58.9482",)"
                 R"("execTime":"1682640000000",)"
                 R"("isMaker":false,)"
                 R"("feeRate":"-0.00103505",)"
                 R"("tradeIv":"",)"
                 R"("markIv":"",)"
                 R"("markPrice":"29474.10",)"
                 R"("indexPrice":"",)"
                 R"("underlyingPrice":"",)"
                 R"("blockTradeId":"",)"
                 R"("closedSize":"0.000")"
                 R"(},{)"
                 R"("symbol":"BTCUSDT",)"
                 R"("orderId":"1682611200-BTCUSDT-460579-Buy",)"
                 R"("orderLinkId":"",)"
                 R"("side":"Buy",)"
                 R"("orderPrice":"0.00",)"
                 R"("orderQty":"0.000",)"
                 R"("leavesQty":"0.000",)"
                 R"("orderType":"UNKNOWN",)"
                 R"("stopOrderType":"UNKNOWN",)"
                 R"("execFee":"0.05848746",)"
                 R"("execId":"dfad755a-6776-44a4-a654-21f3f0687e2d",)"
                 R"("execPrice":"29133.60",)"
                 R"("execQty":"0.002",)"
                 R"("execType":"Funding",)"
                 R"("execValue":"58.2672",)"
                 R"("execTime":"1682611200000",)"
                 R"("isMaker":false,)"
                 R"("feeRate":"0.00100378",)"
                 R"("tradeIv":"",)"
                 R"("markIv":"",)"
                 R"("markPrice":"29133.60",)"
                 R"("indexPrice":"",)"
                 R"("underlyingPrice":"",)"
                 R"("blockTradeId":"",)"
                 R"("closedSize":"0.000")"
                 R"(})"
                 R"(],)"
                 R"("nextPageCursor":"",)"
                 R"("category":"linear")"
                 R"(},)"
                 R"("retExtInfo":{},)"
                 R"("time":1682694922390)"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.ret_code == 0);
    CHECK(obj.ret_msg == "OK"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
