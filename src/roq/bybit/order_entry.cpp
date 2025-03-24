/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bybit/order_entry.hpp"

#include <algorithm>
#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/common.hpp"
#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/bybit/json/map.hpp"
#include "roq/bybit/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
auto const NAME = "om"sv;
}  // namespace

// === HELPERS ===

namespace {
auto get_supports(auto api) {
  auto result = Mask{
      SupportType::CREATE_ORDER,
      SupportType::CANCEL_ORDER,
      SupportType::ORDER_ACK,
      SupportType::FUNDS,
  };
  if (api != tools::API::SPOT)
    result |= SupportType::MODIFY_ORDER;
  return result;
}
}  // namespace

// === CONSTANTS ===

namespace {
auto create_name(auto stream_id, auto const &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.rest.uri;
  auto config = web::rest::Client::Config{
      // connection
      .interface = {},
      .proxy = settings.rest.proxy,
      .uris = {&uri, 1},
      .host = settings.rest.host,
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = {},
      .disconnect_on_idle_timeout = {},
      .connection = web::http::Connection::KEEP_ALIVE,
      // request
      .allow_pipelining = true,
      .request_timeout = settings.rest.request_timeout,
      // response
      .suspend_on_retry_after = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .ping_frequency = settings.rest.ping_freq,
      .ping_path = settings.rest.ping_path,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::rest::Client::create(handler, context, config);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory(settings.app.name, group, function) {}
};

auto get_download_trades_lookback(auto &settings, auto download_trades_is_first) {
  if (download_trades_is_first) {
    if (settings.download.trades_lookback_on_restart.count())
      return settings.download.trades_lookback_on_restart;
  }
  return settings.download.trades_lookback;
}
}  // namespace

// === IMPLEMENTATION ===

OrderEntry::OrderEntry(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account.name)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_(shared.settings.misc.decode_buffer_size),
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .account_info = create_metrics(shared.settings, name_, "account_info"sv),
          .account_info_ack = create_metrics(shared.settings, name_, "account_info_ack"sv),
          .wallet_balance = create_metrics(shared.settings, name_, "wallet_balance"sv),
          .wallet_balance_ack = create_metrics(shared.settings, name_, "wallet_balance_ack"sv),
          .position_info = create_metrics(shared.settings, name_, "position_info"sv),
          .position_info_ack = create_metrics(shared.settings, name_, "position_info_ack"sv),
          .open_orders = create_metrics(shared.settings, name_, "open_orders"sv),
          .open_orders_ack = create_metrics(shared.settings, name_, "open_orders_ack"sv),
          .execution = create_metrics(shared.settings, name_, "execution"sv),
          .execution_ack = create_metrics(shared.settings, name_, "execution_ack"sv),
          .place_order = create_metrics(shared.settings, name_, "place_order"sv),
          .place_order_ack = create_metrics(shared.settings, name_, "place_order_ack"sv),
          .amend_order = create_metrics(shared.settings, name_, "amend_order"sv),
          .amend_order_ack = create_metrics(shared.settings, name_, "amend_order_ack"sv),
          .cancel_order = create_metrics(shared.settings, name_, "cancel_order"sv),
          .cancel_order_ack = create_metrics(shared.settings, name_, "cancel_order_ack"sv),
          .cancel_all_orders = create_metrics(shared.settings, name_, "cancel_all_orders"sv),
          .cancel_all_orders_ack = create_metrics(shared.settings, name_, "cancel_all_orders_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared}, download_{shared.settings.rest.request_timeout, [this](auto state) { return download(state); }} {
}

void OrderEntry::operator()(Event<Start> const &) {
  (*connection_).start();
}

void OrderEntry::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void OrderEntry::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready())
    check_request_queue(now);
}

void OrderEntry::operator()(metrics::Writer &writer) {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.account_info, metrics::Type::PROFILE)
      .write(profile_.account_info_ack, metrics::Type::PROFILE)
      .write(profile_.wallet_balance, metrics::Type::PROFILE)
      .write(profile_.wallet_balance_ack, metrics::Type::PROFILE)
      .write(profile_.position_info, metrics::Type::PROFILE)
      .write(profile_.position_info_ack, metrics::Type::PROFILE)
      .write(profile_.open_orders, metrics::Type::PROFILE)
      .write(profile_.open_orders_ack, metrics::Type::PROFILE)
      .write(profile_.execution, metrics::Type::PROFILE)
      .write(profile_.execution_ack, metrics::Type::PROFILE)
      .write(profile_.place_order, metrics::Type::PROFILE)
      .write(profile_.place_order_ack, metrics::Type::PROFILE)
      .write(profile_.amend_order, metrics::Type::PROFILE)
      .write(profile_.amend_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_order, metrics::Type::PROFILE)
      .write(profile_.cancel_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

uint16_t OrderEntry::operator()(Event<CreateOrder> const &event, server::oms::Order const &order, std::string_view const &request_id) {
  place_order(event, order, request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<ModifyOrder> const &event, server::oms::Order const &order, std::string_view const &request_id, std::string_view const &previous_request_id) {
  amend_order(event, order, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<CancelOrder> const &event, server::oms::Order const &order, std::string_view const &request_id, std::string_view const &previous_request_id) {
  cancel_order(event, order, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  cancel_all_orders(event, request_id);
  return stream_id_;
}

void OrderEntry::operator()(Trace<web::rest::Client::Connected> const &) {
  if (download_.downloading()) {
    download_.bump();
  } else {
    (*this)(ConnectionStatus::DOWNLOADING);
    download_.begin();
  }
}

void OrderEntry::operator()(Trace<web::rest::Client::Disconnected> const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  if (!download_.downloading())
    download_.reset();
}

void OrderEntry::operator()(Trace<web::rest::Client::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void OrderEntry::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = account_.name,
        .supports = get_supports(shared_.api.api),
        .transport = Transport::TCP,
        .protocol = Protocol::HTTP,
        .encoding = {Encoding::JSON},
        .priority = Priority::PRIMARY,
        .connection_status = status_,
        .interface = (*connection_).get_interface(),
        .authority = (*connection_).get_current_authority(),
        .path = (*connection_).get_current_path(),
        .proxy = (*connection_).get_proxy(),
    };
    log::info("stream_status={}"sv, stream_status);
    create_trace_and_dispatch(handler_, trace_info, stream_status);
  }
}

uint32_t OrderEntry::download(OrderEntryState state) {
  switch (state) {
    using enum OrderEntryState;
    case UNDEFINED:
      assert(false);
      break;
    case ACCOUNT_INFO:
      get_account_info();
      return 1;
    case DONE:
      (*this)(ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

void OrderEntry::check_request_queue(std::chrono::nanoseconds now) {
  auto request = [&](auto &message) {
    auto &[topic, symbol] = message;
    if (topic.compare("wallet"sv) == 0) {
      get_wallet_balance();
    } else if (topic.compare("position"sv) == 0) {
      get_position_info(symbol);
    } else if (topic.compare("order"sv) == 0) {
      get_open_orders(symbol);
    } else if (topic.compare("execution"sv) == 0) {
      get_execution(symbol);
    }
  };
  if (account_.request_queue.dispatch(now, request)) {
    log::debug("HERE size={}"sv, std::size(account_.request_queue));
  }
}

void OrderEntry::get_account_info() {
  profile_.account_info([&]() {
    auto path = shared_.api.simple.account_info;
    auto headers = account_.create_headers(path, {}, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, sequence = download_.sequence()]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_account_info_ack(event, sequence);
    };
    (*connection_)("account-info"sv, request, callback);
  });
}

void OrderEntry::get_account_info_ack(Trace<web::rest::Response> const &event, [[maybe_unused]] uint32_t sequence) {
  auto constexpr const STATE = OrderEntryState::ACCOUNT_INFO;
  profile_.account_info_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::AccountInfo account_info{body, decode_buffer_};
      Trace event_2{event, account_info};
      (*this)(event_2);
      download_.check_relaxed(STATE);
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
      if (download_.downloading())
        download_.retry(STATE);
    };
    process_response(event, handle_success, handle_error);
  });
}

void OrderEntry::operator()(Trace<json::AccountInfo> const &event) {
  auto &[trace_info, account_info] = event;
  log::info<2>("account_info={}"sv, account_info);
  // XXX HANS maybe do something with unified account ???
}

void OrderEntry::get_wallet_balance() {
  profile_.wallet_balance([&]() {
    auto path = shared_.api.simple.account_wallet_balance;
    auto account_type = [&]() -> std::string_view {
      switch (shared_.api.api) {
        using enum tools::API;
        case UNDEFINED:
          break;
        case SPOT:
          return "UNIFIED"sv;
        case LINEAR:
          return "UNIFIED"sv;
        case INVERSE:
          return "UNIFIED"sv;
        case OPTION:
          return "UNIFIED"sv;
      }
      log::fatal("Unexpected"sv);
    }();
    auto query = fmt::format("?accountType={}"sv, account_type);
    auto headers = account_.create_headers(path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_wallet_balance_ack(event);
    };
    (*connection_)("account-wallet-balance"sv, request, callback);
  });
}

void OrderEntry::get_wallet_balance_ack(Trace<web::rest::Response> const &event) {
  profile_.wallet_balance_ack([&]() {
    if (event.value.status() == web::http::Status::NOT_FOUND) {
      return;
    }
    auto handle_success = [&](auto &body) {
      if (json::WalletParser::dispatch(*this, body, decode_buffer_, event)) {
      } else {
        log::warn(R"(Unexpected: message="{}")"sv, body);
      }
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
    };
    process_response(event, handle_success, handle_error);
    auto response = Response{
        .account = account_.name,
        .topic = "wallet"sv,
        .symbol = {},
    };
    create_trace_and_dispatch(handler_, event, response);
  });
}

void OrderEntry::operator()(Trace<json::Wallet> const &event) {
  auto &[trace_info, wallet] = event;
  log::info<2>("wallet={}"sv, wallet);
  for (auto &item : wallet.coin) {
    log::info<2>("item={}"sv, item);
    // XXX maybe margin mode is from account_type?
    auto funds_update = FundsUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .currency = item.coin,
        .margin_mode = {},
        .balance = item.wallet_balance,  // XXX item.free ???
        .hold = item.locked,
        .external_account = {},
        .update_type = UpdateType::SNAPSHOT,
        .exchange_time_utc = {},
        .sending_time_utc = {},  // XXX lost when flattened
    };
    create_trace_and_dispatch(handler_, trace_info, funds_update, true);
  }
}

void OrderEntry::get_position_info(std::string_view const &symbol) {
  profile_.position_info([&]() {
    assert(shared_.api.api != tools::API::SPOT);
    auto path = shared_.api.simple.position_list;
    auto category = [&]() -> std::string_view {
      switch (shared_.api.api) {
        using enum tools::API;
        case UNDEFINED:
          break;
        case SPOT:
          break;
        case LINEAR:
          return "linear"sv;
        case INVERSE:
          return "inverse"sv;
        case OPTION:
          return "option"sv;
      }
      log::fatal("Unexpected"sv);
    }();
    auto query = fmt::format("?category={}&symbol={}&limit=200"sv, category, symbol);
    auto headers = account_.create_headers(path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, symbol = std::string{symbol}]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_position_info_ack(event, symbol);
    };
    (*connection_)("position-list"sv, request, callback);
  });
}

void OrderEntry::get_position_info_ack(Trace<web::rest::Response> const &event, std::string_view const &symbol) {
  profile_.position_info_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::PositionInfo position_info{body, decode_buffer_};
      Trace event_2{event, position_info};
      (*this)(event_2);
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
    };
    process_response(event, handle_success, handle_error);
    auto response = Response{
        .account = account_.name,
        .topic = "position"sv,
        .symbol = symbol,
    };
    create_trace_and_dispatch(handler_, event, response);
  });
}

void OrderEntry::operator()(Trace<json::PositionInfo> const &event) {
  auto &[trace_info, position_info] = event;
  log::info<2>("position_info={}"sv, position_info);
  for (auto &item : position_info.result.list) {
    log::info<2>("item={}"sv, item);
    if (shared_.discard_symbol(item.symbol))
      continue;
    auto margin_mode = item.trade_mode == 0 ? MarginMode::CROSS : MarginMode::ISOLATED;
    Side side = json::Map{item.side};
    auto quantity = utils::sign(side) * item.size;
    auto long_quantity = std::max(0.0, quantity);
    auto short_quantity = std::max(0.0, -quantity);
    auto position_update = PositionUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .margin_mode = margin_mode,
        .external_account = {},
        .long_quantity = long_quantity,
        .short_quantity = short_quantity,
        .update_type = UpdateType::SNAPSHOT,
        .exchange_time_utc = item.updated_time,  // XXX created_time ???
        .sending_time_utc = position_info.time,
    };
    create_trace_and_dispatch(handler_, trace_info, position_update, true);
  }
}

void OrderEntry::get_open_orders(std::string_view const &symbol) {
  profile_.open_orders([&]() {
    auto path = shared_.api.simple.order_realtime;
    auto category = [&]() -> std::string_view {
      switch (shared_.api.api) {
        using enum tools::API;
        case UNDEFINED:
          break;
        case SPOT:
          return "spot"sv;
        case LINEAR:
          return "linear"sv;
        case INVERSE:
          return "inverse"sv;
        case OPTION:
          return "option"sv;
      }
      log::fatal("Unexpected"sv);
    }();
    auto query = fmt::format("?category={}&symbol={}&openOnly=0&limit=50"sv, category, symbol);
    auto headers = account_.create_headers(path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, symbol = std::string{symbol}]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_open_orders_ack(event, symbol);
    };
    (*connection_)("order-realtime"sv, request, callback);
  });
}

void OrderEntry::get_open_orders_ack(Trace<web::rest::Response> const &event, std::string_view const &symbol) {
  profile_.open_orders_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::OpenOrders open_orders{body, decode_buffer_};
      Trace event_2{event, open_orders};
      (*this)(event_2);
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
    };
    process_response(event, handle_success, handle_error);
    auto response = Response{
        .account = account_.name,
        .topic = "order"sv,
        .symbol = symbol,
    };
    create_trace_and_dispatch(handler_, event, response);
  });
}

void OrderEntry::operator()(Trace<json::OpenOrders> const &event) {
  auto &[trace_info, open_orders] = event;
  log::info<2>("open_orders={}"sv, open_orders);
  for (auto &item : open_orders.result.list) {
    log::info<2>("item={}"sv, item);
    auto order_update = server::oms::OrderUpdate{
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = json::Map{item.side},
        .position_effect = {},
        .margin_mode = {},
        .max_show_quantity = NaN,
        .order_type = json::Map{item.order_type},
        .time_in_force = json::Map{item.time_in_force},
        .execution_instructions = {},
        .create_time_utc = item.created_time,
        .update_time_utc = item.updated_time,
        .external_account = {},
        .external_order_id = item.order_id,
        .client_order_id = {},
        .order_status = json::Map{item.order_status},
        .quantity = item.qty,
        .price = item.price,
        .stop_price = NaN,  // XXX item.trigger_price ???
        .remaining_quantity = item.leaves_qty,
        .traded_quantity = item.cum_exec_qty,
        .average_traded_price = item.avg_price,
        .last_traded_quantity = NaN,
        .last_traded_price = NaN,
        .last_liquidity = {},
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = UpdateType::SNAPSHOT,
        .sending_time_utc = open_orders.time,
    };
    Trace event_2{trace_info, order_update};
    (*this)(event_2, item.order_link_id);
  }
}

void OrderEntry::get_execution(std::string_view const &symbol) {
  profile_.execution([&]() {
    auto path = shared_.api.simple.execution_list;
    auto category = [&]() -> std::string_view {
      switch (shared_.api.api) {
        using enum tools::API;
        case UNDEFINED:
          break;
        case SPOT:
          return "spot"sv;
        case LINEAR:
          return "linear"sv;
        case INVERSE:
          return "inverse"sv;
        case OPTION:
          return "option"sv;
      }
      log::fatal("Unexpected"sv);
    }();
    auto lookback = get_download_trades_lookback(shared_.settings, download_trades_is_first_);
    log::info<1>("Download trades: lookback={}"sv, lookback);
    auto end_time = clock::get_realtime() + 1min;  // note! make sure we don't miss anything
    auto start_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - lookback);
    auto query = fmt::format(
        "?category={}&symbol={}&startTime={}&execType=Trade&limit={}"sv, category, symbol, start_time.count(), shared_.settings.download.trades_limit);
    auto headers = account_.create_headers(path, query, {});
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, symbol = std::string{symbol}]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_execution_ack(event, symbol);
    };
    (*connection_)("execution"sv, request, callback);
  });
}

void OrderEntry::get_execution_ack(Trace<web::rest::Response> const &event, std::string_view const &symbol) {
  profile_.execution_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::Execution execution{body, decode_buffer_};
      Trace event_2{event, execution};
      (*this)(event_2);
      download_trades_is_first_ = false;
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
    };
    process_response(event, handle_success, handle_error);
    auto response = Response{
        .account = account_.name,
        .topic = "execution"sv,
        .symbol = symbol,
    };
    create_trace_and_dispatch(handler_, event, response);
  });
}

void OrderEntry::operator()(Trace<json::Execution> const &event) {
  auto &trace_info = event.trace_info;
  auto &execution = event.value;
  log::info<2>("execution={}"sv, execution);
  std::string_view order_id, order_link_id, symbol;
  Side side = {};
  std::chrono::nanoseconds exec_time = {};
  auto dispatch = [&]() {
    if (std::empty(shared_.fills))
      return;
    auto trade_update = TradeUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .order_id = {},
        .exchange = shared_.settings.exchange,
        .symbol = symbol,
        .side = side,
        .position_effect = {},
        .margin_mode = {},
        .quantity_type = {},
        .create_time_utc = utils::safe_cast(exec_time),
        .update_time_utc = utils::safe_cast(exec_time),
        .external_account = {},
        .external_order_id = order_id,
        .client_order_id = {},
        .fills = shared_.fills,
        .routing_id = {},
        .update_type = UpdateType::SNAPSHOT,
        .sending_time_utc = execution.time,
        .user = {},
        .strategy_id = {},
    };
    create_trace_and_dispatch(handler_, trace_info, trade_update, true, SOURCE_NONE, order_link_id);
    shared_.fills.clear();
  };
  for (auto &item : execution.result.list) {
    log::info<2>("item={}"sv, item);
    /* XXX doesn't work with spot
    if (item.exec_type != json::ExecType::TRADE)  // note!
      continue;
    */
    if (item.order_id != order_id) {
      dispatch();
      order_id = item.order_id;
      order_link_id = item.order_link_id;
      symbol = item.symbol;
      side = json::Map{item.side};
      exec_time = item.exec_time;
    }
    auto liquidity = item.is_maker ? Liquidity::MAKER : Liquidity::TAKER;
    auto fill = Fill{
        .exchange_time_utc = item.exec_time,
        .external_trade_id = item.exec_id,
        .quantity = item.exec_qty,
        .price = item.exec_price,
        .liquidity = liquidity,
        .quote_quantity = NaN,
        .commission_quantity = item.exec_fee,  // XXX ???
        .commission_currency = {},
    };
    shared_.fills.emplace_back(std::move(fill));
  }
  dispatch();
}

void OrderEntry::place_order(Event<CreateOrder> const &event, server::oms::Order const &order, std::string_view const &request_id) {
  profile_.place_order([&]() {
    if (!ready())
      throw server::oms::NotReady{"not ready"sv};
    auto &[message_info, create_order] = event;
    auto path = shared_.api.simple.order_create;
    std::string buffer;  // XXX
    auto body = json::place_order(buffer, create_order, order, request_id, shared_.api.category);
    auto headers = account_.create_headers(path, {}, body);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = create_order.order_id]([[maybe_unused]] auto &request_id, auto &response) {
      uint32_t version = 1;
      TraceInfo trace_info;
      Trace event{trace_info, response};
      place_order_ack(event, user_id, order_id, version);
    };
    (*connection_)("order-create"sv, request, callback);
  });
}

void OrderEntry::place_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.place_order_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::PlaceOrder place_order{body, decode_buffer_};
      Trace event_2{event, place_order};
      (*this)(event_2, user_id, order_id, version);
    };
    auto handle_error = [&](auto origin, auto status, auto error, auto text) {
      auto response = server::oms::Response{
          .request_type = RequestType::CREATE_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    process_response(event, handle_success, handle_error);
  });
}

void OrderEntry::operator()(Trace<json::PlaceOrder> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, place_order] = event;
  log::info<2>("place_order={}"sv, place_order);
  auto request_status = place_order.ret_code == 0 ? RequestStatus::ACCEPTED : RequestStatus::REJECTED;
  auto error = json::map_error(place_order.ret_code);
  auto text = place_order.ret_msg;
  auto &result = place_order.result;
  auto response = server::oms::Response{
      .request_type = RequestType::CREATE_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = request_status,
      .error = error,
      .text = text,
      .version = version,
      .request_id = result.order_link_id,
      .quantity = NaN,
      .price = NaN,
  };
  /*
  // note! ACCEPTED not managed by fix-bridge
  auto order_status = place_order.ret_code == 0 ? OrderStatus::ACCEPTED : OrderStatus::REJECTED;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = {},
      .side = {},
      .position_effect = {},
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = {},
      .time_in_force = {},
      .execution_instructions = {},
      .create_time_utc = {},
      .update_time_utc = place_order.time,
      .external_account = {},
      .external_order_id = result.order_id,
      .client_order_id = {},
      .order_status = order_status,
      .quantity = NaN,
      .price = NaN,
      .stop_price = NaN,
      .remaining_quantity = NaN,
      .traded_quantity = NaN,
      .average_traded_price = NaN,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = place_order.time,
  };
  */
  Trace event_2{trace_info, response};
  (*this)(event_2, user_id, order_id);
}

void OrderEntry::amend_order(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    [[maybe_unused]] std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  profile_.amend_order([&]() {
    if (shared_.api.api == tools::API::SPOT)
      throw server::oms::NotSupported{"amend_order"sv};
    if (!ready())
      throw server::oms::NotReady{"not ready"sv};
    auto &[message_info, modify_order] = event;
    auto path = shared_.api.simple.order_amend;
    std::string buffer;  // XXX
    auto body = json::amend_order(buffer, modify_order, order, request_id, previous_request_id, shared_.api.category);
    auto headers = account_.create_headers(path, {}, body);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = modify_order.order_id, version = modify_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      amend_order_ack(event, user_id, order_id, version);
    };
    (*connection_)("order-amend"sv, request, callback);
  });
}

void OrderEntry::amend_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.amend_order_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::AmendOrder amend_order{body, decode_buffer_};
      Trace event_2{event, amend_order};
      (*this)(event_2, user_id, order_id, version);
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
      auto response = server::oms::Response{
          .request_type = RequestType::MODIFY_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    process_response(event, handle_success, handle_error);
  });
}

// XXX this is a little weird -- the response tells us the last known (?) status of the order
void OrderEntry::operator()(Trace<json::AmendOrder> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, amend_order] = event;
  log::info<2>("amend_order={}"sv, amend_order);
  auto status = amend_order.ret_code == 0 ? RequestStatus::ACCEPTED : RequestStatus::REJECTED;
  auto error = json::map_error(amend_order.ret_code);
  auto text = amend_order.ret_msg;
  auto response = server::oms::Response{
      .request_type = RequestType::MODIFY_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = status,
      .error = error,
      .text = text,
      .version = version,
      .request_id = {},
      .quantity = NaN,
      .price = NaN,
  };
  auto &result = amend_order.result;
  auto remaining_quantity = result.order_qty - result.exec_qty;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = result.symbol,
      .side = json::Map{result.side},
      .position_effect = {},
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = json::Map{result.order_type},
      .time_in_force = json::Map{result.time_in_force},
      .execution_instructions = {},
      .create_time_utc = {},
      .update_time_utc = {},
      .external_account = {},
      .external_order_id = result.order_id,
      .client_order_id = {},
      .order_status = json::Map{result.status},
      .quantity = result.order_qty,
      .price = result.order_price,
      .stop_price = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = result.exec_qty,
      .average_traded_price = NaN,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = amend_order.time,
  };
  Trace event_2{trace_info, response};
  (*this)(event_2, user_id, order_id, order_update);
}

void OrderEntry::cancel_order(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    [[maybe_unused]] std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  profile_.cancel_order([&]() {
    if (!ready())
      throw server::oms::NotReady{"not ready"sv};
    auto &[message_info, cancel_order] = event;
    auto path = shared_.api.simple.order_cancel;
    std::string buffer;  // XXX
    auto body = json::cancel_order(buffer, cancel_order, order, request_id, previous_request_id, shared_.api.category);
    auto headers = account_.create_headers(path, {}, body);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = cancel_order.order_id, version = cancel_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      cancel_order_ack(event, user_id, order_id, version);
    };
    (*connection_)("order-cancel"sv, request, callback);
  });
}

void OrderEntry::cancel_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.cancel_order_ack([&]() {
    auto handle_success = [&](auto &body) {
      json::CancelOrder cancel_order{body, decode_buffer_};
      Trace event_2{event, cancel_order};
      (*this)(event_2, user_id, order_id, version);
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CANCEL_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    process_response(event, handle_success, handle_error);
  });
}

// XXX this is a little weird -- the response tells us the last known (?) status of the order
void OrderEntry::operator()(Trace<json::CancelOrder> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  auto &[trace_info, cancel_order] = event;
  log::info<2>("cancel_order={}"sv, cancel_order);
  auto status = cancel_order.ret_code == 0 ? RequestStatus::ACCEPTED : RequestStatus::REJECTED;
  auto error = json::map_error(cancel_order.ret_code);
  auto text = cancel_order.ret_msg;
  auto response = server::oms::Response{
      .request_type = RequestType::CANCEL_ORDER,
      .origin = Origin::EXCHANGE,
      .request_status = status,
      .error = error,
      .text = text,
      .version = version,
      .request_id = {},
      .quantity = NaN,
      .price = NaN,
  };
  auto &result = cancel_order.result;
  auto remaining_quantity = result.order_qty - result.exec_qty;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = result.symbol,
      .side = json::Map{result.side},
      .position_effect = {},
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = json::Map{result.order_type},
      .time_in_force = json::Map{result.time_in_force},
      .execution_instructions = {},
      .create_time_utc = {},
      .update_time_utc = utils::safe_cast(result.cancel_time),
      .external_account = {},
      .external_order_id = result.order_id,
      .client_order_id = {},
      .order_status = json::Map{result.status},
      .quantity = result.order_qty,
      .price = result.order_price,
      .stop_price = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = result.exec_qty,
      .average_traded_price = NaN,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = cancel_order.time,
  };
  Trace event_2{trace_info, response};
  (*this)(event_2, user_id, order_id, order_update);
}

void OrderEntry::cancel_all_orders(Event<CancelAllOrders> const &event, [[maybe_unused]] std::string_view const &request_id) {
  profile_.cancel_all_orders([&]() {
    if (!ready()) [[unlikely]]
      throw server::oms::NotReady{"not ready"sv};
    auto &cancel_all_orders = event.value;
    auto send_ack = [&](auto &symbol) {
      auto cancel_all_orders_ack = CancelAllOrdersAck{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = cancel_all_orders.order_id,
          .exchange = cancel_all_orders.exchange,
          .symbol = symbol,
          .side = cancel_all_orders.side,
          .origin = Origin::GATEWAY,
          .request_status = RequestStatus::FORWARDED,
          .error = {},
          .text = {},
          .request_id = request_id,
          .external_account = {},
          .number_of_affected_orders = {},
          .round_trip_latency = {},
          .user = {},
          .strategy_id = cancel_all_orders.strategy_id,
      };
      TraceInfo trace_info{event};
      Trace event_2{trace_info, cancel_all_orders_ack};
      shared_(event_2);
    };
    auto path = shared_.api.simple.order_cancel_all;
    std::string buffer;  // XXX
    if (shared_.dispatcher.get_all_order_symbols(
            [&](auto &symbol) {
              if (!std::empty(cancel_all_orders.symbol) && symbol != cancel_all_orders.symbol)
                return;
              auto body = json::cancel_all_orders(buffer, cancel_all_orders, request_id, symbol, shared_.api.category);
              auto headers = account_.create_headers(path, {}, body);
              auto request = web::rest::Request{
                  .method = web::http::Method::POST,
                  .path = path,
                  .query = {},
                  .accept = web::http::Accept::APPLICATION_JSON,
                  .content_type = web::http::ContentType::APPLICATION_JSON,
                  .headers = headers,
                  .body = body,
                  .quality_of_service = {},
              };
              auto callback = [this](auto &request_id, auto &response) {
                TraceInfo trace_info;
                Trace event{trace_info, response};
                cancel_all_orders_ack(event, request_id);
              };
              (*connection_)("order-cancel-all"sv, request, callback);
              send_ack(symbol);
            },
            account_.name)) {
    } else {
      log::warn("*** NOT POSSIBLE TO CANCEL ALL OPEN ORDERS (NO SYMBOLS) ***"sv);
    }
  });
}

void OrderEntry::cancel_all_orders_ack(Trace<web::rest::Response> const &event, std::string_view const &request_id) {
  profile_.cancel_all_orders_ack([&]() {
    auto send_ack = [&](auto origin, auto status, Error error, std::string_view const &text) {
      auto cancel_all_orders_ack = CancelAllOrdersAck{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = {},
          .symbol = {},
          .side = {},
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .request_id = request_id,
          .external_account = {},
          .number_of_affected_orders = {},
          .round_trip_latency = {},
          .user = {},
          .strategy_id = {},
      };
      Trace event_2{event, cancel_all_orders_ack};
      shared_(event_2);
    };
    auto handle_success = [&](auto &body) {
      json::CancelAllOrders cancel_all_orders{body, decode_buffer_};
      Trace event_2{event, cancel_all_orders};
      (*this)(event_2);
      send_ack(Origin::EXCHANGE, RequestStatus::ACCEPTED, {}, {});
    };
    auto handle_error = [&]([[maybe_unused]] auto origin, [[maybe_unused]] auto status, auto error, auto text) {
      log::warn(R"(error={}, text="{}")"sv, error, text);
      send_ack(origin, RequestStatus::REJECTED, error, text);
    };
    process_response(event, handle_success, handle_error);
  });
}

void OrderEntry::operator()(Trace<json::CancelAllOrders> const &event) {
  auto &[trace_info, cancel_all_orders] = event;
  log::info<2>("cancel_all_orders={}"sv, cancel_all_orders);
}

// helpers

template <typename SuccessHandler, typename ErrorHandler>
void OrderEntry::process_response(web::rest::Response const &response, SuccessHandler success_handler, ErrorHandler error_handler) {
  try {
    auto [status, category, body] = response.result();
    switch (category) {
      using enum web::http::Category;
      case SUCCESS:  // 2xx
        success_handler(body);
        break;
      case CLIENT_ERROR:  // 4xx
        switch (status) {
          using enum web::http::Status;
          case FORBIDDEN:           // 403
            waf_limit_violation();  // note! this is *very* serious
            [[fallthrough]];
          case I_AM_A_TEAPOT:        // 418
          case TOO_MANY_REQUESTS: {  // 429
            auto text = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, text);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default: {
            // json::Error error{body};
            // error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(error.code), error.msg);
          }
        }
        break;
      case SERVER_ERROR: {  // 5xx
        auto text = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, text);
        break;
      }
      default:
        response.expect(web::http::Status::OK);  // throws
    }
  } catch (server::oms::Exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(e.origin, e.status, e.error, e.what());
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

template <typename... Args>
void OrderEntry::operator()(Trace<server::oms::Response> const &event, uint8_t user_id, uint64_t order_id, Args &&...args) {
  auto &[trace_info, response] = event;
  if (shared_.update_order(user_id, order_id, stream_id_, trace_info, response, std::forward<Args>(args)..., []([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("Did not find order: user_id={}, order_id={}"sv, user_id, order_id);
  }
}

void OrderEntry::operator()(Trace<server::oms::OrderUpdate> const &event, std::string_view const &client_order_id) {
  auto &[trace_info, order_update] = event;
  if (shared_.update_order(client_order_id, stream_id_, trace_info, order_update, [&]([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("*** EXTERNAL ORDER ***"sv);
  }
}

void OrderEntry::waf_limit_violation() {
  if (shared_.settings.rest.terminate_on_403) {
    log::fatal("WAF limit violation"sv);
  } else {
    log::warn("WAF limit violation"sv);
    (*connection_).suspend(shared_.settings.rest.back_off_delay);
  }
}

}  // namespace bybit
}  // namespace roq
