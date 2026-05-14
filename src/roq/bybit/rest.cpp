/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bybit/rest.hpp"

#include <algorithm>
#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/bybit/json/map.hpp"
#include "roq/bybit/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace bybit {

// === TODO ===
// => use rate limiter / request queue
// => query instrument-info every N seconds

// === CONSTANTS ===

namespace {
auto const NAME = "rest"sv;

auto const SUPPORTS = Mask{
    SupportType::REFERENCE_DATA,
    SupportType::MARKET_STATUS,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 1;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.rest.uri;
  auto config = web::rest::Client::Config{
      // connection
      .interface = {},
      .proxy = settings.rest.proxy,
      .uris = {&uri, 1},
      .host = settings.rest.host,
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = {},
      .disconnect_on_idle_timeout = {},
      .connection = web::http::Connection::KEEP_ALIVE,
      // request
      .allow_pipelining = true,
      .request_timeout = settings.rest.request_timeout,
      // response
      .suspend_on_retry_after = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .ping_frequency = settings.rest.ping_freq,
      .ping_path = settings.rest.ping_path,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::rest::Client::create(handler, context, config);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};

auto create_rate_limiter(auto &settings) {
  return core::limit::RateLimiter{settings.request.limit, settings.request.limit_interval};
}
}  // namespace

// === IMPLEMENTATION ===

Rest::Rest(Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .instruments_info = create_metrics(shared.settings, name_, "instruments_info"sv),
          .instruments_info_ack = create_metrics(shared.settings, name_, "instruments_info_ack"sv),
          .kline = create_metrics(shared.settings, name_, "kline"sv),
          .kline_ack = create_metrics(shared.settings, name_, "kline_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared}, download_{shared.settings.rest.request_timeout, [this](auto state) { return download(state); }},
      rate_limiter{create_rate_limiter(shared.settings)} {
}

void Rest::operator()(Event<Start> const &) {
  (*connection_).start();
}

void Rest::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void Rest::operator()(Event<Timer> const &event) {
  auto &[message_info, timer] = event;
  (*connection_).refresh(timer.now);
  if (ready()) {
    check_request_queue(timer.now);
  }
}

void Rest::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.instruments_info, metrics::Type::PROFILE)
      .write(profile_.instruments_info_ack, metrics::Type::PROFILE)
      .write(profile_.kline, metrics::Type::PROFILE)
      .write(profile_.kline_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void Rest::operator()(ConnectionStatus connection_status, std::string_view const &reason) {
  connection_status_ = connection_status;
  TraceInfo trace_info;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = {},
      .supports = SUPPORTS,
      .transport = Transport::TCP,
      .protocol = Protocol::HTTP,
      .encoding = {Encoding::JSON},
      .priority = Priority::PRIMARY,
      .connection_status = connection_status_,
      .reason = reason,
      .interface = (*connection_).get_interface(),
      .authority = (*connection_).get_current_authority(),
      .path = (*connection_).get_current_path(),
      .proxy = (*connection_).get_proxy(),
  };
  log::info("stream_status={}"sv, stream_status);
  create_trace_and_dispatch(handler_, trace_info, stream_status);
}

void Rest::operator()(Trace<web::rest::Client::Connected> const &) {
  if (download_.downloading()) {
    download_.bump();
  } else {
    download_.begin();
  }
}

void Rest::operator()(Trace<web::rest::Client::Disconnected> const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  if (!download_.downloading()) {
    download_.reset();
  }
}

void Rest::operator()(Trace<web::rest::Client::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

uint32_t Rest::download(State state) {
  switch (state) {
    using enum State;
    case UNDEFINED:
      assert(false);
      break;
    case GET_INSTRUMENTS_INFO:
      (*this)(ConnectionStatus::DOWNLOADING, "get-instruments-info"sv);
      get_instruments_info();
      return 1;
    case DONE:
      (*this)(ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// instruments-info

void Rest::get_instruments_info() {
  profile_.instruments_info([&]() {
    auto query = fmt::format(
        "?category={}"
        "&status=Trading"
        "&limit=1000"sv,
        shared_.api.category.as_raw_text());
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.market_data.instruments_info,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, sequence = download_.sequence()]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_instruments_info_ack(event, sequence);
    };
    (*connection_)("market-instrument-info"sv, request, callback);
  });
}

void Rest::get_instruments_info_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const STATE = State::GET_INSTRUMENTS_INFO;
  profile_.instruments_info_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(STATE);
    };
    auto handle_success = [&](auto &body) {
      if (download_.skip(sequence, STATE)) {
        log::info("Download state={} has already been processed"sv, STATE);
      } else {
        json::InstrumentsInfoAck instruments_info_ack{body, decode_buffer_};
        if (instruments_info_ack.ret_code == 0) {
          Trace event_2{event, instruments_info_ack};
          (*this)(event_2);
          download_.check(STATE);
        } else {
          handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::map_error(instruments_info_ack.ret_code), instruments_info_ack.ret_msg);
        }
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void Rest::operator()(Trace<json::InstrumentsInfoAck> const &event) {
  auto &[trace_info, instruments_info_ack] = event;
  log::info<4>("instruments_info_ack={}"sv, instruments_info_ack);
  std::vector<Symbol> symbols;
  symbols.reserve(std::size(instruments_info_ack.result.list));  // alloc
  size_t counter = 0;
  for (auto &item : instruments_info_ack.result.list) {
    log::info<2>("item={}"sv, item);
    auto discard = shared_.discard_symbol(item.symbol);
    auto trade_vol_step_size = [&]() {
      if (shared_.api.category == json::Category::type_t::SPOT) {
        return item.lot_size_filter.base_precision;
      }
      return item.lot_size_filter.qty_step;
    }();
    auto reference_data = ReferenceData{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .description = item.display_name,
        .security_type = map(item.contract_type, item.options_type),
        .external_security_id = {},
        .cfi_code = {},
        .base_currency = item.base_coin,
        .quote_currency = item.quote_coin,
        .settlement_currency = item.settle_coin,
        .margin_currency = {},
        .commission_currency = {},
        .tick_size = item.price_filter.tick_size,
        .tick_size_steps = {},
        .multiplier = NaN,
        .min_notional = NaN,
        .min_trade_vol = item.lot_size_filter.min_order_qty,
        .max_trade_vol = item.lot_size_filter.max_order_qty,
        .trade_vol_step_size = trade_vol_step_size,
        .option_type = map(item.options_type),
        .strike_currency = {},
        .strike_price = NaN,
        .underlying = {},
        .time_zone = {},
        .issue_date = utils::safe_cast{item.launch_time},
        .settlement_date = {},
        .expiry_datetime = {},
        .expiry_datetime_utc = utils::safe_cast{item.delivery_time},
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = instruments_info_ack.time,
        .discard = discard,
    };
    create_trace_and_dispatch(handler_, trace_info, reference_data, true);
    if (discard) {
      log::info<1>(R"(Drop symbol="{}")"sv, item.symbol);
      continue;
    }
    if (shared_.all_symbols.emplace(item.symbol).second) {  // only include new
      symbols.emplace_back(item.symbol);
    }
    ++counter;
    auto market_status = MarketStatus{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .trading_status = map(item.status),
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = instruments_info_ack.time,
    };
    create_trace_and_dispatch(handler_, trace_info, market_status, true);
  }
  if (!std::empty(symbols)) {
    auto symbols_update = SymbolsUpdate{
        .symbols = symbols,
    };
    handler_(symbols_update);
  }
  if (counter > 0) {
    log::info("Symbols {} / {}"sv, counter, std::size(instruments_info_ack.result.list));
  }
}

// kline

// TODO paginate by splitting begin/end by interval and limit
void Rest::get_kline(std::string_view const &symbol) {
  profile_.kline([&]() {
    auto now = clock::get_realtime<std::chrono::milliseconds>();
    auto start = now - shared_.settings.time_series.lookback;
    auto query = fmt::format(
        "?category={}"
        "&symbol={}"
        "&interval=1"
        "&start={}"
        "&limit={}"sv,
        shared_.api.category.as_raw_text(),
        symbol,
        start.count(),
        shared_.settings.download.time_series_limit);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.market_data.kline,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, symbol = std::string{symbol}]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_kline_ack(event, symbol);
    };
    (*connection_)("kline"sv, request, callback);
  });
}

void Rest::get_kline_ack(Trace<web::rest::Response> const &event, [[maybe_unused]] std::string_view const &symbol) {
  profile_.instruments_info_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      // XXX FIXME TODO retry ???
    };
    auto handle_success = [&](auto &body) {
      json::KlineAck kline_ack{body, decode_buffer_};
      if (kline_ack.ret_code == 0) {
        assert(kline_ack.result.symbol == symbol);
        Trace event_2{event, kline_ack};
        (*this)(event_2);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::map_error(kline_ack.ret_code), kline_ack.ret_msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void Rest::operator()(Trace<json::KlineAck> const &event) {
  auto &[trace_info, kline_response] = event;
  auto &bars = shared_.bars;
  bars.clear();
  for (auto &item : kline_response.result.list) {
    auto bar = Bar{
        .begin_time_utc = utils::safe_cast(item.start_time),
        .confirmed = true,
        .open_price = item.open_price,
        .high_price = item.high_price,
        .low_price = item.low_price,
        .close_price = item.close_price,
        .quantity = item.volume,
        .base_amount = NaN,
        .quote_amount = item.turnover,
        .number_of_trades = {},
        .vwap = NaN,
    };
    bars.emplace_back(std::move(bar));
  }
  auto time_series_update = TimeSeriesUpdate{
      .stream_id = stream_id_,
      .exchange = shared_.settings.exchange,
      .symbol = kline_response.result.symbol,
      .data_source = DataSource::TRADE_SUMMARY,
      .interval = shared_.settings.time_series.interval,
      .origin = Origin::EXCHANGE,
      .bars = bars,
      .update_type = UpdateType::SNAPSHOT,
      .exchange_time_utc = kline_response.time,
  };
  create_trace_and_dispatch(handler_, trace_info, time_series_update, true);
}

// helpers

void Rest::check_request_queue(std::chrono::nanoseconds now) {
  auto can_request = [&](auto now) { return shared_.rate_limiter.can_request(now); };
  auto request = [&](auto &symbol) { get_kline(symbol); };
  shared_.time_series_request_queue.dispatch(can_request, request, now);
}

void Rest::process_response(web::rest::Response const &response, auto error_handler, auto success_handler) {
  try {
    auto [status, category, body] = response.result();
    switch (category) {
      using enum web::http::Category;
      case UNKNOWN:
      case INFORMATIONAL_RESPONSE:
        response.expect(web::http::Status::OK);  // throws
        break;
      case SUCCESS:
        success_handler(body);
        break;
      case REDIRECTION:
        log::fatal("Unexpected: URL is being redirected"sv);
      case CLIENT_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
      case SERVER_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
    }
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

}  // namespace bybit
}  // namespace roq
