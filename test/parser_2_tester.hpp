/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/protocol/json/parser_2.hpp"

namespace roq {
namespace bybit {

template <typename T>
struct Parser2Tester final : public protocol::json::Parser2::Handler {
  using value_type = std::remove_cvref_t<T>;
  using callback_type = std::function<void(value_type const &)>;  // XXX FIXME TODO doesn't accept multiple arguments...

  static void dispatch(callback_type const &callback, std::string_view const &message, size_t buffer_size, size_t max_depth) {
    core::json::BufferStack buffers{buffer_size, max_depth};
    // simple
    // XXX FIXME TODO catch2 block ???
    T obj{message, buffers};
    callback(obj);
    // parser
    // XXX FIXME TODO catch2 block ???
    Parser2Tester handler{callback};
    auto res = protocol::json::Parser2::dispatch(handler, message, buffers, {}, false);
    CHECK(res == true);
    CHECK(handler.found_ == true);
  }

 protected:
  explicit Parser2Tester(callback_type const &callback) : callback_{callback} {}

  void operator()(Trace<protocol::json::Ping> const &event) override { dispatch_helper(event); }
  void operator()(Trace<protocol::json::Auth2> const &event) override { dispatch_helper(event); }
  void operator()(Trace<protocol::json::Error> const &event) override { dispatch_helper(event); }
  void operator()(Trace<protocol::json::PlaceOrder2> const &event) override { dispatch_helper(event); }
  void operator()(Trace<protocol::json::AmendOrder2> const &event) override { dispatch_helper(event); }
  void operator()(Trace<protocol::json::CancelOrder2> const &event) override { dispatch_helper(event); }

  template <typename U>
  void dispatch_helper(Trace<U> const &event) {
    if constexpr (std::is_invocable_v<callback_type, U>) {
      found_ = true;
      callback_(event);
    } else {
      FAIL();
    }
  }

 private:
  callback_type const callback_;
  bool found_ = false;
};

}  // namespace bybit
}  // namespace roq
