/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/utils/container.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/bybit/account.hpp"
#include "roq/bybit/shared.hpp"

#include "roq/bybit/order_entry.hpp"

#include "roq/bybit/json/parser_2.hpp"

namespace roq {
namespace bybit {

struct OrderEntryWS final : public OrderEntry, public web::socket::Client::Handler, json::Parser2::Handler {
  OrderEntryWS(OrderEntry::Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

 protected:
  // OrderEntry

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Event<ModifyOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Event<CancelOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id) override;

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) override;

  // web::socket::Client::Handler

  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

  // json::Parser2::Handler

  void operator()(Trace<json::Ping> const &) override;
  void operator()(Trace<json::Auth2> const &) override;
  void operator()(Trace<json::Error> const &) override;
  void operator()(Trace<json::PlaceOrder2> const &) override;
  void operator()(Trace<json::AmendOrder2> const &) override;
  void operator()(Trace<json::CancelOrder2> const &) override;

 private:
  bool ready() const;

  void operator()(ConnectionStatus);

  void send_login();

  void parse(std::string_view const &message);

  OrderEntry::Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // web socket
  std::unique_ptr<web::socket::Client> const connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse,  //
        auth,                       //
        create_order, modify_order, cancel_order;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus status_ = {};
  std::chrono::nanoseconds logon_timeout_ = {};
  // ...
  std::string encode_buffer_;
};

}  // namespace bybit
}  // namespace roq
