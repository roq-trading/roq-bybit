/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "roq/core/download.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/bybit/account.hpp"
#include "roq/bybit/order_entry.hpp"
#include "roq/bybit/order_entry_state.hpp"
#include "roq/bybit/shared.hpp"

#include "roq/bybit/json/account_info_ack.hpp"
#include "roq/bybit/json/executions_ack.hpp"
#include "roq/bybit/json/orders_ack.hpp"
#include "roq/bybit/json/positions_ack.hpp"
#include "roq/bybit/json/wallet_balance_ack.hpp"

#include "roq/bybit/json/amend_order_ack.hpp"
#include "roq/bybit/json/cancel_all_orders_ack.hpp"
#include "roq/bybit/json/cancel_order_ack.hpp"
#include "roq/bybit/json/place_order_ack.hpp"

namespace roq {
namespace bybit {

struct OrderEntryREST final : public OrderEntry, public web::rest::Client::Handler {
  OrderEntryREST(OrderEntry::Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

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

  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Client::Connected> const &) override;
  void operator()(Trace<web::rest::Client::Disconnected> const &) override;
  void operator()(Trace<web::rest::Client::Latency> const &) override;

 private:
  bool ready() const { return status_ == ConnectionStatus::READY; }

  void operator()(ConnectionStatus);

  uint32_t download(OrderEntryState state);

  void check_request_queue(std::chrono::nanoseconds now);

  // account-info

  void get_account_info();
  void get_account_info_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<json::AccountInfoAck> const &);

  // wallet-balance

  void get_wallet_balance();
  void get_wallet_balance_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<json::WalletBalanceAck> const &);

  // positions

  void get_positions(std::string_view const &symbol);
  void get_positions_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<json::PositionsAck> const &);

  // orders

  void get_orders(std::string_view const &symbol);
  void get_orders_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<json::OrdersAck> const &);

  // executions

  void get_executions(std::string_view const &symbol);
  void get_executions_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<json::ExecutionsAck> const &);

  // place-order

  void place_order(Event<CreateOrder> const &, server::oms::Order const &, std::string_view const &request_id);
  void place_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::PlaceOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // amend-order

  void amend_order(Event<ModifyOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id);
  void amend_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::AmendOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-order

  void cancel_order(Event<CancelOrder> const &, server::oms::Order const &, std::string_view const &request_id, std::string_view const &previous_request_id);
  void cancel_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<json::CancelOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-all-orders

  void cancel_all_orders(Event<CancelAllOrders> const &, std::string_view const &request_id);
  void cancel_all_orders_ack(Trace<web::rest::Response> const &, std::string_view const &request_id);
  void operator()(Trace<json::CancelAllOrdersAck> const &);

  // helpers

  void process_response(web::rest::Response const &, auto error_handler, auto success_handler);

  template <typename... Args>
  void operator()(Trace<server::oms::Response> const &, uint8_t user_id, uint64_t order_id, Args &&...);

  void operator()(Trace<server::oms::OrderUpdate> const &, std::string_view const &client_order_id);

  void waf_limit_violation();

 private:
  OrderEntry::Handler &handler_;
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
  ConnectionStatus status_ = {};
  core::Download<OrderEntryState> download_;
  bool download_trades_is_first_ = true;
  //
  std::string encode_buffer_;
};

}  // namespace bybit
}  // namespace roq
