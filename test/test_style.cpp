#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif

#include "./test_lib/test_case.hpp"

#include <gtest/gtest.h>

namespace c4 {
namespace yml {

std::string emit2str(Tree const& t)
{
    return emitrs_yaml<std::string>(t);
}

inline void test_container_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_FALSE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml());
}

inline void test_container_block(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_TRUE(n.type().is_block());
    EXPECT_FALSE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml());
}

inline void test_container_flow_sl(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_TRUE(n.type().is_flow());
    EXPECT_TRUE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml());
}

inline void test_container_flow_ml(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_TRUE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_TRUE(n.type().is_flow_ml());
}

inline void test_key_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_TRUE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_val_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_val_styled());
    EXPECT_TRUE(n.type().is_val_plain());
    EXPECT_FALSE(n.type().is_val_squo());
    EXPECT_FALSE(n.type().is_val_dquo());
    EXPECT_FALSE(n.type().is_val_literal());
    EXPECT_FALSE(n.type().is_val_folded());
}

inline void test_key_squo(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_FALSE(n.type().is_key_plain());
    EXPECT_TRUE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_val_squo(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_val_styled());
    EXPECT_FALSE(n.type().is_val_plain());
    EXPECT_TRUE(n.type().is_val_squo());
    EXPECT_FALSE(n.type().is_val_dquo());
    EXPECT_FALSE(n.type().is_val_literal());
    EXPECT_FALSE(n.type().is_val_folded());
}

inline void test_key_dquo(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_FALSE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_TRUE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_val_dquo(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_val_styled());
    EXPECT_FALSE(n.type().is_val_plain());
    EXPECT_FALSE(n.type().is_val_squo());
    EXPECT_TRUE(n.type().is_val_dquo());
    EXPECT_FALSE(n.type().is_val_literal());
    EXPECT_FALSE(n.type().is_val_folded());
}

inline void test_key_literal(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_FALSE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_TRUE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_val_literal(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_val_styled());
    EXPECT_FALSE(n.type().is_val_plain());
    EXPECT_FALSE(n.type().is_val_squo());
    EXPECT_FALSE(n.type().is_val_dquo());
    EXPECT_TRUE(n.type().is_val_literal());
    EXPECT_FALSE(n.type().is_val_folded());
}

inline void test_key_folded(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_FALSE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_TRUE(n.type().is_key_folded());
}
inline void test_val_folded(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_val_styled());
    EXPECT_FALSE(n.type().is_val_plain());
    EXPECT_FALSE(n.type().is_val_squo());
    EXPECT_FALSE(n.type().is_val_dquo());
    EXPECT_FALSE(n.type().is_val_literal());
    EXPECT_TRUE(n.type().is_val_folded());
}

inline void test_key_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.type().is_key_styled());
    EXPECT_FALSE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_val_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.type().is_val_styled());
    EXPECT_FALSE(n.type().is_val_plain());
    EXPECT_FALSE(n.type().is_val_squo());
    EXPECT_FALSE(n.type().is_val_dquo());
    EXPECT_FALSE(n.type().is_val_literal());
    EXPECT_FALSE(n.type().is_val_folded());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(style, flags)
{
    Tree tree = parse_in_arena("foo: bar");
    test_container_block(tree.rootref());
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    test_container_flow_sl(tree.rootref());
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_ML);
    test_container_flow_ml(tree.rootref());
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
    #ifndef RYML_DBG
        #define _showtrees(num)
    #else
        #define _showtrees(num)                                     \
        {                                                           \
            std::cout << "--------\nEMITTED" #num "\n--------\n";   \
            std::cout << ws ## num;                                 \
            std::cout << "--------\nACTUAL" #num "\n--------\n";    \
            print_tree(actual ## num);                              \
            std::cout << "--------\nEXPECTED" #num "\n--------\n";  \
            print_tree(expected);                                   \
        }
    #endif

    std::string ws1, ws2, ws3, ws4;
    emitrs_yaml(expected, &ws1);
    {
        SCOPED_TRACE("actual1");
        Tree actual1 = parse_in_arena(to_csubstr(ws1));
        _showtrees(1);
        test_compare(actual1, expected);
        emitrs_yaml(actual1, &ws2);
    }
    {
        SCOPED_TRACE("actual2");
        Tree actual2 = parse_in_arena(to_csubstr(ws2));
        _showtrees(2);
        test_compare(actual2, expected);
        emitrs_yaml(actual2, &ws3);
    }
    {
        SCOPED_TRACE("actual3");
        Tree actual3 = parse_in_arena(to_csubstr(ws3));
        _showtrees(3);
        test_compare(actual3, expected);
        emitrs_yaml(actual3, &ws4);
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
    auto setcont = [](NodeRef n, NodeType t){
        n |= t;
        test_container_nostyle(n);
        return n;
    };
    auto setval = [](NodeRef n, csubstr key, csubstr val){
        NodeRef ch = n[key];
        ch = val;
        test_key_nostyle(ch);
        test_val_nostyle(ch);
    };
    Tree orig = parse_in_arena("{}");
    NodeRef r = orig.rootref();
    test_container_flow_sl(r);
    {
        NodeRef n = setcont(r["normal"], MAP);
        setval(n, "singleline", "foo");
        NodeRef ml = setcont(n["multiline"], MAP);
        setval(ml, "____________", "foo");
        setval(ml, "____mid_____", "foo\nbar");
        setval(ml, "____mid_end1", "foo\nbar\n");
        setval(ml, "____mid_end2", "foo\nbar\n\n");
        setval(ml, "____mid_end3", "foo\nbar\n\n\n");
        setval(ml, "____________", "foo");
        setval(ml, "____________", "foo bar");
        setval(ml, "________end1", "foo bar\n");
        setval(ml, "________end2", "foo bar\n\n");
        setval(ml, "________end3", "foo bar\n\n\n");
        setval(ml, "beg_________", "\nfoo");
        setval(ml, "beg_mid_____", "\nfoo\nbar");
        setval(ml, "beg_mid_end1", "\nfoo\nbar\n");
        setval(ml, "beg_mid_end2", "\nfoo\nbar\n\n");
        setval(ml, "beg_mid_end3", "\nfoo\nbar\n\n\n");
    }
    {
        NodeRef n = setcont(r["leading_ws"], MAP);
        {
            NodeRef sl = setcont(n["singleline"], MAP);
            sl["space"] = " foo";
            sl["tab"] = "\tfoo";
            sl["space_and_tab0"] = " \tfoo";
            sl["space_and_tab1"] = "\t foo";
        }
        {
            NodeRef ml = setcont(n["multiline"], MAP);
            ml["beg_________"] = "\n \tfoo";
            ml["beg_mid_____"] = "\n \tfoo\nbar";
            ml["beg_mid_end1"] = "\n \tfoo\nbar\n";
            ml["beg_mid_end2"] = "\n \tfoo\nbar\n\n";
            ml["beg_mid_end3"] = "\n \tfoo\nbar\n\n\n";
        }
    }
    std::string emitted = emit2str(orig);
    const Tree parsed = parse_in_place(to_substr(emitted));
    _c4dbg_tree("original", orig);
    _c4dbg_tree("parsed", parsed);
    test_compare(parsed, orig);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(style, scalar_retains_style_after_parse__plain)
{
    const Tree t = parse_in_arena("foo");
    test_val_plain(t.rootref());
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string("foo\n"));
}

TEST(style, scalar_retains_style_after_parse__squo)
{
    const Tree t = parse_in_arena("'foo'");
    test_val_squo(t.rootref());
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string("'foo'\n"));
}

TEST(style, scalar_retains_style_after_parse__dquo)
{
    const Tree t = parse_in_arena("\"foo\"");
    test_val_dquo(t.rootref());
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string("\"foo\"\n"));
}

TEST(style, scalar_retains_style_after_parse__literal)
{
    const Tree t = parse_in_arena("|\n foo");
    test_val_literal(t.rootref());
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string("|\n  foo\n"));
}

TEST(style, scalar_retains_style_after_parse__folded)
{
    const Tree t = parse_in_arena(">\n foo");
    test_val_folded(t.rootref());
    EXPECT_EQ(emitrs_yaml<std::string>(t), std::string(">\n  foo\n"));
}

TEST(style, scalar_retains_style_after_parse__mixed)
{
    std::string yaml = "- foo\n- 'baz'\n- \"bat\"\n- |\n  baq\n- >\n  bax\n";
    const Tree t = parse_in_arena(to_csubstr(yaml));
    test_container_block(t.rootref());
    test_val_plain(t[0]);
    test_val_squo(t[1]);
    test_val_dquo(t[2]);
    test_val_literal(t[3]);
    test_val_folded(t[4]);
    EXPECT_EQ(emitrs_yaml<std::string>(t), yaml);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(scalar, base)
{
    const Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(tree[0].key(), csubstr("this is the key"));
    EXPECT_EQ(tree[0].val(), csubstr("this is the multiline \"val\" with\n'empty' lines"));
    EXPECT_EQ(emit2str(tree), R"(this is the key: >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}


//-----------------------------------------------------------------------------

TEST(scalar, block_literal__key)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE);
    tree._add_flags(tree[0].id(), KEY_LITERAL);
    test_key_literal(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(? |-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, block_literal__val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), VAL_STYLE);
    tree._add_flags(tree[0].id(), VAL_LITERAL);
    test_key_plain(tree[0]);
    test_val_literal(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, block_literal__key_val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE|VAL_STYLE);
    tree._add_flags(tree[0].id(), KEY_LITERAL|VAL_LITERAL);
    test_key_literal(tree[0]);
    test_val_literal(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(? |-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
    check_same_emit(tree);
}


//-----------------------------------------------------------------------------

TEST(scalar, block_folded__key)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE);
    tree._add_flags(tree[0].id(), KEY_FOLDED);
    test_key_folded(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(? >-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, block_folded__val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), VAL_STYLE);
    tree._add_flags(tree[0].id(), VAL_FOLDED);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(this is the key: >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, block_folded__key_val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE|VAL_STYLE);
    tree._add_flags(tree[0].id(), KEY_FOLDED|VAL_FOLDED);
    test_key_folded(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(? >-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}


//-----------------------------------------------------------------------------

TEST(scalar, squo__key)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE);
    tree._add_flags(tree[0].id(), KEY_SQUO);
    test_key_squo(tree[0]);
    test_val_folded(tree[0]);
    EXPECT_EQ(emit2str(tree), R"('this is the key': >-
  this is the multiline "val" with

  'empty' lines
)");
    check_same_emit(tree);
}

TEST(scalar, squo__val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), VAL_STYLE);
    tree._add_flags(tree[0].id(), VAL_SQUO);
    test_key_plain(tree[0]);
    test_val_squo(tree[0]);
    EXPECT_EQ(emit2str(tree), R"(this is the key: 'this is the multiline "val" with

  ''empty'' lines'
)");
    check_same_emit(tree);
}

TEST(scalar, squo__key_val)
{
    Tree tree = parse_in_arena(scalar_yaml);
    test_key_plain(tree[0]);
    test_val_folded(tree[0]);
    tree._rem_flags(tree[0].id(), KEY_STYLE|VAL_STYLE);
    tree._add_flags(tree[0].id(), KEY_SQUO|VAL_SQUO);
    test_key_squo(tree[0]);
    test_val_squo(tree[0]);
    EXPECT_EQ(emit2str(tree), R"('this is the key': 'this is the multiline "val" with

  ''empty'' lines'
)");
    check_same_emit(tree);
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
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(seq, block)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
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
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"([1,2,3,4,5,6])");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keyseq, block)
{
    Tree tree = parse_in_arena("{foo: [1, 2, 3, 4, 5, 6]}");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
    EXPECT_EQ(emit2str(tree), R"(foo: [1,2,3,4,5,6]
)");
    tree._rem_flags(tree["foo"].id(), CONTAINER_STYLE);
    tree._add_flags(tree["foo"].id(), BLOCK);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - 2
  - 3
  - 4
  - 5
  - 6
)");
    tree = parse_in_arena("{foo: [1, [2, 3], 4, [5, 6]]}");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
    EXPECT_EQ(emit2str(tree), R"(foo: [1,[2,3],4,[5,6]]
)");
    tree._rem_flags(tree["foo"].id(), CONTAINER_STYLE);
    tree._add_flags(tree["foo"].id(), BLOCK);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
}

TEST(keyseq, flow_sl)
{
    Tree tree = parse_in_arena("foo: [1, 2, 3, 4, 5, 6]");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,2,3,4,5,6]})");
    //
    tree = parse_in_arena("foo: [1, [2, 3], 4, [5, 6]]");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,[2,3],4,[5,6]]})");
    //
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
    tree._rem_flags(tree["foo"].id(), CONTAINER_STYLE);
    tree._add_flags(tree["foo"].id(), BLOCK);
    tree._add_flags(tree["foo"][1].id(), FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), FLOW_SL);
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
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
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
    Tree tree = parse_in_arena(R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
)");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keymap, block)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
    tree._rem_flags(tree["foo"].id(), CONTAINER_STYLE);
    tree._add_flags(tree["foo"].id(), BLOCK);
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
    Tree tree = parse_in_arena(R"(foo:
  1: 10
  2: 10
  3: 10
  4: 10
  5: 10
  6: 10
)");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}})");
    //
    tree = parse_in_arena(R"(foo:
  1: 10
  2:
    2: 10
    3: 10
  4: 10
  5:
    5: 10
    6: 10
)");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: {2: 10,3: 10},4: 10,5: {5: 10,6: 10}}})");
    tree._rem_flags(tree.root_id(), CONTAINER_STYLE);
    tree._add_flags(tree.root_id(), BLOCK);
    tree._add_flags(tree["foo"][1].id(), FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), FLOW_SL);
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
