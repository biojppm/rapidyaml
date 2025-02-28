
## Fixes

- [PR#503](https://github.com/biojppm/rapidyaml/pull/503): ensure parse error on `a: b: c` and similar cases containing nested maps opening on the same line.
- [PR#502](https://github.com/biojppm/rapidyaml/pull/502): fix parse errors or missing tags:
  - missing tags in empty documents:
    ```yaml
    !yamlscript/v0/bare
    --- !code
    42
    ```
  - trailing empty keys or vals:
    ```yaml
    a:
       :
    ```
  - missing tags in trailing empty keys or vals:
    ```yaml
    a: !tag
      !tag : !tag
    ```
  - missing tags in complex maps:
    ```yaml
    ? a: !tag
      !tag : !tag
    :
      !tag : !tag
    ```
- [PR#501](https://github.com/biojppm/rapidyaml/pull/501): fix missing tag in `- !!seq []`.
