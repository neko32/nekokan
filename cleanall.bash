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

if [ -e makefilegen ]; then
    rm makefilegen
fi

pushd .
cd sample_app/json
make clean
popd
pushd .
cd sample_app/base64
make clean
popd
pushd .
cd sample_app/csv
make clean
popd
pushd .
cd sample_app/cpptanu_cfg
make clean
popd


echo "done."
