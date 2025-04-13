#!/bin/bash

set -e
set -x


if [ -e ./makefilegen ]; then
    rm ./makefilegen
fi
g++-13 -std=c++20 makefile_gen.cpp -o makefilegen

