if [ "${{matrix.sdir}}" == "test/test_install" ] ; then
  mkdir -p $BDIR-staging
  cmake -S . -B $BDIR-staging -DCMAKE_INSTALL_PREFIX=$PDIR -DCMAKE_BUILD_TYPE=${{matrix.bt}} ${{matrix.gen}} ${{matrix.commonvars}}
  cmake --build $BDIR-staging --config ${{matrix.bt}} --target ${{matrix.tgt}} -j
  cmake --build $BDIR-staging --config ${{matrix.bt}} --target install
fi
