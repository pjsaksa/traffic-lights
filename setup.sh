#!/bin/bash

set -e

SRC="$(cd $(dirname "$0") && pwd)"
DST="$(pwd)"

[ -f "$DST"/Makefile ] && \
    {
        echo '"Makefile" already exists.' 2>&1
        exit 1
    }

{
    echo "SRC=$SRC"
    echo "DST=$DST"
    echo 'include $(SRC)/Makefile.rules'
} >"$DST"/Makefile

make all
