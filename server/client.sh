#!/usr/bin/env bash
set -eou pipefail
JSON="{\"wow\":\"def\"}"
URL="http://localhost:5005/data"
FILE="../monitor_0.gif"
tf="$(mktemp)"

#trap "unlink $tf" EXIT

cmd="jo abc=123"
eval "$cmd" > $tf


cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -X POST -d @$tf -vk '$URL'"
echo -e "$cmd"
eval "$cmd"
#cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -X POST -d '$JSON' -vk '$URL'"

#cmd="curl -X POST -H 'Content-Type: application/json' -sk -F file=@$FILE '$URL'"
#eval "$cmd"

