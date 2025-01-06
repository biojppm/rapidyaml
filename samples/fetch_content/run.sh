#!/bin/bash -x

# take the build type from the command line, or default to release
cfg=${1:-Release}
# force cmake's FetchContent to choose a specific branch, or use the default
branch=${2:-master}
# force cmake's FetchContent to choose a specific repo, or use the default
repo=${3:-https://github.com/biojppm/rapidyaml}

# make sure to run from where this file is
cd $(dirname $0)
# configure the sample
cmake -S . -B ./build/$cfg \
      -DCMAKE_BUILD_TYPE=$cfg \
      -DRYML_BRANCH_NAME=$branch \
      -DRYML_REPO_URL=$repo
# build and run the sample
cmake --build ./build/$cfg --config $cfg --target run
