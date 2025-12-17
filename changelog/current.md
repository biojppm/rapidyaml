### Changes

- [PR#563](https://github.com/biojppm/rapidyaml/pull/563) (fixes [#562](https://github.com/biojppm/rapidyaml/issues/562)) - Fix bug in `NodeRef::cend()`
- [PR#547](https://github.com/biojppm/rapidyaml/pull/547) - Fix parsing of implicit first documents with empty sequences, caused by a problem in `Tree::set_root_as_stream()`:
  ```yaml
  []  # this container was lost during parsing
  ---
  more data here
  ```
- [PR#557](https://github.com/biojppm/rapidyaml/pull/557) - `Tree` is now non-empty by default, and `Tree::root_id()` will no longer modify the tree when it is empty. To create an empty tree now it is necessary to use the capacity constructor with a capacity of zero:
  ```c++
  // default-constructed tree is now non-empty
  Tree tree;
  assert(!tree.empty()); // MODIFIED! was empty on previous version
  id_type root = tree.root_id(); // OK. default-constructed tree is now non-empty
  
  // to create an empty tree:
  Tree tree(0); // pass capacity of zero
  assert(tree.empty()); // as expected
  // but watchout, this is no longer possible:
  //id_type root = tree.root_id(); // ERROR: cannot get root of empty tree.
  ```
  This changeset also enables the python library to call `root_id()` on a default-constructed tree (fixes [#556](https://github.com/biojppm/rapidyaml/issues/556)).
