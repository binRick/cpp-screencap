#!/usr/bin/env bash
set -eou pipefail
nodemon -i build -w . -e cpp -x ./run.sh
