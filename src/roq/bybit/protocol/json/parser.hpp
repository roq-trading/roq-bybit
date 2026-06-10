/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/protocol/json/error.hpp"
#include "roq/bybit/protocol/json/ping.hpp"
#include "roq/bybit/protocol/json/subscribe.hpp"

// public
#include "roq/bybit/protocol/json/kline.hpp"
#include "roq/bybit/protocol/json/order_book.hpp"
#include "roq/bybit/protocol/json/public_trade.hpp"
#include "roq/bybit/protocol/json/tickers.hpp"

// private
#include "roq/bybit/protocol/json/auth.hpp"
#include "roq/bybit/protocol/json/execution.hpp"
#include "roq/bybit/protocol/json/order.hpp"
#include "roq/bybit/protocol/json/position.hpp"
#include "roq/bybit/protocol/json/wallet.hpp"

namespace roq {
namespace bybit {
namespace protocol {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<protocol::json::Ping> const &) = 0;
    // response
    virtual void operator()(Trace<protocol::json::Auth> const &) = 0;
    virtual void operator()(Trace<protocol::json::Subscribe> const &) = 0;
    virtual void operator()(Trace<protocol::json::Error> const &) = 0;
    // public stream
    virtual void operator()(Trace<protocol::json::OrderBook> const &, size_t depth) = 0;
    virtual void operator()(Trace<protocol::json::PublicTrade> const &) = 0;
    virtual void operator()(Trace<protocol::json::Tickers> const &) = 0;
    virtual void operator()(Trace<protocol::json::Kline> const &) = 0;
    // private stream
    virtual void operator()(Trace<protocol::json::Wallet> const &) = 0;
    virtual void operator()(Trace<protocol::json::Position> const &) = 0;
    virtual void operator()(Trace<protocol::json::Order> const &) = 0;
    virtual void operator()(Trace<protocol::json::Execution> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace protocol
}  // namespace bybit
}  // namespace roq
