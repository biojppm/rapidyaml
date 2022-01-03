### Fixes

- Accept `infinity`,`inf` and `nan` as special float values (but not mixed case: eg `INFINITY` or `Inf` or `NaN` are not accepted) ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Accept special float values with upper or mixed case: `.Inf`, `.INF`, `.NaN`, `.NAN`. Previously, only low-case `.inf` and `.nan` were accepted ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Accept `null` with upper or mixed case: `Null` or `NULL`. Previously, only low-case `null` was accepted ([PR #186](https://github.com/biojppm/rapidyaml/pull/186)).
- Fix [#182](https://github.com/biojppm/rapidyaml/issues/182): add missing export of DLL symbols, and document requirements for compiling shared library from the amalgamated header. [PR #183](https://github.com/biojppm/rapidyaml/pull/183), also [PR c4core#56](https://github.com/biojppm/c4core/pull/56) and [PR c4core#57](https://github.com/biojppm/c4core/pull/57).
- Fix [#185](https://github.com/biojppm/rapidyaml/issues/185): compilation failures in earlier Xcode versions ([PR #187](https://github.com/biojppm/rapidyaml/pull/187) and [PR c4core#61](https://github.com/biojppm/c4core/pull/61)):
  - `c4/substr_fwd.hpp`: (failure in Xcode 12 and earlier) forward declaration for `std::allocator` is inside the `inline namespace __1`, unlike later versions.
  - `c4/error.hpp`: (failure in debug mode in Xcode 11 and earlier) `__clang_major__` does not mean the same as in the common clang, and as a result the warning `-Wgnu-inline-cpp-without-extern` does not exist there.


### Improvements

- `Callbacks`: add `operator==()` and `operator!=()` ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).
- `Tree`: on error or assert prefer the error callback stored into the tree's current `Callbacks`, rather than the global `Callbacks` ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).
- `detail::stack<>`: improve behavior when assigning from objects `Callbacks`, test all rule-of-5 scenarios ([PR #168](https://github.com/biojppm/rapidyaml/pull/168)).


### Thanks

- @aviktorov
