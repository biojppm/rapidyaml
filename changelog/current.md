This release is mostly focused on compliance with the [YAML test suite](https://github.com/yaml/yaml-test-suite).

### Breaking changes

- Fix parsing behavior of root-level scalars: now these are parsed
into a DOCVAL, not SEQ->VAL ([5ba0d56](https://github.com/biojppm/rapidyaml/pull/144/commits/5ba0d56904daef1509f0073695145c4835ab1b30), from [PR #144](https://github.com/biojppm/rapidyaml/pull/144)). Eg,
  ```yaml
  ---
  this is a scalar
  --- # previously this was parsed as
  - this is a scalar
  ```

### New features & improvements

- `Tree::lookup_path_or_modify()`: add overload to graft existing branches ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- Callbacks: improve test coverage ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- [YAML test suite](https://github.com/yaml/yaml-test-suite) ([PR
#144](https://github.com/biojppm/rapidyaml/pull/144), [PR
#145](https://github.com/biojppm/rapidyaml/pull/145)): big progress towards compliance with the suite. There are still a number of existing problems, which are the subject of ongoing work. See the
[list of current known failures](../test/test_suite/test_suite_parts.cpp) in the test suite file.


### Fixes

#### Anchors
- Fix resolving of nodes with keyref+valref ([PR #144](https://github.com/biojppm/rapidyaml/pull/144)): `{&a a: &b b, *b: *a}`
- Fix parsing of implicit scalars when tags are present ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  - &a  # test case PW8X
  - a
  - &a : a
    b: &b
  - &c : &a
  - ? &d
  - ? &e
    : &a
  ```

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
  # is now correctly parsed as "This has a<TAB>tab"
  ```
- Fix filtering of leading and trailing whitespace within double-quoted scalars ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case 4ZYM, 7A4E, TL85
  "
  <SPC><SPC>foo<SPC>
  <SPC> 
  <SPC><TAB><SPC>bar
  <SPC><SPC>baz
  "
  # is now correctly parsed as " foo\nbar\nbaz "
```
- Fix parsing of tabs within YAML tokens ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  ---<TAB>scalar   # test case K54U
  ---<TAB>{}       # test case Q5MG
  ---              # test case DC7X
  a: b<TAB>
  seq:<TAB>
   - a<TAB>
  c: d<TAB>#X
  ```
- Fix parsing of flow-style maps with ommitted values without any space ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case 4ABK
  - {foo: , bar: , baz: }  # this was parsed correctly as {foo: ~, bar: ~, baz: ~}
  - {foo:, bar:, baz:}     # ... but this was parsed as {'foo:': , 'bar:': ~, 'baz:': ~}
  ```

#### Scalars
- Unescape forward slashes in double quoted string ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  --- escaped slash: "a\/b"   # test case 3UYS
  # is now parsed as:
  --- escaped slash: "a/b"
  ```
- Fix filtering of indented regions in folded scalars ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case 7T8X
  - >
    
    folded
    line
    
    next
    line
      * bullet
    
      * list
      * lines
    
    last
    line
  ```
  is now correctly parsed as `\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n`.
- Fix parsing of comments within implicit keys ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case X8DW
  ? key
  # comment 
  : value
  # now correctly parsed as {key: value}
  ```

#### Document structure
- Empty docs are now parsed as a docval with a null node:
  ```yaml
  ---   # test cases 6XDY, 6ZKB, 9BXL, PUW8
  ---
  ---
  ```
  is now parsed as
  ```yaml
  --- ~
  --- ~
  --- ~
  ```
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
