/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "roq/core/download.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/bybit/account.hpp"
#include "roq/bybit/order_entry_state.hpp"
#include "roq/bybit/shared.hpp"

#include "roq/bybit/json/account_info.hpp"
#include "roq/bybit/json/execution.hpp"
#include "roq/bybit/json/open_orders.hpp"
#include "roq/bybit/json/position_info.hpp"
#include "roq/bybit/json/wallet_ack.hpp"

#include "roq/bybit/json/amend_order.hpp"
#include "roq/bybit/json/cancel_all_orders.hpp"
#include "roq/bybit/json/cancel_order.hpp"
#include "roq/bybit/json/place_order.hpp"

namespace roq {
namespace bybit {

struct OrderEntry {
  struct Response final {
    std::string_view account;
    std::string_view topic;
    std::string_view symbol;
  };

  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<TradeUpdate> const &, bool is_last, uint8_t user_id, std::string_view const &request_id) = 0;
    virtual void operator()(Trace<PositionUpdate> const &, bool is_last) = 0;
    virtual void operator()(Trace<FundsUpdate> const &, bool is_last) = 0;
    //
    virtual void operator()(Trace<Response> const &) = 0;
  };

  OrderEntry() = default;

  OrderEntry(OrderEntry const &) = delete;

  virtual ~OrderEntry() = default;

  virtual void operator()(Event<Start> const &) = 0;
  virtual void operator()(Event<Stop> const &) = 0;
  virtual void operator()(Event<Timer> const &) = 0;

  virtual void operator()(metrics::Writer &) const = 0;

  virtual uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, std::string_view const &request_id) = 0;
  virtual uint16_t operator()(
      Event<ModifyOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) = 0;
  virtual uint16_t operator()(
      Event<CancelOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) = 0;

  virtual uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) = 0;
};

}  // namespace bybit
}  // namespace roq
