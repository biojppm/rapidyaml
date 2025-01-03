set -x
echo GITHUB_REF=$GITHUB_REF
echo GITHUB_HEAD_REF=$GITHUB_HEAD_REF
branch=${GITHUB_HEAD_REF:-${GITHUB_REF#refs/heads/}}  # https://stackoverflow.com/questions/58033366
echo branch=$branch
git init -q .
git config --system --add safe.directory '*'  # needed for running in the docker image. see https://github.com/actions/checkout/issues/1169
git remote add origin $GITHUB_SERVER_URL/$GITHUB_REPOSITORY
git fetch origin $branch
git reset --hard FETCH_HEAD
git submodule update --init --recursive
