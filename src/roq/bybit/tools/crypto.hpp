/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string>
#include <string_view>

#include <roq/utils/hash/sha256.hpp>

#include "roq/utils/mac/hmac.hpp"

namespace roq {
namespace bybit {
namespace tools {

struct Crypto final {
  Crypto(std::string_view const &key, std::string_view const &secret, std::chrono::milliseconds recv_window);

  Crypto(Crypto &&) = delete;
  Crypto(Crypto const &) = delete;

  auto const &get_key() const { return key_; }

  std::string create_signature_v2(std::chrono::milliseconds expires);

  std::string create_headers_v2(std::string_view const &path, std::string_view const &query, std::string_view const &body, std::chrono::milliseconds timestamp);

 private:
  using MAC = utils::mac::HMAC<utils::hash::SHA256>;
  using Digest = std::array<std::byte, MAC::DIGEST_LENGTH>;

  std::string const key_;
  MAC mac_;
  Digest digest_;
  std::string const passphrase_;
  std::string const signed_passphrase_;
  std::chrono::milliseconds const recv_window_;
};

}  // namespace tools
}  // namespace bybit
}  // namespace roq
