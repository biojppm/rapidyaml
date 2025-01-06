set -x
echo "python ${{matrix.config.pythonv}} ${{matrix.config.py_arch}} ${{matrix.config.cibw_arch}}"
# skip 32 bit tests, as Python 32 bits are not available in ubuntu
arch="${{matrix.config.cibw_arch}}"
if [ "$arch" == "x86" ] || [ "$arch" == "i686" ] ; then
  exit 0
fi
python --version
python -c 'import sys ; import struct ; print("python:", sys.version, struct.calcsize("P") * 8, "bits")'
pip --version
pip install -v -r requirements.txt
pip install -v -r api/python/requirements.txt
for whl in dist/* ; do
  pip install -v $whl
  pip show -f rapidyaml
  python -c 'import ryml ; print("ryml", ryml.version, ryml.version_tuple)'
  python -c 'import ryml ; tree = ryml.parse_in_arena(b"{foo: bar}") ; assert tree.key(1) == b"foo" ; assert tree.val(1) == b"bar" ; print(str(tree.key(1), "utf8")) ; print(str(tree.val(1), "utf8"))'
  python -m pytest -vvv api/python/tests
  pip uninstall -y -v rapidyaml
done
