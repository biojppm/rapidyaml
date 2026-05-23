#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
if [ -n "$1" ] ; then
    shift
fi
# take the c4core tag/hash from the command, or default to the current
# submodule version
c4core_tag=$1
if [ -n "$1" ] ; then
    shift
fi
if [ "$c4core_tag" == "." ] || [ -z "$c4core_tag" ] ; then
    c4core_tag=$(git rev-parse HEAD:../../ext/c4core)
fi

# make sure to run from where this file is
cd $(dirname $0)
# configure the sample
cmake -B ./build/$cfg \
    -DCMAKE_BUILD_TYPE=$cfg \
    -DC4CORE_TAG=$c4core_tag \
    "$@"
# build and run the sample
cmake --build ./build/$cfg --config $cfg --target run --parallel
