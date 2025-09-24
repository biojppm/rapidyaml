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
