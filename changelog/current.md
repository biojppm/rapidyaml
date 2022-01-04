### Breaking changes

- Rename `c4::yml::parse()` and `c4::yml::Parser::parse()` overloads
  to either `parse_in_place()` or `parse_in_arena()`:
  - `parse_in_place()` receives only `substr` buffers, ie mutable YAML
    source buffers. Trying to pass a `csubstr` buffer to
    `parse_in_place()` will cause a compile error:
    ```c++
    substr readwrite = /*...*/;
    Tree tree = parse_in_place(readwrite); // OK
    
    csubstr readonly = /*...*/;
    Tree tree = parse_in_place(readonly); // compile error
    ```
  - `parse_in_arena()` receives only `csubstr` buffers, ie immutable
    YAML source buffers. Prior to parsing, the buffer is copied to
    the tree's arena, then the copy is parsed in place. Because
    `parse_in_arena()` is meant for immutable buffers, overloads
    receiving a `substr` YAML buffer are now declared, and
    intentionally left undefined, such that calling
    `parse_in_arena()` with a `substr` will cause a linker
    error.
    ```c++
    substr readwrite = /*...*/;
    Tree tree = parse_in_arena(readwrite); // linker error
    ```
    This is to prevent an accidental copy of the source buffer
    to the tree's arena, because `substr` is implicitly convertible
    to `csubstr`. If you really intend to parse an originally mutable
    buffer in the tree's arena, convert it first to immutable by
    assigning the `substr` to a `csubstr` prior to calling
    `parse_in_arena()`:
    ```c++
    substr readwrite = /*...*/;
    csubstr as_readonly = readwrite; // ok
    Tree tree = parse_in_arena(as_readonly); // ok
    ```
    This approach is not needed for `parse_in_place()`
    because `csubstr` is not implicitly convertible to `substr`.
- `Callbacks`: changed behavior in `Parser` and `Tree`:
  - When a parser creates a tree, the tree will now use a copy of the
    parser's callbacks object.
  - When an existing tree is given to a parser, the parser's callbacks
    object will be overwritten by that of the tree. If the parser
    contains any allocated memory, that memory will be freed before
    any parsing occurs, and then the parser's callbacks will be
    changed to those of the tree.

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
