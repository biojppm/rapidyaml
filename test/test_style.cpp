#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/extra/event_ints.hpp>
#include <c4/yml/extra/emitter_ints.hpp>
#include <c4/yml/extra/emitter_ints.def.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif

#include "./test_lib/tree_and_ints.hpp"
#include "./test_lib/test_case.hpp"
#include "./test_lib/test_events_ints_helpers.hpp"

#include <gtest/gtest.h>

RYML_DEFINE_TEST_MAIN()

// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)

namespace c4 {
namespace yml {
namespace xievt = extra::ievt;

using xievt::evt_size;
using xievt::evt_bits;
constexpr const xievt::evt_bits all_styles_container = xievt::BLCK|xievt::FLOW|xievt::FSL_|xievt::FML1|xievt::FMLN|xievt::FSPC; // NOLINT
constexpr const xievt::evt_bits all_styles_scalar = xievt::PLAI|xievt::SQUO|xievt::DQUO|xievt::LITL|xievt::FOLD; // NOLINT
constexpr const xievt::evt_bits all_styles_ievt = all_styles_container|all_styles_scalar;
constexpr const xievt::evt_bits all_styles_ievtkv = all_styles_ievt|xievt::SCLR|xievt::KEY_|xievt::VAL_;

EmitOptions maxcols(id_type max)
{
    return EmitOptions{}.max_cols(max);
}

size_t emit2buf(substr s, IntBufs const& buf, EmitOptions const& opts={})
{
    xievt::EmitterInts<WriterBuf> e(opts, s);
    e.emit_as(EMIT_YAML, buf.evts.ptr, buf.evts.len, buf.src, buf.arena);
    return e.m_pos;
}
std::string emit2str(IntBufs const& buf, EmitOptions const& opts={})
{
    std::string s;
    s.resize((3 * buf.src.len) / 2);
again:
    size_t len = emit2buf(to_substr(s), buf, opts);
    bool ok = len <= s.size();
    s.resize(len);
    if(!ok)
        goto again; // NOLINT
    return s;
}
std::string emit2str(Tree const& t, EmitOptions const& opts={})
{
    return emitrs_yaml<std::string>(t, opts);
}


//-----------------------------------------------------------------------------


void set_style(TreeAndInts *ti,
               NodeRef which_node, NodeType node_style,
               evt_size which_evt, evt_bits evt_style)
{
    ASSERT_LT(which_evt, ti->ints.evts.len);
    which_node.set_container_style(node_style);
    (ti->ints.evts.ptr[which_evt] &= ~all_styles_container) |= evt_style;
}
void add_style(TreeAndInts *ti,
               NodeRef which_node, NodeType node_style,
               evt_size which_evt, evt_bits evt_style)
{
    ASSERT_LT(which_evt, ti->ints.evts.len);
    which_node.tree()->_add_flags(which_node.id(), node_style);
    ti->ints.evts.ptr[which_evt] |= evt_style;
}
void rem_style(TreeAndInts *ti,
               NodeRef which_node, NodeType node_style,
               evt_size which_evt, evt_bits evt_style)
{
    ASSERT_LT(which_evt, ti->ints.evts.len);
    which_node.tree()->_rem_flags(which_node.id(), node_style);
    ti->ints.evts.ptr[which_evt] &= ~evt_style;
}


//-----------------------------------------------------------------------------

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
inline void test_container_block(IntBufsCR buf, evt_size pos)
{
    EXPECT_EQ(buf.evts.ptr[pos] & (all_styles_ievt|xievt::BEG_), xievt::BLCK|xievt::BEG_);
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
inline void test_container_flow_sl(IntBufsCR buf, evt_size pos)
{
    EXPECT_EQ(buf.evts.ptr[pos] & (all_styles_ievt|xievt::BEG_), xievt::FLOW|xievt::FSL_);
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
inline void test_container_flow_ml(IntBufsCR buf, evt_size pos)
{
    const evt_bits bits = buf.evts.ptr[pos] & (all_styles_ievt|xievt::BEG_);
    EXPECT_TRUE((bits == (xievt::FLOW|xievt::FML1))
                ||
                (bits == (xievt::FLOW|xievt::FMLN)));
}

#define test_int_bits(buf, pos, mask, expected)                 \
    do {                                                        \
        RYML_TRACE_FMT("pos={} len={}", pos, buf.evts.len);     \
        ASSERT_LE(pos, buf.evts.len);                           \
        if((buf.evts.ptr[pos] & (mask)) != (expected))          \
        {                                                       \
            EXPECT_EQ(buf.evts.ptr[pos] & (mask), expected);    \
            buf.print();                                        \
        }                                                       \
    } while(0)

inline void test_key_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.type().is_key_styled());
    EXPECT_TRUE(n.type().is_key_plain());
    EXPECT_FALSE(n.type().is_key_squo());
    EXPECT_FALSE(n.type().is_key_dquo());
    EXPECT_FALSE(n.type().is_key_literal());
    EXPECT_FALSE(n.type().is_key_folded());
}
inline void test_key_plain(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::KEY_|xievt::SCLR|xievt::PLAI);
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
inline void test_val_plain(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::VAL_|xievt::SCLR|xievt::PLAI);
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
inline void test_key_squo(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::KEY_|xievt::SCLR|xievt::SQUO);
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
inline void test_val_squo(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::VAL_|xievt::SCLR|xievt::SQUO);
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
inline void test_key_dquo(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::KEY_|xievt::SCLR|xievt::DQUO);
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
inline void test_val_dquo(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::VAL_|xievt::SCLR|xievt::DQUO);
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
inline void test_key_literal(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::KEY_|xievt::SCLR|xievt::LITL);
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
inline void test_val_literal(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::VAL_|xievt::SCLR|xievt::LITL);
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
inline void test_key_folded(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::KEY_|xievt::SCLR|xievt::FOLD);
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
inline void test_val_folded(IntBufsCR buf, evt_size pos)
{
    test_int_bits(buf, pos, all_styles_ievtkv, xievt::VAL_|xievt::SCLR|xievt::FOLD);
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

void check_same_emit4(Tree const& expected, std::string const* expected_yaml=nullptr)
{
    std::string ws1, ws2, ws3, ws4;
    Tree actual1, actual2, actual3, actual4;
    bool did2 = false, did3 = false, did4 = false;
    emitrs_yaml(expected, &ws1);
    if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws1); }
    {
        SCOPED_TRACE("actual1");
        parse_in_arena(to_csubstr(ws1), &actual1);
        test_compare(actual1, expected);
        emitrs_yaml(actual1, &ws2);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws2); }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual2");
        parse_in_arena(to_csubstr(ws2), &actual2);
        test_compare(actual2, expected);
        emitrs_yaml(actual2, &ws3);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws3); }
        did2 = true;
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual3");
        parse_in_arena(to_csubstr(ws3), &actual3);
        test_compare(actual3, expected);
        emitrs_yaml(actual3, &ws4);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws4); }
        did3 = true;
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual4");
        parse_in_arena(to_csubstr(ws4), &actual4);
        test_compare(actual4, expected);
        did4 = true;
    }
    if(testing::Test::HasFailure())
    {
        auto showtrees_ = [&expected](size_t num, std::string const& ws, Tree const& actual)
        {
            std::cout << "--------\nEMITTED" << num << "\n--------\n";
            std::cout << ws;
            std::cout << "--------\nACTUAL" << num << "\n--------\n";
            print_tree(actual);
            std::cout << "--------\nEXPECTED" << num << "\n--------\n";
            print_tree(expected);
        };
        showtrees_(1, ws1, actual1);
        if(did2) showtrees_(2, ws2, actual2);
        if(did3) showtrees_(3, ws3, actual3);
        if(did4) showtrees_(4, ws4, actual4);
    }
}
void check_same_emit4(IntBufsCR& expected, std::string const* expected_yaml=nullptr)
{
    std::string ws1, ws2, ws3, ws4;
    IntBufs actual1, actual2, actual3, actual4;
    bool did2 = false, did3 = false, did4 = false;
    ws1 = emit2str(expected);
    if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws1); }
    {
        SCOPED_TRACE("actual1");
        parse_ints(to_substr(ws1), &actual1);
        xievt::test_events_ints_compare(expected, actual1);
        ws2 = emit2str(actual1);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws2); }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual2");
        parse_ints(to_substr(ws2), &actual2);
        xievt::test_events_ints_compare(expected, actual2);
        ws3 = emit2str(actual2);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws3); }
        did2 = true;
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual3");
        parse_ints(to_substr(ws3), &actual3);
        xievt::test_events_ints_compare(expected, actual3);
        ws4 = emit2str(actual3);
        if(expected_yaml) { EXPECT_EQ(*expected_yaml, ws4); }
        did3 = true;
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("actual4");
        parse_ints(to_substr(ws4), &actual4);
        xievt::test_events_ints_compare(expected, actual4);
        did4 = true;
    }
    if(testing::Test::HasFailure())
    {
        auto showtrees_ = [&expected](size_t num, std::string const& ws, IntBufsCR actual)
        {
            std::cout << "--------\nEMITTED" << num << "\n--------\n";
            std::cout << ws;
            std::cout << "--------\nACTUAL" << num << "\n--------\n";
            actual.print();
            std::cout << "--------\nEXPECTED" << num << "\n--------\n";
            expected.print();
        };
        showtrees_(1, ws1, actual1);
        if(did2) showtrees_(2, ws2, actual2);
        if(did3) showtrees_(3, ws3, actual3);
        if(did4) showtrees_(4, ws4, actual4);
    }
}
void check_same_emit4(TreeAndInts const& ti, std::string const* expected_yaml)
{
    check_same_emit4(ti.tree, expected_yaml);
    check_same_emit4(ti.ints, expected_yaml);
}
void check_same_emit4(TreeAndInts const& ti, std::string const& expected_yaml={})
{
    check_same_emit4(ti.tree, &expected_yaml);
    check_same_emit4(ti.ints, &expected_yaml);
}

void check_same_emit1(Tree const& tree, std::string const& expected, EmitOptions const& opts={})
{
    SCOPED_TRACE("check_same_emit1");
    test_invariants(tree);
    std::string actual;
    emitrs_yaml(tree, opts, &actual);
    EXPECT_EQ(expected, actual);
    if(testing::Test::HasFailure())
        print_tree(tree);
}
void check_same_emit1(IntBufsCR& ints, std::string const& expected, EmitOptions const& opts={})
{
    SCOPED_TRACE("check_same_emit1");
    std::string actual = emit2str(ints, opts);
    xievt::test_events_ints_invariants(ints.src, ints.arena, ints.evts.ptr, ints.evts.len);
    EXPECT_EQ(expected, actual);
    if(testing::Test::HasFailure())
        ints.print();
}
void check_same_emit1(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={})
{
    SCOPED_TRACE("check_same_emit1");
    check_same_emit1(ti.tree, expected, opts);
    check_same_emit1(ti.ints, expected, opts);
}
void check_same_emit1(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected)
{
    SCOPED_TRACE("check_same_emit1");
    check_same_emit1(ti.tree, expected, opts);
    check_same_emit1(ti.ints, expected, opts);
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
        ch.set_val(val);
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
            sl["space"].set_val(" foo");
            sl["tab"].set_val("\tfoo");
            sl["space_and_tab0"].set_val(" \tfoo");
            sl["space_and_tab1"].set_val("\t foo");
        }
        {
            NodeRef ml = setcont(n["multiline"], MAP);
            ml["beg_________"].set_val("\n \tfoo");
            ml["beg_mid_____"].set_val("\n \tfoo\nbar");
            ml["beg_mid_end1"].set_val("\n \tfoo\nbar\n");
            ml["beg_mid_end2"].set_val("\n \tfoo\nbar\n\n");
            ml["beg_mid_end3"].set_val("\n \tfoo\nbar\n\n\n");
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
    csubstr yaml = "foo";
    RYML_TRACE_FMT("yaml={}", yaml);
    TreeAndInts ti = parse_tree_and_ints(yaml);
    test_val_plain(ti.tree.rootref());
    test_val_plain(ti.ints, 2);
    test_emit(ti, "foo\n");
}

TEST(style, scalar_retains_style_after_parse__squo)
{
    csubstr yaml = "'foo'";
    RYML_TRACE_FMT("yaml={}", yaml);
    TreeAndInts ti = parse_tree_and_ints(yaml);
    test_val_squo(ti.tree.rootref());
    test_val_squo(ti.ints, 2);
    test_emit(ti, "'foo'\n");
}

TEST(style, scalar_retains_style_after_parse__dquo)
{
    csubstr yaml = "\"foo\"";
    RYML_TRACE_FMT("yaml={}", yaml);
    TreeAndInts ti = parse_tree_and_ints(yaml);
    test_val_dquo(ti.tree.rootref());
    test_val_dquo(ti.ints, 2);
    test_emit(ti, "\"foo\"\n");
}

TEST(style, scalar_retains_style_after_parse__literal)
{
    csubstr yaml = "|\n foo";
    RYML_TRACE_FMT("yaml={}", yaml);
    TreeAndInts ti = parse_tree_and_ints(yaml);
    test_val_literal(ti.tree.rootref());
    test_val_literal(ti.ints, 2);
    test_emit(ti, "|\n  foo\n");
}

TEST(style, scalar_retains_style_after_parse__folded)
{
    csubstr yaml = ">\n foo";
    RYML_TRACE_FMT("yaml={}", yaml);
    TreeAndInts ti = parse_tree_and_ints(yaml);
    test_val_folded(ti.tree.rootref());
    test_val_folded(ti.ints, 2);
    test_emit(ti, ">\n  foo\n");
}

TEST(style, scalar_retains_style_after_parse__mixed)
{
    std::string yaml = "- foo\n- 'baz'\n- \"bat\"\n- |\n  baq\n- >\n  bax\n";
    RYML_TRACE_FMT("yaml=~~~\n{}~~~", yaml);
    TreeAndInts ti = parse_tree_and_ints(to_csubstr(yaml));
    test_container_block(ti.tree.rootref());
    test_container_block(ti.ints, 2);
    test_val_plain(ti.tree[0]);
    test_val_plain(ti.ints, 3);
    test_val_squo(ti.tree[1]);
    test_val_squo(ti.ints, 6);
    test_val_dquo(ti.tree[2]);
    test_val_dquo(ti.ints, 9);
    test_val_literal(ti.tree[3]);
    test_val_literal(ti.ints, 12);
    test_val_folded(ti.tree[4]);
    test_val_folded(ti.ints, 15);
    test_emit(ti, yaml);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(scalar, base)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    EXPECT_EQ(ti.tree[0].key(), csubstr("this is the key"));
    EXPECT_EQ(ti.ints.getstr(3), csubstr("this is the key"));
    EXPECT_EQ(ti.tree[0].val(), csubstr("this is the multiline \"val\" with\n'empty' lines"));
    EXPECT_EQ(ti.ints.getstr(6), csubstr("this is the multiline \"val\" with\n'empty' lines"));
    check_same_emit4(ti, R"(this is the key: >-
  this is the multiline "val" with

  'empty' lines
)");
}


//-----------------------------------------------------------------------------

TEST(scalar, block_literal__key)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_LITERAL);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::LITL;
    test_key_literal(ti.tree[0]);
    test_key_literal(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    check_same_emit4(ti, R"(? |-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)");
}

TEST(scalar, block_literal__val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_val_style(VAL_LITERAL);
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::LITL;
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_literal(ti.tree[0]);
    test_val_literal(ti.ints, 6);
    check_same_emit4(ti, R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
}

TEST(scalar, block_literal__key_val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_LITERAL);
    ti.tree[0].set_val_style(VAL_LITERAL);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::LITL;
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::LITL;
    test_key_literal(ti.tree[0]);
    test_key_literal(ti.ints, 3);
    test_key_literal(ti.ints, 3);
    test_val_literal(ti.tree[0]);
    test_val_literal(ti.ints, 6);
    check_same_emit4(ti, R"(? |-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
}


//-----------------------------------------------------------------------------

TEST(scalar, block_folded__key)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_FOLDED);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::FOLD;
    test_key_folded(ti.tree[0]);
    test_key_folded(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    const std::string expected_yaml = R"(? >-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)";
    check_same_emit4(ti, &expected_yaml);
}

TEST(scalar, block_folded__val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_val_style(VAL_FOLDED);
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::FOLD;
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    const std::string expected_yaml = R"(this is the key: >-
  this is the multiline "val" with

  'empty' lines
)";
    check_same_emit4(ti, &expected_yaml);
}

TEST(scalar, block_folded__key_val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_FOLDED);
    ti.tree[0].set_val_style(VAL_FOLDED);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::FOLD;
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::FOLD;
    test_key_folded(ti.tree[0]);
    test_key_folded(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    const std::string expected_yaml = R"(? >-
  this is the key
: >-
  this is the multiline "val" with

  'empty' lines
)";
    check_same_emit4(ti, &expected_yaml);
}


//-----------------------------------------------------------------------------

TEST(scalar, squo__key)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_SQUO);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::SQUO;
    test_key_squo(ti.tree[0]);
    test_key_squo(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    const std::string expected_yaml = R"('this is the key': >-
  this is the multiline "val" with

  'empty' lines
)";
    check_same_emit4(ti, &expected_yaml);
}

TEST(scalar, squo__val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_val_style(VAL_SQUO);
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::SQUO;
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_squo(ti.tree[0]);
    test_val_squo(ti.ints, 6);
    const std::string expected_yaml = R"(this is the key: 'this is the multiline "val" with

  ''empty'' lines'
)";
    check_same_emit4(ti, &expected_yaml);
}

TEST(scalar, squo__key_val)
{
    TreeAndInts ti = parse_tree_and_ints(scalar_yaml);
    test_key_plain(ti.tree[0]);
    test_key_plain(ti.ints, 3);
    test_val_folded(ti.tree[0]);
    test_val_folded(ti.ints, 6);
    ti.tree[0].set_key_style(KEY_SQUO);
    ti.tree[0].set_val_style(VAL_SQUO);
    (ti.ints.evts.ptr[3] &= ~all_styles_scalar) |= xievt::SQUO;
    (ti.ints.evts.ptr[6] &= ~all_styles_scalar) |= xievt::SQUO;
    test_key_squo(ti.tree[0]);
    test_key_squo(ti.ints, 3);
    test_val_squo(ti.tree[0]);
    test_val_squo(ti.ints, 6);
    const std::string expected_yaml = R"('this is the key': 'this is the multiline "val" with

  ''empty'' lines'
)";
    check_same_emit4(ti, &expected_yaml);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(stream, block)
{
    TreeAndInts ti = parse_tree_and_ints(R"(
---
scalar
%YAML 1.2
---
foo
---
bar
)");
    EXPECT_TRUE(ti.tree.rootref().is_stream());
    EXPECT_TRUE(ti.tree.docref(0).is_doc());
    EXPECT_TRUE(ti.tree.docref(0).is_val());
    std::string expected = "--- scalar %YAML 1.2\n--- foo\n--- bar\n";
    check_same_emit4(ti, &expected);
    NodeRef r = ti.tree;
    r.set_container_style(FLOW_SL);
    check_same_emit4(ti, &expected);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(seq, block)
{
    TreeAndInts ti = parse_tree_and_ints("[1, 2, 3, 4, 5, 6]");
    check_same_emit4(ti, "[1,2,3,4,5,6]");
    NodeRef r = ti.tree;
    r.set_container_style(BLOCK);
    (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::BLCK;
    check_same_emit4(ti, R"(- 1
- 2
- 3
- 4
- 5
- 6
)");
}

TEST(seq, block_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("['ab', [2,3]]");
    check_same_emit1(ti, "['ab',[2,3]]");
    set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
    check_same_emit1(ti, R"(- 'ab'
- [2,3]
)");
    rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
    rem_style(&ti, ti.tree[1], CONTAINER_STYLE, 6, all_styles_container);
    check_same_emit1(ti, R"(- ab
- - 2
  - 3
)");
}

TEST(seq, flow_sl)
{
    TreeAndInts ti = parse_tree_and_ints("[1, 2, 3, 4, 5, 6]");
    NodeRef r = ti.tree;
    {
        SCOPED_TRACE("flow_sl");
        r.set_container_style(FLOW_SL);
        (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::FLOW|xievt::FSL_;
        check_same_emit1(ti, R"([1,2,3,4,5,6])");
    }
    {
        SCOPED_TRACE("flow_sl|flow_spc");
        r.set_container_style(FLOW_SL|FLOW_SPC);
        (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::FLOW|xievt::FSL_|xievt::FSPC;
        check_same_emit1(ti, R"([1, 2, 3, 4, 5, 6])");
    }
}

TEST(seq, flow_sl_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("['ab', [2,3]]");
    check_same_emit1(ti, "['ab',[2,3]]");
    rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
    rem_style(&ti, ti.tree[1], CONTAINER_STYLE, 6, all_styles_container);
    check_same_emit1(ti, R"([ab,[2,3]])");
}

TEST(seq, flow_ml_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("[\n  'ab',\n  [2,3]\n]\n");
    check_same_emit1(ti, "[\n  'ab',\n  [2,3]\n]\n");
    rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
    rem_style(&ti, ti.tree[1], CONTAINER_STYLE, 6, all_styles_container);
    check_same_emit1(ti, "[\n  ab,\n  [2,3]\n]\n");
}


static void test_seq_flow_ml1(NodeType extra={}, evt_bits extra_bits={}) // NOLINT
{
    TreeAndInts ti = parse_tree_and_ints("[1, 2, 3, 4, 5, 6]");
    NodeRef r = ti.tree;
    r.set_container_style(FLOW_ML1|extra);
    (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::FLOW|xievt::FML1|extra_bits;
    check_same_emit1(ti,
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(0),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(1),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(10),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(100),
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
    TreeAndInts ti = parse_tree_and_ints("[1, 2, 3, 4, 5, 6]");
    NodeRef r = ti.tree;
    r.set_container_style(FLOW_MLN);
    (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::FLOW|xievt::FMLN;
    check_same_emit1(ti,
              "[\n"
              "  1,2,3,4,5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(3),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(4),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(5),
              "[\n"
              "  1,2,\n"
              "  3,4,\n"
              "  5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(6),
              "[\n"
              "  1,2,\n"
              "  3,4,\n"
              "  5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(7),
              "[\n"
              "  1,2,3,\n"
              "  4,5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(8),
              "[\n"
              "  1,2,3,\n"
              "  4,5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(9),
              "[\n"
              "  1,2,3,4,\n"
              "  5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(10),
              "[\n"
              "  1,2,3,4,\n"
              "  5,6\n"
              "]\n");
    check_same_emit1(ti, maxcols(11),
              "[\n"
              "  1,2,3,4,5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(12),
              "[\n"
              "  1,2,3,4,5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(13),
              "[\n"
              "  1,2,3,4,5,6\n"
              "]\n");
}

TEST(seq, flow_mln_spc)
{
    TreeAndInts ti = parse_tree_and_ints("[1, 2, 3, 4, 5, 6]");
    NodeRef r = ti.tree;
    r.set_container_style(FLOW_MLN|FLOW_SPC);
    (ti.ints.evts.ptr[2] &= ~all_styles_container) |= xievt::FLOW|xievt::FMLN|xievt::FSPC;
    check_same_emit1(ti,
              "[\n"
              "  1, 2, 3, 4, 5, 6\n"
              "]\n");
    check_same_emit1(ti, maxcols(4),
              "[\n"
              "  1,\n"
              "  2,\n"
              "  3,\n"
              "  4,\n"
              "  5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(6),
              "[\n"
              "  1, 2,\n"
              "  3, 4,\n"
              "  5, 6\n"
              "]\n");
    check_same_emit1(ti, maxcols(8),
              "[\n"
              "  1, 2,\n"
              "  3, 4,\n"
              "  5, 6\n"
              "]\n");
    check_same_emit1(ti, maxcols(9),
              "[\n"
              "  1, 2, 3,\n"
              "  4, 5, 6\n"
              "]\n");
    check_same_emit1(ti, maxcols(12),
              "[\n"
              "  1, 2, 3, 4,\n"
              "  5, 6\n"
              "]\n");
    check_same_emit1(ti, maxcols(15),
              "[\n"
              "  1, 2, 3, 4, 5,\n"
              "  6\n"
              "]\n");
    check_same_emit1(ti, maxcols(18),
              "[\n"
              "  1, 2, 3, 4, 5, 6\n"
              "]\n");
}

TEST(seq, flow_ml_nested_1_ml1)
{
    TreeAndInts ti = parse_tree_and_ints("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    NodeRef r = ti.tree;
    {
        SCOPED_TRACE("base");
        check_same_emit1(ti, "[[1,2,3,4,5,6],10,20,25,[100,200,300,400],30,40,50,[7,8,9,10,11,12]]");
    }
    {
        SCOPED_TRACE("1");
        set_style(&ti, r, FLOW_SL|FLOW_SPC, 2, xievt::FLOW|xievt::FSL_|xievt::FSPC);
        check_same_emit1(ti, "[[1,2,3,4,5,6], 10, 20, 25, [100,200,300,400], 30, 40, 50, [7,8,9,10,11,12]]");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, r, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
        set_style(&ti, r[0], FLOW_SL|FLOW_SPC, 3, xievt::FLOW|xievt::FSL_|xievt::FSPC);
        set_style(&ti, r[4], FLOW_SL|FLOW_SPC, 32, xievt::FLOW|xievt::FSL_|xievt::FSPC);
        set_style(&ti, r[8], FLOW_SL|FLOW_SPC, 55, xievt::FLOW|xievt::FSL_|xievt::FSPC);
        check_same_emit1(ti, "[[1, 2, 3, 4, 5, 6],10,20,25,[100, 200, 300, 400],30,40,50,[7, 8, 9, 10, 11, 12]]");
    }
    {
        SCOPED_TRACE("3");
        set_style(&ti, r, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
        set_style(&ti, r[0], FLOW_SL, 3, xievt::FLOW|xievt::FSL_);
        set_style(&ti, r[4], FLOW_SL, 32, xievt::FLOW|xievt::FSL_);
        set_style(&ti, r[8], FLOW_SL, 55, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti,
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
    {
        SCOPED_TRACE("4");
        set_style(&ti, r, FLOW_ML1|FLOW_SPC, 2, xievt::FLOW|xievt::FML1|xievt::FSPC);
        check_same_emit1(ti,
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
}

TEST(seq, flow_ml_nested_2_mln)
{
    TreeAndInts ti = parse_tree_and_ints("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    set_style(&ti, ti.tree, FLOW_MLN, 2, xievt::FLOW|xievt::FMLN);
    {
        SCOPED_TRACE("0");
        check_same_emit1(ti, maxcols(0),
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
    }
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, maxcols(1),
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
    }
    {
        SCOPED_TRACE("10");
        check_same_emit1(ti, maxcols(10),
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
    }
    {
        SCOPED_TRACE("16");
        check_same_emit1(ti, maxcols(16),
                         "[\n"
                         "  [1,2,3,4,5,6],\n"
                         "  10,20,25,[100,\n"
                         "  200,300,400],30,\n"
                         "  40,50,[7,8,9,10,\n"
                         "  11,12]\n"
                         "]\n");
    }
    {
        SCOPED_TRACE("20");
        check_same_emit1(ti, maxcols(20),
                         "[\n"
                         "  [1,2,3,4,5,6],10,20,\n"
                         "  25,[100,200,300,400],\n"
                         "  30,40,50,[7,8,9,10,\n"
                         "  11,12]\n"
                         "]\n");
    }
    {
        SCOPED_TRACE("24");
        check_same_emit1(ti, maxcols(24),
                         "[\n"
                         "  [1,2,3,4,5,6],10,20,25,\n"
                         "  [100,200,300,400],30,40,\n"
                         "  50,[7,8,9,10,11,12]\n"
                         "]\n");
    }
    {
        SCOPED_TRACE("30");
        check_same_emit1(ti, maxcols(30),
                         "[\n"
                         "  [1,2,3,4,5,6],10,20,25,[100,\n"
                         "  200,300,400],30,40,50,[7,8,9,\n"
                         "  10,11,12]\n"
                         "]\n");
    }
}

TEST(seq, flow_ml_nested_2_mln_spc)
{
    TreeAndInts ti = parse_tree_and_ints("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    set_style(&ti, ti.tree, FLOW_MLN|FLOW_SPC, 2, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    {
        SCOPED_TRACE("0");
        check_same_emit1(ti, maxcols(0),
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
    }
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, maxcols(1),
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
    }
    {
        SCOPED_TRACE("10");
        check_same_emit1(ti, maxcols(10),
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
    }
    {
        SCOPED_TRACE("16");
        check_same_emit1(ti, maxcols(16),
              "[\n"
              "  [1, 2, 3, 4, 5,\n"
              "  6], 10, 20, 25,\n"
              "  [100, 200, 300,\n"
              "  400], 30, 40,\n"
              "  50, [7, 8, 9,\n"
              "  10, 11, 12]\n"
              "]\n");
    }
    {
        SCOPED_TRACE("20");
        check_same_emit1(ti, maxcols(20),
              "[\n"
              "  [1, 2, 3, 4, 5, 6],\n"
              "  10, 20, 25, [100,\n"
              "  200, 300, 400], 30,\n"
              "  40, 50, [7, 8, 9,\n"
              "  10, 11, 12]\n"
              "]\n");
    }
    {
        SCOPED_TRACE("24");
        check_same_emit1(ti, maxcols(24),
              "[\n"
              "  [1, 2, 3, 4, 5, 6], 10,\n"
              "  20, 25, [100, 200, 300,\n"
              "  400], 30, 40, 50, [7,\n"
              "  8, 9, 10, 11, 12]\n"
              "]\n");
    }
    {
        SCOPED_TRACE("30");
        check_same_emit1(ti, maxcols(30),
              "[\n"
              "  [1, 2, 3, 4, 5, 6], 10, 20,\n"
              "  25, [100, 200, 300, 400], 30,\n"
              "  40, 50, [7, 8, 9, 10, 11, 12]\n"
              "]\n");
    }
}

TEST(seq, flow_ml_nested_2_mln_spc_nested)
{
    TreeAndInts ti = parse_tree_and_ints("[[1, 2, 3, 4, 5, 6], 10, 20, 25, [100, 200, 300, 400], 30, 40, 50, [7, 8, 9, 10, 11, 12]]");
    set_style(&ti, ti.tree, FLOW_MLN|FLOW_SPC, 2, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    set_style(&ti, ti.tree[0], FLOW_MLN|FLOW_SPC, 3, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    set_style(&ti, ti.tree[4], FLOW_MLN|FLOW_SPC, 32, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    set_style(&ti, ti.tree[8], FLOW_MLN|FLOW_SPC, 55, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    {
        SCOPED_TRACE("0");
        check_same_emit1(ti, maxcols(0),
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
    }
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, maxcols(1),
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
    }
    {
        SCOPED_TRACE("10");
        check_same_emit1(ti, maxcols(10),
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
    }
    {
        SCOPED_TRACE("16");
        check_same_emit1(ti, maxcols(16),
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
    }
    {
        SCOPED_TRACE("20");
        check_same_emit1(ti, maxcols(20),
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
    }
    {
        SCOPED_TRACE("24");
        check_same_emit1(ti, maxcols(24),
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
    {
        SCOPED_TRACE("30");
        check_same_emit1(ti, maxcols(30),
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
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keyseq, block)
{
    TreeAndInts ti = parse_tree_and_ints("{foo: [1, 2, 3, 4, 5, 6]}");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
        check_same_emit1(ti, R"(foo: [1,2,3,4,5,6]
)");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree["foo"], FLOW_SL|FLOW_SPC, 6, xievt::FLOW|xievt::FSL_|xievt::FSPC);
        check_same_emit1(ti, R"(foo: [1, 2, 3, 4, 5, 6]
)");
    }
    {
        SCOPED_TRACE("3");
        set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
        check_same_emit1(ti, R"(foo:
  - 1
  - 2
  - 3
  - 4
  - 5
  - 6
)");
    }
}

TEST(keyseq, block_nested)
{
    TreeAndInts ti = parse_tree_and_ints("{foo: [1, [2, 3], 4, [5, 6]]}");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
        check_same_emit1(ti, R"(foo: [1,[2,3],4,[5,6]]
)");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree["foo"], FLOW_SL, 6, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"(foo: [1,[2,3],4,[5,6]]
)");
    }
    {
        SCOPED_TRACE("3");
        add_style(&ti, ti.tree["foo"], FLOW_SPC, 6, xievt::FSPC);
        check_same_emit1(ti, R"(foo: [1, [2,3], 4, [5,6]]
)");
    }
    {
        SCOPED_TRACE("3");
        add_style(&ti, ti.tree["foo"][1], FLOW_SPC, 10, xievt::FSPC);
        add_style(&ti, ti.tree["foo"][3], FLOW_SPC, 21, xievt::FSPC);
        check_same_emit1(ti, R"(foo: [1, [2, 3], 4, [5, 6]]
)");
    }
    {
        SCOPED_TRACE("4");
        set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
        check_same_emit1(ti, R"(foo:
  - 1
  - [2, 3]
  - 4
  - [5, 6]
)");
    }
    {
        SCOPED_TRACE("5");
        rem_style(&ti, ti.tree["foo"][1], FLOW_SPC, 10, xievt::FSPC);
        rem_style(&ti, ti.tree["foo"][3], FLOW_SPC, 21, xievt::FSPC);
        check_same_emit1(ti, R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
    }
}

TEST(keyseq, flow_sl)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, 2, 3, 4, 5, 6]");
    set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
    check_same_emit1(ti, R"({foo: [1,2,3,4,5,6]})");
}

TEST(keyseq, flow_sl_nested)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, [2, 3], 4, [5, 6]]");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"({foo: [1,[2,3],4,[5,6]]})");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
        set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
        set_style(&ti, ti.tree["foo"][1], FLOW_SL, 10, xievt::FLOW|xievt::FSL_);
        set_style(&ti, ti.tree["foo"][3], FLOW_SL, 21, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
    }
}

TEST(keyseq, flow_ml1)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, 2, 3, 4, 5, 6]");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
        check_same_emit1(ti,
              "{\n"
              "  foo: [1,2,3,4,5,6]\n"
              "}\n");
    }
    {
        SCOPED_TRACE("2");
        add_style(&ti, ti.tree, FLOW_SPC, 2, xievt::FSPC);
        check_same_emit1(ti,
              "{\n"
              "  foo: [1,2,3,4,5,6]\n"
              "}\n");
    }
    {
        SCOPED_TRACE("3");
        set_style(&ti, ti.tree["foo"], FLOW_ML1, 6, xievt::FLOW|xievt::FML1);
        check_same_emit1(ti,
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
    }
    {
        SCOPED_TRACE("4");
        set_style(&ti, ti.tree["foo"], FLOW_MLN, 6, xievt::FLOW|xievt::FMLN);
        check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    }
    {
        SCOPED_TRACE("5");
        add_style(&ti, ti.tree["foo"], FLOW_SPC, 6, xievt::FSPC);
        check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
    }
}

TEST(keyseq, flow_mln)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, 2, 3, 4, 5, 6]");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
        set_style(&ti, ti.tree["foo"], FLOW_MLN|FLOW_SPC, 6, xievt::FLOW|xievt::FMLN|xievt::FSPC);
        check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree["foo"], FLOW_MLN, 6, xievt::FLOW|xievt::FMLN);
        check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    }
    check_same_emit1(ti, maxcols(5),
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
    check_same_emit1(ti, maxcols(6),
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
    check_same_emit1(ti, maxcols(7),
              "{\n"
              "  foo: [\n"
              "    1,2,\n"
              "    3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(8),
              "{\n"
              "  foo: [\n"
              "    1,2,\n"
              "    3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(9),
              "{\n"
              "  foo: [\n"
              "    1,2,3,\n"
              "    4,5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(10),
              "{\n"
              "  foo: [\n"
              "    1,2,3,\n"
              "    4,5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(11),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(12),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,\n"
              "    5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(13),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(14),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(15),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(20),
              "{\n"
              "  foo: [\n"
              "    1,2,3,4,5,6\n"
              "  ]\n"
              "}\n");
}

TEST(keyseq, flow_mln_spc)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, 2, 3, 4, 5, 6]");
    set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
    set_style(&ti, ti.tree["foo"], FLOW_MLN|FLOW_SPC, 6, xievt::FLOW|xievt::FMLN|xievt::FSPC);
    check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(5),
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
    check_same_emit1(ti, maxcols(6),
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
    check_same_emit1(ti, maxcols(7),
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
    check_same_emit1(ti, maxcols(8),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(9),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(10),
              "{\n"
              "  foo: [\n"
              "    1, 2,\n"
              "    3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(11),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(12),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(13),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3,\n"
              "    4, 5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(14),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4,\n"
              "    5, 6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(17),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5,\n"
              "    6\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(20),
              "{\n"
              "  foo: [\n"
              "    1, 2, 3, 4, 5, 6\n"
              "  ]\n"
              "}\n");
}

TEST(keyseq, flow_mln_nested)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, [2, 3], 4, [5, 6]]");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"({foo: [1,[2,3],4,[5,6]]})");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
        check_same_emit1(ti,
                         "{\n"
                         "  foo: [1,[2,3],4,[5,6]]\n"
                         "}\n");
    }
    {
        SCOPED_TRACE("3");
        set_style(&ti, ti.tree["foo"], FLOW_MLN, 6, xievt::FLOW|xievt::FMLN);
        check_same_emit1(ti,
                  "{\n"
                  "  foo: [\n"
                  "    1,[2,3],4,[5,6]\n"
                  "  ]\n"
                  "}\n");
    }
    {
        SCOPED_TRACE("4");
        check_same_emit1(ti, maxcols(5),
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
    }
    {
        SCOPED_TRACE("5");
        check_same_emit1(ti, maxcols(7),
              "{\n"
              "  foo: [\n"
              "    1,[2,\n"
              "    3],\n"
              "    4,[5,\n"
              "    6]\n"
              "  ]\n"
              "}\n");
    }
    {
        SCOPED_TRACE("6");
        set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
        set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
        set_style(&ti, ti.tree["foo"][1], FLOW_SL, 10, xievt::FLOW|xievt::FSL_);
        set_style(&ti, ti.tree["foo"][3], FLOW_SL, 21, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
    }
}

TEST(keyseq, flow_mln_nested_2)
{
    TreeAndInts ti = parse_tree_and_ints("foo: [1, [2, 3, 20, 30, 40], 4, [5, 6, 70, 80, 90]]");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"({foo: [1,[2,3,20,30,40],4,[5,6,70,80,90]]})");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
        check_same_emit1(ti,
                         "{\n"
                         "  foo: [1,[2,3,20,30,40],4,[5,6,70,80,90]]\n"
                         "}\n");
    }
    {
        SCOPED_TRACE("3");
        set_style(&ti, ti.tree["foo"], FLOW_MLN, 6, xievt::FLOW|xievt::FMLN);
        check_same_emit1(ti,
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,30,40],4,[5,6,70,80,90]\n"
              "  ]\n"
              "}\n");
    }
    check_same_emit1(ti, maxcols(5),
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
    check_same_emit1(ti, maxcols(7),
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
    check_same_emit1(ti, maxcols(12),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,\n"
              "    30,40],4,\n"
              "    [5,6,70,\n"
              "    80,90]\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(20),
              "{\n"
              "  foo: [\n"
              "    1,[2,3,20,30,40],\n"
              "    4,[5,6,70,80,90]\n"
              "  ]\n"
              "}\n");
    check_same_emit1(ti, maxcols(25),
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
    TreeAndInts ti = parse_tree_and_ints("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
    check_same_emit1(ti, R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
)");
}

TEST(map, block_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("'ab': [2,3]\n");
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, "'ab': [2,3]\n");
    }
    {
        SCOPED_TRACE("2");
        rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
        rem_style(&ti, ti.tree[0], CONTAINER_STYLE, 6, all_styles_container);
        check_same_emit1(ti, "ab:\n  - 2\n  - 3\n");
    }
}

TEST(map, flow_sl_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("{'ab': [2,3]}");
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, "{'ab': [2,3]}");
    }
    {
        SCOPED_TRACE("2");
        rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
        rem_style(&ti, ti.tree[0], CONTAINER_STYLE, 6, all_styles_container);
        check_same_emit1(ti, "{ab: [2,3]}");
    }
}

TEST(map, flow_ml_picks_default_style)
{
    TreeAndInts ti = parse_tree_and_ints("{\n  'ab': [\n    2,\n    3]\n}");
    {
        SCOPED_TRACE("1");
        check_same_emit1(ti, "{\n  'ab': [\n    2,\n    3\n  ]\n}\n");
    }
    {
        SCOPED_TRACE("2");
        rem_style(&ti, ti.tree[0], SCALAR_STYLE, 3, all_styles_scalar);
        rem_style(&ti, ti.tree[0], CONTAINER_STYLE, 6, all_styles_container);
        check_same_emit1(ti, "{\n  ab: [2,3]\n}\n");
    }
}

TEST(map, flow_sl)
{
    TreeAndInts ti = parse_tree_and_ints(R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
)");
    set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
    check_same_emit1(ti, R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
    add_style(&ti, ti.tree, FLOW_SPC, 2, xievt::FSPC);
    check_same_emit1(ti, R"({1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10})");
    rem_style(&ti, ti.tree, FLOW_SPC, 2, xievt::FSPC);
    check_same_emit1(ti, R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
}

TEST(map, flow_ml_1)
{
    TreeAndInts ti = parse_tree_and_ints(R"(1: 10
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
    set_style(&ti, ti.tree, FLOW_ML1, 2, xievt::FLOW|xievt::FML1);
    check_same_emit1(ti, ""
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
    add_style(&ti, ti.tree, FLOW_SPC, 2, xievt::FSPC);
    check_same_emit1(ti, ""
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
    TreeAndInts ti = parse_tree_and_ints(R"(1: 10
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
    set_style(&ti, ti.tree, FLOW_MLN, 2, xievt::FLOW|xievt::FMLN);
    check_same_emit1(ti, ""
              "{\n"
              "  1: 10,2: 10,3: 10,4: 10,5: 10,6: 10,7: 10,8: 10,9: 10,10: 10,11: 10,12: 10\n"
              "}\n");
    check_same_emit1(ti, maxcols(40), ""
              "{\n"
              "  1: 10,2: 10,3: 10,4: 10,5: 10,6: 10,7: 10,\n"
              "  8: 10,9: 10,10: 10,11: 10,12: 10\n"
              "}\n");
    check_same_emit1(ti, maxcols(20), ""
              "{\n"
              "  1: 10,2: 10,3: 10,\n"
              "  4: 10,5: 10,6: 10,\n"
              "  7: 10,8: 10,9: 10,\n"
              "  10: 10,11: 10,12: 10\n"
              "}\n");
    add_style(&ti, ti.tree, FLOW_SPC, 2, xievt::FSPC);
    check_same_emit1(ti, ""
              "{\n"
              "  1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10, 7: 10, 8: 10, 9: 10, 10: 10, 11: 10,\n"
              "  12: 10\n"
              "}\n");
    check_same_emit1(ti, maxcols(40), ""
              "{\n"
              "  1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10,\n"
              "  7: 10, 8: 10, 9: 10, 10: 10, 11: 10, 12: 10\n"
              "}\n");
    check_same_emit1(ti, maxcols(20), ""
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
    TreeAndInts ti = parse_tree_and_ints("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
    check_same_emit1(ti, ""
                     "foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}\n");
    set_style(&ti, ti.tree["foo"], FLOW_SL|FLOW_SPC, 6, xievt::FLOW|xievt::FSL_|xievt::FSPC);
    check_same_emit1(ti, ""
                     "foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}\n");
    set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
    check_same_emit1(ti, R"(foo:
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
    TreeAndInts ti = parse_tree_and_ints(R"(foo:
  1: 10
  2: 10
  3: 10
  4: 10
  5: 10
  6: 10
)");
    set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
    check_same_emit1(ti, R"({foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}})");
}


TEST(keymap, flow_sl_nested)
{
    TreeAndInts ti = parse_tree_and_ints(R"(foo:
  1: 10
  2:
    2: 10
    3: 10
  4: 10
  5:
    5: 10
    6: 10
)");
    {
        SCOPED_TRACE("1");
        set_style(&ti, ti.tree, FLOW_SL, 2, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"({foo: {1: 10,2: {2: 10,3: 10},4: 10,5: {5: 10,6: 10}}})");
    }
    {
        SCOPED_TRACE("2");
        set_style(&ti, ti.tree, BLOCK, 2, xievt::BLCK);
        set_style(&ti, ti.tree["foo"], BLOCK, 6, xievt::BLCK);
        set_style(&ti, ti.tree["foo"][1], FLOW_SL, 16, xievt::FLOW|xievt::FSL_);
        set_style(&ti, ti.tree["foo"][3], FLOW_SL, 39, xievt::FLOW|xievt::FSL_);
        check_same_emit1(ti, R"(foo:
  1: 10
  2: {2: 10,3: 10}
  4: 10
  5: {5: 10,6: 10}
)");
    }
    {
        SCOPED_TRACE("3");
        add_style(&ti, ti.tree["foo"][1], FLOW_SPC, 16, xievt::FSPC);
        add_style(&ti, ti.tree["foo"][3], FLOW_SPC, 39, xievt::FSPC);
        check_same_emit1(ti, R"(foo:
  1: 10
  2: {2: 10, 3: 10}
  4: 10
  5: {5: 10, 6: 10}
)");
    }
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
