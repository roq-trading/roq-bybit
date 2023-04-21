#!/usr/bin/env bash

CWD="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

if [ "$1" == "debug" ]; then
  PREFIX="gdb --args"
else
  PREFIX=
fi

NAME="bybit"

CONFIG_FILE="$CWD/config/$NAME-testnet.toml"

URI="bybit.com"

REST_URI="https://api-testnet.$URI"
WS_PUBLIC_URI="wss://stream-testnet.$URI/v5/public"
WS_PRIVATE_URI="wss://stream-testnet.$URI/v5/private"


$PREFIX ./roq-bybit-v5 \
  --name "$NAME" \
  --config_file "$CONFIG_FILE" \
  --cache_dir "$HOME/var/lib/roq/cache" \
  --event_log_dir "$HOME/var/lib/roq/data" \
  --event_log_symlink true \
  --client_listen_address "$HOME/run/$NAME.sock" \
  --service_listen_address "$HOME/run/metrics/${NAME}.sock" \
  --rest_uri "$REST_URI" \
  --ws_public_uri_spot "$WS_PUBLIC_URI/spot" \
  --ws_public_uri_linear "$WS_PUBLIC_URI/linear" \
  --ws_public_uri_inverse "$WS_PUBLIC_URI/inverse" \
  --ws_public_uri_option "$WS_PUBLIC_URI/option" \
  --ws_private_uri "$WS_PRIVATE_URI" \
  $@
