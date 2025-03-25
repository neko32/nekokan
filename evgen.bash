#!/bin/bash

set -e
set -x


if [ -e ./evgen ]; then
    rm ./evgen
fi
g++-13 -std=c++20 envvar_gen.cpp -o evgen

