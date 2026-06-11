set(TARGET_NAME ${PROJECT_NAME}-protocol-json)

set(SOURCES encoder.cpp map.cpp parser.cpp parser_2.cpp utils.cpp)

include(RoqAutogen)

set(AUTOGEN_SCHEMAS
    account_info_ack.json
    account_info_ack_result.json
    account_type.json
    amend_order2_data.json
    amend_order2.json
    amend_order_ack.json
    amend_order_ack_result.json
    auth2.json
    auth.json
    cancel_all_orders_ack.json
    cancel_all_orders_ack_result.json
    cancel_all_orders_ack_result_list_item.json
    cancel_order2_data.json
    cancel_order2.json
    cancel_order_ack.json
    cancel_order_ack_result.json
    cancel_type.json
    category.json
    contract_type.json
    error.json
    event_type.json
    exec_type.json
    execution.json
    execution_data_item.json
    executions_ack.json
    executions_ack_result.json
    executions_ack_result_list_item.json
    header2.json
    instruments_info_ack.json
    instruments_info_ack_result.json
    instruments_info_ack_result_list_item.json
    kline_ack.json
    kline_ack_result.json
    kline_ack_result_list_item.json
    kline_item.json
    kline.json
    leverage_filter.json
    lot_size_filter.json
    message.json
    operation.json
    options_type.json
    order_book_data.json
    order_book.json
    order_book_level.json
    order_data_item.json
    order.json
    orders_ack.json
    orders_ack_result.json
    orders_ack_result_list_item.json
    order_status.json
    order_type.json
    ping.json
    place_order2_data.json
    place_order2.json
    place_order_ack.json
    place_order_ack_result.json
    position_data_item.json
    position.json
    positions_ack.json
    positions_ack_result.json
    positions_ack_result_list_item.json
    position_status.json
    price_filter.json
    public_trade_data.json
    public_trade.json
    reject_reason.json
    side.json
    status.json
    stop_order_type.json
    subscribe_data.json
    subscribe.json
    tick_direction.json
    tickers_data.json
    tickers.json
    time_in_force.json
    topic.json
    tp_sl_mode.json
    trigger_by.json
    wallet_balance_ack.json
    wallet_balance_ack_result.json
    wallet_balance_ack_result_list_item_coin_item.json
    wallet_balance_ack_result_list_item.json
    wallet_data_item_coin_item.json
    wallet_data_item.json
    wallet.json)

roq_autogen_hpp(
  OUTPUT
  AUTOGEN_HEADERS
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

add_custom_target(protocol-json-autogen-headers ALL DEPENDS ${AUTOGEN_HEADERS})

roq_autogen_cpp(
  OUTPUT
  AUTOGEN_SOURCES
  SOURCES
  ${AUTOGEN_SCHEMAS}
  TEMPLATE_DIR
  ${TEMPLATE_DIR}
  TEMPLATE_TYPE
  "json")

roq_gitignore(OUTPUT .gitignore SOURCES ${TARGET_NAME} ${AUTOGEN_HEADERS} ${AUTOGEN_SOURCES})

add_library(${TARGET_NAME} OBJECT ${SOURCES} ${AUTOGEN_SOURCES})

add_dependencies(${TARGET_NAME} protocol-json-autogen-headers)

target_link_libraries(${TARGET_NAME} PRIVATE roq-core::roq-core roq-codec::roq-codec roq-utils::roq-utils roq-logging::roq-logging fmt::fmt)
