### New features

- [PR#550](https://github.com/biojppm/rapidyaml/pull/550) - Implement flow multiline style (`FLOW_ML`):
  - The parser now detects this style automatically for flow seqs or maps when the terminating bracket sits on a line different from the opening bracket.
  - Added `ParserOptions::detect_flow_ml()` to enable/disable this behavior
  - Added `EmitOptions::indent_flow_ml()` to control indentation of FLOW_ML containers
  - The emit implementation logic was refactored, and is now significantly cleaner
  - Emitted YAML will now have anchors emitted before tags, as is customary ([see example](https://play.yaml.io/main/parser?input=LSAhdGFnICZhbmNob3IgfAogIG5vdGUgaG93IHRoZSBhbmNob3IgY29tZXMKICBmaXJzdCBpbiB0aGUgZXZlbnRz)).
  - Added `ParserOptions` defaulted argument to temp-parser overloads of `parse_{yaml,json}_in_{place,arena}()`


### API changes

- **BREAKING** [PR#503](https://github.com/biojppm/rapidyaml/pull/503) (fixes [#399](https://github.com/biojppm/rapidyaml/issues/399)): change error callbacks.
  - Errors in ryml now have one of these types:
    - parse error: when parsing YAML/JSON. See: `pfn_error_parse`,  `ErrorDataParse`, `ExceptionParse`, `err_parse_format()`, `sample_error_parse`.
    - visit error: when visiting a tree (reading or writing). See: `pfn_error_visit`,  `ErrorDataVisit`, `ExceptionVisit`, `err_visit_format()`, `sample_error_visit`.
    - basic error: other, non specific errors. See: `pfn_error_basic`,  `ErrorDataBasic`, `ExceptionBasic`, `err_basic_format()`, `sample_error_basic`.
    - parse and visit errors/exceptions can be treated/caught as basic errors/exceptions.
  - Add message formatting functions to simplify user-provided implementation of error callbacks:
    - `err_parse_format()`: format/print a full error message for a parse error
    - `err_visit_format()`: format/print a full error message for a visit error
    - `err_basic_format()`: format/print a full error message for a basic error
    - `location_format()`: format/print a location
    - `location_format_with_context()`: useful to create a rich error message showing the YAML region causing the error, maybe even for a visit error if the source is kept and locations are enabled.
    - `format_exc()`: format an exception (when exceptions are enabled)
  - See the new header `c4/yml/error.hpp` (and `c4/yml/error.def.hpp` for definitions of the functions in `c4/yml/error.hpp`)
  - See the relevant sample functions in the quickstart sample: `sample_error_basic`, `sample_error_parse` and `sample_error_visit`.
  - There are breaking user-facing changes in the `Callbacks` structure:
    - Removed member `m_error `
    - Added members `m_error_basic`, `m_error_parse`, `m_error_visit`
    - Added methods `.set_error_basic()`, `.set_error_parse()` and `.set_error_visit()`.
- **BREAKING** [PR#557](https://github.com/biojppm/rapidyaml/pull/557) - `Tree` is now non-empty by default, and `Tree::root_id()` will no longer modify the tree when it is empty. To create an empty tree, it is now necessary to use the capacity constructor with a capacity of zero:
  ```c++
  // breaking change: default-constructed tree is now non-empty
  Tree tree;
  assert(!tree.empty()); // MODIFIED! was empty on previous version
  id_type root = tree.root_id(); // OK. default-constructed tree is now non-empty
  
  // to create an empty tree (as happened before):
  Tree tree(0); // pass capacity of zero
  assert(tree.empty()); // as expected
  // but watchout, this is no longer possible:
  //id_type root = tree.root_id(); // ERROR: cannot get root of empty tree.
  ```
  This changeset also enables the python library to call `root_id()` on a default-constructed tree (fixes [#556](https://github.com/biojppm/rapidyaml/issues/556)).
- [PR#560](https://github.com/biojppm/rapidyaml/pull/560) (see also [#554](https://github.com/biojppm/rapidyaml/issues/554)): python improvements:
  - expose `Tree::to_arena()` in python. This allows safer and easier programatic creation of trees in python by ensuring scalars are placed into the tree and so have the same lifetime as the tree:
  ```python
  t = ryml.Tree()
  s = t.to_arena(temp_string()) # Copy/serialize a temporary string to the tree's arena.
                                # Works also with integers and floats.
  t.to_val(t.root_id(), s)      # Now we can safely use the scalar in the tree:
                                # there is no longer any risk of it being deallocated
  ```
  - improve behavior of `Tree` methods accepting scalars: all standard buffer types are now accepted (ie, `str`, `bytes`, `bytearray` and `memoryview`).
- [PR#565](https://github.com/biojppm/rapidyaml/pull/565) (fixes [#564](https://github.com/biojppm/rapidyaml/issues/564)) - `Tree` arena: allow relocation of zero-length strings when placed at the end (relax assertions triggered in `Tree::_relocated()`)
- [PR#563](https://github.com/biojppm/rapidyaml/pull/563) (fixes [#562](https://github.com/biojppm/rapidyaml/issues/562)) - Fix bug in `NodeRef::cend()`


### Fixes in YAML parsing

- [PR#561](https://github.com/biojppm/rapidyaml/pull/561) (fixes [#559](https://github.com/biojppm/rapidyaml/issues/559)) - Byte Order Mark: account for BOM length when determining block indentation
- [PR#547](https://github.com/biojppm/rapidyaml/pull/547) - Fix parsing of implicit first documents with empty sequences, caused by a problem in `Tree::set_root_as_stream()`:
  ```yaml
  []  # this container was lost during parsing
  ---
  more data here
  ```
