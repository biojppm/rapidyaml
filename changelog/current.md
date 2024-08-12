## Fixes

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

- @toge
- @musicinmybrain
- @buty4649
