## Fixes

- Fix [#442](https://github.com/biojppm/rapidyaml/issues/442) ([PR#443](https://github.com/biojppm/rapidyaml/pull/443)):
  - Ensure leading `+` is accepted when deserializing numbers.
  - Ensure numbers are not quoted by fixing the heuristics in `scalar_style_query_plain()` and `scalar_style_choose()`.
  - Add quickstart sample for overflow detection (only of integral types).
- Parse engine: cleanup unused macros
