Other languages
===============

One of the aims of ryml is to provide an efficient YAML API for other
languages. JavaScript is fully available, and there is already a
cursory implementation for Python using only the low-level API. After
ironing out the general approach, other languages are likely to follow
suit.



Event buffer int handler
------------------------

Recently we added an alternative parser event handler (not part of the
library). This handler parses the YAML source into a linear buffer of
integers, which contains events encoded as bitmasks, interleaved with
strings encoded as an offset (from the beginning of the source buffer)
and length.

This handler is fully compliant (ie it can handle container keys,
unlike the ryml C++ tree), and is also 2x to 3x faster to parse.

This handler is meant to be used in other programming languages while
also minimizing speed-killing inter-language calls, creating a full
representation of the YAML tree that can be processed at once in the
target programming language.

You can find the int event handler in the `src_extra source folder
<https://github.com/biojppm/rapidyaml/tree/master/src_extra>`__. See
its doxygen documentation for details on how to use it, and how to
process the event array.



JavaScript
----------

A JavaScript+WebAssembly port is available, compiled through
`emscripten <https://emscripten.org/>`__.

Here's a quick example on how to configure, compile and run the tests
using `emcmake`:

.. code:: bash

   git clone --recursive https://github.com/biojppm/rapidyaml
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

(Note that this is a work in progress. Additions will be made and
things will be changed.). The python port is using only the
index-based low-level API, which works with node indices and string
views. This API is very fast, but you may find it hard to use: it does not
build a python structure of dicts/seqs/scalars, and all the scalars
are string views into the source buffer, and not typed. With that
said, it is really fast, and once you have the tree, you can still walk
over the tree to create the native python structure. Have a look at
this `test file
<https://github.com/biojppm/rapidyaml/tree/v0.10.0/api/python/tests/test_readme.py>`__
to see how the python API works, and to judge whether it may be useful to your
case.

As for performance, in a `timeit benchmark <https://github.com/biojppm/rapidyaml/tree/v0.10.0/api/python/bm/parse_bm.py>`__ comparing against
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
