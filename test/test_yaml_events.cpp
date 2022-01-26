#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include <gtest/gtest.h>

#include "./test_case.hpp"
#include "./test_suite/test_suite_events.hpp"
#include "./test_suite/test_suite_events_emitter.cpp" // HACK

namespace c4 {
namespace yml {

void test_evts(csubstr src, std::string expected)
{
    Tree tree = parse_in_arena(src);
    #if RYML_DBG
    print_tree(tree);
    #endif
    auto actual = emit_events<std::string>(tree);
    EXPECT_EQ(actual, expected);
}

TEST(events, empty)
{
    test_evts(
        R"()",
        R"(+STR
-STR
)"
        );
}

TEST(events, empty_whitespace)
{
    test_evts(
        R"(     )",
        R"(+STR
-STR
)"
        );
}

TEST(events, empty_whitespace_newlines)
{
    test_evts(
        R"(   
    )",
        R"(+STR
-STR
)"
        );
}

TEST(events, empty_whitespace_newlines_comments)
{
    test_evts(
        R"(   
# a comment
    )",
        R"(+STR
-STR
)"
        );
}

TEST(events, docval)
{
    test_evts(
        R"('quoted val'
)",
        R"(+STR
+DOC
=VAL 'quoted val
-DOC
-STR
)"
        );
}

TEST(events, docsep)
{
    test_evts(
        R"(--- 'quoted val'
--- another
...
--- and yet another
...
---
...
)",
        R"(+STR
+DOC
=VAL 'quoted val
-DOC
+DOC
=VAL :another
-DOC
+DOC
=VAL :and yet another
-DOC
+DOC
=VAL :
-DOC
-STR
)"
        );
}

TEST(events, basic_map)
{
    test_evts(
        "{foo: bar}",
        R"(+STR
+DOC
+MAP
=VAL :foo
=VAL :bar
-MAP
-DOC
-STR
)"
        );
}

TEST(events, basic_seq)
{
    test_evts(
        "[foo, bar]",
        R"(+STR
+DOC
+SEQ
=VAL :foo
=VAL :bar
-SEQ
-DOC
-STR
)"
        );
}

TEST(events, dquo_chars)
{
    test_evts(
        R"("\b\r\n\0\f\/")",
        "+STR\n"
        "+DOC\n"
        "=VAL '\\b\\r\\n\\0\\f/\n"
        "-DOC\n"
        "-STR\n"
        );
}

TEST(events, dquo_bytes)
{
    test_evts(
        R"("\x0a\x0a\u263A\x0a\x55\x56\x57\x0a\u2705\U0001D11E")",
        "+STR\n"
        "+DOC\n"
        "=VAL '\\n\\n☺\\nUVW\\n✅𝄞\n"
        "-DOC\n"
        "-STR\n"
        );
}


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
