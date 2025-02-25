
## Fixes

- [PR#501](https://github.com/biojppm/rapidyaml/pull/501): fix missing tag in `- !!seq []`
- [PR#502](https://github.com/biojppm/rapidyaml/pull/502): fix parse error in tagged empty documents:
  ```yaml
  !yamlscript/v0/bare
  --- !code
  42
  ```
