#!/bin/bash

set -e
set -x


if [ -e ./evgen ]; then
    rm ./evgen
fi
clang++ -std=c++23 envvar_gen.cpp -o evgen

