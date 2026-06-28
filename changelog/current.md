## TL;DR

This release is focused on API cleaning and tidying, **in preparation of release 1.0**:

  - adds serialization to/from `Tree` (not just `NodeRef`/`ConstNodeRef`)
  - improves tracking of nodes causing deserialization errors
  - deprecates a good number of functions, mostly around serialization and tree/node building
  - **brings code coverage to 100%** (rounded from >99.5%)

Significant effort went into maintaining backward compatibility. Nevertheless the cleanup introduced a number of breaking changes, and some other opt-in but recommended changes: see the migration guide in the next section.

The second major change is the removal of the c4core submodule, vendoring in the c4core source code. Now **cloning with git no longer requires `--recursive`**. Other than that, this change is mostly internal, and all existing workflows to consume rapidyaml and or c4core are kept unchanged (eg `RYML_STANDALONE`, or the newly added `RYML_SYSTEM_C4CORE` still work the same).

Finally there are other fixes and improvements. For the details refer to the [full changelog](v0.16changelogsection), below the migration guide.


## Migration guide

This is the list of migration changes, linking to the relevant section below:

 - Deprecated (but define `RYML_WITH_LEGACY_OPERATORS` to avoid the deprecation):
    - tree building: [do not use operators `|=` or `=`](#notequal)
    - serialization: [do not use operators `<<` or `>>`](#notshift)
 - Optional, **recommended**:
   - [user-implemented `read()` should now return `ReadResult`](#readresult)
   - [user-implemented `read()`/`write()` should now receive `Tree` and id](#treeid)
   - [do not use `.load()` (former `>>`) inside `read()`](#noloadinread)

Be sure to also read through the quickstart for the full overview on how the new features work. Hint to see what changed: run `git diff v0.15.2...v0.16.0 samples/quickstart.cpp` on an up-to-date repo clone (and if the tag v0.16.0 does not exist on the date you try this command, just omit that tag -- it was not yet released).


<a>notequal</a>
### Do not use operators |= or =

For tree building methods `.to_*()` and `operator|=` and `operator=` were deprecated in favour of the newly-added `.set_*()` methods:

```c++
c4::yml::Tree tree = ...;
c4::yml::id_type node_id = ...;
c4::yml::NodeRef node = ...;

// before (deprecated)
node |= MAP|BLOCK;
node["key"] = "val";
node["key"] |= VAL_SQUO;
node["seq"] |= SEQ|FLOW;
node["seq2"] |= SEQ;
node.append_child().to_seq("map");

// now, same as above:
node.set_map(BLOCK);
node["key"].set_val("val", VAL_SQUO);
node["seq"].set_seq(FLOW);
node["seq2"].set_seq();
c4::yml::NodeRef child = node.append_child();
child.set_seq("map");

// or now, same as above, but from the tree:
// more code, but also more efficient
tree.set_map(node_id, BLOCK);
c4::yml::id_type id_key  = tree.append_child(node_id),
                 id_seq  = tree.append_child(node_id),
                 id_seq2 = tree.append_child(node_id),
                 id_map  = tree.append_child(node_id);
tree.set_key(id_key, "key");
tree.set_val(id_key, "val", VAL_SQUO);
tree.set_key(id_seq, "seq");
tree.set_seq(id_seq, FLOW);
tree.set_key(id_seq2, "seq2");
tree.set_seq(id_seq2);
tree.set_key(id_map, "map");
tree.set_map(id_map);
```

If you prefer to keep on using the legacy operators `=`, `|=`, `<<`, `>>`, you can enable the cmake symbol (or define the macro) `RYML_WITH_LEGACY_OPERATORS`.


<a>notshift</a>
### Do not use operators << or >>

For serialization, `operator<<` and `operator>>` were deprecated in favour of `.set_serialized()` and `.deserialize()` (which introduce return on error) or  `.save()` and `.load()` (which trigger error callback on error):
```c++
c4::yml::Tree tree = ...;
c4::yml::id_type node_id = ...;
c4::yml::NodeRef node = ...;
const T var1 = ...;
T var2 = ...;

// before (deprecated)
node << var1;
node >> var2;
node << key(var1);
node >> key(var2);

// now - save() and load() do checks and call error on failure
node.save(var1);
node.load(&var2);
node.save_key(var1);
node.load_key(&var2);
// or now, without exceptional flow: use .set_serialized() and .deserialize()
node.set_serialized(var1);
if( ! node.deserialize(&var2))
  ...
node.set_key_serialized(var1);
if( ! node.deserialize_key(&var2))
  ...

// also now possible from tree!
tree.save(node_id, var1);
tree.load(node_id, &var2);
tree.save_key(node_id, var1);
tree.load_key(node_id, &var2);
tree.set_serialized(node_id, var1);
if( ! tree.deserialize(node_id, &var2))
  ...
tree.set_key_serialized(node_id, var1);
if( ! tree.deserialize_key(node_id, &var2))
  ...
```

If you prefer to keep on using the legacy operators `=`, `|=`, `<<`, `>>`, you can enable the cmake variable (or define the macro) `RYML_WITH_LEGACY_OPERATORS`.

The rest of the changes are optional, **but advised, and recommended**. If you don't do these changes, your code will go on working as before, but the new features and improved behavior will not be available.


<a>readresult</a>
### Optional: user-implemented `read()` should now return `ReadResult`

The **first optional change** is that your `read()` functions should now return a `c4::yml::ReadResult`. This enables reporting the exact node on which a deserialization error happens, even if it is nested deep in the tree, which in turn helps tremendously pinpointing troubles in YAML source (especially with location tracking):
```c++
// optional: your implementation of read() should be changed from...
bool read(c4::yml::ConstNodeRef const& node, T *var)
{
    bool success = ...;
    return success;
}
// optional: change to this:
c4::yml::ReadResult read(c4::yml::ConstNodeRef const& node, T *var)
{
    bool success = ...;
    return c4::yml::ReadResult(success, node.id());
}
```
If you don't change, rapidyaml will still report the error, but it will be report the node of the outer-most function returning false, instead of the node at the inner-most function.


<a>treeid</a>
### Optional: user-implemented `read()`/`write()` should now receive `Tree` and id

The **second optional change** is that the `read()` and `write()` implementations should now be rewritten for the `Tree`, instead of the `NodeRef`. This will enable using your `read()`/`write()` functions **with both the `Tree` and `NodeRef` methods**:
```c++
// optional: your implementation of write()/read() should be changed from...
void write(c4::yml::NodeRef *node, T const& var)
{
    ...
}
c4::yml::ReadResult read(c4::yml::ConstNodeRef const& node, T *var)
{
    bool success = ...;
    return c4::yml::ReadResult(success, node.id());
}

// optional: change to this to enable serialization from/to both Tree and NodeRef:
void write(c4::yml::Tree* tree, c4::yml::id_type id, T const& var)
{
    ...
}
c4::yml::ReadResult read(c4::yml::Tree const* tree, c4::yml::id_type id, T *var)
{
    bool success = ...;
    return c4::yml::ReadResult(success, id);
}
```
If you don't change, you will not be able to serialize to/from `Tree`. That is, **having only the node implementation will not work with the `Tree` methods**. You can also provide both functions, and they would be picked up correctly, but that is pointless. In short, **prefer the Tree** version.

Note also that **inside the tree version, you can still use the node API**. For example:
```c++
c4::yml::ReadResult read(c4::yml::Tree const* tree, c4::yml::id_type id, T *var)
{
    c4::yml::ConstNodeRef n(tree, id);
    ... // this way you can keep the function body as before
        // BUT read next note
}
```


<a>noloadinread</a>
### Optional: do not use `.load()` (former `>>`) inside `read()`

The **last optional change** is that inside your `write()/read()` implementation **you should now use the new `.deserialize()` calls**. These methods will play nice when they are called from `.deserialize()` on upper-level objects. On the contrary, `.load()` (formerly `>>`) will interrupt execution immediately, and this will prevent those upper-level calls functions from successfully receiving the actual `ReadResult` and reporting it upwards.

That is, if you want to be able to write code such as
```c++
struct T { int foo, bar; };
T var;
if( ! node.deserialize(&var))
    ...
```
... then your `read(tree,id,T)` implementation should not use `.load()`:
```c++
c4::yml::ReadResult read(c4::yml::Tree const* tree, c4::yml::id_type id, T *var)
{
    c4::yml::id_type id_foo = ...;
    c4::yml::id_type id_bar = ...;
    //tree->load(id_foo, &var->bar); // wrong: calls error_visit on failure!
    //tree->load(id_bar, &var->bar); // wrong: calls error_visit on failure!
    // do this instead:
    c4::yml::ReadResult result;
    result = tree->deserialize(id_foo, &var->foo); // ok, enables return on failure, reported upwards
    if(result) // note the result will report the first detected error!
      result = tree->deserialize(id_bar, &var->bar); // ok, enables return on failure, reported upwards
    return result;
}
```

Note that this `read()` implementation will still enable a calling `.load()` to work as intended.


<a>v0.16changelogsection</a>
## Full changelog

- [PR#639](https://github.com/biojppm/rapidyaml/pull/639) fix names using leading/double underscore (renames only, no logic changes).
- [PR#638](https://github.com/biojppm/rapidyaml/pull/638) improve quickstart-ints.
- [PR#637](https://github.com/biojppm/rapidyaml/pull/637) add cmake option `RYML_SYSTEM_C4CORE` to consume c4core from `find_package()`. Thanks @uilianries!
- [PR#636](https://github.com/biojppm/rapidyaml/pull/636) **Remove c4core submodule**, and copy c4core files to rapidyaml (and manage sync):
  - `ext/c4core.src/`: c4core code used in the rapidyaml library 
  - `ext/c4core.dev/`: c4core code used by tests/benchmarks
  - `proj/`: project files like cmake files and toolchains
  - `RYML_STANDALONE` can still be set to `OFF` to compile against an out-of-source c4core (see also the newly added `RYML_SYSTEM_C4CORE`)
  - the amalgamation tool now adds only the files in `c4core.src` files, and has new option `--c4core-dev` to include the `c4core.dev` files
  - adds (internal) tools to manage c4core synchronization scenarios. See [ext/README.md](https://github.com/biojppm/rapidyaml/tree/master/ext) for details with this.
- [PR#635](https://github.com/biojppm/rapidyaml/pull/635) API cleanup: `NodeType` and `extra::ievt::EventFlags`:
  - On `Tree` and `NodeRef`, overloads taking `NodeType_e` are now receiving `type_bits`.
  - This saves operator calls on type queries.
  - Remove bitwise operators, no longer needed.
  - Rename `NodeType_e` to `NodeTypeBits` (deprecate `NodeType_e`)
  - Low impact, no changes should be needed on user code, other than renaming unlikely uses of `NodeType_e`.
  - Also, for int events: rename `extra::ievt::EventFlags` to `extra::ievt::EventBits`
- [PR#620](https://github.com/biojppm/rapidyaml/pull/620) API cleanup: `Tree` and `NodeRef`:
  - Deprecate `NodeInit`
  - `Tree` and `NodeRef`:
    - deprecate `.to_val()` and friends -- add `.set_val()` and friends.
    - deprecate `operator=(csubstr)` and friends -- use `.set_val()` instead.
    - deprecate `operator|=(NodeType)` and `operator=(NodeType)` -- use appropriate overload `.set_*(T, NodeType)`.
    - You can disable compiler deprecation warnings from use of these operators: by enabling the cmake variable (or defining the macro) `RYML_WITH_LEGACY_OPERATORS`.
    - deprecate `NodeInit` and `NodeScalar` methods in `Tree` and `NodeRef` (use `.set_*()`)
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
    - If you don't want to migrate yet, use `RYML_WITH_LEGACY_OPERATORS` to disable compiler deprecation warnings from use of these operators.
    - Serialization with `Tree` API is now fully implemented, working exactly the same as `NodeRef`
      - serialization:
        - `NodeRef` serialized with `write(NodeRef *, T const&)` and `write_key()`
        - `Tree` serialized with `write(Tree *, id_type, T const&)` and `write_key()`
        - No changes in types using `to_chars()` serialization
      - deserialization:
        - `ConstNodeRef` deserialized with `read(ConstNodeRef const&, T *)` and `read_key()`
        - `Tree` deserialized with `read(Tree const*, id_type, T *)` and `read_key()` (removed old `readkey()` approach.)
        - No changes in types using `from_chars()` serialization
      - Enables bypass of `NodeRef` serialization to use the tree API, which is faster to compile.
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
- [PR#626](https://github.com/biojppm/rapidyaml/pull/626): ensure accurate reporting of nodes causing deeply nested serialization error. This enables reporting the inner-most node causing the error! To profit from this, `read()` user functions should now return `ReadResult` instead of `bool`. This change is backwards-compatible with legacy `read()` functions returning `bool`, but the legacy version will remain less precise: it will report an inner error on the upmost node returning false, possibly the root if all levels are returning bool. See the new `sample_deserialize_error()` on the quickstart, and the [latest doxygen documentation](https://rapidyaml.readthedocs.io/latest/doxygen/group__doc__serialization__user__types.html). The user functions should change (but not mandatory):
  ```c++
  // before
  bool read(ryml::ConstNodeRef n, T *var) { ... }
  // now (should be changed, but not mandatory)
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
  - `Tree` and `NodeRef`: deprecate `.type_str()`. Use `.type().type_str()`
  - Tools: add ints parsing to ryml-emit-parse
- [PR#634](https://github.com/biojppm/rapidyaml/pull/634): int events: add sample containing ints-only library
- [PR#590](https://github.com/biojppm/rapidyaml/pull/590): minor: prefer calling some predicates from `NodeType`


## Thanks

- @uilianries
