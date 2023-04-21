/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/gateway.hpp"

#include <algorithm>
#include <cctype>
#include <limits>

#include "roq/logging.hpp"

#include "roq/clock.hpp"
#include "roq/core/charconv.hpp"
#include "roq/core/utils.hpp"

#include "roq/bybit/flags.hpp"

#include "roq/bybit/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === HELPERS ===

namespace {
template <typename R>
auto create_authenticator(auto const &config) {
  R result;
  for (auto &[_, account] : config.accounts)
    result.try_emplace(account.name, std::make_unique<Authenticator>(config, account.name));
  return result;
}

template <typename R>
auto create_order_entry(auto &gateway, auto &context, auto &stream_id, auto &authenticator_by_account, auto &shared) {
  R result;
  for (auto &[account, authenticator] : authenticator_by_account)
    result.try_emplace(account, std::make_unique<OrderEntry>(gateway, context, ++stream_id, *authenticator, shared));
  return result;
}

template <typename R>
auto create_drop_copy(auto &gateway, auto &context, auto &stream_id, auto &authenticator_by_account, auto &shared) {
  R result;
  for (auto &[account, authenticator] : authenticator_by_account)
    result.try_emplace(account, std::make_unique<DropCopy>(gateway, context, ++stream_id, *authenticator, shared));
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

Gateway::Gateway(server::Dispatcher &dispatcher, Config const &config, io::Context &context)
    : dispatcher_{dispatcher}, master_account_{config.get_master_account()},
      authenticator_{create_authenticator<decltype(authenticator_)>(config)}, context_{context}, shared_{dispatcher},
      rest_{*this, context_, ++stream_id_, shared_},
      order_entry_{create_order_entry<decltype(order_entry_)>(*this, context_, stream_id_, authenticator_, shared_)},
      drop_copy_{create_drop_copy<decltype(drop_copy_)>(*this, context_, stream_id_, authenticator_, shared_)} {
}

void Gateway::operator()(Event<Start> const &event) {
  log::info("Starting..."sv);
  rest_(event);
  for (auto &[_, order_entry] : order_entry_)
    (*order_entry)(event);
  for (auto &[_, drop_copy] : drop_copy_)
    (*drop_copy)(event);
  assert(std::empty(market_data_spot_));
  assert(std::empty(market_data_linear_));
  assert(std::empty(market_data_inverse_));
  assert(std::empty(market_data_option_));
}

void Gateway::operator()(Event<Stop> const &event) {
  log::info("Stopping..."sv);
  for (auto &iter : market_data_spot_)
    (*iter)(event);
  for (auto &iter : market_data_linear_)
    (*iter)(event);
  for (auto &iter : market_data_inverse_)
    (*iter)(event);
  for (auto &iter : market_data_option_)
    (*iter)(event);
  for (auto &[_, drop_copy] : drop_copy_)
    (*drop_copy)(event);
  for (auto &[_, order_entry] : order_entry_)
    (*order_entry)(event);
  rest_(event);
}

void Gateway::operator()(Event<Timer> const &event) {
  rest_(event);
  for (auto &[_, order_entry] : order_entry_)
    (*order_entry)(event);
  for (auto &[_, drop_copy] : drop_copy_)
    (*drop_copy)(event);
  for (auto &iter : market_data_spot_)
    (*iter)(event);
  for (auto &iter : market_data_linear_)
    (*iter)(event);
  for (auto &iter : market_data_inverse_)
    (*iter)(event);
  for (auto &iter : market_data_option_)
    (*iter)(event);
}

void Gateway::operator()(Event<Connected> const &) {
}

void Gateway::operator()(Event<Disconnected> const &event) {
  auto const &[message_info, disconnected] = event;
  log::warn(
      R"(Disconnected: source="{}", order_cancel_policy={})"sv,
      message_info.source_name,
      disconnected.order_cancel_policy);
  switch (disconnected.order_cancel_policy) {
    using enum OrderCancelPolicy;
    case UNDEFINED:
      break;
    case MANAGED_ORDERS:
      log::warn("*** CANCEL MANAGED ORDERS NOT IMPLEMENTED ***"sv);
      break;
    case BY_ACCOUNT:
      log::warn("*** CANCEL ALL ACCOUNT ORDERS ***"sv);
      for (auto &[account, order_entry] : order_entry_) {
        if (dispatcher_.can_user_trade_account(account, message_info.source)) {
          log::warn(R"(- account="{}")"sv, account);
          CancelAllOrders cancel_all_orders{
              .account = account,
          };
          Event event{message_info, cancel_all_orders};
          (*order_entry)(event, {});
        }
      }
  }
}

void Gateway::operator()(Trace<StreamStatus> const &event) {
  dispatcher_(event);
}

void Gateway::operator()(Trace<ExternalLatency> const &event) {
  dispatcher_(event);
}

void Gateway::operator()(Trace<ReferenceData> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Trace<MarketStatus> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Trace<TopOfBook> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Trace<MarketByPriceUpdate> const &event, bool is_last) {
  auto callback = []([[maybe_unused]] auto &market_by_price) {};
  dispatcher_(event, is_last, bids_, asks_, callback);
}

void Gateway::operator()(Trace<TradeSummary> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Trace<StatisticsUpdate> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Trace<oms::TradeUpdate> const &event, uint16_t stream_id, bool is_last, uint8_t user_id) {
  dispatcher_(event, stream_id, is_last, user_id);
}

void Gateway::operator()(Trace<FundsUpdate> const &event, bool is_last) {
  dispatcher_(event, is_last);
}

void Gateway::operator()(Rest::SymbolsUpdate &symbols_update) {
  auto process = [&](auto &market_data, auto &symbols, auto &uri, auto mbp_depth) {
    auto [size, start_from] = symbols(symbols_update.symbols);
    ensure_symbol_slices(market_data, size, symbols, uri, mbp_depth);
    for (auto &iter : market_data)
      (*iter).subscribe(start_from);
  };
  switch (symbols_update.category) {
    using enum json::Category::type_t;
    case UNDEFINED:
    case UNKNOWN:
      log::fatal("Unexpected"sv);
      break;
    case SPOT:
      process(market_data_spot_, shared_.symbols.spot, Flags::ws_public_uri_spot(), Flags::ws_mbp_depth_spot());
      break;
    case LINEAR:
      process(market_data_linear_, shared_.symbols.linear, Flags::ws_public_uri_linear(), Flags::ws_mbp_depth_linear());
      break;
    case INVERSE:
      process(
          market_data_inverse_, shared_.symbols.inverse, Flags::ws_public_uri_inverse(), Flags::ws_mbp_depth_inverse());
      break;
    case OPTION:
      process(market_data_option_, shared_.symbols.option, Flags::ws_public_uri_option(), Flags::ws_mbp_depth_option());
      break;
  }
}

void Gateway::ensure_symbol_slices(auto &market_data, size_t size, auto &symbols, auto &uri, size_t mbp_depth) {
  while (std::size(market_data) < size) {
    log::debug("Create market-data (user-stream)"sv);
    auto item = std::make_unique<MarketData>(
        *this, context_, ++stream_id_, shared_, symbols, std::size(market_data), uri, mbp_depth);
    MessageInfo message_info;
    Start start;
    create_event_and_dispatch(*item, message_info, start);
    market_data.emplace_back(std::move(item));
  }
}

uint16_t Gateway::operator()(
    Event<CreateOrder> const &event, oms::Order const &order, std::string_view const &request_id) {
  assert(!std::empty(event.value.account));
  return get_order_entry(event.value.account)(event, order, request_id);
}

uint16_t Gateway::operator()(
    Event<ModifyOrder> const &event,
    oms::Order const &order,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  assert(!std::empty(event.value.account));
  assert(event.value.account == order.account);
  return get_order_entry(event.value.account)(event, order, request_id, previous_request_id);
}

uint16_t Gateway::operator()(
    Event<CancelOrder> const &event,
    oms::Order const &order,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  assert(!std::empty(event.value.account));
  assert(event.value.account == order.account);
  return get_order_entry(event.value.account)(event, order, request_id, previous_request_id);
}

uint16_t Gateway::operator()(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  assert(!std::empty(event.value.account));
  return get_order_entry(event.value.account)(event, request_id);
}

void Gateway::operator()(metrics::Writer &writer) {
  for (auto &[_, order_entry] : order_entry_)
    (*order_entry)(writer);
  for (auto &[_, drop_copy] : drop_copy_)
    if (static_cast<bool>(drop_copy))
      (*drop_copy)(writer);
  for (auto &iter : market_data_spot_)
    (*iter)(writer);
  for (auto &iter : market_data_linear_)
    (*iter)(writer);
  for (auto &iter : market_data_inverse_)
    (*iter)(writer);
  for (auto &iter : market_data_option_)
    (*iter)(writer);
}

OrderEntry &Gateway::get_order_entry(std::string_view const &account) {
  auto iter = order_entry_.find(account);
  if (iter != std::end(order_entry_))
    return *(*iter).second;
  throw RuntimeError{R"(Unknown account="{}")"sv, account};
}

}  // namespace bybit
}  // namespace roq
