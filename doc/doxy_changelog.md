@page Changelog Changelog
@tableofcontents

## Current
Changes since latest release: [current.md](https://github.com/biojppm/rapidyaml/blob/master/changelog/current.md)

----------------------------
## 0.8.0

[Github release: 0.8.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.8.0)

### Breaking changes

- [BREAKING] Fix [#480](https://github.com/biojppm/rapidyaml/issues/480) ([PR#489](https://github.com/biojppm/rapidyaml/pull/489)):
  - Deserializing an empty quoted string *will not* cause an error.
  - Deserializing an empty string *will* cause an error: the empty string is read in as an empty scalar.
  - Ensure keys are deserialized using all the rules applying to vals.
  - Added [KEYNIL](@ref c4::yml::NodeType_e::KEYNIL) and [VALNIL](@ref c4::yml::NodeType_e::VALNIL) to [NodeType_e](@ref c4::yml::NodeType_e), used by the parser to mark the key or val as empty. This changed the values of the [NodeType_e](@ref c4::yml::NodeType_e) enumeration.
  - Added `.key_is_null()` and `.val_is_null()`:
    - [NodeType](@ref c4::yml::NodeType): [.key_is_null()](@ref c4::yml::NodeType::key_is_null()) and [.val_is_null()](@ref c4::yml::NodeType::val_is_null())
    - [Tree](@ref c4::yml::Tree): [.key_is_null()](@ref c4::yml::Tree::key_is_null()) and [.val_is_null()](@ref c4::yml::Tree::val_is_null())
    - [ConstNodeRef](@ref c4::yml::ConstNodeRef): [.key_is_null()](@ref c4::yml::ConstNodeRef::key_is_null()) and [.val_is_null()](@ref c4::yml::ConstNodeRef::val_is_null())
    - [NodeRef](@ref c4::yml::NodeRef): [.key_is_null()](@ref c4::yml::NodeRef::key_is_null()) and [.val_is_null()](@ref c4::yml::NodeRef::val_is_null())
- [BREAKING] Fix [#477](https://github.com/biojppm/rapidyaml/issues/477) ([PR#479](https://github.com/biojppm/rapidyaml/pull/479)): changed [`read<std::map>()`](@ref c4/yml/std/map.hpp) to overwrite existing entries. The provided implementations had an inconsistency between [`read<std::map>()`](@ref c4/yml/std/map.hpp) (which wasn't overwriting) and [`read<std:vector>()`](@ref c4/yml/std/vector.hpp) (which *was* overwriting).


### Fixes

- [PR#488](https://github.com/biojppm/rapidyaml/pull/488):
  - add workarounds for problems with codegen of gcc 11,12,13.
  - improve CI coverage of gcc and clang optimization levels.
- [PR#496](https://github.com/biojppm/rapidyaml/pull/496) and [c4core PR#148](https://github.com/biojppm/c4core/pull/148): Add CI-proven support for CPU architectures:
  - mips, mipsel, mips64, mips64el
  - sparc, sparc64
  - riscv64
  - loongarch64
- Fix [#476](https://github.com/biojppm/rapidyaml/issues/476) ([PR#493](https://github.com/biojppm/rapidyaml/pull/493)): add handling of Byte Order Marks.
- [PR#492](https://github.com/biojppm/rapidyaml/pull/492): fix emit of explicit keys when indented:
  ```yaml
  fixed:
      ? explicit key
      : value
  previously:
      ? explicit key
  : value   # this was not indented
  ```
- [PR#492](https://github.com/biojppm/rapidyaml/pull/492): fix parser reset for full reuse (`m_doc_empty` was not resetted), which would cause problems under specific scenarios in subsequent reuse.
- [PR#485](https://github.com/biojppm/rapidyaml/pull/485): improve the CI workflows (thanks to \@ingydotnet):
  - amazing code reuse and organization, thanks to the use of YamlScript to generate the final workflows
  - all optimization levels are now covered for gcc, clang and Visual Studio.
- [PR#499](https://github.com/biojppm/rapidyaml/pull/499): fix warnings with `-Wundef`.


### Thanks

- \@ingydotnet
- \@perlpunk
- \@Delian0


---------------------------------------
## 0.7.2
- [0.7.2](https://github.com/biojppm/rapidyaml/releases/tag/v0.7.2)

### Fixes

- Fix [#464](https://github.com/biojppm/rapidyaml/issues/464): test failures with g++14 -O2 in ppc64le ([PR#467](https://github.com/biojppm/rapidyaml/pull/467))


### Thanks

- \@musicinmybrain


---------------------------------------
## 0.7.1
[Github release: 0.7.1](https://github.com/biojppm/rapidyaml/releases/tag/v0.7.1)

### New features

- [PR#459](https://github.com/biojppm/rapidyaml/pull/459): Add [version functions and macros](@ref c4/yml/version.hpp):
  ```cpp
  #define RYML_VERSION "0.7.1"
  #define RYML_VERSION_MAJOR 0
  #define RYML_VERSION_MINOR 7
  #define RYML_VERSION_PATCH 1
  csubstr version();
  int version_major();
  int version_minor();
  int version_patch();
  ```

### Fixes

- Fix [#455](https://github.com/biojppm/rapidyaml/issues/455): parsing of trailing val-less nested maps when deindented to maps ([PR#460](https://github.com/biojppm/rapidyaml/pull/460))
- Fix filtering of double-quoted keys in block maps ([PR#452](https://github.com/biojppm/rapidyaml/pull/452))
- Fix [#440](https://github.com/biojppm/rapidyaml/issues/440): some tests failing with gcc -O2 (hypothetically due to undefined behavior)
  - This was accomplished by refactoring some internal parser functions; see the comments in [#440](https://github.com/biojppm/rapidyaml/issues/440) for further details.
  - Also, fix all warnings from `scan-build`.
- Use malloc.h instead of alloca.h on MinGW ([PR#447](https://github.com/biojppm/rapidyaml/pull/447))
- Fix [#442](https://github.com/biojppm/rapidyaml/issues/442) ([PR#443](https://github.com/biojppm/rapidyaml/pull/443)):
  - Ensure leading `+` is accepted when deserializing numbers.
  - Ensure numbers are not quoted by fixing the heuristics in [scalar_style_query_plain()](@ref c4::yml::scalar_style_query_plain()) and [scalar_style_choose()](@ref c4::yml::scalar_style_choose()).
  - Add quickstart sample for overflow detection (only of integral types).
- Parse engine: cleanup unused macros


### Thanks

- \@marcalff
- \@toge
- \@musicinmybrain
- \@buty4649


---------------------------------------
## 0.7.0
[Github release: 0.7.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.7.0)

Most of the changes are from the giant Parser refactor described below. Before getting to that, some other minor changes first.


### Fixes

- [PR#431](https://github.com/biojppm/rapidyaml/pull/431) - Emitter: prevent stack overflows when emitting malicious trees by providing a max tree depth for the emit visitor. This was done by adding an `EmitOptions` structure as an argument both to the emitter and to the emit functions, which is then forwarded to the emitter. This `EmitOptions` structure has a max tree depth setting with a default value of 64.
- [PR#431](https://github.com/biojppm/rapidyaml/pull/431) - Fix `_RYML_CB_ALLOC()` using `(T)` in parenthesis, making the macro unusable.
- [#434](https://github.com/biojppm/rapidyaml/issues/434) - Ensure empty vals are not deserialized ([PR#436](https://github.com/biojppm/rapidyaml/pull/436)).
- [PR#433](https://github.com/biojppm/rapidyaml/pull/433):
  - Fix some corner cases causing read-after-free in the tree's arena when it is relocated while filtering scalars.
  - Improve YAML error conformance - detect YAML-mandated parse errors when:
    - directives are misplaced (eg [9MMA](https://matrix.yaml.info/details/9MMA.html), [9HCY](https://matrix.yaml.info/details/9HCY.html), [B63P](https://matrix.yaml.info/details/B63P.html), [EB22](https://matrix.yaml.info/details/EB22.html), [SF5V](https://matrix.yaml.info/details/SF5V.html)).
    - comments are misplaced (eg [MUS6/00](https://matrix.yaml.info/details/MUS6:00.html), [9JBA](https://matrix.yaml.info/details/9JBA.html), [SU5Z](https://matrix.yaml.info/details/SU5Z.html))
    - a node has both an anchor and an alias (eg [SR86](https://matrix.yaml.info/details/SR86.html), [SU74](https://matrix.yaml.info/details/SU74.html)).
    - tags contain [invalid characters](https://yaml.org/spec/1.2.2/#tag-shorthands) `,{}[]` (eg [LHL4](https://matrix.yaml.info/details/LHL4.html), [U99R](https://matrix.yaml.info/details/U99R.html), [WZ62](https://matrix.yaml.info/details/WZ62.html)).
  

### New features

- [PR#431](https://github.com/biojppm/rapidyaml/pull/431) - append-emitting to existing containers in the `emitrs_` functions, suggested in [#345](https://github.com/biojppm/rapidyaml/issues/345). This was achieved by adding a `bool append=false` as the last parameter of these functions.
- [PR#431](https://github.com/biojppm/rapidyaml/pull/431) - add depth query methods:
  ```cpp
  Tree::depth_asc(id_type) const;   // O(log(num_tree_nodes)) get the depth of a node ascending (ie, from root to node)
  Tree::depth_desc(id_type) const;  // O(num_tree_nodes) get the depth of a node descending (ie, from node to deep-most leaf node)
  ConstNodeRef::depth_asc() const;  // likewise
  ConstNodeRef::depth_desc() const;
  NodeRef::depth_asc() const;
  NodeRef::depth_desc() const;
  ```
- [PR#432](https://github.com/biojppm/rapidyaml/pull/432) - Added a function to estimate the required tree capacity, based on yaml markup:
  ```cpp
  size_t estimate_tree_capacity(csubstr); // estimate number of nodes resulting from yaml
  ```


------
All other changes come from [PR#414](https://github.com/biojppm/rapidyaml/pull/414).

### Parser refactor

The parser was completely refactored ([PR#414](https://github.com/biojppm/rapidyaml/pull/414)). This was a large and hard job carried out over several months, but it brings important improvements.

- The new parser is an event-based parser, based on an event dispatcher engine. This engine is templated on event handler, where each event is a function call, which spares branches on the event handler. The parsing code was fully rewritten, and is now much more simple (albeit longer), and much easier to work with and fix.
- YAML standard-conformance was improved significantly. Along with many smaller fixes and additions, (too many to list here), the main changes are the following:
  - The parser engine can now successfully parse container keys, emitting all the events in correctly, **but** as before, the ryml tree cannot accomodate these (and this constraint is no longer enforced by the parser, but instead by `EventHandlerTree`). For an example of a handler which can accomodate key containers, see the one which is used for the test suite at `test/test_suite/test_suite_event_handler.hpp`
  - Anchor keys can now be terminated with colon (eg, `&anchor: key: val`), as dictated by the standard.
- The parser engine can now be used to create native trees in other programming languages, or in cases where the user *must* have container keys.
- Performance of both parsing and emitting improved significantly; see some figures below.


### Strict JSON parser

- A strict JSON parser was added. Use the `parse_json_...()` family of functions to parse json in stricter mode (and faster) than flow-style YAML.


### YAML style preserved while parsing

- The YAML style information is now fully preserved through parsing/emitting round trips. This was made possible because the event model of the new parsing engine now incorporates style varieties. So, for example:
  - a scalar parsed from a plain/single-quoted/double-quoted/block-literal/block-folded scalar will be emitted always using its original style in the YAML source
  - a container parsed in block-style will always be emitted in block-style
  - a container parsed in flow-style will always be emitted in flow-style
  Because of this, the style of YAML emitted by ryml changes from previous releases.
- Scalar filtering was improved and is now done directly in the source being parsed (which may be in place or in the arena), except in the cases where the scalar expands and does not fit its initial range, in which case the scalar is filtered out of place to the tree's arena.
  - Filtering can now be disabled while parsing, to ensure a fully-readonly parse (but this feature is still experimental and somewhat untested, given the scope of the rewrite work).
  - The parser now offers methods to filter scalars in place or out of place.
- Style flags were added to `NodeType_e`:
  ```cpp
    FLOW_SL     ///< mark container with single-line flow style (seqs as '[val1,val2], maps as '{key: val,key2: val2}')
    FLOW_ML     ///< mark container with multi-line flow style (seqs as '[\n  val1,\n  val2\n], maps as '{\n  key: val,\n  key2: val2\n}')
    BLOCK       ///< mark container with block style (seqs as '- val\n', maps as 'key: val')
    KEY_LITERAL ///< mark key scalar as multiline, block literal |
    VAL_LITERAL ///< mark val scalar as multiline, block literal |
    KEY_FOLDED  ///< mark key scalar as multiline, block folded >
    VAL_FOLDED  ///< mark val scalar as multiline, block folded >
    KEY_SQUO    ///< mark key scalar as single quoted '
    VAL_SQUO    ///< mark val scalar as single quoted '
    KEY_DQUO    ///< mark key scalar as double quoted "
    VAL_DQUO    ///< mark val scalar as double quoted "
    KEY_PLAIN   ///< mark key scalar as plain scalar (unquoted, even when multiline)
    VAL_PLAIN   ///< mark val scalar as plain scalar (unquoted, even when multiline)
  ```
- Style predicates were added to `NodeType`, `Tree`, `ConstNodeRef` and `NodeRef`:
  ```cpp
    bool is_container_styled() const;
    bool is_block() const 
    bool is_flow_sl() const;
    bool is_flow_ml() const;
    bool is_flow() const;

    bool is_key_styled() const;
    bool is_val_styled() const;
    bool is_key_literal() const;
    bool is_val_literal() const;
    bool is_key_folded() const;
    bool is_val_folded() const;
    bool is_key_squo() const;
    bool is_val_squo() const;
    bool is_key_dquo() const;
    bool is_val_dquo() const;
    bool is_key_plain() const;
    bool is_val_plain() const;
  ```
- Style modifiers were also added:
  ```cpp
    void set_container_style(NodeType_e style);
    void set_key_style(NodeType_e style);
    void set_val_style(NodeType_e style);
  ```
- Emit helper predicates were added, and are used when an emitted node was built programatically without style flags:
  ```cpp
  /** choose a YAML emitting style based on the scalar's contents */
  NodeType_e scalar_style_choose(csubstr scalar) noexcept;
  /** query whether a scalar can be encoded using single quotes.
   * It may not be possible, notably when there is leading
   * whitespace after a newline. */
  bool scalar_style_query_squo(csubstr s) noexcept;
  /** query whether a scalar can be encoded using plain style (no
   * quotes, not a literal/folded block scalar). */
  bool scalar_style_query_plain(csubstr s) noexcept;
  ```

### Breaking changes

As a result of the refactor, there are some limited changes with impact in client code. Even though this was a large refactor, effort was directed at keeping maximal backwards compatibility, and the changes are not wide. But they still exist:

- The existing `parse_...()` methods in the `Parser` class were all removed. Use the corresponding `parse_...(Parser*, ...)` function from the header [`c4/yml/parse.hpp`](https://github.com/biojppm/rapidyaml/blob/master/src/c4/yml/parse.hpp).
- When instantiated by the user, the parser now needs to receive a `EventHandlerTree` object, which is responsible for building the tree. Although fully functional and tested, the structure of this class is still somewhat experimental and is still likely to change. There is an alternative event handler implementation responsible for producing the events for the YAML test suite in `test/test_suite/test_suite_event_handler.hpp`.
- The declaration and definition of `NodeType` was moved to a separate header file `c4/yml/node_type.hpp` (previously it was in `c4/yml/tree.hpp`).
- Some of the node type flags were removed, and several flags (and combination flags) were added. 
  - Most of the existing flags are kept, as well as their meaning.
  - `KEYQUO` and `VALQUO` are now masks of the several style flags for quoted scalars. In general, however, client code using these flags and `.is_val_quoted()` or `.is_key_quoted()` is not likely to require any changes.


### New type for node IDs

A type `id_type` was added to signify the integer type for the node id, defaulting to the backwards-compatible `size_t` which was previously used in the tree. In the future, this type is likely to change, *and probably to a signed type*, so client code is encouraged to always use `id_type` instead of the `size_t`, and specifically not to rely on the signedness of this type.


### Reference resolver is now exposed

The reference (ie, alias) resolver object is now exposed in
[`c4/yml/reference_resolver.hpp`](https://github.com/biojppm/rapidyaml/blob/master/src/c4/yml/reference_resolver.hpp). Previously this object was temporarily instantiated in `Tree::resolve()`. Exposing it now enables the user to reuse this object through different calls, saving a potential allocation on every call.


### Tag utilities

Tag utilities were moved to the new header [`c4/yml/tag.hpp`](https://github.com/biojppm/rapidyaml/blob/master/src/c4/yml/tag.hpp). The types `Tree::tag_directive_const_iterator` and `Tree::TagDirectiveProxy` were deprecated. Fixed also an unitialization problem with `Tree::m_tag_directives`.


### Performance improvements

To compare performance before and after this changeset, the benchmark runs were run (in the same PC), and the results were collected into these two files:
  - [results before newparser](https://github.com/biojppm/rapidyaml/blob/master/bm/results/results_before_newparser.md)
  - [results after newparser](https://github.com/biojppm/rapidyaml/blob/master/bm/results/results_after_newparser.md)
  - (suggestion: compare these files in a diff viewer)

There are a lot of results in these files, and many insights can be obtained by browsing them; too many to list here. Below we show only some selected results.


#### Parsing
Here are some figures for parsing performance, for `bm_ryml_inplace_reuse` (name before) / `bm_ryml_yaml_inplace_reuse` (name after):

|------|------------|-----------|--------|
| case | B/s before newparser | B/s after newparser | improv % |
|------|------------|-----------|--------|
| [PARSE/appveyor.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/appveyor.yml) | 168.628Mi/s | 232.017Mi/s | ~+40% |
| [PARSE/compile_commands.json](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/compile_commands.yml) | 630.17Mi/s | 609.877Mi/s | ~-3% |
| [PARSE/travis.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/travis.yml) | 193.674Mi/s | 271.598Mi/s | ~+50% |
| [PARSE/scalar_dquot_multiline.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/scalar_dquot_multiline.yml) | 224.796Mi/s | 187.335Mi/s | ~-10% |
| [PARSE/scalar_dquot_singleline.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/scalar_dquot_singleline.yml) | 339.889Mi/s | 388.924Mi/s | ~-16% |

Some conclusions:
- parse performance improved by ~30%-50% for YAML without filtering-heavy parsing.
- parse performance *decreased* by ~10%-15% for YAML with filtering-heavy parsing. There is still some scope for improvement in the parsing code, so this cost may hopefully be minimized in the future.


#### Emitting

Here are some figures emitting performance improvements retrieved from these files, for `bm_ryml_str_reserve` (name before) / `bm_ryml_yaml_str_reserve` (name after):

|------|------------|-----------|
| case | B/s before newparser | B/s after newparser |
|------|------------|-----------|
| [EMIT/appveyor.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/appveyor.yml) | 311.718Mi/s | 1018.44Mi/s |
| [EMIT/compile_commands.json](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/compile_commands.yml) | 434.206Mi/s | 771.682Mi/s |
| [EMIT/travis.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/travis.yml) | 333.322Mi/s | 1.41597Gi/s |
| [EMIT/scalar_dquot_multiline.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/scalar_dquot_multiline.yml) | 868.6Mi/s | 692.564Mi/s |
| [EMIT/scalar_dquot_singleline.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/scalar_dquot_singleline.yml) | 336.98Mi/s | 638.368Mi/s |
| [EMIT/style_seqs_flow_outer1000_inner100.yml](https://github.com/biojppm/rapidyaml/blob/master/bm/cases/style_seqs_flow_outer1000_inner100.yml) | 136.826Mi/s | 279.487Mi/s |

Emit performance improved everywhere by over 1.5x and as much as 3x-4x for YAML without filtering-heavy parsing.


---------------------------------------
## 0.6.0
[Github release: 0.6.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.6.0)

### Add API documentation

- [PR#423](https://github.com/biojppm/rapidyaml/pull/423): **add Doxygen-based API documentation, now hosted in [https://rapidyaml.readthedocs.io/](https://rapidyaml.readthedocs.io/)!**
- It uses the base doxygen docs, as I couldn't get doxyrest or breathe or exhale to produce anything meaningful using the doxygen groups already defined in the source code.


### Error handling

Fix major error handling problem reported in [#389](https://github.com/biojppm/rapidyaml/issues/389) ([PR#411](https://github.com/biojppm/rapidyaml/pull/411)):

  - The [NodeRef](@ref c4::yml::NodeRef) and [ConstNodeRef](@ref c4::yml::ConstNodeRef) classes are now conditional noexcept using `RYML_NOEXCEPT`, which evaluates either to nothing when assertions are enabled, and to `noexcept` otherwise. The problem was that these classes had many methods explicitly marked `noexcept`, but were doing assertions which could throw exceptions, causing an abort instead of a throw whenever the assertion called an exception-throwing error callback.
  - This problem was compounded by assertions being enabled in every build type -- despite the intention to have them only in debug builds. There was a problem in the preprocessor code to enable assertions which led to assertions being enabled in release builds even when `RYML_USE_ASSERT` was defined to 0. Thanks to \@jdrouhard for reporting this.
  - Although the code is and was extensively tested, the testing was addressing mostly the happy path. Tests were added to ensure that the error behavior is as intended.
  - Together with this changeset, a major revision was carried out of the asserting/checking status of each function in the node classes. In most cases, assertions were added to functions that were missing them. So **beware** - some user code that was invalid will now assert or error out. Also, assertions and checks are now directed as much as possible to the callbacks of the closest scope: ie, if a tree has custom callbacks, errors within the tree class should go through those callbacks.
  - Also, the intended assertion behavior is now in place: *no assertions in release builds*. **Beware** as well - user code which was relying on this will now silently succeed and return garbage in release builds. See the next points, which may help.
  - Added new methods to the [NodeRef](@ref c4::yml::NodeRef)/[ConstNodeRef](@ref c4::yml::ConstNodeRef) classes:
    - `.readable()`: [NodeRef::readable()](@ref c4::yml::NodeRef::readable()) and  [ConstNodeRef::readable()](@ref c4::yml::ConstNodeRef::readable())
    - `.at()`: [NodeRef::at()](@ref c4::yml::NodeRef::at()) and  [ConstNodeRef::at()](@ref c4::yml::ConstNodeRef::at())
  - The state for [NodeRef](@ref c4::yml::NodeRef) was refined, and now there are three mutually exclusive states (and class predicates) for an object of this class:
    - [.invalid()](@ref c4::yml::NodeRef::invalid()) when the object was not initialized to any node
    - [.readable()](@ref c4::yml::NodeRef::readable()) when the object points at an existing tree+node
    - [.is_seed()](@ref c4::yml::NodeRef::is_seed()) when the object points at an hypotethic tree+node
    - The previous state `.valid()` was deprecated: its semantics were confusing as it actually could be any of `.readable()` or `.is_seed()`
  - Deprecated also the following methods for [NodeRef](@ref c4::yml::NodeRef)/[ConstNodeRef](@ref c4::yml::ConstNodeRef):
    ```c++
    RYML_DEPRECATED() bool operator== (std::nullptr_t) const;
    RYML_DEPRECATED() bool operator!= (std::nullptr_t) const;
    RYML_DEPRECATED() bool operator== (csubstr val) const;
    RYML_DEPRECATED() bool operator!= (csubstr val) const;
    ```
  - Added macros and respective cmake options to control error handling:
    - `RYML_USE_ASSERT` - enable assertions regardless of build type. This is disabled by default. This macro was already defined; the current PR adds the cmake option.
    - `RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS` - make the default error handler provided by ryml throw exceptions instead of calling `std::abort()`. This is disabled by default.
  - Also, `RYML_DEBUG_BREAK()` is now enabled only if `RYML_DBG` is defined, as reported in [#362](https://github.com/biojppm/rapidyaml/issues/362).
  - As part of [PR#423](https://github.com/biojppm/rapidyaml/pull/423), to improve linters and codegen:
    - annotate the error handlers with `[[noreturn]]`/`C4_NORETURN`
    - annotate some error sites with `C4_UNREACHABLE_AFTER_ERR()`


### More fixes

- `Tree::arena() const`  was returning a `substr`; this was an error. This function was changed to:
  ```
  csubstr Tree::arena() const;
  substr Tree::arena();
  ```
- Fix [#390](https://github.com/biojppm/rapidyaml/issues/390) - `csubstr::first_real_span()` failed on scientific numbers with one digit in the exponent ([PR#415](https://github.com/biojppm/rapidyaml/pull/415)).
- Fix [#361](https://github.com/biojppm/rapidyaml/issues/361) - parse error on map scalars containing `:` and starting on the next line:
  ```yaml
  ---
  # failed to parse:
  description:
    foo:bar
  ---
  # but this was ok:
  description: foo:bar
  ```
- [PR#368](https://github.com/biojppm/rapidyaml/pull/368) - fix pedantic compiler warnings.
- Fix [#373](https://github.com/biojppm/rapidyaml/issues/373) - false parse error with empty quoted keys in block-style map ([PR#374](https://github.com/biojppm/rapidyaml/pull/374)).
- Fix [#356](https://github.com/biojppm/rapidyaml/issues/356) - fix overzealous check in `emit_as()`. An id may be larger than the tree's size, eg when nodes were removed. ([PR#357](https://github.com/biojppm/rapidyaml/pull/357)).
- Fix [#417](https://github.com/biojppm/rapidyaml/issues/417) - add quickstart example explaining how to avoid precision loss while serializing floats ([PR#420](https://github.com/biojppm/rapidyaml/pull/420)).
- Fix [#380](https://github.com/biojppm/rapidyaml/issues/380) - Debug visualizer .natvis file for Visual Studio was missing [ConstNodeRef](@ref c4::yml::ConstNodeRef) ([PR#383](https://github.com/biojppm/rapidyaml/issues/383)).
- FR [#403](https://github.com/biojppm/rapidyaml/issues/403) - install is now optional when using cmake. The relevant option is `RYML_INSTALL`.


### Python

- Fix [#428](https://github.com/biojppm/rapidyaml/issues/428)/[#412](https://github.com/biojppm/rapidyaml/discussions/412) - Parse errors now throw `RuntimeError` instead of aborting.



### Thanks

- \@Neko-Box-Coder
- \@jdrouhard
- \@dmachaj


---------------------------------------
## 0.5.0
[Github release: 0.5.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.5.0)

### Breaking changes

- Make the node API const-correct ([PR#267](https://github.com/biojppm/rapidyaml/pull/267)): added `ConstNodeRef` to hold a constant reference to a node. As the name implies, a `ConstNodeRef` object cannot be used in any tree-mutating operation. It is also smaller than the existing `NodeRef` (and faster because it does not need to check its own validity on every access). As a result of this change, there are now some constraints when obtaining a ref from a tree, and existing code is likely to break in this type of situation:
  ```c++
  const Tree const_tree = ...;
  NodeRef nr = const_tree.rootref(); // ERROR (was ok): cannot obtain a mutating NodeRef from a const Tree
  ConstNodeRef cnr = const_tree.rootref(); // ok
  
  Tree tree = ...;
  NodeRef nr = tree.rootref(); // ok
  ConstNodeRef cnr = tree.rootref(); // ok (implicit conversion from NodeRef to ConstNodeRef)
  // to obtain a ConstNodeRef from a mutable Tree
  // while avoiding implicit conversion, use the `c`
  // prefix:
  ConstNodeRef cnr = tree.crootref();
  // likewise for tree.ref() and tree.cref().
  
  nr = cnr; // ERROR: cannot obtain NodeRef from ConstNodeRef
  cnr = nr; // ok
  ```
  The use of `ConstNodeRef` also needs to be propagated through client code. One such place is when deserializing types:
  ```c++
  // needs to be changed from:
  template<class T> bool read(ryml::NodeRef const& n, T *var);
  // ... to:
  template<class T> bool read(ryml::ConstNodeRef const& n, T *var);
  ```
  - The initial version of `ConstNodeRef/NodeRef` had the problem that const methods in the CRTP base did not participate in overload resolution ([#294](https://github.com/biojppm/rapidyaml/issues/294)), preventing calls from `const NodeRef` objects. This was fixed by moving non-const methods to the CRTP base and disabling them with SFINAE ([PR#295](https://github.com/biojppm/rapidyaml/pull/295)).
  - Also added disambiguation iteration methods: `.cbegin()`, `.cend()`, `.cchildren()`, `.csiblings()` ([PR#295](https://github.com/biojppm/rapidyaml/pull/295)).
- Deprecate `emit()` and `emitrs()` ([#120](https://github.com/biojppm/rapidyaml/issues/120), [PR#303](https://github.com/biojppm/rapidyaml/pull/303)): use `emit_yaml()` and `emitrs_yaml()` instead. This was done to improve compatibility with Qt, which leaks a macro named `emit`. For more information, see [#120](https://github.com/biojppm/rapidyaml/issues/120).
  - In the Python API:
    - Deprecate `emit()`, add `emit_yaml()` and `emit_json()`.
    - Deprecate `compute_emit_length()`, add `compute_emit_yaml_length()` and `compute_emit_json_length()`.
    - Deprecate `emit_in_place()`, add `emit_yaml_in_place()` and `emit_json_in_place()`.
    - Calling the deprecated functions will now trigger a warning.
- Location querying is no longer done lazily ([#260](https://github.com/biojppm/rapidyaml/issues/260), [PR#307](https://github.com/biojppm/rapidyaml/pull/307)). It now requires explicit opt-in when instantiating the parser. With this change, the accelerator structure for location querying is now built when parsing:
  ```c++
  Parser parser(ParserOptions().locations(true));
  // now parsing also builds location lookup:
  Tree t = parser.parse_in_arena("myfile.yml", "foo: bar");
  assert(parser.location(t["foo"]).line == 0u);
  ```
  - Locations are disabled by default:
  ```c++
  Parser parser;
  assert(parser.options().locations() == false);
  ```
- Deprecate `Tree::arena_pos()`: use `Tree::arena_size()` instead ([PR#290](https://github.com/biojppm/rapidyaml/pull/290)).
- Deprecate pointless `has_siblings()`: use `Tree::has_other_siblings()` instead ([PR#330](https://github.com/biojppm/rapidyaml/pull/330).


### Performance improvements

- Improve performance of integer serialization and deserialization (in [c4core](https://github.com/biojppm/c4core)). Eg, on Linux/g++11.2, with integral types:
  - `c4::to_chars()` can be expected to be roughly...
    - ~40% to 2x faster than `std::to_chars()`
    - ~10x-30x faster than `sprintf()`
    - ~50x-100x faster than a naive `stringstream::operator<<()` followed by `stringstream::str()`
  - `c4::from_chars()` can be expected to be roughly...
    - ~10%-30% faster than `std::from_chars()`
    - ~10x faster than `scanf()`
    - ~30x-50x faster than a naive `stringstream::str()` followed by `stringstream::operator>>()`
  For more details, see [the changelog for c4core 0.1.10](https://github.com/biojppm/c4core/releases/tag/v0.1.10).
- Fix [#289](https://github.com/biojppm/rapidyaml/issues/289) and [#331](https://github.com/biojppm/rapidyaml/issues/331) - parsing of single-line flow-style sequences had quadratic complexity, causing long parse times in ultra long lines [PR#293](https://github.com/biojppm/rapidyaml/pull/293)/[PR#332](https://github.com/biojppm/rapidyaml/pull/332).
  - This was due to scanning for the token `: ` before scanning for `,` or `]`, which caused line-length scans on every scalar scan. Changing the order of the checks was enough to address the quadratic complexity, and the parse times for flow-style are now in line with block-style.
  - As part of this changeset, a significant number of runtime branches was eliminated by separating `Parser::_scan_scalar()` into several different `{seq,map}x{block,flow}` functions specific for each context. Expect some improvement in parse times.
  - Also, on Debug builds (or assertion-enabled builds) there was a paranoid assertion calling `Tree::has_child()` in `Tree::insert_child()` that caused quadratic behavior because the assertion had linear complexity. It was replaced with a somewhat equivalent O(1) assertion.
  - Now the byte throughput is independent of line size for styles and containers. This can be seen in the table below, which shows parse troughputs in MB/s of 1000 containers of different styles and sizes (flow containers are in a single line):

  | Container | Style | 10elms      | 100elms      | 1000elms      |
  |-----------|-------|-------------|--------------|---------------| 
  | 1000 Maps | block | 50.8MB/s    | 57.8MB/s     | 63.9MB/s      |
  | 1000 Maps | flow  | 58.2MB/s    | 65.9MB/s     | 74.5MB/s      |
  | 1000 Seqs | block | 55.7MB/s    | 59.2MB/s     | 60.0MB/s      |
  | 1000 Seqs | flow  | 52.8MB/s    | 55.6MB/s     | 54.5MB/s      |
- Fix [#329](https://github.com/biojppm/rapidyaml/issues/329): complexity of `has_sibling()` and `has_child()` is now O(1), previously was linear ([PR#330](https://github.com/biojppm/rapidyaml/pull/330)).


### Fixes

- Fix [#233](https://github.com/biojppm/rapidyaml/issues/233) - accept leading colon in the first key of a flow map (`UNK` node) [PR#234](https://github.com/biojppm/rapidyaml/pull/234):
  ```yaml
  :foo:           # parse error on the leading colon
    :bar: a       # parse error on the leading colon
    :barbar: b    # was ok
    :barbarbar: c # was ok
  foo:            # was ok
    bar: a        # was ok
    :barbar: b    # was ok
    :barbarbar: c # was ol
  ```
- Fix [#253](https://github.com/biojppm/rapidyaml/issues/253): double-quoted emitter should encode carriage-return `\r` to preserve roundtrip equivalence:
  ```yaml
  Tree tree;
  NodeRef root = tree.rootref();
  root |= MAP;
  root["s"] = "t\rt";
  root["s"] |= _WIP_VAL_DQUO;
  std::string s = emitrs<std::string>(tree);
  EXPECT_EQ(s, "s: \"t\\rt\"\n");
  Tree tree2 = parse_in_arena(to_csubstr(s));
  EXPECT_EQ(tree2["s"].val(), tree["s"].val());
  ```
- Fix parsing of empty block folded+literal scalars when they are the last child of a container (part of [PR#264](https://github.com/biojppm/rapidyaml/issues/264)):
  ```yaml
  seq:
    - ""
    - ''
    - >
    - |  # error, the resulting val included all the YAML from the next node
  seq2:
    - ""
    - ''
    - |
    - >  # error, the resulting val included all the YAML from the next node
  map:
    a: ""
    b: ''
    c: >
    d: |  # error, the resulting val included all the YAML from the next node
  map2:
    a: ""
    b: ''
    c: |
    d: >  # error, the resulting val included all the YAML from the next node
  lastly: the last
  ```
- Fix [#274](https://github.com/biojppm/rapidyaml/issues/274) ([PR#296](https://github.com/biojppm/rapidyaml/pull/296)): Lists with unindented items and trailing empty values parse incorrectly:
  ```yaml
  foo:
  - bar
  -
  baz: qux
  ```
  was wrongly parsed as
  ```yaml
  foo:
  - bar
  - baz: qux
  ```
- Fix [#277](https://github.com/biojppm/rapidyaml/issues/277) ([PR#340](https://github.com/biojppm/rapidyaml/pull/340)): merge fails with duplicate keys.
- Fix [#337](https://github.com/biojppm/rapidyaml/issues/337) ([PR#338](https://github.com/biojppm/rapidyaml/pull/338)): empty lines in block scalars shall not have tab characters `\t`.
- Fix [#268](https://github.com/biojppm/rapidyaml/issues/268) ([PR#339](https://github.com/biojppm/rapidyaml/pull/339)): don't override key type_bits when copying val. This was causing problematic resolution of anchors/references.
- Fix [#309](https://github.com/biojppm/rapidyaml/issues/309) ([PR#310](https://github.com/biojppm/rapidyaml/pull/310)): emitted scalars containing `@` or `` ` ``  should be quoted.
  - The quotes should be added only when they lead the scalar. See [#320](https://github.com/biojppm/rapidyaml/issues/320) and [PR#334](https://github.com/biojppm/rapidyaml/pull/334).
- Fix [#297](https://github.com/biojppm/rapidyaml/issues/297) ([PR#298](https://github.com/biojppm/rapidyaml/pull/298)): JSON emitter should escape control characters.
- Fix [#292](https://github.com/biojppm/rapidyaml/issues/292) ([PR#299](https://github.com/biojppm/rapidyaml/pull/299)): JSON emitter should quote version string scalars like `0.1.2`.
- Fix [#291](https://github.com/biojppm/rapidyaml/issues/291) ([PR#299](https://github.com/biojppm/rapidyaml/pull/299)): JSON emitter should quote scalars with leading zero, eg `048`.
- Fix [#280](https://github.com/biojppm/rapidyaml/issues/280) ([PR#281](https://github.com/biojppm/rapidyaml/pull/281)): deserialization of `std::vector<bool>` failed because its `operator[]` returns a `reference` instead of `value_type`.
- Fix [#288](https://github.com/biojppm/rapidyaml/issues/288) ([PR#290](https://github.com/biojppm/rapidyaml/pull/290)): segfault on successive calls to `Tree::_grow_arena()`, caused by using the arena position instead of its length as starting point for the new arena capacity.
- Fix [#324](https://github.com/biojppm/rapidyaml/issues/324) ([PR#328](https://github.com/biojppm/rapidyaml/pull/328)): eager assertion prevented moving nodes to the first position in a parent.
- Fix `Tree::_clear_val()`: was clearing key instead ([PR#335](https://github.com/biojppm/rapidyaml/pull/335)).
- YAML test suite events emitter: fix emission of inheriting nodes. The events for `{<<: *anchor, foo: bar}` are now correctly emitted as:
  ```yaml
  =VAL :<<  # previously was =ALI <<
  =ALI *anchor
  =VAL :foo
  =VAL :bar
  ```
- Fix [#246](https://github.com/biojppm/rapidyaml/issues/246): add missing `#define` for the include guard of the amalgamated header.
- Fix [#326](https://github.com/biojppm/rapidyaml/issues/326): honor runtime settings for calling debugbreak, add option to disable any calls to debugbreak.
- Fix [cmake#8](https://github.com/biojppm/cmake/issues/8): `SOVERSION` missing from shared libraries.


### Python

- The Python packages for Windows and MacOSX are causing problems in the CI, and were mostly disabled. The problematic packages are successfully made, but then fail to be imported. This was impossible to reproduce outside of the CI, and they were disabled since they were delaying the release. As a consequence, the Python release will have very limited compiled packages for Windows (only Python 3.6 and 3.7) or MacOSX. Help would be appreciated from those interested in these packages.


### Thanks

- \@NaN-git
- \@dancingbug


---------------------------------------
## 0.4.1
[Github release: 0.4.1](https://github.com/biojppm/rapidyaml/releases/tag/v0.4.1)

### Fixes

- Fix [#223](https://github.com/biojppm/rapidyaml/issues/223): assertion peeking into the last line when it was whitespaces only.


---------------------------------------
## 0.4.0
[Github release: 0.4.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.4.0)

This release improves compliance with the [YAML test suite](https://github.com/yaml/yaml-test-suite/) (thanks \@ingydotnet and \@perlpunk for extensive and helpful cooperation), and adds node location tracking using the parser.


### Breaking changes

As part of the [new feature to track source locations](https://github.com/biojppm/rapidyaml/pull/168), opportunity was taken to address a number of pre-existing API issues. These changes consisted of:

- Deprecate `c4::yml::parse()` and `c4::yml::Parser::parse()` overloads; all these functions will be removed in short order. Until removal, any call from client code will trigger a compiler warning.
- Add `parse()` alternatives, either `parse_in_place()` or `parse_in_arena()`:
  - `parse_in_place()` receives only `substr` buffers, ie mutable YAML source buffers. Trying to pass a `csubstr` buffer to `parse_in_place()` will cause a compile error:
    ```c++
    substr readwrite = /*...*/;
    Tree tree = parse_in_place(readwrite); // OK
    
    csubstr readonly = /*...*/;
    Tree tree = parse_in_place(readonly); // compile error
    ```
  - `parse_in_arena()` receives only `csubstr` buffers, ie immutable YAML source buffers. Prior to parsing, the buffer is copied to the tree's arena, then the copy is parsed in place. Because `parse_in_arena()` is meant for immutable buffers, overloads receiving a `substr` YAML buffer are now declared but marked deprecated, and intentionally left undefined, such that calling `parse_in_arena()` with a `substr` will cause a linker error as well as a compiler warning.
    ```c++
    substr readwrite = /*...*/;
    Tree tree = parse_in_arena(readwrite); // compile warning+linker error
    ```
    This is to prevent an accidental extra copy of the mutable source buffer to the tree's arena: `substr` is implicitly convertible to `csubstr`. If you really intend to parse an originally mutable buffer in the tree's arena, convert it first explicitly to immutable by assigning the `substr` to a `csubstr` prior to calling `parse_in_arena()`:
    ```c++
    substr readwrite = /*...*/;
    csubstr readonly = readwrite; // ok
    Tree tree = parse_in_arena(readonly); // ok
    ```
    This problem does not occur with `parse_in_place()` because `csubstr` is not implicitly convertible to `substr`. 
- In the python API, `ryml.parse()` was removed and not just deprecated; the `parse_in_arena()` and `parse_in_place()` now replace this.
- `Callbacks`: changed behavior in `Parser` and `Tree`:
  - When a tree is copy-constructed or move-constructed to another, the receiving tree will start with the callbacks of the original.
  - When a tree is copy-assigned or move-assigned to another, the receiving tree will now change its callbacks to the original.
  - When a parser creates a new tree, the tree will now use a copy of the parser's callbacks object.
  - When an existing tree is given directly to the parser, both the tree and the parser now retain their own callback objects; any allocation or error during parsing will go through the respective callback object.


### New features

- Add tracking of source code locations. This is useful for reporting semantic errors after the parsing phase (ie where the YAML is syntatically valid and parsing is successful, but the tree contents are semantically invalid). The locations can be obtained lazily from the parser when the first location is queried:
  ```c++
  // To obtain locations, use of the parser is needed:
  ryml::Parser parser;
  ryml::Tree tree = parser.parse_in_arena("source.yml", R"({
  aa: contents,
  foo: [one, [two, three]]
  })");
  // After parsing, on the first call to obtain a location,
  // the parser will cache a lookup structure to accelerate
  // tracking the location of a node, with complexity
  // O(numchars(srcbuffer)). Then it will do the lookup, with
  // complexity O(log(numlines(srcbuffer))).
  ryml::Location loc = parser.location(tree.rootref());
  assert(parser.location_contents(loc).begins_with("{"));
  // note the location members are zero-based:
  assert(loc.offset == 0u);
  assert(loc.line == 0u);
  assert(loc.col == 0u);
  // On the next call to location(), the accelerator is reused
  // and only the lookup is done.
  loc = parser.location(tree["aa"]);
  assert(parser.location_contents(loc).begins_with("aa"));
  assert(loc.offset == 2u);
  assert(loc.line == 1u);
  assert(loc.col == 0u);
  // KEYSEQ in flow style: points at the key
  loc = parser.location(tree["foo"]);
  assert(parser.location_contents(loc).begins_with("foo"));
  assert(loc.offset == 16u);
  assert(loc.line == 2u);
  assert(loc.col == 0u);
  loc = parser.location(tree["foo"][0]);
  assert(parser.location_contents(loc).begins_with("one"));
  assert(loc.line == 2u);
  assert(loc.col == 6u);
  // SEQ in flow style: location points at the opening '[' (there's no key)
  loc = parser.location(tree["foo"][1]);
  assert(parser.location_contents(loc).begins_with("["));
  assert(loc.line == 2u);
  assert(loc.col == 11u);
  loc = parser.location(tree["foo"][1][0]);
  assert(parser.location_contents(loc).begins_with("two"));
  assert(loc.line == 2u);
  assert(loc.col == 12u);
  loc = parser.location(tree["foo"][1][1]);
  assert(parser.location_contents(loc).begins_with("three"));
  assert(loc.line == 2u);
  assert(loc.col == 17u);
  // NOTE: reusing the parser with a new YAML source buffer
  // will invalidate the accelerator.
  ```
  See more details in the [quickstart sample](https://github.com/biojppm/rapidyaml/blob/bfb073265abf8c58bbeeeed7fb43270e9205c71c/samples/quickstart.cpp#L3759). Thanks to \@cschreib for submitting a working example proving how simple it could be to achieve this.
- `Parser`:
  - add `source()` and `filename()` to get the latest buffer and filename to be parsed
  - add `callbacks()` to get the parser's callbacks
- Add `from_tag_long()` and `normalize_tag_long()`:
  ```c++
  assert(from_tag_long(TAG_MAP) == "<tag:yaml.org,2002:map>");
  assert(normalize_tag_long("!!map") == "<tag:yaml.org,2002:map>");
  ```
- Add an experimental API to resolve tags based on the tree's tag directives. This API is still imature and will likely be subject to changes, so we won't document it yet.
- Regarding emit styles (see issue [#37](https://github.com/biojppm/rapidyaml/issues/37)): add an experimental API to force flow/block style on container nodes, as well as block-literal/block-folded/double-quoted/single-quoted/plain styles on scalar nodes. This API is also immature and will likely be subject to changes, so we won't document it yet. But if you are desperate for this functionality, the new facilities will let you go further.
- Add preliminary support for bare-metal ARM architectures, with CI tests pending implementation of QEMU action. ([#193](https://github.com/biojppm/rapidyaml/issues/193), [c4core#63](https://github.com/biojppm/c4core/issues/63)).
- Add preliminary support for RISC-V architectures, with CI tests pending availability of RISC-V based github actions. ([c4core#69](https://github.com/biojppm/c4core/pulls/69)).


### Fixes

- Fix edge cases of parsing of explicit keys (ie keys after `?`) ([PR#212](https://github.com/biojppm/rapidyaml/pulls/212)):
  ```yaml
  # all these were fixed:
  ? : # empty
  ? explicit key   # this comment was not parsed correctly
  ?    # trailing empty key was not added to the map
  ```
- Fixed parsing of tabs used as whitespace tokens after `:` or `-`. This feature [is costly (see some benchmark results here)](https://github.com/biojppm/rapidyaml/pull/211#issuecomment-1030688035) and thus it is disabled by default, and requires defining a macro or cmake option `RYML_WITH_TAB_TOKENS` to enable ([PR#211](https://github.com/biojppm/rapidyaml/pulls/211)).
- Allow tab indentation in flow seqs ([PR#215](https://github.com/biojppm/rapidyaml/pulls/215)) (6CA3).
- ryml now parses successfully compact JSON code `{"like":"this"}` without any need for preprocessing. This code was not valid YAML 1.1, but was made valid in YAML 1.2. So the `preprocess_json()` functions, used to insert spaces after `:` are no longer necessary and have been removed. If you were using these functions, remove the calls and just pass the original source directly to ryml's parser ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)).
- Fix handling of indentation when parsing block scalars ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)):
  ```yaml
  ---
  |
  hello
  there
  ---
  |
  ciao
  qua
  ---
  - |
   hello
   there
  - |
   ciao
   qua
  ---
  foo: |
   hello
   there
  bar: |
   ciao
   qua
  ```
- Fix parsing of maps when opening a scope with whitespace before the colon ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)):
  ```yaml
  foo0 : bar
  ---
  foo1 : bar  # the " :" was causing an assert
  ---
  foo2 : bar
  ---
  foo3	: bar
  ---
  foo4   	  : bar
  ```
- Ensure container keys preserve quote flags when the key is quoted ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)).
- Ensure scalars beginning with `%` are emitted with quotes (([PR#216](https://github.com/biojppm/rapidyaml/pulls/216)).
- Fix [#203](https://github.com/biojppm/rapidyaml/issues/203): when parsing, do not convert `null` or `~` to null scalar strings. Now the scalar strings contain the verbatim contents of the original scalar; to query whether a scalar value is null, use `Tree::key_is_null()/val_is_null()` and `NodeRef::key_is_null()/val_is_null()` which return true if it is empty or any of the unquoted strings `~`, `null`, `Null`, or `NULL`. ([PR#207](https://github.com/biojppm/rapidyaml/pulls/207)):
- Fix [#205](https://github.com/biojppm/rapidyaml/issues/205): fix parsing of escaped characters in double-quoted strings: `"\\\"\n\r\t\<TAB>\/\<SPC>\0\b\f\a\v\e\_\N\L\P"` ([PR#207](https://github.com/biojppm/rapidyaml/pulls/207)).
- Fix [#204](https://github.com/biojppm/rapidyaml/issues/204): add decoding of unicode codepoints `\x` `\u` `\U` in double-quoted scalars:
  ```c++
  Tree tree = parse_in_arena(R"(["\u263A \xE2\x98\xBA \u2705 \U0001D11E"])");
  assert(tree[0].val() == "☺ ☺ ✅ 𝄞");
  ```
  This is mandated by the YAML standard and was missing from ryml ([PR#207](https://github.com/biojppm/rapidyaml/pulls/207)).
- Fix emission of nested nodes which are sequences: when these are given as the emit root, the `- ` from the parent node was added ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)):
  ```c++
  const ryml::Tree tree = ryml::parse_in_arena(R"(
  - - Rochefort 10
    - Busch
    - Leffe Rituel
    - - and so
      - many other
      - wonderful beers
  )");
  // before (error), YAML valid but not expected
  //assert(ryml::emitrs<std::string>(tree[0][3]) == R"(- - and so
  //  - many other
  //  - wonderful beers
  //)");
  // now: YAML valid and expected
  assert(ryml::emitrs<std::string>(tree[0][3]) == R"(- and so
  - many other
  - wonderful beers
  )");
  ```
- Fix parsing of isolated `!`: should be an empty val tagged with `!` (UKK06-02) ([PR#215](https://github.com/biojppm/rapidyaml/pulls/215)).
- Fix [#193](https://github.com/biojppm/rapidyaml/issues/193): amalgamated header missing `#include <stdarg.h>` which prevented compilation in bare-metal `arm-none-eabi` ([PR #195](https://github.com/biojppm/rapidyaml/pull/195), requiring also [c4core #64](https://github.com/biojppm/c4core/pull/64)).
- Accept `infinity`,`inf` and `nan` as special float values (but not mixed case: eg `InFiNiTy` or `Inf` or `NaN` are not accepted) ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Accept special float values with upper or mixed case: `.Inf`, `.INF`, `.NaN`, `.NAN`. Previously, only low-case `.inf` and `.nan` were accepted ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Accept `null` with upper or mixed case: `Null` or `NULL`. Previously, only low-case `null` was accepted ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Fix [#182](https://github.com/biojppm/rapidyaml/issues/182): add missing export of DLL symbols, and document requirements for compiling shared library from the amalgamated header. [PR #183](https://github.com/biojppm/rapidyaml/pull/183), also [PR c4core#56](https://github.com/biojppm/c4core/pull/56) and [PR c4core#57](https://github.com/biojppm/c4core/pull/57).
- Fix [#185](https://github.com/biojppm/rapidyaml/issues/185): compilation failures in earlier Xcode versions ([PR #187](https://github.com/biojppm/rapidyaml/pull/187) and [PR c4core#61](https://github.com/biojppm/c4core/pull/61)):
  - `c4/substr_fwd.hpp`: (failure in Xcode 12 and earlier) forward declaration for `std::allocator` is inside the `inline namespace __1`, unlike later versions.
  - `c4/error.hpp`: (failure in debug mode in Xcode 11 and earlier) `__clang_major__` does not mean the same as in the common clang, and as a result the warning `-Wgnu-inline-cpp-without-extern` does not exist there.
- Ensure error messages do not wrap around the buffer when the YAML source line is too long ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)).
- Ensure error is emitted on unclosed flow sequence characters eg `[[[` ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)). Same thing for `[]]`.
- Refactor error message building and parser debug logging to use the new dump facilities in c4core ([PR#212](https://github.com/biojppm/rapidyaml/pulls/212)).
- Parse: fix read-after-free when duplicating a parser state node, when pushing to the stack requires a stack buffer resize ([PR#210](https://github.com/biojppm/rapidyaml/pulls/210)).
- Add support for legacy gcc 4.8 ([PR#217](https://github.com/biojppm/rapidyaml/pulls/217)).


### Improvements

- Rewrite filtering of scalars to improve parsing performance ([PR #188](https://github.com/biojppm/rapidyaml/pull/188)). Previously the scalar strings were filtered in place, which resulted in quadratic complexity in terms of scalar length. This did not matter for small scalars fitting the cache (which is the more frequent case), but grew in cost as the scalars grew larger. To achieve linearity, the code was changed so that the strings are now filtered to a temporary scratch space in the parser, and copied back to the output buffer after filtering, if any change occurred. The improvements were large for the folded scalars; the table below shows the benchmark results of throughput (MB/s) for several files containing large scalars of a single type:
  | scalar type	| before |	after |	improvement |
  |:------------|-------:|-------:|---------:|
  | block folded   | 276	| 561	| 103% |
  | block literal  | 331	| 611	| 85% |
  | single quoted  | 247	| 267	| 8% |
  | double quoted  | 212	| 230	| 8% |
  | plain (unquoted) | 173	| 186	| 8% |
  
  The cost for small scalars is negligible, with benchmark improvement in the interval of -2% to 5%, so well within the margin of benchmark variability in a regular OS. In the future, this will be optimized again by copying each character in place, thus completely avoiding the staging arena.
- `Callbacks`: add `operator==()` and `operator!=()` ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).
- `Tree`: on error or assert prefer the error callback stored into the tree's current `Callbacks`, rather than the global `Callbacks` ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).
- `detail::stack<>`: improve behavior when assigning from objects `Callbacks`, test all rule-of-5 scenarios ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).
- Improve formatting of error messages.


### Thanks

- \@ingydotnet
- \@perlpunk
- \@cschreib
- \@fargies
- \@Xeonacid
- \@aviktorov
- \@xTVaser


---------------------------------------
## 0.3.0
[Github release: 0.3.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.3.0)

### Breaking changes

Despite ryml being still in a non-stable 0.x.y version, considerable effort goes into trying to avoid breaking changes. However, this release has to collect on the [semantic versioning](https://semver.org/) prerogative for breaking changes. This is a needed improvement, so sorry for any nuisance!

**The allocation and error callback logic was revamped** on the [amalgamation PR](https://github.com/biojppm/rapidyaml/pull/172). Now trees and parsers receive (and store) a full `ryml::Callbacks` object instead of the (now removed) `ryml::Allocator` which had a pointer to a (now removed) `ryml::MemoryResourceCallbacks`, which was a (now removed) `ryml::MemoryResource`. To be clear, the `Callbacks` class is unchanged, other than removing some unneeded helper methods.

These changes were motivated by unfortunate name clashes between `c4::Allocator/ryml::Allocator` and `c4::MemoryResource/ryml::MemoryResource`, occurring if `<c4/allocator.hpp>` or `<c4/memory_resource.hpp>` were included before `<c4/yml/common.hpp>`. They also significantly simplify this part of the API, making it really easier to understand.

As a consequence of the above changes, the global memory resource getters and setters for ryml were also removed: `ryml::get_memory_resource()/ryml::set_memory_resource()`.

Here's an example of the required changes in client code. First the old client code (from the quickstart):

```c++
struct PerTreeMemoryExample : public ryml::MemoryResource
{
    void *allocate(size_t len, void * hint) override;
    void free(void *mem, size_t len) override;
};

PerTreeMemoryExample mrp;
PerTreeMemoryExample mr1;
PerTreeMemoryExample mr2;

ryml::Parser parser = {ryml::Allocator(&mrp)};
ryml::Tree   tree1  = {ryml::Allocator(&mr1)};
ryml::Tree   tree2  = {ryml::Allocator(&mr2)};
```

Should now be rewritten to:

```c++
struct PerTreeMemoryExample
{
    ryml::Callbacks callbacks() const; // helper to create the callbacks
};

PerTreeMemoryExample mrp;
PerTreeMemoryExample mr1;
PerTreeMemoryExample mr2;

ryml::Parser parser = {mrp.callbacks()};
ryml::Tree   tree1  = {mr1.callbacks()};
ryml::Tree   tree2  = {mr2.callbacks()};
```


### New features
- Add amalgamation into a single header file ([PR #172](https://github.com/biojppm/rapidyaml/pull/172)):
  - The amalgamated header will be available together with the deliverables from each release.
  - To generate the amalgamated header:
    ```console
    $ python tools/amalgamate.py ryml_all.hpp
    ```
  - To use the amalgamated header:
    - Include at will in any header of your project.
    - In one - and only one - of your project source files, `#define RYML_SINGLE_HDR_DEFINE_NOW` and then `#include <ryml_all.hpp>`. This will enable the function and class definitions in the header file. For example, here's a sample program:
      ```c++
      #include <iostream>
      #define RYML_SINGLE_HDR_DEFINE_NOW // do this before the include
      #include <ryml_all.hpp>
      int main()
      {
          auto tree = ryml::parse("{foo: bar}");
          std::cout << tree["foo"].val() << "\n";
      }
      ```
- Add `Tree::change_type()` and `NodeRef::change_type()` ([PR #171](https://github.com/biojppm/rapidyaml/pull/171)):
  ```c++
  // clears a node and sets its type to a different type (one of `VAL`, `SEQ`, `MAP`):
  Tree t = parse("{keyval0: val0, keyval1: val1, keyval2: val2}");
  t[0].change_type(VAL);
  t[1].change_type(MAP);
  t[2].change_type(SEQ);
  Tree expected = parse("{keyval0: val0, keyval1: {}, keyval2: []}");
  assert(emitrs<std::string>(t) == emitrs<std::string>(expected));
  ```
- Add support for compilation with emscripten (WebAssembly+javascript) ([PR #176](https://github.com/biojppm/rapidyaml/pull/176)).

### Fixes

- Take block literal indentation as relative to current indentation level, rather than as an absolute indentation level ([PR #178](https://github.com/biojppm/rapidyaml/pull/178)):
  ```yaml
  foo:
    - |
     child0
    - |2
      child2  # indentation is 4, not 2
  ```
- Fix parsing when seq member maps start without a key ([PR #178](https://github.com/biojppm/rapidyaml/pull/178)):
  ```yaml
  # previously this resulted in a parse error
  - - : empty key
  - - : another empty key
  ```
- Prefer passing `substr` and `csubstr` by value instead of const reference ([PR #171](https://github.com/biojppm/rapidyaml/pull/171))
- Fix [#173](https://github.com/biojppm/rapidyaml/issues/173): add alias target `ryml::ryml` ([PR #174](https://github.com/biojppm/rapidyaml/pull/174))
- Speedup compilation of tests by removing linking with yaml-cpp and libyaml. ([PR #177](https://github.com/biojppm/rapidyaml/pull/177))
- Fix [c4core#53](https://github.com/biojppm/c4core/issues/53): cmake install targets were missing call to `export()` ([PR #179](https://github.com/biojppm/c4core/pull/179)).
- Add missing export to `Tree` ([PR #181](https://github.com/biojppm/c4core/pull/181)).


### Thanks

- \@aviktorov



---------------------------------------
## 0.2.3
[Github release: 0.2.3](https://github.com/biojppm/rapidyaml/releases/tag/v0.2.3)

This release is focused on bug fixes and compliance with the [YAML test suite](https://github.com/yaml/yaml-test-suite).

### New features
- Add support for CPU architectures aarch64, ppc64le, s390x.
- Update c4core to [0.1.7](https://github.com/biojppm/c4core/releases/tag/v0.1.7)
- `Tree` and `NodeRef`: add document getter `doc()` and `docref()`
  ```c++
  Tree tree = parse(R"(---
  doc0
  ---
  doc1
  )");
  NodeRef stream = t.rootref();
  assert(stream.is_stream());
  // tree.doc(i): get the index of the i-th doc node.
  // Equivalent to tree.child(tree.root_id(), i)
  assert(tree.doc(0) == 1u);
  assert(tree.doc(1) == 2u);
  // tree.docref(i), same as above, return NodeRef
  assert(tree.docref(0).val() == "doc0");
  assert(tree.docref(1).val() == "doc1");
  // stream.doc(i), same as above, given NodeRef
  assert(stream.doc(0).val() == "doc0");
  assert(stream.doc(1).val() == "doc1");
  ```

### Fixes

- Fix compilation with `C4CORE_NO_FAST_FLOAT` ([PR #163](https://github.com/biojppm/rapidyaml/pull/163))

#### Flow maps

- Fix parse of multiline plain scalars inside flow maps ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test case UT92
  # all parsed as "matches %": 20
  - { matches
  % : 20 }
  - { matches
  %: 20 }
  - { matches
  %:
   20 }
  ```


#### Tags

- Fix parsing of tags followed by comments in sequences ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test case 735Y
  - !!map # Block collection
    foo : bar
  ```

#### Quoted scalars
- Fix filtering of tab characters in quoted scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  ---
  # test case 5GBF
  "Empty line
   <TAB>
  as a line feed"
  # now correctly parsed as "Empty line\nas a line feed"
  ---
  # test case PRH3
  ' 1st non-empty
  
  <SPC>2nd non-empty<SPC>
  <TAB>3rd non-empty '
  # now correctly parsed as " 1st non-empty\n2nd non-empty 3rd non-empty "
  ```
- Fix filtering of backslash characters in double-quoted scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test cases NP9H, Q8AD
  "folded<SPC>
  to a space,<TAB>
  <SPC>
  to a line feed, or <TAB>\
   \ <TAB>non-content"
  # now correctly parsed as "folded to a space,\nto a line feed, or \t \tnon-content"
  ```
- Ensure filtering of multiline quoted scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # all scalars now correctly parsed as "quoted string",
  # both for double and single quotes
  ---
  "quoted
  string"
  --- "quoted
  string"
  ---
  - "quoted
    string"
  ---
  - "quoted
  string"
  ---
  "quoted
    string": "quoted
    string"
  ---
  "quoted
  string": "quoted
  string"
  ```


#### Block scalars
- Ensure no newlines are added when emitting block scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161))
- Fix parsing of block spec with both chomping and indentation: chomping may come before or after the indentation ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # the block scalar specs below now have the same effect.
  # test cases: D83L, P2AD
  - |2-
    explicit indent and chomp
  - |-2
    chomp and explicit indent
  ```
- Fix [inference of block indentation](https://yaml.org/spec/1.2.2/#8111-block-indentation-indicator) with leading blank lines ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test cases: 4QFQ, 7T8X
  - >
   
    
    # child1
  # parsed as "\n\n child1"
  --- # test case DWX9
  |
   
    
    literal
     
    
    text
  
   # Comment
  # parsed as "\n\nliteral\n \n\ntext\n"
  ```
- Fix parsing of same-indentation block scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test case W4TN
  # all docs have the same value: "%!PS-Adobe-2.0"
  --- |
   %!PS-Adobe-2.0
  ...
  --- >
   %!PS-Adobe-2.0
  ...
  --- |
  %!PS-Adobe-2.0
  ...
  --- >
  %!PS-Adobe-2.0
  ...
  --- |
   %!PS-Adobe-2.0
  --- >
   %!PS-Adobe-2.0
  --- |
  %!PS-Adobe-2.0
  --- >
  %!PS-Adobe-2.0
  ```
- Folded block scalars: fix folding of newlines at the border of indented parts ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test case 6VJK
  # now correctly parsed as "Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting Average\n\nWhat a year!\n"
  >
    Sammy Sosa completed another
    fine season with great stats.
   
      63 Home Runs
      0.288 Batting Average
   
    What a year!
  ---
  # test case MJS9
  # now correctly parsed as "foo \n\n \t bar\n\nbaz\n"
  >
    foo<SPC>
  <SPC>
    <SPC><TAB><SPC>bar
  
    baz
  ```
- Folded block scalars: fix folding of newlines when the indented part is at the begining of the scalar ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  # test case F6MC
  a: >2
     more indented
    regular
  # parsed as a: " more indented\nregular\n"
  b: >2
  
  
     more indented
    regular
  # parsed as b: "\n\n more indented\nregular\n"
  ```

#### Plain scalars
- Fix parsing of whitespace within plain scalars ([PR #161](https://github.com/biojppm/rapidyaml/pull/161)):
  ```yaml
  ---
  # test case NB6Z
  key:
    value
    with
     	
    tabs
    tabs
     	
      foo
     	
        bar
          baz
     	
  # is now correctly parsed as "value with\ntabs tabs\nfoo\nbar baz"
  ---
  # test case 9YRD, EX5H (trailing whitespace)
  a
  b  
    c
  d
  
  e
  # is now correctly parsed as "a b c d\ne"
  ```
- Fix parsing of unindented plain scalars at the root level scope ([PR #161](https://github.com/biojppm/rapidyaml/pull/161))
  ```yaml
  --- # this parsed
  Bare
   scalar
   is indented
  # was correctly parsed as "Bare scalar is indented"
  --- # but this failed to parse successfully:
  Bare
  scalar
  is not indented
  # is now correctly parsed as "Bare scalar is not indented"
  --- # test case NB6Z
  value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
    	
  # now correctly parsed as "value with\ntabs tabs\nfoo\nbar baz"
  ---
  --- # test cases EXG3, 82AN
  ---word1
  word2
  # now correctly parsed as "---word1 word2"
  ```
- Fix parsing of comments within plain scalars
  ```yaml
  # test case 7TMG
  --- # now correctly parsed as "word1"
  word1
  # comment
  --- # now correctly parsed as [word1, word2]
  [ word1
  # comment
  , word2]
  ```

#### Python API
- Add missing node predicates in SWIG API definition ([PR #166](https://github.com/biojppm/rapidyaml/pull/166)):
  - `is_anchor_or_ref()`
  - `is_key_quoted()`
  - `is_val_quoted()`
  - `is_quoted()`


### Thanks

- \@mbs-c
- \@simu
- \@QuellaZhang


---------------------------------------
## 0.2.2
[Github release: 0.2.2](https://github.com/biojppm/rapidyaml/releases/tag/v0.2.2)

Yank python package 0.2.1, was accidentally created while iterating the PyPI submission from the Github action. This release does not add any change, and is functionally the same as [0.2.1](https://github.com/biojppm/rapidyaml/releases/tag/v0.2.1).


---------------------------------------
## 0.2.1
[Github release: 0.2.1](https://github.com/biojppm/rapidyaml/releases/tag/v0.2.1)

This release is focused on bug fixes and compliance with the [YAML test suite](https://github.com/yaml/yaml-test-suite).

### Breaking changes

- Fix parsing behavior of root-level scalars: now these are parsed into a DOCVAL, not SEQ->VAL ([5ba0d56](https://github.com/biojppm/rapidyaml/pull/144/commits/5ba0d56904daef1509f0073695145c4835ab1b30), from [PR #144](https://github.com/biojppm/rapidyaml/pull/144)). Eg,
  ```yaml
  ---
  this is a scalar
  --- # previously this was parsed as
  - this is a scalar
  ```
- Cleanup type predicate API ([PR #155](https://github.com/biojppm/rapidyaml/pull/155))):
  - ensure all type predicates from `Tree` and `NodeRef` forward to the corresponding predicate in `NodeType`
  - remove all type predicates and methods from `NodeData`; use the equivalent call from `Tree` or `NodeRef`. For example, for `is_map()`:
    ```c++
    Tree t = parse("{foo: bar}");
    size_t map_id = t.root_id();
    NodeRef map = t.rootref();
    t.get(map_id)->is_map(); // compile error: no longer exists
    assert(t.is_map(map_id)); // OK
    assert(map.is_map()); // OK
    ```
  - Further cleanup to the type predicate API will be done in the future, especially around the `.has_*()` vs corresponding `.is_*()` naming scheme.


### New features & improvements

- `Tree::lookup_path_or_modify()`: add overload to graft existing branches ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- Callbacks: improve test coverage ([PR #141](https://github.com/biojppm/rapidyaml/pull/141))
- [YAML test suite](https://github.com/yaml/yaml-test-suite) ([PR #144](https://github.com/biojppm/rapidyaml/pull/144), [PR #145](https://github.com/biojppm/rapidyaml/pull/145)): big progress towards compliance with the suite. There are still a number of existing problems, which are the subject of ongoing work. See the [list of current known failures](../test/test_suite/test_suite_parts.cpp) in the test suite file.
- Python wheels and source package are now [uploaded to PyPI](https://pypi.org/project/rapidyaml/) as part of the release process.


### Fixes

#### Anchors and references
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
- Fix [#151](https://github.com/biojppm/rapidyaml/issues/151): scalars beginning with `*` or `&` or `<<` are now correctly quoted when emitting ([PR #156](https://github.com/biojppm/rapidyaml/pull/156)).
- Also from [PR #156](https://github.com/biojppm/rapidyaml/pull/156), map inheritance nodes like `<<: *anchor` or `<<: [*anchor1, *anchor2]` now have a `KEYREF` flag in their type (until a call to `Tree::resolve()`):
  ```c++
  Tree tree = parse("{map: &anchor {foo: bar}, copy: {<<: *anchor}}");
  assert(tree["copy"]["<<"].is_key_ref()); // previously this did not hold
  assert(tree["copy"]["<<"].is_val_ref()); // ... but this did
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
- Fix parsing of special characters within plain scalars ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case 3MYT
  k:#foo
    &a !t s
    !t s
  # now correctly parsed as "k:#foo &a !t s !t s"
  ```
- Fix parsing of comments after complex keys ([PR #145](https://github.com/biojppm/rapidyaml/pull/145)):
  ```yaml
  # test case X8DW
  ? key
  # comment 
  : value
  # now correctly parsed as {key: value}
  ```
- Fix parsing of consecutive complex keys within maps ([PR #145](https://github.com/biojppm/rapidyaml/pull/145))
  ```yaml
  # test case 7W2P, ZWK4
  ? a
  ? b
  c:
  ? d
  e:
  # now correctly parsed as {a: ~, b: ~, c: ~, d: ~, e: ~}
  ```
- Fix [#152](https://github.com/biojppm/rapidyaml/issues/152):  parse error with folded scalars that are the last in a container ([PR #157](https://github.com/biojppm/rapidyaml/pull/157)):
  ```yaml
  exec:
    command:
      # before the fix, this folded scalar failed to parse
      - |
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    parses: no
  ```
- Fix: documents consisting of a quoted scalar now retain the VALQUO flag ([PR #156](https://github.com/biojppm/rapidyaml/pull/156))
  ```c++
  Tree tree = parse("'this is a quoted scalar'");
  assert(tree.rootref().is_doc());
  assert(tree.rootref().is_val());
  assert(tree.rootref().is_val_quoted());
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
- Update c4core to [0.1.4](https://github.com/biojppm/c4core/releases/tag/v0.1.4)


### Special thanks

- \@Gei0r



---------------------------------------
## 0.2.0

[Github release: 0.2.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.2.0)

### New features & improvements
- Enable parsing into nested nodes ([87f4184](https://github.com/biojppm/rapidyaml/commit/87f4184))
- `as_json()` can now be called with tree and node id ([4c23041](https://github.com/biojppm/rapidyaml/commit/4c23041))
- Add `Parser::reserve_stack()` ([f31fb9f](https://github.com/biojppm/rapidyaml/commit/f31fb9f))
- Add uninstall target ([PR #122](https://github.com/biojppm/rapidyaml/pull/122))
- Update [c4core](https://github.com/biojppm/c4core) to v0.1.1
- Add a [quickstart sample](samples/quickstart.cpp) with build examples.
- Update README.md to refer to the quickstart
- Add [gdb visualizers](src/ryml-gdbtypes.py)
- Add `SO_VERSION` to shared builds

### Fixes
- Fix [#139](https://github.com/biojppm/rapidyaml/issues/139): substr and csubstr not found in ryml namespace
- Fix [#131](https://github.com/biojppm/rapidyaml/issues/131): resolve references to map keys
- Fix [#129](https://github.com/biojppm/rapidyaml/issues/129): quoted strings starting with * parsed as references
- Fix [#128](https://github.com/biojppm/rapidyaml/issues/128): segfault on nonexistent anchor
- Fix [#124](https://github.com/biojppm/rapidyaml/issues/124): parse failure in comments with trailing colon
- Fix [#121](https://github.com/biojppm/rapidyaml/issues/121): preserve quotes when emitting scalars
- Fix [#103](https://github.com/biojppm/rapidyaml/issues/103): ambiguous parsing of null/empty scalars
- Fix [#90](https://github.com/biojppm/rapidyaml/issues/90): CMAKE_CXX_STANDARD ignored
- Fix [#40](https://github.com/biojppm/rapidyaml/issues/40): quadratic complexity from use of `sscanf(%f)`
- Fix emitting json to streams ([dc6af83](https://github.com/biojppm/rapidyaml/commit/dc6af83))
- Set the global memory resource when setting global callbacks ([511cba0](https://github.com/biojppm/rapidyaml/commit/511cba0))
- Fix python packaging ([PR #102](https://github.com/biojppm/rapidyaml/pull/102))

### Special thanks
- \@Gei0r
- \@litghost
- \@costashatz


---------------------------------------
## 0.1.0
[Github release: 0.1.0](https://github.com/biojppm/rapidyaml/releases/tag/v0.1.0)

This is the first ryml release. Future releases will have a more organized changelog; for now, only recent major changes are listed.

Please be aware that there are still some anticipated breaking changes in the API before releasing the 1.0 major version. These are highlighted in [the repo ROADMAP](https://github.com/biojppm/rapidyaml/blob/v0.1.0/ROADMAP.md).

* 2020/October
  * [MR#89](https://github.com/biojppm/rapidyaml/pull/89):
     * fix python API generation in windows
     * use github actions for testing and releasing
  * [MR#88](https://github.com/biojppm/rapidyaml/pull/88): [fix MacOS compilation and installs](https://github.com/biojppm/rapidyaml/issues/75). This is a fix from [c4core](https://github.com/biojppm/cmake/issues/1).
  * [MR#88](https://github.com/biojppm/rapidyaml/pull/88): [fix boolean handling](https://github.com/biojppm/rapidyaml/issues/74). This is a fix from [c4core](https://github.com/biojppm/c4core/pull/18/).  `true` and  `false` are now parsed correctly into `bool` variables:
    ```c++
    auto tree = parse("{foo: true, bar: false}");
    ```
    Emitting `bool` variables still defaults to `0`/`1`, like the default behaviour in the STL. To explicitly request `true`/`false` use `c4::fmt::boolalpha()`:
    ```c++
    node << var;                     // "1"    or "0"
    node << c4::fmt::boolalpha(var); // "true" or "false"
    ```
* 2020/September
  * [***Breaking change***] [MR#85](https://github.com/biojppm/rapidyaml/pull/85) null values in YAML are now parsed to null strings instead of YAML null token "~":
    ```c++
    auto tree = parse("{foo: , bar: ''}");
    // previous:
    assert(tree["foo"].val() == "~");
    assert(tree["bar"].val() == "");
    // now:
    assert(tree["foo"].val() == nullptr); // notice that this is now null
    assert(tree["bar"].val() == "");
    ```
  * [MR#85](https://github.com/biojppm/rapidyaml/pull/85) Commas after tags are now allowed:
    ```yaml
    {foo: !!str, bar: ''}  # now the comma does not cause an error
    ```
  * [MR#81](https://github.com/biojppm/rapidyaml/pull/81): Always compile with extra pedantic warnings.
* 2020/May
  *  [***Breaking change***] the error callback now receives a source location object:
    ```c++
    // previous
    using pfn_error = void (*)(const char* msg, size_t msg_len, void *user_data);
    // now:
    using pfn_error = void (*)(const char* msg, size_t msg_len, Location location, void *user_data);
    ```
  * Parser fixes to improve test suite success: [MR#73](https://github.com/biojppm/rapidyaml/pull/73), [MR#71](https://github.com/biojppm/rapidyaml/pull/71), [MR#68](https://github.com/biojppm/rapidyaml/pull/68), [MR#67](https://github.com/biojppm/rapidyaml/pull/67), [MR#66](https://github.com/biojppm/rapidyaml/pull/66)
  * Fix compilation as DLL on windows [MR#69](https://github.com/biojppm/rapidyaml/pull/69)
