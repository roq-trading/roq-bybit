/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "roq/server/oms/order.hpp"

#include "roq/utils/patterns.hpp"

#include "roq/utils/charconv/from_chars.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/core/charconv/datetime.hpp"

#include "roq/bybit/json/category.hpp"

namespace roq {
namespace bybit {
namespace json {

template <typename T>
inline void update(T &result, core::json::Value const &value) {
  result = core::json::get<T>(value);
}

template <>
inline void update(std::chrono::milliseconds &result, core::json::Value const &value) {
  return std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = std::chrono::milliseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::milliseconds{value}; },
          [&](double value) { result = std::chrono::milliseconds{static_cast<int64_t>(value * 1e3)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<int64_t>(value);
            // note! have seen 1000
            result = std::chrono::milliseconds{tmp > 1000 ? tmp : 0};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::microseconds &result, core::json::Value const &value) {
  return std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = std::chrono::microseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::microseconds{value}; },
          [&](double value) { result = std::chrono::microseconds{static_cast<int64_t>(value * 1e6)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<double>(value);
            result = std::chrono::microseconds{static_cast<int64_t>(tmp * 1e6)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::nanoseconds &result, core::json::Value const &value) {
  return std::visit(
      utils::overloaded{
          [&](core::json::Null const &) { result = std::chrono::nanoseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::nanoseconds{value}; },
          [&](double value) { result = std::chrono::nanoseconds{static_cast<int64_t>(value * 1e9)}; },
          [&](std::string_view const &value) {
            auto tmp = utils::charconv::from_chars<double>(value);
            result = std::chrono::nanoseconds{static_cast<int64_t>(tmp * 1e9)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

extern std::string_view place_order(std::string &buffer, roq::CreateOrder const &, server::oms::Order const &, std::string_view const &request_id, Category);

extern std::string_view amend_order(
    std::string &buffer,
    roq::ModifyOrder const &,
    server::oms::Order const &,
    std::string_view const &request_id,
    std::string_view const &previous_request_id,
    Category);

extern std::string_view cancel_order(
    std::string &buffer,
    roq::CancelOrder const &,
    server::oms::Order const &,
    std::string_view const &request_id,
    std::string_view const &previous_request_id,
    Category);

extern std::string_view cancel_all_orders(
    std::string &buffer, roq::CancelAllOrders const &, std::string_view const &request_id, std::string_view const &symbol, Category);

extern Error map_error(int32_t ret_code);

}  // namespace json
}  // namespace bybit
}  // namespace roq
