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
- Scalars:
  - Do not accept block scalars with 0 indentation:
    ```yaml
    # test suite case 2G84
    --- |0   # error
    --- |10  # error
    --- |1-  # ok
    --- |1+  # ok
    ```
- Maps
  - ensure parse error on keyless terminating scalars:
    ```yaml
    ---
    foo:
      bar
    valid:    # ok
    --- # test suite case 236B
    foo:
      bar
    invalid   # parse error expected
    --- # test suite case 7MNF
    top1:
      key1: val1
    invalid   # parse error expected
    --- # test suite case 9CWY
    key:
     - item1
     - item2
    invalid   # parse error expected
    ```
