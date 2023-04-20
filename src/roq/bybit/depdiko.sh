#!/usr/bin/env bash

CWD="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

if [ "$1" == "debug" ]; then
  PREFIX="gdb --args"
else
  PREFIX=
fi

NAME="bybit"

CONFIG_FILE="$CWD/config/depdiko.toml"

URI="bybit.com"

REST_URI="https://api.$URI"
WS_PUBLIC_URI="wss://stream.$URI/spot/public/v3"
WS_PRIVATE_URI="wss://stream.$URI/spot/private/v3"

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
