
### New features
- Add `Tree::change_type()` and `NodeRef::change_type()` ([PR #171](https://github.com/biojppm/rapidyaml/pull/171)):
  ```c++
  // clears a node and sets its type to a different type (one of `VAL`, `SEQ`, `MAP`):
  Tree t = parse("{keyval0: val0, keyval1: val1, keyval2: val2}");
  t[0].change_type(VAL);
  t[1].change_type(MAP);
  t[2].change_type(SEQ);
  Tree expected = parse("{keyval0: val0, keyval1: {}, keyval2: []}");
  assert(emitrs<std::string>(t) == emitrs<std::string>(expected));
  ```

### Fixes

- prefer passing `substr` and `csubstr` by value instead of const reference ([PR #171](https://github.com/biojppm/rapidyaml/pull/171))
