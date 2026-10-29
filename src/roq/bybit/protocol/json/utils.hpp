/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>

#include "roq/error.hpp"

#include "roq/utils/charconv/from_chars.hpp"

#include "roq/core/json/parser.hpp"

namespace roq {
namespace bybit {
namespace protocol {
namespace json {

template <typename T>
inline void update(T &result, core::json::Value const &value) {
  result = core::json::get<T>(value);
}

inline auto datetime_helper(int64_t value) -> std::chrono::microseconds {
  if (value < 10000000000000) {
    return std::chrono::microseconds{value * 1000};
  }
  return std::chrono::microseconds{value};
}

template <>
inline void update(std::chrono::microseconds &result, core::json::Value const &value) {
  using result_type = std::remove_cvref_t<decltype(result)>;
  std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = result_type{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = datetime_helper(value); },
          [&](double value) { result = datetime_helper(static_cast<int64_t>(value * 1e9)); },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<double>(value);
            result = datetime_helper(tmp);
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

extern roq::Error map_error(int32_t ret_code);

}  // namespace json
}  // namespace protocol
}  // namespace bybit
}  // namespace roq
