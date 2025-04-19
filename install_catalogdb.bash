#!/bin/bash

set -e
set -x

cataqlog_fname=catalog.sqlite3

if [ -z ${NEKOKAN_INDB_PATH} ]; then
    echo "env NEKOKAN_INDB_PATH must exist"
    exit 1
fi

db_path=${NEKOKAN_INDB_PATH}/nekokan/db

if [ ! -e ${db_path} ]; then
    mkdir -p ${db_path}
elif [ -e ${db_path}/${cataqlog_fname} ]; then
    rm ${db_path}/${cataqlog_fname}
fi

cp db/${cataqlog_fname} ${db_path}

echo "done"
