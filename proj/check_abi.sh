#!/bin/bash

set -xeuo pipefail

rootdir=`git rev-parse --show-toplevel`
branch=`git rev-parse --abbrev-ref HEAD`

libname=$1 ; shift
currver=$1 ; shift
nextver=$1 ; shift
nextsoname=$1 ; shift

flags="-g -Og"

function gitcheckout()
{
    id=$1
    git checkout $id
    if [ -f $rootdir/.gitmodules ] ; then
        git submodule foreach --recursive git submodule update --recursive
    fi
    git status
}

# https://lvc.github.io/abi-compliance-checker/
function dumpbuild()
{
    id=$1
    soname=
    if [ -v 2 ] ; then
        soname=$2
    fi
    bdir=$rootdir/build/abicheck-$id
    mkdir -p $bdir
    cmake -S $rootdir -B $bdir \
          -D CMAKE_BUILD_TYPE=Debug \
          -D CMAKE_CXX_FLAGS="$flags" \
          -D CMAKE_C_FLAGS="$flags" \
          -D BUILD_SHARED_LIBS=ON
    cmake --build $bdir -j
    solib=$bdir/lib$libname.so
    abi-dumper $solib \
               -o $bdir/abi.dump \
               -lver $id
    if [ -n "$soname" ] ; then
        objdump -p $solib | grep SONAME
        soname_actual=$(objdump -p $solib | grep SONAME | sed "s:.*SONAME.*$libname\.so\.\(.*\):\1:")
        if [ "$soname_actual" != "$soname" ] ; then
            echo "error: SONAME: expected $soname, got $soname_actual"
            exit 1
        fi
    fi
}

if [ $nextver != master ] ; then
    gitcheckout $nextver
fi
dumpbuild $nextver $nextsoname ; newabi=$bdir/abi.dump
gitcheckout $currver
dumpbuild $currver ; oldabi=$bdir/abi.dump
gitcheckout $branch

stat=0
cd $rootdir/build/
abi-compliance-checker -l $libname \
                       -old $oldabi \
                       -new $newabi \
    | sed '/Report: compat_/d' \
    || stat=1

set +x
echo "REPORT:"
echo "    $rootdir/build/compat_reports/c4core/${currver}_to_${nextver}/compat_report.html"
echo "    status=$stat"

exit $stat
