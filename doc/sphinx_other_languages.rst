Other languages
===============

One of the aims of ryml is to provide an efficient YAML API for other
languages. JavaScript is fully available, and there is already a cursory
implementation for Python using only the low-level API. After ironing
out the general approach, other languages are likely to follow (all of
this is possible because we’re using `SWIG <http://www.swig.org/>`__,
which makes it easy to do so).


JavaScript
----------

A JavaScript+WebAssembly port is available, compiled through
`emscripten <https://emscripten.org/>`__. Here's a quick example of
how to compile ryml with emscripten using ``emcmake``:

.. code:: bash

   git clone --recursive https://github.com/biojppm/rapidyaml
   cd rapidyaml
   emcmake cmake -S . -B build \
       -DCMAKE_CXX_FLAGS="-s DISABLE_EXCEPTION_CATCHING=0"


Here's a quick example on how to configure, compile and run the tests
using `emscripten`:

.. code:: bash

   cd rapidyaml
   emcmake cmake -S . -B build/emscripten \
      -D RYML_DEV=ON \
      -D RYML_BUILD_TESTS=ON \
      -D RYML_BUILD_BENCHMARKS=OFF \
      -D RYML_TEST_SUITE=OFF \
      -D RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON \
      -D CMAKE_BUILD_TYPE=Release \
      -D CMAKE_CXX_FLAGS='-s DISABLE_EXCEPTION_CATCHING=0'
   cmake --build build/emscripten --target ryml-test-run -j



Python
------

(Note that this is a work in progress. Additions will be made and things
will be changed.) With that said, here’s an example of the Python API:

.. code:: python

   import ryml

   # ryml cannot accept strings because it does not take ownership of the
   # source buffer; only bytes or bytearrays are accepted.
   src = b"{HELLO: a, foo: b, bar: c, baz: d, seq: [0, 1, 2, 3]}"

   def check(tree):
       # for now, only the index-based low-level API is implemented
       assert tree.size() == 10
       assert tree.root_id() == 0
       assert tree.first_child(0) == 1
       assert tree.next_sibling(1) == 2
       assert tree.first_sibling(5) == 2
       assert tree.last_sibling(1) == 5
       # use bytes objects for queries
       assert tree.find_child(0, b"foo") == 1
       assert tree.key(1) == b"foo")
       assert tree.val(1) == b"b")
       assert tree.find_child(0, b"seq") == 5
       assert tree.is_seq(5)
       # to loop over children:
       for i, ch in enumerate(ryml.children(tree, 5)):
           assert tree.val(ch) == [b"0", b"1", b"2", b"3"][i]
       # to loop over siblings:
       for i, sib in enumerate(ryml.siblings(tree, 5)):
           assert tree.key(sib) == [b"HELLO", b"foo", b"bar", b"baz", b"seq"][i]
       # to walk over all elements
       visited = [False] * tree.size()
       for n, indentation_level in ryml.walk(tree):
           # just a dumb emitter
           left = "  " * indentation_level
           if tree.is_keyval(n):
              print("{}{}: {}".format(left, tree.key(n), tree.val(n))
           elif tree.is_val(n):
              print("- {}".format(left, tree.val(n))
           elif tree.is_keyseq(n):
              print("{}{}:".format(left, tree.key(n))
           visited[inode] = True
       assert False not in visited
       # NOTE about encoding!
       k = tree.get_key(5)
       print(k)  # '<memory at 0x7f80d5b93f48>'
       assert k == b"seq"               # ok, as expected
       assert k != "seq"                # not ok - NOTE THIS! 
       assert str(k) != "seq"           # not ok
       assert str(k, "utf8") == "seq"   # ok again

   # parse immutable buffer
   tree = ryml.parse_in_arena(src)
   check(tree) # OK

   # parse mutable buffer.
   # requires bytearrays or objects offering writeable memory
   mutable = bytearray(src)
   tree = ryml.parse_in_place(mutable)
   check(tree) # OK

As expected, the performance results so far are encouraging. In a
`timeit benchmark <api/python/parse_bm.py>`__ compared against
`PyYaml <https://pyyaml.org/>`__ and
`ruamel.yaml <https://yaml.readthedocs.io/en/latest/>`__, ryml parses
quicker by generally 100x and up to 400x:

::

   +----------------------------------------+-------+----------+----------+-----------+
   | style_seqs_blck_outer1000_inner100.yml | count | time(ms) | avg(ms)  | avg(MB/s) |
   +----------------------------------------+-------+----------+----------+-----------+
   | parse:RuamelYamlParse                  |     1 | 4564.812 | 4564.812 |     0.173 |
   | parse:PyYamlParse                      |     1 | 2815.426 | 2815.426 |     0.280 |
   | parse:RymlParseInArena                 |    38 |  588.024 |   15.474 |    50.988 |
   | parse:RymlParseInArenaReuse            |    38 |  466.997 |   12.289 |    64.202 |
   | parse:RymlParseInPlace                 |    38 |  579.770 |   15.257 |    51.714 |
   | parse:RymlParseInPlaceReuse            |    38 |  462.932 |   12.182 |    64.765 |
   +----------------------------------------+-------+----------+----------+-----------+

(Note that the parse timings above are somewhat biased towards ryml,
because it does not perform any type conversions in Python-land: return
types are merely ``memoryviews`` to the source buffer, possibly copied
to the tree’s arena).

As for emitting, the improvement can be as high as 3000x:

::

   +----------------------------------------+-------+-----------+-----------+-----------+
   | style_maps_blck_outer1000_inner100.yml | count |  time(ms) |  avg(ms)  | avg(MB/s) |
   +----------------------------------------+-------+-----------+-----------+-----------+
   | emit_yaml:RuamelYamlEmit               |     1 | 18149.288 | 18149.288 |     0.054 |
   | emit_yaml:PyYamlEmit                   |     1 |  2683.380 |  2683.380 |     0.365 |
   | emit_yaml:RymlEmitToNewBuffer          |    88 |   861.726 |     9.792 |    99.976 |
   | emit_yaml:RymlEmitReuse                |    88 |   437.931 |     4.976 |   196.725 |
   +----------------------------------------+-------+-----------+-----------+-----------+
