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

ryml is available both as a single header file, or as a simple library
with cmake -- both separately (ie build -> install ->
``find_package()``) or together with your project (ie with
``add_subdirectory()``). (See below for examples).

ryml can use custom global and per-tree memory allocators and error
handler callbacks, and is exception-agnostic. ryml provides a default
implementation for the allocator (using ``std::malloc()``) and error
handlers (using either exceptions or ``std::abort()``), but you can
opt out the default implementation and provide your own memory
allocation and error callbacks.

For maximum portability, ryml does not depend on the STL, ie, it does
not use any std container as part of its data structures. But ryml can
serialize and deserialize these containers into the data tree, with
the use of optional headers. ryml ships with `c4core
<https://github.com/biojppm/c4core>`_, a small C++ utilities
multiplatform library.

ryml is written in C++11, and compiles cleanly with:

* Visual Studio 2015 and later

* g++ 4.8 and later

* clang++ 3.9 and later

* Intel Compiler

 .. note::
    If you are looking to use an YAML tree with override facilities
    as the configuration for your programs, take a look at `c4conf
    <https://github.com/biojppm/c4conf>`_.


----

Table of contents
=================

.. toctree::
   :maxdepth: 3

   Doxygen docs <doxygen/index.html#http://>
   YAML playground <https://play.yaml.io/main/parser?input=IyBFZGl0IE1lIQoKJVlBTUwgMS4yCi0tLQpmb286IEhlbGxvLCBZQU1MIQpiYXI6IFsxMjMsIHRydWVdCmJhejoKLSBvbmUKLSB0d28KLSBudWxsCg==>
   ./sphinx_quicklinks
   ./sphinx_is_it_rapid
   ./sphinx_yaml_standard
   ./sphinx_using
   ./sphinx_other_languages
   ./sphinx_alternative_libraries

----

API teaser
==========

Here's a short teaser from the API quickstart overview (`see on
doxygen <doxygen/group__doc__quickstart.html>`_ / `see full code on
github
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/quickstart.cpp>`_):

.. code-block:: c++

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
    bar[0] >> bar0;
    bar[1] >> bar1;
    CHECK(bar0 == 2);
    CHECK(bar1 == 3);

    // serializing:
    bar[0] << 10; // creates a string in the tree's arena
    bar[1] << 11;
    CHECK(bar[0].val() == "10");
    CHECK(bar[1].val() == "11");

    // add nodes
    bar.append_child() << 12; // see also operator= (explanation below)
    CHECK(bar[2].val() == "12");

    // emit tree
    // to std::string
    CHECK(ryml::emitrs_yaml<std::string>(tree) == R"(foo: 1
    bar:
      - 10
      - 11
      - 12
    john: doe
    )");
    std::cout << tree; // emit to stdout
    ryml::emit_yaml(tree, stdout); // emit to file

    // emit node
    ryml::ConstNodeRef foo = tree["foo"];
    // to std::string
    CHECK(ryml::emitrs_yaml<std::string>(foo) == "foo: 1\n");
    std::cout << foo; // emit node to stdout
    ryml::emit_yaml(foo, stdout); // emit node to file


.. note::
   Please refer to the `Doxygen documentation
   <./doxygen/index.html>`_, and read the `overview sample
   <doxygen/group__doc__quickstart.html#ga1118e0fb55403d0e061626cf8a07cc0c>`_;
   this will quickly teach you the basic concepts and caveats, which
   will save you a lot of time.

.. include:: sphinx_try_quickstart.rst
