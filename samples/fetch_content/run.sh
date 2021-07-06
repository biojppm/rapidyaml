#!/bin/bash -x

BUILD_TYPE=Release
if [ "$#" -ge 1 ] ; then
    BUILD_TYPE=$1
fi

THIS_DIR=$(pwd)
BUILD_DIR=$THIS_DIR/build/$BUILD_TYPE
mkdir -p $BUILD_DIR

# configure the sample
cmake -S "$THIS_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE
# build and run the sample
cmake --build "$BUILD_DIR" --config $BUILD_TYPE --target run
