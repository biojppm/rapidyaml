#!/bin/bash -x

BUILD_TYPE=Release
if [ "$#" -ge 1 ] ; then
    BUILD_TYPE=$1
fi

RYML_DIR=$(cd ../.. ; pwd)
THIS_DIR=$(pwd)

BUILD_DIR=$THIS_DIR/build/$BUILD_TYPE
mkdir -p $BUILD_DIR

RYML_BUILD_DIR=$BUILD_DIR/ryml-build
RYML_INSTALL_DIR=$BUILD_DIR/ryml-install


# configure ryml
cmake -S "$RYML_DIR" -B "$RYML_BUILD_DIR" "-DCMAKE_INSTALL_PREFIX=$RYML_INSTALL_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE
# build ryml
cmake --build "$RYML_BUILD_DIR" --parallel --config $BUILD_TYPE
# install ryml
cmake --build "$RYML_BUILD_DIR" --config $BUILD_TYPE --target install


# configure the sample
cmake -S "$THIS_DIR" -B "$BUILD_DIR" "-DCMAKE_PREFIX_PATH=$RYML_INSTALL_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE
# build and run the sample
cmake --build "$BUILD_DIR" --config $BUILD_TYPE --target run
