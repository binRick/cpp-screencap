#!/usr/bin/env bash
set -eou pipefail
source setup.sh
exec flask --app server run --host=127.0.0.1 --port=5005
