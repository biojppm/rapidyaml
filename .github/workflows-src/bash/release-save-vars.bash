source vars.sh
version_body=${{github.workspace}}/changelog/$SRC_VERSION.md
if [ ! -f $version_body ] ; then
  echo "version body file was not found: $version_body"
  exit 1
fi
echo "VERSION=$SRC_VERSION >> $GITHUB_OUTPUT"
echo "VERSION_BODY=$version_body >> $GITHUB_OUTPUT"
