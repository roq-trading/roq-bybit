set(TARGET_NAME "${PROJECT_NAME}-json")

set(SOURCES parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    account.json
    account_result_item.json
    account_result.json
    auth.json
    book_ticker_data.json
    book_ticker.json
    cancel_order.json
    cancel_order_result.json
    cancel_orders.json
    cancel_orders_result.json
    create_order.json
    create_order_result.json
    error.json
    event_type.json
    liquidation.json
    message_field.json
    operation.json
    operation.json
    order_book_data.json
    order_book.json
    order_book_level.json
    order_data_item.json
    order.json
    orders.json
    orders_result_item.json
    orders_result.json
    order_status.json
    order_type.json
    outbound_account_info_data_item.json
    outbound_account_info_data_item_item.json
    outbound_account_info.json
    pong.json
    side.json
    subscribe.json
    tickers_data.json
    tickers.json
    ticket_info_data_item.json
    ticket_info.json
    time_in_force.json
    topic.json
    trade_data.json
    trade.json
    trades.json
    trades_result_item.json
    trades_result.json
    market_info.json
    market_info_result.json
    market_info_result_item.json
    price_filter.json
    leverage_filter.json
    lot_size_filter.json
    contract_type.json
    options_type.json
    status.json)

roq_autogen(
  OUTPUT
  AUTOGEN_HEADERS
  NAMESPACE
  "roq/bybit/json"
  OUTPUT_TYPE
  "json"
  FILE_TYPE
  "hpp"
  SOURCES
  ${AUTOGEN_SCHEMAS})

add_custom_target(autogen-json-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen(
  OUTPUT
  AUTOGEN_SOURCES
  NAMESPACE
  "roq/bybit/json"
  OUTPUT_TYPE
  "json"
  FILE_TYPE
  "cpp"
  SOURCES
  ${AUTOGEN_SCHEMAS})

roq_gitignore(OUTPUT ".gitignore" SOURCES "${TARGET_NAME}" ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library("${TARGET_NAME}" OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies("${TARGET_NAME}" autogen-json-headers)

target_link_libraries("${TARGET_NAME}" roq-core::roq-core roq-logging::roq-logging fmt::fmt)
