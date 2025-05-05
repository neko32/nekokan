#!/bin/bash

set -e
set -x

echo "starting mcphost.."

#${NEKOKAN_BIN_DIR}/mcphost/20250504/mcphost -m ollama:llama3.1 --debug --config ${NEKOKAN_CONF_DIR}/mcpserver/20250504_01/nekokanmcp.json 
${NEKOKAN_BIN_DIR}/mcphost/20250504/mcphost -m ollama:llama3.1 --config ${NEKOKAN_CONF_DIR}/mcpserver/20250504_01/nekokanmcp.json 

#echo "mcphost is running as a background process. You may have to wait a bit more till all MCP servers are ready to use."
echo "done."
