# rapidyaml

* Begin by looking at the [project's README](https://github.com/biojppm/rapidyaml/blob/v0.7.2/README.md)
* [Documentation page](https://rapidyaml.readthedocs.org)
* Next, skim the docs for the @ref doc_quickstart sample.
* Good! Now the main ryml topics:
  * @ref doc_parse - how to parse YAML/JSON into a tree
  * @ref doc_emit - how to emit YAML/JSON from an existing tree
  * How to visit and create ryml trees:
    * @ref doc_node_type
    * @ref doc_tree
    * @ref doc_node_classes
  * For serialization/deserialization, see @ref doc_serialization.
  * @ref doc_ref_utils - how to resolve references to anchors
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


