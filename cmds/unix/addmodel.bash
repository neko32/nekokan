#!/bin/bash

if [ $# -eq 2 ]; then
    REPO=$1
    MODEL=$2
    VERSION=latest

elif [ $# -eq 3 ]; then
    REPO=$1
    MODEL=$2
    VERSION=$3
else
    echo "addmodel [Repo name on Huggingface] [GGAF-formatted model's name on Huggingface] [optionnal,version. defaulted to latest]"
    echo "for instance: addmodel mradermacher Llama-3-ELYZA-JP-8B-GGUF latest"
    exit 1
fi

PARAM="hf.co/${REPO}/${MODEL}:${VERSION}"

echo "Command to run - ollama run ${PARAM}"
ollama run ${PARAM}
