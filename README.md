# Rapid YAML
[![MIT Licensed](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/biojppm/rapidyaml/blob/master/LICENSE.txt)
[![Build Status](https://travis-ci.org/biojppm/rapidyaml.svg?branch=master)](https://travis-ci.org/biojppm/rapidyaml)
[![Build status](https://ci.appveyor.com/api/projects/status/github/biojppm/rapidyaml?branch=master&svg=true)](https://ci.appveyor.com/project/biojppm/rapidyaml)
[![Coverage: coveralls](https://coveralls.io/repos/github/biojppm/rapidyaml/badge.svg)](https://coveralls.io/github/biojppm/rapidyaml)
[![Coverage: codecov](https://codecov.io/gh/biojppm/rapidyaml/branch/master/graph/badge.svg)](https://codecov.io/gh/biojppm/rapidyaml)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/biojppm/rapidyaml.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/biojppm/rapidyaml/alerts/)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/biojppm/rapidyaml.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/biojppm/rapidyaml/context:cpp)


Or ryml, for short. ryml is a library to parse and emit YAML, and do it fast.

ryml parses both read-only and in-situ source buffers; the resulting data
nodes hold only views to sub-ranges of the source buffer. No string copies or
duplications are done, and no virtual functions are used. The data tree is a
flat index-based structure stored in a single array (which you can reserve at
will). Serialization happens only at the user's request after parsing /
before emitting; internally the data tree representation has no knowledge of
types (but of course, every node can have a YAML type tag). The resulting
tree is easy and fast to iterate through and change.

ryml can use custom per-tree memory allocators, and is
exception-agnostic. Errors are reported via a custom error handler callback
(and a default implementation using std::abort is provided, but you can opt
out).

ryml has respect for your compilation times and therefore it is NOT
header-only. (Work is still needed on installing it, but you can just
add it as a subdirectory of your CMake project).

ryml has no dependencies, not even on the STL (although it does use the
libc). But it provides optional headers that let you serialize/deserialize
STL strings and containers.

ryml is written in C++11, and is known to compile with:
* Visual Studio 2015 and later
* clang++ 3.9 and later
* g++ 5 and later


------

## Is it rapid?

You bet!

This project is fairly new, and only recently there was the change to
benchmark it. The results are extremely satisfying. On a i7-6800K CPU @
3.40GHz:
 * ryml parses YAML at about ~150MB/s on Linux and ~100MB/s on Windows (vs2017). 
 * **ryml parses JSON at about 450MB/s on Linux**, faster than sajson (didn't
   try yet on Windows).
 * compared against the other existing YAML libraries for C/C++:
   * ryml is in general between 2 and 3 times faster than [libyaml](https://github.com/yaml/libyaml)
   * ryml is in general between 20 and 70 times faster than [yaml-cpp](https://github.com/jbeder/yaml-cpp)

(The results presented below are a bit scattered; and they need to be
sistematized.) [Here's the benchmark](./bm/parse.cpp). Using different
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
ballpark, allbeit at its lower end. This is something to be proud of, as the
YAML specification is much more complex than JSON.


### Performance reading JSON

So how does ryml compare against other JSON readers? Well, it's one of the
fastest! 

The benchmark is the [same as above](./bm/parse.cpp), and it is reading
the [compile_commands.json](./bm/cases/compile_commands.json), The `_ro`
suffix notes parsing a read-only buffer (so buffer copies are performed),
while the `_rw` suffix means that the source buffer can be parsed in
situ. The `_reuse` means the data tree and/or parser are reused on each
benchmark repeat.

Here's what we get with g++ 8.2:

```
|------------------|-------------------------------|-------------------------------
|                  |           Release             |           Debug               
| Benchmark        |  Iterations    Bytes/sec      |  Iterations    Bytes/sec      
|------------------|-------------------------------|-------------------------------
| rapidjson_ro     |        7941    509.855M/s     |         633    43.3632M/s     
| rapidjson_rw     |       21400    1.32937G/s     |        1067    68.171M/s     
| sajson_rw        |        6808    434.245M/s     |        2770    176.478M/s     
| sajson_ro        |        6726    430.723M/s     |        2748    175.613M/s     
| jsoncpp_ro       |        2871    183.616M/s     |        2941    187.937M/s     
| nlohmann_json_ro |        1807    115.801M/s     |         337    21.5237M/s     
| yamlcpp_ro       |         261    16.6322M/s     |          25    1.58178M/s     
| libyaml_ro       |        1786    113.909M/s     |         560    35.6599M/s     
| libyaml_ro_reuse |        1797    114.594M/s     |         561    35.8531M/s     
| ryml_ro          |        6088    388.585M/s     |         576    36.8634M/s     
| ryml_rw          |        6179    393.658M/s     |         577    36.8474M/s     
| ryml_ro_reuse    |        6986    446.248M/s     |        1164    74.636M/s      
| ryml_rw_reuse    |        7157    457.076M/s     |        1175    74.8721M/s     
|------------------|-------------------------------|-------------------------------
```

You can verify that (for this test) ryml beats most json parsers at their own
game, with the notable exception
of [rapidjson](https://github.com/Tencent/rapidjson) --- *but this exception
only occurs when in Release mode*. When in Debug
mode, [rapidjson](https://github.com/Tencent/rapidjson) is slower than ryml,
and only [sajson](https://github.com/chadaustin/sajson) manages to be
faster. 

More json comparison benchmarks will be added, but seem unlikely to
significantly alter these results.


------

## Quick start

If you're wondering whether ryml's speed comes at a usage cost, you need
not. With ryml, you can have your cake and eat it too: being rapid is
definitely NOT the same as being unpractical! ryml was written with easy AND
efficient usage in mind, and comes with a two level API for accessing and
traversing the data tree.

The low-level interface is an index-based API available through
the [`ryml::Tree`](src/c4/yml/tree.hpp) class (see examples below). This
class is essentially a contiguous array of `NodeData` elements; these are
linked to parent, children and siblings via indices.

On top of this index-based API, there is a thin
abstraction [`ryml::NodeRef`](src/c4/yml/node.hpp) which is essentially a
non-owning pointer to a `NodeData` element. It provides convenient methods
for accessing the `NodeData` properties wrapping it via a class allowing for
a more object-oriented use.


### Parsing

A parser takes a source buffer and fills
a [`ryml::Tree`](src/c4/yml/tree.hpp) object:

```c++
#include <ryml.hpp>

// not needed by ryml, just for these examples (and below)
#include <iostream>
// convenience functions to print a node
void show_keyval(ryml::NodeRef n)
{
    assert(n.has_keyval());
    std::cout << n.key() << ": " << n.val() << "\n";
}
void show_val(ryml::NodeRef n)
{
    assert(n.has_val());
    std::cout << n.val() << "\n";
}
    
int main()
{
    // ryml can parse in situ (and read-only buffers too):
    char src[] = "{foo: 1, bar: [a: 2, b: 3]}";
    // there are also overloads for reusing the tree and parser
    ryml::Tree tree = ryml::parse(src);

    // get a reference to the "foo" node
    ryml::NodeRef node = tree["foo"];

    show_keyval(node);  // "foo: 1"
    show_val(node["bar"][0]);  // "2"
    show_val(node["bar"][1]);  // "3"

    // deserializing:
    int foo;
    node >> foo; // now foo == 1
}
```

It is also possible to parse read-only buffers, but note these will be copied
over to an arena buffer in the tree object, and that buffer copy will be the
one to be parsed:

```c++
// "{foo: 1}" is a read-only buffer; it will be
// copied to the tree's arena before parsing
ryml::Tree tree = ryml::parse("{foo: 1}");
```

When parsing, you can reuse the existing trees and parsers. You can also
parse into particular tree nodes, so that you can parse an entire file into a
node which is deep in the hierarchy of an existing tree. To see the various
overloads for parser, consult
the [c4/yml/parse.hpp header](src/c4/yml/parse.hpp).
The free-standing `parse()`
functions (towards the end of the file) are just convenience wrappers for
calling the several `Parser::parse()` overloads.


### Browsing the tree

The data tree is an index-linked array of `NodeData` elements. These are
defined roughly as (browse the [c4/yml/tree.hpp header](src/c4/yml/tree.hpp)):

```c++
// (inside namespace c4::yml)

typedef enum : int // bitflags for marking node features
{
   KEY=1<<0,
   VAL=1<<1,
   MAP=1<<2,
   SEQ=1<<4,
   DOC=1<<5,
   TAG=...,
   REF=...,
   ANCHOR=..., // etc 
} NodeType_e;
struct NodeType
{
   NodeType_e m_flags;
   // ... predicate methods such as
   // has_key(), is_map(), is_seq(), etc
};
struct NodeScalar // this is both for keys and vals
{
    csubstr tag;
    csubstr scalar;
    csubstr anchor;
    // csubstr is a constant substring:
    // a non-owning read-only string view
    // consisting of a pointer and a length
}
constexpr const size_t NONE = (size_t)-1;
struct NodeData
{
    NodeType   type;
    NodeScalar key; // data for the key (if applicable)
    NodeScalar val; // data for the value
    
    size_t     parent;      // NONE when this is the root node
    size_t     first_child; // NONE if this is a leaf node
    size_t     last_child;  // etc
    size_t     next_sibling;
    size_t     prev_sibling;
}
```

Please note that you should not rely on this particular structure; the above
definitions are given only to provide an idea on how the tree is structured.
To access and modify node properties, please use the APIs provided through
the Tree (low-level) or the NodeRef (high-level) classes.

You may have noticed above the use of a `csubstr` class. This class is
defined in another library, [c4core](https://github.com/biojppm/c4core),
which is imported by ryml (so technically it's not a dependency, is
it?). This is a library I use with my projects consisting of multiplatform
low-level utilities. One of these is `c4::csubstr` (the name comes from
"constant substring") which is a non-owning read-only string view, with many
methods that make it practical to use (I would certainly argue more practical
than `std::string`). (In fact, `c4::csubstr` and its writeable counterpart
`c4::substr` are the workhorses of the ryml parsing and serialization code;
you can browse these classes here:
[c4/substr.hpp](https://github.com/biojppm/c4core/blob/master/src/c4/substr.hpp).)

Now, let's parse and go through a tree. To obtain a `NodeRef` from the tree,
you only need to invoke `operator[]`. This operator can take indices (when
invoked on sequence and map nodes) and also strings (only when invoked on map
nodes):

```c++
ryml::Tree tree = ryml::parse("[a, b, {c: 0, d: 1}]");

// note: show_val() was defined above

show_val(tree[0]); // "a"
show_val(tree[1]); // "b"
show_val(tree[2][ 0 ]); // "0" // index-based
show_val(tree[2][ 1 ]); // "1" // index-based
show_val(tree[2]["c"]); // "0" // string-based
show_val(tree[2]["d"]); // "1" // string-based

// note that trying to obtain the value on a non-value
// node such as a container will fail an assert:
// ERROR, assertion triggered: a container has no value
show_val(tree[2]);
// ERROR: the same
show_val(tree.rootref());

// the same for keys:
show_keyval(tree[0]); // ERROR: sequence element has no key
show_keyval(tree[2][0]); // ok
```

Please note that since a ryml tree uses linear storage, the complexity of
`operator[]` is linear on the number of children of the node on which it is
invoked. If you use it with a large tree with many siblings at the root
level, you may get a performance hit. To avoid this, you can create your own
accelerator structure (eg, do a single loop at the root level to fill an
`std::map<csubstr,size_t>` mapping key names to node indices; with a node
index, a lookup is O(1), so this way you can get O(log n) lookup from a key.)

What about `NodeRef`? Let's consider when a non-existing key or index is
requested via `operator[]`. Unlike with `std::map`, **this operator does not
modify the tree**. Instead you get a seed-state `NodeRef`, and the tree will
be modified only when this seed-state reference is written to. Thus `NodeRef`
can either point to a valid tree node, or if no such node exists it will be
in seed-state by holding the index or name passed to `operator[]`. To allow
for this, `NodeRef` is a simple structure with a declaration like:

```c++
class NodeRef
{
    // a pointer to the tree
    Tree * m_tree; 
    
    // either the (tree-scoped) index of an existing node or the (node-scoped) index of a seed state
    size_t m_node_or_seed_id;
    
    // the key name of a seed state. null when valid
    const char* m_seed_name;

public:

    // this can be used to query whether a node is in seed state
    bool valid()
    {
        return m_node_or_seed_id != NONE
               &&
               m_seed_name == nullptr;
    }

    // forward all calls to m_tree. For example:
    csubstr val() const { assert(valid()); return m_tree->val(m_node_or_seed_id); }
    void set_val(csubstr v) { if(!valid()) {/*create node in tree*/;} m_tree->set_val(m_node_or_seed_id, v); }

    // etc...
};
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


### Creating a tree

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




### Low-level API

The low-level api is an index-based API accessible from
the [`ryml::Tree`](src/c4/yml/tree.hpp) object. Here are some examples:

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

assert(baz == t.next_sibling(bar_id));
assert(bar == t.prev_sibling(baz_id));

print_keyval(t, foo_id); // "foo: 1"
print_keyval(t, bar_id); // "bar: 2"
print_keyval(t, baz_id); // "baz: 3"

// to iterate over the children of a node:
for(size_t i  = t.first_child(root_id);
           i != ryml::NONE;
           i  = t.next_sibling(i))
{
    // ...
}

// to iterate over the siblings of a node:
for(size_t i  = t.first_sibling(foo_id);
           i != ryml::NONE;
           i  = t.next_sibling(i))
{
    // ...
}
```


### Custom types

ryml provides code to serialize the basic intrinsic types (integers, floating
points and strings): you can see it in the [the `c4/to_str.hpp`
header]([c4core](https://github.com/biojppm/c4core/src/c4/to_str.hpp). For
types other than these, you need to instruct ryml how to serialize your type.

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



### STL interoperation

ryml does not use the STL internally, but you can use ryml to serialize and
deserialize STL containers. That is, the use of STL is opt-in: you need to
`#include` the proper ryml header for the container you want to serialize, or
provide an implementation of your own, as above. Having done that, you can
serialize / deserialize your containers with a single step. For example:

```c++
#include <ryml_std.hpp>
int main()
{
    std::map<std::string, int> m({{"foo", 1}, {"bar", 2}});
    ryml::Tree t;
    t.rootref() << m; // serialization of the map happens here
    
    emit(t);
    // foo: 1
    // bar: 2
    
    t["foo"] << 1111;  // serialize an integer into
                       // the tree's arena, and make
                       // foo's value point at it
    t["bar"] << 2222;  // the same, but for bar
    
    emit(t);
    // foo: 1111
    // bar: 2222
    
    m.clear();
    t.rootref() >> m; // deserialization of the map happens here

    assert(m["foo"] == 1111); // ok
    assert(m["bar"] == 2222); // ok
}
```

The [`<ryml_std.hpp>`](src/ryml_std.hpp) header includes every std type
implementation available in ryml. But you can include just a specific header
if you are interested only in a particular container; these headers are
located under a specific directory in the ryml source
folder: [c4/yml/std](src/c4/yml/std) You can see browse them to learn how to
implement your custom type: for containers, see for
example [the `std::vector` implementation](src/c4/yml/std/vector.hpp),
or [the `std::map` implementation](src/c4/yml/std/map.hpp); for example of
value nodes, see [the `std::string` implementation]() `fo. If you'd like to
see a particular STL container implemented, feel free
to
[submit a pull request or open an issue](https://github.com/biojppm/rapidyaml/issues).



### Custom allocators and error handlers

ryml accepts your own allocators and error handlers. Read through
[this header file](src/c4/yml/common.hpp) to set it up.

Please note the following about the use of custom allocators with ryml. If
you use static ryml trees or parsers, you need to make sure that their
allocator has the same lifetime. So you can't use ryml's default allocator,
as it is declared in a ryml file, and the standard provides no guarantee on
the relative initialization order, such that the allocator is constructed
before and destroyed after your variables (in fact you are pretty much
guaranteed to see this fail). So please carefully consider your choices, and
ponder whether you really need to use ryml static trees and parsers. If you
do need this, then you will need to declare and use an allocator from a ryml
memory resource that outlives the tree and/or parser.


------

## YAML standard conformance

ryml is under active development, but is close to feature complete. (With the
notable exception of UTF8 support, which we haven't had the chance to verify.)

The following core features are tested:
* mappings
* sequences
* complex keys
* literal blocks
* quoted scalars
* tags
* anchors and references

Of course, there are *many* dark corners in YAML, and there certainly can
appear some cases which YAML fails to parse. So we welcome
your
[bug reports or pull requests!](https://github.com/biojppm/rapidyaml/issues).

Integration of the ~300 cases in
the [YAML test suite](https://github.com/yaml/yaml-test-suite) is ongoing
work.



------

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

[yaml-cpp](https://github.com/jbeder/yaml-cpp) is full of functionality, but
is heavy on the use of node-based structures, allocations, string copies and
slow C++ stream serializations. This is generally a sure way of making your
code slower, and strong evidence of this can be seen in the benchmark results
above.

If you care about performance and low latency, using contiguous structures,
parsing in place and using non-owning strings is of central importance. Hence
this rapid YAML library, which bridges the gap from efficiency to
usability. This library takes inspiration from RapidXML and RapidJSON.


------
## License

ryml is permissively licensed under the [MIT license](LICENSE.txt).

