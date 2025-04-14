#!/bin/bash

set -e
set -x

if [ -z ${NEKOKAN_CMD_DIR} ]; then
    echo "env NEKOKAN_CMD_DIR must exist"
    exit 1
fi

if [ ! -e ${NEKOKAN_CMD_DIR} ]; then
    echo "directory ${NEKOKAN_CMD_DIR} must exist"
    exit 1
fi

rm -fR ${NEKOKAN_CMD_DIR}/*
cp -fR cmds/unix/* ${NEKOKAN_CMD_DIR}

echo "done."
