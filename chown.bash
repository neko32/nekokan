#!/bin/bash

set -e
set -x

if [ $# -ne 3 ]; then
    echo "./chown.bash [group] [name] [dir]"
    exit 1
fi

group=$1
name=$2
dir=$3

chown -R ${group}:${name} ${dir}
