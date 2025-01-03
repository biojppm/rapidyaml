set -x
branch_name=${GITHUB_REF#refs/heads/}
# builds triggered from PRs have the branch_name like this: refs/pull/150/merge
# so filter to eg pr0150_merge
branch_name=`echo $branch_name | sed "s:refs/pull/\([0-9]*\)/\(.*\):pr0\1_\2:"`
# sanitize the branch name; eg merge/foo-bar -> merge_foo_bar
branch_name=`echo $branch_name | sed 's:[/.-]:_:g'`
SRC_TAG=$(git describe || git rev-parse --short HEAD)  # eg v0.2.0-110-gda837e0
SRC_VERSION="${branch_name}-${SRC_TAG}"
cat <<EOF > vars.sh
export SRC_TAG=$SRC_TAG
export SRC_VERSION=$SRC_VERSION
EOF
