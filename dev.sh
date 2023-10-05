#!/usr/bin/env bash
set -eou pipefail
nodemon -i subprojects -i build -w . -w Makefile -e cpp,c,h,sh,build -x ./run.sh
