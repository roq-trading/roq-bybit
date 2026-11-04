/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/gateway/drop_copy.hpp"

#include "roq/mask.hpp"

#include "roq/utils/common.hpp"
#include "roq/utils/safe_cast.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/bybit/protocol/json/encoder.hpp"
#include "roq/bybit/protocol/json/map.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace bybit {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const AUTH_EXPIRES = 1s;

size_t const MAX_DECODE_BUFFER_DEPTH = 2;

auto const CHECK_STALE_ORDERS_FREQ = 10s;
}  // namespace

// === HELPERS ===

namespace {
auto get_supports(auto api) {
  auto result = Mask{
      SupportType::FUNDS,
      SupportType::ORDER,
      SupportType::TRADE,
  };
  if (api != tools::API::SPOT) {
    result |= SupportType::POSITION;
  }
  return result;
}

auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context, auto &shared) {
  auto uri = settings.ws.private_uri;
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .host = settings.ws.private_host,
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = settings.net.connection_timeout,
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = settings.ws.ping_freq,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::socket::Client::create(handler, context, config, shared.throttle, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

DropCopy::DropCopy(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, Request &request)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, connection_{create_connection(*this, shared.settings, context, shared)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .auth = create_metrics(shared.settings, name_, "auth"sv),
          .wallet = create_metrics(shared.settings, name_, "wallet"sv),
          .order = create_metrics(shared.settings, name_, "order"sv),
          .execution = create_metrics(shared.settings, name_, "execution"sv),
          .position = create_metrics(shared.settings, name_, "position"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
          .heartbeat = create_metrics(shared.settings, name_, "heartbeat"sv),
      },
      account_{account}, shared_{shared}, request_{request}, download_{shared.settings.rest.request_timeout, [this](auto &event) { return download(event); }} {
}

// server::Stream

void DropCopy::operator()(Trace<Start> const &) {
  (*connection_).start();
}

void DropCopy::operator()(Trace<Stop> const &) {
  (*connection_).stop();
}

void DropCopy::operator()(Trace<Timer> const &event) {
  auto &[trace_info, timer] = event;
  if ((*connection_).refresh(trace_info)) {
    check_request_queue(trace_info);
    if (ready()) {
      check_stale_orders(trace_info);
    }
  }
}

void DropCopy::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.auth, metrics::Type::PROFILE)
      .write(profile_.wallet, metrics::Type::PROFILE)
      .write(profile_.order, metrics::Type::PROFILE)
      .write(profile_.execution, metrics::Type::PROFILE)
      .write(profile_.position, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY)
      .write(latency_.heartbeat, metrics::Type::LATENCY);
}

void DropCopy::operator()(Trace<ConnectionStatus> const &event, std::string_view const &reason) {
  auto &[trace_info, connection_status] = event;
  connection_status_ = connection_status;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = account_.name,
      .supports = get_supports(shared_.api.api),
      .transport = Transport::TCP,
      .protocol = Protocol::WS,
      .encoding = {Encoding::JSON},
      .priority = Priority::PRIMARY,
      .connection_status = connection_status_,
      .reason = reason,
      .interface = (*connection_).get_interface(),
      .authority = (*connection_).get_current_authority(),
      .path = (*connection_).get_current_path(),
      .proxy = (*connection_).get_proxy(),
  };
  log::info("stream_status={}"sv, stream_status);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, stream_status);
}

// web::socket::Client::Handler

void DropCopy::operator()(Trace<web::socket::Connected> const &event) {
  auto &[trace_info, connected] = event;
  assert(logon_timeout_.count() == 0);
  logon_timeout_ = trace_info.origin_create_time + shared_.settings.ws.request_timeout;
}

void DropCopy::operator()(Trace<web::socket::Disconnected> const &event) {
  auto &[trace_info, disconnected] = event;
  ++counter_.disconnect;
  create_trace_and_dispatch_2(trace_info, ConnectionStatus::DISCONNECTED);
  logon_timeout_ = {};
  next_ping_ = {};
  download_.reset();
}

void DropCopy::operator()(Trace<web::socket::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(shared_.dispatcher, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void DropCopy::operator()(Trace<web::socket::Ready> const &event) {
  auto &[trace_info, ready] = event;
  send_login(trace_info);
  create_trace_and_dispatch_2(trace_info, ConnectionStatus::LOGIN_SENT);
}

void DropCopy::operator()(Trace<web::socket::Close> const &event) {
  auto &[trace_info, close] = event;
  log::warn("close={}"sv, close);
}

void DropCopy::operator()(Trace<web::socket::Text> const &event) {
  auto &[trace_info, text] = event;
  parse(trace_info, text.payload);
}

void DropCopy::operator()(Trace<web::socket::Binary> const &) {
  log::fatal("Unexpected"sv);
}

// core::Download

int32_t DropCopy::download(Trace<State> const &event) {
  auto &[trace_info, state] = event;
  auto publish_downloading = [&]() { create_trace_and_dispatch_2(trace_info, ConnectionStatus::DOWNLOADING, magic_enum::enum_name(state)); };
  switch (state) {
    using enum State;
    case UNDEFINED:
      assert(false);
      break;
    case GET_ACCOUNT:
      publish_downloading();
      request_.account.request(trace_info);
      return 1;
    case SUBSCRIBE_BALANCES:
      publish_downloading();
      subscribe(state, "wallet"sv);
      return 1;
    case GET_BALANCES:
      publish_downloading();
      request_.balances.request(trace_info);
      return 1;
    case SUBSCRIBE_POSITIONS:
      if (shared_.api.api == tools::API::SPOT) {
        return 0;
      } else {
        publish_downloading();
        subscribe(state, "position"sv);
        return 1;
      }
    case GET_POSITIONS:
      if (shared_.api.api == tools::API::SPOT) {
        return 0;
      } else {
        publish_downloading();
        request_.positions.request(trace_info);
        return 1;
      }
    case SUBSCRIBE_ORDERS:
      publish_downloading();
      subscribe(state, "order"sv);
      return 1;
    case GET_OPEN_ORDERS:
      publish_downloading();
      request_.open_orders.request(trace_info);
      return 1;
    case GET_STALE_ORDERS:
      if (std::empty(request_.details_stale_orders)) {
        return 0;
      } else {
        publish_downloading();
        request_.stale_orders.request(trace_info);
        return 1;
      }
    case SUBSCRIBE_TRADES:
      publish_downloading();
      subscribe(state, "execution"sv);
      return 1;
    case GET_TRADES:
      if (shared_.settings.download.trades_lookback.count()) {
        publish_downloading();
        request_.trades.request(trace_info);
        return 1;
      } else {
        return 0;
      }
    case DONE:
      create_trace_and_dispatch_2(trace_info, ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// protocol::json::Parser::Handler

void DropCopy::operator()(Trace<protocol::json::Ping> const &event) {
  auto &[trace_info, ping] = event;
  log::info<4>("ping={}"sv, ping);
}

void DropCopy::operator()(Trace<protocol::json::Auth> const &event) {
  profile_.auth([&]() {
    auto &[trace_info, auth] = event;
    log::info<4>("auth={}"sv, auth);
    if (auth.success) {
      download_.begin(trace_info);
    } else {
      if (shared_.settings.experimental.retry_logon) {
        log::error("[{}] auth={}"sv, account_.name, auth);
        log::warn("Disconnecting..."sv);
        (*connection_).close();
      } else {
        log::fatal("[{}] auth={}"sv, account_.name, auth);
      }
    }
  });
}

void DropCopy::operator()(Trace<protocol::json::Subscribe> const &event) {
  auto &[trace_info, subscribe] = event;
  log::info<4>("subscribe={}"sv, subscribe);
  auto request_id = magic_enum::enum_cast<State>(subscribe.req_id);
  if (request_id.has_value()) {
    auto state = request_id.value();
    download_.check(trace_info, state);
  } else {
    log::warn(R"(Unexpected: request_id="{}")"sv, subscribe.req_id);
  }
}

void DropCopy::operator()(Trace<protocol::json::Error> const &event) {
  auto &[trace_info, error] = event;
  log::info<4>("error={}"sv, error);
  log::fatal("error={}"sv, error);
}

void DropCopy::operator()(Trace<protocol::json::OrderBook> const &, [[maybe_unused]] size_t depth) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::PublicTrade> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::Tickers> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::Kline> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::Wallet> const &event) {
  profile_.wallet([&]() {
    auto &[trace_info, wallet] = event;
    log::info<4>("wallet={}"sv, wallet);
    // XXX probably we need to filter and match --api
    for (auto &item : wallet.data) {
      for (auto &item_2 : item.coin) {
        // XXX maybe margin mode is from account_type?
        auto funds_update = FundsUpdate{
            .stream_id = stream_id_,
            .account = account_.name,
            .currency = item_2.coin,
            .margin_mode = {},
            .balance = item_2.wallet_balance,  // XXX item.free ???
            .hold = item_2.locked,
            .borrowed = NaN,
            .unrealized_pnl = NaN,
            .external_account = {},
            .update_type = UpdateType::INCREMENTAL,
            .exchange_time_utc = {},
            .sending_time_utc = {},  // XXX lost when flattened
        };
        create_trace_and_dispatch(shared_.dispatcher, trace_info, funds_update, true);
      }
    }
  });
}

void DropCopy::operator()(Trace<protocol::json::Position> const &event) {
  profile_.position([&]() {
    auto &[trace_info, position] = event;
    log::info<4>("position={}"sv, position);
    for (auto &item : position.data) {
      log::info<2>("item={}"sv, item);
      if (shared_.dispatcher.discard_symbol(item.symbol)) {
        continue;
      }
      auto margin_mode = item.trade_mode == 0 ? MarginMode::CROSS : MarginMode::ISOLATED;
      auto side = map(item.side).template get<Side>();
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
          .update_type = UpdateType::INCREMENTAL,
          .exchange_time_utc = item.updated_time,
          .sending_time_utc = position.creation_time,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, position_update, true);
    }
  });
}

void DropCopy::operator()(Trace<protocol::json::Order> const &event) {
  profile_.order([&]() {
    auto &[trace_info, order] = event;
    log::info<4>("order={}"sv, order);
    for (auto &item : order.data) {
      log::info<2>("item={}"sv, item);
      auto order_update = server::oms::OrderUpdate{
          .account = account_.name,
          .exchange = shared_.settings.exchange,
          .symbol = item.symbol,
          .side = map(item.side),
          .position_effect = {},
          .margin_mode = {},
          .max_show_quantity = NaN,
          .order_type = map(item.order_type),
          .time_in_force = map(item.time_in_force),
          .execution_instructions = {},
          .execution_destination = {},
          .create_time_utc = item.created_time,
          .update_time_utc = item.updated_time,
          .external_account = {},
          .external_order_id = item.order_id,
          .client_order_id = item.order_link_id,
          .order_status = map(item.order_status),
          .error = {},
          .text = {},
          .quantity = item.qty,
          .price = item.price,
          .stop_price = NaN,
          .leverage = NaN,
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
          .update_type = UpdateType::INCREMENTAL,
          .sending_time_utc = order.creation_time,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
    }
  });
}

void DropCopy::operator()(Trace<protocol::json::Execution> const &event) {
  profile_.execution([&]() {
    auto &[trace_info, execution] = event;
    log::info<4>("execution={}"sv, execution);
    std::string_view order_id;
    std::string_view order_link_id;
    std::string_view symbol;
    Side side = {};
    std::chrono::nanoseconds exec_time = {};
    double multiplier = NaN;
    auto dispatch = [&]() {
      if (std::empty(shared_.fills)) {
        return;
      }
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
          .client_order_id = order_link_id,
          .fills = shared_.fills,
          .routing_id = {},
          .update_type = UpdateType::INCREMENTAL,
          .sending_time_utc = execution.creation_time,
          .user = {},
          .strategy_id = {},
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, trade_update, true, SOURCE_NONE);
      shared_.fills.clear();
    };
    for (auto &item : execution.data) {
      log::info<2>("item={}"sv, item);
      /* XXX doesn't work with spot
      if (item.exec_type != protocol::json::ExecType::TRADE)  // note!
        continue;
      */
      if (item.order_id != order_id) {
        dispatch();
        order_id = item.order_id;
        order_link_id = item.order_link_id;
        symbol = item.symbol;
        side = map(item.side);
        exec_time = item.exec_time;
        auto ref_data = shared_.dispatcher.get_ref_data(shared_.settings.exchange, symbol);
        multiplier = ref_data.multiplier;
      }
      auto liquidity = item.is_maker ? Liquidity::MAKER : Liquidity::TAKER;
      auto profit_loss_amount = utils::compute_profit_loss_amount(side, item.exec_qty, item.exec_price, multiplier);
      auto fill = Fill{
          .external_trade_id = item.exec_id,
          .quantity = item.exec_qty,
          .price = item.exec_price,
          .liquidity = liquidity,
          .commission_amount = item.exec_fee,  // XXX ???
          .commission_currency = item.fee_currency,
          .base_amount = NaN,
          .quote_amount = NaN,
          .profit_loss_amount = profit_loss_amount,
      };
      shared_.fills.emplace_back(std::move(fill));
    }
    dispatch();
  });
}

// helpers

void DropCopy::send_login(TraceInfo const &trace_info) {
  auto expires_utc = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.origin_create_time_utc + AUTH_EXPIRES);
  auto signature = account_.create_signature(expires_utc);
  auto message = protocol::json::Encoder::login_ws(encode_buffer_, account_.get_key(), expires_utc, signature);
  (*connection_).send_text(message);
}

void DropCopy::subscribe(State state, std::string_view const &topic) {
  auto message = fmt::format(
      R"({{)"
      R"("req_id":"{}",)"
      R"("op":"subscribe",)"
      R"("args":["{}"])"
      R"(}})"sv,
      state,
      topic);
  (*connection_).send_text(message);
}

void DropCopy::check_request_queue(TraceInfo const &trace_info) {
  auto state = download_.state();
  switch (state) {
    using enum State;
    case UNDEFINED:
      break;
    case GET_ACCOUNT:
      if (request_.account.has_response()) {
        download_.check(trace_info, state);
      }
      break;
    case SUBSCRIBE_BALANCES:
      break;
    case GET_BALANCES:
      if (request_.balances.has_response()) {
        download_.check(trace_info, state);
      }
      break;
    case SUBSCRIBE_POSITIONS:
      break;
    case GET_POSITIONS:
      if (request_.positions.has_response()) {
        download_.check(trace_info, state);
      }
      break;
    case SUBSCRIBE_ORDERS:
      break;
    case GET_OPEN_ORDERS:
      if (request_.open_orders.has_response()) {
        check_stale_orders();  // note! *before* updating download
        download_.check(trace_info, state);
      }
      break;
    case GET_STALE_ORDERS:
      if (request_.stale_orders.has_response()) {
        if (!std::empty(request_.details_stale_orders)) {
          log::fatal("Unexpected"sv);
        }
        download_.check(trace_info, state);
      }
      break;
    case SUBSCRIBE_TRADES:
      break;
    case GET_TRADES:
      if (request_.trades.has_response()) {
        download_.check(trace_info, state);
      }
      break;
    case DONE:
      break;
  }
}

bool DropCopy::check_stale_orders() {
  size_t counter = 0;
  auto helper = [&](auto &order) {
    auto key = std::make_pair(order.user_id, order.order_id);
    auto res = request_.details_stale_orders.insert(key);
    if (res.second) {
      log::warn(
          R"(DEBUG stale order: )"
          R"(order_id={}, client_order_id="{}", external_order_id="{}")"sv,
          order.order_id,
          order.client_order_id,
          order.external_order_id);
      ++counter;
    }
  };
  if (shared_.dispatcher.get_all_stale_orders(helper, account_.name)) {
  } else {
  }
  if (counter) {
    log::info("Found {} new lost order(s)"sv, counter);
    return true;
  } else {
    return false;
  }
}

void DropCopy::check_stale_orders(TraceInfo const &trace_info) {
  if (next_check_stale_orders_ < trace_info.origin_create_time) {
    next_check_stale_orders_ = trace_info.origin_create_time + CHECK_STALE_ORDERS_FREQ;
    if (check_stale_orders()) {
      request_.stale_orders.request(trace_info);
    }
  }
}

void DropCopy::parse(TraceInfo const &trace_info, std::string_view const &message) {
  profile_.parse([&]() {
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      if (!protocol::json::Parser::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
