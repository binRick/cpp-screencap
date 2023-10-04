#!/usr/bin/env bash
set -eou pipefail
nodemon -i build -w . -e cpp,c,h,sh,meson.build,Makefile -x ./run.sh
