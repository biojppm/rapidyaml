# Rapid YAML
[![MIT Licensed](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt)
[![release](https://img.shields.io/github/v/release/biojppm/rapidyaml?color=g&include_prereleases&label=release%20&sort=semver)](https://github.com/biojppm/rapidyaml/releases)
[![Documentation Status](https://readthedocs.org/projects/rapidyaml/badge/?version=latest)](https://rapidyaml.readthedocs.io/latest/?badge=latest)

[![PyPI](https://img.shields.io/pypi/v/rapidyaml?color=g)](https://pypi.org/project/rapidyaml/)
[![Gitter](https://badges.gitter.im/rapidyaml/community.svg)](https://gitter.im/rapidyaml/community)

<!-- [![Coveralls](https://coveralls.io/repos/github/biojppm/rapidyaml/badge.svg?branch=master)](https://coveralls.io/github/biojppm/rapidyaml) -->
[![Codecov](https://codecov.io/gh/biojppm/rapidyaml/branch/master/graph/badge.svg?branch=master)](https://codecov.io/gh/biojppm/rapidyaml)


Or ryml, for short. ryml is a C++ library to parse and emit YAML,
and do it fast, on everything from x64 to bare-metal chips without
operating system. (If you are looking to use your programs with a YAML tree
as a configuration tree with override facilities, take a look at
[c4conf](https://github.com/biojppm/c4conf)).

ryml parses both read-only and in-situ source buffers; the resulting
data nodes hold only views to sub-ranges of the source buffer. No
string copies or duplications are done, and no virtual functions are
used. The data tree is a flat index-based structure stored in a single
array. Serialization happens only at your direct request, after
parsing / before emitting. Internally, the data tree representation
stores only string views and has no knowledge of types, but of course,
every node can have a YAML type tag. ryml makes it easy and fast to
read and modify the data tree.

ryml is available as a single header file, or it can be used as a
simple library with cmake -- both separately (ie
build->install->`find_package()`) or together with your project (ie with
`add_subdirectory()`). (See below for examples).

ryml can use custom global and per-tree memory allocators and error
handler callbacks, and is exception-agnostic. ryml provides a default
implementation for the allocator (using `std::malloc()`) and error
handlers (using using either exceptions, `longjmp()` or
`std::abort()`), but you can opt out and provide your own memory
allocation and eg, exception-throwing callbacks.

ryml does not depend on the STL, ie, it does not use any std container
as part of its data structures), but it can serialize and deserialize
these containers into the data tree, with the use of optional
headers. ryml ships with [c4core](https://github.com/biojppm/c4core), a
small C++ utilities multiplatform library.

ryml is written in C++11, and compiles cleanly with:
* Visual Studio 2015 and later
* clang++ 3.9 and later
* g++ 4.8 and later
* Intel Compiler

ryml's API documentation is [available at
ReadTheDocs](https://rapidyaml.readthedocs.io/latest/).

ryml is [extensively unit-tested in Linux, Windows and
MacOS](https://github.com/biojppm/rapidyaml/actions). The tests cover
x64, x86, wasm (emscripten), arm, aarch64, ppc64le and s390x
architectures, and include analysing ryml with:
  * valgrind
  * clang-tidy
  * gcc/clang sanitizers:
    * memory
    * address
    * undefined behavior

ryml also [runs in
bare-metal](https://github.com/biojppm/rapidyaml/issues/193), and
[RISC-V
architectures](https://github.com/biojppm/c4core/pull/69). Both of
these are pending implementation of CI actions for continuous
validation, but ryml has been proven to work there.

ryml is [available in Python](https://pypi.org/project/rapidyaml/),
and can very easily be compiled to JavaScript through emscripten (see
below).

See also [the changelog](https://github.com/biojppm/rapidyaml/tree/master/changelog)
and [the roadmap](https://github.com/biojppm/rapidyaml/tree/master/ROADMAP.md).

<!-- endpythonreadme -->


------

## Table of contents
* [Is it rapid?](#is-it-rapid)
  * [Comparison with yaml-cpp](#comparison-with-yaml-cpp)
  * [Performance reading JSON](#performance-reading-json)
  * [Performance emitting](#performance-emitting)
* [Quick start](#quick-start)
* [Using ryml in your project](#using-ryml-in-your-project)
  * [Package managers](#package-managers)
  * [Single header file](#single-header-file)
  * [As a library](#as-a-library)
  * [Quickstart samples](#quickstart-samples)
  * [CMake build settings for ryml](#cmake-build-settings-for-ryml)
     * [Forcing ryml to use a different c4core version](#forcing-ryml-to-use-a-different-c4core-version)
* [Other languages](#other-languages)
  * [JavaScript](#javascript)
  * [Python](#python)
* [YAML standard conformance](#yaml-standard-conformance)
  * [Test suite status](#test-suite-status)
* [Known limitations](#known-limitations)
* [Alternative libraries](#alternative-libraries)
* [License](#license)


------

## Is it rapid?

You bet! On a i7-6800K CPU @3.40GHz:
 * ryml parses YAML at about ~150MB/s on Linux and ~100MB/s on Windows (vs2017). 
 * **ryml parses JSON at about ~450MB/s on Linux**, faster than sajson (didn't
   try yet on Windows).
 * compared against the other existing YAML libraries for C/C++:
   * ryml is in general between 2 and 3 times faster than [libyaml](https://github.com/yaml/libyaml)
   * ryml is in general between 10 and 70 times faster than
     [yaml-cpp](https://github.com/jbeder/yaml-cpp), and in some cases as
     much as 100x and [even
     200x](https://github.com/biojppm/c4core/pull/16#issuecomment-700972614) faster.

[Here's the benchmark](./bm/bm_parse.cpp). Using different
approaches within ryml (in-situ/read-only vs. with/without reuse), a YAML /
JSON buffer is repeatedly parsed, and compared against other libraries.

### Comparison with yaml-cpp

The first result set is for Windows, and is using a [appveyor.yml config
file](./bm/cases/appveyor.yml). A comparison of these results is
summarized on the table below:

| Read rates (MB/s)            | ryml   | yamlcpp | compared     |
|------------------------------|--------|---------|--------------|
| appveyor / vs2017 / Release  | 101.5  | 5.3     |  20x / 5.2%  |
| appveyor / vs2017 / Debug    |   6.4  | 0.0844  |  76x / 1.3%  |


The next set of results is taken in Linux, comparing g++ 8.2 and clang++ 7.0.1 in
parsing a YAML buffer from a [travis.yml config
file](./bm/cases/travis.yml) or a JSON buffer from a [compile_commands.json
file](./bm/cases/compile_commands.json). You
can [see the full results here](./bm/results/parse.linux.i7_6800K.md).
Summarizing:

| Read rates (MB/s)           | ryml   | yamlcpp | compared   |
|-----------------------------|--------|---------|------------|
| json   / clang++ / Release  | 453.5  | 15.1    |  30x / 3%  |
| json   /     g++ / Release  | 430.5  | 16.3    |  26x / 4%  |
| json   / clang++ / Debug    |  61.9  | 1.63    |  38x / 3%  |
| json   /     g++ / Debug    |  72.6  | 1.53    |  47x / 2%  |
| travis / clang++ / Release  | 131.6  | 8.08    |  16x / 6%  |
| travis /     g++ / Release  | 176.4  | 8.23    |  21x / 5%  |
| travis / clang++ / Debug    |  10.2  | 1.08    |   9x / 1%  |
| travis /     g++ / Debug    |  12.5  | 1.01    |  12x / 8%  |

The 450MB/s read rate for JSON puts ryml squarely in the same ballpark
as [RapidJSON](https://github.com/Tencent/rapidjson) and other fast json
readers
([data from here](https://lemire.me/blog/2018/05/03/how-fast-can-you-parse-json/)).
Even parsing full YAML is at ~150MB/s, which is still in that performance
ballpark, albeit at its lower end. This is something to be proud of, as the
YAML specification is much more complex than JSON: [23449 vs 1969 words](https://www.arp242.net/yaml-config.html#its-pretty-complex).


### Performance reading JSON

So how does ryml compare against other JSON readers? Well, it's one of the
fastest! 

The benchmark is the [same as above](./bm/parse.cpp), and it is reading
the [compile_commands.json](./bm/cases/compile_commands.json), The `_arena`
suffix notes parsing a read-only buffer (so buffer copies are performed),
while the `_inplace` suffix means that the source buffer can be parsed in
place. The `_reuse` means the data tree and/or parser are reused on each
benchmark repeat.

Here's what we get with g++ 8.2:

| Benchmark             | Release,MB/s | Debug,MB/s  |
|:----------------------|-------------:|------------:|
| rapidjson_arena       |       509.9  |       43.4  |
| rapidjson_inplace     |      1329.4  |       68.2  |
| sajson_inplace        |       434.2  |      176.5  |
| sajson_arena          |       430.7  |      175.6  |
| jsoncpp_arena         |       183.6  |    ? 187.9  |
| nlohmann_json_arena   |       115.8  |       21.5  |
| yamlcpp_arena         |        16.6  |        1.6  |
| libyaml_arena         |       113.9  |       35.7  |
| libyaml_arena_reuse   |       114.6  |       35.9  |
| ryml_arena            |       388.6  |       36.9  |
| ryml_inplace          |       393.7  |       36.9  |
| ryml_arena_reuse      |       446.2  |       74.6  |
| ryml_inplace_reuse    |       457.1  |       74.9  |

You can verify that (at least for this test) ryml beats most json
parsers at their own game, with the only exception of
[rapidjson](https://github.com/Tencent/rapidjson). And actually, in
Debug, [rapidjson](https://github.com/Tencent/rapidjson) is slower
than ryml, and [sajson](https://github.com/chadaustin/sajson)
manages to be faster (but not sure about jsoncpp; need to scrutinize there
the suspicious fact that the Debug result is faster than the Release result).


### Performance emitting

[Emitting benchmarks](bm/bm_emit.cpp) also show similar speedups from
the existing libraries, also anecdotally reported by some users [(eg,
here's a user reporting 25x speedup from
yaml-cpp)](https://github.com/biojppm/rapidyaml/issues/28#issue-553855608). Also, in
some cases (eg, block folded multiline scalars), the speedup is as
high as 200x (eg, 7.3MB/s -> 1.416MG/s).


### CI results and request for files

While a more effective way of showing the benchmark results is not
available yet, you can browse through the [runs of the benchmark
workflow in the
CI](https://github.com/biojppm/rapidyaml/actions/workflows/benchmarks.yml)
to scroll through the results for yourself.

Also, if you have a case where ryml behaves very nicely or not as
nicely as claimed above, we would definitely like to see it! Please
open an issue, or submit a pull request adding the file to
[bm/cases](bm/cases), or just send us the files.


------

## Quick start

If you're wondering whether ryml's speed comes at a usage cost, you
need not: with ryml, you can have your cake and eat it too. Being
rapid is definitely NOT the same as being unpractical, so ryml was
written with easy AND efficient usage in mind, and comes with a two
level API for accessing and traversing the data tree.

The following snippet is a very quick overview taken from quickstart
sample ([see on
doxygen](https://rapidyaml.readthedocs.io/latest/group__doc__quickstart.html)/[see
on github](samples/quickstart.cpp). After cloning ryml
(don't forget the `--recursive` flag for git), you can very easily
build and run this executable using any of the build samples, eg the
[`add_subdirectory()` sample](samples/add_subdirectory/) (see [the relevant section](#quickstart-samples)).

```cpp
// Parse YAML code in place, potentially mutating the buffer:
char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
ryml::Tree tree = ryml::parse_in_place(yml_buf);

// ryml has a two-level API:
//
// The lower level index API is based on the indices of nodes,
// where the node's id is the node's position in the tree's data
// array. This API is very efficient, but somewhat difficult to use:
size_t root_id = tree.root_id();
size_t bar_id = tree.find_child(root_id, "bar"); // need to get the index right
CHECK(tree.is_map(root_id)); // all of the index methods are in the tree
CHECK(tree.is_seq(bar_id));  // ... and receive the subject index

// The node API is a lightweight abstraction sitting on top of the
// index API, but offering a much more convenient interaction:
ryml::ConstNodeRef root = tree.rootref();  // a const node reference
ryml::ConstNodeRef bar = tree["bar"];
CHECK(root.is_map());
CHECK(bar.is_seq());

// The resulting tree stores only string views to the YAML source buffer.
CHECK(root["foo"] == "1");
CHECK(root["foo"].key().str == yml_buf + 1);
CHECK(bar[0] == "2");
CHECK(root["john"] == "doe");

//------------------------------------------------------------------
// To get actual values, you need to deserialize the nodes.
// Deserializing: use operator>>
{
    int foo = 0, bar0 = 0, bar1 = 0;
    std::string john_str;
    std::string bar_str;
    root["foo"] >> foo;
    root["bar"][0] >> bar0;
    root["bar"][1] >> bar1;
    root["john"] >> john_str; // requires from_chars(std::string). see API doc.
    root["bar"] >> ryml::key(bar_str); // to deserialize the key, use the tag function ryml::key()
    CHECK(foo == 1);
    CHECK(bar0 == 2);
    CHECK(bar1 == 3);
    CHECK(john_str == "doe");
    CHECK(bar_str == "bar");
}

//------------------------------------------------------------------
// To modify existing nodes, use operator= or operator<<.

// operator= assigns an existing string to the receiving node.
// The contents are NOT copied, and this pointer will be in effect
// until the tree goes out of scope! So BEWARE to only assign from
// strings outliving the tree.
wroot["foo"] = "says you";
wroot["bar"][0] = "-2";
wroot["bar"][1] = "-3";
wroot["john"] = "ron";
// Now the tree is _pointing_ at the memory of the strings above.
// In this case it is OK because those are static strings and will
// outlive the tree.
CHECK(root["foo"].val() == "says you");
CHECK(root["bar"][0].val() == "-2");
CHECK(root["bar"][1].val() == "-3");
CHECK(root["john"].val() == "ron");
// But WATCHOUT: do not assign from temporary objects:
// {
//     std::string crash("will dangle");
//     root["john"] = ryml::to_csubstr(crash);
// }
// CHECK(root["john"] == "dangling"); // CRASH! the string was deallocated

// operator<< first serializes the input to the tree's arena, then
// assigns the serialized string to the receiving node. This avoids
// constraints with the lifetime, since the arena lives with the tree.
CHECK(tree.arena().empty());
wroot["foo"] << "says who";  // requires to_chars(). see serialization samples below.
wroot["bar"][0] << 20;
wroot["bar"][1] << 30;
wroot["john"] << "deere";
CHECK(root["foo"].val() == "says who");
CHECK(root["bar"][0].val() == "20");
CHECK(root["bar"][1].val() == "30");
CHECK(root["john"].val() == "deere");
CHECK(tree.arena() == "says who2030deere"); // the result of serializations to the tree arena


//------------------------------------------------------------------
// Adding new nodes:

// adding a keyval node to a map:
CHECK(root.num_children() == 5);
wroot["newkeyval"] = "shiny and new"; // using these strings
wroot.append_child() << ryml::key("newkeyval (serialized)") << "shiny and new (serialized)"; // serializes and assigns the serialization
CHECK(root.num_children() == 7);
CHECK(root["newkeyval"].key() == "newkeyval");
CHECK(root["newkeyval"].val() == "shiny and new");
CHECK(root["newkeyval (serialized)"].key() == "newkeyval (serialized)");
CHECK(root["newkeyval (serialized)"].val() == "shiny and new (serialized)");


//------------------------------------------------------------------
// Emitting:

ryml::csubstr expected_result = R"(foo: says who
bar:
- 20
- 30
- oh so nice
- oh so nice (serialized)
john: in_scope
float: 2.4
digits: 2.400000
newkeyval: shiny and new
newkeyval (serialized): shiny and new (serialized)
newseq: []
newseq (serialized): []
newmap: {}
newmap (serialized): {}
I am something: indeed
)";

// emit to a FILE*
ryml::emit_yaml(tree, stdout);
// emit to a stream
std::stringstream ss;
ss << tree;
std::string stream_result = ss.str();
// emit to a buffer:
std::string str_result = ryml::emitrs_yaml<std::string>(tree);
// can emit to any given buffer:
char buf[1024];
ryml::csubstr buf_result = ryml::emit_yaml(tree, buf);
// now check
CHECK(buf_result == expected_result);
CHECK(str_result == expected_result);
CHECK(stream_result == expected_result);

//------------------------------------------------------------------
// UTF8
ryml::Tree langs = ryml::parse_in_arena(R"(
en: Planet (Gas)
fr: Planète (Gazeuse)
ru: Планета (Газ)
ja: 惑星（ガス）
zh: 行星（气体）
# UTF8 decoding only happens in double-quoted strings,
# as per the YAML standard
decode this: "\u263A \xE2\x98\xBA"
and this as well: "\u2705 \U0001D11E"
not decoded: '\u263A \xE2\x98\xBA'
neither this: '\u2705 \U0001D11E'
)");
// in-place UTF8 just works:
CHECK(langs["en"].val() == "Planet (Gas)");
CHECK(langs["fr"].val() == "Planète (Gazeuse)");
CHECK(langs["ru"].val() == "Планета (Газ)");
CHECK(langs["ja"].val() == "惑星（ガス）");
CHECK(langs["zh"].val() == "行星（气体）");
// and \x \u \U codepoints are decoded, but only when they appear
// inside double-quoted strings, as dictated by the YAML
// standard:
CHECK(langs["decode this"].val() == "☺ ☺");
CHECK(langs["and this as well"].val() == "✅ 𝄞");
CHECK(langs["not decoded"].val() == "\\u263A \\xE2\\x98\\xBA");
CHECK(langs["neither this"].val() == "\\u2705 \\U0001D11E");


//------------------------------------------------------------------
// Getting the location of nodes in the source:
//
// Location tracking is opt-in:
ryml::Parser parser(ryml::ParserOptions().locations(true));
// Now the parser will start by building the accelerator structure:
ryml::Tree tree2 = parser.parse_in_arena("expected.yml", expected_result);
// ... and use it when querying
ryml::Location loc = parser.location(tree2["bar"][1]);
CHECK(parser.location_contents(loc).begins_with("30"));
CHECK(loc.line == 3u);
CHECK(loc.col == 4u);
```


------

## Using ryml in your project

### Single header file
ryml is provided chiefly as a cmake library project, but it can also
be used as a single header file, and there is a [tool to
amalgamate](./tools/amalgamate.py) the code into a single header
file. The amalgamated header file is provided with each release, but
you can also generate a customized file suiting your particular needs
(or commit):

```console
[user@host rapidyaml]$ python3 tools/amalgamate.py -h
usage: amalgamate.py [-h] [--c4core | --no-c4core] [--fastfloat | --no-fastfloat] [--stl | --no-stl] [output]

positional arguments:
  output          output file. defaults to stdout

optional arguments:
  -h, --help      show this help message and exit
  --c4core        amalgamate c4core together with ryml. this is the default.
  --no-c4core     amalgamate c4core together with ryml. the default is --c4core.
  --fastfloat     enable fastfloat library. this is the default.
  --no-fastfloat  enable fastfloat library. the default is --fastfloat.
  --stl           enable stl interop. this is the default.
  --no-stl        enable stl interop. the default is --stl.
```

The amalgamated header file contains all the function declarations and
definitions. To use it in the project, `#include` the header at will
in any header or source file in the project, but in one source file,
and only in that one source file, `#define` the macro
`RYML_SINGLE_HDR_DEFINE_NOW` **before including the header**. This
will enable the function definitions. For example:
```c++
// foo.h
#include <ryml_all.hpp>

// foo.cpp
// ensure that foo.h is not included before this define!
#define RYML_SINGLE_HDR_DEFINE_NOW
#include <ryml_all.hpp>
```

If you wish to package the single header into a shared library, then
you will need to define the preprocessor symbol `RYML_SHARED` during
compilation.


### As a library
The single header file is a good approach to quickly try the library,
but if you wish to make good use of CMake and its tooling ecosystem,
(and get better compile times), then ryml has you covered.

As with any other cmake library, you have the option to integrate ryml into
your project's build setup, thereby building ryml together with your
project, or -- prior to configuring your project -- you can have ryml
installed either manually or through package managers.

Currently [cmake](https://cmake.org/) is required to build ryml; we
recommend a recent cmake version, at least 3.13.

Note that ryml uses submodules. Take care to use the `--recursive` flag
when cloning the repo, to ensure ryml's submodules are checked out as well:
```bash
git clone --recursive https://github.com/biojppm/rapidyaml
```
If you omit `--recursive`, after cloning you
will have to do `git submodule update --init --recursive`
to ensure ryml's submodules are checked out.

### Package managers

ryml is available in most package managers (thanks to all the
contributors!) and linux distributions. But please be aware: those
packages are maintained downstream of this repository, so if you have
issues with the package, file a report with the respective maintainer.

Here's a quick roundup (not maintained):
* Package managers:
  * [conan](https://conan.io/center/recipes/rapidyaml)
  * [vcpkg](https://vcpkg.io/en/packages.html): `vcpkg install ryml`
  * [PyPI](https://pypi.org/project/rapidyaml/)
* Linux distributions:
  * Arch Linux/Manjaro:
    * [rapidyaml (aarch64)](https://archlinuxarm.org/packages/aarch64/rapidyaml)
    * [rapidyaml-git (AUR)](https://aur.archlinux.org/packages/rapidyaml-git/)
    * [python-rapidyaml-git (AUR)](https://aur.archlinux.org/packages/python-rapidyaml-git/)
  * [Fedora Linux](https://getfedora.org/)/[EPEL](https://docs.fedoraproject.org/en-US/epel/):
    * `dnf install rapidyaml-devel`
    * `dnf install python3-rapidyaml`
  * [Gentoo](https://packages.gentoo.org/packages/dev-cpp/rapidyaml)
  * [OpenSuse](https://build.openbuildservice.org/package/show/Emulators/rapidyaml)
  * [Slackbuilds](https://slackbuilds.org/repository/15.0/libraries/rapidyaml/)
  * [AltLinux](https://packages.altlinux.org/en/sisyphus/srpms/rapidyaml/3006055151670528141)

Although package managers are very useful for quickly getting up to
speed, the advised way is still to bring ryml as a submodule of your
project, building both together. This makes it easy to track any
upstream changes in ryml. Also, ryml is small and quick to build, so
there's not much of a cost for building it with your project.


### Quickstart samples

These samples show different ways of getting ryml into your application. All the
samples use [the same quickstart executable
source](./samples/quickstart.cpp), but are built in different ways,
showing several alternatives to integrate ryml into your project. We
also encourage you to refer to the [quickstart source](./samples/quickstart.cpp) itself, which
extensively covers most of the functionality that you may want out of
ryml.

Each sample brings a `run.sh` script with the sequence of commands
required to successfully build and run the application (this is a bash
script and runs in Linux and MacOS, but it is also possible to run in
Windows via Git Bash or the WSL). Click on the links below to find out
more about each sample:

| Sample name        | ryml is part of build?   | cmake file   | commands     |
|:-------------------|--------------------------|:-------------|:-------------|
| [`singleheader`](./samples/singleheader) | **yes**<br>ryml brought as a single header file,<br>not as a library | [`CMakeLists.txt`](./samples/singleheader/CMakeLists.txt) | [`run.sh`](./samples/singleheader/run.sh) |
| [`singleheaderlib`](./samples/singleheaderlib) | **yes**<br>ryml brought as a library<br>but from the single header file | [`CMakeLists.txt`](./samples/singleheaderlib/CMakeLists.txt) | [`run_shared.sh` (shared library)](./samples/singleheaderlib/run_shared.sh)<br> [`run_static.sh` (static library)](./samples/singleheaderlib/run_static.sh) |
| [`add_subdirectory`](./samples/add_subdirectory) | **yes**                      | [`CMakeLists.txt`](./samples/add_subdirectory/CMakeLists.txt) | [`run.sh`](./samples/add_subdirectory/run.sh) |
| [`fetch_content`](./samples/fetch_content)      | **yes**                      | [`CMakeLists.txt`](./samples/fetch_content/CMakeLists.txt) | [`run.sh`](./samples/fetch_content/run.sh) |
| [`find_package`](./samples/find_package)        | **no**<br>needs prior install or package  | [`CMakeLists.txt`](./samples/find_package/CMakeLists.txt) | [`run.sh`](./samples/find_package/run.sh) |


### CMake build settings for ryml
The following cmake variables can be used to control the build behavior of
ryml:

  * `RYML_WITH_TAB_TOKENS=ON/OFF`. Enable/disable support for tabs as
    valid container tokens after `:` and `-`. Defaults to `OFF`,
    because this may cost up to 10% in processing time.
  * `RYML_DEFAULT_CALLBACKS=ON/OFF`. Enable/disable ryml's default
    implementation of error and allocation callbacks. Defaults to `ON`.
  * `RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON/OFF` - Enable/disable
    the same-named macro, which will make the default error handler
    provided by ryml throw a `std::runtime_error` exception.
  * `RYML_USE_ASSERT` - enable assertions in the code regardless of
    build type. This is disabled by default. Failed assertions will
    trigger a call to the error callback.
  * `RYML_STANDALONE=ON/OFF`. ryml uses
    [c4core](https://github.com/biojppm/c4core), a C++ library with low-level
    multi-platform utilities for C++. When `RYML_STANDALONE=ON`, c4core is
    incorporated into ryml as if it is the same library. Defaults to `ON`.
  * `RYML_INSTALL=ON/OFF`. enable/disable install target. Defaults to `ON`.

If you're developing ryml or just debugging problems with ryml itself, the
following cmake variables can be helpful:
  * `RYML_DEV=ON/OFF`: a bool variable which enables development targets such as
    unit tests, benchmarks, etc. Defaults to `OFF`.
  * `RYML_DBG=ON/OFF`: a bool variable which enables verbose prints from
    parsing code; can be useful to figure out parsing problems. Defaults to
    `OFF`.

#### Forcing ryml to use a different c4core version

ryml is strongly coupled to c4core, and this is reinforced by the fact
that c4core is a submodule of the current repo. However, it is still
possible to use a c4core version different from the one in the repo
(of course, only if there are no incompatibilities between the
versions). You can find out how to achieve this by looking at the
[`custom_c4core` sample](./samples/custom_c4core/CMakeLists.txt).


------

## Other languages

One of the aims of ryml is to provide an efficient YAML API for other
languages. JavaScript is fully available, and there is already a
cursory implementation for Python using only the low-level API. After
ironing out the general approach, other languages are likely to
follow (all of this is possible because we're using
[SWIG](http://www.swig.org/), which makes it easy to do so).

### JavaScript

A JavaScript+WebAssembly port is available, compiled through [emscripten](https://emscripten.org/).


### Python

(Note that this is a work in progress. Additions will be made and things will
be changed.) With that said, here's an example of the Python API:

```python
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
```
As expected, the performance results so far are encouraging. In
a [timeit benchmark](api/python/parse_bm.py) compared
against [PyYaml](https://pyyaml.org/)
and [ruamel.yaml](https://yaml.readthedocs.io/en/latest/), ryml parses
quicker by generally 100x and up to 400x:
```
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
```
(Note that the parse timings above are somewhat biased towards ryml, because
it does not perform any type conversions in Python-land: return types
are merely `memoryviews` to the source buffer, possibly copied to the tree's
arena).

As for emitting, the improvement can be as high as 3000x:
```
+----------------------------------------+-------+-----------+-----------+-----------+
| style_maps_blck_outer1000_inner100.yml | count |  time(ms) |  avg(ms)  | avg(MB/s) |
+----------------------------------------+-------+-----------+-----------+-----------+
| emit_yaml:RuamelYamlEmit               |     1 | 18149.288 | 18149.288 |     0.054 |
| emit_yaml:PyYamlEmit                   |     1 |  2683.380 |  2683.380 |     0.365 |
| emit_yaml:RymlEmitToNewBuffer          |    88 |   861.726 |     9.792 |    99.976 |
| emit_yaml:RymlEmitReuse                |    88 |   437.931 |     4.976 |   196.725 |
+----------------------------------------+-------+-----------+-----------+-----------+
```


------

## YAML standard conformance

ryml is feature complete with regards to the YAML specification. All
the YAML features are well covered in the unit tests, and expected to
work, unless in the exceptions noted below.

Of course, there are many dark corners in YAML, and there certainly
can appear cases which ryml fails to parse. Your [bug reports or pull
requests](https://github.com/biojppm/rapidyaml/issues) are very
welcome.

See also [the roadmap](./ROADMAP.md) for a list of future work.


### Known limitations

ryml deliberately makes no effort to follow the YAML standard in the
following situations:

* ryml's tree does NOT accept containers as map keys: keys stored in
  the tree must always be scalars. HOWEVER, this is a limitation only
  of the final tree. The event-based parse engine DOES parse container
  keys, as it is is meant to be used by other programming languages to
  create their native data-structures, and it is fully tested and
  fully conformant (other than the general error permissiveness noted
  below).
* Tab characters after `:` and `-` are not accepted tokens, unless
  ryml is compiled with the macro `RYML_WITH_TAB_TOKENS`. This
  requirement exists because checking for tabs introduces branching
  into the parser's hot code and in some cases costs as much as 10%
  in parsing time.
* Non-unique map keys are allowed. Enforcing key uniqueness in the
  parser or in the tree would cause log-linear parsing complexity (for
  root children on a mostly flat tree), and would increase code size
  through added structural, logical and cyclomatic complexity. So
  enforcing uniqueness in the parser would hurt users who may not care
  about it (they may not care either because non-uniqueness is OK for
  their use case, or because it is impossible to occur). On the other
  hand, any user who requires uniqueness can easily enforce it by
  doing a post-parse walk through the tree. So choosing to not enforce
  key uniqueness adheres to the spirit of "don't pay for what you
  don't use".
* `%YAML` directives have no effect and are ignored.
* `%TAG` directives are limited to a default maximum of 4 instances
  per `Tree`. To increase this maximum, define the preprocessor symbol
  `RYML_MAX_TAG_DIRECTIVES` to a suitable value. This arbitrary limit
  reflects the usual practice of having at most 1 or 2 tag directives;
  also, be aware that this feature is under consideration for removal
  in YAML 1.3.
* ryml tends to be on the permissive side in several cases where the
  YAML standard dictates that there should be an error; in many of these
  cases, ryml will tolerate the input. This may be good or bad, but in
  any case is being improved on, meaning ryml will grow progressively
  less tolerant of YAML errors in the coming releases. So we strongly
  suggest to stay away from those dark corners of YAML which are
  generally a source of problems; this is good practice anyway.

If you do run into trouble and would like to investigate conformance
of your YAML code, **beware** of existing online YAML linters, many of
which are not fully conformant. Instead, try using
[https://play.yaml.io](https://play.yaml.io), an amazingly useful tool
which lets you dynamically input your YAML and continuously see the
results from all the existing parsers (kudos to @ingydotnet and the
people from the YAML test suite). And of course, if you detect
anything wrong with ryml, please [open an
issue](https://github.com/biojppm/rapidyaml/issues) so that we can
improve.


### Test suite status

As part of its CI testing, ryml uses the [YAML test
suite](https://github.com/yaml/yaml-test-suite). (See also the test
suite results at
[https://matrix.yaml.info/](https://matrix.yaml.info/), but be aware
that the results there may be using an older version of ryml.) This is
an extensive and merciless set of reference cases covering the full
YAML spec. Each of these cases has several subparts:
 * `in-yaml`: mildly, plainly or extremely difficult-to-parse YAML
 * `in-json`: equivalent JSON (where possible/meaningful)
 * `out-yaml`: equivalent standard YAML
 * `emit-yaml`: equivalent standard YAML
 * `events`: reference events according to the YAML standard

When testing, ryml parses each of the yaml/json parts, then emits the
parsed tree, then parses the emitted result and verifies that emission
is idempotent, ie that the round trip emitted result is semantically
the same as its input without any loss of information.

To ensure consistency, this happens over four successive levels of
parse->emit round trips. And to ensure correctness, each of the stages
is compared against the `events` spec from the test, which constitutes
the reference. The tests also check for equality between the reference
events in the test case and the events emitted by ryml from the data
tree parsed from the test case input. All of this is then carried out
with several variations: both unix `\n` vs windows `\r\n` line
endings, emitting to string, file or streams, which results in ~250
tests per case part.

With multiple parts per case and ~400 reference
cases in the test suite, this makes over several hundred thousand
individual tests to which ryml is subjected, which are added to the
unit tests in ryml, which also employ the same extensive combinatorial
approach.

Also, note that in [their own words](http://matrix.yaml.info/), the
tests from the YAML test suite *contain a lot of edge cases that don't
play such an important role in real world examples*. And yet, despite
the extreme focus of the test suite, currently ryml only fails a minor
fraction of the test cases, mostly related with the deliberate
limitations noted above.

Other than those limitations, by far the main issue with ryml is that
several standard-mandated parse errors fail to materialize (this will
be addressed in the coming releases). For the up-to-date list of ryml
failures in the test-suite, refer to the [list of known
exceptions](test/test_suite/test_suite_parts.cpp) from ryml's test
suite runner, which is used as part of ryml's CI setup.



------

## Alternative libraries

Why this library? Because none of the existing libraries was quite
what I wanted. When I started this project in 2018, I was aware of these two
alternative C/C++ libraries:

  * [libyaml](https://github.com/yaml/libyaml). This is a bare C
    library. It does not create a representation of the data tree, so
    I don't see it as practical. My initial idea was to wrap parsing
    and emitting around libyaml's convenient event handling, but to my
    surprise I found out it makes heavy use of allocations and string
    duplications when parsing. I briefly pondered on sending PRs to
    reduce these allocation needs, but not having a permanent tree to
    store the parsed data was too much of a downside.
  * [yaml-cpp](https://github.com/jbeder/yaml-cpp). This library may
    be full of functionality, but is heavy on the use of
    node-pointer-based structures like `std::map`, allocations, string
    copies, polymorphism and slow C++ stream serializations. This is
    generally a sure way of making your code slower, and strong
    evidence of this can be seen in the benchmark results above.

Recently [libfyaml](https://github.com/pantoniou/libfyaml)
appeared. This is a newer C library, fully conformant to the YAML
standard with an amazing 100% success in the test suite; it also offers
the tree as a data structure. As a downside, it does not work in
Windows, and it is also multiple times slower parsing and emitting.

When performance and low latency are important, using contiguous
structures for better cache behavior and to prevent the library from
trampling caches, parsing in place and using non-owning strings is of
central importance. Hence this Rapid YAML library which, with minimal
compromise, bridges the gap from efficiency to usability. This library
takes inspiration from
[RapidJSON](https://github.com/Tencent/rapidjson) and
[RapidXML](http://rapidxml.sourceforge.net/).

------
## License

ryml is permissively licensed under the [MIT license](LICENSE.txt).
 
