import re
import os
from os.path import abspath, dirname
import sys
import subprocess
import argparse


projdir = abspath(dirname(dirname(__file__)))
sys.path.insert(0, f"{projdir}/ext/c4core/cmake")
import amalgamate_utils as am
sys.path.insert(0, f"{projdir}/ext/c4core/tools")
import amalgamate as am_c4core

ryml_defmacro = "RYML_SINGLE_HDR_DEFINE_NOW"
c4core_defmacro = "C4CORE_SINGLE_HDR_DEFINE_NOW"
exports_def_code = f""" // shared library: export when defining
#if defined(RYML_SHARED) && defined({ryml_defmacro}) && !defined(RYML_EXPORTS)
#define RYML_EXPORTS
#endif
"""
c4core_def_code = f""" // propagate defines to c4core
#if defined({ryml_defmacro}) && !defined({c4core_defmacro})
#define {c4core_defmacro}
#endif

#if defined(RYML_EXPORTS) && !defined(C4CORE_EXPORTS)
#define C4CORE_EXPORTS
#endif

#if defined(RYML_SHARED) && !defined(C4CORE_SHARED)
#define C4CORE_SHARED
#endif

// workaround for include removal while amalgamating
// resulting in <stdarg.h> missing in arm-none-eabi-g++
// https://github.com/biojppm/rapidyaml/issues/193
#include <stdarg.h>
"""


def amalgamate_ryml(filename: str,
                    with_c4core: bool,
                    with_fastfloat: bool,
                    with_stl: bool):
    c4core_amalgamated = ""
    if with_c4core:
        c4core_amalgamated = "src/c4/c4core_all.hpp"
        am_c4core.amalgamate_c4core(f"{projdir}/{c4core_amalgamated}",
                                    with_fastfloat=with_fastfloat,
                                    with_stl=with_stl)
    repo = "https://github.com/biojppm/rapidyaml"
    ryml_preamble = f"""
Rapid YAML - a library to parse and emit YAML, and do it fast.

{repo}

DO NOT EDIT. This file is generated automatically.
This is an amalgamated single-header version of the library.

INSTRUCTIONS:

  - Include at will in any header of your project. Because the
    amalgamated header file is large, to speed up compilation of
    your project, protect the include with its include guard
    `_RYML_SINGLE_HEADER_AMALGAMATED_HPP_`, ie like this:
    ```
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
    #include <ryml_all.hpp>
    #endif
    ```

  - In one (and only one) of your project source files, #define
    {ryml_defmacro} and then include this header. This will enable
    the function and class definitions in the header file.

  - To compile into a shared library, define the preprocessor symbol
    RYML_SHARED before including the header. This will take care of
    symbol export/import.

"""
    srcfiles = [
        am.cmttext(ryml_preamble),
        am.cmtfile("LICENSE.txt"),
        am.injcode(exports_def_code),
        am.onlyif(with_c4core, am.injcode(c4core_def_code)),
        am.onlyif(with_c4core, c4core_amalgamated),
        "src/c4/yml/export.hpp",
        "src/c4/yml/fwd.hpp",
        "src/c4/yml/version.hpp",
        "src/c4/yml/common.hpp",
        "src/c4/yml/node_type.hpp",
        "src/c4/yml/tag.hpp",
        "src/c4/yml/tree.hpp",
        "src/c4/yml/node.hpp",
        "src/c4/yml/writer.hpp",
        "src/c4/yml/detail/parser_dbg.hpp",
        am.injcode("#define C4_YML_EMIT_DEF_HPP_"),
        "src/c4/yml/emit.hpp",
        "src/c4/yml/emit.def.hpp",
        "src/c4/yml/detail/stack.hpp",
        "src/c4/yml/filter_processor.hpp",
        "src/c4/yml/parser_state.hpp",
        "src/c4/yml/event_handler_stack.hpp",
        "src/c4/yml/event_handler_tree.hpp",
        "src/c4/yml/parse_engine.hpp",
        "src/c4/yml/preprocess.hpp",
        "src/c4/yml/reference_resolver.hpp",
        "src/c4/yml/parse.hpp",
        am.onlyif(with_stl, "src/c4/yml/std/map.hpp"),
        am.onlyif(with_stl, "src/c4/yml/std/string.hpp"),
        am.onlyif(with_stl, "src/c4/yml/std/vector.hpp"),
        am.onlyif(with_stl, "src/c4/yml/std/std.hpp"),
        "src/c4/yml/version.cpp",
        "src/c4/yml/common.cpp",
        "src/c4/yml/node_type.cpp",
        "src/c4/yml/tag.cpp",
        "src/c4/yml/tree.cpp",
        "src/c4/yml/parse_engine.def.hpp",
        "src/c4/yml/reference_resolver.cpp",
        "src/c4/yml/parse.cpp",
        "src/c4/yml/node.cpp",
        "src/c4/yml/preprocess.cpp",
        "src/c4/yml/detail/checks.hpp",
        "src/c4/yml/detail/print.hpp",
        "src/c4/yml/yml.hpp",
        "src/ryml.hpp",
    ]
    result = am.catfiles(srcfiles,
                         projdir,
                         # comment out lines with these patterns:
                         include_regexes=[
                             re.compile(r'^\s*#\s*include "(c4/yml/.*)".*$'),
                             re.compile(r'^\s*#\s*include <(c4/yml/.*)>.*$'),
                             re.compile(r'^\s*#\s*include "(c4/.*)".*$'),
                             re.compile(r'^\s*#\s*include <(c4/.*)>.*$'),
                         ],
                         definition_macro=ryml_defmacro,
                         repo=repo,
                         result_incguard="_RYML_SINGLE_HEADER_AMALGAMATED_HPP_")
    result_with_only_first_includes = am.include_only_first(result)
    am.file_put_contents(filename, result_with_only_first_includes)


def mkparser():
    return am.mkparser(c4core=(True, "amalgamate c4core together with ryml"),
                       fastfloat=(True, "enable fastfloat library"),
                       stl=(True, "enable stl interop"))


if __name__ == "__main__":
    args = mkparser().parse_args()
    amalgamate_ryml(filename=args.output,
                    with_c4core=args.c4core,
                    with_fastfloat=args.fastfloat,
                    with_stl=args.stl)
