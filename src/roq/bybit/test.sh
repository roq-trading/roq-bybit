#!/usr/bin/env bash

CWD="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

if [ "$1" == "debug" ]; then
  PREFIX="gdb --args"
else
  PREFIX=
fi

KERNEL="$(uname -a)"

case "$KERNEL" in
  Linux*)
    LOCAL_INTERFACE=$(ip route get 8.8.8.8 | sed -n 's/.*src \([^\ ]*\).*/\1/p')
    ;;
  Darwin*)
    LOCAL_INTERFACE=$(osascript -e "IPv4 address of (system info)")
    ;;
  *)
    (>&2 echo -e "\033[1;31mERROR: Unknown architecture.\033[0m") && exit 1
esac

DATABASE_URI="http://192.168.188.70:8123"
#DATABASE_URI="http://localhost:8123"

NAME="bybit"

CONFIG="${CONFIG:-$NAME-testnet}"

CONFIG_FILE="$ROQ_CONFIG_PATH/roq-bybit/$CONFIG.toml"

URI="bybit.com"

REST_URI="https://api-testnet.$URI"
WS_PUBLIC_URI="wss://stream-testnet.$URI/v5/public"
WS_PRIVATE_URI="wss://stream-testnet.$URI/v5/private"


$PREFIX ./roq-bybit \
  --name "$NAME" \
  --config_file "$CONFIG_FILE" \
  --cache_dir "$HOME/var/lib/roq/cache" \
  --event_log_dir "$HOME/var/lib/roq/data" \
  --event_log_symlink true \
  --client_listen_address "$HOME/run/$NAME.sock" \
  --service_listen_address "$HOME/run/metrics/${NAME}.sock" \
  --rest_uri "$REST_URI" \
  --ws_public_uri "$WS_PUBLIC_URI" \
  --ws_private_uri "$WS_PRIVATE_URI" \
  --download_trades_lookback=5m \
  --oms_cache=true \
  --oms_multicast_port 1234 \
  --oms_multicast_address=224.1.1.1 \
  --oms_local_interface="$LOCAL_INTERFACE" \
  --oms_multicast_ttl 4 \
  --oms_multicast_loop=true \
  --oms_listen_port 9876 \
  --cache_database_uri "$DATABASE_URI" \
  --cache_database_name "roq" \
  --enable_portfolio=true \
  --test_oms_disable_local_cache=true \
  $@
