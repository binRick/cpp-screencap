#!/usr/bin/env bash
set -eou pipefail
make
time ./build/screencap
