/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <chrono>

#include "roq/error.hpp"

#include "roq/utils/charconv/from_chars.hpp"

#include "roq/core/json/parser.hpp"

namespace roq {
namespace bybit {
namespace json {

template <typename T>
inline void update(T &result, core::json::Value const &value) {
  result = core::json::get<T>(value);
}

template <>
inline void update(std::chrono::milliseconds &result, core::json::Value const &value) {
  using result_type = std::remove_cvref_t<decltype(result)>;
  std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = result_type{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = result_type{value}; },
          [&](double value) { result = result_type{static_cast<int64_t>(value * 1e3)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<int64_t>(value);
            // note! have seen 1000
            result = result_type{tmp > 1000 ? tmp : 0};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::microseconds &result, core::json::Value const &value) {
  using result_type = std::remove_cvref_t<decltype(result)>;
  std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = result_type{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = result_type{value}; },
          [&](double value) { result = result_type{static_cast<int64_t>(value * 1e6)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<double>(value);
            result = result_type{static_cast<int64_t>(tmp * 1e6)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::nanoseconds &result, core::json::Value const &value) {
  using result_type = std::remove_cvref_t<decltype(result)>;
  std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = result_type{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = result_type{value}; },
          [&](double value) { result = result_type{static_cast<int64_t>(value * 1e9)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<double>(value);
            result = result_type{static_cast<int64_t>(tmp * 1e9)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

extern roq::Error map_error(int32_t ret_code);

}  // namespace json
}  // namespace bybit
}  // namespace roq
