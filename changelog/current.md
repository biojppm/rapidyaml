## Parsing fixes

- [PR#583](https://github.com/biojppm/rapidyaml/pull/583): Fix corner cases of container keys. Eg, parsing of [explicit keys forming valid YAML](https://play.yaml.com/?show=xd#PwogID8gICMgd2FzIGNhdXNpbmcgYSBwYXJzZSBlcnJvcgo/ICAgICMgcG9wcGluZyB3YXMgYWxzbyBjYXVzaW5nIGEgcGFyc2UgZXJyb3IKLS0tCj8gW2E6IGJdOiB4CjogeQo=) like:
  ```yaml
  ?
    ?  # was causing a parse error
  ?    # popping was also causing a parse error
  ---
  ? [a: b]: x
  : y
  ```


## Fixes

- [PR#580](https://github.com/biojppm/rapidyaml/pull/580): fix compilation error when `RYML_NO_DEFAULT_CALLBACKS` is defined (thanks @toge)
- [PR#582](https://github.com/biojppm/rapidyaml/pull/582): fix compilation with clang-cl


## Python

- [PR#579](https://github.com/biojppm/rapidyaml/pull/579): python packaging files and CI infrastructure was moved to a different repo [biojppm/rapidyaml-python](https://github.com/biojppm/rapidyaml-python). This was done because python packaging is notoriously hard and has always posed trouble in the CI, standing in the way of C++ development and releases.


## Thanks

- @toge
