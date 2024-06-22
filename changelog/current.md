## Fixes

- (WIP) Fix [#440](https://github.com/biojppm/rapidyaml/issues/440) Some tests failing with gcc -O2 (due to undefined behavior)
  - Fix all warnings from `scan-build`
- Use malloc.h instead of alloca.h on MinGW ([PR#447](https://github.com/biojppm/rapidyaml/pull/447))
- Fix [#442](https://github.com/biojppm/rapidyaml/issues/442) ([PR#443](https://github.com/biojppm/rapidyaml/pull/443)):
  - Ensure leading `+` is accepted when deserializing numbers.
  - Ensure numbers are not quoted by fixing the heuristics in `scalar_style_query_plain()` and `scalar_style_choose()`.
  - Add quickstart sample for overflow detection (only of integral types).
- Parse engine: cleanup unused macros


## Thanks

- @toge
- @musicinmybrain
