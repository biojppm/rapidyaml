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
    EventHandlerYamlStd::EventSink sink;
    EventHandlerYamlStd handler(&sink);
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    std::string src_copy(ec.src.str, ec.src.len);
    parser.parse_in_place_ev("(testyaml)", to_substr(src_copy));
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    // use the diff from std::string which is nice
    std::string exp_copy(ec.expected_events_from_parser.str, ec.expected_events_from_parser.len);
    std::string result_copy(result.str, result.len);
    EXPECT_EQ(result_copy, exp_copy);
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
        "",
        "+STR\n"
        "-STR\n"),
    //=======================================================
    _ec("empty_whitespace",
        "     ",
        "+STR\n"
        "-STR\n"),
    //=======================================================
    _ec("empty_whitespace_newlines",
        "\n    ",
        "+STR\n"
        "-STR\n"),
    //=======================================================
    _ec("empty_whitespace_newlines_comments",
        "\n"
        "# a comment\n"
        "    ",
        "+STR\n"
        "-STR\n"),
    //=======================================================
    _ec("docval",
        "'quoted val'\n"
        ,
        "+STR\n"
        "+DOC\n"
        "=VAL 'quoted val\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("docsep",
        "--- 'quoted val'\n"
        "--- another\n"
        "...\n"
        "--- and yet another\n"
        "...\n"
        "---\n"
        "...\n"
        ,
        "+STR\n"
        "+DOC ---\n"
        "=VAL 'quoted val\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :another\n"
        "-DOC ...\n"
        "+DOC ---\n"
        "=VAL :and yet another\n"
        "-DOC ...\n"
        "+DOC ---\n"
        "=VAL :\n"
        "-DOC ...\n"
        "-STR\n"
        ,
        "+STR\n"
        "+DOC ---\n"
        "=VAL 'quoted val\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :another\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :and yet another\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("docsep_v2"
        ,
        "\n"
        "doc1\n"
        "---\n"
        "doc2\n"
        "...\n"
        "doc3\n"
        ,
        "+STR\n"
        "+DOC\n"
        "=VAL :doc1\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :doc2\n"
        "-DOC ...\n"
        "+DOC\n"
        "=VAL :doc3\n"
        "-DOC\n"
        "-STR\n"
        ,
        "+STR\n"
        "+DOC ---\n"
        "=VAL :doc1\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :doc2\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL :doc3\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("basic_map",
        "{foo: bar}"
        ,
        "+STR\n"
        "+DOC\n"
        "+MAP {}\n"
        "=VAL :foo\n"
        "=VAL :bar\n"
        "-MAP\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("basic_seq",
        "[foo, bar]",
        "+STR\n"
        "+DOC\n"
        "+SEQ []\n"
        "=VAL :foo\n"
        "=VAL :bar\n"
        "-SEQ\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("escapes",
        "\"\\t\\	\\ \\r\\n\\0\\f\\/\\a\\v\\e\\N\\_\\L\\P  \\b\"",
        "+STR\n"
        "+DOC\n"
        "=VAL \"\\t\\t \\r\\n\\0\\f/\\a\\v\\e\\N\\_\\L\\P  \\b" "\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("dquo_bytes",
        "\"\\x0a\\x0a\\u263A\\x0a\\x55\\x56\\x57\\x0a\\u2705\\U0001D11E\"",
        "+STR\n"
        "+DOC\n"
        "=VAL \"\\n\\n☺\\nUVW\\n✅𝄞" "\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("sets",
        "--- !!set\n"
        "? Mark McGwire\n"
        "? Sammy Sosa\n"
        "? Ken Griff\n",
        "+STR\n"
        "+DOC ---\n"
        "+MAP <tag:yaml.org,2002:set>\n"
        "=VAL :Mark McGwire\n"
        "=VAL :\n"
        "=VAL :Sammy Sosa\n"
        "=VAL :\n"
        "=VAL :Ken Griff\n"
        "=VAL :\n"
        "-MAP\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("binary"
        ,
        "canonical: !!binary \"\\\n"
        " R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\\\n"
        " OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\\\n"
        " +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\\\n"
        " AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=\"\n"
        "generic: !!binary |\n"
        " R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\n"
        " OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\n"
        " +f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\n"
        " AgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=\n"
        "description:\n"
        " The binary value above is a tiny arrow encoded as a gif image.\n"
        ,
        "+STR\n"
        "+DOC\n"
        "+MAP\n"
        "=VAL :canonical\n"
        "=VAL <tag:yaml.org,2002:binary> \"R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5OTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLCAgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=\n"
        "=VAL :generic\n"
        "=VAL <tag:yaml.org,2002:binary> |R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5\\nOTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/+\\n+f/++f/++f/++f/++f/++SH+Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\\nAgjoEwnuNAFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84BwwEeECcgggoBADs=\\n\n"
        "=VAL :description\n"
        "=VAL :The binary value above is a tiny arrow encoded as a gif image.\n"
        "-MAP\n"
        "-DOC\n"
        "-STR\n"
        ),
    //=======================================================
    _ec("tag_directives_wtf",
        "!!foo fluorescent",
        "+STR\n"
        "+DOC\n"
        "=VAL <tag:yaml.org,2002:foo> :fluorescent\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_6CK3",
        "\n"
        "%TAG !e! tag:example.com,2000:app/\n"
        "---\n"
        "- !local foo\n"
        "- !!str bar\n"
        "- !e!tag%21 baz\n",
        "+STR\n"
        "+DOC ---\n"
        "+SEQ\n"
        "=VAL <!local> :foo\n"
        "=VAL <tag:yaml.org,2002:str> :bar\n"
        "=VAL <tag:example.com,2000:app/tag!> :baz\n"
        "-SEQ\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_6VLF",
        "\n"
        "%FOO  bar baz # Should be ignored\n"
        "              # with a warning.\n"
        "--- \"foo\"\n",
        "+STR\n"
        "+DOC ---\n"
        "=VAL \"foo\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_6WLZ",
        "\n"
        "# Private\n"
        "---\n"
        "!foo \"bar\"\n"
        "...\n"
        "# Global\n"
        "%TAG ! tag:example.com,2000:app/\n"
        "---\n"
        "!foo \"bar\"\n",
        "+STR\n"
        "+DOC ---\n"
        "=VAL <!foo> \"bar\n"
        "-DOC ...\n"
        "+DOC ---\n"
        "=VAL <tag:example.com,2000:app/foo> \"bar\n"
        "-DOC\n"
        "-STR\n",
        "+STR\n"
        "+DOC ---\n"
        "=VAL <!foo> \"bar\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL <tag:example.com,2000:app/foo> \"bar\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_9WXW",
        "\n"
        "# Private\n"
        "#---   # note this is commented out\n"
        "!foo \"bar\"\n"
        "...\n"
        "# Global\n"
        "%TAG ! tag:example.com,2000:app/\n"
        "---\n"
        "!foo \"bar\"\n",
        "+STR\n"
        "+DOC\n"
        "=VAL <!foo> \"bar\n"
        "-DOC ...\n"
        "+DOC ---\n"
        "=VAL <tag:example.com,2000:app/foo> \"bar\n"
        "-DOC\n"
        "-STR\n",
        "+STR\n"
        "+DOC ---\n"
        "=VAL <!foo> \"bar\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL <tag:example.com,2000:app/foo> \"bar\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_7FWL",
        "!<tag:yaml.org,2002:str> foo :\n"
        "  !<!bar> baz\n",
        "+STR\n"
        "+DOC\n"
        "+MAP\n"
        "=VAL <tag:yaml.org,2002:str> :foo\n"
        "=VAL <!bar> :baz\n"
        "-MAP\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_P76L",
        "\n"
        "%TAG !! tag:example.com,2000:app/\n"
        "---\n"
        "!!int 1 - 3 # Interval, not integer\n",
        "+STR\n"
        "+DOC ---\n"
        "=VAL <tag:example.com,2000:app/int> :1 - 3\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_S4JQ",
        "\n"
        "- \"12\"\n"
        "- 12\n"
        "- ! 12\n"
        ,
        "+STR\n"
        "+DOC\n"
        "+SEQ\n"
        "=VAL \"12\n"
        "=VAL :12\n"
        "=VAL <!> :12\n"
        "-SEQ\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("tag_directives_lookup",
        "\n"
        "%TAG !m! !my-\n"
        "--- # Bulb here\n"
        "!m!light fluorescent\n"
        "...\n"
        "%TAG !m! !meta-\n"
        "--- # Color here\n"
        "!m!light green\n"
        ,
        "+STR\n"
        "+DOC ---\n"
        "=VAL <!my-light> :fluorescent\n"
        "-DOC ...\n"
        "+DOC ---\n"
        "=VAL <!meta-light> :green\n"
        "-DOC\n"
        "-STR\n"
        ,
        "+STR\n"
        "+DOC ---\n"
        "=VAL <!my-light> :fluorescent\n"
        "-DOC\n"
        "+DOC ---\n"
        "=VAL <!meta-light> :green\n"
        "-DOC\n"
        "-STR\n"),
    //=======================================================
    _ec("anchors_refs",
        "\n"
        "A: &A\n"
        "    V: 3\n"
        "    L:\n"
        "        - 1\n"
        "B:\n"
        "    <<: *A\n"
        "    V: 4\n"
        "    L:\n"
        "        -5\n",
        "+STR\n"
        "+DOC\n"
        "+MAP\n"
        "=VAL :A\n"
        "+MAP &A\n"
        "=VAL :V\n"
        "=VAL :3\n"
        "=VAL :L\n"
        "+SEQ\n"
        "=VAL :1\n"
        "-SEQ\n"
        "-MAP\n"
        "=VAL :B\n"
        "+MAP\n"
        "=VAL :<<\n"
        "=ALI *A\n"
        "=VAL :V\n"
        "=VAL :4\n"
        "=VAL :L\n"
        "=VAL :-5\n"
        "-MAP\n"
        "-MAP\n"
        "-DOC\n"
        "-STR\n"),
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
