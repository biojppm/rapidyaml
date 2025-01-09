set -x
start_time=$SECONDS
uname -a
pwd
ls -lFhp .
#
bdir=build_${{matrix.arch}}_${{matrix.bt}}_${{matrix.std}}
idir=install_${{matrix.arch}}_${{matrix.bt}}_${{matrix.std}}
mkdir -p $bdir
#
time cmake -S . -B $bdir \
  -DCMAKE_INSTALL_PREFIX=$idir \
  -DCMAKE_BUILD_TYPE=${{matrix.bt}} \
  -DC4_CXX_STANDARD=${{matrix.std}} \
  -DCXX_STANDARD=${{matrix.std}} \
  -DRYML_DEV=ON \
  -DRYML_TEST_SUITE=ON \
  -DRYML_BUILD_BENCHMARKS=OFF \
  -DRYML_SANITIZE=OFF \
  -DRYML_LINT=OFF \
  -DRYML_VALGRIND=OFF
#
time cmake --build $bdir -j 3 --target ryml-test-build
#
time cmake --build $bdir -j 3 --target ryml-test-run
echo "run took $((SECONDS - start_time))"
