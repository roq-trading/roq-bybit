/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// bybit::json => roq

// bybit::json::EventType ==> roq::UpdateType

template <>
template <>
constexpr Helper<bybit::json::EventType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::EventType::type_t;
    case UNDEFINED_INTERNAL:
      return UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return UpdateType::UNDEFINED;
    case ERROR:
      return UpdateType::UNDEFINED;
    case SNAPSHOT:
      return UpdateType::SNAPSHOT;
    case DELTA:
      return UpdateType::INCREMENTAL;
    case COMMAND_RESP:
      return UpdateType::UNDEFINED;
  }
  return {};
}

static_assert(Helper{bybit::json::EventType{bybit::json::EventType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{bybit::json::EventType{bybit::json::EventType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{bybit::json::EventType{bybit::json::EventType::DELTA}} == roq::UpdateType::INCREMENTAL);
static_assert(Helper{bybit::json::EventType{bybit::json::EventType::COMMAND_RESP}} == roq::UpdateType::UNDEFINED);

template <>
template <>
std::optional<roq::UpdateType> Map<bybit::json::EventType>::helper() const {
  return Helper{args_};
}

// bybit::json::OptionsType ==> roq::OptionType

template <>
template <>
constexpr Helper<bybit::json::OptionsType>::operator std::optional<roq::OptionType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::OptionsType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OptionType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OptionType::UNDEFINED;
    case CALL:
      return roq::OptionType::CALL;
    case PUT:
      return roq::OptionType::PUT;
  }
  return {};
}

static_assert(Helper{bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} == roq::OptionType::UNDEFINED);
static_assert(Helper{bybit::json::OptionsType{bybit::json::OptionsType::CALL}} == roq::OptionType::CALL);
static_assert(Helper{bybit::json::OptionsType{bybit::json::OptionsType::PUT}} == roq::OptionType::PUT);

template <>
template <>
std::optional<roq::OptionType> Map<bybit::json::OptionsType>::helper() const {
  return Helper{args_};
}

// bybit::json::OrderStatus ==> roq::OrderStatus

template <>
template <>
constexpr Helper<bybit::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::OrderStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case CREATED:
      return roq::OrderStatus::WORKING;
    case NEW:
      return roq::OrderStatus::WORKING;
    case REJECTED:
      return roq::OrderStatus::REJECTED;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED_CANCELED:
      return roq::OrderStatus::CANCELED;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELLED:
      return roq::OrderStatus::CANCELED;
    case UNTRIGGERED:
      return roq::OrderStatus::UNDEFINED;
    case TRIGGERED:
      return roq::OrderStatus::UNDEFINED;
    case DEACTIVATED:
      return roq::OrderStatus::UNDEFINED;
    case ACTIVE:
      return roq::OrderStatus::UNDEFINED;
  }
  return {};
}

static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::CREATED}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::REJECTED}} == roq::OrderStatus::REJECTED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::PARTIALLY_FILLED_CANCELED}} == roq::OrderStatus::CANCELED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::CANCELLED}} == roq::OrderStatus::CANCELED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::UNTRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::TRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::DEACTIVATED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::json::OrderStatus{bybit::json::OrderStatus::ACTIVE}} == roq::OrderStatus::UNDEFINED);

template <>
template <>
std::optional<roq::OrderStatus> Map<bybit::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// bybit::json::OrderType ==> roq::OrderType

template <>
template <>
constexpr Helper<bybit::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN:
      return roq::OrderType::UNDEFINED;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{bybit::json::OrderType{bybit::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{bybit::json::OrderType{bybit::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{bybit::json::OrderType{bybit::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);

template <>
template <>
std::optional<roq::OrderType> Map<bybit::json::OrderType>::helper() const {
  return Helper{args_};
}

// bybit::json::Side ==> roq::Side

template <>
template <>
constexpr Helper<bybit::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::Side::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Side::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Side::UNDEFINED;
    case NONE:
      return roq::Side::UNDEFINED;
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  return {};
}

static_assert(Helper{bybit::json::Side{bybit::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{bybit::json::Side{bybit::json::Side::NONE}} == roq::Side::UNDEFINED);
static_assert(Helper{bybit::json::Side{bybit::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{bybit::json::Side{bybit::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<bybit::json::Side>::helper() const {
  return Helper{args_};
}

// bybit::json::Status ==> roq::TradingStatus

template <>
template <>
constexpr Helper<bybit::json::Status>::operator std::optional<roq::TradingStatus>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::Status::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case PRE_LAUNCH:
      return roq::TradingStatus::UNDEFINED;
    case TRADING:
      return roq::TradingStatus::OPEN;
    case SETTLING:
      return roq::TradingStatus::UNDEFINED;
    case DELIVERING:
      return roq::TradingStatus::UNDEFINED;
    case CLOSED:
      return roq::TradingStatus::CLOSE;
  }
  return {};
}

static_assert(Helper{bybit::json::Status{bybit::json::Status::UNDEFINED_INTERNAL}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::json::Status{bybit::json::Status::PRE_LAUNCH}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::json::Status{bybit::json::Status::TRADING}} == roq::TradingStatus::OPEN);
static_assert(Helper{bybit::json::Status{bybit::json::Status::SETTLING}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::json::Status{bybit::json::Status::DELIVERING}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::json::Status{bybit::json::Status::CLOSED}} == roq::TradingStatus::CLOSE);

template <>
template <>
std::optional<roq::TradingStatus> Map<bybit::json::Status>::helper() const {
  return Helper{args_};
}

// bybit::json::TimeInForce ==> roq::TimeInForce

template <>
template <>
constexpr Helper<bybit::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case GTC:
      return roq::TimeInForce::GTC;
    case FOK:
      return roq::TimeInForce::FOK;
    case IOC:
      return roq::TimeInForce::IOC;
  }
  return {};
}

static_assert(Helper{bybit::json::TimeInForce{bybit::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{bybit::json::TimeInForce{bybit::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{bybit::json::TimeInForce{bybit::json::TimeInForce::FOK}} == roq::TimeInForce::FOK);
static_assert(Helper{bybit::json::TimeInForce{bybit::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);

template <>
template <>
std::optional<roq::TimeInForce> Map<bybit::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// bybit::json::{ ContractType, OptionsType } ==> roq::SecurityType

template <>
template <>
constexpr Helper<bybit::json::ContractType, bybit::json::OptionsType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<1>(args_)) {
    using enum bybit::json::OptionsType::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case CALL:
    case PUT:
      return SecurityType::OPTION;
  }
  switch (std::get<0>(args_)) {
    using enum bybit::json::ContractType::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case INVERSE_PERPETUAL:
    case LINEAR_PERPETUAL:
      return SecurityType::SWAP;
    case LINEAR_FUTURES:
    case INVERSE_FUTURES:
      return SecurityType::FUTURES;
  }
  return SecurityType::SPOT;
}

static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::UNDEFINED_INTERNAL}, bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::SPOT);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::UNDEFINED_INTERNAL}, bybit::json::OptionsType{bybit::json::OptionsType::CALL}} ==
    roq::SecurityType::OPTION);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::UNDEFINED_INTERNAL}, bybit::json::OptionsType{bybit::json::OptionsType::PUT}} ==
    roq::SecurityType::OPTION);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::INVERSE_PERPETUAL}, bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::SWAP);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::LINEAR_PERPETUAL}, bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::SWAP);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::LINEAR_FUTURES}, bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::FUTURES);
static_assert(
    Helper{bybit::json::ContractType{bybit::json::ContractType::INVERSE_FUTURES}, bybit::json::OptionsType{bybit::json::OptionsType::UNDEFINED_INTERNAL}} ==
    roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<bybit::json::ContractType, bybit::json::OptionsType>::helper() const {
  return Helper{args_};
}

// roq ==>

// roq::OrderType ==> bybit::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<bybit::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return bybit::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return bybit::json::OrderType::MARKET;
    case LIMIT:
      return bybit::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == bybit::json::OrderType{bybit::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == bybit::json::OrderType{bybit::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == bybit::json::OrderType{bybit::json::OrderType::LIMIT});

template <>
template <>
std::optional<bybit::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// roq::Side ==> bybit::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<bybit::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return bybit::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return bybit::json::Side::BUY;
    case SELL:
      return bybit::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == bybit::json::Side{bybit::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == bybit::json::Side{bybit::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == bybit::json::Side{bybit::json::Side::SELL});

template <>
template <>
std::optional<bybit::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce ==> bybit::json::TimeInForce

template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<bybit::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return bybit::json::TimeInForce::GTC;
    case OPG:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return bybit::json::TimeInForce::IOC;
    case FOK:
      return bybit::json::TimeInForce::FOK;
    case GTX:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return bybit::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::TimeInForce::UNDEFINED} == bybit::json::TimeInForce{bybit::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == bybit::json::TimeInForce{bybit::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::IOC} == bybit::json::TimeInForce{bybit::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK} == bybit::json::TimeInForce{bybit::json::TimeInForce::FOK});

template <>
template <>
std::optional<bybit::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
