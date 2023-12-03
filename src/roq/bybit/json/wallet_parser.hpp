/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/bybit/json/wallet.hpp"

namespace roq {
namespace bybit {
namespace json {

struct WalletParser final {
  struct Handler {
    virtual void operator()(Trace<json::Wallet> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, std::span<std::byte> const &, TraceInfo const &);
};

}  // namespace json
}  // namespace bybit
}  // namespace roq
