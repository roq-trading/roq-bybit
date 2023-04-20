set(TARGET_NAME "${PROJECT_NAME}-test")

set(SOURCES
    json_account.cpp
    json_auth.cpp
    json_book_ticker.cpp
    json_cancel_order.cpp
    json_cancel_orders.cpp
    json_create_order.cpp
    json_error.cpp
    json_order_book.cpp
    json_order.cpp
    json_orders.cpp
    json_outbound_account_info.cpp
    json_pong.cpp
    json_subscribe.cpp
    json_symbols.cpp
    json_tickers.cpp
    json_ticket_info.cpp
    json_trade.cpp
    json_trades.cpp
    main.cpp)

roq_gitignore(OUTPUT ".gitignore" SOURCES "${TARGET_NAME}")

add_executable("${TARGET_NAME}" ${SOURCES})

target_link_libraries(
  "${TARGET_NAME}"
  "${PROJECT_NAME}-json"
  roq-io::roq-io
  roq-logging::roq-logging
  aklomp::base64
  Catch2::Catch2
  ${RT_LIBRARIES})

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s)

# run

add_test(NAME "${TARGET_NAME}" COMMAND "${TARGET_NAME}")
