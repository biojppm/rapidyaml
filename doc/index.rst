rapidyaml
=========


Or ryml, for short. ryml is a C++ library to parse and emit YAML,
and do it fast, on everything from x64 to bare-metal chips without
operating system.

And if you're wondering whether ryml's speed comes at the cost of
usability, you need not: with ryml, you can have your cake
and eat it too. Being rapid is definitely NOT the same as being
unpractical, so ryml was written with easy AND efficient usage in
mind.

ryml is available as a single header file, or it can be used as a
simple library with cmake -- both separately (ie
build -> install -> ``find_package()``) or together with your project (ie with
``add_subdirectory()``). (See below for examples).

ryml can use custom global and per-tree memory allocators and error
handler callbacks, and is exception-agnostic. ryml provides a default
implementation for the allocator (using ``std::malloc()``) and error
handlers (using either exceptions, ``longjmp()`` or ``std::abort()``),
but you can opt out of and provide your own memory allocation and
error callbacks.

For maximum portability, ryml does not depend on the STL, ie, it does
not use any std container as part of its data structures. But ryml can
serialize and deserialize these containers into the data tree, with
the use of optional headers. ryml ships with `c4core
<https://github.com/biojppm/c4core>`_, a small C++ utilities
multiplatform library.

ryml is written in C++11, and compiles cleanly with:

* Visual Studio 2015 and later

* clang++ 3.9 and later

* g++ 4.8 and later

* Intel Compiler

 .. note::
    If you are looking to use an YAML tree with override facilities
    as the configuration for your programs, take a look at `c4conf
    <https://github.com/biojppm/c4conf>`_.


Table of contents
=================

.. toctree::
   :maxdepth: 3

   ./sphinx_api_documentation
   ./sphinx_quicklinks
   ./sphinx_is_it_rapid
   ./sphinx_yaml_standard
   ./sphinx_using
   ./sphinx_other_languages
   ./sphinx_alternative_libraries
