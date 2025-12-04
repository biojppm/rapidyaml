### Changes

- [PR#547](https://github.com/biojppm/rapidyaml/pull/547) - Fix parsing of implicit first documents with empty sequences, caused by a problem in `Tree::set_root_as_stream()`:
  ```yaml
  []  # this container was lost during parsing
  ---
  more data here
  ```
- [PR#561](https://github.com/biojppm/rapidyaml/pull/561) (fixes [#559](https://github.com/biojppm/rapidyaml/issues/559)) - Byte Order Mark: account for BOM when determining block indentation
- [PR#563](https://github.com/biojppm/rapidyaml/pull/563) (fixes [#562](https://github.com/biojppm/rapidyaml/issues/562)) - Fix bug in `NodeRef::cend()`
- [PR#565](https://github.com/biojppm/rapidyaml/pull/565) (fixes [#564](https://github.com/biojppm/rapidyaml/issues/564)) - `Tree` arena: allow relocation of zero-length strings when placed at the end (relax assertions triggered in `Tree::_relocated()`)
- [PR#557](https://github.com/biojppm/rapidyaml/pull/557) - `Tree` is now non-empty by default, and `Tree::root_id()` will no longer modify the tree when it is empty. To create an empty tree now, it is necessary to use the capacity constructor with a capacity of zero:
  ```c++
  // breaking change: default-constructed tree is now non-empty
  Tree tree;
  assert(!tree.empty()); // MODIFIED! was empty on previous version
  id_type root = tree.root_id(); // OK. default-constructed tree is now non-empty
  
  // to create an empty tree (as happened before):
  Tree tree(0); // pass capacity of zero
  assert(tree.empty()); // as expected
  // but watchout, this is no longer possible:
  //id_type root = tree.root_id(); // ERROR: cannot get root of empty tree.
  ```
  This changeset also enables the python library to call `root_id()` on a default-constructed tree (fixes [#556](https://github.com/biojppm/rapidyaml/issues/556)).
- [PR#560](https://github.com/biojppm/rapidyaml/pull/560) (see also [#554](https://github.com/biojppm/rapidyaml/issues/554)): python improvements:
  - expose `Tree::to_arena()` in python. This allows safer and easier programatic creation of trees in python by ensuring scalars have the same lifetime as the tree:
  ```python
  t = ryml.Tree()
  s = t.to_arena(temp_string()) # Save a temporary string to the tree's arena.
                                # Works also with integers and floats.
  t.to_val(t.root_id(), s)      # Now we can safely use the scalar in the tree:
                                # there is no longer any risk of it being deallocated
  ```
  - improve behavior of `Tree` methods accepting scalars: all standard buffer types are now accepted (ie, `str`, `bytes`, `bytearray` and `memoryview`).
