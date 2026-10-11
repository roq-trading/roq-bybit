/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

namespace roq {
namespace bybit {
namespace gateway {

struct Response final {
  std::string_view account;
  std::string_view topic;
  std::string_view symbol;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
