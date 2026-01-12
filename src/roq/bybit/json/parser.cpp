/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/utils/charconv/from_chars.hpp"

#include "roq/core/json/array_parser.hpp"

#include "roq/bybit/json/message.hpp"
#include "roq/bybit/json/topic.hpp"
#include "roq/bybit/json/utils.hpp"

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

auto parse_topic(auto const &value) {
  return Topic{value.substr(0, value.find_first_of('.'))};
}

constexpr auto parse_mbp_depth(auto const &value) {
  auto pos1 = value.find_first_of('.');
  ++pos1;
  auto pos2 = value.find_first_of('.', pos1);
  auto sub = value.substr(pos1, pos2 - pos1);
  return utils::charconv::from_chars<uint32_t>(sub);
}

// static_assert(parse_mbp_depth("orderbook.1.xxx"sv) == 1);
// static_assert(parse_mbp_depth("orderbook.50.xxx"sv) == 50);
}  // namespace

// === IMPLEMENTATION ===

bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
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
      dispatch_helper<Wallet>(handler, message, buffer_stack, trace_info);
      return true;
    case POSITION:
      dispatch_helper<Position>(handler, message, buffer_stack, trace_info);
      return true;
    case ORDER:
      dispatch_helper<Order>(handler, message, buffer_stack, trace_info);
      return true;
    case EXECUTION:
      dispatch_helper<Execution>(handler, message, buffer_stack, trace_info);
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
      dispatch_helper<Auth>(handler, message, buffer_stack, trace_info);
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
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
