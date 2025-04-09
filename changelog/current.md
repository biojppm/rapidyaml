
### Fixes

- Fix [#400](https://github.com/biojppm/rapidyaml/issues/400) ([PR#506](https://github.com/biojppm/rapidyaml/pull/506)): clear anchors after resolving.
- Fix [#484](https://github.com/biojppm/rapidyaml/issues/484) ([PR#506](https://github.com/biojppm/rapidyaml/pull/506)): fix merge key order for last element to be overriden.
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
- [PR#508](https://github.com/biojppm/rapidyaml/pull/508): fix build with cmake 4.
- [PR#517](https://github.com/biojppm/rapidyaml/pull/517) (fixes [#516](https://github.com/biojppm/rapidyaml/issues/516)): fix python wheels for windows and macosx.
- Fix [#120](https://github.com/biojppm/rapidyaml/issues/120) (again): add workaround for #define emit in Qt


### Thanks

- @davidrudlstorfer
