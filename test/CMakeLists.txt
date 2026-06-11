include(RoqAutogen)

set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES
    json_account_info_ack.cpp
    json_amend_order_2.cpp
    json_amend_order_ack.cpp
    json_auth_2.cpp
    json_auth.cpp
    json_cancel_all_orders_ack.cpp
    json_cancel_order_2.cpp
    json_cancel_order_ack.cpp
    json_encoder.cpp
    json_error.cpp
    json_execution_2.cpp
    json_executions_ack.cpp
    json_instruments_info_ack.cpp
    json_order_book.cpp
    json_order.cpp
    json_orders_ack.cpp
    json_ping.cpp
    json_place_order_2.cpp
    json_place_order_ack.cpp
    json_position.cpp
    json_positions_ack.cpp
    json_public_trade.cpp
    json_subscribe.cpp
    json_tickers.cpp
    json_wallet_balance_ack.cpp
    json_wallet.cpp
    main.cpp)

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME}-protocol-json roq-server::roq-server Catch2::Catch2)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
