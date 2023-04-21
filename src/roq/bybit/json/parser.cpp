/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/core/charconv/number.hpp"

#include "roq/core/json/array_parser.hpp"

#include "roq/bybit/json/message_field.hpp"
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

constexpr auto parse_mbp_depth(auto const &value) {
  auto pos1 = value.find_first_of('.');
  ++pos1;
  auto pos2 = value.find_first_of('.', pos1);
  auto sub = value.substr(pos1, pos2 - pos1);
  return core::charconv::from_string<size_t>(sub);
}

static_assert(parse_mbp_depth("orderbook.1.xxx"sv) == 1);
static_assert(parse_mbp_depth("orderbook.50.xxx"sv) == 50);

template <typename T>
void dispatch_helper(
    auto &handler, auto &buffer, auto &trace_info, auto &value, auto type, auto &topic, auto timestamp) {
  using data_type = decltype(T::data);
  T result;
  result.type = type;
  result.topic = topic;
  result.timestamp = timestamp;
  if constexpr (utils::is_iterable<data_type>::value) {
    if (!core::json::is_null(value)) {
      result.data =
          core::json::ArrayParser<data_type, core::json::Array>::parse(buffer, std::get<core::json::Array>(value));
    }
    create_trace_and_dispatch(handler, trace_info, result);
  } else {
    new (&result.data) data_type{value, buffer};
    create_trace_and_dispatch(handler, trace_info, result);
  }
}

template <typename T>
void dispatch_helper_flatten(
    auto &handler, auto &buffer, auto &trace_info, auto &value, auto type, auto &topic, auto timestamp) {
  using data_type = decltype(T::data);
  T result;
  result.type = type;
  result.topic = topic;
  result.ts = timestamp;
  assert(core::json::is_array(value));
  for (auto item : std::get<core::json::Array>(value)) {
    new (&result.data) data_type{item, buffer};
    create_trace_and_dispatch(handler, trace_info, result);
  }
}
}  // namespace

// === IMPLEMENTATION ===

// note!
// 2-pass parsing required due to "data" potentially being nested arrays (OutboundAccountInfo)
// we only prioritize the efficiency of messages containing the "data" field
bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::Buffer &buffer, TraceInfo const &trace_info) {
  Operation operation;
  EventType type;
  std::string_view raw_topic;
  Topic topic;
  std::chrono::milliseconds timestamp = {};
  for (auto i = 0; i < 2; ++i) {
    core::json::Parser parser{message};
    auto root = parser.root();
    for (auto [key, value] : std::get<core::json::Object>(root)) {
      MessageField field{key};
      switch (field) {
        using enum MessageField::type_t;
        case UNDEFINED:
        case UNKNOWN:
          break;
        case OP:
          operation = value;
          switch (operation) {
            using enum Operation::type_t;
            case UNDEFINED:
            case UNKNOWN:
              break;
            case AUTH: {
              Auth auth{message, buffer};
              create_trace_and_dispatch(handler, trace_info, auth);
              return true;
            }
            case PING:
            case PONG: {
              Pong pong{message, buffer};
              create_trace_and_dispatch(handler, trace_info, pong);
              return true;
            }
            case SUBSCRIBE: {
              Subscribe subscribe{message, buffer};
              create_trace_and_dispatch(handler, trace_info, subscribe);
              return true;
            }
          }
          break;
        case TYPE:
          type = value;
          switch (type) {
            using enum EventType::type_t;
            case UNDEFINED:
            case UNKNOWN:
              break;
            case ERROR: {
              Error error{message, buffer};
              create_trace_and_dispatch(handler, trace_info, error);
              return true;
            }
            case SNAPSHOT:
            case DELTA:
              break;
          }
          break;
        case TOPIC:
          raw_topic = std::get<std::string_view>(value);
          topic = parse_topic(raw_topic);
          switch (topic) {
            using enum Topic::type_t;
            case UNDEFINED:
            case UNKNOWN:
              break;
            case ORDERBOOK: {
              OrderBook order_book{message, buffer};
              auto mbp_depth = parse_mbp_depth(raw_topic);
              create_trace_and_dispatch(handler, trace_info, order_book, mbp_depth);
              return true;
              break;
            }
            case PUBLIC_TRADE: {
              PublicTrade public_trade{message, buffer};
              create_trace_and_dispatch(handler, trace_info, public_trade);
              return true;
            }
            case TICKERS: {
              Tickers tickers{message, buffer};
              create_trace_and_dispatch(handler, trace_info, tickers);
              return true;
            }
            case OUTBOUND_ACCOUNT_INFO:
              break;
              return true;
            case ORDER:
              break;
            case TICKET_INFO:
              break;
          }
          break;
        case TS:
          update(timestamp, value);
          break;
        case DATA: {
          auto ready = type != EventType::UNDEFINED && timestamp.count();
          if (ready) {
            switch (topic) {
              using enum Topic::type_t;
              case UNDEFINED:
              case UNKNOWN:
                break;
              case ORDERBOOK:
                log::fatal("Unexpected"sv);
                return true;
              case PUBLIC_TRADE:
                log::fatal("Unexpected"sv);
                return true;
              case TICKERS:
                log::fatal("Unexpected"sv);
                return true;
              case OUTBOUND_ACCOUNT_INFO:
                dispatch_helper_flatten<OutboundAccountInfo>(
                    handler, buffer, trace_info, value, type, raw_topic, timestamp);
                return true;
              case ORDER:
                dispatch_helper<Order>(handler, buffer, trace_info, value, type, raw_topic, timestamp);
                return true;
              case TICKET_INFO:
                dispatch_helper<TicketInfo>(handler, buffer, trace_info, value, type, raw_topic, timestamp);
                return true;
            }
          }
          break;
        }
        case ARGS:
        case REQ_ID:
        case SUCCESS:
        case RET_CODE:
        case RET_MSG:
        case CONN_ID:
          break;
      };
    }
  }
  return false;
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
