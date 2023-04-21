/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/core/buffer.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/server.hpp"

#include "roq/bybit/json/error.hpp"
#include "roq/bybit/json/pong.hpp"
#include "roq/bybit/json/subscribe.hpp"

// public
#include "roq/bybit/json/order_book.hpp"
#include "roq/bybit/json/tickers.hpp"
#include "roq/bybit/json/trade.hpp"

// private
#include "roq/bybit/json/auth.hpp"
#include "roq/bybit/json/order.hpp"
#include "roq/bybit/json/outbound_account_info.hpp"
#include "roq/bybit/json/ticket_info.hpp"

namespace roq {
namespace bybit {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Error> const &) = 0;
    virtual void operator()(Trace<json::Pong> const &) = 0;
    virtual void operator()(Trace<json::Subscribe> const &) = 0;
    // public
    virtual void operator()(Trace<json::OrderBook> const &, size_t depth) = 0;
    virtual void operator()(Trace<json::Trade> const &) = 0;
    virtual void operator()(Trace<json::Tickers> const &) = 0;
    // private
    virtual void operator()(Trace<json::Auth> const &) = 0;
    virtual void operator()(Trace<json::OutboundAccountInfo> const &) = 0;
    virtual void operator()(Trace<json::Order> const &) = 0;
    virtual void operator()(Trace<json::TicketInfo> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::Buffer &, TraceInfo const &);
};

}  // namespace json
}  // namespace bybit
}  // namespace roq
