
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
- Add support for compilation with emscripten (WebAssembly+javascript) ([PR #176](https://github.com/biojppm/rapidyaml/pull/176)).

### Fixes

- Understand block literal indentation as relative to current indentation level, rather than as an absolute indentation level ([PR #176](https://github.com/biojppm/rapidyaml/pull/176)):
  ```yaml
  foo:
  - |
   child0
  - |2
     child2  # indentation is 4, not 2
  ```
- Prefer passing `substr` and `csubstr` by value instead of const reference ([PR #171](https://github.com/biojppm/rapidyaml/pull/171))
- Fix [#173](https://github.com/biojppm/rapidyaml/issues/173): add alias target `ryml::ryml` ([PR #174](https://github.com/biojppm/rapidyaml/pull/174))
- Speedup compilation of tests by removing linking with yaml-cpp and libyaml. ([PR #177](https://github.com/biojppm/rapidyaml/pull/177))
