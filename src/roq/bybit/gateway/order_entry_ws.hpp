/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

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
#include "roq/bybit/gateway/shared.hpp"

#include "roq/bybit/protocol/json/parser_2.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct OrderEntryWS final : public Base<OrderEntryWS>, public server::OrderActionStream, public web::socket::Client::Handler, protocol::json::Parser2::Handler {
  struct Handler {};

  OrderEntryWS(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

 protected:
  friend base_type;

  // server::Stream

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

  // server::OrderActionStream

  void operator()(Trace<server::oms::OrderActionBatchBegin> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchCommit> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchRollback> const &) override;

  void operator()(Trace<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  void operator()(
      Trace<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  void operator()(
      Trace<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  void operator()(Trace<CancelAllOrders> const &, std::string_view const &request_id) override;

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
    TEST_ORDER,
    DONE,
  };

  int32_t download(Trace<State> const &);

  // protocol::json::Parser2::Handler

  void operator()(Trace<protocol::json::Ping> const &) override;
  void operator()(Trace<protocol::json::Pong> const &) override;
  void operator()(Trace<protocol::json::Auth2> const &) override;
  void operator()(Trace<protocol::json::Error> const &) override;
  void operator()(Trace<protocol::json::PlaceOrder2> const &) override;
  void operator()(Trace<protocol::json::AmendOrder2> const &) override;
  void operator()(Trace<protocol::json::CancelOrder2> const &) override;

  // helpers

  void send_login(TraceInfo const &);

  void check_timeout(TraceInfo const &);

  void send_test_order(TraceInfo const &);

  void send_ping(TraceInfo const &);

  void parse(TraceInfo const &, std::string_view const &message);

 private:
  [[maybe_unused]] Handler &handler_;
  //
  std::string const name_;
  std::unique_ptr<web::socket::Client> const connection_;
  //
  core::json::BufferStack decode_buffer_;
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
  Account &account_;
  Shared &shared_;
  core::Download2<State> download_;
  std::string encode_buffer_;
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
