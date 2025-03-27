#!/bin/bash

set -e
set -x

if [ -e ./evgen ]; then
    rm ./evgen
fi
g++-13 -std=c++20 nkcall.cpp -o nkcall
