/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "roq/oms/order.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/core/charconv.hpp"
#include "roq/core/charconv/datetime.hpp"

#include "roq/bybit/json/order_status.hpp"
#include "roq/bybit/json/order_type.hpp"
#include "roq/bybit/json/side.hpp"
#include "roq/bybit/json/time_in_force.hpp"

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
      overloaded{
          [&](core::json::Null const &) { result = std::chrono::milliseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::milliseconds{value}; },
          [&](double value) { result = std::chrono::milliseconds{static_cast<int64_t>(value * 1000000.0)}; },
          [&](std::string_view const &value) {
            result = core::charconv::datetime_from_string<std::remove_reference<decltype(result)>::type>(value);
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::microseconds &result, core::json::Value const &value) {
  return std::visit(
      overloaded{
          [&](core::json::Null const &) { result = std::chrono::microseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::microseconds{value}; },
          [&](double value) { result = std::chrono::microseconds{static_cast<int64_t>(value * 1000000.0)}; },
          [&](std::string_view const &value) {
            auto tmp = core::from_chars<double>(value);
            result = std::chrono::microseconds{static_cast<int64_t>(tmp * 1000000.0)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

template <>
inline void update(std::chrono::nanoseconds &result, core::json::Value const &value) {
  return std::visit(
      overloaded{
          [&](core::json::Null const &) { result = std::chrono::nanoseconds{}; },
          [](bool) { throw std::bad_cast{}; },
          [&](int64_t value) { result = std::chrono::nanoseconds{value}; },
          [&](double value) { result = std::chrono::nanoseconds{static_cast<int64_t>(value * 1.0e9)}; },
          [&](std::string_view const &value) {
            auto tmp = core::from_chars<double>(value);
            result = std::chrono::nanoseconds{static_cast<int64_t>(tmp * 1.0e9)};
          },
          [](core::json::Object const &) { throw std::bad_cast{}; },
          [](core::json::Array const &) { throw std::bad_cast{}; },
      },
      value);
}

// side

inline roq::Side map(json::Side value) {
  switch (value) {
    using enum json::Side::type_t;
    case UNDEFINED:
      break;
    case UNKNOWN:
      break;
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  return {};
}

inline json::Side map(roq::Side value) {
  switch (value) {
    using enum roq::Side;
    case UNDEFINED:
      break;
    case BUY:
      return json::Side::BUY;
    case SELL:
      return json::Side::SELL;
  }
  return {};
}

// order type
// XXX need maker

inline roq::OrderType map(json::OrderType value) {
  switch (value) {
    using enum json::OrderType::type_t;
    case UNDEFINED:
      break;
    case UNKNOWN:
      break;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
    case LIMIT_MAKER:
      return roq::OrderType::LIMIT;
  }
  return {};
}

inline json::OrderType map(roq::OrderType value) {
  switch (value) {
    using enum roq::OrderType;
    case UNDEFINED:
      break;
    case MARKET:
      return json::OrderType::MARKET;
    case LIMIT:
      return json::OrderType::LIMIT;
  }
  return {};
}

// time in force

inline roq::TimeInForce map(json::TimeInForce value) {
  switch (value) {
    using enum json::TimeInForce::type_t;
    case UNDEFINED:
      break;
    case UNKNOWN:
      break;
    case GTC:
      return roq::TimeInForce::GTC;
    case FOK:
      return roq::TimeInForce::FOK;
    case IOC:
      return roq::TimeInForce::IOC;
  }
  return {};
}

inline json::TimeInForce map(roq::TimeInForce value) {
  switch (value) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      break;
    case GFD:
      break;
    case GTC:
      return json::TimeInForce::GTC;
    case OPG:
      break;
    case IOC:
      return json::TimeInForce::IOC;
    case FOK:
      return json::TimeInForce::FOK;
    case GTX:
      break;
    case GTD:
      break;
    case AT_THE_CLOSE:
      break;
    case GOOD_THROUGH_CROSSING:
      break;
    case AT_CROSSING:
      break;
    case GOOD_FOR_TIME:
      break;
    case GFA:
      break;
    case GFM:
      break;
  }
  return {};
}

// order status

inline roq::OrderStatus map(json::OrderStatus value) {
  switch (value) {
    using enum json::OrderStatus::type_t;
    case UNDEFINED:
      break;
    case UNKNOWN:
      break;
    case NEW:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELED:
      return roq::OrderStatus::CANCELED;
    case PENDING_CANCEL:
      return roq::OrderStatus::WORKING;
    case PENDING_NEW:
      return roq::OrderStatus::SENT;
    case REJECTED:
      return roq::OrderStatus::REJECTED;
  }
  return {};
}

extern std::string_view strip_symbol(std::string_view const &topic);

extern std::string_view create_order(
    std::string &buffer, roq::CreateOrder const &, oms::Order const &, std::string_view const &request_id);

extern std::string_view cancel_order(
    std::string &buffer,
    roq::CancelOrder const &,
    oms::Order const &,
    std::string_view const &request_id,
    std::string_view const &prevoius_request_id);

extern std::string_view cancel_all_orders(
    std::string &buffer, CancelAllOrders const &, std::string_view const &request_id, std::string_view const &symbol);

extern Error map_error(int32_t ret_code);

}  // namespace json
}  // namespace bybit
}  // namespace roq
