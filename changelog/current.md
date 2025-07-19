### Fixes

- Fix [#524](https://github.com/biojppm/rapidyaml/issues/524) ([PR#525](https://github.com/biojppm/rapidyaml/pull/525)): problem parsing nested map value in complex map. Kudos to @MatthewSteel!
- [PR#542](https://github.com/biojppm/rapidyaml/pull/542): `\x` Unicode sequences were not decoded. Thanks to @mutativesystems!
- [PR#541](https://github.com/biojppm/rapidyaml/pull/541): `std::is_trivial` deprecated in c++26. Thanks to @P3RK4N!
- Fix [#529](https://github.com/biojppm/rapidyaml/issues/529) ([PR#530](https://github.com/biojppm/rapidyaml/pull/530)): double-quoted `"<<"` was mistaken for an inheriting reference
- Fix argument handling in ryml-parse-emit


### Thanks

- @MatthewSteel
- @mutativesystems
- @P3RK4N
