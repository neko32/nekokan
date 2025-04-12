#!/bin/bash

if [ -z "${NEKORC_PATH}" ]; then
    echo "Env variable NEKORC_PATH must be set"
    exit 1
fi

cd ${NEKORC_PATH}
git clone git@github.com:neko32/nekorc.git

mv nekorc/* .
rm -fR ./nekorc

echo "done."
