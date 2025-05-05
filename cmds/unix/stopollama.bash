#!/bin/bash

ollama_exist=`ps -ef|egrep "ollama serve"|egrep -v grep|wc -l`

if [ ${ollama_exist} -eq 0 ]; then
    echo "ollama is not serving."
else
    pid=`ps -ef|egrep "ollama serve"|egrep -v grep|awk '{print $2}'`
    echo "stopping ollama (PID:${pid}).."
    kill -9 ${pid}

    ollama_exist=`ps -ef|egrep "ollama serve"|egrep -v grep|wc -l`
    if [ ${ollama_exist} -eq 0 ]; then
        echo "done."
    else
        echo "couldn't stop ollama."
    fi
fi
