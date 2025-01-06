mkdir -p $BDIR
mkdir -p $IDIR
cmake -S ${{matrix.sdir}} -B $BDIR \
  -DRYML_BUILD_TESTS=ON \
  -DRYML_VALGRIND=OFF \
  -DCMAKE_BUILD_TYPE=${{matrix.bt}} \
  -DCMAKE_INSTALL_PREFIX=$IDIR \
  ${{matrix.gen}} \
  ${{matrix.vars}} \
  ${{matrix.commonvars}}
