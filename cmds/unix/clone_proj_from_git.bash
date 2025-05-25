#!/bin/bash

set -e
set -x

if [ $# -eq 3 ]; then
    DOWNLOAD_DIR=${HOME_TMP_DIR}
    GIT_LOC=$1
    MOD_NAME=$2
    MKFILE_LOC=$3
    cd ${DOWNLOAD_DIR}
    git clone ${GIT_LOC}
    cd ${MOD_NAME}/${MKFILE_LOC}
    make install
    cd ${HOME_TMP_DIR}
    rm -fR ${MOD_NAME}
else
    echo "missing git location and/or mkfile location and/or module name. exitting.."
    exit 1
fi
