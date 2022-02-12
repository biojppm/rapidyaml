#!/bin/bash

set -x
set -e

d=$1
[ "$d" == "" ] && d=.
if [ ! -d $d ] ; then
    echo "$d is not a directory"
    exit 1
fi
d=$(cd $d ; pwd) # get absolute path

cd $d/yaml-test-runtimes
make force build
cd $d/yaml-test-suite
make clean run-tests export
xsel -b <export.tsv
