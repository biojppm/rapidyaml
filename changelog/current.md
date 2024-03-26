### Parser refactor

The parser was completely refactored. This was a large and hard job carried out over several months, but the result is:

- A new event-based parser engine was added, enabling the improvements described below. This engine uses a templated event handler. The parsing code was fully rewritten, and is now much more simple, and much easier to work with and fix.
- YAML standard-conformance was improved significantly. Along with many other smaller fixes (too many to list here), the main changes are the following:
  - The parser engine can now successfully parse container keys, **but** as before, the ryml tree cannot accomodate these.
  - Anchor keys can now be terminated with colon (eg, `&anchor: key: val`).
- The parser engine can be used to create native trees in other programming languages, or in cases where the user *must* have container keys.
- The YAML style information is now fully preserved through parsing/emitting round trips. This was made possible because the event model of the parsing engine now incorporates style varieties. So, for example:
  - a scalar parsed from a plain/single-quoted/double-quoted/block-literal/block-folded scalar will be emitted always using its original style in the YAML source
  - a container parsed in block-style will always be emitted in block-style
  - a container parsed in flow-style will always be emitted in flow-style
  Because of this, the style of YAML emitted by ryml will now change, if that YAML was parsed through ryml.
- Scalar filtering was improved and is now done in place, except where the scalar expands and does not fit its initial range, in which case the scalar is filtered out of place to the tree's arena.
  - The parser now offers methods to filter scalars in place or out of place. Filtering can be disabled while parsing, to ensure a fully-readonly parse, but this feature is still experimental and untested, given the scope of the rewrite work. 
- Parsing performance improved (benchmark results incoming) from reduced parser branching.
- Emitting performance improved (benchmark results incoming), as the emitting code no longer has to read the full scalars to decide on an appropriate emit style.

As a result of the refactor, there are several changes with potential impact in client code:
- The existing parser methods were all removed. Use the corresponding `c4::yml::parse_...(Parser*, ...)` function from the header `c4/yml/parse.hpp`.
- `NodeType` declaration went to a separate header file `c4/yml/node_type.hpp` (previously it was in `c4/yml/tree.hpp`).
- Some of the node type flags were removed, and some other flags were added. Caveat emptor.
  - Notably `KEYQUO` and `VALQUO` are now masks of the several quoted style flags, and saw their meaning adjusted accordingly.


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
