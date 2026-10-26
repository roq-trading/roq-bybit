/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/core/limit/request.hpp"

#include "roq/bybit/gateway/shared.hpp"

namespace roq {
namespace bybit {
namespace gateway {

struct Request final : public core::limit::Request::Handler {
  explicit Request(Shared &);

  // core::Request::Handler

  bool ready() const override;
  bool can_request(TraceInfo const &) override;

  // ...

  core::limit::Request account;
  core::limit::Request wallet;
  core::limit::Request positions;
  core::limit::Request orders;
  core::limit::Request executions;

 private:
  Shared &shared_;
};

}  // namespace gateway
}  // namespace bybit
}  // namespace roq
