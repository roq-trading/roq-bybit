/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/core/download.hpp"

#include "roq/core/metrics/counter.hpp"
#include "roq/core/metrics/latency.hpp"
#include "roq/core/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/server.hpp"

#include "roq/bybit/authenticator.hpp"
#include "roq/bybit/drop_copy_state.hpp"
#include "roq/bybit/shared.hpp"

#include "roq/bybit/json/parser.hpp"

namespace roq {
namespace bybit {

struct DropCopy final : public web::socket::Client::Handler, json::Parser::Handler {
  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<oms::TradeUpdate> const &, uint16_t stream_id, bool is_last, uint8_t user_id) = 0;
    virtual void operator()(Trace<FundsUpdate> const &, bool is_last) = 0;
  };

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Authenticator &, Shared &);

  DropCopy(DropCopy &&) = delete;
  DropCopy(DropCopy const &) = delete;

  bool ready() const;

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &);

 protected:
  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

  void operator()(Trace<json::Error> const &) override;
  void operator()(Trace<json::Pong> const &) override;
  void operator()(Trace<json::Subscribe> const &) override;
  // public
  void operator()(Trace<json::OrderBook> const &, size_t depth) override;
  void operator()(Trace<json::Trade> const &) override;
  void operator()(Trace<json::Tickers> const &) override;
  // private
  void operator()(Trace<json::Auth> const &) override;
  void operator()(Trace<json::OutboundAccountInfo> const &) override;
  void operator()(Trace<json::Order> const &) override;
  void operator()(Trace<json::TicketInfo> const &) override;

 private:
  void operator()(ConnectionStatus);

  uint32_t download(DropCopyState);

  void subscribe();

  void subscribe(std::string_view const &topic);

  void parse(std::string_view const &message);

 private:
  Handler &handler_;
  // config
  const uint16_t stream_id_;
  const std::string name_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::Buffer decode_buffer_;
  // session
  uint64_t request_id_ = {};
  // metrics
  struct {
    core::metrics::Counter disconnect;
  } counter_;
  struct {
    core::metrics::Profile parse, auth, outbound_account_info, order, ticket_info;
  } profile_;
  struct {
    core::metrics::Latency ping, heartbeat;
  } latency_;
  // authenticator
  Authenticator &authenticator_;
  // cache
  Shared &shared_;
  // state
  bool welcome_ = false;
  bool ready_ = false;
  ConnectionStatus status_ = {};
  core::Download<DropCopyState> download_;
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace bybit
}  // namespace roq
