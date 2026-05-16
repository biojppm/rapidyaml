#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
shift

# make sure to run from where this file is
cd $(dirname $0)
# configure the sample
cmake -S . -B ./build/$cfg-shared -DCMAKE_BUILD_TYPE=$cfg "$@"
# build and run the sample
cmake --build ./build/$cfg-shared --config $cfg --target run
