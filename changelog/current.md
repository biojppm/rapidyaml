## Fixes

- [BREAKING] Fix [#477](https://github.com/biojppm/rapidyaml/issues/477): changed `read<std::map>()` to overwrite existing entries. The provided implementations had an inconsistency between `std::map` (which wasn't overwriting) and `std::vector` (which *was* overwriting).
