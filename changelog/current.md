### Fixes

- Fix [#246](https://github.com/biojppm/rapidyaml/issues/246): add missing `#define` for the include guard of the amalgamated header.
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
- Maps: ensure parse error on keyless trailing scalars:
  ```yaml
  --- # test suite case 236B
  foo:
    bar
  invalid
  --- # test suite case 7MNF
  top1:
    key1: val1
  top2
  --- # test suite case 9CWY
  key:
   - item1
   - item2
  invalid
  ```
