#!/usr/bin/env bash
set -eou pipefail
JSON="{\"wow\":\"def\"}"
cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -X POST -d '$JSON' -vk 'http://localhost:5005/data'"
eval "$cmd"
