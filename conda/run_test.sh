#!/usr/bin/env bash

set -x

NAME="roq-binance-futures"

KERNEL="$(uname -s)"
MACHINE="$(uname -m)"

echo "ARCH=$ARCH"
echo "KERNEL=$KERNEL"
echo "MACHINE=$MACHINE"

case "$ARCH" in
  64)
    if [ "$MACHINE" != "x86_64" ]; then
      (>&2 echo -e "\033[1;31mWARN: Drop testing when cross-compiling...\033[0m") && exit 0
    fi
    ;;
  arm64)
    if [ "$MACHINE" != "arm64" ] || [ "$KERNEL" != "Darwin" ]; then
      (>&2 echo -e "\033[1;31mWARN: Drop testing when cross-compiling...\033[0m") && exit 0
    fi
    ;;
  aarch64)
    if [ "$MACHINE" != "aarch64" ] || [ "$KERNEL" != "Linux" ]; then
      (>&2 echo -e "\033[1;31mWARN: Drop testing when cross-compiling...\033[0m") && exit 0
    fi
    ;;
  *)
    (>&2 echo -e "\033[1;31mERROR: Unsupported ARCH=$ARCH.\033[0m") && exit 1
    ;;
esac

echo -e "\033[1;34m--- START ---\033[0m"

if "$NAME" --help; then
  (>&2 echo -e "\033[1;31mERROR: Unexpected error code.\033[0m") && exit 1
fi

if ! "$NAME-benchmark" --help; then
  (>&2 echo -e "\033[1;31mERROR: Unexpected error code.\033[0m") && exit 1
fi

echo -e "\033[1;34m--- DONE ---\033[0m"
