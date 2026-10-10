/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/container.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/bybit/gateway/account.hpp"
#include "roq/bybit/gateway/shared.hpp"

#include "roq/bybit/gateway/order_entry_rest.hpp"  // response
#include "roq/bybit/gateway/order_entry_ws.hpp"    // response
#include "roq/bybit/gateway/rest.hpp"              // symbols

#include "roq/bybit/protocol/json/parser.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct DropCopy final : public Base<DropCopy>, public server::Stream, public web::socket::Client::Handler, protocol::json::Parser::Handler {
  struct Handler {};

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

  void operator()(Rest::SymbolsUpdate &);

  void operator()(Trace<OrderEntryREST::Response> const &);
  void operator()(Trace<OrderEntryWS::Response> const &);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override;

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

 protected:
  // web::socket::Client::Handler

  void operator()(Trace<web::socket::Connected> const &) override;
  void operator()(Trace<web::socket::Disconnected> const &) override;
  void operator()(Trace<web::socket::Latency> const &) override;
  void operator()(Trace<web::socket::Ready> const &) override;
  void operator()(Trace<web::socket::Close> const &) override;
  void operator()(Trace<web::socket::Text> const &) override;
  void operator()(Trace<web::socket::Binary> const &) override;

  // protocol::json::Parser::Handler

  void operator()(Trace<protocol::json::Ping> const &) override;
  // response
  void operator()(Trace<protocol::json::Auth> const &) override;
  void operator()(Trace<protocol::json::Subscribe> const &) override;
  void operator()(Trace<protocol::json::Error> const &) override;
  // public stream
  void operator()(Trace<protocol::json::OrderBook> const &, size_t depth) override;
  void operator()(Trace<protocol::json::PublicTrade> const &) override;
  void operator()(Trace<protocol::json::Tickers> const &) override;
  void operator()(Trace<protocol::json::Kline> const &) override;
  // private stream
  void operator()(Trace<protocol::json::Wallet> const &) override;
  void operator()(Trace<protocol::json::Position> const &) override;
  void operator()(Trace<protocol::json::Order> const &) override;
  void operator()(Trace<protocol::json::Execution> const &) override;

  // helpers

  void send_login();

  void subscribe();

  void subscribe(std::string_view const &topic);

  void parse(std::string_view const &message);

 private:
  [[maybe_unused]] Handler &handler_;
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
    utils::metrics::Profile parse, auth, wallet, order, execution, position;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus connection_status_ = {};
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
  // ...
  utils::unordered_set<std::string> symbols_;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
