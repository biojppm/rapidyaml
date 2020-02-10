# Roadmap

Roughly in order of priority:

  * Cleanup:
    * turn calls to `C4_ASSERT()` into calls to `RYML_ASSERT()`
    * use `c4::MemoryResource` in place of `c4::yml::MemoryResource`, and
      remove `c4::yml::MemoryResource`
  * Review `parse()` API: add suffixes `_in_situ` and `_in_arena` to clarify
    intent. Ie:
    * rename `parse(substr)` to `parse_in_situ(substr)`
    * rename `parse(csubstr)` to `parse_in_arena(csubstr)`
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
  * Increase YAML test suite coverage
  * use `csubstr` instead `csubstr const&` in return and parameter types, but
    quantify effect in runtimes.
