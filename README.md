# Rapid YAML

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
    std::cout << node.key(); // "key"
    std::cout << node.val(); // "1"

    // deserializing:
    int foo;
    node >> foo; // now foo = 2
}
```

Creating a tree programatically:
```c++
ryml::Tree tree;
auto r = tree.rootref();

r = ryml::MAP;  // this is needed to make the root a map

r["foo"] = "1"; // ryml works only with strings.
                // Note that the tree will be __pointing__ at the
                // strings "foo" and "1" used here. You need
                // to make sure they have at least the same
                // lifetime as the tree.

auto s = r["seq"]; // does not change the tree until s is written to.
s = ryml::SEQ;
r["seq"].append_child() = "bar0";
r["seq"].append_child() = "bar1";
r["seq"].append_child() = "bar2";

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

### Custom types

Deserialization does not involve allocations. Serialization may need to
resize the tree's internal string arena.

... describe how to add serialization and stringification of user
types. Example in [src/c4/yml/std/string.hpp](the std::string serialization code).

### STL interoperation

ryml does not require use of the STL. Use of STL is opt-in: you need to
`#include` the proper ryml header.


### Low-level API

... describe index-based API of the tree.

### Custom allocation

... describe [src/c4/yml/common.hpp](custom allocation callbacks)

### Custom error handling

... describe [src/c4/yml/common.hpp](the custom error handler callback)


## How rapid is it?

... Show here.

## Alternative libraries

* libyaml
* yaml-cpp

## License

ryml is permissively licensed under the [LICENSE.txt](MIT license).

