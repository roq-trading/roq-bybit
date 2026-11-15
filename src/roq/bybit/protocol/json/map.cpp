/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/protocol/json/map.hpp"

#include "roq/server/tools/log_once.hpp"

using namespace std::literals;

namespace roq {

namespace {
server::tools::LogOnce LOG_ONCE{"Map"sv};
}
namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// bybit::json => roq

// int32_t ==> roq::Error

template <>
template <>
constexpr Helper<int32_t>::operator std::optional<roq::Error>() const {
  auto value = std::get<0>(args_);
  switch (value) {
    case 0:
      return Error::UNDEFINED;
    case 5'019:  // User-service error
      break;
    case 5'004:  // Upstream server timeout
      break;
    case 7'001:  // Grpc request filed type error
      break;
    case 10'003:  // Invalid api_key
      break;
    case 10'006:  // Too many visits
      break;
    case 10'001:  // Params error
      return Error::INVALID_REQUEST_ARGS;
    case 10'002:  // Invalid request, please check your timestamp and recv_window param
      break;
    case 10'004:  // Error sign
      break;
    case 10'005:  // Permission denied
      break;
    case 12'940:  // Connection reset
      break;
    case 12'999:  // Server Error
      break;
    case 12'800:  // Invalid request
      return Error::INVALID_REQUEST_ARGS;
    case 12'801:  // Invalid JSON
      return Error::INVALID_REQUEST_ARGS;
    case 12'802:  // Invalid event
      break;
    case 12'803:  // Event required
      break;
    case 12'804:  // Invalid topic
      break;
    case 12'805:  // Topic required
      break;
    case 12'806:  // Params required
      return Error::INVALID_REQUEST_ARGS;
    case 12'807:  // Period required
      break;
    case 12'808:  // Invalid period
      break;
    case 12'809:  // Invalid Symbols
      return Error::INVALID_SYMBOL;
    case 12'810:  // Not supported symbols
      return Error::INVALID_SYMBOL;
    case 12'811:  // There is no such broker.
      break;
    case 12'812:  // DumpScale required
      break;
    case 12'813:  // Invalid exchangeId
      return Error::INVALID_EXCHANGE;
    case 12'814:  // Param %s should be %s.
      return Error::INVALID_REQUEST_ARGS;
    case 12'815:  // Symbol required
      return Error::INVALID_SYMBOL;
    case 12'816:  // Timeout. Retry later.
      return Error::TIMEOUT;
    case 12'817:  // No business with any exchange.
      break;
    case 12'818:  // Client disconnected.
      break;
    case 12'819:  // OrgId required.
      return Error::INVALID_REQUEST_ID;
    case 12'820:  // OrgId must be a number.
      return Error::INVALID_REQUEST_ID;
    case 12'821:  // DumpScale error.
      break;
    case 12'822:  // Index name error.
      break;
    case 12'823:  // Parameter error
      return Error::INVALID_REQUEST_ARGS;
    case 12'824:  // Parameter %s [%s]
      return Error::INVALID_REQUEST_ARGS;
    case 12'825:  // The number of each symbol request cannot exceed 300
      break;
    case 12'001:  // Request failed to be processed. Please try again.
      break;
    case 12'007:  // Response timeout from backend server. Delivery and request status unknown.
      break;
    case 12'014:  // Trading pairs not supported.
      return Error::INVALID_SYMBOL;
    case 12'005:  // Too many new orders. Please lower request frequency.
      break;
    case 12'016:  // Service not available.
      break;
    case 12'032:  // Fusing error
      break;
    case 12'114:  // NETWORK_BUSY
      break;
    case 12'015:  // UID_NOT_EXIST
      break;
    case 12'011:  // INNOVATION_BUY_FAILED_BY_QUOTA
      break;
    case 12'010:  // ETP_BUY_FAILED_BY_QUOTA
      break;
    case 12'012:  // ETP_NAV_IS_ABNORMAL
      break;
    case 12'017:  // CROSS_MARGIN_PRE_BUY_QUOTA_FAILED
      break;
    case 10'020:  // CROSS_MARGIN_PRE_SELL_QUOTA_FAILED
      break;
    case 12'019:  // Your account has been restricted for trades. If you have any questions, please email us at
                  // support@bybit.com
      break;
    case 12'124:  // Order amount is too big
      return Error::INSUFFICIENT_FUNDS;
    case 12'031:  // The feature has been suspended
      break;
    case 12'105:  // Empty parameter.
      break;
    case 12'115:  // Invalid timeInForce.
      return Error::INVALID_TIME_IN_FORCE;
    case 12'116:  // Invalid orderType.
      return Error::INVALID_ORDER_TYPE;
    case 12'117:  // Invalid direction.
      return Error::INVALID_SIDE;
    case 12'121:  // Invalid symbol.
      return Error::INVALID_SYMBOL;
    case 12'130:  // Invalid parameter sent.
      return Error::INVALID_REQUEST_ARGS;
    case 12'131:  // Insufficient balance
      return Error::INSUFFICIENT_FUNDS;
    case 12'132:  // Order price exceeded upper limit.
      return Error::INVALID_PRICE;
    case 12'133:  // Order price exceeded lower limit.
      return Error::INVALID_PRICE;
    case 12'134:  // Order price has too many decimals.
      return Error::INVALID_PRICE;
    case 12'135:  // Order quantity exceeded upper limit.
      return Error::INVALID_QUANTITY;
    case 12'136:  // Order quantity exceeded lower limit.
      return Error::INVALID_QUANTITY;
    case 12'137:  // Order quantity has too many decimals.
      return Error::INVALID_QUANTITY;
    case 12'138:  // Order price exceeded limits.
      return Error::INVALID_PRICE;
    case 12'139:  // Order has been filled.
      return Error::TOO_LATE_TO_MODIFY_OR_CANCEL;
    case 12'140:  // Order value exceeded lower limit.
      break;
    case 12'141:  // Duplicate clientOrderId.
      return Error::INVALID_REQUEST_ID;
    case 12'142:  // Order has been canceled.
      return Error::TOO_LATE_TO_MODIFY_OR_CANCEL;
    case 12'143:  // Order not found.
      return Error::UNKNOWN_ORDER_ID;
    case 12'144:  // Order being cancelled. Operation not supported.
      return Error::TOO_LATE_TO_MODIFY_OR_CANCEL;
    case 12'145:  // Order cannot be canceled.
      return Error::TOO_LATE_TO_MODIFY_OR_CANCEL;
    case 12'146:  // Order creation timeout.
      break;
    case 12'147:  // Order cancellation timeout.
      break;
    case 12'148:  // Market order amount decimal too long
      return Error::INVALID_QUANTITY;
    case 12'149:  // Create order failed
      break;
    case 12'150:  // Cancel order failed
      break;
    case 12'151:  // The trading pair is not open yet
      break;
    case 12'156:  // Order quantity invalid
      return Error::INVALID_QUANTITY;
    case 12'157:  // The trading pair is not available for api trading
      break;
    case 12'158:  // create limit maker order failed
      break;
    case 12'159:  // Market Order is not supported within the first %s minutes of newly launched pairs due to risk
                  // control.
      break;
    case 12'190:  // Cancel order has been finished.
      break;
    case 12'191:  // Can not cancel order, please try again later.
      break;
    case 12'192:  // Order price cannot be higher than %s .
      return Error::INVALID_PRICE;
    case 12'193:  // Buy order price cannot be higher than %s.
      return Error::INVALID_PRICE;
    case 12'194:  // Sell order price cannot be lower than %s.
      return Error::INVALID_PRICE;
    case 12'195:  // Please note that your order may not be filled
      break;
    case 12'196:  // Please note that your order may not be filled
      break;
    case 12'197:  // Your order quantity to buy is too large. The filled price may deviate significantly from the market
                  // price. Please try again
      return Error::INVALID_QUANTITY;
      break;
    case 12'198:  // Your order quantity to sell is too large. The filled price may deviate significantly from the market
                  // price. Please try again
      return Error::INVALID_QUANTITY;
    case 12'199:  // Your order quantity to buy is too large. The filled price may deviate significantly from the nav.
                  // Please try again.
      return Error::INVALID_QUANTITY;
    case 12'200:  // Your order quantity to sell is too large. The filled price may deviate significantly from the nav.
                  // Please try again.
      return Error::INVALID_QUANTITY;
    case 12'201:  // Invalid orderCategory parameter
      break;
    case 12'202:  // Please enter the TP/SL price
      break;
    case 12'203:  // trigger price cannot be higher than 110% price
      return Error::INVALID_PRICE;
    case 12'204:  // trigger price cannot be lower than 90% of qty
      return Error::INVALID_PRICE;
    case 12'205:  // CROSS_MARGIN_USER_NOT_ALLOW
      break;
    case 12'206:  // Stop_limit Order is not supported within the first 5 minutes of newly launched pairs
      break;
    case 12'210:  // New order rejected.
      break;
    case 12'211:  // Cancelation rejected.
      break;
    case 12'213:  // Order does not exist.
      break;
    case 12'217:  // Only LIMIT-MAKER order is supported for the current pair.
      break;
    case 12'218:  // The LIMIT-MAKER order is rejected due to invalid price.
      break;
    case 12'221:  // This coin does not exist
      return Error::INVALID_SYMBOL;
    case 12'222:  // Too many requests in this time frame
      break;
    case 12'223:  // Your Spot Account with Institutional Lending triggers an alert or liquidation
      break;
    case 12'224:  // You're not a user of the Innovation Zone
      break;
    case 12'225:  // You've failed to check the Risk Alert for Leveraged Tokens
      break;
    case 12'226:  // Your Spot Account for Margin Trading is being liquidated
      break;
    case 12'227:  // This feature is not supported
      break;
    case 12'228:  // The purchase amount of each order exceeds the estimated maximum purchase amount
      return Error::INVALID_QUANTITY;
    case 12'229:  // The sell quantity per order exceeds the estimated maximum sell quantity
      return Error::INVALID_QUANTITY;
    case 12'230:  // The reserved quota for Spot Margin Trading does not meet the LTV ratio requirement
      break;
    case 12'231:  // Users with Spot Margin Trading are not allowed to perform Block Trades
      break;
    case 12'232:  // Users with Institutional Lending are not allowed to perform Block Trades
      break;
    case 12'233:  // Users with Leverage Tokens are not allowed to perform Block Trades
      break;
    case 12'234:  // System Error
      break;
    case 12'400:  // The serialNum is already in use.
      break;
    case 12'401:  // Daily purchase limit has been exceeded. Please try again later.
      break;
    case 12'402:  // There's a large number of purchase orders. Please try again later.
      break;
    case 12'403:  // Insufficient available balance. Please make a deposit and try again.
      return Error::INSUFFICIENT_FUNDS;
    case 12'404:  // Daily redemption limit has been exceeded. Please try again later.
      break;
    case 12'405:  // There's a large number of redemption orders. Please try again later.
      break;
    case 12'406:  // Insufficient available balance. Please make a deposit and try again.
      return Error::INSUFFICIENT_FUNDS;
    case 12'407:  // Order not found.
      return Error::UNKNOWN_ORDER_ID;
    case 12'408:  // Purchase period hasn't started yet.
      break;
    case 12'409:  // Purchase amount has exceeded the upper limit.
      break;
    case 12'410:  // You haven't passed the quiz yet! To purchase and/or redeem an LT, please complete the quiz first.
      break;
    case 12'412:  // Redemption period hasn't started yet.
      break;
    case 12'413:  // Redemption amount has exceeded the upper limit.
      break;
    case 12'414:  // Purchase of the LT has been temporarily suspended.
      break;
    case 12'415:  // Redemption of the LT has been temporarily suspended.
      break;
    case 12'416:  // Invalid format. Please check the length and numeric precision.
      return Error::INVALID_REQUEST_ARGS;
    case 12'417:  // Failed to place order：Exceed the maximum position limit of leveraged tokens
      break;
    case 12'601:  // Query user repay history error
      break;
    case 12'602:  // Query user account info error
      break;
    case 12'603:  // Query user loan history error
      break;
    case 12'604:  // Query order history start time exceeds end time
      break;
    case 12'605:  // Failed to borrow
      break;
    case 12'606:  // Repayment Failed
      break;
    case 12'607:  // User not found
      break;
    case 12'608:  // You haven't enabled Cross Margin Trading yet. To do so
      break;
    case 12'609:  // You haven't enabled Cross Margin Trading yet. To do so
      break;
    case 12'610:  // Failed to locate the coins to borrow
      break;
    case 12'611:  // Cross Margin Trading not yet supported by the selected coin
      break;
    case 12'612:  // Pair not available
      break;
    case 12'613:  // Cross Margin Trading not yet supported by the selected pair
      break;
    case 12'614:  // Repeated repayment requests
      break;
    case 12'615:  // Insufficient available balance
      break;
    case 12'616:  // No repayment required
      break;
    case 12'617:  // Repayment amount has exceeded the total liability
      break;
    case 12'618:  // Settlement in progress
      break;
    case 12'619:  // Liquidation in progress
      break;
    case 12'620:  // Failed to locate repayment history
      break;
    case 12'621:  // Repeated borrowing requests
      break;
    case 12'622:  // Coins to borrow not generally available yet
      break;
    case 12'623:  // Pair to borrow not generally available yet
      break;
    case 12'624:  // Invalid user status
      break;
    case 12'625:  // Amount to borrow cannot be lower than the min. amount to borrow (per transaction)
      break;
    case 12'626:  // Amount to borrow cannot be larger than the max. amount to borrow (per transaction)
      break;
    case 12'627:  // Amount to borrow cannot be higher than the max. amount to borrow per user
      break;
    case 12'628:  // Amount to borrow has exceeded Bybit's max. amount to borrow
      break;
    case 12'629:  // Amount to borrow has exceeded the user's estimated max. amount to borrow
      break;
    case 12'630:  // Query user loan info error
      break;
    case 12'631:  // Decimal of decimals has exceeded the maximum precision
      break;
    case 12'632:  // Decimal of decimals has exceeded the maximum precision
      break;
    case 33'004:  // api_key expire
      break;
    case 110'001:  // Order does not exist
      return Error::TOO_LATE_TO_MODIFY_OR_CANCEL;
    case 110'007:  // Available balance is insufficient
      return Error::INSUFFICIENT_FUNDS;
    case 110'123:  // You must agree to the Trading Terms
      return Error::UNKNOWN;
  }
  LOG_ONCE("error_code"sv, value);
  return Error::UNKNOWN;
}

static_assert(Helper{int32_t{0}} == roq::Error::UNDEFINED);

template <>
template <>
std::optional<roq::Error> Map<int32_t>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::EventType ==> roq::UpdateType

template <>
template <>
constexpr Helper<bybit::protocol::json::EventType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::EventType::type_t;
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

static_assert(Helper{bybit::protocol::json::EventType{bybit::protocol::json::EventType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{bybit::protocol::json::EventType{bybit::protocol::json::EventType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{bybit::protocol::json::EventType{bybit::protocol::json::EventType::DELTA}} == roq::UpdateType::INCREMENTAL);
static_assert(Helper{bybit::protocol::json::EventType{bybit::protocol::json::EventType::COMMAND_RESP}} == roq::UpdateType::UNDEFINED);

template <>
template <>
std::optional<roq::UpdateType> Map<bybit::protocol::json::EventType>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::OptionsType ==> roq::OptionType

template <>
template <>
constexpr Helper<bybit::protocol::json::OptionsType>::operator std::optional<roq::OptionType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::OptionsType::type_t;
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

static_assert(Helper{bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::OptionType::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::CALL}} == roq::OptionType::CALL);
static_assert(Helper{bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::PUT}} == roq::OptionType::PUT);

template <>
template <>
std::optional<roq::OptionType> Map<bybit::protocol::json::OptionsType>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::OrderStatus ==> roq::OrderStatus

template <>
template <>
constexpr Helper<bybit::protocol::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::OrderStatus::type_t;
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

static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::CREATED}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::REJECTED}} == roq::OrderStatus::REJECTED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::PARTIALLY_FILLED_CANCELED}} == roq::OrderStatus::CANCELED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::CANCELLED}} == roq::OrderStatus::CANCELED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::UNTRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::TRIGGERED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::DEACTIVATED}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OrderStatus{bybit::protocol::json::OrderStatus::ACTIVE}} == roq::OrderStatus::UNDEFINED);

template <>
template <>
std::optional<roq::OrderStatus> Map<bybit::protocol::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::OrderType ==> roq::OrderType

template <>
template <>
constexpr Helper<bybit::protocol::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::OrderType::type_t;
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
    case BLOCK_TRADE_LIMIT:
      return roq::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);

template <>
template <>
std::optional<roq::OrderType> Map<bybit::protocol::json::OrderType>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::Side ==> roq::Side

template <>
template <>
constexpr Helper<bybit::protocol::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::Side::type_t;
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

static_assert(Helper{bybit::protocol::json::Side{bybit::protocol::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Side{bybit::protocol::json::Side::NONE}} == roq::Side::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Side{bybit::protocol::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{bybit::protocol::json::Side{bybit::protocol::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<bybit::protocol::json::Side>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::Status ==> roq::TradingStatus

template <>
template <>
constexpr Helper<bybit::protocol::json::Status>::operator std::optional<roq::TradingStatus>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::Status::type_t;
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

static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::UNDEFINED_INTERNAL}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::PRE_LAUNCH}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::TRADING}} == roq::TradingStatus::OPEN);
static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::SETTLING}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::DELIVERING}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{bybit::protocol::json::Status{bybit::protocol::json::Status::CLOSED}} == roq::TradingStatus::CLOSE);

template <>
template <>
std::optional<roq::TradingStatus> Map<bybit::protocol::json::Status>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::TimeInForce ==> roq::TimeInForce

template <>
template <>
constexpr Helper<bybit::protocol::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::TimeInForce::type_t;
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
    case POST_ONLY:
      return roq::TimeInForce::GTC;
  }
  return {};
}

static_assert(Helper{bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::FOK}} == roq::TimeInForce::FOK);
static_assert(Helper{bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);
static_assert(Helper{bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::POST_ONLY}} == roq::TimeInForce::GTC);

template <>
template <>
std::optional<roq::TimeInForce> Map<bybit::protocol::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// bybit::protocol::json::{ ContractType, OptionsType } ==> roq::SecurityType

template <>
template <>
constexpr Helper<bybit::protocol::json::ContractType, bybit::protocol::json::OptionsType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<1>(args_)) {
    using enum bybit::protocol::json::OptionsType::type_t;
    case UNDEFINED_INTERNAL:
    case UNKNOWN_INTERNAL:
      break;
    case CALL:
    case PUT:
      return SecurityType::OPTION;
  }
  switch (std::get<0>(args_)) {
    using enum bybit::protocol::json::ContractType::type_t;
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
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::UNDEFINED_INTERNAL},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::SecurityType::SPOT);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::UNDEFINED_INTERNAL},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::CALL}} == roq::SecurityType::OPTION);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::UNDEFINED_INTERNAL},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::PUT}} == roq::SecurityType::OPTION);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::INVERSE_PERPETUAL},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::SecurityType::SWAP);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::LINEAR_PERPETUAL},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::SecurityType::SWAP);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::LINEAR_FUTURES},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::SecurityType::FUTURES);
static_assert(
    Helper{
        bybit::protocol::json::ContractType{bybit::protocol::json::ContractType::INVERSE_FUTURES},
        bybit::protocol::json::OptionsType{bybit::protocol::json::OptionsType::UNDEFINED_INTERNAL}} == roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<bybit::protocol::json::ContractType, bybit::protocol::json::OptionsType>::helper() const {
  return Helper{args_};
}

// roq ==>

// roq::OrderType ==> bybit::protocol::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<bybit::protocol::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return bybit::protocol::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return bybit::protocol::json::OrderType::MARKET;
    case LIMIT:
      return bybit::protocol::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == bybit::protocol::json::OrderType{bybit::protocol::json::OrderType::LIMIT});

template <>
template <>
std::optional<bybit::protocol::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// roq::Side ==> bybit::protocol::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<bybit::protocol::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return bybit::protocol::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return bybit::protocol::json::Side::BUY;
    case SELL:
      return bybit::protocol::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == bybit::protocol::json::Side{bybit::protocol::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == bybit::protocol::json::Side{bybit::protocol::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == bybit::protocol::json::Side{bybit::protocol::json::Side::SELL});

template <>
template <>
std::optional<bybit::protocol::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// {roq::TimeInForce, Mask<ExecutionInstruction>} ==> bybit::protocol::json::TimeInForce

template <>
template <>
constexpr Helper<roq::TimeInForce, Mask<ExecutionInstruction>>::operator std::optional<bybit::protocol::json::TimeInForce>() const {
  auto &[time_in_force, execution_instructions] = args_;
  auto participate_do_not_initiate = execution_instructions.has(ExecutionInstruction::PARTICIPATE_DO_NOT_INITIATE);
  switch (time_in_force) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      if (participate_do_not_initiate) {
        return bybit::protocol::json::TimeInForce::POST_ONLY;
      } else {
        return bybit::protocol::json::TimeInForce::GTC;
      }
    case OPG:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return bybit::protocol::json::TimeInForce::IOC;
    case FOK:
      return bybit::protocol::json::TimeInForce::FOK;
    case GTX:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(
    Helper{roq::TimeInForce::UNDEFINED, Mask<ExecutionInstruction>{}} ==
    bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC, Mask<ExecutionInstruction>{}} == bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::IOC, Mask<ExecutionInstruction>{}} == bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK, Mask<ExecutionInstruction>{}} == bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::FOK});
static_assert(
    Helper{roq::TimeInForce::GTC, Mask{ExecutionInstruction::PARTICIPATE_DO_NOT_INITIATE}} ==
    bybit::protocol::json::TimeInForce{bybit::protocol::json::TimeInForce::POST_ONLY});

template <>
template <>
std::optional<bybit::protocol::json::TimeInForce> Map<roq::TimeInForce, Mask<ExecutionInstruction>>::helper() const {
  return Helper{args_};
}

}  // namespace roq
