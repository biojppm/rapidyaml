Is it rapid?
============

You bet!

Compared against the other existing YAML libraries for
C/C++
- ryml is in general between 2 and 3 times faster than
`libyaml <https://github.com/yaml/libyaml>`__
- ryml is in general between 10 and 70 times faster than
`yaml-cpp <https://github.com/jbeder/yaml-cpp>`__, and in some cases as
much as 100x and `even
200x <https://github.com/biojppm/c4core/pull/16#issuecomment-700972614>`__
faster.

.. Note::

   While a more effective way of showing the benchmark results is not
   available yet, you can browse through the `runs of the benchmark
   workflow in the CI
   <https://github.com/biojppm/rapidyaml/actions/workflows/benchmarks.yml>`__
   to scroll through the results for yourself.
   
   Also, if you have a case where ryml behaves very nicely or not as
   nicely as claimed here, we would definitely like to see it! Please
   open an issue, or submit a pull request adding the file to
   `bm/cases
   <https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/cases>`__, or
   just send us the files.

`Here’s a parsing benchmark
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/bm_parse.cpp>`__. Using
different approaches within ryml (in-situ/read-only vs. with/without
reuse), a YAML / JSON buffer is repeatedly parsed, and compared
against other libraries.


Comparison with yaml-cpp
------------------------

The first result set is for Windows, and is using a `appveyor.yml
config file
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/cases/appveyor.yml>`__. A
comparison of these results is summarized on the table below:

=========================== ===== ======= ==========
Read rates (MB/s)           ryml  yamlcpp compared
=========================== ===== ======= ==========
appveyor / vs2017 / Release 101.5 5.3     20x / 5.2%
appveyor / vs2017 / Debug   6.4   0.0844  76x / 1.3%
=========================== ===== ======= ==========

The next set of results is taken in Linux, comparing g++ 8.2 and
clang++ 7.0.1 in parsing a YAML buffer from a `travis.yml config file
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/cases/travis.yml>`__
or a JSON buffer from a `compile_commands.json file
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/cases/compile_commands.json>`__. You
can `see the full results here
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/results/parse.linux.i7_6800K.md>`__. Summarizing:

========================== ===== ======= ========
Read rates (MB/s)          ryml  yamlcpp compared
========================== ===== ======= ========
json / clang++ / Release   453.5 15.1    30x / 3%
json / g++ / Release       430.5 16.3    26x / 4%
json / clang++ / Debug     61.9  1.63    38x / 3%
json / g++ / Debug         72.6  1.53    47x / 2%
travis / clang++ / Release 131.6 8.08    16x / 6%
travis / g++ / Release     176.4 8.23    21x / 5%
travis / clang++ / Debug   10.2  1.08    9x / 1%
travis / g++ / Debug       12.5  1.01    12x / 8%
========================== ===== ======= ========

The 450MB/s read rate for JSON puts ryml squarely in the same ballpark
as `RapidJSON <https://github.com/Tencent/rapidjson>`__ and other fast
json readers (`data from
here <https://lemire.me/blog/2018/05/03/how-fast-can-you-parse-json/>`__).
Even parsing full YAML is at ~150MB/s, which is still in that
performance ballpark, albeit at its lower end. This is something to be
proud of, as the YAML specification is much more complex than JSON:
`23449 vs 1969
words <https://www.arp242.net/yaml-config.html#its-pretty-complex>`__.


Performance reading JSON
------------------------

So how does ryml compare against other JSON readers? Well, it may not
be the fastest, but it's definitely ahead of the pack!

The benchmark is the `same as above
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/bm_parse.cpp>`__,
and it is reading the `compile_commands.json
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/cases/compile_commands.json>`__,
The ``_arena`` suffix notes parsing a read-only buffer (so buffer
copies are performed), while the ``_inplace`` suffix means that the
source buffer can be parsed in place. The ``_reuse`` means the data
tree and/or parser are reused on each benchmark repeat.

Here’s what we get with g++ 8.2:

=================== ============ ==========
Benchmark           Release,MB/s Debug,MB/s
=================== ============ ==========
rapidjson_arena     509.9        43.4
rapidjson_inplace   1329.4       68.2
sajson_inplace      434.2        176.5
sajson_arena        430.7        175.6
jsoncpp_arena       183.6        ? 187.9
nlohmann_json_arena 115.8        21.5
yamlcpp_arena       16.6         1.6
libyaml_arena       113.9        35.7
libyaml_arena_reuse 114.6        35.9
ryml_arena          388.6        36.9
ryml_inplace        393.7        36.9
ryml_arena_reuse    446.2        74.6
ryml_inplace_reuse  457.1        74.9
=================== ============ ==========

You can verify that (at least for this test) ryml beats most json
parsers at their own game, with the only exception of
`rapidjson <https://github.com/Tencent/rapidjson>`__. And actually, in
Debug, `rapidjson <https://github.com/Tencent/rapidjson>`__ is slower
than ryml, and `sajson <https://github.com/chadaustin/sajson>`__ manages
to be faster (but not sure about jsoncpp; need to scrutinize there the
suspicious fact that the Debug result is faster than the Release
result).


Performance emitting
--------------------

`Emitting benchmarks
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/bm/bm_emit.cpp>`__
also show similar speedups from the existing libraries, also
anecdotally reported by some users `(eg, here’s a user reporting 25x
speedup from yaml-cpp)
<https://github.com/biojppm/rapidyaml/issues/28#issue-553855608>`__.
Also, in some cases (eg, block folded multiline scalars), the speedup
is as high as 200x (eg, 7.3MB/s -> 1.416GB/s).


Serialization performance
-------------------------

ryml uses the `charconv facilities
<doxygen/group__doc__charconv.html>`__ from `c4core
<https://github.com/biojppm/c4core>`__; these functions are blazing
fast, and generally outperform the fastest equivalent facilities in
the standard library by a significant margin; refer to the
documentation of `charconv facilities
<doxygen/group__doc__charconv.html>`__ for further details. For
example, here are some results for `ryml::xtoa<int64>` (`documentation
<doxygen/group__doc__xtoa.html>`__):

.. |xtoa64_linux| image:: doxygen/linux-x86_64-gxx12.1-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png
   :target: doxygen/linux-x86_64-gxx12.1-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png
.. |xtoa64_windows| image:: doxygen/windows-x86_64-vs2019-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png
   :target: doxygen/windows-x86_64-vs2019-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png

+----------------+--------------------+
| g++12, linux   | Visual Studio 2019 |
+================+====================+
| |xtoa64_linux| | |xtoa64_windows|   |
+----------------+--------------------+
