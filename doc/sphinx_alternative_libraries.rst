Alternative libraries
=====================

Why this library? Because none of the existing libraries was quite what
I wanted. When I started this project in 2018, I was aware of these two
alternative C/C++ libraries:

-  `libyaml <https://github.com/yaml/libyaml>`__. This is a bare C
   library. It does not create a representation of the data tree, so I
   don’t see it as practical. My initial idea was to wrap parsing and
   emitting around libyaml’s convenient event handling, but to my
   surprise I found out it makes heavy use of allocations and string
   duplications when parsing. I briefly pondered on sending PRs to
   reduce these allocation needs, but not having a permanent tree to
   store the parsed data was too much of a downside.
-  `yaml-cpp <https://github.com/jbeder/yaml-cpp>`__. This library may
   be full of functionality, but is heavy on the use of
   node-pointer-based structures like ``std::map``, allocations, string
   copies, polymorphism and slow C++ stream serializations. This is
   generally a sure way of making your code slower, and strong evidence
   of this can be seen in the benchmark results above.

Recently `libfyaml <https://github.com/pantoniou/libfyaml>`__ appeared.
This is a newer C library, fully conformant to the YAML standard with an
amazing 100% success in the test suite; it also offers the tree as a
data structure. As a downside, it does not work in Windows, and it is
also multiple times slower parsing and emitting.

When performance and low latency are important, using contiguous
structures for better cache behavior and to prevent the library from
trampling caches, parsing in place and using non-owning strings is of
central importance. Hence this Rapid YAML library which, with minimal
compromise, bridges the gap from efficiency to usability. This library
takes inspiration from
`RapidJSON <https://github.com/Tencent/rapidjson>`__ and
`RapidXML <http://rapidxml.sourceforge.net/>`__.
