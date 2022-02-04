#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif

#include "./test_case.hpp"

#include <gtest/gtest.h>

namespace c4 {
namespace yml {

std::string emit2str(Tree const& t)
{
    return emitrs<std::string>(t);
}


TEST(style, flags)
{
    Tree tree = parse_in_arena("foo: bar");
    EXPECT_TRUE(tree.rootref().type().default_block());
    EXPECT_FALSE(tree.rootref().type().marked_flow());
    EXPECT_FALSE(tree.rootref().type().marked_flow_sl());
    EXPECT_FALSE(tree.rootref().type().marked_flow_ml());
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_TRUE(tree.rootref().type().marked_flow());
    EXPECT_TRUE(tree.rootref().type().marked_flow_sl());
    EXPECT_FALSE(tree.rootref().type().marked_flow_ml());
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_ML);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_TRUE(tree.rootref().type().marked_flow());
    EXPECT_FALSE(tree.rootref().type().marked_flow_sl());
    EXPECT_TRUE(tree.rootref().type().marked_flow_ml());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
csubstr scalar_yaml = R"(
this is the key: >-
  this is the multiline
  "val" with

  'empty' lines
)";

void check_same_emit(Tree const& expected)
{
    #if 0
        #define _showtrees(num)                                     \
            std::cout << "--------\nEMITTED" #num "\n--------\n";   \
            std::cout << ws ## num;                                 \
            std::cout << "--------\nACTUAL" #num "\n--------\n";    \
            print_tree(actual ## num);                              \
            std::cout << "--------\nEXPECTED" #num "\n--------\n";  \
            print_tree(expected)
    #else
        #define _showtrees(num)
    #endif

    std::string ws1, ws2, ws3, ws4;
    emitrs(expected, &ws1);
    {
        SCOPED_TRACE("actual1");
        Tree actual1 = parse_in_arena(to_csubstr(ws1));
        _showtrees(1);
        test_compare(actual1, expected);
        emitrs(actual1, &ws2);
    }
    {
        SCOPED_TRACE("actual2");
        Tree actual2 = parse_in_arena(to_csubstr(ws2));
        _showtrees(2);
        test_compare(actual2, expected);
        emitrs(actual2, &ws3);
    }
    {
        SCOPED_TRACE("actual3");
        Tree actual3 = parse_in_arena(to_csubstr(ws3));
        _showtrees(3);
        test_compare(actual3, expected);
        emitrs(actual3, &ws4);
    }
    {
        SCOPED_TRACE("actual4");
        Tree actual4 = parse_in_arena(to_csubstr(ws4));
        _showtrees(4);
        test_compare(actual4, expected);
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


TEST(style, noflags)
{
    Tree expected = parse_in_arena("{}");
    NodeRef r = expected.rootref();
    r["normal"] |= MAP;
    r["normal"]["singleline"] = "foo";
    r["normal"]["multiline"] |= MAP;
    r["normal"]["multiline"]["____________"] = "foo";
    r["normal"]["multiline"]["____mid_____"] = "foo\nbar";
    r["normal"]["multiline"]["____mid_end1"] = "foo\nbar\n";
    r["normal"]["multiline"]["____mid_end2"] = "foo\nbar\n\n";
    r["normal"]["multiline"]["____mid_end3"] = "foo\nbar\n\n\n";
    r["normal"]["multiline"]["____________"] = "foo";
    r["normal"]["multiline"]["____________"] = "foo bar";
    r["normal"]["multiline"]["________end1"] = "foo bar\n";
    r["normal"]["multiline"]["________end2"] = "foo bar\n\n";
    r["normal"]["multiline"]["________end3"] = "foo bar\n\n\n";
    r["normal"]["multiline"]["beg_________"] = "\nfoo";
    r["normal"]["multiline"]["beg_mid_____"] = "\nfoo\nbar";
    r["normal"]["multiline"]["beg_mid_end1"] = "\nfoo\nbar\n";
    r["normal"]["multiline"]["beg_mid_end2"] = "\nfoo\nbar\n\n";
    r["normal"]["multiline"]["beg_mid_end3"] = "\nfoo\nbar\n\n\n";
    r["leading_ws"] |= MAP;
    r["leading_ws"]["singleline"] |= MAP;
    r["leading_ws"]["singleline"]["space"] = " foo";
    r["leading_ws"]["singleline"]["tab"] = "\tfoo";
    r["leading_ws"]["singleline"]["space_and_tab0"] = " \tfoo";
    r["leading_ws"]["singleline"]["space_and_tab1"] = "\t foo";
    r["leading_ws"]["multiline"] |= MAP;
    r["leading_ws"]["multiline"]["beg_________"] = "\n \tfoo";
    r["leading_ws"]["multiline"]["beg_mid_____"] = "\n \tfoo\nbar";
    r["leading_ws"]["multiline"]["beg_mid_end1"] = "\n \tfoo\nbar\n";
    r["leading_ws"]["multiline"]["beg_mid_end2"] = "\n \tfoo\nbar\n\n";
    r["leading_ws"]["multiline"]["beg_mid_end3"] = "\n \tfoo\nbar\n\n\n";
    check_same_emit(expected);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef WIP
TEST(style, scalar_retains_style_after_parse)
{
    {
        Tree t = parse_in_arena("foo");
        EXPECT_TRUE(t.rootref().type().val_marked_plain());
        EXPECT_FALSE(t.rootref().type().val_marked_squo());
        EXPECT_FALSE(t.rootref().type().val_marked_dquo());
        EXPECT_FALSE(t.rootref().type().val_marked_literal());
        EXPECT_FALSE(t.rootref().type().val_marked_folded());
        EXPECT_EQ(emitrs<std::string>(t), std::string("foo\n"));
    }
    {
        Tree t = parse_in_arena("'foo'");
        EXPECT_FALSE(t.rootref().type().val_marked_plain());
        EXPECT_TRUE(t.rootref().type().val_marked_squo());
        EXPECT_FALSE(t.rootref().type().val_marked_dquo());
        EXPECT_FALSE(t.rootref().type().val_marked_literal());
        EXPECT_FALSE(t.rootref().type().val_marked_folded());
        EXPECT_EQ(emitrs<std::string>(t), std::string("'foo'\n"));
    }
    {
        Tree t = parse_in_arena("'foo'");
        EXPECT_FALSE(t.rootref().type().val_marked_plain());
        EXPECT_FALSE(t.rootref().type().val_marked_squo());
        EXPECT_TRUE(t.rootref().type().val_marked_dquo());
        EXPECT_FALSE(t.rootref().type().val_marked_literal());
        EXPECT_FALSE(t.rootref().type().val_marked_folded());
        EXPECT_EQ(emitrs<std::string>(t), std::string("'foo'\n"));
    }
    {
        Tree t = parse_in_arena("[foo, 'baz', \"bat\"]");
        EXPECT_TRUE(t.rootref().type().marked_flow());
        EXPECT_TRUE(t[0].type().val_marked_plain());
        EXPECT_FALSE(t[0].type().val_marked_squo());
        EXPECT_FALSE(t[0].type().val_marked_dquo());
        EXPECT_FALSE(t[0].type().val_marked_literal());
        EXPECT_FALSE(t[0].type().val_marked_folded());
        EXPECT_FALSE(t[1].type().val_marked_plain());
        EXPECT_TRUE(t[1].type().val_marked_squo());
        EXPECT_FALSE(t[1].type().val_marked_dquo());
        EXPECT_FALSE(t[1].type().val_marked_literal());
        EXPECT_FALSE(t[1].type().val_marked_folded());
        EXPECT_FALSE(t[2].type().val_marked_plain());
        EXPECT_FALSE(t[2].type().val_marked_squo());
        EXPECT_TRUE(t[2].type().val_marked_dquo());
        EXPECT_FALSE(t[2].type().val_marked_literal());
        EXPECT_FALSE(t[2].type().val_marked_folded());
        EXPECT_EQ(emitrs<std::string>(t), std::string("foo"));
    }
}
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(scalar, base)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(tree[0].key(), csubstr("this is the key"));
    EXPECT_EQ(tree[0].val(), csubstr("this is the multiline \"val\" with\n'empty' lines"));
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, block_literal)
{
    Tree tree = parse_in_arena(scalar_yaml);
    {
        SCOPED_TRACE("val only");
        EXPECT_FALSE(tree[0].type().key_marked_literal());
        EXPECT_FALSE(tree[0].type().val_marked_literal());
        tree._add_flags(tree[0].id(), _WIP_VAL_LITERAL);
        EXPECT_FALSE(tree[0].type().key_marked_literal());
        EXPECT_TRUE(tree[0].type().val_marked_literal());
        EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key+val");
        tree._add_flags(tree[0].id(), _WIP_KEY_LITERAL);
        EXPECT_TRUE(tree[0].type().key_marked_literal());
        EXPECT_TRUE(tree[0].type().val_marked_literal());
        EXPECT_EQ(emit2str(tree), R"(? |-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key only");
        tree._rem_flags(tree[0].id(), _WIP_VAL_LITERAL);
        EXPECT_TRUE(tree[0].type().key_marked_literal());
        EXPECT_FALSE(tree[0].type().val_marked_literal());
        EXPECT_EQ(emit2str(tree), R"(? |-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
}

TEST(scalar, block_folded)
{
    Tree tree = parse_in_arena(scalar_yaml);
    {
        SCOPED_TRACE("val only");
        EXPECT_FALSE(tree[0].type().key_marked_folded());
        EXPECT_FALSE(tree[0].type().val_marked_folded());
        tree._add_flags(tree[0].id(), _WIP_VAL_FOLDED);
        EXPECT_FALSE(tree[0].type().key_marked_folded());
        EXPECT_TRUE(tree[0].type().val_marked_folded());
        EXPECT_EQ(emit2str(tree), R"(this is the key: >-
  this is the multiline "val" with

  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key+val");
        tree._add_flags(tree[0].id(), _WIP_KEY_FOLDED);
        EXPECT_TRUE(tree[0].type().key_marked_folded());
        EXPECT_TRUE(tree[0].type().val_marked_folded());
        EXPECT_EQ(emit2str(tree), R"(? >-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("val only");
        tree._rem_flags(tree[0].id(), _WIP_VAL_FOLDED);
        EXPECT_TRUE(tree[0].type().key_marked_folded());
        EXPECT_FALSE(tree[0].type().val_marked_folded());
        EXPECT_EQ(emit2str(tree), R"(? >-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
}

TEST(scalar, squot)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_FALSE(tree[0].type().key_marked_squo());
    EXPECT_FALSE(tree[0].type().val_marked_squo());
    {
        SCOPED_TRACE("val only");
        tree._add_flags(tree[0].id(), _WIP_VAL_SQUO);
        EXPECT_FALSE(tree[0].type().key_marked_squo());
        EXPECT_TRUE(tree[0].type().val_marked_squo());
        EXPECT_EQ(emit2str(tree), R"(this is the key: 'this is the multiline "val" with

  ''empty'' lines'
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key+val");
        tree._add_flags(tree[0].id(), _WIP_KEY_SQUO);
        EXPECT_TRUE(tree[0].type().key_marked_squo());
        EXPECT_TRUE(tree[0].type().val_marked_squo());
        EXPECT_EQ(emit2str(tree), R"('this is the key': 'this is the multiline "val" with

  ''empty'' lines'
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key only");
        tree._rem_flags(tree[0].id(), _WIP_VAL_SQUO);
        EXPECT_TRUE(tree[0].type().key_marked_squo());
        EXPECT_FALSE(tree[0].type().val_marked_squo());
        EXPECT_EQ(emit2str(tree), R"('this is the key': |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
}

TEST(scalar, dquot)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_FALSE(tree[0].type().key_marked_dquo());
    EXPECT_FALSE(tree[0].type().val_marked_dquo());
    {
        SCOPED_TRACE("val only");
        tree._add_flags(tree[0].id(), _WIP_VAL_DQUO);
        EXPECT_FALSE(tree[0].type().key_marked_dquo());
        EXPECT_TRUE(tree[0].type().val_marked_dquo());
        // visual studio fails to compile this string when used inside
        // the EXPECT_EQ() macro below. So we declare it separately
        // instead:
        csubstr yaml = R"(this is the key: "this is the multiline \"val\" with

  'empty' lines"
)";
        EXPECT_EQ(emit2str(tree), yaml);
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key+val");
        tree._add_flags(tree[0].id(), _WIP_KEY_DQUO);
        EXPECT_TRUE(tree[0].type().key_marked_dquo());
        EXPECT_TRUE(tree[0].type().val_marked_dquo());
        // visual studio fails to compile this string when used inside
        // the EXPECT_EQ() macro below. So we declare it separately
        // instead:
        csubstr yaml = R"("this is the key": "this is the multiline \"val\" with

  'empty' lines"
)";
        EXPECT_EQ(emit2str(tree), yaml);
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key only");
        tree._rem_flags(tree[0].id(), _WIP_VAL_DQUO);
        EXPECT_TRUE(tree[0].type().key_marked_dquo());
        EXPECT_FALSE(tree[0].type().val_marked_dquo());
        EXPECT_EQ(emit2str(tree), R"("this is the key": |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
}

TEST(scalar, plain)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_FALSE(tree[0].type().key_marked_plain());
    EXPECT_FALSE(tree[0].type().val_marked_plain());
    {
        SCOPED_TRACE("val only");
        tree._add_flags(tree[0].id(), _WIP_VAL_PLAIN);
        EXPECT_FALSE(tree[0].type().key_marked_plain());
        EXPECT_TRUE(tree[0].type().val_marked_plain());
        EXPECT_EQ(emit2str(tree), R"(this is the key: this is the multiline "val" with

  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key+val");
        tree._add_flags(tree[0].id(), _WIP_KEY_PLAIN);
        EXPECT_TRUE(tree[0].type().key_marked_plain());
        EXPECT_TRUE(tree[0].type().val_marked_plain());
        EXPECT_EQ(emit2str(tree), R"(this is the key: this is the multiline "val" with

  'empty' lines
)");
        check_same_emit(tree);
    }
    {
        SCOPED_TRACE("key only");
        tree._rem_flags(tree[0].id(), _WIP_VAL_PLAIN);
        EXPECT_TRUE(tree[0].type().key_marked_plain());
        EXPECT_FALSE(tree[0].type().val_marked_plain());
        EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
        check_same_emit(tree);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(stream, block)
{
    Tree tree = parse_in_arena(R"(
---
scalar
%YAML 1.2
---
foo
---
bar
)");
    EXPECT_TRUE(tree.rootref().is_stream());
    EXPECT_TRUE(tree.docref(0).is_doc());
    EXPECT_TRUE(tree.docref(0).is_val());
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(seq, block)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    EXPECT_EQ(emit2str(tree), R"(- 1
- 2
- 3
- 4
- 5
- 6
)");
}

TEST(seq, flow_sl)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"([1,2,3,4,5,6])");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keyseq, block)
{
    Tree tree = parse_in_arena("{foo: [1, 2, 3, 4, 5, 6]}");
    EXPECT_TRUE(tree.rootref().type().default_block());
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - 2
  - 3
  - 4
  - 5
  - 6
)");
    tree = parse_in_arena("{foo: [1, [2, 3], 4, [5, 6]]}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - - 2
    - 3
  - 4
  - - 5
    - 6
)");
}

TEST(keyseq, flow_sl)
{
    Tree tree = parse_in_arena("{foo: [1, 2, 3, 4, 5, 6]}");
    EXPECT_TRUE(tree.rootref().type().default_block());
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_EQ(emit2str(tree), R"({foo: [1,2,3,4,5,6]})");
    //
    tree = parse_in_arena("{foo: [1, [2, 3], 4, [5, 6]]}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,[2,3],4,[5,6]]})");
    //
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][1].id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(map, block)
{
    Tree tree = parse_in_arena("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    EXPECT_EQ(emit2str(tree), R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
)");
}

TEST(map, flow_sl)
{
    Tree tree = parse_in_arena("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keymap, block)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: 10
  3: 10
  4: 10
  5: 10
  6: 10
)");
}


TEST(keymap, flow_sl)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}})");
    //
    tree = parse_in_arena("{foo: {1: 10, 2: {2: 10, 3: 10}, 4: 10, 5: {5: 10, 6: 10}}}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2:
    2: 10
    3: 10
  4: 10
  5:
    5: 10
    6: 10
)");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: {2: 10,3: 10},4: 10,5: {5: 10,6: 10}}})");
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][1].id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: {2: 10,3: 10}
  4: 10
  5: {5: 10,6: 10}
)");
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
