#!/bin/sh
# Copyright (c) 2020 Piql AS

set -e
srcdir="$(dirname $0)"
cd "$srcdir"
which autoreconf pkg-config >/dev/null || \
  (echo "configuration failed, please install autoconf first" && exit 1)
autoreconf --install --force --warnings=all
