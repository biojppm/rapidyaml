#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
# make sure to run from where this file is
cd $(dirname $0)

#------------------------------
# first, build and install ryml
#------------------------------
RYML_SRC=$(cd ../.. ; pwd)
RYML_DIR=./build/$cfg/ryml-install # install ryml to this directory
# configure ryml
cmake -S "$RYML_SRC" -B ./build/$cfg/ryml-build "-DCMAKE_INSTALL_PREFIX=$RYML_DIR" -DCMAKE_BUILD_TYPE=$cfg
# build ryml
cmake --build ./build/$cfg/ryml-build --parallel --config $cfg
# install ryml
cmake --build ./build/$cfg/ryml-build --config $cfg --target install

#-----------------------------
# now build and run the sample
#-----------------------------
# configure the sample
cmake -S . -B ./build/$cfg "-DCMAKE_PREFIX_PATH=$RYML_DIR" -DCMAKE_BUILD_TYPE=$cfg
# build and run the sample
cmake --build ./build/$cfg --config $cfg --target run
