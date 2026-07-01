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
100.00% of the cases in the YAML test suite. The parser is
state-machine based and not recursive. No known vulnerabilities
exist. The code is robust and extremely tested and fuzzed.

ryml parses both read-only and in-place source buffers; the resulting
tree holds only views to sub-ranges of the source buffer. No string
copies or duplications are done, and no virtual functions are
used. The data tree is a flat index-based structure stored in a single
array.

With the tree, (de)serialization happens only at your direct request,
after parsing / before emitting. Internally, the tree representation
stores only string views and assumes nothing on user types. And it is
not just parsing or emitting which is fast; the serialization is
extremely fast, in many cases faster than the fastest C++ facilities
like `std::to_chars()`. ryml makes it easy and fast to read and modify
the data tree.


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
as part of its data structures, but it can serialize and deserialize
these containers into the data tree, with the use of optional
headers. ryml ships with [c4core](https://github.com/biojppm/c4core),
a small C++ utilities multiplatform library, but you can ignore the
in-source version of c4core and use a custom or system-installed version.

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
## License

ryml is permissively licensed under the [MIT license](LICENSE.txt).


------

## Table of contents
* [License](#license)
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
nicely as claimed above, we would definitely like to see it. Please
open an issue, or submit a pull request adding the file to
[bm/cases](bm/cases), or just send us the files.


------

## Quick start

If you're wondering whether ryml's speed comes at a convenience cost, you
need not: with ryml, you can have your cake and eat it too. Being
rapid is definitely not the same as being unpractical, and ryml was
written with easy and efficient usage in mind:

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

See [the quickstart sample](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__quickstart.html). After cloning ryml
you can easily build and run this executable using any of the build samples, eg the
[`add_subdirectory()` sample](samples/add_subdirectory/) (see [the relevant section](#quickstart-samples)).


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
usage: amalgamate.py [-h] [--c4core | --no-c4core] [--fastfloat | --no-fastfloat]
                     [--stl | --no-stl]
                     [-e {tree,testsuite,int,all,none} [{tree,testsuite,int,all,none} ...]]
                     [output]

positional arguments:
  output                output file. defaults to stdout

options:
  -h, --help            show this help message and exit
  --c4core              amalgamate c4core together with ryml. this is the default.
  --no-c4core           amalgamate c4core together with ryml. the default is
                        --c4core.
  --fastfloat           enable fastfloat library. this is the default.
  --no-fastfloat        enable fastfloat library. the default is --fastfloat.
  --stl                 enable stl interop. this is the default.
  --no-stl              enable stl interop. the default is --stl.
  -e, --events {tree,testsuite,int,all,none} [{tree,testsuite,int,all,none} ...]
                        Specify which event handlers to include. Possible values
                        are: 'tree': (the default) enable the normal ryml event
                        handler to create the tree, and additionally the Tree, Node,
                        parser and emitter utilities; if this is not enabled, none
                        of these components will be included in the amalgamated
                        file. 'testsuite': enable the (extra) YAML test suite event
                        handler. 'int': enable the (extra) integer-based event
                        handler. 'all': enable all event handlers. 'none': disable
                        all event handlers. The default is tree.
```

Note that you can select which event handlers are to be included in the
amalgamated header. This is useful for example when using only the int
event parsing (withou the ryml tree/node/parse/emit) for a programming
language or special application.

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
  * [brew](https://github.com/Homebrew/homebrew-core/blob/master/Formula/r/rapidyaml.rb)
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
| [`singleheader-ints`](./samples/singleheader-ints) | **yes**<br>ryml brought as a single header file,<br>not as a library | [`CMakeLists.txt`](./samples/singleheader-ints/CMakeLists.txt) | [`run.sh`](./samples/singleheader-ints/run.sh) |
| [`singleheaderlib-ints`](./samples/singleheaderlib-ints) | **yes**<br>ryml brought as a library<br>but from the single header file | [`CMakeLists.txt`](./samples/singleheaderlib-ints/CMakeLists.txt) | [`run_shared.sh` (shared library)](./samples/singleheaderlib-ints/run_shared.sh)<br> [`run_static.sh` (static library)](./samples/singleheaderlib-ints/run_static.sh) |
| [`add_subdirectory`](./samples/add_subdirectory) | **yes**                      | [`CMakeLists.txt`](./samples/add_subdirectory/CMakeLists.txt) | [`run.sh`](./samples/add_subdirectory/run.sh) |
| [`fetch_content`](./samples/fetch_content)      | **yes**                      | [`CMakeLists.txt`](./samples/fetch_content/CMakeLists.txt) | [`run.sh`](./samples/fetch_content/run.sh) |
| [`find_package`](./samples/find_package)        | **no**<br>needs prior install or package  | [`CMakeLists.txt`](./samples/find_package/CMakeLists.txt) | [`run.sh`](./samples/find_package/run.sh) |
| [`ints_only`](./samples/ints_only)        | **yes**<br>but custom selection of ryml files: only ints parsing  | [`CMakeLists.txt`](./samples/ints_only/CMakeLists.txt) | [`run.sh`](./samples/ints_only/run.sh) |


### CMake build settings for ryml

See the [relevant documentation page](https://rapidyaml.readthedocs.io/latest/sphinx_using.html#cmake-build-settings-for-ryml).


------

## Other languages

One of the aims of ryml is to provide an efficient YAML API for other
languages. JavaScript is fully available, and there is already a
cursory implementation for Python using only the low-level API. After
ironing out the general approach, other languages are likely to follow
suit.


### Event buffer int handler

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

You can find the int event handler in the [`src_extra` source
folder](https://github.com/biojppm/rapidyaml/tree/master/src_extra). See
its doxygen documentation for details on how to use it, and how to
process the event array.


### JavaScript

A JavaScript+WebAssembly port is available, compiled through [emscripten](https://emscripten.org/).


### Python

There is a blazing fast rapidyaml Python package; it now lives in its
own [dedicated repo](https://github.com/biojppm/rapidyaml-python).



------

## YAML standard conformance

ryml is fully conformant and feature-complete with regards to the YAML
specification, and it passes all of the cases in the YAML test
suite. All the YAML features are well covered in the unit tests, and
expected to work, unless in the exceptions noted below.

Of course, there are many dark corners in YAML, and there certainly
can appear cases which ryml fails to parse. If you find any case where
ryml fails, your [bug reports or pull
requests](https://github.com/biojppm/rapidyaml/issues) are very
welcome.


### Known limitations

ryml deliberately makes no effort to follow the YAML standard in the
following situations:

* ryml's parser engine correctly handles all YAML features. But the 
  destination tree does NOT accept containers as map keys: map keys
  must always be scalars. But note that this is a limitation
  only of the final tree.
* Tab characters after `:` and `-` are not accepted tokens, unless
  ryml is compiled with the macro `RYML_WITH_TAB_TOKENS`. This
  requirement exists because checking for tabs introduces branching
  into the parser's hot code and in some cases costs as much as 10%
  in parsing time.
* Non-unique map keys are allowed. Enforcing key uniqueness in the
  parser or in the tree would cause log-linear parsing complexity (for
  root children on a mostly flat tree), and would increase code size
  through added structural, logical and cyclomatic
  complexity. Enforcing uniqueness in the parser would hurt users who
  may not care about it (they may not care either because
  non-uniqueness is OK for their use case, or because it is impossible
  to occur). On the other hand, any user who requires uniqueness can
  easily enforce it by doing a post-parse walk through the tree. So
  choosing to not enforce key uniqueness adheres to the spirit of
  "don't pay for what you don't use".
* `%YAML` directives have no effect and are ignored.
* `%TAG` directives are limited to a default maximum of 4 instances
  per `Tree`. To increase this maximum, define the preprocessor symbol
  `RYML_MAX_TAG_DIRECTIVES` to a suitable value. This arbitrary limit
  reflects the usual practice of having at most 1 or 2 tag directives;
  also, be aware that this feature is under consideration for removal
  in YAML 1.3.
* Byte Order Marks: while ryml correctly handles BOMs at the beginning
  of the stream or documents (as per the standard), BOMs inside
  scalars are ignored. The [standard mandates that they should be
  quoted](https://yaml.org/spec/1.2.2/#52-character-encodings) when
  emitted; this is not done.

If you do run into trouble and would like to investigate conformance
of your YAML code, **beware** of existing online YAML linters, many of
which are not fully conformant. Instead, try using
[https://play.yaml.com](https://play.yaml.com/), an amazingly useful tool
which lets you dynamically input your YAML and continuously see the
results from all the existing parsers (kudos to @ingydotnet and the
people from the YAML test suite). And of course, if you detect
anything wrong with ryml, please [open an
issue](https://github.com/biojppm/rapidyaml/issues) so that we can
improve.


### Test suite status

As part of its CI testing, ryml uses and passes all the cases from the
[YAML test suite](https://github.com/yaml/yaml-test-suite). (See also
the test suite results at
[https://matrix.yaml.info/](https://matrix.yaml.info/), but be aware
that the results there may be using an older version of ryml.) This is
an extensive and merciless set of reference cases covering the YAML
specification. Each of these cases has several subparts:
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
unit tests in ryml, also employing the same extensive combinatorial
approach.

Also, note that in [their own words](http://matrix.yaml.info/), the
tests from the YAML test suite *contain a lot of edge cases that don't
play such an important role in real world examples*. And yet, despite
the extreme focus of the test suite, ryml passes all of its test cases.
