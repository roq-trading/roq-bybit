/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/order_entry_ws.hpp"

#include <algorithm>
#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/common.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/bybit/json/encoder.hpp"
#include "roq/bybit/json/utils.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace bybit {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const AUTH_EXPIRES = 1s;

size_t const MAX_DECODE_BUFFER_DEPTH = 1;
}  // namespace

// === HELPERS ===

namespace {
auto get_supports(auto api) {
  auto result = Mask{
      SupportType::CREATE_ORDER,
      SupportType::CANCEL_ORDER,
      SupportType::ORDER_ACK,
  };
  if (api != tools::API::SPOT) {
    result |= SupportType::MODIFY_ORDER;
  }
  return result;
}

auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.ws.trade_uri;
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .host = settings.ws.trade_host,
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = settings.net.connection_timeout,
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = settings.ws.ping_freq,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::socket::Client::create(handler, context, config, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

OrderEntryWS::OrderEntryWS(OrderEntry::Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .auth = create_metrics(shared.settings, name_, "auth"sv),
          .create_order = create_metrics(shared.settings, name_, "create_order"sv),
          .modify_order = create_metrics(shared.settings, name_, "modify_order"sv),
          .cancel_order = create_metrics(shared.settings, name_, "cancel_order"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared} {
}

// OrderEntry

void OrderEntryWS::operator()(Event<Start> const &) {
  (*connection_).start();
}

void OrderEntryWS::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void OrderEntryWS::operator()(Event<Timer> const &event) {
  (*connection_).refresh(event.value.now);
}

void OrderEntryWS::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.auth, metrics::Type::PROFILE)
      .write(profile_.create_order, metrics::Type::PROFILE)
      .write(profile_.modify_order, metrics::Type::PROFILE)
      .write(profile_.cancel_order, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

uint16_t OrderEntryWS::operator()(Event<CreateOrder> const &event, server::oms::Order const &order, std::string_view const &request_id) {
  profile_.create_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, create_order] = event;
    auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
    auto message =
        json::Encoder::place_order_ws(encode_buffer_, create_order, order, request_id, shared_.api.category, now_utc, shared_.settings.rest.recv_window);
    log::debug("{}"sv, message);
    (*connection_).send_text(message);
  });
  return stream_id_;
}

uint16_t OrderEntryWS::operator()(
    Event<ModifyOrder> const &event, server::oms::Order const &order, std::string_view const &request_id, std::string_view const &previous_request_id) {
  profile_.modify_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, modify_order] = event;
    auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
    auto message = json::Encoder::amend_order_ws(
        encode_buffer_, modify_order, order, request_id, previous_request_id, shared_.api.category, now_utc, shared_.settings.rest.recv_window);
    log::debug("{}"sv, message);
    (*connection_).send_text(message);
  });
  return stream_id_;
}

uint16_t OrderEntryWS::operator()(
    Event<CancelOrder> const &event, server::oms::Order const &order, std::string_view const &request_id, std::string_view const &previous_request_id) {
  profile_.cancel_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_order] = event;
    auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
    auto message = json::Encoder::cancel_order_ws(
        encode_buffer_, cancel_order, order, request_id, previous_request_id, shared_.api.category, now_utc, shared_.settings.rest.recv_window);
    log::debug("{}"sv, message);
    (*connection_).send_text(message);
  });
  return stream_id_;
}

uint16_t OrderEntryWS::operator()(Event<CancelAllOrders> const &, [[maybe_unused]] std::string_view const &request_id) {
  log::fatal("Unexpected"sv);
}

// web::socket::Client::Handler

void OrderEntryWS::operator()(web::socket::Client::Connected const &) {
  assert(logon_timeout_.count() == 0);
  auto now = clock::get_system();
  logon_timeout_ = now + shared_.settings.ws.request_timeout;
}

void OrderEntryWS::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  logon_timeout_ = {};
  account_.request_queue.clear();
}

void OrderEntryWS::operator()(web::socket::Client::Ready const &) {
  send_login();
  (*this)(ConnectionStatus::LOGIN_SENT);
}

void OrderEntryWS::operator()(web::socket::Client::Close const &) {
}

void OrderEntryWS::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void OrderEntryWS::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void OrderEntryWS::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

// helpers

bool OrderEntryWS::ready() const {
  return (*connection_).ready();
}

void OrderEntryWS::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = account_.name,
        .supports = get_supports(shared_.api.api),
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

void OrderEntryWS::send_login() {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  auto expires_utc = now_utc + AUTH_EXPIRES;
  auto signature = account_.create_signature(expires_utc);
  auto message = fmt::format(
      R"({{)"
      R"("req_id":"auth",)"
      R"("op": "auth",)"
      R"("args":["{}",{},"{}"])"
      R"(}})"sv,
      account_.get_key(),
      expires_utc.count(),
      signature);
  log::debug("{}"sv, message);
  (*connection_).send_text(message);
}

void OrderEntryWS::parse(std::string_view const &message) {
  profile_.parse([&]() {
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      TraceInfo trace_info;
      if (!json::Parser2::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

// json::Parser2::Handler

void OrderEntryWS::operator()(Trace<json::Ping> const &event) {
  auto &[trace_info, ping] = event;
  log::info<4>("event={{ping={}, trace_info={}}}"sv, ping, trace_info);
}

void OrderEntryWS::operator()(Trace<json::Auth2> const &event) {
  profile_.auth([&]() {
    auto &[trace_info, auth] = event;
    log::info<4>("event={{auth={}, trace_info={}}}"sv, auth, trace_info);
    if (auth.ret_code == 0) {
      (*this)(ConnectionStatus::READY);
    } else {
      log::fatal("Unexpected: auth={}"sv, auth);
    }
  });
}

void OrderEntryWS::operator()(Trace<json::Error> const &event) {
  auto &[trace_info, error] = event;
  log::info<4>("event={{error={}, trace_info={}}}"sv, error, trace_info);
  log::fatal("error={}"sv, error);
}

void OrderEntryWS::operator()(Trace<json::PlaceOrder2> const &event) {
  auto &[trace_info, place_order] = event;
  if (place_order.ret_code != 0) {
    auto error = json::map_error(place_order.ret_code);
    auto response = server::oms::Response{
        .request_type = RequestType::CREATE_ORDER,
        .origin = Origin::EXCHANGE,
        .request_status = RequestStatus::REJECTED,
        .error = error,
        .text = place_order.ret_msg,
        .version = {},
        .request_id = place_order.req_id,
        .quantity = NaN,
        .price = NaN,
    };
    if (shared_.update_order(response.request_id, stream_id_, trace_info, response, []([[maybe_unused]] auto &order) {})) {
    } else {
      log::warn(R"(Did not find order: request_id="{}")"sv, response.request_id);
    }
  }
}

void OrderEntryWS::operator()(Trace<json::AmendOrder2> const &event) {
  auto &[trace_info, amend_order] = event;
  if (amend_order.ret_code != 0) {
    auto error = json::map_error(amend_order.ret_code);
    auto response = server::oms::Response{
        .request_type = RequestType::MODIFY_ORDER,
        .origin = Origin::EXCHANGE,
        .request_status = RequestStatus::REJECTED,
        .error = error,
        .text = amend_order.ret_msg,
        .version = {},
        .request_id = amend_order.req_id,
        .quantity = NaN,
        .price = NaN,
    };
    if (shared_.update_order(response.request_id, stream_id_, trace_info, response, []([[maybe_unused]] auto &order) {})) {
    } else {
      log::warn(R"(Did not find order: request_id="{}")"sv, response.request_id);
    }
  }
}

void OrderEntryWS::operator()(Trace<json::CancelOrder2> const &event) {
  auto &[trace_info, cancel_order] = event;
  if (cancel_order.ret_code != 0) {
    auto error = json::map_error(cancel_order.ret_code);
    auto response = server::oms::Response{
        .request_type = RequestType::CANCEL_ORDER,
        .origin = Origin::EXCHANGE,
        .request_status = RequestStatus::REJECTED,
        .error = error,
        .text = cancel_order.ret_msg,
        .version = {},
        .request_id = cancel_order.req_id,
        .quantity = NaN,
        .price = NaN,
    };
    if (shared_.update_order(response.request_id, stream_id_, trace_info, response, []([[maybe_unused]] auto &order) {})) {
    } else {
      log::warn(R"(Did not find order: request_id="{}")"sv, response.request_id);
    }
  }
}

}  // namespace bybit
}  // namespace roq
