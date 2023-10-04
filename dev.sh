#!/usr/bin/env bash
set -eou pipefail
nodemon -i build -w . -w Makefile -e cpp,c,h,sh,meson.build -x ./run.sh
