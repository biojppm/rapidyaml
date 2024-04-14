API documentation
=================

.. note::
   Please refer to the `Doxygen documentation
   <./doxygen/index.html>`_, and please read the `overview sample
   <doxygen/group__doc__quickstart.html#ga1118e0fb55403d0e061626cf8a07cc0c>`_;
   it will quickly teach you the basic concepts and caveats, which
   will save you a lot of time.


.. include:: sphinx_try_quickstart.rst

Here's a short selection from the quickstart overview (`see on
doxygen <doxygen/group__doc__quickstart.html>`_ / `see full code
on github <https://github.com/biojppm/rapidyaml/blob/v0.5.0/samples/quickstart.cpp>`_):

.. code-block:: c++
                 
    // Parse YAML code in place, potentially mutating the buffer:
    char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
    ryml::Tree tree = ryml::parse_in_place(yml_buf);

    // read from the tree:
    ryml::NodeRef bar = tree["bar"];
    CHECK(bar[0].val() == "2");
    CHECK(bar[1].val() == "3");
    CHECK(bar[0].val().str == yml_buf + 15); // points at the source buffer
    CHECK(bar[1].val().str == yml_buf + 17);

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
    bar.append_child() << 12;
    CHECK(bar[1].val() == "12");

    // emit to stdout
    std::cout << tree;

    // emit to std::string
    CHECK(emitrs_yaml<std::string>(tree) == "{foo: 1,bar: [10,11,12],john: doe}");
