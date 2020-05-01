#!/usr/bin/env bash

set -e
set -x

pwd
RYML_DIR=$(pwd)

export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')
$CXX_ --version
$CC_ --version
cmake --version


# add cmake flags
function addcmflags()
{
    CMFLAGS="$CMFLAGS $*"
}

case "$LINT" in
    all       ) addcmflags -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=ON  -DRYML_LINT_PVS_STUDIO=ON ;;
    clang-tidy) addcmflags -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=ON  -DRYML_LINT_PVS_STUDIO=OFF ;;
    pvs-studio) addcmflags -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=OFF -DRYML_LINT_PVS_STUDIO=ON ;;
    *         ) addcmflags -DRYML_LINT=OFF ;;
esac

case "$SAN" in
    ALL) addcmflags -DRYML_SANITIZE=ON ;;
    A  ) addcmflags -DRYML_SANITIZE=ON -DRYML_ASAN=ON  -DRYML_TSAN=OFF -DRYML_MSAN=OFF -DRYML_UBSAN=OFF ;;
    T  ) addcmflags -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=ON  -DRYML_MSAN=OFF -DRYML_UBSAN=OFF ;;
    M  ) addcmflags -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=OFF -DRYML_MSAN=ON  -DRYML_UBSAN=OFF ;;
    UB ) addcmflags -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=OFF -DRYML_MSAN=OFF -DRYML_UBSAN=ON ;;
    *  ) addcmflags -DRYML_SANITIZE=OFF ;;
esac

case "$SAN_ONLY" in
    ON) addcmflags -DRYML_SANITIZE_ONLY=ON ;;
    * ) addcmflags -DRYML_SANITIZE_ONLY=OFF ;;
esac

case "$VG" in
    ON) addcmflags -DRYML_VALGRIND=ON -DRYML_VALGRIND_SGCHECK=OFF ;; # FIXME SGCHECK should be ON
    * ) addcmflags -DRYML_VALGRIND=OFF -DRYML_VALGRIND_SGCHECK=OFF ;;
esac

case "$BM" in
    ON) addcmflags -DRYML_BUILD_BENCHMARKS=ON ;;
    * ) addcmflags -DRYML_BUILD_BENCHMARKS=OFF ;;
esac

if [ "$STD" != "" ] ; then
    addcmflags -DC4_CXX_STANDARD=$STD -DRYML_CXX_STANDARD=$STD
fi

if [ "$BT" == "Coverage" ] ; then
    # the coverage repo tokens need to be set in the travis environment:
    # export CODECOV_TOKEN=.......
    # export COVERALLS_REPO_TOKEN=.......
    addcmflags -DRYML_COVERAGE_CODECOV=ON
    addcmflags -DRYML_COVERAGE_COVERALLS=ON
fi

echo "building with additional cmake flags: $CMFLAGS"

export C4_EXTERN_DIR=`pwd`/build/extern
mkdir -p $C4_EXTERN_DIR


function ryml_cfg_test()
{
    bits=$1
    linktype=$2
    #
    build_dir=`pwd`/build/$bits-$linktype
    install_dir=`pwd`/install/$bits-$linktype
    mkdir -p $build_dir
    mkdir -p $install_dir
    #
    case "$linktype" in
        static) linktype="-DBUILD_SHARED_LIBS=OFF" ;;
        dynamic) linktype="-DBUILD_SHARED_LIBS=ON" ;;
    esac
    cmake -S $RYML_DIR -B $build_dir \
          -DCMAKE_C_COMPILER=$CC_ -DCMAKE_C_FLAGS="-std=c99 -m$bits" \
          -DCMAKE_CXX_COMPILER=$CXX_ -DCMAKE_CXX_FLAGS="-m$bits" \
          -DCMAKE_INSTALL_PREFIX="$install_dir" \
          -DCMAKE_BUILD_TYPE=$BT \
          -DRYML_DEV=ON \
          $CMFLAGS \
          $linktype
    cmake --build $build_dir --target help | sed 1d | sort
}

function ryml_run_test()
{
    bits=$1
    linktype=$2
    #
    build_dir=`pwd`/build/$bits-$linktype
    export CTEST_OUTPUT_ON_FAILURE=1
    cmake --build $build_dir --target test
}

function ryml_submit_coverage()
{
    if [ "$BT" == "Coverage" ] ; then
        bits=$1
        linktype=$2
        coverage_service=$3
        build_dir=`pwd`/build/$bits-$linktype
        echo "Submitting coverage data: $build_dir --> $coverage_service"
        cmake --build $build_dir --target ryml-coverage-submit-$coverage_service
    fi
}
