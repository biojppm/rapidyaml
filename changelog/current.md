### Extra event handlers

[PR#536](https://github.com/biojppm/rapidyaml/pull/536) adds a new major extra feature: a parser event handler that creates a compact representation of the YAML tree in a buffer of integers containing masks (to represent events) and offset+length (to represent strings in the source buffer).

This handler is meant for use by other programming languages, and it supports container keys (unlike the ryml tree). You can find this handler among the other headers in the [new `src_extra` folder](https://github.com/biojppm/rapidyaml/tree/master/src_extra).


### Changes

- In [PR#536](https://github.com/biojppm/rapidyaml/pull/536) the location functions were moved from `ParserEngine` to `Tree` and `ConstNodeRef`. The parser engine is now fully agnostic vis-a-vis the type of the event-handler. (The location functions in the parser engine were a legacy of the initial implementation of the parser which was meant to create only ryml trees).
- The tool ryml-yaml-events was updated to also dump integer events (and its command line options were changed to enable the different choices).


### Fixes

- Fix [#524](https://github.com/biojppm/rapidyaml/issues/524) ([PR#525](https://github.com/biojppm/rapidyaml/pull/525)): problem parsing nested map value in complex map. Kudos to @MatthewSteel!
- [PR#542](https://github.com/biojppm/rapidyaml/pull/542): `\x` Unicode sequences were not decoded. Thanks to @mutativesystems!
- [PR#541](https://github.com/biojppm/rapidyaml/pull/541): `std::is_trivial` deprecated in c++26. Thanks to @P3RK4N!
- Fix [#529](https://github.com/biojppm/rapidyaml/issues/529) ([PR#530](https://github.com/biojppm/rapidyaml/pull/530)): double-quoted `"<<"` was mistaken for an inheriting reference.
- [PR#543](https://github.com/biojppm/rapidyaml/pull/543): improvements to experimental style API:
  - Add getters to `NodeType`, `Tree`, `NodeRef`, and `ConstNodeRef`:
    - `.key_style()`: get the style flags in a node's key
    - `.val_style()`: get the style flags in a node's val
    - `.container_style()`: get the style flags in a node's container
  - Add style modifiers to `NodeType`, `Tree`, `NodeRef`, and `ConstNodeRef`:
    - `.clear_style(bool recurse)`
    - `.set_style_conditionally(bool recurse)`
- Fix argument handling in ryml-parse-emit.


### Thanks

- @MatthewSteel
- @mutativesystems
- @P3RK4N
