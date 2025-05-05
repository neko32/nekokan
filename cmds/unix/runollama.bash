#!/bin/bash

ollama_exist=`ps -ef|egrep "ollama serve"|egrep -v grep|wc -l`

if [ ${ollama_exist} -eq 1 ]; then
    echo "ollama is already serving."
else
    echo "starting ollama.."
    ollama serve & > /dev/null 2>&1
    echo "done."
fi
