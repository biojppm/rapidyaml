- [PR#607](https://github.com/biojppm/rapidyaml/pull/607) add file utilities:
  - `file_put_contents()`
  - `file_get_contents()`
  - `stdin_get_contents()`
- [PR#609](https://github.com/biojppm/rapidyaml/pull/609): fix misbuild in gcc 16.
- [PR#610](https://github.com/biojppm/rapidyaml/pull/610) fix clang warnings: `-Weverything`. Thanks @TedLyngmo !
- [PR#613](https://github.com/biojppm/rapidyaml/pull/613) fix [#612](https://github.com/biojppm/rapidyaml/pull/612) fix parse error on whitespace-only after block scalar indicators `|` or `>`:
  ```yaml
  space after: >\space
  tag after: >\t
  ```


### Thanks

- @TedLyngmo
