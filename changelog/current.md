### Improvements

- [PR#503](https://github.com/biojppm/rapidyaml/pull/503) (fixes [#399](https://github.com/biojppm/rapidyaml/issues/399)): change error callbacks.
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
  - See the new header `c4/yml/error.hpp` (and `c4/yml/error.def.hpp` for definitions of the functions in `c4/yml/error.hpp`)
  - See the relevant sample functions in the quickstart sample: `sample_error_basic`, `sample_error_parse` and `sample_error_visit`.
  - There are breaking user-facing changes in the `Callbacks` structure:
    - Removed member `m_error `
    - Added members `m_error_basic`, `m_error_parse`, `m_error_visit`
    - Added methods `.set_error_basic()`, `.set_error_parse()` and `.set_error_visit()`.


### Fixes

- Fix [#400](https://github.com/biojppm/rapidyaml/issues/400) ([PR#506](https://github.com/biojppm/rapidyaml/pull/506)): clear anchors after resolving.
- Fix [#484](https://github.com/biojppm/rapidyaml/issues/484) ([PR#506](https://github.com/biojppm/rapidyaml/pull/506)): fix merge key order for last element to be overriden.
- [PR#504](https://github.com/biojppm/rapidyaml/pull/504): ensure parse error on `a: b: c` and similar cases containing nested maps opening on the same line.
- [PR#502](https://github.com/biojppm/rapidyaml/pull/502): fix parse errors or missing tags:
  - missing tags in empty documents:
    ```yaml
    !yamlscript/v0/bare
    --- !code
    42
    ```
  - trailing empty keys or vals:
    ```yaml
    a:
       :
    ```
  - missing tags in trailing empty keys or vals:
    ```yaml
    a: !tag
      !tag : !tag
    ```
  - missing tags in complex maps:
    ```yaml
    ? a: !tag
      !tag : !tag
    :
      !tag : !tag
    ```
- [PR#501](https://github.com/biojppm/rapidyaml/pull/501): fix missing tag in `- !!seq []`.
- [PR#508](https://github.com/biojppm/rapidyaml/pull/508): fix build with cmake 4.
