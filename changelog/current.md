### Parser refactor

The parser was completely refactored ([#PR414](https://github.com/biojppm/rapidyaml/pull/414)). This was a large and hard job carried out over several months, and the result is:

- A new event-based parser engine is now in place, enabling the improvements described below. This engine uses a templated event handler. The parsing code was fully rewritten, and is now much more simple (albeit longer), and much easier to work with and fix.
- YAML standard-conformance was improved significantly. Along with many smaller fixes and additions, (too many to list here), the main changes are the following:
  - The parser engine can now successfully parse container keys, **but** as before, the ryml tree cannot accomodate these.
  - Anchor keys can now be terminated with colon (eg, `&anchor: key: val`).
- The parser engine can now be used to create native trees in other programming languages, or in cases where the user *must* have container keys.
- A strict JSON parser was added. Use the `parse_json_...()` family of functions to parse json in stricter mode than flow-style YAML.
- The YAML style information is now fully preserved through parsing/emitting round trips. This was made possible because the event model of the parsing engine now incorporates style varieties. So, for example:
  - a scalar parsed from a plain/single-quoted/double-quoted/block-literal/block-folded scalar will be emitted always using its original style in the YAML source
  - a container parsed in block-style will always be emitted in block-style
  - a container parsed in flow-style will always be emitted in flow-style
  Because of this, the style of YAML emitted by ryml changes with from before, if that YAML was parsed through ryml.
- Scalar filtering was improved and is now done in place, except where the scalar expands and does not fit its initial range, in which case the scalar is filtered out of place to the tree's arena.
  - The parser now offers methods to filter scalars in place or out of place. Filtering can be disabled while parsing, to ensure a fully-readonly parse (but this feature is still experimental and untested, given the scope of the rewrite work). 
- Parsing performance improved (benchmark results incoming) from reduced parser branching.
- Emitting performance improved (benchmark results incoming), as the emitting code no longer has to read the full scalars to decide on an appropriate emit style.
- Emitting helper predicates were added:
  ```
  /** choose an emitting style based on the scalar's contents */
  type_bits scalar_style_choose(csubstr scalar) noexcept;
  /** query whether a scalar can be encoded using single quotes. This is
   * possible, notably when there is leading whitespace after a newline. */
  bool scalar_style_query_squo(csubstr s) noexcept;
  /** query whether a scalar can be encoded using plain style (no
   * quotes, not a literal/folded block scalar). */
  bool scalar_style_query_plain(csubstr s) noexcept;
  ```


As a result of the refactor, there are some limited changes with impact in client code. Even though this was a large refactor, effort was directed at keeping maximal backwards compatibility, and the changes are not wide. But they still exist:
- The existing `parse_...()` methods in the `Parser` class were all removed. Use the corresponding `parse_...(Parser*, ...)` function from the header [`c4/yml/parse.hpp`](https://github.com/biojppm/master/src/c4/yml/parse.hpp).
- When instantiated by the user, the parser now needs to receive a `EventHandlerTree` object, which is responsible for building the tree. Although fully functional and tested, the structure of this class is still somewhat experimental and is still likely to change. There is an alternative event handler implementation responsible for producing the events for the YAML test suite in `test/test_suite/test_suite_event_handler.hpp`.
- The declaration and definition of `NodeType` was moved to a separate header file `c4/yml/node_type.hpp` (previously it was in `c4/yml/tree.hpp`).
- Some of the node type flags were removed, and several flags (and combination flags) were added. 
  - Most of the existing flags are kept, as well as their meaning.
  - `KEYQUO` and `VALQUO` are now masks of the several style flags for quoted scalars. In general, however, client code using these flags and `.is_val_quoted()` or `.is_key_quoted()` is not likely to require any changes.


### Fixes

- Fix [#361](https://github.com/biojppm/rapidyaml/pull/361) - parse error on map scalars containing `:` and starting on the next line:
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
- Fix [#356](https://github.com/biojppm/rapidyaml/issues/356) - fix overzealous check in `emit_as()`. An id may be larger than the tree's size, eg when nodes were removed.
([PR#357](https://github.com/biojppm/rapidyaml/pull/357)).
