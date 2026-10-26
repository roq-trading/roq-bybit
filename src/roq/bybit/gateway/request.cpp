/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/gateway/request.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {
namespace gateway {

// === IMPLEMENTATION ===

Request::Request(Shared &shared) : account{*this}, wallet{*this}, positions{*this}, orders{*this}, executions{*this}, shared_{shared} {
}

bool Request::ready() const {
  return account.ready() && wallet.ready() && positions.ready() && orders.ready() && executions.ready();
}

bool Request::can_request(TraceInfo const &trace_info) {
  return shared_.rate_limiter.can_request(trace_info);
}

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
