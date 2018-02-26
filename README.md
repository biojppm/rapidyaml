# Rapid YAML
[![MIT Licensed](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt)
[![Build Status](https://travis-ci.org/biojppm/rapidyaml.svg?branch=master)](https://travis-ci.org/biojppm/rapidyaml)
[![Build status](https://ci.appveyor.com/api/projects/status/github/biojppm/rapidyaml?branch=master&svg=true)](https://ci.appveyor.com/project/biojppm/rapidyaml)

(This is under development).

Or ryml, for short. ryml is a library to parse and emit YAML, and do it fast.

ryml parses the source buffer in place, and the resulting tree holds only
sub-ranges of the source buffer. No string copies or duplications are done, and
no virtual functions are used. Serialization happens only when the user wants
it to happen, so internally the data tree representation has no knowledge of
types.

ryml can use custom memory allocators, and is made exception-agnostic via a
custom error handler callback.

ryml does not depend on the STL, but can interact with it via `#include
<ryml_std.hpp>`.

ryml is written in C++11, and is known to compile with:
* Visual Studio 2013 and later
* clang++ 3.9 and later
* g++ 5 and later

## Quick start

Being fast doesn't need to mean being unpractical, AKA "you can have your cake
and eat it too".

Parsing from source:
```c++
#include <iostream>
#include <ryml.hpp>
int main()
{
    char src[] = "{foo: 1}"; // needs to be writable
    auto tree = ryml::parse(src);
    auto node = tree["foo"];
    std::cout << node.key() << "\n"; // "foo"
    std::cout << node.val() << "\n"; // "1"

    // deserializing:
    int foo;
    node >> foo; // now foo == 1
}
```

Creating a tree programatically:
```c++
ryml::Tree tree;
auto r = tree.rootref();

r |= ryml::MAP;  // this is needed to make the root a map

r["foo"] = "1"; // ryml works only with strings.
// Note that the tree will be __pointing__ at the
// strings "foo" and "1" used here. You need
// to make sure they have at least the same
// lifetime as the tree.

auto s = r["seq"]; // does not change the tree until s is written to.
s |= ryml::SEQ;
s.append_child() = "bar0"; // value of this child is now __pointing__ at "bar0"
s.append_child() = "bar1";
s.append_child() = "bar2";

// emit to stdout (can also emit to FILE* or ryml::span)
emit(tree); // prints the following:
            // foo: 1
            // seq:
            //  - bar0
            //  - bar1
            //  - bar2

// serializing: using operator<< instead of operator=
// will make the tree serialize the value into a char
// arena inside the tree. This arena can be reserved at will.
int ch3 = 33, ch4 = 44;
s.append_child() << ch3;
s.append_child() << ch4;

{
    std::string tmp = "child5";
    s.append_child() << tmp;
    // now tmp can go safely out of scope, as it was
    // serialized to the tree's internal string arena
}

emit(tree); // now prints the following:
            // foo: 1
            // seq:
            //  - bar0
            //  - bar1
            //  - bar2
            //  - 33
            //  - 44
            //  - child5

// to serialize keys:
r.append_child() << ryml::key(66) << 7;

emit(tree); // now prints the following:
            // foo: 1
            // seq:
            //  - bar0
            //  - bar1
            //  - bar2
            //  - 33
            //  - 44
            //  - child5
            // 66: 7
}
```


```c++
//To iterate over children:
for(auto c : node.children())
{
    std::cout << c.key() << "---" << c.val() << "\n";
}
//To iterate over siblings:
for(auto c : node.siblings())
{
    std::cout << c.key() << "---" << c.val() << "\n";
}
```
 

### STL interoperation

ryml does not require use of the STL. Use of STL is opt-in: you need to
`#include` the proper ryml header. Having done that, you can serialize them with a
single step. For example:

```c++
#include <ryml_std.hpp>
int main()
{
    std::map< std::string, int > m({{"foo", 1}, {"bar", 2}});
    ryml::Tree t;
    t.rootref() << m;
    
    emit(t);
    // foo: 1
    // bar: 2
    
    t["foo"] << 10;
    t["bar"] << 20;
    
    m.clear();
    t.rootref() >> m;
    
    std::cout << m["foo"] << "\n"; // 10
    std::cout << m["bar"] << "\n"; // 20
}
```

The `<ryml_std.hpp>` header includes every default std type implementation
for ryml. You can include just a specific header if you are interested only
in a particular container; these headers are located under a specific
directory in the source folder [c4/yml/std](src/c4/yml/std).

These headers also showcase how to implement your custom type. See for
example [the map implementation](src/c4/yml/std/map.hpp).

### Custom types

ryml comes stocked with code to serialize the basic intrinsic types (integers
and floating points). For types other than these, you need to instruct ryml
how to serialize your type.

There are two distinct categories of types when serializing to a YAML tree:

* The type is serializable to a string, and it will be a leaf node in
  the tree. For these, overload the `to_str()/from_str()` functions. An example
  can be seen in
  the [the `std::string` serialization code](src/c4/yml/std/string.hpp).

* The type requires child nodes (it is either a sequence or map).
  For these, overload the `write()/read()` functions. Examples can be seen in
  the serialization of [`std::vector`](src/c4/yml/std/vector.hpp)
  or [`std::map`](src/c4/yml/std/map.hpp).
  
It is important to overload these functions in the namespace where the type
you're serializing was defined, to
harness [C++'s ADL rules](http://en.cppreference.com/w/cpp/language/adl).

### Low-level API

... describe index-based API of the tree.

### Custom allocation

... describe [custom allocation callbacks](src/c4/yml/common.hpp)

### Custom error handling

... describe [the custom error handler callback](src/c4/yml/common.hpp)

## YAML standard conformance

ryml is under active development, but is close to feature complete. UTF8 has
not yet been tested.

The following core features are tested:
* mappings
* sequences
* complex keys
* literal blocks
* quoted scalars
* tags
* anchors and references

Of course, there are some dark corners in YAML, and in those cases I welcome
your bug reports or pull requests.

## How rapid is it?

... Show here.

## Alternative libraries

Why this library? Because none of the existing libraries was quite what I
wanted. There are two C/C++ libraries that I know of:

* [libyaml](https://github.com/yaml/libyaml)
* [yaml-cpp](https://github.com/jbeder/yaml-cpp)

The standard [libyaml](https://github.com/yaml/libyaml) is a bare C
library. It does not create a representation of the data tree, so it can't
qualify as practical. My initial idea was to wrap parsing and emitting
around libyaml, but to my surprise I found out it makes heavy use of
allocations and string duplications when parsing.

[yaml-cpp](https://github.com/jbeder/yaml-cpp) is full of functionality and
is very ably maintained, but is heavy on the use of streams, exceptions and
string copies and allocations.

For heavy or low-latency use cases, parsing in place and using non-owning
strings is of central importance. Hence this rapid YAML library, which
bridges the gap from efficiency to usability. This library takes inspiration
from RapidXML and RapidJSON.

## License

ryml is permissively licensed under the [MIT license](LICENSE.txt).

