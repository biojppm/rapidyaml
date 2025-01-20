## Fixes

- Fix [#480](https://github.com/biojppm/rapidyaml/issues/480) ([PR#489](https://github.com/biojppm/rapidyaml/pull/489)):
  - Deserializing an empty quoted string *will not* cause an error.
  - Deserializing an empty string *will* cause an error.
  - Ensure keys are deserialized using all the rules applying to vals.
  - Added `KEYNIL` and `VALNIL` to `NodeType_e`, used by the parser to mark the key or val as empty. This changed the values of the `NodeType_e` enumeration.
  - Added `NodeType::key_is_null()` and `NodeType::val_is_null()`.
- [PR#488](https://github.com/biojppm/rapidyaml/pull/488):
  - add workarounds for problems with codegen of gcc 11,12,13.
  - improve CI coverage of gcc and clang optimization levels.
- [BREAKING] Fix [#477](https://github.com/biojppm/rapidyaml/issues/477) ([PR#479](https://github.com/biojppm/rapidyaml/pull/479)): changed `read<std::map>()` to overwrite existing entries. The provided implementations had an inconsistency between `std::map` (which wasn't overwriting) and `std::vector` (which *was* overwriting).
- [PR#492](https://github.com/biojppm/rapidyaml/pull/492): fix emit of explicit keys when indented:
  ```yaml
  fixed:
      ? explicit key
      : value
  previously:
      ? explicit key
  : value   # this was not indented
  ```


## Thanks

- @Delian0
- @perlpunk
