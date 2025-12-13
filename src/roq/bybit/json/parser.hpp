/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/error.hpp"
#include "roq/bybit/json/ping.hpp"
#include "roq/bybit/json/subscribe.hpp"

// public
#include "roq/bybit/json/kline.hpp"
#include "roq/bybit/json/order_book.hpp"
#include "roq/bybit/json/public_trade.hpp"
#include "roq/bybit/json/tickers.hpp"

// private
#include "roq/bybit/json/auth.hpp"
#include "roq/bybit/json/execution.hpp"
#include "roq/bybit/json/order.hpp"
#include "roq/bybit/json/position.hpp"
#include "roq/bybit/json/wallet.hpp"

namespace roq {
namespace bybit {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Ping> const &) = 0;
    // response
    virtual void operator()(Trace<json::Auth> const &) = 0;
    virtual void operator()(Trace<json::Subscribe> const &) = 0;
    virtual void operator()(Trace<json::Error> const &) = 0;
    // public stream
    virtual void operator()(Trace<json::OrderBook> const &, size_t depth) = 0;
    virtual void operator()(Trace<json::PublicTrade> const &) = 0;
    virtual void operator()(Trace<json::Tickers> const &) = 0;
    virtual void operator()(Trace<json::Kline> const &) = 0;
    // private stream
    virtual void operator()(Trace<json::Wallet> const &) = 0;
    virtual void operator()(Trace<json::Position> const &) = 0;
    virtual void operator()(Trace<json::Order> const &) = 0;
    virtual void operator()(Trace<json::Execution> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace bybit
}  // namespace roq
