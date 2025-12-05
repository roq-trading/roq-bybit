/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/bybit/json/parser.hpp"

namespace roq {
namespace bybit {

template <typename T>
struct ParserTester final : public json::Parser::Handler {
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
    ParserTester handler{callback};
    auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
    CHECK(res == true);
    CHECK(handler.found_ == true);
  }

 protected:
  explicit ParserTester(callback_type const &callback) : callback_{callback} {}

  void operator()(Trace<json::Ping> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Auth> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Subscribe> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Error> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::OrderBook> const &event, [[maybe_unused]] size_t depth) override { dispatch_helper(event); }
  void operator()(Trace<json::PublicTrade> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Tickers> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Kline> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Wallet> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Position> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Order> const &event) override { dispatch_helper(event); }
  void operator()(Trace<json::Execution> const &event) override { dispatch_helper(event); }

  /* XXX FIXME TODO
  template <typename U, typename... Args>
  void dispatch_helper(Trace<U> const &event, Args &&...args) {
    if constexpr (std::is_invocable_v<callback_type, U, Args...>) {
      found_ = true;
      callback_(event, std::forward<Args>(args)...);
    } else {
      FAIL();
    }
  }
  */
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
