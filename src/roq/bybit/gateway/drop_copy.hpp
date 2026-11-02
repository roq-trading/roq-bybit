/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/container.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/download_2.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/bybit/gateway/account.hpp"
#include "roq/bybit/gateway/request.hpp"
#include "roq/bybit/gateway/shared.hpp"

#include "roq/bybit/gateway/symbols_update.hpp"

#include "roq/bybit/protocol/json/parser.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct DropCopy final : public Base<DropCopy>, public server::Stream, public web::socket::Client::Handler, protocol::json::Parser::Handler {
  struct Handler {};

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, Request &);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override { return connection_status_ == ConnectionStatus::READY; }

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

  // core::Download

  enum class State {
    UNDEFINED = 0,
    GET_ACCOUNT,
    SUBSCRIBE_BALANCES,
    GET_BALANCES,
    SUBSCRIBE_POSITIONS,
    GET_POSITIONS,
    SUBSCRIBE_ORDERS,
    GET_OPEN_ORDERS,
    GET_STALE_ORDERS,
    SUBSCRIBE_TRADES,
    GET_TRADES,
    DONE,
  };

  int32_t download(Trace<State> const &);

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

  void send_login(TraceInfo const &);

  void subscribe(State, std::string_view const &topic);

  void check_request_queue(TraceInfo const &);

  bool check_stale_orders();
  void check_stale_orders(TraceInfo const &);

  void parse(TraceInfo const &, std::string_view const &message);

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
  Request &request_;
  // state
  ConnectionStatus connection_status_ = {};
  core::Download2<State> download_;
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
  // ...
  utils::unordered_set<std::string> symbols_;
  // ...
  std::string encode_buffer_;
  //
  std::chrono::nanoseconds next_check_stale_orders_ = {};
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
