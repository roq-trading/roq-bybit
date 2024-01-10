/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/server/flags/settings.hpp"

#include "roq/bybit/flags/common.hpp"
#include "roq/bybit/flags/flags.hpp"
#include "roq/bybit/flags/rest.hpp"
#include "roq/bybit/flags/ws.hpp"

namespace roq {
namespace bybit {

struct Settings final : public server::flags::Settings {
  explicit Settings(args::Parser const &);

  std::string_view exchange;

  flags::Common common;
  flags::REST rest;
  flags::WS ws;

 private:
  Settings(args::Parser const &, flags::Flags const &);
};

}  // namespace bybit
}  // namespace roq

template <>
struct fmt::formatter<roq::bybit::Settings> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::bybit::Settings const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(exchange="{}", )"
        R"(common={}, )"
        R"(rest={}, )"
        R"(ws={}, )"
        R"(server={})"
        R"(}})"sv,
        value.exchange,
        value.common,
        value.rest,
        value.ws,
        static_cast<roq::server::Settings const &>(value));
  }
};
