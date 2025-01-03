pwd
ls -lFhp
source vars.sh
echo SRC_TAG=$SRC_TAG
echo SRC_VERSION=$SRC_VERSION
id=${PROJ_PKG_NAME}${SRC_VERSION}
name=${id}-src
mkdir -p assets
git-archive-all --prefix $name assets/$name.tgz
git-archive-all --prefix $name assets/$name.zip
python --version
python tools/amalgamate.py assets/$id.hpp
