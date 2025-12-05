/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::bybit;

using namespace std::literals;

using value_type = json::Wallet;

TEST_CASE("parser", "[json_wallet]") {
  auto message = R"({)"
                 R"("id":"460579-2-c4815897-f283-4ccd-b4d7-dac6bde51502-1682339173119",)"
                 R"("topic":"wallet",)"
                 R"("creationTime":1682339173118,)"
                 R"("data":[{)"
                 R"("accountType":"SPOT",)"
                 R"("accountIMRate":"",)"
                 R"("accountMMRate":"",)"
                 R"("accountLTV":"",)"
                 R"("totalEquity":"",)"
                 R"("totalWalletBalance":"",)"
                 R"("totalMarginBalance":"",)"
                 R"("totalAvailableBalance":"",)"
                 R"("totalPerpUPL":"",)"
                 R"("totalInitialMargin":"",)"
                 R"("totalMaintenanceMargin":"",)"
                 R"("coin":[{)"
                 R"("coin":"USDT",)"
                 R"("equity":"",)"
                 R"("usdValue":"",)"
                 R"("walletBalance":"48855.56",)"
                 R"("free":"48855.56",)"
                 R"("locked":"0",)"
                 R"("availableToWithdraw":"",)"
                 R"("availableToBorrow":"",)"
                 R"("borrowAmount":"",)"
                 R"("accruedInterest":"",)"
                 R"("totalOrderIM":"",)"
                 R"("totalPositionIM":"",)"
                 R"("totalPositionMM":"",)"
                 R"("unrealisedPnl":"",)"
                 R"("cumRealisedPnl":"")"
                 R"(})"
                 R"(])"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [](value_type const &obj) {
    CHECK(obj.id == "460579-2-c4815897-f283-4ccd-b4d7-dac6bde51502-1682339173119"sv);
    CHECK(obj.topic == "wallet"sv);
    CHECK(obj.creation_time == 1682339173118ms);
    REQUIRE(std::size(obj.data) == 1);
    CHECK(obj.data[0].account_type == json::AccountType::SPOT);
    REQUIRE(std::size(obj.data[0].coin) == 1);
    CHECK(obj.data[0].coin[0].coin == "USDT"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("parser_2", "[json_wallet]") {
  auto message = R"({)"
                 R"("id":"460579-3-a5da51e6-2a7e-4dc6-9546-d85721fb0d1b-1682341631264",)"
                 R"("topic":"wallet",)"
                 R"("creationTime":1682341631264,)"
                 R"("data":[{)"
                 R"("accountType":"SPOT",)"
                 R"("accountIMRate":"",)"
                 R"("accountMMRate":"",)"
                 R"("accountLTV":"",)"
                 R"("totalEquity":"",)"
                 R"("totalWalletBalance":"",)"
                 R"("totalMarginBalance":"",)"
                 R"("totalAvailableBalance":"",)"
                 R"("totalPerpUPL":"",)"
                 R"("totalInitialMargin":"",)"
                 R"("totalMaintenanceMargin":"",)"
                 R"("coin":[{)"
                 R"("coin":"USDT",)"
                 R"("equity":"",)"
                 R"("usdValue":"",)"
                 R"("walletBalance":"48855.56",)"
                 R"("free":"48828.5061",)"
                 R"("locked":"27.0539",)"
                 R"("availableToWithdraw":"",)"
                 R"("availableToBorrow":"",)"
                 R"("borrowAmount":"",)"
                 R"("accruedInterest":"",)"
                 R"("totalOrderIM":"",)"
                 R"("totalPositionIM":"",)"
                 R"("totalPositionMM":"",)"
                 R"("unrealisedPnl":"",)"
                 R"("cumRealisedPnl":"")"
                 R"(})"
                 R"(])"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.id == "460579-3-a5da51e6-2a7e-4dc6-9546-d85721fb0d1b-1682341631264"sv);
    CHECK(obj.topic == "wallet"sv);
    CHECK(obj.creation_time == 1682341631264ms);
    REQUIRE(std::size(obj.data) == 1);
    CHECK(obj.data[0].account_type == json::AccountType::SPOT);
    REQUIRE(std::size(obj.data[0].coin) == 1);
    CHECK(obj.data[0].coin[0].coin == "USDT"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
