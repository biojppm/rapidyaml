## Fixes

- [PR#488](https://github.com/biojppm/rapidyaml/pull/488):
  - add workarounds for problems with codegen of gcc 11,12,13.
  - improve CI coverage of gcc and clang optimization levels.
- [BREAKING] Fix [#477](https://github.com/biojppm/rapidyaml/issues/477): changed `read<std::map>()` to overwrite existing entries. The provided implementations had an inconsistency between `std::map` (which wasn't overwriting) and `std::vector` (which *was* overwriting).
- Fix [#475](https://github.com/biojppm/rapidyaml/issues/475): parse error on trailing whitespace in block containers.
