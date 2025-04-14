#!/bin/bash

if [ $# -eq 0 ]; then
    if [ -e ${NEKOKAN_CODE_DIR}/nekokan ]; then
        cd ${NEKOKAN_CODE_DIR}/nekokan
    else
        cd ${NEKOKAN_CODE_DIR}
    fi
elif [ $# -eq 1 ]; then
    cd ${NEKOKAN_CODE_DIR}/$1
fi
