### Changes

- [PR#547](https://github.com/biojppm/rapidyaml/pull/547) - Fix parsing of implicit first documents with empty sequences, caused by a problem in `Tree::set_root_as_stream()`:
  ```yaml
  []  # this container was lost during parsing
  ---
  more data here
  ```
- [PR#550](https://github.com/biojppm/rapidyaml/pull/550) - Implement FLOW_ML style (flow multiline).
  - The parser now sets this style automatically for flow seqs or maps when the terminating bracket sits on a line different from the opening bracket.
  - Added `ParserOptions::detect_flow_ml()` to control this behavior
  - Added `EmitOptions::indent_flow_ml()` to control indentation of FLOW_ML containers
  - The emit implementation logic was refactored, and is now significantly cleaner
  - Emitted YAML will now have anchors emitted before tags, as is customary ([see example](https://play.yaml.io/main/parser?input=LSAhdGFnICZhbmNob3IgfAogIG5vdGUgaG93IHRoZSBhbmNob3IgY29tZXMKICBmaXJzdCBpbiB0aGUgZXZlbnRz)).
  - Added `ParserOptions` defaulted argument to temp-parser overloads of `parse_{yaml,json}_in_{place,arena}()`
 [PR#503](https://github.com/biojppm/rapidyaml/pull/503) (fixes [#399](https://github.com/biojppm/rapidyaml/issues/399)): change error callbacks.
  - Errors in ryml now have one of these types:
    - parse error: when parsing YAML/JSON. See: `pfn_error_parse`,  `ErrorDataParse`, `ExceptionParse`, `err_parse_format()`, `sample_error_parse`.
    - visit error: when visiting a tree (reading or writing). See: `pfn_error_visit`,  `ErrorDataVisit`, `ExceptionVisit`, `err_visit_format()`, `sample_error_visit`.
    - basic error: other, non specific errors. See: `pfn_error_basic`,  `ErrorDataBasic`, `ExceptionBasic`, `err_basic_format()`, `sample_error_basic`.
    - parse and visit errors/exceptions can be treated/caught as basic errors/exceptions
  - Add message formatting functions to simplify implementation of error callbacks:
    - `err_parse_format()`: format/print a full error message for a parse error
    - `err_visit_format()`: format/print a full error message for a visit error
    - `err_basic_format()`: format/print a full error message for a basic error
    - `location_format()`: format/print a location
    - `location_format_with_context()`: useful to create a rich error message showing the YAML region causing the error, maybe even for a visit error if the source is kept and locations are enabled.
    - `format_exc()`: when exceptions are enabled
  - See the new header `c4/yml/error.hpp` (and `c4/yml/error.def.hpp` for definitions of the functions in `c4/yml/error.hpp`)
  - See the relevant sample functions in the quickstart sample: `sample_error_basic`, `sample_error_parse` and `sample_error_visit`.
  - There are breaking user-facing changes in the `Callbacks` structure:
    - Removed member `m_error `
    - Added members `m_error_basic`, `m_error_parse`, `m_error_visit`
    - Added methods `.set_error_basic()`, `.set_error_parse()` and `.set_error_visit()`.

