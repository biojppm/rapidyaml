#!/bin/bash -x

# make sure to run from where this file is
cd $(dirname $0)
./run.sh "$@" -DBUILD_SHARED_LIBS=OFF
