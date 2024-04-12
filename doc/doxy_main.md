# rapidyaml

* Begin by looking at the [project's README](https://github.com/biojppm/rapidyaml/blob/v0.5.0/README.md)
* [Documentation page](https://rapidyaml.readthedocs.org)
* Next, skim the docs for the @ref doc_quickstart sample.
* Good! Now the main ryml topics:
  * @ref doc_parse - how to parse YAML/JSON into a tree
  * @ref doc_emit - how to emit YAML/JSON from an existing tree
  * How to visit and create ryml trees:
    * @ref doc_node_type
    * @ref doc_tree
    * @ref doc_node_classes
  * For serialization/deserialization:
    * See @ref sample_scalar_types for when the type is scalar (a leaf node in the YAML tree, containing a string representation): 
      * See examples on how to @ref sample_to_chars_scalar
      * See examples on how to @ref sample_from_chars_scalar
      * See the sample @ref sample::sample_user_scalar_types
      * When serializing floating point values in C++ earlier than 17,
        be aware that there may be a truncation of the precision with
        the default to_chars implementation. To enforce a particular
        precision, use for example @ref c4::fmt::real, or call
        directly @ref c4::ftoa or @ref c4::dtoa, or any other method
        (remember that ryml only stores the final string, so nothing
        prevents you from creating it). See the relevant sample: @ref
        sample::sample_float_precision.
    * See @ref sample_container_types for when the type is a container (ie, a node which has children, which may themselves be containers).
      * See the sample @ref sample::sample_user_container_types
    * ryml does not use any STL containers internally, but it can be
      used to serialize and deserialize these containers. See @ref
      sample::sample_std_types for an example. See the header @ref
      ryml_std.hpp and also the headers it includes.
  * @ref doc_tag_utils - how to resolve tags
  * @ref doc_callbacks - how to set up error/allocation/deallocation
    callbacks either globally for the library, or for specific objects
    such as @ref c4::yml::Tree or @ref c4::yml::Parser 
* rapidyaml uses these facilities from [c4core](https://github.com/biojppm/c4core), so their documentation
  is also shown here:
  * @ref doc_substr
  * @ref doc_charconv
  * @ref doc_format_utils
  * @ref doc_base64
  
You should also be aware that the @ref doc_charconv facilities in
[c4core](https://github.com/biojppm/c4core) are extremely fast, and
generally outperform the fastest equivalent facilities in the standard
library by a significant margin; refer to the documentation of @ref
doc_charconv for further details. For example:

<table>
<caption id="xtoa-i64">xtoa,int64_t</caption>
<tr><th>g++12, linux <th>Visual Studio 2019
<tr><td> \image html linux-x86_64-gxx12.1-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png <td> \image html windows-x86_64-vs2019-Release-c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png
</table>


