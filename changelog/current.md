
### Fixes

- Fix [#361](https://github.com/biojppm/rapidyaml/pull/361) - parse error on map scalars containing `:` and starting on the next line:
  ```yaml
  ---
  # failed to parse:
  description:
    foo:bar
  ---
  # but this was ok:
  description: foo:bar
  ```
- [PR#368](https://github.com/biojppm/rapidyaml/pull/368) - fix pedantic compiler warnings.
- Fix [#373](https://github.com/biojppm/rapidyaml/issues/373) - false parse error with empty quoted keys in block-style map ([PR#374](https://github.com/biojppm/rapidyaml/pull/374)).
- Fix [#356](https://github.com/biojppm/rapidyaml/issues/356) - fix overzealous check in `emit_as()`. An id may be larger than the tree's size, eg when nodes were removed.
([PR#357](https://github.com/biojppm/rapidyaml/pull/357)).
