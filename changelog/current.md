- [PR#616](https://github.com/biojppm/rapidyaml/pull/616): Clean emit API **[BREAKING]**
  - `WriterFile` and `WriterOStream` no longer track the number of emitted bytes.
  - `error_on_excess` is now used in the emit-to-buffer overloads, and no longer in the main `Emitter::emit_as()` driver function.
- [PR#617](https://github.com/biojppm/rapidyaml/pull/617): Clean emit API, part 2: tidy emit classes among new header files:
    - Top-level emit headers:
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
- [PR#618](https://github.com/biojppm/rapidyaml/pull/618): Clean emit API, part 3:
  - Improve handling of NaN and Inf in json emitting.
  - Expose scalar style helpers for json emitting:
    ```c++
    /** JSON-sense query of plain number */
    bool scalar_is_plain_number_json(csubstr s);
    /** JSON-sense query of plain number */
    bool scalar_is_special_json(csubstr s);
    /** Query if a scalar is nan (nan, NaN, NAN)
     * @warning length must be 3 (asserted)
     */
    bool scalar_is_inf3(csubstr s);
    /** Query if a scalar is inf (inf, Inf, INF)
     * @warning length must be 3 (asserted)
     */
    bool scalar_is_nan3(csubstr s);
    /** Same as scalar_is_inf3() || scalar_is_nan3()
     * @warning length must be 3 (asserted)
     */
    bool scalar_is_inf_or_nan3(csubstr s);
    ```
  - Writers: add `C4_ALWAYS_INLINE`. Results in ~10-20% emit improvements.
  - `file_put_contents()`: add `FILE*` overloads
- [PR#622](https://github.com/biojppm/rapidyaml/pull/622): Remove preprocess utilities.
- [PR#619](https://github.com/biojppm/rapidyaml/pull/619): Clean `Tree` and `NodeRef`:
  - Deprecate `NodeInit`
  - `Tree` and `NodeRef`:
    - deprecate `.to_val()` and friends -- add `.set_val()` and friends.
    - deprecate `operator|=(NodeType)` and `operator=(NodeType)` -- use appropriate overload `.set_*(T, NodeType)`. For example:
      ```c++
      // before
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
      ```
    - deprecate `NodeInit` and `NodeScalar` methods (use `.set_*()`)
    - deprecate single-arg `NodeRef::{duplicate,move}(ConstNodeRef)`
    - deprecate `NodeRef::visit()` and `NodeRef::visit_stacked()`
    - add `Tree::arena_rem()`
    - add `RYML_DEFAULT_TREE_ARENA_CAPACITY_START` with default value of 256
  - `parse_*()`: internal simplification, no semantic changes
  
