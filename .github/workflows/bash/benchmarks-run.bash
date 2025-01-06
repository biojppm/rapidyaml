set -x
source vars.sh
echo SRC_TAG=$SRC_TAG
echo SRC_VERSION=$SRC_VERSION
desc=$SRC_TAG
for bl in ${{matrix.bitlinks}} ; do
  dst=$(echo ryml-benchmark_results/$desc/x86_64/${{matrix.cxx}}-${{matrix.bt}}-c++${{matrix.std}}-$bl | sed 's:++-:xx:g' | sed 's:+:x:g')
  mkdir -p $dst
  find build -name bm-results
  mv -vf build/$bl/bm/bm-results/* $dst/.
done
