#!/usr/bin/env bash

if [ "$1" == "debug" ]; then
  PREFIX="gdb --args"
else
  PREFIX=
fi

NAME="bybit"

CONFIG="${CONFIG:-$NAME}"

CONFIG_FILE="$ROQ_CONFIG_PATH/roq-bybit/$CONFIG.toml"

URI="bybit.com"

REST_URI="https://api.$URI"
WS_PUBLIC_URI="wss://stream.$URI/v5/public"
WS_PRIVATE_URI="wss://stream.$URI/v5/private"

$PREFIX ./roq-bybit \
  --name "bybit" \
  --config_file "$CONFIG_FILE" \
  --cache_dir "$HOME/var/lib/roq/cache" \
  --event_log_dir "$HOME/var/lib/roq/data" \
  --event_log_symlink true \
  --client_listen_address "$HOME/run/$NAME.sock" \
  --service_listen_address "$HOME/run/metrics/${NAME}.sock" \
  --rest_uri "$REST_URI" \
  --ws_public_uri "$WS_PUBLIC_URI" \
  --ws_private_uri "$WS_PRIVATE_URI" \
  $@
