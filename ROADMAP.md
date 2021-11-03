# Roadmap

Roughly in order of priority:

  * Cleanup:
    * Review & cleanup API surface.
    * Review `parse()` API: add suffixes `_in_situ` and `_in_arena` to clarify
      intent. Ie:
      * rename `parse(substr)` to `parse_in_situ(substr)`
      * rename `parse(csubstr)` to `parse_in_arena(csubstr)`
    * turn calls to `C4_ASSERT()` into calls to `RYML_ASSERT()`
    * use `c4::MemoryResource` in place of `c4::yml::MemoryResource`, and
      remove `c4::yml::MemoryResource`
    * same for allocators and error callbacks: Use the facilities from c4core.
    * use `csubstr` instead of `csubstr const&` in return and parameter types, but
      quantify the performance effect.
  * Add emit formatting controls:
    * add single-line flow formatter
    * add multi-line flow formatters
      * indenting
      * non indenting
    * keep current block formatter
    * add customizable linebreak limits (number of columns) to every formatter
    * add per node format flags
    * (lesser priority) add auto formatter using reasonable heuristics to
      switch between other existing formatters
  * Optionally preserve Location of nodes in the original YAML source
  * Investigate possibility of comment-preserving roundtrips
