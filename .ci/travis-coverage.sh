#!/bin/bash

export COVERALLS_REPO_TOKEN=7S2bJ858djL2egNuaaOJ2itxnPBhkhFfs
export CODECOV_TOKEN=a1a6f2e9-9490-4f16-afe0-d2b6dab53b9e

function submit_coverage()
{
    coveralls --include src --root .. --build-root . --gcov-options '\-lp'
    curl -s https://codecov.io/bash > submit_codecov.bash
    chmod a+x submit_codecov.bash
    ./submit_codecov.bash -t $CODECOV_TOKEN -g test -G src -p .. -a '\-lp'
}

if [ "${BT}" == "Coverage" ] ; then
    pwd
    cd build
    make c4core-coverage && submit_coverage
fi
