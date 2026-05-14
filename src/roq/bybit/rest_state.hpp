/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <cstdint>

namespace roq {
namespace bybit {

enum class RestState : uint8_t {
  UNDEFINED = 0,
  GET_INSTRUMENTS_INFO,
  DONE,
};

}  // namespace bybit
}  // namespace roq
