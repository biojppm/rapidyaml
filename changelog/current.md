
### Breaking changes

- Change parsing behavior of root-level scalars: now these are parsed
into a DOCVAL, not SEQ->VAL ([5ba0d56](https://github.com/biojppm/rapidyaml/pull/144/commits/5ba0d56904daef1509f0073695145c4835ab1b30), from [PR #144](https://github.com/biojppm/rapidyaml/pull/144)). Eg,
  ```yaml
  this is a scalar
  ```
  is now emitted as was, instead of the previous
  ```yaml
  - this is a scalar
  ```

### New features & improvements

- `Tree::lookup_path_or_modify()`: add overload to graft existing branches ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- Callbacks: improve test coverage ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- [YAML test suite](https://github.com/yaml/yaml-test-suite)
([PR #144](https://github.com/biojppm/rapidyaml/pull/144)): implemented event parsing and comparison with the reference tree. Results are satisfactory, but it did reveal a number of existing problems, which are the subject of ongoing work. See the [list of current known
failures](../test/test_suite/test_suite_parts.cpp) in the test suite file.


### Fixes

#### Anchors
- Fix resolving of nodes with keyref+valref ([PR #144](https://github.com/biojppm/rapidyaml/pull/144)): `{&a a: &b b, *b: *a}`

#### Tags
- Fix parsing of tag dense maps and seqs ([PR #144](https://github.com/biojppm/rapidyaml/pull/144)):
  ```yaml
  --- !!map {
    k: !!seq [ a, !!str b],
    j: !!seq
       [ a, !!str b]
  --- !!seq [
    !!map { !!str k: v},
    !!map { !!str ? k: v}
  ]
  ```
- Fix parsing of tags for explicit flow maps and seqs ([PR #144](https://github.com/biojppm/rapidyaml/pull/144)):
  ```yaml
  --- !!map
  !!str foo: !!map  # there was a parse error with the multiple tags
    !!int 1: !!float 20.0
    !!int 3: !!float 40.0
  --- !!seq
  - !!map
    !!str k1: v1
    !!str k2: v2
    !!str k3: v3
  ```

#### Whitespace
- Fix parsing of double-quoted scalars with tabs ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  "This has a\ttab"
  # is now correctly parsed as "This has a	tab" 
  ```

#### Document structure
- Prevent creation of DOC nodes from stream-level comments or tags ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  !foo "bar"
  ...
  # Global
  %TAG ! tag:example.com,2000:app/
  ---
  !foo "bar"
  ```
  was parsed as
  ```yaml
  ---
  !foo "bar"
  ---
  # notice the empty doc in here
  ---
  !foo "bar"
  ```
  and it is now correctly parsed as
  ```yaml
  ---
  !foo "bar"
  ---
  !foo "bar"
  ```
  (other than the known limitation that ryml does not do tag lookup).


#### General

- Fix [#147](https://github.com/biojppm/rapidyaml/issues/147): serialize/deserialize special float values `.nan`, `.inf`, `-.inf` ([PR #149](https://github.com/biojppm/rapidyaml/pull/149))
- Fix [#142](https://github.com/biojppm/rapidyaml/issues/142): `preprocess_json()`: ensure quoted ranges are skipped when slurping containers
- Ensure error macros expand to a single statement ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))


### Special thanks

- @Gei0r
