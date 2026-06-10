/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/io/web/uri.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/bybit/gateway/shared.hpp"

#include "roq/bybit/protocol/json/category.hpp"
#include "roq/bybit/protocol/json/parser.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct MarketData final : public web::socket::Client::Handler, public protocol::json::Parser::Handler {
  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<MarketStatus> const &, bool is_last) = 0;
    virtual void operator()(Trace<TopOfBook> const &, bool is_last) = 0;
    virtual void operator()(Trace<MarketByPriceUpdate> const &, bool is_last) = 0;
    virtual void operator()(Trace<TradeSummary> const &, bool is_last) = 0;
    virtual void operator()(Trace<StatisticsUpdate> const &, bool is_last) = 0;
    virtual void operator()(Trace<TimeSeriesUpdate> const &, bool is_last) = 0;
  };

  MarketData(Handler &, io::Context &, uint16_t stream_id, Shared &, size_t index);

  MarketData(MarketData const &) = delete;

  uint16_t stream_id() const { return stream_id_; }

  bool ready() const { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &) const;

  void subscribe(size_t start_from = 0);

 protected:
  // web::socket::Client::Handler

  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

 private:
  void operator()(ConnectionStatus, std::string_view const &reason = {});

  void subscribe(std::span<Symbol const> const &symbols);
  void subscribe(std::string_view const &topic, std::span<Symbol const> const &symbols);
  void subscribe(std::string_view const &topic, std::span<Symbol const> const &symbols, std::chrono::minutes interval);

  void send_ping(std::chrono::nanoseconds now);

  void parse(std::string_view const &message);

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

  Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  size_t const index_;
  std::chrono::nanoseconds const ping_frequency_;
  bool const spot_;
  size_t const mbp_depth_;
  std::string const mbp_topic_;
  // web socket
  std::unique_ptr<web::socket::Client> const connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // session
  uint64_t request_id_ = {};
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse, order_book, trade, tickers, kline;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus connection_status_ = {};
  // ping
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
