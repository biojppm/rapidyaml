import re
import os
from os.path import abspath, dirname
import sys
import subprocess
import argparse
from typing import List
from enum import Enum

projdir = abspath(dirname(dirname(__file__)))
sys.path.insert(0, f"{projdir}/proj/c4proj")
import amalgamate_utils as am


# -----------------------------------------------------------------------------

class AmalgamateType(Enum):
    single_hdr = "single_hdr"
    single_src = "single_src"
    def __str__(self):
        return self.value


amalgamate_type_doc = {
    AmalgamateType.single_hdr: """amalgamate everything (all
    .cpp+.hpp) files into a single .hpp file""",
    AmalgamateType.single_src: """amalgamate all .cpp files into a
    single .cpp file, and all .hpp files into a single .hpp file"""
}


# -----------------------------------------------------------------------------

class Event(Enum):
    tree = "tree"
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
    Event.ints: "enable the (extra) integer-based event handler",
    Event.ints_utils: "enable the (extra) integer-based event handler utils",
    Event.ints_to_testsuite: "enable the (extra) integer events conversion to testsuite events",
    Event.all: "enable all event handlers",
    Event.none: "disable all event handlers",
}


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

singlehdr_guard_macro = "RYML_SINGLE_HEADER_AMALGAMATED_HPP_"
singlesrc_guard_macro = "RYML_AMALGAMATED_HPP_"

singlehdr_ryml_defmacro = "RYML_SINGLE_HDR_DEFINE_NOW"
singlehdr_c4core_defmacro = "C4CORE_SINGLE_HDR_DEFINE_NOW"

singlehdr_instructions = f"""
This is an amalgamated single-header version of the library.

INSTRUCTIONS:

  - Copy this file to you project.

  - Include at will in any header of your project. Because the
    amalgamated header file is large, to speed up compilation of
    your project, protect the include with its include guard
    `{singlehdr_guard_macro}`, ie like this:
    ```
    #ifndef {singlehdr_guard_macro}
    #include <header_file_name.hpp>
    #endif
    ```

  - In one -- and only one -- of your project source files, #define
    {singlehdr_ryml_defmacro} and then include this header before any
    other include. This will enable the function and class definitions
    in the header file.

  - To compile into a shared library, define the preprocessor symbol
    RYML_SHARED before including the header. This will take care of
    symbol export/import.
"""
singlesrc_instructions_hdr = f"""
This is an amalgamated header containing all library headers, and
accompanies an amalgamated source file.

INSTRUCTIONS:

  - Copy and add these two files to your project.

  - Include at will in any header of your project. Because the
    amalgamated header file is large, to speed up compilation of
    your project, protect the include with its include guard
    `{singlesrc_guard_macro}`, ie like this:
    ```
    #ifndef {singlesrc_guard_macro}
    #include <header_file_name.hpp>
    #endif
    ```

  - To compile into a shared library, define the preprocessor symbols
    RYML_SHARED and RYML_EXPORTS before including the header. This
    will take care of symbol export.

  - To use as a shared library, define the preprocessor symbol
    RYML_SHARED (but do not define RYML_EXPORTS). This will take care
    of symbol import.
"""
singlesrc_instructions_src = """
This is an amalgamated source file containing all library sources, and
accompanies an amalgamated header file.

INSTRUCTIONS:

  - Simply add this and the header file to your project.
"""


# -----------------------------------------------------------------------------
# exports

singlehdr_ryml_def_code = f"""// shared library: export when defining
#if defined(RYML_SHARED) && defined({singlehdr_ryml_defmacro}) && !defined(RYML_EXPORTS)
#define RYML_EXPORTS
#endif
"""
singlesrc_ryml_def_code = ""


singlehdr_c4core_def_code = f""" // propagate defines to c4core
#if defined({singlehdr_ryml_defmacro}) && !defined({singlehdr_c4core_defmacro})
#define {singlehdr_c4core_defmacro}
#endif

#if defined(RYML_SHARED) && !defined(C4CORE_SHARED)
#define C4CORE_SHARED
#endif

#if defined(RYML_EXPORTS) && !defined(C4CORE_EXPORTS)
#define C4CORE_EXPORTS
#endif
"""
singlesrc_c4core_def_code = f""" // propagate defines to c4core
#if defined(RYML_SHARED) && !defined(C4CORE_SHARED)
#define C4CORE_SHARED
#endif

#if defined(RYML_EXPORTS) && !defined(C4CORE_EXPORTS)
#define C4CORE_EXPORTS
#endif
"""


# -----------------------------------------------------------------------------

required_includes = """
//-----------------------------------------------------------------------------
// (amalgamate) workarounds for include removal

// <stdarg.h> missing in arm-none-eabi-g++
// https://github.com/biojppm/rapidyaml/issues/193
#include <stdarg.h>

// conditional includes in the gcc4.8 shim
#include <cstdint>
#include <type_traits>
#include <cstring>

// conditional includes in charconv.hpp
#if (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)) || (__cplusplus >= 201703L)
#include <charconv>
#endif

"""


# -----------------------------------------------------------------------------

def c4core_filelist(with_c4core: bool,
                    with_c4core_dev: bool,
                    with_fastfloat: bool,
                    with_stl: bool):
    hdrfiles = []
    srcfiles = []
    if with_c4core:
        hdrfiles += [
            "ext/c4core.src/c4/export.hpp",
            "ext/c4core.src/c4/version.hpp",
            "ext/c4core.src/c4/preprocessor.hpp",
            "ext/c4core.src/c4/platform.hpp",
            "ext/c4core.src/c4/cpu.hpp",
            "ext/c4core.src/c4/gcc-4.8.hpp",
            "ext/c4core.src/c4/compiler.hpp",
            "ext/c4core.src/c4/language.hpp",
            "ext/c4core.src/c4/types.hpp",
            "ext/c4core.src/c4/config.hpp",
            am.hdrfile("ext/c4core.src/c4/ext/debugbreak/debugbreak.h",
                       "c4/ext/debugbreak/debugbreak.h",
                       "DEBUG_BREAK_H"),
            "ext/c4core.src/c4/error.hpp",
            "ext/c4core.src/c4/memory_util.hpp",
            "ext/c4core.src/c4/blob.hpp",
            "ext/c4core.src/c4/substr_fwd.hpp",
            "ext/c4core.src/c4/substr.hpp",
            am.onlyif(with_fastfloat, am.injfile("ext/c4core.src/c4/ext/fast_float_all.h",
                                                 "c4/ext/fast_float_all.h")),
            am.onlyif(with_fastfloat, "ext/c4core.src/c4/ext/fast_float.hpp"),
            "ext/c4core.src/c4/std/vector_fwd.hpp",
            "ext/c4core.src/c4/std/span_fwd.hpp",
            "ext/c4core.src/c4/std/string_fwd.hpp",
            "ext/c4core.src/c4/std/string_view_fwd.hpp",
            "ext/c4core.src/c4/std/std_fwd.hpp",
            "ext/c4core.src/c4/charconv.hpp",
            "ext/c4core.src/c4/utf.hpp",
            "ext/c4core.src/c4/format.hpp",
            "ext/c4core.src/c4/dump.hpp",
            "ext/c4core.src/c4/base64.hpp",
            "ext/c4core.src/c4/format_base64.hpp",
            am.onlyif(with_stl, am.ignfile("ext/c4core.src/c4/std/std.hpp")), # this is an umbrella include
            am.onlyif(with_stl, "ext/c4core.src/c4/std/span.hpp"),
            am.onlyif(with_stl, "ext/c4core.src/c4/std/string.hpp"),
            am.onlyif(with_stl, "ext/c4core.src/c4/std/string_view.hpp"),
            am.onlyif(with_stl, "ext/c4core.src/c4/std/vector.hpp"),
            am.onlyif(with_stl, "ext/c4core.src/c4/std/std.hpp"),
            am.ignfile("ext/c4core.src/c4/common.hpp"),
        ]
        srcfiles += [
            "ext/c4core.src/c4/version.cpp",
            "ext/c4core.src/c4/language.cpp",
            "ext/c4core.src/c4/memory_util.cpp",
            "ext/c4core.src/c4/format.cpp",
            "ext/c4core.src/c4/utf.cpp",
            "ext/c4core.src/c4/base64.cpp",
            am.injcode("#define C4_WINDOWS_POP_HPP_"),
            "ext/c4core.src/c4/windows_push.hpp",
            "ext/c4core.src/c4/windows.hpp",
            "ext/c4core.src/c4/windows_pop.hpp", # do NOT include this before windows.hpp
            "ext/c4core.src/c4/error.cpp",
        ]
    if with_c4core_dev:
        hdrfiles += [
            "ext/c4core.dev/c4/alloc.hpp",
            "ext/c4core.dev/c4/memory_resource.hpp",
            "ext/c4core.dev/c4/szconv.hpp",
            "ext/c4core.dev/c4/enum.hpp",
            "ext/c4core.dev/c4/bitmask.hpp",
            "ext/c4core.dev/c4/span.hpp",
            am.ignfile("ext/c4core.dev/c4/c4_push.hpp"),
            am.ignfile("ext/c4core.dev/c4/c4_pop.hpp"),
            am.ignfile("ext/c4core.dev/c4/restrict.hpp"),
            am.ignfile("ext/c4core.dev/c4/unrestrict.hpp"),
        ]
        srcfiles += [
            "ext/c4core.dev/c4/alloc.cpp",
            "ext/c4core.dev/c4/memory_resource.cpp",
        ]
    return hdrfiles, srcfiles


# -----------------------------------------------------------------------------

def ryml_filelist(with_stl: bool,
                  events: List[Event],
                  with_engine_def_on_hdr: bool):
    def has(*which):
        if Event.all in events:
            return True
        for e in which:
            if e in events:
                return True
        return False
    hdrfiles = []
    srcfiles = []
    hdrfiles += [
        "src/c4/yml/export.hpp",
        "src/c4/yml/fwd.hpp",
        "src/c4/yml/version.hpp",
        "src/c4/yml/common.hpp",
        "src/c4/yml/error.hpp",
        "src/c4/yml/error.def.hpp",
        "src/c4/yml/escape_scalar.hpp",
        "src/c4/yml/node_type.hpp",
        "src/c4/yml/parse_options.hpp",
        "src/c4/yml/detail/stack.hpp",
        "src/c4/yml/file.hpp",
        "src/c4/yml/tag.hpp",
        am.onlyif(has(Event.tree), "src/c4/yml/scalar_charconv.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/tree.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/node.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/writer_buf.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/writer_file.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/writer_ostream.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/writer.hpp"),
        "src/c4/yml/detail/dbgprint.hpp",
        am.onlyif(has(Event.tree), "src/c4/yml/emit_options.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/scalar_style.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emitter.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emitter.def.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_buf.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_container.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_file.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_ostream.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit.hpp"),
        "src/c4/yml/filter_processor.hpp",
        "src/c4/yml/parser_state.hpp",
        "src/c4/yml/event_handler_stack.hpp",
        am.onlyif(has(Event.tree), "src/c4/yml/event_handler_tree.hpp"),
        am.onlyif(has(Event.ints), "src_extra/c4/yml/extra/event_handler_ints.hpp"),
        am.onlyif(has(Event.ints_utils), "src_extra/c4/yml/extra/ints_utils.hpp"),
        am.onlyif(has(Event.ints_to_testsuite), "src_extra/c4/yml/extra/ints_to_testsuite.hpp"),
        "src/c4/yml/parse_engine.hpp",
        am.onlyif(has(Event.tree), "src/c4/yml/reference_resolver.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/parse.hpp"),
        am.onlyif(with_stl and has(Event.tree), "src/c4/yml/std/map.hpp"),
        am.onlyif(with_stl and has(Event.tree), "src/c4/yml/std/string.hpp"),
        am.onlyif(with_stl and has(Event.tree), "src/c4/yml/std/vector.hpp"),
        am.onlyif(with_stl and has(Event.tree), "src/c4/yml/std/std.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/detail/checks.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/detail/print.hpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/yml.hpp"),
        am.onlyif(has(Event.tree), "src/ryml.hpp"),
        am.onlyif(with_engine_def_on_hdr, "src/c4/yml/parse_engine.def.hpp"),
    ]
    srcfiles += [
        "src/c4/yml/version.cpp",
        "src/c4/yml/common.cpp",
        "src/c4/yml/node_type.cpp",
        "src/c4/yml/tag.cpp",
        am.onlyif(not with_engine_def_on_hdr, "src/c4/yml/parse_engine.def.hpp"),
        am.onlyif(has(Event.ints), "src_extra/c4/yml/extra/event_handler_ints.cpp"),
        am.onlyif(has(Event.ints_utils), "src_extra/c4/yml/extra/ints_utils.cpp"),
        am.onlyif(has(Event.ints_to_testsuite), "src_extra/c4/yml/extra/ints_to_testsuite.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/tree.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/reference_resolver.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/parse.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/scalar_style.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_buf.cpp"),
        am.onlyif(has(Event.tree), "src/c4/yml/emit_file.cpp"),
    ]
    return hdrfiles, srcfiles


# -----------------------------------------------------------------------------

def amalgamate_ryml(filename: str,
                    amtype: AmalgamateType,
                    with_c4core: bool,
                    with_c4core_dev: bool,
                    with_fastfloat: bool,
                    with_stl: bool,
                    events: List[Event],
                    engine_hdr: bool):
    repo = "https://github.com/biojppm/rapidyaml"
    ryml_preamble = f"""
Rapid YAML - a library to parse and emit YAML, and do it fast.

{repo}

DO NOT EDIT. This file is generated automatically.
"""
    hdrfiles = []
    srcfiles = []
    if amtype == AmalgamateType.single_hdr:
        hdrfiles += [
            am.cmttext(ryml_preamble),
            am.cmttext(singlehdr_instructions),
            am.cmtfile("LICENSE.txt"),
            am.injcode(singlehdr_ryml_def_code),
            am.injcode(singlehdr_c4core_def_code),
            am.injcode(required_includes),
        ]
    elif amtype == AmalgamateType.single_src:
        if filename is None:
            raise Exception("filename required")
        hdrname = f"{filename}.hpp"
        srcname = f"{filename}.cpp"
        hdrfiles += [
            am.cmttext(ryml_preamble),
            am.cmttext(singlesrc_instructions_hdr),
            am.cmtfile("LICENSE.txt"),
            am.injcode(singlesrc_ryml_def_code),
            am.injcode(singlesrc_c4core_def_code),
            am.injcode(required_includes),
        ]
        srcfiles += [
            am.cmttext(ryml_preamble),
            am.cmttext(singlesrc_instructions_src),
            am.cmtfile("LICENSE.txt"),
            am.injcode(f"#include <{hdrname}>")
        ]

    hdr, src = c4core_filelist(with_c4core=with_c4core,
                               with_c4core_dev=with_c4core_dev,
                               with_fastfloat=with_fastfloat,
                               with_stl=with_stl)
    hdrfiles += hdr
    srcfiles += src

    hdr, src = ryml_filelist(with_stl=with_stl,
                             events=events,
                             with_engine_def_on_hdr=engine_hdr)
    hdrfiles += hdr
    srcfiles += src

    # comment out lines with these patterns:
    rxs = [
        re.compile(r'^\s*#\s*include "(c4/yml/.*)".*$'),
        re.compile(r'^\s*#\s*include <(c4/yml/.*)>.*$'),
        re.compile(r'^\s*#\s*include "(c4/.*)".*$'),
        re.compile(r'^\s*#\s*include <(c4/.*)>.*$'),
    ]

    if amtype == AmalgamateType.single_hdr:
        result = am.catfiles(hdrfiles+srcfiles,
                             projdir,
                             include_regexes=rxs,
                             definition_macro=singlehdr_ryml_defmacro,
                             repo=repo,
                             result_incguard=singlehdr_guard_macro)
        result_with_only_first_includes = am.include_only_first(result)
        am.file_put_contents(filename, result_with_only_first_includes)
    elif amtype == AmalgamateType.single_src:
        # amalgamate the headers
        result = am.catfiles(hdrfiles,
                             projdir,
                             include_regexes=rxs,
                             definition_macro=None, # not needed
                             repo=repo,
                             result_incguard=singlesrc_guard_macro)
        filtered_hdr = am.include_only_first(result)
        am.file_put_contents(hdrname, filtered_hdr)
        # amalgamate the sources
        result = am.catfiles(srcfiles,
                             projdir,
                             include_regexes=rxs,
                             definition_macro=None, # not needed
                             repo=repo,
                             result_incguard=None) # not needed
        filtered_src = am.include_only_first(result)
        am.file_put_contents(srcname, filtered_src)


def mkparser():
    p = am.mkparser(
        c4core=(True, "amalgamate c4core together with ryml"),
        c4core_dev=(False, "amalgamate c4core (dev utilities) together with ryml"),
        fastfloat=(True, "enable fastfloat library"),
        stl=(True, "enable stl interop"),
        engine_hdr=(False, """place the parse engine definition header
        on the header file when amalgamating as single_src""")
    )
    # add arg to choose amalgamate type
    default = str(AmalgamateType.single_hdr)
    typesdoc = '. '.join([f"'{t}': {amalgamate_type_doc[t]}" for t in AmalgamateType])
    p.add_argument('-t', '--type', type=str, default=default, choices=[str(t) for t in AmalgamateType],
                   help=f"""Choose amalgamation type. Possible
                   values are - {typesdoc}. The default is {default}.""")
    # add arg to choose events
    default = [str(Event.tree)]
    evtdoc = '. '.join([f"'{e}': {event_doc[e]}" for e in Event])
    defaultdoc = ','.join([str(e) for e in default])
    p.add_argument('-e', '--events', type=str, default=default, nargs="+",
                   choices=[str(e) for e in Event],
                   help=f"""Specify which event handlers to include. Possible
                   values are - {evtdoc}. The default is {defaultdoc}.""")
    return p


if __name__ == "__main__":
    args = mkparser().parse_args()
    args.events = [Event(e) for e in args.events] # is there a better way to do this?
    args.type = AmalgamateType(args.type) # is there a better way to do this?
    amalgamate_ryml(filename=args.output,
                    amtype=args.type,
                    with_c4core=args.c4core,
                    with_c4core_dev=args.c4core_dev,
                    with_fastfloat=args.fastfloat,
                    with_stl=args.stl,
                    events=args.events,
                    engine_hdr=args.engine_hdr)
