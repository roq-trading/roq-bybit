/* Copyright (c) 2017-2025, Hans Erik Thrane */

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
auto parse_topic(auto const &value) {
  return Topic{value.substr(0, value.find_first_of('.'))};
}

auto parse_kline_symbol(auto const &value) {
  auto pos = value.find_last_of('.');
  if (pos == value.npos) {
    return std::string_view{};
  }
  return value.substr(pos + 1);
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

bool dispatch_helper_flatten_wallet(auto &handler, auto &buffer_stack, auto &trace_info, auto &message) {
  core::json::Parser parser{message};
  auto root = parser.root();
  for (auto [key, value] : std::get<core::json::Object>(root)) {
    if (key == "data"sv) {
      for (auto item : std::get<core::json::Array>(value)) {
        Wallet wallet{item, buffer_stack};
        create_trace_and_dispatch(handler, trace_info, wallet);
      }
      return true;
    }
  }
  return false;
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser::dispatch(Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info) {
  Message message_{message, buffer_stack};
  auto topic = parse_topic(message_.topic);
  switch (topic) {
    using enum Topic::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case ORDERBOOK: {
      OrderBook order_book{message, buffer_stack};
      auto mbp_depth = parse_mbp_depth(message_.topic);
      create_trace_and_dispatch(handler, trace_info, order_book, mbp_depth);
      return true;
      break;
    }
    case PUBLIC_TRADE: {
      PublicTrade public_trade{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, public_trade);
      return true;
    }
    case TICKERS: {
      Tickers tickers{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, tickers);
      return true;
    }
    case KLINE: {
      Kline kline{message, buffer_stack};
      kline.symbol = parse_kline_symbol(message_.topic);
      create_trace_and_dispatch(handler, trace_info, kline);
      return true;
    }
    case WALLET:
      return dispatch_helper_flatten_wallet(handler, buffer_stack, trace_info, message);
    case POSITION: {
      Position position{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, position);
      return true;
    }
    case ORDER: {
      Order order{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, order);
      return true;
    }
    case EXECUTION: {
      Execution2 execution{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, execution);
      return true;
    }
  }
  switch (message_.op) {
    using enum Operation::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case AUTH: {
      Auth auth{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, auth);
      return true;
    }
    case PING: {
      Ping ping{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, ping);
      return true;
    }
    case PONG: {
      // note! don't process (only the option api)
      return true;
    }
    case SUBSCRIBE: {
      Subscribe subscribe{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, subscribe);
      return true;
    }
  }
  switch (message_.type) {
    using enum EventType::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case ERROR: {
      // XXX check that this is a real message
      Error error{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, error);
      return true;
    }
    case SNAPSHOT:
    case DELTA:
      break;
    case COMMAND_RESP: {
      Subscribe subscribe{message, buffer_stack};
      create_trace_and_dispatch(handler, trace_info, subscribe);
      return true;
    }
  }
  return false;
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
