cmake -S . -B build_o2 \
  -DCMAKE_CXX_COMPILER=g++-${{matrix.gcc}} \
  -DCMAKE_C_COMPILER=gcc-${{matrix.gcc}} \
  -DCMAKE_CXX_FLAGS_RELEASE:STRING="-O2 -g -DNDEBUG" \
  -DRYML_BUILD_TESTS:BOOL=ON \
  -DRYML_DBG:BOOL=OFF
