#/bin/bash

repo=$1
tag=$2

echo "fetch c4core from repo: $repo"
echo "fetch c4core from tag: $tag"

set -xeo pipefail

mkdir -p c4core
git -C c4core init -q .
git -C c4core remote add origin $repo
git -C c4core fetch --tags origin $tag
git -C c4core reset --hard FETCH_HEAD
git -C c4core submodule update --init --recursive
