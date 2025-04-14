#!/bin/bash

set -e
set -x

if [ -e ./nkcall ]; then
    rm ./nkcall
fi
g++-13 -std=c++20 nkcall.cpp -o nkcall -lsqlite3 -lcurl
