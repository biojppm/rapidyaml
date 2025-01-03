# https://github.community/t/how-to-get-just-the-tag-name/16241/11
SRC_TAG=${GITHUB_REF#refs/tags/}
SRC_VERSION=${GITHUB_REF#refs/tags/v}
cat <<EOF > vars.sh
export SRC_TAG=$SRC_TAG
export SRC_VERSION=$SRC_VERSION
EOF
