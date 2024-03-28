#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include <gtest/gtest.h>

#include "./test_lib/test_case.hpp"
#include "./test_suite/test_suite_events.hpp"
#include "./test_suite/test_suite_event_handler.hpp"
#include "./test_suite/test_suite_events_emitter.cpp" // HACK

namespace c4 {
namespace yml {

struct EventsCase
{
    const char* file;
    int line;
    // previously, the strings below were of type std::string, but
    // valgrind was complaining of a problem during initialization of
    // the parameterized test cases. Probably some SIOF?
    //
    // So we use csubstr:
    csubstr name;
    csubstr src;
    csubstr expected_events_from_parser;
    csubstr expected_events_from_tree;
    EventsCase(const char *file_, int line_, csubstr name_, csubstr src_, csubstr from_parser, csubstr from_tree)
        : file(file_)
        , line(line_)
        , name(name_)
        , src(src_)
        , expected_events_from_parser(from_parser)
        , expected_events_from_tree(from_tree)
    {
    }
    EventsCase(const char *file_, int line_, csubstr name_, csubstr src_, csubstr evts)
        : file(file_)
        , line(line_)
        , name(name_)
        , src(src_)
        , expected_events_from_parser(evts)
        , expected_events_from_tree(evts)
    {
    }
};

class EventsTest : public testing::TestWithParam<EventsCase> {};


TEST_P(EventsTest, from_parser)
{
    EventsCase const& ec = GetParam();
    printf("%s:%d: %s", ec.file, ec.line, ec.name.str);
    RYML_TRACE_FMT("defined in:\n{}:{}: {}", ec.file, ec.line, ec.name);
    EventSink sink;
    EventHandlerYamlStd handler(&sink);
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    std::string src_copy(ec.src.str, ec.src.len);
    parser.parse_in_place_ev("(testyaml)", to_substr(src_copy));
    _c4dbgpf("~~~\n{}~~~\n", sink.result);
    std::string exp_copy(ec.expected_events_from_parser.str, ec.expected_events_from_parser.len);
    EXPECT_EQ(sink.result, exp_copy); // use the diff from std::string which is nice
}

TEST_P(EventsTest, from_tree)
{
    EventsCase const& ec = GetParam();
    printf("%s:%d: %s", ec.file, ec.line, ec.name.str);
    RYML_TRACE_FMT("defined in:\n{}:{}: {}", ec.file, ec.line, ec.name);
    const Tree tree = parse_in_arena(to_csubstr(ec.src));
    _c4dbg_tree("parsed tree", tree);
    std::string exp_copy(ec.expected_events_from_tree.str, ec.expected_events_from_tree.len);
    EXPECT_EQ(emit_events_from_tree<std::string>(tree), exp_copy);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define _ec(name, src, ...) EventsCase{__FILE__, __LINE__, name, src, __VA_ARGS__}
const EventsCase events_cases[] = {
    //=======================================================
    _ec("empty",
        R"()",
        R"(+STR
-STR
)"),
    //=======================================================
    _ec("empty_whitespace",
        R"(     )",
        R"(+STR
-STR
)"),
    //=======================================================
    _ec("empty_whitespace_newlines",
        R"(
    )",
        R"(+STR
-STR
)"),
    //=======================================================
    _ec("empty_whitespace_newlines_comments",
        R"(
# a comment
    )",
        R"(+STR
-STR
)"),
    //=======================================================
    _ec("docval",
        R"('quoted val'
)",
        R"(+STR
+DOC
=VAL 'quoted val
-DOC
-STR
)"),
    //=======================================================
    _ec("docsep",
        R"(--- 'quoted val'
--- another
...
--- and yet another
...
---
...
)",
        R"(+STR
+DOC ---
=VAL 'quoted val
-DOC
+DOC ---
=VAL :another
-DOC ...
+DOC ---
=VAL :and yet another
-DOC ...
+DOC ---
=VAL :
-DOC ...
-STR
)",
        R"(+STR
+DOC ---
=VAL 'quoted val
-DOC
+DOC ---
=VAL :another
-DOC
+DOC ---
=VAL :and yet another
-DOC
+DOC ---
=VAL :
-DOC
-STR
)"),
    //=======================================================
    _ec("docsep_v2",
        R"(
doc1
---
doc2
...
doc3
)",
        R"(+STR
+DOC
=VAL :doc1
-DOC
+DOC ---
=VAL :doc2
-DOC ...
+DOC
=VAL :doc3
-DOC
-STR
)",
        R"(+STR
+DOC ---
=VAL :doc1
-DOC
+DOC ---
=VAL :doc2
-DOC
+DOC ---
=VAL :doc3
-DOC
-STR
)"),
    //=======================================================
    _ec("basic_map",
        "{foo: bar}",
        R"(+STR
+DOC
+MAP {}
=VAL :foo
=VAL :bar
-MAP
-DOC
-STR
)"),
    //=======================================================
    _ec("basic_seq",
        "[foo, bar]",
        R"(+STR
+DOC
+SEQ []
=VAL :foo
=VAL :bar
-SEQ
-DOC
-STR
)"),
    //=======================================================
    _ec("escapes",
        R"("\t\	\ \r\n\0\f\/\a\v\e\N\_\L\P  \b")",
        "+STR\n"
        "+DOC\n"
        "=VAL \"\\t\\t \\r\\n\\0\\f/\\a\\v\\e\\N\\_\\L\\P  \\b" "\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("dquo_bytes",
        R"("\x0a\x0a\u263A\x0a\x55\x56\x57\x0a\u2705\U0001D11E")",
        "+STR\n"
        "+DOC\n"
        "=VAL \"\\n\\n☺\\nUVW\\n✅𝄞" "\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("sets",
        R"(--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
        R"(+STR
+DOC ---
+MAP <tag:yaml.org,2002:set>
=VAL :Mark McGwire
=VAL :
=VAL :Sammy Sosa
=VAL :
=VAL :Ken Griff
=VAL :
-MAP
-DOC
-STR
)"),
    //=======================================================
    _ec("binary",
        R"(canonical: !!binary "\
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\
 OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\
 +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\
 AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs="
generic: !!binary |
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5
 OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+
 +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC
 AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=
description:
 The binary value above is a tiny arrow encoded as a gif image.
)",
        R"(+STR
+DOC
+MAP
=VAL :canonical
=VAL <tag:yaml.org,2002:binary> "R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLCAgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=
=VAL :generic
=VAL <tag:yaml.org,2002:binary> |R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\nOTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\n+f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\nAgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=\n
=VAL :description
=VAL :The binary value above is a tiny arrow encoded as a gif image.
-MAP
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_wtf",
        R"(!!foo fluorescent)",
        R"(+STR
+DOC
=VAL <tag:yaml.org,2002:foo> :fluorescent
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_6CK3",
        R"(
%TAG !e! tag:example.com,2000:app/
---
- !local foo
- !!str bar
- !e!tag%21 baz
)",
        R"(+STR
+DOC ---
+SEQ
=VAL <!local> :foo
=VAL <tag:yaml.org,2002:str> :bar
=VAL <tag:example.com,2000:app/tag!> :baz
-SEQ
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_6VLF",
        R"(
%FOO  bar baz # Should be ignored
              # with a warning.
--- "foo"
)",
        R"(+STR
+DOC ---
=VAL "foo
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_6WLZ",
        R"(
# Private
---
!foo "bar"
...
# Global
%TAG ! tag:example.com,2000:app/
---
!foo "bar"
)",
        R"(+STR
+DOC ---
=VAL <!foo> "bar
-DOC ...
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)",
        R"(+STR
+DOC ---
=VAL <!foo> "bar
-DOC
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_9WXW",
        R"(
# Private
#---   # note this is commented out
!foo "bar"
...
# Global
%TAG ! tag:example.com,2000:app/
---
!foo "bar"
)",
        R"(+STR
+DOC
=VAL <!foo> "bar
-DOC ...
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)",
        R"(+STR
+DOC ---
=VAL <!foo> "bar
-DOC
+DOC ---
=VAL <tag:example.com,2000:app/foo> "bar
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_7FWL",
        R"(!<tag:yaml.org,2002:str> foo :
  !<!bar> baz
)",
        R"(+STR
+DOC
+MAP
=VAL <tag:yaml.org,2002:str> :foo
=VAL <!bar> :baz
-MAP
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_P76L",
        R"(
%TAG !! tag:example.com,2000:app/
---
!!int 1 - 3 # Interval, not integer
)",
        R"(+STR
+DOC ---
=VAL <tag:example.com,2000:app/int> :1 - 3
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_S4JQ",
        R"(
- "12"
- 12
- ! 12
)",
        R"(+STR
+DOC
+SEQ
=VAL "12
=VAL :12
=VAL <!> :12
-SEQ
-DOC
-STR
)"),
    //=======================================================
    _ec("tag_directives_lookup",
        R"(
%TAG !m! !my-
--- # Bulb here
!m!light fluorescent
...
%TAG !m! !meta-
--- # Color here
!m!light green
)",
        R"(+STR
+DOC ---
=VAL <!my-light> :fluorescent
-DOC ...
+DOC ---
=VAL <!meta-light> :green
-DOC
-STR
)",
        R"(+STR
+DOC ---
=VAL <!my-light> :fluorescent
-DOC
+DOC ---
=VAL <!meta-light> :green
-DOC
-STR
)"),
    //=======================================================
    _ec("anchors_refs",
        R"(
A: &A
    V: 3
    L:
        - 1
B:
    <<: *A
    V: 4
    L:
        -5
)",
        R"(+STR
+DOC
+MAP
=VAL :A
+MAP &A
=VAL :V
=VAL :3
=VAL :L
+SEQ
=VAL :1
-SEQ
-MAP
=VAL :B
+MAP
=VAL :<<
=ALI *A
=VAL :V
=VAL :4
=VAL :L
=VAL :-5
-MAP
-MAP
-DOC
-STR
)"),
};


INSTANTIATE_TEST_SUITE_P(Events, EventsTest, testing::ValuesIn(events_cases));

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
