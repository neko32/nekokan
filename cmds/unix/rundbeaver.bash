#!/bin/bash

dbeaver_exist=`which dbeaver|wc -l`
if [ ${dbeaver_exist} -eq 0 ]; then
    echo "dbeaver is not installed yet."
    exit 1
fi

dbeaver & > /dev/null 2>&1
