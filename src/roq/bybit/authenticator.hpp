/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/bybit/config.hpp"

#include "roq/bybit/tools/crypto.hpp"

namespace roq {
namespace bybit {

struct Authenticator final {
  Authenticator(Config const &, std::string_view const &account);

  Authenticator(Authenticator &&) = delete;
  Authenticator(Authenticator const &) = delete;

  std::string_view get_account() const { return account_; }
  std::string_view get_key() const { return crypto_.get_key(); }

  std::string create_signature(std::chrono::milliseconds expires);
  std::string create_headers(std::string_view const &path, std::string_view const &query, std::string_view const &body);

 private:
  std::string const account_;
  tools::Crypto crypto_;
};

}  // namespace bybit
}  // namespace roq
