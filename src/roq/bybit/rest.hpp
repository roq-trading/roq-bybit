/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "roq/utils/container.hpp"

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/core/limit/rate_limiter.hpp"

#include "roq/bybit/rest_state.hpp"
#include "roq/bybit/shared.hpp"

#include "roq/bybit/json/instruments_info_ack.hpp"
#include "roq/bybit/json/kline_ack.hpp"

namespace roq {
namespace bybit {

struct Rest final : public web::rest::Client::Handler {
  struct SymbolsUpdate final {
    std::vector<Symbol> &symbols;
  };

  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<ReferenceData> const &, bool is_last) = 0;
    virtual void operator()(Trace<MarketStatus> const &, bool is_last) = 0;
    virtual void operator()(Trace<TimeSeriesUpdate> const &, bool is_last) = 0;
    // cross-communication
    virtual void operator()(SymbolsUpdate &) = 0;
  };

  Rest(Handler &, io::Context &context, uint16_t stream_id, Shared &);

  Rest(Rest const &) = delete;

  bool ready() const { return status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &) const;

 protected:
  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Client::Connected> const &) override;
  void operator()(Trace<web::rest::Client::Disconnected> const &) override;
  void operator()(Trace<web::rest::Client::Latency> const &) override;

  void operator()(ConnectionStatus);

  uint32_t download(RestState);

  // instruments-info

  void get_instruments_info();
  void get_instruments_info_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<json::InstrumentsInfoAck> const &);

  // kline

  void get_kline(std::string_view const &symbol);
  void get_kline_ack(Trace<web::rest::Response> const &, std::string_view const &symbol);
  void operator()(Trace<json::KlineAck> const &);

  // helpers

  void check_request_queue(std::chrono::nanoseconds now);

  void process_response(web::rest::Response const &, auto error_handler, auto success_handler);

 private:
  Handler &handler_;
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
    utils::metrics::Profile instruments_info, instruments_info_ack, kline, kline_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // cache
  Shared &shared_;
  utils::unordered_set<std::string> symbols_;
  // state
  ConnectionStatus status_ = {};
  core::Download<RestState> download_;
  // ...
  core::limit::RateLimiter rate_limiter;
};

}  // namespace bybit
}  // namespace roq
