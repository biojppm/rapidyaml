## Fixes

- [PR#580](https://github.com/biojppm/rapidyaml/pull/580): fix compilation error when `RYML_NO_DEFAULT_CALLBACKS` is defined (thanks @toge)
- [PR#582](https://github.com/biojppm/rapidyaml/pull/582): fix compilation with clang-cl
- Fix [#584](https://github.com/biojppm/rapidyaml/pull/584): install: `RYML_VERSION` was missing from rymlConfig.cmake


## Python

- [PR#579](https://github.com/biojppm/rapidyaml/pull/579): python packaging files and CI infrastructure was moved to a different repo [biojppm/rapidyaml-python](https://github.com/biojppm/rapidyaml-python). This was done because python packaging is notoriously hard and has
always posed trouble in the CI, standing in the way of C++ development and releases.


## Thanks

- @toge
