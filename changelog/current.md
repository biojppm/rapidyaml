- Fix [256](https://github.com/biojppm/rapidyaml/issues/256): installation directory on Linux 64bit ([PR#599](https://github.com/biojppm/rapidyaml/pull/599)). See also original fix at [cmake#16](https://github.com/biojppm/cmake/pull/16). Big thanks to @GabrielBarrantes and @musicinmybrain, not just for their fixes but also for all their downstream work!
- Fix [600](https://github.com/biojppm/rapidyaml/issues/600): missing symbols in shared library on Windows ([PR#601](https://github.com/biojppm/rapidyaml/pull/601)).
- [PR#604](https://github.com/biojppm/rapidyaml/pull/604): add string_view and span to the `c4/yml/std/std.hpp` interop umbrella header.
- [PR#605](https://github.com/biojppm/rapidyaml/pull/605): fix parse errors:
  - `:` and `#` on continuation lines of multiline plain scalars:
     ```yaml
     [plain
      :scalar] # leading colon belongs to the scalar
     ```
  - Tabs inside quoted scalars opening a map were being mistook for indentation tabs:
    ```yaml
    foo:
      '\ta': b
    ```
- [PR#606](https://github.com/biojppm/rapidyaml/pull/606): scalar style utilities now are specific to flow/block mode. This enables stable-style roundtrips of block-mode plain scalars with characters invalid in flow mode:
  ```yaml
  # these scalars were previously emitted as single-quoted style
  - doe: a deer, a female deer
  - (10,11)
  - also scalars containing [] and {}
  ```
  - `scalar_style_choose()` was split into `scalar_style_choose_{flow,block}()`
  - `scalar_style_query_plain()` was split into `scalar_style_query_plain_{flow,block}()`
  - `NodeType::type_str(NodeType,flags)`: remove zero-termination, use common approach of returning required size
  - Add `NodeType::type_str_sub()`


### Thanks

 - @GabrielBarrantes
 - @musicinmybrain
