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
constexpr auto const KEY_OP = "op"sv;
}

// === HELPERS ===

namespace {
template <typename T, typename... Args>
auto dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info, Args &&...args) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj, std::forward<Args>(args)...);
  return true;
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser2::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  auto result = false;
  auto helper = [&](auto &key, auto &value) {
    auto key_2 = utils::hash::FNV::compute(key);
    switch (key_2) {
      case utils::hash::FNV::compute(KEY_OP): {
        Operation op{value};
        switch (op) {
          using enum Operation::type_t;
          case UNDEFINED_INTERNAL:
            log::fatal("Unexpected"sv);
          case UNKNOWN_INTERNAL:
            break;
          case AUTH:
            result = dispatch_helper<Auth2>(handler, message, buffer_stack, trace_info);
            break;
          case PING:
            break;
          case PONG:
            break;
          case SUBSCRIBE:
            break;
          case ORDER_CREATE:
            result = dispatch_helper<PlaceOrder2>(handler, message, buffer_stack, trace_info);
            break;
          case ORDER_AMEND:
            result = dispatch_helper<AmendOrder2>(handler, message, buffer_stack, trace_info);
            break;
          case ORDER_CANCEL:
            result = dispatch_helper<CancelOrder2>(handler, message, buffer_stack, trace_info);
            break;
        }
        return true;
      }
    }
    return result;
  };
  core::json::Parser::dispatch<core::json::Object>(helper, message);
  if (result || allow_unknown_event_types) {
    return result;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
