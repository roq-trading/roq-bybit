/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/core/download_2.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/bybit/gateway/account.hpp"
#include "roq/bybit/gateway/shared.hpp"

#include "roq/bybit/protocol/json/account_info_ack.hpp"
#include "roq/bybit/protocol/json/executions_ack.hpp"
#include "roq/bybit/protocol/json/orders_ack.hpp"
#include "roq/bybit/protocol/json/positions_ack.hpp"
#include "roq/bybit/protocol/json/wallet_balance_ack.hpp"

#include "roq/bybit/protocol/json/amend_order_ack.hpp"
#include "roq/bybit/protocol/json/cancel_all_orders_ack.hpp"
#include "roq/bybit/protocol/json/cancel_order_ack.hpp"
#include "roq/bybit/protocol/json/place_order_ack.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct OrderEntryREST final : public Base<OrderEntryREST>, public server::OrderActionStream, public web::rest::Client::Handler {
  struct Response final {
    std::string_view account;
    std::string_view topic;
    std::string_view symbol;
  };

  struct Handler {
    virtual void operator()(Trace<Response> const &) = 0;
  };

  OrderEntryREST(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &) override;
  void operator()(Event<Stop> const &) override;
  void operator()(Event<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

  // server::OrderActionStream

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) override;

  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Connected> const &) override;
  void operator()(Trace<web::rest::Disconnected> const &) override;
  void operator()(Trace<web::rest::Latency> const &) override;

  // core::Download

  enum class State {
    UNDEFINED = 0,
    ACCOUNT_INFO,
    DONE,
  };

  int32_t download(Trace<State> const &);

  // account-info

  void get_account_info();
  void get_account_info_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::AccountInfoAck> const &);

  // wallet-balance

  void get_wallet_balance();
  void get_wallet_balance_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::WalletBalanceAck> const &);

  // positions

  void get_positions(std::string_view const &symbol);
  void get_positions_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<protocol::json::PositionsAck> const &);

  // orders

  void get_orders(std::string_view const &symbol);
  void get_orders_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<protocol::json::OrdersAck> const &);

  // executions

  void get_executions(std::string_view const &symbol);
  void get_executions_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<protocol::json::ExecutionsAck> const &);

  // place-order

  void place_order(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  void place_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::PlaceOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // amend-order

  void amend_order(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void amend_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::AmendOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-order

  void cancel_order(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void cancel_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::CancelOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-all-orders

  void cancel_all_orders(Event<CancelAllOrders> const &, std::string_view const &request_id);
  void cancel_all_orders_ack(Trace<web::rest::Response> const &, std::string_view const &request_id);
  void operator()(Trace<protocol::json::CancelAllOrdersAck> const &);

  // helpers

  void check_request_queue(std::chrono::nanoseconds now);

  void process_response(Trace<web::rest::Response> const &, auto error_handler, auto success_handler);

  void waf_limit_violation();

 private:
  [[maybe_unused]] Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // connection
  std::unique_ptr<web::rest::Client> const connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile  //
        account_info,
        account_info_ack,                    //
        wallet_balance, wallet_balance_ack,  //
        position_info, position_info_ack,    //
        open_orders, open_orders_ack,        //
        execution, execution_ack,            //
        place_order, place_order_ack,        //
        amend_order, amend_order_ack,        //
        cancel_order, cancel_order_ack,      //
        cancel_all_orders, cancel_all_orders_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus connection_status_ = {};
  core::Download2<State> download_;
  bool download_trades_is_first_ = true;
  //
  std::string encode_buffer_;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
