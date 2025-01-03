set -x
source vars.sh
mkdir -p assets
asset_src=`ls -1 ./build/shared64/*-${{matrix.config.sfxg}}`
asset_dst=./assets/${PROJ_PKG_NAME}${SRC_VERSION}-${{matrix.config.sfxp}}
[ ! -f $asset_src ] && exit 1
cp -fav $asset_src $asset_dst
