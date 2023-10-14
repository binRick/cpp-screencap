#!/usr/bin/env bash
set -eou pipefail
JSON="{\"wow\":\"def\"}"
URL="http://localhost:5005/"
FILE="../monitor_0.gif"
tf="$(mktemp)"

trap "unlink $tf" EXIT

cmd="jo ts=123456 gif=abcdef started_ts=1 ended_ts=2 frames_qty=9 monitor_id=0"
eval "$cmd" > $tf


#cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -X POST -d @$tf -vk '$URL'"
#echo -e "$cmd"
#eval "$cmd"
#cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -X POST -d '$JSON' -vk '$URL'"

#cmd="curl -X POST -H 'Content-Type: application/json' -sk -F file=@$FILE '$URL'"
#eval "$cmd"

cmd="curl -H 'Content-Type: application/json' -H 'Accept: application/json' -d @$tf -vk '$URL/capture'"
echo -e "$cmd"
eval "$cmd"
