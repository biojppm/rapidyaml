#!/usr/bin/env bash

set -e
set -x

pwd
RYML_DIR=$(pwd)

export CC_=$(echo "$CXX_" | sed 's:clang++:clang:g' | sed 's:g++:gcc:g')

if   [ "$LINT" == "all"        ] ; then CMFLAGS="$CMFLAGS -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=ON  -DRYML_LINT_PVS_STUDIO=ON"
elif [ "$LINT" == "clang-tidy" ] ; then CMFLAGS="$CMFLAGS -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=ON  -DRYML_LINT_PVS_STUDIO=OFF"
elif [ "$LINT" == "pvs-studio" ] ; then CMFLAGS="$CMFLAGS -DRYML_LINT=ON -DRYML_LINT_TESTS=ON -DRYML_LINT_CLANG_TIDY=OFF -DRYML_LINT_PVS_STUDIO=ON"
else
    CMFLAGS="$CMFLAGS -DRYML_LINT=OFF"
fi

if   [ "$SAN" == "ALL" ] ; then CMFLAGS="$CMFLAGS -DRYML_SANITIZE=ON"
elif [ "$SAN" == "A"   ] ; then CMFLAGS="$CMFLAGS -DRYML_SANITIZE=ON -DRYML_ASAN=ON  -DRYML_TSAN=OFF -DRYML_MSAN=OFF -DRYML_UBSAN=OFF"
elif [ "$SAN" == "T"   ] ; then CMFLAGS="$CMFLAGS -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=ON  -DRYML_MSAN=OFF -DRYML_UBSAN=OFF"
elif [ "$SAN" == "M"   ] ; then CMFLAGS="$CMFLAGS -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=OFF -DRYML_MSAN=ON  -DRYML_UBSAN=OFF"
elif [ "$SAN" == "UB"  ] ; then CMFLAGS="$CMFLAGS -DRYML_SANITIZE=ON -DRYML_ASAN=OFF -DRYML_TSAN=OFF -DRYML_MSAN=OFF -DRYML_UBSAN=ON"
else
    CMFLAGS="$CMFLAGS -DRYML_SANITIZE=OFF"
fi

if [ "$SAN_ONLY" == "ON" ] ; then
    CMFLAGS="$CMFLAGS -DRYML_SANITIZE_ONLY=ON"
else
    CMFLAGS="$CMFLAGS -DRYML_SANITIZE_ONLY=OFF"
fi

if [ "$VG" == "ON" ] ; then
    CMFLAGS="$CMFLAGS -DRYML_VALGRIND=ON -DRYML_VALGRIND_SGCHECK=OFF" # FIXME SGCHECK should be ON
elif [ "$VG" == "OFF" ] ; then
    CMFLAGS="$CMFLAGS -DRYML_VALGRIND=OFF -DRYML_VALGRIND_SGCHECK=OFF"
fi

if [ "$BM" == "ON" ] ; then
    CMFLAGS="$CMFLAGS -DRYML_BUILD_BENCHMARKS=ON"
elif [ "$BM" == "OFF" ] || [ "$BM" == "" ] ; then
    CMFLAGS="$CMFLAGS -DRYML_BUILD_BENCHMARKS=OFF"
fi

if [ ! -z "$CMFLAGS" ] ; then
    echo "additional cmake flags: $CMFLAGS"
fi

CFLAGS_="-std=c99"
XFLAGS_="-std=c++11"
if [ "$A" == "32" ] ; then
    CFLAGS_="$CFLAGS_ -m32"
    XFLAGS_="$XFLAGS_ -m32"
fi

mkdir build
cd build
cmake -DCMAKE_C_COMPILER=$CC_ -DCMAKE_C_FLAGS="$CFLAGS_" \
      -DCMAKE_CXX_COMPILER=$CXX_ -DCMAKE_CXX_FLAGS="$XFLAGS_" \
      -DCMAKE_BUILD_TYPE=$BT \
      -DRYML_DEV=ON \
      $CMFLAGS \
      $RYML_DIR
make help | sed 1d | sort
make CTEST_OUTPUT_ON_FAILURE=1 ryml-test
cd -

exit 0
