#!/bin/bash

set -e

SRC="$(dirname "$0")"
DST="$(pwd)"

# clean

rm -rf backend static

# front-end

cd "$SRC"
cp -a static "$DST"/static

# back-end

cd "$DST"
go build -o backend "$SRC"/back/Crossing.go "$SRC"/back/Direction.go "$SRC"/back/Lane.go "$SRC"/back/main.go
