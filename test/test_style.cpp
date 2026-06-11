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

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

std::string emit2str(Tree const& t, EmitOptions const& opts={})
{
    return emitrs_yaml<std::string>(t, opts);
}
EmitOptions maxcols(id_type max)
{
    return EmitOptions{}.max_cols(max);
}

inline void test_container_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_FALSE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml1());
    EXPECT_FALSE(n.type().is_flow_mln());
    EXPECT_FALSE(n.type().is_flow_mlx());
}

inline void test_container_block(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_TRUE(n.type().is_block());
    EXPECT_FALSE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml1());
    EXPECT_FALSE(n.type().is_flow_mln());
    EXPECT_FALSE(n.type().is_flow_mlx());
}

inline void test_container_flow_sl(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_TRUE(n.type().is_flow());
    EXPECT_TRUE(n.type().is_flow_sl());
    EXPECT_FALSE(n.type().is_flow_ml1());
    EXPECT_FALSE(n.type().is_flow_mln());
    EXPECT_FALSE(n.type().is_flow_mlx());
}

inline void test_container_flow_ml(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_container_styled());
    EXPECT_FALSE(n.type().is_block());
    EXPECT_TRUE(n.type().is_flow());
    EXPECT_FALSE(n.type().is_flow_sl());
    EXPECT_TRUE(n.type().is_flow_ml1() ||
                n.type().is_flow_mln());
    EXPECT_TRUE(n.type().is_flow_mlx());
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
    tree.set_container_style(tree.root_id(), FLOW_SL);
    test_container_flow_sl(tree.rootref());
    tree.set_container_style(tree.root_id(), FLOW_ML1);
    test_container_flow_ml(tree.rootref());
    tree.set_container_style(tree.root_id(), FLOW_MLN);
    test_container_flow_ml(tree.rootref());

    NodeRef r = tree;
    r.clear_style();
    r.set_container_style(FLOW_SL);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_TRUE(r.is_flow_sl());
    EXPECT_FALSE(r.is_flow_ml1());
    EXPECT_FALSE(r.is_flow_mln());
    EXPECT_FALSE(r.is_flow_mlx());
    EXPECT_FALSE(r.has_flow_space());
    //
    r.set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_TRUE(r.is_flow_sl());
    EXPECT_FALSE(r.is_flow_ml1());
    EXPECT_FALSE(r.is_flow_mln());
    EXPECT_FALSE(r.is_flow_mlx());
    EXPECT_TRUE(r.has_flow_space());
    //
    r.set_container_style(FLOW_ML1);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_FALSE(r.is_flow_sl());
    EXPECT_TRUE(r.is_flow_ml1());
    EXPECT_FALSE(r.is_flow_mln());
    EXPECT_TRUE(r.is_flow_mlx());
    EXPECT_FALSE(r.has_flow_space());
    //
    r.set_container_style(FLOW_ML1|FLOW_SPC);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_FALSE(r.is_flow_sl());
    EXPECT_TRUE(r.is_flow_ml1());
    EXPECT_FALSE(r.is_flow_mln());
    EXPECT_TRUE(r.is_flow_mlx());
    EXPECT_TRUE(r.has_flow_space());
    //
    r.set_container_style(FLOW_MLN);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_FALSE(r.is_flow_sl());
    EXPECT_FALSE(r.is_flow_ml1());
    EXPECT_TRUE(r.is_flow_mln());
    EXPECT_TRUE(r.is_flow_mlx());
    EXPECT_FALSE(r.has_flow_space());
    //
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_FALSE(r.is_block());
    EXPECT_TRUE(r.is_flow());
    EXPECT_FALSE(r.is_flow_sl());
    EXPECT_FALSE(r.is_flow_ml1());
    EXPECT_TRUE(r.is_flow_mln());
    EXPECT_TRUE(r.is_flow_mlx());
    EXPECT_TRUE(r.has_flow_space());
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
        n.create();
        n.tree()->_add_flags(n.id(), t);
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
    tree[0].set_key_style(KEY_LITERAL);
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
    tree[0].set_val_style(VAL_LITERAL);
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
    tree[0].set_key_style(KEY_LITERAL);
    tree[0].set_val_style(VAL_LITERAL);
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
    tree[0].set_key_style(KEY_FOLDED);
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
    tree[0].set_val_style(VAL_FOLDED);
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
    tree[0].set_key_style(KEY_FOLDED);
    tree[0].set_val_style(VAL_FOLDED);
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
    tree[0].set_key_style(KEY_SQUO);
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
    tree[0].set_val_style(VAL_SQUO);
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
    tree[0].set_key_style(KEY_SQUO);
    tree[0].set_val_style(VAL_SQUO);
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
    NodeRef r = tree;
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(seq, block)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(BLOCK);
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
    NodeRef r = tree;
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"([1,2,3,4,5,6])");
    r.set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"([1, 2, 3, 4, 5, 6])");
}

static void test_seq_flow_ml1(NodeType extra={}) // NOLINT
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_ML1|extra);
    EXPECT_EQ(emit2str(tree),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(0)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(1)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(100)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
}
TEST(seq, flow_ml1)
{
    test_seq_flow_ml1();
}
TEST(seq, flow_ml1_spc)
{
    test_seq_flow_ml1(FLOW_SPC);
}

TEST(seq, flow_mln)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree),
              "[\n"
              "  1,2,3,4,5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(3)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(4)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(5)),
              "[\n"
              "  1,2,\n"
              "  3,4,\n"
              "  5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(6)),
              "[\n"
              "  1,2,\n"
              "  3,4,\n"
              "  5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(7)),
              "[\n"
              "  1,2,3,\n"
              "  4,5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(8)),
              "[\n"
              "  1,2,3,\n"
              "  4,5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(9)),
              "[\n"
              "  1,2,3,4,\n"
              "  5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "[\n"
              "  1,2,3,4,\n"
              "  5,6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(11)),
              "[\n"
              "  1,2,3,4,5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(12)),
              "[\n"
              "  1,2,3,4,5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(13)),
              "[\n"
              "  1,2,3,4,5,6\n"
              "]\n");
}

TEST(seq, flow_mln_spc)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "[\n"
              "  1, 2, 3, 4, 5, 6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(4)),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(6)),
              "[\n"
              "  1, 2,\n"
              "  3, 4,\n"
              "  5, 6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(8)),
              "[\n"
              "  1, 2,\n"
              "  3, 4,\n"
              "  5, 6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(9)),
              "[\n"
              "  1, 2, 3,\n"
              "  4, 5, 6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(12)),
              "[\n"
              "  1, 2, 3, 4,\n"
              "  5, 6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(15)),
              "[\n"
              "  1, 2, 3, 4, 5,\n"
              "  6\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(18)),
              "[\n"
              "  1, 2, 3, 4, 5, 6\n"
              "]\n");
}

TEST(seq, flow_ml_nested_1_ml1)
{
    Tree tree = parse_in_arena("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    NodeRef r = tree;
    EXPECT_EQ(emit2str(tree), "[[1,2,3,4,5,6],10,20,25,[100,200,300,400],30,40,50,[7,8,9,10,11,12]]");
    r.set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), "[[1,2,3,4,5,6], 10, 20, 25, [100,200,300,400], 30, 40, 50, [7,8,9,10,11,12]]");
    r.set_container_style(FLOW_SL);
    r[0].set_container_style(FLOW_SL|FLOW_SPC);
    r[4].set_container_style(FLOW_SL|FLOW_SPC);
    r[8].set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), "[[1, 2, 3, 4, 5, 6],10,20,25,[100, 200, 300, 400],30,40,50,[7, 8, 9, 10, 11, 12]]");
    r[0].set_container_style(FLOW_SL);
    r[4].set_container_style(FLOW_SL);
    r[8].set_container_style(FLOW_SL);
    r.set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree),
              "[\n"
              "  [1,2,3,4,5,6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,200,300,400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,8,9,10,11,12]\n"
              "]\n");
    r.set_container_style(FLOW_ML1|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "[\n"
              "  [1,2,3,4,5,6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,200,300,400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,8,9,10,11,12]\n"
              "]\n");
}

TEST(seq, flow_ml_nested_2_mln)
{
    Tree tree = parse_in_arena("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree, maxcols(0)),
              "[\n"
              "  [1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,\n"
              "  200,\n"
              "  300,\n"
              "  400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,\n"
              "  8,\n"
              "  9,\n"
              "  10,\n"
              "  11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(1)),
              "[\n"
              "  [1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,\n"
              "  200,\n"
              "  300,\n"
              "  400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,\n"
              "  8,\n"
              "  9,\n"
              "  10,\n"
              "  11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "[\n"
              "  [1,2,3,4,\n"
              "  5,6],10,\n"
              "  20,25,[100,\n"
              "  200,300,\n"
              "  400],30,\n"
              "  40,50,[7,\n"
              "  8,9,10,11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(16)),
              "[\n"
              "  [1,2,3,4,5,6],\n"
              "  10,20,25,[100,\n"
              "  200,300,400],30,\n"
              "  40,50,[7,8,9,10,\n"
              "  11,12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "[\n"
              "  [1,2,3,4,5,6],10,20,\n"
              "  25,[100,200,300,400],\n"
              "  30,40,50,[7,8,9,10,\n"
              "  11,12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(24)),
              "[\n"
              "  [1,2,3,4,5,6],10,20,25,\n"
              "  [100,200,300,400],30,40,\n"
              "  50,[7,8,9,10,11,12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(30)),
              "[\n"
              "  [1,2,3,4,5,6],10,20,25,[100,\n"
              "  200,300,400],30,40,50,[7,8,9,\n"
              "  10,11,12]\n"
              "]\n");
}

TEST(seq, flow_ml_nested_2_mln_spc)
{
    Tree tree = parse_in_arena("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree, maxcols(0)),
              "[\n"
              "  [1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,\n"
              "  200,\n"
              "  300,\n"
              "  400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,\n"
              "  8,\n"
              "  9,\n"
              "  10,\n"
              "  11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(1)),
              "[\n"
              "  [1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [100,\n"
              "  200,\n"
              "  300,\n"
              "  400],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [7,\n"
              "  8,\n"
              "  9,\n"
              "  10,\n"
              "  11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "[\n"
              "  [1, 2, 3,\n"
              "  4, 5, 6],\n"
              "  10, 20,\n"
              "  25, [100,\n"
              "  200, 300,\n"
              "  400], 30,\n"
              "  40, 50,\n"
              "  [7, 8, 9,\n"
              "  10, 11,\n"
              "  12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(16)),
              "[\n"
              "  [1, 2, 3, 4, 5,\n"
              "  6], 10, 20, 25,\n"
              "  [100, 200, 300,\n"
              "  400], 30, 40,\n"
              "  50, [7, 8, 9,\n"
              "  10, 11, 12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "[\n"
              "  [1, 2, 3, 4, 5, 6],\n"
              "  10, 20, 25, [100,\n"
              "  200, 300, 400], 30,\n"
              "  40, 50, [7, 8, 9,\n"
              "  10, 11, 12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(24)),
              "[\n"
              "  [1, 2, 3, 4, 5, 6], 10,\n"
              "  20, 25, [100, 200, 300,\n"
              "  400], 30, 40, 50, [7,\n"
              "  8, 9, 10, 11, 12]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(30)),
              "[\n"
              "  [1, 2, 3, 4, 5, 6], 10, 20,\n"
              "  25, [100, 200, 300, 400], 30,\n"
              "  40, 50, [7, 8, 9, 10, 11, 12]\n"
              "]\n");
}

TEST(seq, flow_ml_nested_2_mln_spc_nested)
{
    Tree tree = parse_in_arena("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    r[0].set_container_style(FLOW_MLN|FLOW_SPC);
    r[4].set_container_style(FLOW_MLN|FLOW_SPC);
    r[8].set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree, maxcols(0)),
              "[\n"
              "  [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [\n"
              "    100,\n"
              "    200,\n"
              "    300,\n"
              "    400\n"
              "  ],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [\n"
              "    7,\n"
              "    8,\n"
              "    9,\n"
              "    10,\n"
              "    11,\n"
              "    12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(1)),
              "[\n"
              "  [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ],\n"
              "  10,\n"
              "  20,\n"
              "  25,\n"
              "  [\n"
              "    100,\n"
              "    200,\n"
              "    300,\n"
              "    400\n"
              "  ],\n"
              "  30,\n"
              "  40,\n"
              "  50,\n"
              "  [\n"
              "    7,\n"
              "    8,\n"
              "    9,\n"
              "    10,\n"
              "    11,\n"
              "    12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "[\n"
              "  [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ], 10, 20,\n"
              "  25, [\n"
              "    100, 200,\n"
              "    300, 400\n"
              "  ], 30, 40,\n"
              "  50, [\n"
              "    7, 8,\n"
              "    9, 10,\n"
              "    11, 12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(16)),
              "[\n"
              "  [\n"
              "    1, 2, 3, 4,\n"
              "    5, 6\n"
              "  ], 10, 20, 25,\n"
              "  [\n"
              "    100, 200, 300,\n"
              "    400\n"
              "  ], 30, 40, 50,\n"
              "  [\n"
              "    7, 8, 9, 10,\n"
              "    11, 12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "[\n"
              "  [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ], 10, 20, 25, [\n"
              "    100, 200, 300, 400\n"
              "  ], 30, 40, 50, [\n"
              "    7, 8, 9, 10, 11,\n"
              "    12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(24)),
              "[\n"
              "  [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ], 10, 20, 25, [\n"
              "    100, 200, 300, 400\n"
              "  ], 30, 40, 50, [\n"
              "    7, 8, 9, 10, 11, 12\n"
              "  ]\n"
              "]\n");
    EXPECT_EQ(emit2str(tree, maxcols(30)),
              "[\n"
              "  [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ], 10, 20, 25, [\n"
              "    100, 200, 300, 400\n"
              "  ], 30, 40, 50, [\n"
              "    7, 8, 9, 10, 11, 12\n"
              "  ]\n"
              "]\n");
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
    tree._add_flags(tree["foo"].id(), FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"(foo: [1, 2, 3, 4, 5, 6]
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
    tree._add_flags(tree["foo"].id(), FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"(foo: [1, [2,3], 4, [5,6]]
)");
    tree._add_flags(tree["foo"][1].id(), FLOW_SPC);
    tree._add_flags(tree["foo"][3].id(), FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"(foo: [1, [2, 3], 4, [5, 6]]
)");
    tree._rem_flags(tree["foo"].id(), CONTAINER_STYLE);
    tree._add_flags(tree["foo"].id(), BLOCK);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - [2, 3]
  - 4
  - [5, 6]
)");
    tree._rem_flags(tree["foo"][1].id(), FLOW_SPC);
    tree._rem_flags(tree["foo"][3].id(), FLOW_SPC);
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

TEST(keyseq, flow_ml1)
{
    Tree tree = parse_in_arena("foo: [1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [1,2,3,4,5,6]\n"
              "}\n");
    r.set_container_style(FLOW_ML1|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [1,2,3,4,5,6]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
}

TEST(keyseq, flow_mln_0)
{
    Tree tree = parse_in_arena("foo: [1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_ML1);
    r["foo"].set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(5)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(6)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(7)),
              "{\n"
              "  foo: [\n"
              "    1,2,\n"
              "    3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(8)),
              "{\n"
              "  foo: [\n"
              "    1,2,\n"
              "    3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(9)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,\n"
              "    4,5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,\n"
              "    4,5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(11)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(12)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(13)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(14)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(15)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
}

TEST(keyseq, flow_mln_1)
{
    Tree tree = parse_in_arena("foo: [1, 2, 3, 4, 5, 6]");
    NodeRef r = tree;
    r.set_container_style(FLOW_ML1);
    r["foo"].set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(5)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(6)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(7)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    2,\n"
              "    3,\n"
              "    4,\n"
              "    5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(8)),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(9)),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(10)),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(11)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(12)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(13)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(14)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(17)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
}

TEST(keyseq, flow_mln_nested)
{
    Tree tree = parse_in_arena("foo: [1, [2, 3], 4, [5, 6]]");
    NodeRef r = tree;
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,[2,3],4,[5,6]]})");
    r.set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [1,[2,3],4,[5,6]]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1,[2,3],4,[5,6]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(5)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    [2,\n"
              "    3],\n"
              "    4,\n"
              "    [5,\n"
              "    6]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(7)),
              "{\n"
              "  foo: [\n"
              "    1,[2,\n"
              "    3],\n"
              "    4,[5,\n"
              "    6]\n"
              "  ]\n"
              "}\n");
    //
    r.set_container_style(BLOCK);
    r["foo"].set_container_style(BLOCK);
    r["foo"][1].set_container_style(FLOW_SL);
    r["foo"][3].set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
}

TEST(keyseq, flow_mln_nested_2)
{
    Tree tree = parse_in_arena("foo: [1, [2, 3, 20, 30, 40], 4, [5, 6, 70, 80, 90]]");
    NodeRef r = tree;
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,[2,3,20,30,40],4,[5,6,70,80,90]]})");
    r.set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [1,[2,3,20,30,40],4,[5,6,70,80,90]]\n"
              "}\n");
    r["foo"].set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,30,40],4,[5,6,70,80,90]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(5)),
              "{\n"
              "  foo: [\n"
              "    1,\n"
              "    [2,\n"
              "    3,\n"
              "    20,\n"
              "    30,\n"
              "    40],\n"
              "    4,\n"
              "    [5,\n"
              "    6,\n"
              "    70,\n"
              "    80,\n"
              "    90]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(7)),
              "{\n"
              "  foo: [\n"
              "    1,[2,\n"
              "    3,20,\n"
              "    30,\n"
              "    40],\n"
              "    4,[5,\n"
              "    6,70,\n"
              "    80,\n"
              "    90]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(12)),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,\n"
              "    30,40],4,\n"
              "    [5,6,70,\n"
              "    80,90]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,30,40],\n"
              "    4,[5,6,70,80,90]\n"
              "  ]\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(25)),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,30,40],4,[5,\n"
              "    6,70,80,90]\n"
              "  ]\n"
              "}\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(map, block)
{
    Tree tree = parse_in_arena("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    NodeRef r = tree;
    r.set_container_style(BLOCK);
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
    NodeRef r = tree;
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
    r.set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"({1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10})");
    r.set_container_style(FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
}

TEST(map, flow_ml_1)
{
    Tree tree = parse_in_arena(R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
7: 10
8: 10
9: 10
10: 10
11: 10
12: 10
)");
    NodeRef r = tree;
    r.set_container_style(FLOW_ML1);
    EXPECT_EQ(emit2str(tree), ""
              "{\n"
              "  1: 10,\n"
              "  2: 10,\n"
              "  3: 10,\n"
              "  4: 10,\n"
              "  5: 10,\n"
              "  6: 10,\n"
              "  7: 10,\n"
              "  8: 10,\n"
              "  9: 10,\n"
              "  10: 10,\n"
              "  11: 10,\n"
              "  12: 10\n"
              "}\n");
    r.set_container_style(FLOW_ML1|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), ""
              "{\n"
              "  1: 10,\n"
              "  2: 10,\n"
              "  3: 10,\n"
              "  4: 10,\n"
              "  5: 10,\n"
              "  6: 10,\n"
              "  7: 10,\n"
              "  8: 10,\n"
              "  9: 10,\n"
              "  10: 10,\n"
              "  11: 10,\n"
              "  12: 10\n"
              "}\n");
}

TEST(map, flow_ml_n)
{
    Tree tree = parse_in_arena(R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
7: 10
8: 10
9: 10
10: 10
11: 10
12: 10
)");
    NodeRef r = tree;
    r.set_container_style(FLOW_MLN);
    EXPECT_EQ(emit2str(tree), ""
              "{\n"
              "  1: 10,2: 10,3: 10,4: 10,5: 10,6: 10,7: 10,8: 10,9: 10,10: 10,11: 10,12: 10\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(40)), ""
              "{\n"
              "  1: 10,2: 10,3: 10,4: 10,5: 10,6: 10,7: 10,\n"
              "  8: 10,9: 10,10: 10,11: 10,12: 10\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)), ""
              "{\n"
              "  1: 10,2: 10,3: 10,\n"
              "  4: 10,5: 10,6: 10,\n"
              "  7: 10,8: 10,9: 10,\n"
              "  10: 10,11: 10,12: 10\n"
              "}\n");
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), ""
              "{\n"
              "  1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10, 7: 10, 8: 10, 9: 10, 10: 10, 11: 10,\n"
              "  12: 10\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(40)), ""
              "{\n"
              "  1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10,\n"
              "  7: 10, 8: 10, 9: 10, 10: 10, 11: 10, 12: 10\n"
              "}\n");
    EXPECT_EQ(emit2str(tree, maxcols(20)), ""
              "{\n"
              "  1: 10, 2: 10, 3: 10,\n"
              "  4: 10, 5: 10, 6: 10,\n"
              "  7: 10, 8: 10, 9: 10,\n"
              "  10: 10, 11: 10, 12: 10\n"
              "}\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keymap, block)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    NodeRef r = tree;
    r.set_container_style(BLOCK);
    EXPECT_EQ(emit2str(tree), "foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}\n");
    r["foo"].set_container_style(FLOW_SL|FLOW_SPC);
    EXPECT_EQ(emit2str(tree), "foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}\n");
    r["foo"].set_container_style(BLOCK);
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
    tree._rem_flags(tree["foo"][1].id(), BLOCK);
    tree._add_flags(tree["foo"][1].id(), FLOW_SL);
    tree._rem_flags(tree["foo"][3].id(), BLOCK);
    tree._add_flags(tree["foo"][3].id(), FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: {2: 10,3: 10}
  4: 10
  5: {5: 10,6: 10}
)");
    tree._add_flags(tree["foo"][1].id(), FLOW_SPC);
    tree._add_flags(tree["foo"][3].id(), FLOW_SPC);
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: {2: 10, 3: 10}
  4: 10
  5: {5: 10, 6: 10}
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
