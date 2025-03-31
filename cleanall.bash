#!/bin/bash
set -e
set -x

if [ -e nkcall ]; then
    rm nkcall
fi

if [ -e buildenv ]; then
    rm buildenv
fi

if [ -e evgen ]; then
    rm evgen
fi

cd sample_app/json
make clean

echo "done."
