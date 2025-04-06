/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/bybit/json/contract_type.hpp"
#include "roq/bybit/json/event_type.hpp"
#include "roq/bybit/json/options_type.hpp"
#include "roq/bybit/json/order_status.hpp"
#include "roq/bybit/json/order_type.hpp"
#include "roq/bybit/json/side.hpp"
#include "roq/bybit/json/status.hpp"
#include "roq/bybit/json/time_in_force.hpp"

#include "roq/option_type.hpp"
#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/security_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/trading_status.hpp"
#include "roq/update_type.hpp"

#include "roq/map.hpp"

namespace roq {

template <>
template <>
std::optional<UpdateType> Map<bybit::json::EventType>::helper() const;

template <>
template <>
std::optional<OptionType> Map<bybit::json::OptionsType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<bybit::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<bybit::json::OrderType>::helper() const;

template <>
template <>
std::optional<Side> Map<bybit::json::Side>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<bybit::json::Status>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<bybit::json::TimeInForce>::helper() const;

// ===

template <>
template <>
std::optional<SecurityType> Map<bybit::json::ContractType, bybit::json::OptionsType>::helper() const;

// ===

template <>
template <>
std::optional<bybit::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<bybit::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<bybit::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
