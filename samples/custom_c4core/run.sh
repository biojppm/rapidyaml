#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
if [ -n "$1" ] ; then
    shift
fi
# take the c4core tag/hash from the command, or default to the current
# c4core version
c4core_tag=$1
if [ -n "$1" ] ; then
    shift
fi
if [ "$c4core_tag" == "." ] || [ -z "$c4core_tag" ] ; then
    # previously, c4core was a git submodule, but now it is a
    # copy. let's get the tag from the project spec:
    c4core_tag=$(cat ../../ext/c4core.mk \
                 | grep C4CORE_TAG \
                 | grep = \
                 | sed 's/.*=\s*\([0-9a-fA-F]*\)/\1/g')
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
