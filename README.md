# Rapid YAML
[![MIT Licensed](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt)
[![Build Status](https://travis-ci.org/biojppm/rapidyaml.svg?branch=master)](https://travis-ci.org/biojppm/rapidyaml)
[![Build status](https://ci.appveyor.com/api/projects/status/github/biojppm/rapidyaml?branch=master&svg=true)](https://ci.appveyor.com/project/biojppm/rapidyaml)
[![Coverage: coveralls](https://coveralls.io/repos/github/biojppm/rapidyaml/badge.svg)](https://coveralls.io/github/biojppm/rapidyaml)
[![Coverage: codecov](https://codecov.io/gh/biojppm/rapidyaml/branch/master/graph/badge.svg)](https://codecov.io/gh/biojppm/rapidyaml)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/biojppm/rapidyaml.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/biojppm/rapidyaml/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/biojppm/rapidyaml.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/biojppm/rapidyaml/context:cpp)


Or ryml, for short. ryml is a library to parse and emit YAML, and do it fast.

ryml parses the source buffer in place, and the resulting tree holds only
sub-ranges of the source buffer. No string copies or duplications are done, and
no virtual functions are used. Serialization happens only when the user wants
it to happen, so internally the data tree representation has no knowledge of
types. The resulting tree is easy and fast to iterate through and change.

ryml can use custom memory allocators, and is made exception-agnostic via a
custom error handler callback.

ryml does not depend on the STL, but can interact with it via `#include
<ryml_std.hpp>`.

ryml is written in C++11, and is known to compile with:
* Visual Studio 2015 and later
* clang++ 3.9 and later
* g++ 5 and later


## Rapid? How rapid is it?

Quite a lot, actually! The first microbenchmark results are in. On my i7-6800K CPU @
3.40GHz, I get:
 * ryml parses YAML at about ~150MB/s on Linux and ~100MB/s on Windows. 
 * But it gets better. **ryml parses JSON at about 450MB/s on Linux**
   (didn't try yet on Windows).

[Here's the benchmark](./bm/parse.cpp). Using different approaches within
ryml (in-situ/read-only vs. with/without reuse), a YAML / JSON buffer is
repeatedly parsed, comparing with [yaml-cpp](https://github.com/jbeder/yaml-cpp). I'll eventually
compare and systematize the results also against libyaml, RapidJSON and other
alternatives, but for now the existing figures are very encouraging.

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
file](./bm/cases/compile_commands.yml). You
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
ballpark, allbeit at its lower end. This is something to be proud of, as YAML
is much more complex than JSON.


## Quick start

You can have your cake and eat it too: being rapid doesn't mean being
unpractical!

ryml was written with easy usage in mind, and comes with a two level
API for accessing and traversing the data tree: a low-level index-based API
and a higher level wrapping it via a ``NodeRef`` class. The examples in this
section are using the high-level ``NodeRef``.

Parsing from source:
```c++
#include <ryml.hpp>
// not needed by ryml, just for these examples
#include <iostream>
int main()
{
    // ryml can parse in situ (and read-only buffers too):
    char src[] = "{foo: 1}";
    // there are also overloads for reusing the tree and parser
    ryml::Tree tree = ryml::parse(src);

    // get a reference to the "foo" node
    ryml::NodeRef node = tree["foo"];

    // "foo: 1"
    std::cout << node.key() << ": " << node.val() << "\n";

    // deserializing:
    int foo;
    node >> foo; // now foo == 1
}
```

It is also possible to parse constant buffers, but before parsing ryml will
copy these buffers over to an arena buffer in the tree object, and modify
those while parsing:
```c++
// "{foo: 1}" is a read-only buffer; it will be copied to the tree's arena
ryml::Tree tree = ryml::parse("{foo: 1}");
```

`node.key()` and `node.val()` return an object of type `c4::csubstr` (the
name comes from constant substring) which is a read-only string view, with
some more methods that make it practical to use. There's also a writable
`c4::substr` string view, which in fact is used heavily by ryml to transform
YAML blocks and scalars during parsing. You can browse these classes
here: [c4/substr.hpp](https://github.com/biojppm/c4core/blob/master/src/c4/substr.hpp).


To create a tree programatically:
```c++
ryml::Tree tree;
NodeRef r = tree.rootref();

// Each container node must be explicitly set (either MAP or SEQ):
r |= ryml::MAP;

r["foo"] = "1"; // ryml works only with strings.
// Note that the tree will be __pointing__ at the
// strings "foo" and "1" used here. You need
// to make sure they have at least the same
// lifetime as the tree.

// does not change the tree until s is written to.
ryml::NodeRef s = r["seq"]; // here, s is not valid()
s |= ryml::SEQ; // now s is valid()

s.append_child() = "bar0"; // this child is now __pointing__ at "bar0"
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


To iterate over children:
```c++
for(NodeRef c : node.children())
{
    std::cout << c.key() << "---" << c.val() << "\n";
}
```

To iterate over siblings:
```c++
for(NodeRef c : node.siblings())
{
    std::cout << c.key() << "---" << c.val() << "\n";
}
```

For container-type nodes, the square-bracket operator can
be used either with integers (both for sequence and map nodes) or with
strings (only for map nodes):
```c++
ryml::Tree = ryml::parse("[a, b, {c: 0, d: 1}]");
ryml::NodeRef root = tree.rootref();
std::cout << root[0].val() << "\n"; // "a"
std::cout << root[1].val() << "\n"; // "b"
std::cout << root[2][ 0 ].val() << "\n"; // "0" // index-based
std::cout << root[2]["c"].val() << "\n"; // "0" // string-based
std::cout << root[2][ 1 ].val() << "\n"; // "1" // index-based
std::cout << root[2]["d"].val() << "\n"; // "1" // string-based
```

What about `NodeRef`? Let's consider when a non-existing key or index is
requested via the square-bracket operator. Unlike with `std::map`, **this
operator does not modify the tree**. Instead you get a seed `NodeRef`, and
the tree will be modified only when this seed-state reference is written
to. Thus `NodeRef` can either point to a valid tree node, or if no such node
exists it will be in seed-state by holding the index or name passed to
`operator[]`. To allow for this, `NodeRef` is a simple structure with a
declaration like:

```c++
class NodeRef
{
    // a pointer to the tree
    Tree * m_tree; 
    
    // either the (tree-scoped) index of an existing node or the (node-scoped) index of a seed state
    size_t m_node_or_seed_id;
    
    // the key name of a seed state
    const char* m_seed_name;

public:

    // this can be used to query whether a node is in seed state
    bool valid() { return m_node_id != NONE && m_node_name == nullptr; }

    // forward all calls to m_tree. For example:
    csubstr val() const { assert(valid()); return m_tree->val(m_node_or_seed_id); }
    void set_val(csubstr v) { if(!valid()) {/*create node in tree*/;} m_tree->set_val(m_node_or_seed_id, v); }

    // etc...
};
```

So using this high-level API is not going to cost a lot more than the less
practical low level API.


### STL interoperation

ryml does not require use of the STL. Use of STL is opt-in: you need to
`#include` the proper ryml header. Having done that, you can serialize them
with a single step. For example:

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
directory in the ryml source folder: [c4/yml/std](src/c4/yml/std).

These headers also showcase how to implement your custom type. See for
example [the map implementation](src/c4/yml/std/map.hpp).


### Custom types

ryml comes stocked with code to serialize the basic intrinsic types (integers
and floating points). For types other than these, you need to instruct ryml
how to serialize your type.

There are two distinct type categories when serializing to a YAML tree:

* Container types requiring child nodes (ie, either sequences or maps). For
  these, overload the `write()/read()` functions. For example,
  ```c++
  namespace foo {
  struct MyStruct; // a container-type struct 
  {
      int subject;
      std::map<std::string, int> counts;
  };
  
  // ... will need these functions to convert to YAML:
  void write(c4::yml::NodeRef *n, MyStruct const& v);
  void  read(c4::yml::NodeRef const& n, MyStruct *v);
  } // namespace foo
  ```
  which could be implemented as:
  ```c++
  #include <c4/yml/std/map.hpp>
  #include <c4/yml/std/string.hpp>
  
  void foo::read(c4::yml::NodeRef const& n, MyStruct *v)
  {
      n["subject"] >> v->subject;
      n["counts"] >> v->counts;
  }
  
  void foo::write(c4::yml::NodeRef *n, MyStruct const& v)
  {
      *n |= c4::yml::MAP;
      
      NodeRef ch = n->append_child();
      ch.set_key("subject");
      ch.set_val_serialized(v.subject);
      
      ch = n->append_child();
      ch.set_key("counts");
      write(&ch, v.counts);
  }
  ```
  To harness [C++'s ADL rules](http://en.cppreference.com/w/cpp/language/adl),
  it is important to overload these functions in the namespace where the type
  you're serializing was defined (or in the c4::yml namespace). Generic
  examples can be seen in the (optional) implementations of `std::vector`
  or `std::map`, at their respective headers 
  [`c4/yml/std/vector.hpp`](src/c4/yml/std/vector.hpp) and
  [`c4/yml/std/map.hpp`](src/c4/yml/std/map.hpp).

* The second category is for types which should serialize to a string,
  resulting in leaf node in the YAML tree. For these, overload the
  `to_str(c4::substr, T)/from_str(c4::csubstr,
  *T)` functions. Here's an example for a 3D vector type:
  ```c++
  struct vec3 { float x, y, z; };

  // format v to the given string view + return the number of
  // characters written into it. The view size (buf.len) must
  // be strictly respected. Return the number of characters
  // that need to be written. So if the return value
  // is larger than buf.len, ryml will resize the buffer and
  // call this again with a larger buffer.
  size_t to_str(c4::substr buf, vec3 v)
  {
      // this call to c4::format() is a type-safe version
      // of snprintf(buf.str, buf.len, "(%f,%f,%f)", v.x, v.y, v.z)
      return c4::format(buf, "({},{},{})", v.x, v.y, v.z);
  }

  bool from_str(c4::csubstr buf, vec3 *v)
  {
      // equivalent to sscanf(buf.str, "(%f,%f,%f)", &v.x, &v.y, &v.z)
      // --- actually snscanf(buf.str, buf.len, ...) but there's
      // no such function in the standard.
      size_t ret = c4::unformat(buf, "({},{},{})", v.x, v.y, v.z);
      return ret != c4::csubstr::npos;
  }
  ```
   You can also look at [the `std::string` serialization code](https://github.com/biojppm/c4core/blob/master/src/c4/std/string.hpp).


### Low-level API

Some examples:

```c++
void print_keyval(Tree const& t, size_t elm_id)
{
    std::cout << t.get_key(elm_id)
              << ": "
              << t.get_val(elm_id) << "\n";
}

ryml::Tree t = parse("{foo: 1, bar: 2, baz: 3}")

size_t root_id = t.root_id();
size_t foo_id  = t.first_child(root_id);
size_t bar_id  = t.next_sibling(foo_id);
size_t baz_id  = t.last_child(root_id);

assert(baz == t.next_sibling(ibar));
assert(bar == t.prev_sibling(ibaz));

print_keyval(t, foo_id); // "foo: 1"
print_keyval(t, bar_id); // "bar: 2"
print_keyval(t, baz_id); // "baz: 3"

// to iterate over the children of a node:
for(size_t i = t.first_child(root_id); i != ryml::NONE; i = t->next_sibling(i))
{
    // ...
}

// to iterate over the siblings of a node:
for(size_t i = t.first_sibling(foo_id); i != ryml::NONE; i = t->next_sibling(i))
{
    // ...
}
```


### Custom allocators and error handlers

ryml accepts your own allocators and error handlers. Read through
[this header file](src/c4/yml/common.hpp) to set it up.


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

Of course, there are some dark corners in YAML, and there certainly can
appear many cases which YAML fails to parse. So we welcome
your bug reports or pull requests!

I am currently working on integrating the ~300 cases in the YAML
test suite.


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

