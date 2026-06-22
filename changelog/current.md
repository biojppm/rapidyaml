- [PR#620](https://github.com/biojppm/rapidyaml/pull/620) API cleanup: `Tree` and `NodeRef`:
  - Deprecate `NodeInit`
  - `Tree` and `NodeRef`:
    - deprecate `.to_val()` and friends -- add `.set_val()` and friends.
    - deprecate `operator=(csubstr)` and friends -- use `.set_val()` instead.
    - deprecate `operator|=(NodeType)` and `operator=(NodeType)` -- use appropriate overload `.set_*(T, NodeType)`.
    - Migration:
      ```c++
      NodeRef node = ...;
      // before (deprecated)
      node |= MAP|BLOCK;
      node["key"] = "val";
      node["key"] |= VAL_SQUO;
      node["seq"] |= SEQ|FLOW;
      node["seq2"] |= SEQ;
      // now:
      node.set_map(BLOCK);
      node["key"].set_val("val", VAL_SQUO);
      node["seq"].set_seq(FLOW);
      node["seq2"].set_seq();
      // see the doxygen documentation
      ```
    - You can disable compiler deprecation warnings from use of these operators: by enabling the cmake variable (or defining the macro) `RYML_WITH_LEGACY
    _OPERATORS`.
    - deprecate `NodeInit` and `NodeScalar` methods (use `.set_*()`)
    - deprecate single-arg `NodeRef::{duplicate,move}(ConstNodeRef)`
    - deprecate `NodeRef::visit()` and `NodeRef::visit_stacked()`
    - add `Tree::arena_rem()`
    - add `RYML_DEFAULT_TREE_ARENA_CAPACITY_START` with default value of 256
  - `parse_*()`: internal simplification, no semantic changes
- [PR#589](https://github.com/biojppm/rapidyaml/pull/589) API cleanup: serialization and tree building
  - Refactor serialization code:
    - Deprecate `operator<<`, use `.load()` / `.load_key()` or `.deserialize()` / `.deserialize_key()`
    - Deprecate `operator>>`, use `.save()` / `.save_key()` or `.set_serialized()` / `.set_key_serialized()`
    - Deprecate `key()` tag function and `Key` tag type (needed only for the operators above)
    - Migration of code triggering serialization:
      ```c++
      NodeRef node = ...;
      T var1 = ..., var2 = ...;
      // before (deprecated)
      node << var1;
      node >> var2;
      node << key(var1);
      node >> key(var2);
      // now - with checks and call to error
      node.save(var1);
      node.load(&var2);
      node.save_key(var1);
      node.load_key(&var2);
      // or now - without exceptional flow
      node.set_serialized(var1);
      if( ! node.deserialize(&var2)) ...;
      node.set_key_serialized(var1);
      if( ! node.deserialize_key(&var2)) ...;
      // see the doxygen documentation
      ```
    - You can disable compiler deprecation warnings from use of these operators: by enabling the cmake variable (or defining the macro) `RYML_WITH_LEGACY_OPERATORS`.
    - Serialization with `Tree` API is now fully implemented, working exactly the same as `NodeRef`
      - serialization:
        - `NodeRef` serialized with `write(NodeRef *, T const&)` and `write_key()`
        - `Tree` serialized with `write(Tree *, id_type, T const&)` and `write_key()`
        - No changes in types using `to_chars()` serialization
      - deserialization:
        - `ConstNodeRef` deserialized with `read(ConstNodeRef const&, T *)` and `read_key()`
        - `Tree` deserialized with `read(Tree const*, id_type, T *)` and `read_key()` (removed old `readkey()` approach.)
        - No changes in types using `from_chars()` serialization
      - Enables bypass of `NodeRef` serialization to use the tree API, which is more efficient/faster to compile.
      - Organize serialization in layers, ensuring use of free functions to enable the user to override at all levels with ADL, for total control of type behavior (explained in the doxygen documentation).
      - Added many more unit tests to ensure all scenarios are working.
  - Move scalar specific code to new top-level headers `c4/yml/scalar_charconv.hpp` and `c4/yml/scalar_style.hpp`
  - Relax deserialization of nodes with `VALNIL` or `KEYNIL`, such that non-fundamental types can now be null-initialized (eg initialize a string from a null scalar). Fundamental types such as ints or floats will still report an error.
    ```c++
    Tree tree = parse_in_arena("{empty: }");
    std::string str;
    tree["empty"].load(&str); // relaxed: no longer a deserialization error
    assert(s.empty());
    int val;
    tree["empty"].load(&val); // ERROR! as before.
    ```
- [PR#626](https://github.com/biojppm/rapidyaml/pull/626): ensure accurate reporting of nodes causing deeply nested serialization error. To profit from this, `read()` user functions should now return `ReadResult` instead of `bool`. This change is backwards-compatible with legacy `read()` functions returning `bool`, but the legacy version remains less precise. See the new `sample_deserialize_error()` and the [latest doxygen documentation](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__serialization__user__types.html) . The user functions should change (but not mandatory):
  ```c++
  // before
  bool read(ryml::ConstNodeRef n, T *var) { ... }
  // now
  ryml::ReadResult read(ryml::ConstNodeRef n, T *var) { ... }
  ```
- [PR#625](https://github.com/biojppm/rapidyaml/pull/625): Set plain scalar style when serializing arithmetic scalars: improves emit speed of numeric-heavy data payloads.
- [PR#616](https://github.com/biojppm/rapidyaml/pull/616) API cleanup: emit
  - `WriterFile` and `WriterOStream` no longer track the number of emitted bytes.
  - `error_on_excess` is now used in the emit-to-buffer overloads, and no longer in the main `Emitter::emit_as()` driver function.
- [PR#617](https://github.com/biojppm/rapidyaml/pull/617) API cleanup: emit, part 2
    - Tidy emit classes among new top-level header files:
      - `c4/yml/emit_container.hpp`: emit to resizeable contiguous char container (eg `std::string`, `std::vector<char>`)
      - `c4/yml/emit_buf.hpp`: emit to char buffer (`substr`)
      - `c4/yml/emit_file.hpp`: emit to C `FILE*`
      - `c4/yml/emit_ostream.hpp`: emit to STL-like ostreams
    - The old `c4/yml/emit.hpp` is now a pure umbrella header, including all of the above. For better compilation speed, avoid the umbrella header, and prefer including the concrete header (container, buf, file or stream).
    - The rest of the emit code was split over these new implementation headers:
      - `c4/yml/emit_options.hpp`: options to control emitting
      - `c4/yml/emitter.hpp`: main emitter class
      - `c4/yml/emitter.def.hpp`: definitions of main emitter class. 
      - `c4/yml/writer_buf.hpp`: policy class to emit to char buffer (`substr`)
      - `c4/yml/writer_file.hpp`: policy class to emit to C `FILE*`
      - `c4/yml/writer_ostream.hpp`: policy class to emit to STL-like ostreams
    - There are no semantic changes: all the `emit_*()` functions remain the same.
    - Other changes in this PR:
      - Added `Tree::root_id_maybe()` which is safe to call on an empty tree.
      - Deprecate `Emitter::max_depth()`
      - Deprecate `Emitter::options()` setter
- [PR#618](https://github.com/biojppm/rapidyaml/pull/618): API cleanup: emit, part 3
  - Improve handling of `NaN` and `Inf` in json emitting.
  - Expose scalar style helpers for json emitting:
    ```c++
    bool scalar_is_plain_number_json();
    bool scalar_is_special_json();
    bool scalar_is_inf3();
    bool scalar_is_nan3();
    bool scalar_is_inf_or_nan3();
    ```
  - Writers: add `C4_ALWAYS_INLINE`. Results in ~10-20% emit improvements.
  - `file_put_contents()`: add `FILE*` overloads
- [PR#621](https://github.com/biojppm/rapidyaml/pull/621) API cleanup: `NodeRef`:
  - Simplify internal implementation of `{Const}NodeRef::{iterator,children_view}`.
  - Stop using SFINAE on Node CRTP to distinguish const vs non const, by duplicating the functions in `NodeRef` vs `ConstNodeRef`. No semantic changes. This should improve compilation speed of code containing many node calls.
- [PR#622](https://github.com/biojppm/rapidyaml/pull/622) API cleanup: remove preprocess utilities.
- [PR#623](https://github.com/biojppm/rapidyaml/pull/623): YAML fuzzing fixes, and close to 1 billion fuzz runs without any errors. These were the only two problems found:
  - Ensure parse error on multiline keys opening YAML:
    ```yaml
    multiline
      key: value
    ```
  - Fix parse error on Byte Order Mark opening containers:
    ```yaml
    <BOM>- 
      - a
    -
    ```
- [PR#628](https://github.com/biojppm/rapidyaml/pull/628): Add serialization fuzzing. Relax `c4::atof()` / `c4::atod()`, disable redundant assertions that prevent returning false on bad strings.
- [PR#629](https://github.com/biojppm/rapidyaml/pull/629): int events:
  - Rename `to_chars(substr,DataType)` to `to_str()` (the existing c4 overload would always win because it's an int)
  - Do not use c4/bitmask.hpp
- [PR#633](https://github.com/biojppm/rapidyaml/pull/633): improve coverage to ~99.5%:
  - Add tests to cover missed lines
  - Change some errors to assertions; those errors are caught before calling.
  - `Tree` and `NodeRef`: deprecate `.type_str(). Use `.type().type_str()`
  - Tools: add ints parsing to ryml-emit-parse
- [PR#634](https://github.com/biojppm/rapidyaml/pull/664): int events: add sample containing ints-only library
