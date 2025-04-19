#/bin/bash

repo=${1:-https://github.com/biojppm/c4core}
tag=${2:-v0.2.6}

echo "fetch c4core from repo: $repo"
echo "fetch c4core from tag: $tag"

set -xeo pipefail

mkdir -p c4core

git -C c4core init -q .
git -C c4core remote add origin $repo
git -C c4core fetch --tags origin $tag
git -C c4core reset --hard FETCH_HEAD
git -C c4core submodule update --init --recursive

git describe --tags --abbrev=0 || echo "no tag"
git rev-parse HEAD
git rev-parse --short HEAD
