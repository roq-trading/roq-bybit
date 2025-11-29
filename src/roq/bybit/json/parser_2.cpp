/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bybit/json/parser_2.hpp"

#include "roq/logging.hpp"

#include "roq/utils/hash/fnv.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {
namespace json {

// === HELPERS ===

namespace {
template <typename T, typename... Args>
void dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info, Args &&...args) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj, std::forward<Args>(args)...);
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser2::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  auto stop = false;
  auto result = false;
  auto helper = [&](auto &key, auto &value) {
    auto k = utils::hash::FNV::compute(key);
    switch (k) {
      case utils::hash::FNV::compute("op"sv): {
        Operation op{std::get<std::string_view>(value)};
        switch (op) {
          using enum Operation::type_t;
          case UNDEFINED_INTERNAL:
            break;  // XXX stop?
          case UNKNOWN_INTERNAL:
            if (allow_unknown_event_types) {
              stop = true;
            }
            break;
          case AUTH:
            result = true;
            dispatch_helper<Auth2>(handler, message, buffer_stack, trace_info);
            break;
          case PING:
            stop = true;
            break;
          case PONG:
            stop = true;
            break;
          case SUBSCRIBE:
            stop = true;
            break;
          case ORDER_CREATE:
            result = true;
            dispatch_helper<PlaceOrder2>(handler, message, buffer_stack, trace_info);
            break;
          case ORDER_AMEND:
            result = true;
            dispatch_helper<AmendOrder2>(handler, message, buffer_stack, trace_info);
            break;
          case ORDER_CANCEL:
            result = true;
            dispatch_helper<CancelOrder2>(handler, message, buffer_stack, trace_info);
            break;
        }
      } break;
    }
  };
  core::json::Parser parser{message};
  auto value = parser.root();
  std::get<core::json::Object>(value).dispatch(helper);
  if (result || stop) {
    return result;
  }
  /*
  Message message_2{message, buffer_stack};
  auto topic = parse_topic(message_2.topic);
  switch (topic) {
    using enum Topic::type_t;
    case UNDEFINED_INTERNAL:
      break;
    case UNKNOWN_INTERNAL:
      break;  // note! we only expect the topics we have subscribed to
    case ORDERBOOK: {
      auto mbp_depth = parse_mbp_depth(message_2.topic);
      dispatch_helper<OrderBook>(handler, message, buffer_stack, trace_info, mbp_depth);
      return true;
    }
    case PUBLIC_TRADE:
      dispatch_helper<PublicTrade>(handler, message, buffer_stack, trace_info);
      return true;
    case TICKERS:
      dispatch_helper<Tickers>(handler, message, buffer_stack, trace_info);
      return true;
    case KLINE:
      dispatch_helper<Kline>(handler, message, buffer_stack, trace_info);
      return true;
    case WALLET:
      return dispatch_helper_flatten_wallet(handler, buffer_stack, trace_info, message);
    case POSITION:
      dispatch_helper<Position>(handler, message, buffer_stack, trace_info);
      return true;
    case ORDER:
      dispatch_helper<Order>(handler, message, buffer_stack, trace_info);
      return true;
    case EXECUTION:
      dispatch_helper<Execution2>(handler, message, buffer_stack, trace_info);
      return true;
  }
  switch (message_2.op) {
    using enum Operation::type_t;
    case UNDEFINED_INTERNAL:
      break;
    case UNKNOWN_INTERNAL:
      if (allow_unknown_event_types) {
        return false;
      }
      break;
    case AUTH:
      dispatch_helper<Auth2>(handler, message, buffer_stack, trace_info);
      return true;
    case PING:
      dispatch_helper<Ping>(handler, message, buffer_stack, trace_info);
      return true;
    case PONG: {
      // note! drop (only the option api)
      return true;
    }
    case SUBSCRIBE:
      dispatch_helper<Subscribe>(handler, message, buffer_stack, trace_info);
      return true;
    case ORDER_CREATE:
      // dispatch_helper<OrderCreate>(handler, message, buffer_stack, trace_info);
      return true;
    case ORDER_AMEND:
      // dispatch_helper<OrderAmend>(handler, message, buffer_stack, trace_info);
      return true;
    case ORDER_CANCEL:
      // dispatch_helper<OrderCancel>(handler, message, buffer_stack, trace_info);
      return true;
  }
  switch (message_2.type) {
    using enum EventType::type_t;
    case UNDEFINED_INTERNAL:
      break;
    case UNKNOWN_INTERNAL:
      if (allow_unknown_event_types) {
        return false;
      }
      break;
    case ERROR:
      // XXX check that this is a real message
      dispatch_helper<Error>(handler, message, buffer_stack, trace_info);
      return true;
    case SNAPSHOT:
    case DELTA:
      // note! drop
      break;
    case COMMAND_RESP:
      dispatch_helper<Subscribe>(handler, message, buffer_stack, trace_info);
      return true;
  }
  */
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
