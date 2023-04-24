/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/bybit/drop_copy.hpp"

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/core/metrics/factory.hpp"

#include "roq/core/json/buffer.hpp"

#include "roq/web/socket/client_factory.hpp"

#include "roq/bybit/flags.hpp"

#include "roq/bybit/json/utils.hpp"

using namespace std::literals;
using namespace std::chrono_literals;  // NOLINT

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const SUPPORTS = Mask{
    SupportType::ORDER,
    SupportType::TRADE,
    SupportType::FUNDS,
};

auto const AUTH_EXPIRES = 1s;
}  // namespace

// === CONSTANTS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &context) {
  auto uri = flags::Flags::ws_private_uri();
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .validate_certificate = server::Flags::net_tls_validate_certificate(),
      // connection manager
      .connection_timeout = server::Flags::net_connection_timeout(),
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = Flags::ws_ping_freq(),
      // implementation
      .decode_buffer_size = Flags::decode_buffer_size(),
      .encode_buffer_size = Flags::encode_buffer_size(),
  };
  return web::socket::ClientFactory::create(handler, context, config, []() { return std::string(); });
}

struct create_metrics final : public core::metrics::Factory {
  explicit create_metrics(auto const &group, auto const &function)
      : core::metrics::Factory(server::Flags::name(), group, function) {}
};
}  // namespace

// === IMPLEMENTATION ===

DropCopy::DropCopy(
    Handler &handler, io::Context &context, uint16_t stream_id, Authenticator &authenticator, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)},
      connection_{create_connection(*this, context)}, decode_buffer_{Flags::decode_buffer_size()},
      counter_{
          .disconnect = create_metrics(name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(name_, "parse"sv),
          .auth = create_metrics(name_, "auth"sv),
          .outbound_account_info = create_metrics(name_, "outbound_account_info"sv),
          .order = create_metrics(name_, "order"sv),
          .ticket_info = create_metrics(name_, "ticket_info"sv),
      },
      latency_{
          .ping = create_metrics(name_, "ping"sv),
          .heartbeat = create_metrics(name_, "heartbeat"sv),
      },
      authenticator_{authenticator}, shared_{shared}, download_{{}, [this](auto state) { return download(state); }} {
}

bool DropCopy::ready() const {
  return (*connection_).ready();
}

void DropCopy::operator()(Event<Start> const &) {
  (*connection_).start();
}

void DropCopy::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void DropCopy::operator()(Event<Timer> const &event) {
  (*connection_).refresh(event.value.now);
}

void DropCopy::operator()(metrics::Writer &writer) {
  writer
      // counter
      .write(counter_.disconnect, metrics::COUNTER)
      // profile
      .write(profile_.parse, metrics::PROFILE)
      .write(profile_.auth, metrics::PROFILE)
      .write(profile_.outbound_account_info, metrics::PROFILE)
      .write(profile_.order, metrics::PROFILE)
      .write(profile_.ticket_info, metrics::PROFILE)
      // latency
      .write(latency_.ping, metrics::LATENCY)
      .write(latency_.heartbeat, metrics::LATENCY);
}

void DropCopy::operator()(web::socket::Client::Connected const &) {
  assert(logon_timeout_.count() == 0);
  auto now = clock::get_system();
  logon_timeout_ = now + Flags::ws_request_timeout();
}

void DropCopy::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  ready_ = false;
  (*this)(ConnectionStatus::DISCONNECTED);
  download_.reset();
  welcome_ = false;
  logon_timeout_ = {};
  next_ping_ = {};
}

void DropCopy::operator()(web::socket::Client::Ready const &) {
  auto now = clock::get_realtime();
  auto expires = std::chrono::duration_cast<std::chrono::milliseconds>(now + AUTH_EXPIRES);
  auto signature = authenticator_.create_signature(expires);
  auto message = fmt::format(
      R"({{)"
      R"("req_id":"{}",)"
      R"("op": "auth",)"
      R"("args":["{}",{},"{}"])"
      R"(}})"sv,
      ++request_id_,
      authenticator_.get_key(),
      expires.count(),
      signature);
  log::debug(R"(message="{}")"sv, message);
  (*connection_).send_text(message);
  (*this)(ConnectionStatus::LOGIN_SENT);
}

void DropCopy::operator()(web::socket::Client::Close const &) {
}

void DropCopy::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = authenticator_.get_account(),
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void DropCopy::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void DropCopy::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = authenticator_.get_account(),
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::WS,
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

uint32_t DropCopy::download(DropCopyState state) {
  switch (state) {
    using enum DropCopyState;
    case UNDEFINED:
      assert(false);
      break;
    case SUBSCRIBE:
      subscribe();
      return {};
    case DONE:
      (*this)(ConnectionStatus::READY);
      assert(!ready_);
      ready_ = true;
      return {};
  }
  assert(false);
  return {};
}

void DropCopy::subscribe() {
  subscribe("wallet"sv);
  subscribe("position"sv);
  subscribe("order"sv);
  subscribe("execution"sv);
}

void DropCopy::subscribe(std::string_view const &topic) {
  auto message = fmt::format(
      R"({{)"
      R"("req_id":"{}",)"
      R"("op":"subscribe",)"
      R"("args":["{}"])"
      R"(}})"sv,
      ++request_id_,
      topic);
  log::debug("message={}"sv, message);
  (*connection_).send_text(message);
}

void DropCopy::parse(std::string_view const &message) {
  profile_.parse([&]() {
    try {
      log::debug(R"(message="{}")"sv, message);
      TraceInfo trace_info;
      core::json::Buffer buffer{decode_buffer_};
      if (json::Parser::dispatch(*this, message, buffer, trace_info)) {
      } else {
        log::fatal(R"(Unexpected: failed to parse message="{}")"sv, message);
      }
    } catch (...) {
      log::warn(R"(message="{}")"sv, message);
      core::tools::UnhandledException::terminate();
    }
  });
}

void DropCopy::operator()(Trace<json::Error> const &event) {
  auto &[trace_info, error] = event;
  log::info<4>("event={{error={}, trace_info={}}}"sv, error, trace_info);
  log::fatal("error={}"sv, error);
}

void DropCopy::operator()(Trace<json::Pong> const &event) {
  auto &[trace_info, pong] = event;
  log::info<4>("event={{pong={}, trace_info={}}}"sv, pong, trace_info);
}

void DropCopy::operator()(Trace<json::Subscribe> const &event) {
  auto &[trace_info, subscribe] = event;
  log::info<4>("event={{subscribe={}, trace_info={}}}"sv, subscribe, trace_info);
}

void DropCopy::operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::PublicTrade> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::Tickers> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::Auth> const &event) {
  profile_.auth([&]() {
    auto &[trace_info, auth] = event;
    log::info<4>("event={{auth={}, trace_info={}}}"sv, auth, trace_info);
    if (auth.success) {
      (*this)(ConnectionStatus::READY);
      subscribe();
    } else {
      // XXX ???
    }
  });
}

void DropCopy::operator()(Trace<json::Wallet> const &event) {
  profile_.order([&]() {
    auto &[trace_info, wallet] = event;
    log::info<4>("event={{wallet={}, trace_info={}}}"sv, wallet, trace_info);
    /*
    for (auto &item : wallet.data.list) {
      auto funds_update = FundsUpdate{
          .stream_id = stream_id_,
          .account = authenticator_.get_account(),
          .currency = item.asset,
          .balance = item.available,
          .hold = item.locked,
          .external_account = {},
          .update_type = UpdateType::INCREMENTAL,
          .exchange_time_utc = wallet.ts,
          .sending_time_utc = {},
      };
      create_trace_and_dispatch(handler_, trace_info, funds_update, true);
    }
    */
  });
}

void DropCopy::operator()(Trace<json::Order> const &event) {
  profile_.order([&]() {
    auto &[trace_info, order] = event;
    log::info<4>("event={{order={}, trace_info={}}}"sv, order, trace_info);
    for (auto &item : order.data) {
      auto side = json::map(item.side);
      auto order_type = json::map(item.order_type);
      auto time_in_force = json::map(item.time_in_force);
      auto order_status = json::map(item.order_status);
      /*
      auto remaining_quantity = item.quantity - item.total_filled_quantity;
      auto average_traded_price =
          utils::is_zero(item.total_filled_quantity) ? NaN : item.total_filled_value / item.total_filled_quantity;
      // XXX can limit_maker map to last_liquidity? or just an order creation flag?
      auto order_update = oms::OrderUpdate{
          .account = authenticator_.get_account(),
          .exchange = Flags::exchange(),
          .symbol = item.symbol,
          .side = side,
          .position_effect = {},
          .max_show_quantity = NaN,
          .order_type = order_type,
          .time_in_force = time_in_force,
          .execution_instructions = {},
          .create_time_utc = item.order_creation_time,
          .update_time_utc = {},
          .external_account = {},
          .external_order_id = item.order_id,
          .status = order_status,
          .quantity = item.quantity,
          .price = item.price,
          .stop_price = NaN,
          .remaining_quantity = remaining_quantity,
          .traded_quantity = item.total_filled_quantity,
          .average_traded_price = average_traded_price,
          .last_traded_quantity = item.last_filled_quantity,
          .last_traded_price = item.last_filled_price,
          .last_liquidity = {},
          .update_type = UpdateType::INCREMENTAL,
          .sending_time_utc = {},
      };
      if (shared_.update_order(
              item.order_link_id, stream_id_, trace_info, order_update, [&]([[maybe_unused]] auto &order) {
                // no fills here
              })) {
      } else {
        log::warn("*** EXTERNAL ORDER ***"sv);
        log::warn("order={}"sv, item);
      }
      */
    }
  });
}

void DropCopy::operator()(Trace<json::TicketInfo> const &event) {
  profile_.ticket_info([&]() {
    auto &trace_info = event.trace_info;
    auto &ticket_info = event.value;
    log::info<4>("event={{ticket_info={}, trace_info={}}}"sv, ticket_info, trace_info);
    for (auto &item : ticket_info.data) {
      if (shared_.find_order(item.order_link_id, [&](auto &order) {
            auto liquidity = item.is_maker ? Liquidity::MAKER : Liquidity::TAKER;
            auto fill = Fill{
                .external_trade_id = item.trade_id,
                .quantity = item.quantity,
                .price = item.price,
                .liquidity = liquidity,
            };
            auto trade_update = oms::TradeUpdate{
                .account = order.account,
                .order_id = order.order_id,
                .exchange = order.exchange,
                .symbol = order.symbol,
                .side = order.side,
                .position_effect = order.position_effect,
                .create_time_utc = utils::safe_cast(item.event_time),
                .update_time_utc = utils::safe_cast(item.event_time),
                .external_account = item.account_id,
                .external_order_id = item.order_id,
                .fills = {&fill, 1},
                .update_type = {},
                .sending_time_utc = {},
            };
            create_trace_and_dispatch(handler_, trace_info, trade_update, stream_id_, true, order.user_id);
          })) {
      } else {
        log::warn<1>("*** EXTERNAL ORDER ***"sv);
        log::warn<2>("ticket_info={}"sv, item);
      }
    }
  });
}

}  // namespace bybit
}  // namespace roq
