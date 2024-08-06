/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/bybit/json/map.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {
namespace json {

// === HELPERS ===

namespace {
// note! constexpr helper for static testing
template <typename... Args>
struct Helper final {
  explicit constexpr Helper(std::tuple<Args...> const &args) : args_{args} {}
  explicit constexpr Helper(Args &&...args_) : args_{std::forward<Args>(args_)...} {}

  template <typename R>
  constexpr operator R();

 private:
  std::tuple<Args...> const args_;
};

// ==> roq

// EventType ==> roq::UpdateType

template <>
template <>
constexpr Helper<EventType>::operator roq::UpdateType() {
  switch (std::get<0>(args_)) {
    using enum EventType::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case ERROR:
      break;
    case SNAPSHOT:
      return UpdateType::SNAPSHOT;
    case DELTA:
      return UpdateType::INCREMENTAL;
    case COMMAND_RESP:
      return {};  // note!
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::UpdateType>(Helper{EventType{EventType::UNDEFINED__}}) == roq::UpdateType::UNDEFINED);
static_assert(static_cast<roq::UpdateType>(Helper{EventType{EventType::SNAPSHOT}}) == roq::UpdateType::SNAPSHOT);
static_assert(static_cast<roq::UpdateType>(Helper{EventType{EventType::DELTA}}) == roq::UpdateType::INCREMENTAL);
static_assert(static_cast<roq::UpdateType>(Helper{EventType{EventType::COMMAND_RESP}}) == roq::UpdateType::UNDEFINED);

// Status ==> roq::TradingStatus

template <>
template <>
constexpr Helper<Status>::operator roq::TradingStatus() {
  switch (std::get<0>(args_)) {
    using enum Status::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case PRE_LAUNCH:
      return {};  // note!
    case TRADING:
      return roq::TradingStatus::OPEN;
    case SETTLING:
      return {};  // note!
    case DELIVERING:
      return {};  // note!
    case CLOSED:
      return roq::TradingStatus::CLOSE;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::UNDEFINED__}}) == roq::TradingStatus::UNDEFINED);
static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::PRE_LAUNCH}}) == roq::TradingStatus::UNDEFINED);
static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::TRADING}}) == roq::TradingStatus::OPEN);
static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::SETTLING}}) == roq::TradingStatus::UNDEFINED);
static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::DELIVERING}}) == roq::TradingStatus::UNDEFINED);
static_assert(static_cast<roq::TradingStatus>(Helper{Status{Status::CLOSED}}) == roq::TradingStatus::CLOSE);

// Side ==> roq::Side

template <>
template <>
constexpr Helper<Side>::operator roq::Side() {
  switch (std::get<0>(args_)) {
    using enum Side::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case NONE:
      return {};  // note!
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::Side>(Helper{Side{Side::UNDEFINED__}}) == roq::Side::UNDEFINED);
static_assert(static_cast<roq::Side>(Helper{Side{Side::NONE}}) == roq::Side::UNDEFINED);
static_assert(static_cast<roq::Side>(Helper{Side{Side::BUY}}) == roq::Side::BUY);
static_assert(static_cast<roq::Side>(Helper{Side{Side::SELL}}) == roq::Side::SELL);

// OrderType ==> roq::OrderType

template <>
template <>
constexpr Helper<OrderType>::operator roq::OrderType() {
  switch (std::get<0>(args_)) {
    using enum json::OrderType::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case UNKNOWN:
      break;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::OrderType>(Helper{OrderType{OrderType::UNDEFINED__}}) == roq::OrderType::UNDEFINED);
static_assert(static_cast<roq::OrderType>(Helper{OrderType{OrderType::MARKET}}) == roq::OrderType::MARKET);
static_assert(static_cast<roq::OrderType>(Helper{OrderType{OrderType::LIMIT}}) == roq::OrderType::LIMIT);

// TimeInForce ==> roq::TimeInForce

template <>
template <>
constexpr Helper<TimeInForce>::operator roq::TimeInForce() {
  switch (std::get<0>(args_)) {
    using enum json::TimeInForce::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case GTC:
      return roq::TimeInForce::GTC;
    case FOK:
      return roq::TimeInForce::FOK;
    case IOC:
      return roq::TimeInForce::IOC;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::TimeInForce>(Helper{TimeInForce{TimeInForce::UNDEFINED__}}) == roq::TimeInForce::UNDEFINED);
static_assert(static_cast<roq::TimeInForce>(Helper{TimeInForce{TimeInForce::GTC}}) == roq::TimeInForce::GTC);
static_assert(static_cast<roq::TimeInForce>(Helper{TimeInForce{TimeInForce::FOK}}) == roq::TimeInForce::FOK);
static_assert(static_cast<roq::TimeInForce>(Helper{TimeInForce{TimeInForce::IOC}}) == roq::TimeInForce::IOC);

// OrderStatus ==> roq::OrderStatus

template <>
template <>
constexpr Helper<OrderStatus>::operator roq::OrderStatus() {
  switch (std::get<0>(args_)) {
    using enum json::OrderStatus::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
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
      return {};  // note!
    case TRIGGERED:
      return {};  // note!
    case DEACTIVATED:
      return {};  // note!
    case ACTIVE:
      return {};  // note!
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::UNDEFINED__}}) == roq::OrderStatus::UNDEFINED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::CREATED}}) == roq::OrderStatus::WORKING);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::NEW}}) == roq::OrderStatus::WORKING);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::REJECTED}}) == roq::OrderStatus::REJECTED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::PARTIALLY_FILLED}}) == roq::OrderStatus::WORKING);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::PARTIALLY_FILLED_CANCELED}}) == roq::OrderStatus::CANCELED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::FILLED}}) == roq::OrderStatus::COMPLETED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::CANCELLED}}) == roq::OrderStatus::CANCELED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::UNTRIGGERED}}) == roq::OrderStatus::UNDEFINED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::TRIGGERED}}) == roq::OrderStatus::UNDEFINED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::DEACTIVATED}}) == roq::OrderStatus::UNDEFINED);
static_assert(static_cast<roq::OrderStatus>(Helper{OrderStatus{OrderStatus::ACTIVE}}) == roq::OrderStatus::UNDEFINED);

// OptionsType ==> roq::OptionType

template <>
template <>
constexpr Helper<OptionsType>::operator roq::OptionType() {
  switch (std::get<0>(args_)) {
    using enum OptionsType::type_t;
    case UNDEFINED__:
      return {};
    case UNKNOWN__:
      break;
    case CALL:
      return roq::OptionType::CALL;
    case PUT:
      return roq::OptionType::PUT;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<roq::OptionType>(Helper{OptionsType{OptionsType::UNDEFINED__}}) == roq::OptionType::UNDEFINED);
static_assert(static_cast<roq::OptionType>(Helper{OptionsType{OptionsType::CALL}}) == roq::OptionType::CALL);
static_assert(static_cast<roq::OptionType>(Helper{OptionsType{OptionsType::PUT}}) == roq::OptionType::PUT);

// { ContractType, OptionsType } ==> roq::SecurityType

template <>
template <>
constexpr Helper<ContractType, OptionsType>::operator roq::SecurityType() {
  switch (std::get<1>(args_)) {
    using enum OptionsType::type_t;
    case UNDEFINED__:
    case UNKNOWN__:
      break;
    case CALL:
    case PUT:
      return SecurityType::OPTION;
  }
  switch (std::get<0>(args_)) {
    using enum ContractType::type_t;
    case UNDEFINED__:
    case UNKNOWN__:
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
    static_cast<roq::SecurityType>(Helper{ContractType{ContractType::UNDEFINED__}, OptionsType{OptionsType::UNDEFINED__}}) == roq::SecurityType::SPOT);
static_assert(static_cast<roq::SecurityType>(Helper{ContractType{ContractType::UNDEFINED__}, OptionsType{OptionsType::CALL}}) == roq::SecurityType::OPTION);
static_assert(static_cast<roq::SecurityType>(Helper{ContractType{ContractType::UNDEFINED__}, OptionsType{OptionsType::PUT}}) == roq::SecurityType::OPTION);
static_assert(
    static_cast<roq::SecurityType>(Helper{ContractType{ContractType::INVERSE_PERPETUAL}, OptionsType{OptionsType::UNDEFINED__}}) == roq::SecurityType::SWAP);
static_assert(
    static_cast<roq::SecurityType>(Helper{ContractType{ContractType::LINEAR_PERPETUAL}, OptionsType{OptionsType::UNDEFINED__}}) == roq::SecurityType::SWAP);
static_assert(
    static_cast<roq::SecurityType>(Helper{ContractType{ContractType::LINEAR_FUTURES}, OptionsType{OptionsType::UNDEFINED__}}) == roq::SecurityType::FUTURES);
static_assert(
    static_cast<roq::SecurityType>(Helper{ContractType{ContractType::INVERSE_FUTURES}, OptionsType{OptionsType::UNDEFINED__}}) == roq::SecurityType::FUTURES);

// roq ==>

// roq::Side ==> Side

template <>
template <>
constexpr Helper<roq::Side>::operator Side() {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return {};
    case BUY:
      return Side::BUY;
    case SELL:
      return Side::SELL;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<Side>(Helper{roq::Side::UNDEFINED}) == Side::UNDEFINED__);
static_assert(static_cast<Side>(Helper{roq::Side::BUY}) == Side::BUY);
static_assert(static_cast<Side>(Helper{roq::Side::SELL}) == Side::SELL);

template <>
template <>
constexpr Helper<roq::OrderType>::operator OrderType() {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return {};
    case MARKET:
      return json::OrderType::MARKET;
    case LIMIT:
      return json::OrderType::LIMIT;
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<OrderType>(Helper{roq::OrderType::UNDEFINED}) == OrderType::UNDEFINED__);
static_assert(static_cast<OrderType>(Helper{roq::OrderType::MARKET}) == OrderType::MARKET);
static_assert(static_cast<OrderType>(Helper{roq::OrderType::LIMIT}) == OrderType::LIMIT);

template <>
template <>
constexpr Helper<roq::TimeInForce>::operator TimeInForce() {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return {};
    case GFD:
      return {};  // note!
    case GTC:
      return json::TimeInForce::GTC;
    case OPG:
      return {};  // note!
    case IOC:
      return json::TimeInForce::IOC;
    case FOK:
      return json::TimeInForce::FOK;
    case GTX:
      return {};  // note!
    case GTD:
      return {};  // note!
    case AT_THE_CLOSE:
      return {};  // note!
    case GOOD_THROUGH_CROSSING:
      return {};  // note!
    case AT_CROSSING:
      return {};
    case GOOD_FOR_TIME:
      return {};
    case GFA:
      return {};  // note!
    case GFM:
      return {};  // note!
  }
  roq::log::fatal("Unexpected"sv);
}

static_assert(static_cast<TimeInForce>(Helper{roq::TimeInForce::UNDEFINED}) == TimeInForce::UNDEFINED__);
static_assert(static_cast<TimeInForce>(Helper{roq::TimeInForce::GTC}) == TimeInForce::GTC);
static_assert(static_cast<TimeInForce>(Helper{roq::TimeInForce::IOC}) == TimeInForce::IOC);
static_assert(static_cast<TimeInForce>(Helper{roq::TimeInForce::FOK}) == TimeInForce::FOK);
}  // namespace

// === IMPLEMENTATION ===

// ==> roq

template <>
template <>
Map<EventType>::operator roq::UpdateType() {
  return Helper{args_};
}

template <>
template <>
Map<Status>::operator roq::TradingStatus() {
  return Helper{args_};
}

template <>
template <>
Map<Side>::operator roq::Side() {
  return Helper{args_};
}

template <>
template <>
Map<OrderType>::operator roq::OrderType() {
  return Helper{args_};
}

template <>
template <>
Map<TimeInForce>::operator roq::TimeInForce() {
  return Helper{args_};
}

template <>
template <>
Map<OrderStatus>::operator roq::OrderStatus() {
  return Helper{args_};
}

template <>
template <>
Map<OptionsType>::operator roq::OptionType() {
  return Helper{args_};
}

template <>
template <>
Map<ContractType, OptionsType>::operator roq::SecurityType() {
  return Helper{args_};
}

// roq ==>

template <>
template <>
Map<roq::Side>::operator Side() {
  return Helper{args_};
}

template <>
template <>
Map<roq::OrderType>::operator OrderType() {
  return Helper{args_};
}

template <>
template <>
Map<roq::TimeInForce>::operator TimeInForce() {
  return Helper{args_};
}

}  // namespace json
}  // namespace bybit
}  // namespace roq
