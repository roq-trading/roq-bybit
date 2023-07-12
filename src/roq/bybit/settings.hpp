/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <fmt/compile.h>
#include <fmt/format.h>

#include "roq/server/flags/settings.hpp"

#include "roq/bybit/flags/common.hpp"
#include "roq/bybit/flags/rest.hpp"
#include "roq/bybit/flags/ws.hpp"

namespace roq {
namespace bybit {

struct Settings final : public server::flags::Settings {
  explicit Settings(args::Parser const &);

  std::string_view exchange;

  flags::Common__flags common;
  flags::REST__flags rest;
  flags::WS__flags ws;
};

}  // namespace bybit
}  // namespace roq

template <>
struct fmt::formatter<roq::bybit::Settings> {
  template <typename Context>
  constexpr auto parse(Context &context) {
    return std::begin(context);
  }
  template <typename Context>
  auto format(roq::bybit::Settings const &value, Context &context) const {
    using namespace fmt::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(exchange="{}", )"
        R"(common={}, )"
        R"(rest={}, )"
        R"(ws={}, )"
        R"(server={})"
        R"(}})"_cf,
        value.exchange,
        value.common,
        value.rest,
        value.ws,
        static_cast<roq::server::Settings const &>(value));
  }
};
