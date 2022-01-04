#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
# force cmake's FetchContent to choose a specific branch, or default to nothing
branch=${2:-}
# make sure to run from where this file is
cd $(dirname $0)
# configure the sample
cmake -S . -B ./build/$cfg -DCMAKE_BUILD_TYPE=$cfg -DRYML_BRANCH_NAME="$branch"
# build and run the sample
cmake --build ./build/$cfg --config $cfg --target run
