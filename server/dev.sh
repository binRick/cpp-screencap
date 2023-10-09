#!/usr/bin/env bash
set -eou pipefail
nodemon -w . -e py,yaml,json,txt,sh -x ./server.sh
