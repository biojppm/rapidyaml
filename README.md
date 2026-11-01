# Rapid YAML
[![MIT Licensed](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt)
[![release](https://img.shields.io/github/v/release/biojppm/rapidyaml?color=g&include_prereleases&label=release%20&sort=semver)](https://github.com/biojppm/rapidyaml/releases)
<!-- [![Coveralls](https://coveralls.io/repos/github/biojppm/rapidyaml/badge.svg?branch=master)](https://coveralls.io/github/biojppm/rapidyaml) -->
[![Codecov](https://codecov.io/gh/biojppm/rapidyaml/branch/master/graph/badge.svg?branch=master)](https://codecov.io/gh/biojppm/rapidyaml)
[![Documentation Status](https://readthedocs.org/projects/rapidyaml/badge/?version=latest)](https://rapidyaml.readthedocs.io/latest/?badge=latest)

<!-- [![PyPI](https://img.shields.io/pypi/v/rapidyaml?color=g)](https://pypi.org/project/rapidyaml/) -->


Or ryml, for short. ryml is a C++ library to parse and emit YAML,
and do it fast, on everything from x64 to bare-metal chips without
operating system.

The library is fully conformant to the YAML 1.2 spec, and passes
100% of the cases in the YAML test suite. The parser is
state-machine based and not recursive. No known vulnerabilities
exist. The code is robust and extremely tested and fuzzed.

ryml parses both read-only and in-place source buffers; the resulting
tree holds only views to sub-ranges of the source buffer. No string
copies or duplications are done, and no virtual functions are
used. Unlike parsing models that build heavy dictionary trees, ryml
parses data into a flat, index-based data tree, making it highly
efficient for massive datasets, and bare-metal systems.

With the tree, (de)serialization happens only at your direct request,
after parsing / before emitting. Internally, the tree representation
stores only string views and assumes nothing on user types. And it is
not just parsing or emitting which is fast; the serialization is
extremely fast, in many cases faster than the fastest C++ facilities
like `std::to_chars()`. ryml makes it easy and fast to read and modify
the data tree.


ryml can use custom global and per-tree memory allocators and error
handler callbacks, and is exception-agnostic. ryml provides a default
implementation for the allocator (using `std::malloc()`) and error
handlers (using using either exceptions, `longjmp()` or
`std::abort()`), but you can opt out and provide your own memory
allocation and eg, exception-throwing callbacks.

ryml does not depend on the STL, ie, it does not use any std container
as part of its data structures, but it can serialize and deserialize
these containers into the data tree. ryml ships with parts of
[c4core](https://github.com/biojppm/c4core), a small C++ utilities
multiplatform library, but you can ignore the in-source version of
c4core and use a custom or system-installed c4core version.

ryml is written in C++11, and compiles cleanly with:
* Visual Studio 2015 and later
* clang++ 3.9 and later
* g++ 4.8 and later
* Intel Compiler

ryml's [CI pipeline is
merciless](https://github.com/biojppm/rapidyaml/actions) and extremely
demanding, covering Linux, Windows and MacOS. The tests include
analysing ryml with valgrind, clang-tidy, and gcc/clang sanitizers
(asan, ubsan, lsan), and cover most architectures: x64, x86, wasm
(emscripten), aarch64, armv7, armv6, armv5, armv4, loongarch64, mips,
mipsel, mips64, mips64el, riscv64, s390x, ppc, ppc64, ppc64le and
sparc64. ryml also [runs in
bare-metal](https://github.com/biojppm/rapidyaml/issues/193).

ryml is [available in Python](https://pypi.org/project/rapidyaml/) (secondary repo is [rapidyaml-python](https://github.com/biojppm/rapidyaml-python)),
and can very easily be compiled to JavaScript through emscripten (see below).

See also [the changelog](https://github.com/biojppm/rapidyaml/tree/master/changelog)
and [the roadmap](https://github.com/biojppm/rapidyaml/tree/master/ROADMAP.md).

<!-- endpythonreadme -->


## License

ryml is permissively licensed under the [MIT license](LICENSE.txt).


## YAML standard conformance

ryml is fully conformant to the YAML specification, and it passes all
of the cases in the YAML test suite. There are some minor deliberate
deviations in default behavior; [refer to the relevant
page](https://rapidyaml.readthedocs.io/latest/sphinx_yaml_standard.html)
for more information.


## Is it rapid?

Talk is cheap, so here are some performance plots with [an example yaml file](./bm/cases/travis.yml) using [parse](./bm/bm_parse.cpp) and [emit](./bm/bm_emit.cpp) benchmarks, comparing ryml with [yamlcpp](https://github.com/jbeder/yaml-cpp), [libyaml](https://github.com/yaml/libyaml) and [fyaml](https://github.com/pantoniou/libfyaml) (click the image to see the data):

<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>

To prove that this is not coincidence for a particular file, here are scatter plots across YAML files with different styles and sizes. The ryml variants are `ryml_yaml_inplace_reuse_reserve` and `ryml_yaml_str_reserve` from the plots above (again, click the image to see the data):

<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
<br>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>

In absolute terms, ryml parses at ~200MB/s and emits at ~600MB/s, and is generally 30x (parse) / 150x (emit) faster than yamlcpp, and never less than 10x (parse) / 50x (emit). These are stark numbers, and justify the library name. For more details, [refer to this page](https://rapidyaml.readthedocs.io/latest/sphinx_is_it_rapid.html).


### JSON performance

ryml is also among the fastest JSON handlers. It may not be the fastest, but is well ahead of the pack; here are some
performance results. The benchmark code is the same as above, and it is reading a
[compile_commands.json](./bm/cases/compile_commands.json) file. The json libraries are [rapidjson](https://github.com/Tencent/rapidjson), [sajson](https://github.com/chadaustin/sajson), [jsoncpp](https://github.com/open-source-parsers/jsoncpp) and [nlohmann](https://github.com/nlohmann/json):

<table>
<tr><th>Parse performance</th><th>Emit performance</th></tr>
<tr>
<td>

| parse               |    MB/s | MB/s(x) |
|:--------------------|--------:|--------:|
| ryml_json           |  908.00 |  40.31x |
| ryml_yaml           |  881.58 |  39.13x |
| libyaml_arena       |  135.06 |   6.00x |
| libfyaml_arena      |  149.15 |   6.62x |
| yamlcpp_arena       |   22.53 |   1.00x |
| rapidjson_arena     |  756.93 |  33.60x |
| rapidjson_inplace   | 1741.62 |  77.31x |
| sajson_arena        |  592.57 |  26.30x |
| sajson_inplace      |  599.41 |  26.61x |
| jsoncpp_arena       |  298.60 |  13.25x |
| nlohmann_arena      |  197.17 |   8.75x |

</td>
<td>

| emit              |    MB/s | MB/s(x) |
|-------------------|--------:|--------:|
| ryml_yaml_str     |  805.03 | 128.36x |
| ryml_json_str     | 1034.93 | 165.01x |
| fyaml_str         |  425.42 |  67.83x |
| yamlcpp           |    6.27 |   1.00x |
| rapidjson         |  751.47 | 119.82x |
| jsoncpp           |  562.65 |  89.71x |
| nlohmann          |  299.40 |  47.74x |

</td>
</tr>
</table>

So ryml beats most json parsers at their own game, with the only
exception of rapidjson. As for emitting, rapidyaml is hands-down the
fastest, exceeding all JSON libraries. Even parsing full YAML is at
~200MB/s, which is still in the JSON performance ballpark, albeit at
its lower end. This is something to be proud of, as the YAML
specification is much more complex than JSON: [23449 vs 1969
words](https://www.arp242.net/yaml-config.html#its-pretty-complex).


### Serialization performance

Serialization speed also matters for overall speed. For (de)serialization ryml uses the [charconv
facilities](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__charconv.html)
from [c4core](https://github.com/biojppm/c4core); these functions are
blazing fast, and generally outperform the fastest equivalent
facilities in the standard library by a significant margin. Refer to
its [documentation](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__charconv.html).
For example, here are some results for [`c4::xtoa<int64>()`](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__xtoa.html):

<table>
<tr><th>gcc 12.1</th><th>Visual Studio 2019</th></tr>
<tr>
<td>

<a href="https://github.com/biojppm/c4core/blob/master/doc/img/linux-x86_64-gxx12.1-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true"><img src="https://github.com/biojppm/c4core/blob/master/doc/img/linux-x86_64-gxx12.1-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true" width="400" height="314"></a>

</td>
<td>

<a href="https://github.com/biojppm/c4core/blob/master/doc/img/windows-x86_64-vs2019-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true"><img src="https://github.com/biojppm/c4core/blob/master/doc/img/windows-x86_64-vs2019-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true" width="400" height="314"></a>

</td>
</tr>
</table>


------
## Companies and institutions using rapidyaml

- [Google](https://github.com/google/jsonnet)
- Nvidia: [nsight-perf-sdk](https://developer.download.nvidia.com/assets/tools/secure/nsight-perf-sdk/2025_5/NVIDIA_Nsight_Perf_SDK_2025.5_Getting_Started_Guide_rev26.pdf) and [Energon](https://github.com/NVIDIA/Megatron-Energon)
- CERN: [ROOT](https://root.cern/) and [LHCb](https://home.cern/science/experiments/lhcb/)
- [opentelemetry.io](https://github.com/open-telemetry/opentelemetry-cpp) (CNCF / Linux Foundation)
- [Toyota](https://github.com/ToyotaResearchInstitute/rapidyaml-flattened)
- [Qrypt](https://github.com/QryptInc/qseed)
- [OneArc](https://onearc.com/)
- [BrainGlobe](https://github.com/brainglobe/)

This list above was gathered in a quick web search. Please reach out if you'd rather not see yours listed here. And do reach out to let me know how ryml helped solve your problems; this motivates me to continue improving the library.


### Success stories

- [200x faster read](https://github.com/biojppm/c4core/pull/16#issuecomment-700972614) (parse+serialization)
- [67x faster parse](https://openxcom.org/forum/index.php?msg=167331) (OPENXCOM)
- [10x to 70x faster parse](https://rathena.org/board/topic/130698-transitioned-from-yaml-cpp-to-rapidyaml/) (rATHENA)
- [35x faster serialization](https://github.com/biojppm/rapidyaml/issues/242#issuecomment-1088754744)
- python:
  - [150x faster read](https://github.com/4C-multiphysics/fourcipp/pull/11) (4C-multiphysics)
  - [25x faster emit](https://github.com/biojppm/rapidyaml/issues/28#issue-553855608)


### Testimonials

* > I've switched from yaml-cpp to ryml and it went from an approx. speed of 0.5mb/s to 111mb/s, a total life saver. (...) it has been very helpful. ([src](https://github.com/biojppm/c4core/pull/16#issuesrc-700972614))
* > memory consumption (virtual) went from 1.1GB to 94MB, indicating less fragmentation. ([src](https://gitlab.cern.ch:8443/lhcb/Detector/-/merge_requests/574))
* > Thanks again for your help and your amazing libraries! (...) here are my thoughts so far:
  >
  >  * Better code verbosity
  >  * So much customisation !
  >  * Obviously speed!!!
  >
  > ([src](https://github.com/biojppm/rapidyaml/issues/242#issuesrc-1090738911))
* > awesome library for YAML parsing. I'm very happy with it. ([src](https://github.com/biojppm/rapidyaml/issues/87#issue-716890826))
* > thanks for the awesome library! ([src](https://github.com/biojppm/rapidyaml/issues/554#issue-3662609931))
* > I am enjoying using the library to help my C++ applications safely load YAML configuration files ([src](https://github.com/biojppm/rapidyaml/issues/401#issue-2103955221))
* > impressive library ([src](https://github.com/biojppm/rapidyaml/issues/270#issuesrc-1172856960))
* > Thanks for this impressive library! ([src](https://github.com/biojppm/rapidyaml/issues/226#issuesrc-1067063569))
* > rapidyaml combined with c4conf is extremely powerful ([src](https://github.com/biojppm/rapidyaml/issues/352#issue-1534798200))


-----
## Quick start

If you're wondering whether ryml's speed comes at a convenience cost, you
need not. ryml was written with easy and efficient usage in mind:

```cpp
// Parse YAML code in place, potentially mutating the buffer:
char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
ryml::Tree tree = ryml::parse_in_place(yml_buf);

// read from the tree:
ryml::NodeRef bar = tree["bar"];
CHECK(bar[0].val() == "2");
CHECK(bar[1].val() == "3");
CHECK(bar[0].val().str == yml_buf + 15); // points at the source buffer
CHECK(bar[1].val().str == yml_buf + 18);

// deserializing:
int bar0 = 0, bar1 = 0;
bar[0].load(&bar0); // also checks the node is readable, and conversion succeeded
bar[1].load(&bar1); // see also .deserialize()
CHECK(bar0 == 2);
CHECK(bar1 == 3);

// serializing:
bar[0].save(10); // creates a string in the tree's arena
bar[1].save(11); // see also .set_serialized()
CHECK(bar[0].val() == "10");
CHECK(bar[1].val() == "11");

// add nodes
tree["new"].set_val("node");
bar.append_child().save(12);
CHECK(bar[2].val() == "12");

// emit tree
std::string expected = "{foo: 1,bar: [10,11,12],john: doe,new: node}";
// emit tree to std::string
CHECK(ryml::emitrs_yaml<std::string>(tree) == expected);
// emit tree to FILE*
ryml::emit_yaml(tree, stdout); printf("\n");
// emit tree to ostream
std::cout << tree << "\n";

// emit node
ryml::ConstNodeRef foo = tree["foo"];
expected = "foo: 1\n";
// emit node to std::string
CHECK(ryml::emitrs_yaml<std::string>(foo) == expected);
// emit node to FILE*
ryml::emit_yaml(foo, stdout);
// emit node to ostream
std::cout << foo;
```

> [!TIP]
> Do see [the quickstart sample](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__quickstart.html), which covers everything in the library. In particular, do not start writing code without first [reading the quickstart overview](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__quickstart__overview.html). 


## Using ryml in your project

There are [many ways you can use ryml in your project](https://rapidyaml.readthedocs.io/latest/sphinx_using.html):

  - As a library with cmake:
    - using `add_subdirectory()`
    - using `fetch_content()`
    - using `find_package()`, via system install or using package managers (eg, [CPM](https://github.com/cpm-cmake/cpm.cmake), [Conan](https://github.com/conan-io/conan), [vcpkg](https://vcpkg.io/en/), etc)
  - Amalgamated (customizable):
    - Single-header file
    - Single-header file + single-source file: faster to compile than single header
    - There is a [tool to amalgamate](./tools/amalgamate.py) which you can use to customize the result, by opting in or out of different features, or picking a custom commit


### CMake build settings for ryml

See the [relevant documentation page](https://rapidyaml.readthedocs.io/latest/sphinx_cmake_build_settings.html).



------

## Other languages

One of the aims of ryml is to provide an efficient YAML API for other
languages. JavaScript is fully available, and there is already a
cursory implementation for Python using only the low-level API. After
ironing out the general approach, other languages are likely to follow
suit.


### Event buffer int handler

Recently we added an optional parser event handler. This enables
parsing the YAML source into a linear buffer of integers, which
contains events encoded as bitmasks, interleaved with strings encoded
as an offset (from the beginning of the source buffer) and length.

This handler is fully compliant (ie it can handle container keys,
unlike the ryml tree). It is meant to be used in other programming
languages while also minimizing speed-killing inter-language calls, by
creating a full linear representation of the YAML tree that can be
processed at once in the target programming language.

You can find the ints event handler in the [`src_extra` source
folder](https://github.com/biojppm/rapidyaml/tree/master/src_extra). See
the Detailed Description in [its doxygen
documentation](https://rapidyaml.readthedocs.io/latest/doxygen/structc4_1_1yml_1_1extra_1_1EventHandlerInts.html)
for details on how to use it, and how to process the event array.


### JavaScript

A JavaScript+WebAssembly port is available, compiled through [emscripten](https://emscripten.org/).


### Python

There is a blazing fast rapidyaml Python package; it now lives in its
own [dedicated repo](https://github.com/biojppm/rapidyaml-python).
