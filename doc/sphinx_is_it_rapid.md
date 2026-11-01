Is it rapid?
============


You bet! rapidyaml is hands down the fastest YAML processor, and among
the fastest JSON processors. But talk is cheap, so this page shows
performance results to substantiate this bold claim.



## Linux, gcc 16

This section shows results using gcc 16.1. First, here are some
performance plots with [an example yaml
file](https://github.com/biojppm/rapidyaml/blob/v0.15.2/bm/cases/travis.yml)
using
[parse](https://github.com/biojppm/rapidyaml/blob/v0.15.2/bm/bm_parse.cpp)
and
[emit](https://github.com/biojppm/rapidyaml/blob/v0.15.2/bm/bm_emit.cpp)
benchmarks, comparing ryml with
[yamlcpp](https://github.com/jbeder/yaml-cpp),
[libyaml](https://github.com/yaml/libyaml) and
[fyaml](https://github.com/pantoniou/libfyaml) (click the image to see
the data, and see the terminology section at the end for info on the
different variants):

<table>
<tr><th>Parse</th><th>Emit</th></tr>
<tr>
<td>

<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>
</td>

<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>
</td>

</tr>
</table>

To prove that this is not coincidence for a particular
file, here are scatter plots across YAML files with different styles
and sizes, where we pick one of the variants above. The ryml variants
are `ryml_yaml_inplace_reuse_reserve` and `ryml_yaml_str_reserve` from
the plots above (again, click the image to see the data):


<table>
<tr><th>Parse</th><th>Emit</th></tr>
<tr>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
</td>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
</td>
</tr>

<tr>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-parse-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>
</td>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/linux-release/ryml-bm-emit-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>
</td>
`</tr>
</table>

In absolute terms, ryml parses at ~200MB/s and emits at ~600MB/s, and is generally 30x (parse) / 150x (emit) faster than yamlcpp, and never less than 10x (parse) / 50x (emit). These are stark numbers, and justify the library name.



## Windows, Visual Studio 2026

Same plots as above (click the image to see the data):

<table>
<tr><th>Parse</th><th>Emit</th></tr>
<tr>
<td>

<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>
</td>

<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-travis.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-travis-mega_bytes_per_second.png?raw=true" width="400" height="314"></a>
</td>

</tr>
</table>


<table>
<tr><th>Parse</th><th>Emit</th></tr>
<tr>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
</td>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-scatter-mega_bytes_per_second-vals.png?raw=true" width="400" height="314"></a>
</td>
</tr>

<tr>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-parse-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>
</td>
<td>
<a href="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-scatter-mega_bytes_per_second.txt"><img src="https://github.com/biojppm/rapidyaml-data/blob/master/bm/results/0.16.0-pre/windows-release/ryml-bm-emit-scatter-mega_bytes_per_second-rel.png?raw=true" width="400" height="314"></a>
</td>
`</tr>
</table>


## JSON performance

ryml is also among the fastest JSON handlers. It may not be the
fastest, but is well ahead of the pack; here are some performance
results. The benchmark code is the same as above, and it is reading a
[compile_commands.json](https://github.com/biojppm/rapidyaml/blob/v0.15.2/bm/cases/compile_commands.json)
file. The json libraries are
[rapidjson](https://github.com/Tencent/rapidjson),
[sajson](https://github.com/chadaustin/sajson),
[jsoncpp](https://github.com/open-source-parsers/jsoncpp) and
[nlohmann](https://github.com/nlohmann/json):

<table>
<tr><th>Parse performance</th><th>Emit performance</th></tr>
<tr>
<td>

| parse               |    MB/s | MB/s(x) |
|:--------------------|--------:|--------:|
| ryml_json           |  908.00 |  40.31x |
| ryml_yaml           |  881.58 |  39.13x |
| libyaml_arena       |  135.06 |   6.00x |
| libfyaml_arena      |  149.15 |   6.62x |
| yamlcpp_arena       |   22.53 |   1.00x |
| rapidjson_arena     |  756.93 |  33.60x |
| rapidjson_inplace   | 1741.62 |  77.31x |
| sajson_arena        |  592.57 |  26.30x |
| sajson_inplace      |  599.41 |  26.61x |
| jsoncpp_arena       |  298.60 |  13.25x |
| nlohmann_arena      |  197.17 |   8.75x |

</td>
<td>

| emit              |    MB/s | MB/s(x) |
|-------------------|--------:|--------:|
| ryml_yaml_str     |  805.03 | 128.36x |
| ryml_json_str     | 1034.93 | 165.01x |
| fyaml_str         |  425.42 |  67.83x |
| yamlcpp           |    6.27 |   1.00x |
| rapidjson         |  751.47 | 119.82x |
| jsoncpp           |  562.65 |  89.71x |
| nlohmann          |  299.40 |  47.74x |

</td>
</tr>
</table>

So ryml beats most json parsers at their own game, with the only
exception of rapidjson out of the tested libraries. (There are
probably other JSON parsers faster than rapidyaml, likely
[simdjson](https://github.com/simdjson/simdjson)). As for emitting,
rapidyaml is hands-down the fastest, exceeding all JSON libraries
tested.  Even parsing full YAML is at ~200MB/s, which is still in the
JSON performance ballpark, albeit at its lower end. This is something
to be proud of, as the YAML specification is much more complex than
JSON: [23449 vs 1969
words](https://www.arp242.net/yaml-config.html#its-pretty-complex).


## Serialization performance

Serialization speed also matters for overall speed. For
(de)serialization ryml uses the charconv
facilities from
[c4core](https://github.com/biojppm/c4core); these functions are
blazing fast, and generally outperform the fastest equivalent
facilities in the standard library by a significant margin. For
example, here are some results for
`c4::xtoa<int64_t>()`:

<table>
<tr><th>gcc 12.1</th><th>Visual Studio 2019</th></tr>
<tr>
<td>

<a href="https://github.com/biojppm/c4core/blob/master/doc/img/linux-x86_64-gxx12.1-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true"><img src="https://github.com/biojppm/c4core/blob/master/doc/img/linux-x86_64-gxx12.1-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true" width="400" height="314"></a>

</td>
<td>

<a href="https://github.com/biojppm/c4core/blob/master/doc/img/windows-x86_64-vs2019-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true"><img src="https://github.com/biojppm/c4core/blob/master/doc/img/windows-x86_64-vs2019-Release/c4core-bm-charconv-xtoa-mega_bytes_per_second-i64.png?raw=true" width="400" height="314"></a>

</td>
</tr>
</table>



## Terminology

rapidyaml offers different ways to parse yaml; here's a legend of the
terms used above:
  - `_inplace` means the source is directly parsed (`parse_in_place()` or `parse_json_in_place()`)
  - `_arena` means the source is first copied to the tree arena and then the copy is parsed (`parse_in_arena()` or `parse_json_in_arena()`)
  - `_reserve` means the target tree or buffer was reserved to a suitable size before parsing / emitting
  - for parsing:
    - `ryml_yaml` means the standard ryml tree parser in YAML mode,
      ie by calling `parse_in_arena()` or `parse_in_place()`
    - `ryml_json` means the standard ryml tree parser in JSON mode,
      ie by calling `parse_json_in_arena()` or `parse_json_in_place()`
    - `ryml_ints` means the integer events parser
    - `_reuse` means the parser or emitter are reused during each benchmark; otherwise they are newly created and destroyed on each benchmark iteration
    - `_nofilter` means scalar filtering was disabled during parsing (see `ParseOptions`)
  - for emitting:
    - `ryml_yaml` means the standard ryml tree emitter in YAML mode, ie by calling `emit_yaml()` or `emitrs_yaml()`
    - `ryml_json` means the standard ryml tree emitter in YAML mode, ie by calling `emit_yaml()` or `emitrs_json()`
    - `_str` means emit to string
    - `_file` means emit to `FILE*`
    - `_str_file` means emit first to string, then to `FILE*` (40% faster than `_file` because it results in less system calls)
    - `_ostream` means emit to `std::stringstream`
    - `_ofstream` means emit to `std::ofstream`
