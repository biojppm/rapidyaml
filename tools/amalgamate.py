import re
import os
from os.path import abspath, dirname
import sys
import subprocess
import argparse
from typing import List
from enum import Enum

projdir = abspath(dirname(dirname(__file__)))
sys.path.insert(0, f"{projdir}/ext/c4core/cmake")
import amalgamate_utils as am
sys.path.insert(0, f"{projdir}/ext/c4core/tools")
import amalgamate as am_c4core


class Event(Enum):
    tree = "tree"
    testsuite = "testsuite"
    ints = "ints"
    ints_utils = "ints_utils"
    ints_to_testsuite = "ints_to_testsuite"
    all = "all"
    none = "none"
    def __str__(self):
        return self.value


event_doc = {
    Event.tree: """(the default) enable the normal ryml event handler
    to create the tree, and additionally the Tree, Node, parser and
    emitter utilities; if this is not enabled, none of these
    components will be included in the amalgamated file""",
    Event.testsuite: "enable the (extra) YAML test suite event handler",
    Event.ints: "enable the (extra) integer-based event handler",
    Event.ints_utils: "enable the (extra) integer-based event handler utils",
    Event.ints_to_testsuite: "enable the (extra) integer events conversion to testsuite events",
    Event.all: "enable all event handlers",
    Event.none: "disable all event handlers",
}


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
                    with_stl: bool,
                    events: List[Event]):
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
    #include <header_file_name.hpp>
    #endif
    ```

  - In one -- and only one -- of your project source files, #define
    {ryml_defmacro} and then include this header. This will enable
    the function and class definitions in the header file.

  - To compile into a shared library, define the preprocessor symbol
    RYML_SHARED before including the header. This will take care of
    symbol export/import.

"""
    def has_evt(*which):
        if Event.all in events:
            return True
        for e in which:
            if e in events:
                return True
        return False
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
        am.onlyif(has_evt(Event.tree), "src/c4/yml/tree.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/node.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/writer.hpp"),
        "src/c4/yml/detail/dbgprint.hpp",
        am.onlyif(has_evt(Event.tree), am.injcode("#define C4_YML_EMIT_DEF_HPP_")),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/emit.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/emit.def.hpp"),
        "src/c4/yml/detail/stack.hpp",
        "src/c4/yml/filter_processor.hpp",
        "src/c4/yml/parser_state.hpp",
        "src/c4/yml/event_handler_stack.hpp",
        am.onlyif(has_evt(Event.tree), "src/c4/yml/event_handler_tree.hpp"),
        am.onlyif(has_evt(Event.ints), "src_extra/c4/yml/extra/event_handler_ints.hpp"),
        am.onlyif(has_evt(Event.ints_utils), "src_extra/c4/yml/extra/ints_utils.hpp"),
        am.onlyif(has_evt(Event.ints_to_testsuite), "src_extra/c4/yml/extra/ints_to_testsuite.hpp"),
        am.onlyif(has_evt(Event.testsuite), "src_extra/c4/yml/extra/string.hpp"),
        am.onlyif(has_evt(Event.testsuite), "src_extra/c4/yml/extra/event_handler_testsuite.hpp"),
        am.onlyif(has_evt(Event.ints_utils, Event.testsuite), "src_extra/c4/yml/extra/scalar.hpp"),
        "src/c4/yml/parse_engine.hpp",
        "src/c4/yml/preprocess.hpp",
        am.onlyif(has_evt(Event.tree), "src/c4/yml/reference_resolver.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/parse.hpp"),
        am.onlyif(with_stl and has_evt(Event.tree), "src/c4/yml/std/map.hpp"),
        am.onlyif(with_stl and has_evt(Event.tree), "src/c4/yml/std/string.hpp"),
        am.onlyif(with_stl and has_evt(Event.tree), "src/c4/yml/std/vector.hpp"),
        am.onlyif(with_stl and has_evt(Event.tree), "src/c4/yml/std/std.hpp"),
        "src/c4/yml/version.cpp",
        "src/c4/yml/common.cpp",
        "src/c4/yml/node_type.cpp",
        "src/c4/yml/tag.cpp",
        "src/c4/yml/parse_engine.def.hpp",
        am.onlyif(has_evt(Event.tree), "src/c4/yml/tree.cpp"),
        am.onlyif(has_evt(Event.ints), "src_extra/c4/yml/extra/event_handler_ints.cpp"),
        am.onlyif(has_evt(Event.ints_utils, Event.testsuite), "src_extra/c4/yml/extra/scalar.cpp"),
        am.onlyif(has_evt(Event.ints_utils), "src_extra/c4/yml/extra/ints_utils.cpp"),
        am.onlyif(has_evt(Event.ints_to_testsuite), "src_extra/c4/yml/extra/ints_to_testsuite.cpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/reference_resolver.cpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/parse.cpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/node.cpp"),
        "src/c4/yml/preprocess.cpp",
        am.onlyif(has_evt(Event.tree), "src/c4/yml/detail/checks.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/detail/print.hpp"),
        am.onlyif(has_evt(Event.tree), "src/c4/yml/yml.hpp"),
        am.onlyif(has_evt(Event.tree), "src/ryml.hpp"),
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
    p = am.mkparser(
        c4core=(True, "amalgamate c4core together with ryml"),
        fastfloat=(True, "enable fastfloat library"),
        stl=(True, "enable stl interop")
    )
    default = [str(Event.tree)]
    evtdoc = '. '.join([f"'{e}': {event_doc[e]}" for e in Event])
    defaultdoc = ','.join([str(e) for e in default])
    p.add_argument('-e', '--events', type=str, default=default, choices=[str(e) for e in Event], nargs="+",
                   help=f"""Specify which event handlers to include. Possible
                   values are: {evtdoc}. The default is {defaultdoc}.""")
    return p


if __name__ == "__main__":
    args = mkparser().parse_args()
    args.events = [Event(e) for e in args.events] # is there a better way to do this?
    amalgamate_ryml(filename=args.output,
                    with_c4core=args.c4core,
                    with_fastfloat=args.fastfloat,
                    with_stl=args.stl,
                    events=args.events)
