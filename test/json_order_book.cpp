/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/parser.hpp"

#include "roq/bybit/json/order_book.hpp"
#include "roq/bybit/json/parser.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;
using namespace std::chrono_literals;

namespace {
auto const MESSAGE = R"({)"
                     R"("data":{)"
                     R"("s":"ETHUSDT",)"
                     R"("t":1669300793109,)"
                     R"("b":[)"
                     R"(["1194.32","0.838"],)"
                     R"(["1194.31","2.80252"],)"
                     R"(["1194.16","1.6748"],)"
                     R"(["1194.15","4.84647"],)"
                     R"(["1194.12","0.09958"],)"
                     R"(["1194.11","2.50191"],)"
                     R"(["1194.1","12.77"],)"
                     R"(["1194.09","3.687"],)"
                     R"(["1194.07","2.1846"],)"
                     R"(["1194.05","5.357"],)"
                     R"(["1193.9","2.504"],)"
                     R"(["1193.88","3.17605"],)"
                     R"(["1193.87","2.46613"],)"
                     R"(["1193.85","3.082"],)"
                     R"(["1193.83","2.48409"],)"
                     R"(["1193.79","25.14066"],)"
                     R"(["1193.74","4.194"],)"
                     R"(["1193.73","1.98847"],)"
                     R"(["1193.72","0.26133"],)"
                     R"(["1193.57","2.24709"],)"
                     R"(["1193.5","26.28"],)"
                     R"(["1193.44","2.42124"],)"
                     R"(["1193.36","1.7238"],)"
                     R"(["1193.32","2.29199"],)"
                     R"(["1193.18","0.02987"],)"
                     R"(["1193.12","0.87397"],)"
                     R"(["1193.11","6.96596"],)"
                     R"(["1193.09","2.32319"],)"
                     R"(["1193.08","8.38116"],)"
                     R"(["1193.04","19.25507"],)"
                     R"(["1192.95","4.01677"],)"
                     R"(["1192.94","2.25414"],)"
                     R"(["1192.89","24.11479"],)"
                     R"(["1192.8","22.72859"],)"
                     R"(["1192.79","0.02625"],)"
                     R"(["1192.78","2.46136"],)"
                     R"(["1192.75","10.56555"],)"
                     R"(["1192.74","3.97758"],)"
                     R"(["1192.71","21.25987"],)"
                     R"(["1192.68","21.92825"]],)"
                     R"("a":[)"
                     R"(["1194.64","1.26328"],)"
                     R"(["1194.93","2.57053"],)"
                     R"(["1194.95","0.838"],)"
                     R"(["1194.98","0.08069"],)"
                     R"(["1194.99","2"],)"
                     R"(["1195","2.41386"],)"
                     R"(["1195.07","0.13986"],)"
                     R"(["1195.1","0.43123"],)"
                     R"(["1195.14","2.504"],)"
                     R"(["1195.15","2.45951"],)"
                     R"(["1195.16","2.36976"],)"
                     R"(["1195.2","2.89791"],)"
                     R"(["1195.27","1.10317"],)"
                     R"(["1195.3","0.02397"],)"
                     R"(["1195.33","3.78169"],)"
                     R"(["1195.34","4.68788"],)"
                     R"(["1195.35","2.24709"],)"
                     R"(["1195.36","1.71895"],)"
                     R"(["1195.44","5.358"],)"
                     R"(["1195.47","25.14066"],)"
                     R"(["1195.5","0.01157"],)"
                     R"(["1195.55","2.21669"],)"
                     R"(["1195.6","0.05053"],)"
                     R"(["1195.63","2.18455"],)"
                     R"(["1195.64","14.76922"],)"
                     R"(["1195.65","0.02624"],)"
                     R"(["1195.7","2.37219"],)"
                     R"(["1195.77","0.00382"],)"
                     R"(["1195.88","8.36194"],)"
                     R"(["1195.9","0.01205"],)"
                     R"(["1195.92","2.41012"],)"
                     R"(["1195.96","0.01107"],)"
                     R"(["1196","3.18374"],)"
                     R"(["1196.02","0.00346"],)"
                     R"(["1196.05","0.0066"],)"
                     R"(["1196.08","0.01464"],)"
                     R"(["1196.1","10.68012"],)"
                     R"(["1196.14","2.22548"],)"
                     R"(["1196.16","26.28"],)"
                     R"(["1196.18", "37.99"])"
                     R"(])"
                     R"(},)"
                     R"("type":"delta",)"
                     R"("topic":"orderbook.40.ETHUSDT",)"
                     R"("ts":1669300793111)"
                     R"(})"sv;
}  // namespace

TEST_CASE("json_order_book_simple", "[json_order_book]") {
  core::Buffer buffer(8192);
  json::OrderBook order_book{MESSAGE, buffer};
}

TEST_CASE("json_order_book_parser", "[json_order_book]") {
  struct Handler final : public json::Parser::Handler {
    void operator()(Trace<json::Error> const &) override { FAIL(); }
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Subscribe> const &) override { FAIL(); }
    // public
    void operator()(Trace<json::OrderBook> const &, [[maybe_unused]] size_t depth) override { found = true; }
    void operator()(Trace<json::Trade> const &) override { FAIL(); }
    void operator()(Trace<json::Tickers> const &) override { FAIL(); }
    // private
    void operator()(Trace<json::Auth> const &) override { FAIL(); }
    void operator()(Trace<json::OutboundAccountInfo> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::TicketInfo> const &) override { FAIL(); }

    bool found = false;
  } handler;
  core::Buffer buffer(8192);
  core::json::Buffer buffer_2{buffer};
  auto res = json::Parser::dispatch(handler, MESSAGE, buffer_2, {});
  CHECK(res == true);
  CHECK(handler.found == true);
}
