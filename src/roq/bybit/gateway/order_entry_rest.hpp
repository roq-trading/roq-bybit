/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/download_2.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/bybit/gateway/account.hpp"
#include "roq/bybit/gateway/request.hpp"
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
  struct Handler {};

  OrderEntryREST(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, Request &);

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

  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Connected> const &) override;
  void operator()(Trace<web::rest::Disconnected> const &) override;
  void operator()(Trace<web::rest::Latency> const &) override;

  // core::Download

  enum class State {
    UNDEFINED = 0,
    TEST_ORDER,
    DONE,
  };

  int32_t download(Trace<State> const &);

  // account-info

  void get_account_info(TraceInfo const &);
  void get_account_info_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::AccountInfoAck> const &);

  // wallet-balance

  void get_wallet_balance(TraceInfo const &);
  void get_wallet_balance_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::WalletBalanceAck> const &);

  // positions

  void get_positions(TraceInfo const &);
  void get_positions_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::PositionsAck> const &);

  // orders

  void get_orders(TraceInfo const &);
  void get_orders_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::OrdersAck> const &);

  // order

  void get_order(TraceInfo const &, std::string_view const &symbol, std::string_view const &client_order_id, std::string_view const &external_order_id);
  void get_order_ack(Trace<web::rest::Response> const &, std::string_view const &client_order_id);

  // executions

  void get_executions(TraceInfo const &);
  void get_executions_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::ExecutionsAck> const &);

  // place-order

  void place_order(Trace<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  void place_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::PlaceOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // amend-order

  void amend_order(
      Trace<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void amend_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::AmendOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-order

  void cancel_order(
      Trace<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void cancel_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::CancelOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-all-orders

  void cancel_all_orders(Trace<CancelAllOrders> const &, std::string_view const &request_id);
  void cancel_all_orders_ack(Trace<web::rest::Response> const &, std::string_view const &request_id);
  void operator()(Trace<protocol::json::CancelAllOrdersAck> const &);

  // helpers

  void send_test_order(TraceInfo const &);

  void check_request_queue(TraceInfo const &);

  void process_response(Trace<web::rest::Response> const &, auto error_handler, auto success_handler);

  void waf_limit_violation();

 private:
  [[maybe_unused]] Handler &handler_;
  //
  std::string const name_;
  std::unique_ptr<web::rest::Client> const connection_;
  //
  core::json::BufferStack decode_buffer_;
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
  Account &account_;
  Shared &shared_;
  Request &request_;
  core::Download2<State> download_;
  std::string encode_buffer_;
  size_t download_stale_orders_counter_ = {};
  bool download_trades_is_first_ = true;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
