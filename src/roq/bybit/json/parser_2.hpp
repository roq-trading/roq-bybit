/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/bybit/json/amend_order2.hpp"
#include "roq/bybit/json/auth2.hpp"
#include "roq/bybit/json/cancel_order2.hpp"
#include "roq/bybit/json/error.hpp"
#include "roq/bybit/json/ping.hpp"
#include "roq/bybit/json/place_order2.hpp"

namespace roq {
namespace bybit {
namespace json {

struct Parser2 final {
  struct Handler {
    virtual void operator()(Trace<json::Ping> const &) = 0;
    virtual void operator()(Trace<json::Auth2> const &) = 0;
    virtual void operator()(Trace<json::Error> const &) = 0;
    virtual void operator()(Trace<json::PlaceOrder2> const &) = 0;
    virtual void operator()(Trace<json::AmendOrder2> const &) = 0;
    virtual void operator()(Trace<json::CancelOrder2> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace bybit
}  // namespace roq
