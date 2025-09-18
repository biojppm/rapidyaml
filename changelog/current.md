### Extra event handlers

[PR#536](https://github.com/biojppm/rapidyaml/pull/536) adds a new
major extra feature: a parser event handler that creates a compact
representation of the YAML tree in a buffer of integers containing
masks (to represent events) and offset+length (to represent strings in
the source buffer).

This handler is meant for use by other programming languages, and it
supports container keys (unlike the ryml tree). See this handler among
the other headers in
[src_extra](https://github.com/biojppm/rapidyaml/tree/master/src_extra).


### Changes

- In [PR#536](https://github.com/biojppm/rapidyaml/pull/536) the location functions were moved to `Tree` and `ConstNodeRef`. The parser is now handler-agnostic (these location functions were a legacy of the initial parser which was meant to create only ryml trees).


### Fixes

- Fix [#524](https://github.com/biojppm/rapidyaml/issues/524) ([PR#525](https://github.com/biojppm/rapidyaml/pull/525)): problem parsing nested map value in complex map. Kudos to @MatthewSteel!
- [PR#541](https://github.com/biojppm/rapidyaml/pull/541): `std::is_trivial` deprecated in c++26. Thanks to @P3RK4N!
- Fix argument handling in ryml-parse-emit


### Thanks

- @MatthewSteel
- @P3RK4N
