
### Modifications

- **BREAKING CHANGE**: make the node API const-correct ([PR#267](https://github.com/biojppm/rapidyaml/pull/267)): added `ConstNodeRef` to hold a constant reference to a node. As the name implies, a `ConstNodeRef` object cannot be used in any tree-mutating operation. It is also smaller than the existing `NodeRef` (and faster because it does not need to check its own validity on every access). As a result of this change, there are now some constraints when obtaining a ref from a tree, and existing code is likely to break in this type of situation:
  ```c++
  const Tree const_tree = ...;
  NodeRef nr = const_tree.rootref(); // ERROR (was ok): cannot obtain a mutating NodeRef from a const Tree
  ConstNodeRef cnr = const_tree.rootref(); // ok
  
  Tree tree = ...;
  NodeRef nr = tree.rootref(); // ok
  ConstNodeRef cnr = tree.rootref(); // ok (implicit conversion from NodeRef to ConstNodeRef)
  // to obtain a ConstNodeRef from a mutable Tree
  // while avoiding implicit conversion, use the `c`
  // prefix:
  ConstNodeRef cnr = tree.crootref();
  // likewise for tree.ref() and tree.cref().
  
  nr = cnr; // ERROR: cannot obtain NodeRef from ConstNodeRef
  cnr = nr; // ok
  ```
  Naturally, use of `ConstNodeRef` needs to be propagated through client code. One such place is when deserializing types:
  ```c++
  // needs to be changed from:
  template<class T> bool read(ryml::NodeRef const& n, T *var);
  // ... to:
  template<class T> bool read(ryml::ConstNodeRef const& n, T *var);
  ```


### Fixes

- Fix [#233](https://github.com/biojppm/rapidyaml/issues/233) - accept leading colon in the first key of a flow map (`UNK` node) [PR#234](https://github.com/biojppm/rapidyaml/pull/234):
  ```yaml
  :foo:           # parse error on the leading colon
    :bar: a       # parse error on the leading colon
    :barbar: b    # was ok
    :barbarbar: c # was ok
  foo:            # was ok
    bar: a        # was ok
    :barbar: b    # was ok
    :barbarbar: c # was ol
  ```
- Fix [#246](https://github.com/biojppm/rapidyaml/issues/246): add missing `#define` for the include guard of the amalgamated header.
- Fix [#253](https://github.com/biojppm/rapidyaml/issues/253): double-quoted emitter should encode carriage-return `\r` to preserve roundtrip equivalence:
  ```yaml
  Tree tree;
  NodeRef root = tree.rootref();
  root |= MAP;
  root["s"] = "t\rt";
  root["s"] |= _WIP_VAL_DQUO;
  std::string s = emitrs<std::string>(tree);
  EXPECT_EQ(s, "s: \"t\\rt\"\n");
  Tree tree2 = parse_in_arena(to_csubstr(s));
  EXPECT_EQ(tree2["s"].val(), tree["s"].val());
  ```
- Fix [#280](https://github.com/biojppm/rapidyaml/issues/280) ([PR#281](https://github.com/biojppm/rapidyaml/pull/281)): deserialization of `std::vector<bool>` failed because its `operator[]` returns a `reference` instead of `value_type`.
- Fix [#288](https://github.com/biojppm/rapidyaml/issues/288) ([PR#290](https://github.com/biojppm/rapidyaml/pull/290)): segfault on successive calls to `Tree::_grow_arena()`, caused by using the arena position instead of its length as starting point for the new arena capacity.
- Deprecate `Tree::arena_pos()`: use `Tree::arena_size()` instead ([PR#290](https://github.com/biojppm/rapidyaml/pull/290)).
- YAML test suite events emitter: fix emission of inheriting nodes. The events for `{<<: *anchor, foo: bar}` are now correctly emitted as:
  ```yaml
  =VAL :<<  # previously was =ALI <<
  =ALI *anchor
  =VAL :foo
  =VAL :bar
  ```
- Fix [cmake#8](https://github.com/biojppm/cmake/issues/8): `SOVERSION` missing from shared libraries.
