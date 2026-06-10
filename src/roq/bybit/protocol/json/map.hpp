/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/bybit/protocol/json/contract_type.hpp"
#include "roq/bybit/protocol/json/event_type.hpp"
#include "roq/bybit/protocol/json/options_type.hpp"
#include "roq/bybit/protocol/json/order_status.hpp"
#include "roq/bybit/protocol/json/order_type.hpp"
#include "roq/bybit/protocol/json/side.hpp"
#include "roq/bybit/protocol/json/status.hpp"
#include "roq/bybit/protocol/json/time_in_force.hpp"

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
std::optional<UpdateType> Map<bybit::protocol::json::EventType>::helper() const;

template <>
template <>
std::optional<OptionType> Map<bybit::protocol::json::OptionsType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<bybit::protocol::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<bybit::protocol::json::OrderType>::helper() const;

template <>
template <>
std::optional<Side> Map<bybit::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<bybit::protocol::json::Status>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<bybit::protocol::json::TimeInForce>::helper() const;

// ===

template <>
template <>
std::optional<SecurityType> Map<bybit::protocol::json::ContractType, bybit::protocol::json::OptionsType>::helper() const;

// ===

template <>
template <>
std::optional<bybit::protocol::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<bybit::protocol::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<bybit::protocol::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
