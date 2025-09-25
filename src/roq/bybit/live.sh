#!/usr/bin/env bash

if [ "$1" == "debug" ]; then
  PREFIX="gdb --args"
else
  PREFIX=
fi

NAME="bybit"

CONFIG="${CONFIG:-$NAME}"

CONFIG_FILE="$ROQ_CONFIG_PATH/roq-bybit/$CONFIG.toml"

FLAGFILE="../../../share/flags/prod/flags.cfg"

$PREFIX ./roq-bybit \
  --name "bybit" \
  --config_file "$CONFIG_FILE" \
  --flagfile "$FLAGFILE" \
  --cache_dir "$HOME/var/lib/roq/cache" \
  --event_log_dir "$HOME/var/lib/roq/data" \
  --client_listen_address "$HOME/run/$NAME.sock" \
  --service_listen_address "$HOME/run/metrics/${NAME}.sock" \
  $@
