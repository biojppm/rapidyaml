**All the changes described come from a single PR: [#PR414](https://github.com/biojppm/rapidyaml/pull/414).**


### Parser refactor

The parser was completely refactored ([#PR414](https://github.com/biojppm/rapidyaml/pull/414)). This was a large and hard job carried out over several months, and the result is:

- A new event-based parser engine is now in place, enabling the improvements described below. This engine uses a templated event handler, where each event is a function call, which spares branches on the event handler. The parsing code was fully rewritten, and is now much more simple (albeit longer), and much easier to work with and fix.
- YAML standard-conformance was improved significantly. Along with many smaller fixes and additions, (too many to list here), the main changes are the following:
  - The parser engine can now successfully parse container keys, emitting all the events in the correct , **but** as before, the ryml tree cannot accomodate these (and this constraint is no longer enforced by the parser, but instead by `EventHandlerTree`). For an example of a handler which can accomodate key containers, see the one which is used for the test suite at `test/test_suite/test_suite_event_handler.hpp`
  - Anchor keys can now be terminated with colon (eg, `&anchor: key: val`), as dictated by the standard.
- The parser engine can now be used to create native trees in other programming languages, or in cases where the user *must* have container keys.
- Parsing performance improved (benchmark results incoming) from reduced parser branching.
- Emitting performance improved (benchmark results incoming), as the emitting code no longer has to read the full scalars to decide on an appropriate emit style.


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

- The existing `parse_...()` methods in the `Parser` class were all removed. Use the corresponding `parse_...(Parser*, ...)` function from the header [`c4/yml/parse.hpp`](https://github.com/biojppm/master/src/c4/yml/parse.hpp) (link valid after this branch is merged).
- When instantiated by the user, the parser now needs to receive a `EventHandlerTree` object, which is responsible for building the tree. Although fully functional and tested, the structure of this class is still somewhat experimental and is still likely to change. There is an alternative event handler implementation responsible for producing the events for the YAML test suite in `test/test_suite/test_suite_event_handler.hpp`.
- The declaration and definition of `NodeType` was moved to a separate header file `c4/yml/node_type.hpp` (previously it was in `c4/yml/tree.hpp`).
- Some of the node type flags were removed, and several flags (and combination flags) were added. 
  - Most of the existing flags are kept, as well as their meaning.
  - `KEYQUO` and `VALQUO` are now masks of the several style flags for quoted scalars. In general, however, client code using these flags and `.is_val_quoted()` or `.is_key_quoted()` is not likely to require any changes.


### New type for node IDs

A type `id_type` was added to signify the integer type for the node id, defaulting to the backwards-compatible `size_t` which was previously used in the tree. In the future, this type is likely to change, *and probably to a signed type*, so client code is encouraged to always use `id_type` instead of the `size_t`, and specifically not to rely on the signedness of this type.


### Reference resolver is now exposed

The reference (ie, alias) resolver object is now exposed in
[`c4/yml/reference_resolver.hpp`](https://github.com/biojppm/master/src/c4/yml/reference_resolver.hpp) (link valid after this PR is merged). Previously this object was temporarily instantiated in `Tree::resolve()`. Exposing it now enables the user to reuse this object through different calls, saving a potential allocation on every call.
