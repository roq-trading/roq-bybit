set(TARGET_NAME "${PROJECT_NAME}-json")

set(SOURCES parser.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    account_info.json
    account_info_result.json
    position_info.json
    position_info_result.json
    wallet_balance.json
    wallet_balance_result_item.json
    wallet_balance_result.json
    wallet.json
    open_orders.json
    open_orders_result_item.json
    open_orders_result.json
    execution.json
    execution_result_item.json
    execution_result.json
    order_book_data.json
    order_book.json
    order_book_level.json
    order_data_item.json
    order.json
    order_status.json
    order_type.json
    side.json
    tickers_data.json
    tickers.json
    time_in_force.json
    public_trade_data.json
    public_trade.json
    instrument_info.json
    instrument_info_result.json
    instrument_info_result_item.json
    price_filter.json
    leverage_filter.json
    lot_size_filter.json
    category.json
    exec_type.json
    stop_order_type.json
    cancel_type.json
    tick_direction.json
    position_status.json
    reject_reason.json
    account_type.json
    trigger_by.json
    status.json
    contract_type.json
    options_type.json
    auth.json
    error.json
    pong.json
    subscribe.json
    subscribe_data.json
    cancel_order.json
    cancel_order_result.json
    cancel_orders.json
    cancel_orders_result.json
    create_order.json
    create_order_result.json
    event_type.json
    message_field.json
    operation.json
    topic.json
    ticket_info_data_item.json
    ticket_info.json)

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
