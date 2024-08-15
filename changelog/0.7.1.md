## New features

- [PR#459](https://github.com/biojppm/rapidyaml/pull/459): Add version functions and macros:
  ```cpp
  #define RYML_VERSION "0.7.1"
  #define RYML_VERSION_MAJOR 0
  #define RYML_VERSION_MINOR 7
  #define RYML_VERSION_PATCH 1
  csubstr version();
  int version_major();
  int version_minor();
  int version_patch();
  ```

## Fixes

- Fix [#455](https://github.com/biojppm/rapidyaml/issues/455): parsing of trailing val-less nested maps when deindented to maps ([PR#460](https://github.com/biojppm/rapidyaml/pull/460))
- Fix filtering of double-quoted keys in block maps ([PR#452](https://github.com/biojppm/rapidyaml/pull/452))
- Fix [#440](https://github.com/biojppm/rapidyaml/issues/440): some tests failing with gcc -O2 (hypothetically due to undefined behavior)
  - This was accomplished by refactoring some internal parser functions; see the comments in [#440](https://github.com/biojppm/rapidyaml/issues/440) for further details.
  - Also, fix all warnings from `scan-build`.
- Use malloc.h instead of alloca.h on MinGW ([PR#447](https://github.com/biojppm/rapidyaml/pull/447))
- Fix [#442](https://github.com/biojppm/rapidyaml/issues/442) ([PR#443](https://github.com/biojppm/rapidyaml/pull/443)):
  - Ensure leading `+` is accepted when deserializing numbers.
  - Ensure numbers are not quoted by fixing the heuristics in `scalar_style_query_plain()` and `scalar_style_choose()`.
  - Add quickstart sample for overflow detection (only of integral types).
- Parse engine: cleanup unused macros


## Thanks

- @marcalff
- @toge
- @musicinmybrain
- @buty4649
