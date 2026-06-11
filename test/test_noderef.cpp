#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/format_base64.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include "./test_lib/callbacks_tester.hpp"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389) // signed/unsigned mismatch
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(NodeRef, general)
{
    Tree t;

    NodeRef root(&t);

    root.set_map();
    (root.append_child() = key("a")) = "0";
    root["b"].set_map();
    root["b"]["seq"].set_seq();
    root["b"]["seq"][0] = "0";
    root["b"]["seq"][1] = "1";
    root["b"]["seq"][2] = "2";
    (root["b"]["seq"][3] = "3").set_val_tag("!!str");
    (root["b"]["seq"][3] = "3").set_val_tag("!!str");
    NodeRef ch4 = root["b"]["seq"][3].append_sibling() = "4";
    EXPECT_EQ(ch4.id(), root["b"]["seq"][4].id());
    EXPECT_EQ(ch4.get(), root["b"]["seq"][4].get());
    {
        const NodeRef constch4 = ch4;
        auto *nd = constch4.get();
        EXPECT_EQ(nd, ch4.get());
        static_assert(std::is_same<decltype(nd), NodeData const*>::value, "must be const");
    }
    EXPECT_EQ((type_bits)root["b"]["seq"][4].type(), (type_bits)VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");
    NodeRef aaa = root["b"]["seq"].append_sibling();
    aaa.set_key("aaa");
    aaa.set_key_tag("!!str");
    aaa.set_val("0");
    aaa.set_val_tag("!!int");
    EXPECT_EQ((type_bits)root["b"]["seq"][4].type(), (type_bits)VAL);
    EXPECT_EQ((type_bits)aaa.type(), (type_bits)(KEY|VAL|KEYTAG|VALTAG));
    aaa.set_val("0", VAL_DQUO);
    EXPECT_EQ((type_bits)aaa.type(), (type_bits)(KEY|VAL|KEYTAG|VALTAG|VAL_DQUO));
    aaa.set_key("aaa", KEY_SQUO);
    EXPECT_EQ((type_bits)aaa.type(), (type_bits)(KEY|VAL|KEYTAG|VALTAG|KEY_SQUO|VAL_DQUO));

    root["b"]["key"] = "val";
    NodeRef seq = root["b"]["seq"];
    NodeRef seq2 = root["b"]["seq2"];
    EXPECT_TRUE(seq2.is_seed());
    root["b"]["seq2"].set_seq();
    seq2 = root["b"]["seq2"];
    EXPECT_FALSE(seq2.is_seed());
    EXPECT_TRUE(seq2.is_seq());
    EXPECT_EQ(seq2.num_children(), 0);
    EXPECT_EQ(root["b"]["seq2"].get(), seq2.get());
    NodeRef seq20 = seq2[0];
    EXPECT_TRUE(seq20.is_seed());
    EXPECT_TRUE(seq2[0].is_seed());
    EXPECT_EQ(seq2.num_children(), 0);
    EXPECT_TRUE(seq2[0].is_seed());
    EXPECT_TRUE(seq20.is_seed());
    EXPECT_NE(seq.get(), seq2.get());
    seq20 = root["b"]["seq2"][0];
    EXPECT_TRUE(seq20.is_seed());
    root["b"]["seq2"][0] = "00";
    seq20 = root["b"]["seq2"][0];
    EXPECT_FALSE(seq20.is_seed());
    NodeRef before = root["b"]["key"];
    EXPECT_EQ(before.key(), "key");
    EXPECT_EQ(before.val(), "val");
    root["b"]["seq2"][1] = "01";
    NodeRef after = root["b"]["key"];
    EXPECT_EQ(before.key(), "key");
    EXPECT_EQ(before.val(), "val");
    EXPECT_EQ(after.key(), "key");
    EXPECT_EQ(after.val(), "val");
    root["b"]["seq2"][2] = "02";
    root["b"]["seq2"][3] = "03";
    int iv = 0;
    root["b"]["seq2"][4] << 55;
    root["b"]["seq2"][4] >> iv;
    EXPECT_EQ(iv, 55);
    size_t zv = 0;
    root["b"]["seq2"][5] << size_t(55);
    root["b"]["seq2"][5] >> zv;
    EXPECT_EQ(zv, size_t(55));
    float fv = 0;
    root["b"]["seq2"][6] << 2.0f;
    root["b"]["seq2"][6] >> fv;
    EXPECT_EQ(fv, 2.f);
    double dv = 0;
    root["b"]["seq2"][7] << 2.0;
    root["b"]["seq2"][7] >> dv;
    EXPECT_EQ(dv, 2.0);

    EXPECT_EQ(root["b"]["key"].key(), "key");
    EXPECT_EQ(root["b"]["key"].val(), "val");


    emit_yaml(t);

    EXPECT_TRUE(root.type().is_map());
    EXPECT_TRUE(root["a"].type().is_keyval());
    EXPECT_EQ(root["a"].key(), "a");
    EXPECT_EQ(root["a"].val(), "0");

    EXPECT_TRUE(root["b"].type().has_key());
    EXPECT_TRUE(root["b"].type().is_map());

    EXPECT_TRUE(root["b"]["seq"].type().has_key());
    EXPECT_TRUE(root["b"]["seq"].type().is_seq());
    EXPECT_EQ  (root["b"]["seq"].key(), "seq");
    EXPECT_TRUE(root["b"]["seq"][0].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][0].val(), "0");
    EXPECT_TRUE(root["b"]["seq"][1].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][1].val(), "1");
    EXPECT_TRUE(root["b"]["seq"][2].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][2].val(), "2");
    EXPECT_TRUE(root["b"]["seq"][3].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][3].val(), "3");
    EXPECT_EQ(  root["b"]["seq"][3].val_tag(), "!!str");
    EXPECT_TRUE(root["b"]["seq"][4].type().is_val());
    EXPECT_EQ(  root["b"]["seq"][4].val(), "4");

    int tv;
    EXPECT_EQ(root["b"]["key"].key(), "key");
    EXPECT_EQ(root["b"]["key"].val(), "val");
    EXPECT_EQ(root["b"]["seq2"][0].val(), "00"); root["b"]["seq2"][0] >> tv; EXPECT_EQ(tv, 0);
    EXPECT_EQ(root["b"]["seq2"][1].val(), "01"); root["b"]["seq2"][1] >> tv; EXPECT_EQ(tv, 1);
    EXPECT_EQ(root["b"]["seq2"][2].val(), "02"); root["b"]["seq2"][2] >> tv; EXPECT_EQ(tv, 2);
    EXPECT_EQ(root["b"]["seq2"][3].val(), "03"); root["b"]["seq2"][3] >> tv; EXPECT_EQ(tv, 3);
    EXPECT_EQ(root["b"]["seq2"][4].val(), "55"); EXPECT_EQ(iv, 55);
    EXPECT_EQ(root["b"]["seq2"][5].val(), "55"); EXPECT_EQ(zv, size_t(55));
    EXPECT_EQ(root["b"]["seq2"][6].val(), "2"); EXPECT_EQ(fv, 2.f);
    EXPECT_EQ(root["b"]["seq2"][6].val(), "2"); EXPECT_EQ(dv, 2.);

    root["b"]["seq"][2].set_val_serialized(22);

    emit_yaml(t);

    EXPECT_TRUE(root["b"]["aaa"].type().is_keyval());
    EXPECT_EQ(root["b"]["aaa"].key_tag(), "!!str");
    EXPECT_EQ(root["b"]["aaa"].key(), "aaa");
    EXPECT_EQ(root["b"]["aaa"].val_tag(), "!!int");
    EXPECT_EQ(root["b"]["aaa"].val(), "0");
}

TEST(NodeRef, operator_equal_equal)
{
    Tree tree1 = parse_in_arena("{a: a1, b: b1}");
    NodeRef a1 = tree1["a"];
    NodeRef b1 = tree1["b"];
    NodeRef a1_ = a1;
    NodeRef b1_ = b1;
    NodeRef seedc1 = tree1["c"];
    NodeRef seedc1_ = seedc1;
    NodeRef seedd1 = tree1["d"];
    NodeRef seedd1_ = seedd1;
    EXPECT_EQ(a1, a1_);
    EXPECT_EQ(b1, b1_);
    EXPECT_NE(a1, b1);
    EXPECT_NE(b1, a1);
    EXPECT_EQ(seedc1, seedc1_);
    EXPECT_EQ(seedd1, seedd1_);
    EXPECT_NE(seedc1, seedd1);
    Tree tree2 = parse_in_arena("{a: a2, b: b2}");
    NodeRef a2 = tree2["a"];
    NodeRef b2 = tree2["b"];
    NodeRef a2_ = a2;
    NodeRef b2_ = b2;
    NodeRef seedc2 = tree2["c"];
    NodeRef seedc2_ = seedc2;
    NodeRef seedd2 = tree2["d"];
    NodeRef seedd2_ = seedd2;
    EXPECT_EQ(a2, a2_);
    EXPECT_EQ(b2, b2_);
    EXPECT_NE(a2, b2);
    EXPECT_NE(b2, a2);
    EXPECT_EQ(seedc2, seedc2_);
    EXPECT_EQ(seedd2, seedd2_);
    EXPECT_NE(seedc2, seedd2);
    //
    EXPECT_NE(a1, a2);
    EXPECT_NE(b1, b2);
    EXPECT_NE(seedc1, seedc2);
    EXPECT_NE(seedd1, seedd2);
}

TEST(NodeRef, valid_vs_seed_vs_readable)
{
    static_assert(!ConstNodeRef::is_seed(), "ConstNodeRef must never be a seed");
    Tree tree = parse_in_arena("foo: bar");
    NodeRef foo = tree["foo"];
    ConstNodeRef const_foo = tree["foo"];
    EXPECT_FALSE(foo.invalid());
    EXPECT_FALSE(foo.is_seed());
    EXPECT_TRUE(foo.readable());
    EXPECT_FALSE(const_foo.invalid());
    EXPECT_FALSE(const_foo.is_seed());
    EXPECT_TRUE(const_foo.readable());
    NodeRef none;
    EXPECT_TRUE(none.invalid());
    EXPECT_FALSE(none.is_seed());
    EXPECT_FALSE(none.readable());
    ConstNodeRef const_none;
    EXPECT_TRUE(const_none.invalid());
    EXPECT_FALSE(const_none.is_seed());
    EXPECT_FALSE(const_none.readable());
    none = tree["none"];
    EXPECT_FALSE(none.invalid());
    EXPECT_TRUE(none.is_seed());
    EXPECT_FALSE(none.readable());
    ASSERT_FALSE(tree.rootref().has_child(none));
    const_none = tree["none"];
    EXPECT_TRUE(const_none.invalid());
    EXPECT_FALSE(const_none.is_seed());
    EXPECT_TRUE(none.is_seed());
    EXPECT_FALSE(none.readable());
}

namespace {
const auto errbasic = ExpectedErrorType::err_basic;
const auto errvisit = ExpectedErrorType::err_visit;
const auto errany = ExpectedErrorType::err_any;

// define these functions here to minimize the binary size
using errfn = std::function<void()>;
C4_NO_INLINE void check_assert(ExpectedErrorType errty, Tree *tree, errfn const& fn) noexcept
{
    ExpectError::check_assert(errty, tree, fn);
}
C4_NO_INLINE void check_success(Tree *tree, errfn const& fn) noexcept
{
    ExpectError::check_success(tree, fn);
}
#define _TEST_FAIL_(errtype, tree, method_expr) \
    {                                           \
        SCOPED_TRACE(#method_expr);             \
        check_assert(errtype, tree, [&]{        \
            auto ret = (method_expr);           \
            (void)ret;                          \
        });                                     \
    }
#define _TEST_SUCCEED(tree, method_expr)        \
    {                                           \
        SCOPED_TRACE(#method_expr);             \
        check_success(tree, [&]{                \
            auto ret = (method_expr);           \
            (void)ret;                          \
        });                                     \
    }

template<class NodeT>
C4_NO_INLINE void test_fail_read(Tree *tree, NodeT node, ExpectedErrorType errtype) noexcept
{
    _TEST_SUCCEED(tree, node.get())
    _TEST_FAIL_(errtype, tree, node.type())
    _TEST_FAIL_(errtype, tree, node.type_str())
    _TEST_FAIL_(errtype, tree, node.key())
    _TEST_FAIL_(errtype, tree, node.key_tag())
    _TEST_FAIL_(errtype, tree, node.key_anchor())
    _TEST_FAIL_(errtype, tree, node.key_ref())
    _TEST_FAIL_(errtype, tree, node.key_is_null())
    _TEST_FAIL_(errtype, tree, node.keysc())
    _TEST_FAIL_(errtype, tree, node.val())
    _TEST_FAIL_(errtype, tree, node.val_tag())
    _TEST_FAIL_(errtype, tree, node.val_anchor())
    _TEST_FAIL_(errtype, tree, node.val_ref())
    _TEST_FAIL_(errtype, tree, node.val_is_null())
    _TEST_FAIL_(errtype, tree, node.valsc())
    _TEST_FAIL_(errtype, tree, node.is_map())
    _TEST_FAIL_(errtype, tree, node.empty())
    _TEST_FAIL_(errtype, tree, node.is_stream())
    _TEST_FAIL_(errtype, tree, node.is_doc())
    _TEST_FAIL_(errtype, tree, node.is_container())
    _TEST_FAIL_(errtype, tree, node.is_map())
    _TEST_FAIL_(errtype, tree, node.is_seq())
    _TEST_FAIL_(errtype, tree, node.has_val())
    _TEST_FAIL_(errtype, tree, node.has_key())
    _TEST_FAIL_(errtype, tree, node.is_keyval())
    _TEST_FAIL_(errtype, tree, node.has_key_tag())
    _TEST_FAIL_(errtype, tree, node.has_val_tag())
    _TEST_FAIL_(errtype, tree, node.has_key_anchor())
    _TEST_FAIL_(errtype, tree, node.has_val_anchor())
    _TEST_FAIL_(errtype, tree, node.has_anchor())
    _TEST_FAIL_(errtype, tree, node.is_key_ref())
    _TEST_FAIL_(errtype, tree, node.is_val_ref())
    _TEST_FAIL_(errtype, tree, node.is_ref())
    _TEST_FAIL_(errtype, tree, node.parent_is_seq())
    _TEST_FAIL_(errtype, tree, node.parent_is_map())
    _TEST_FAIL_(errtype, tree, node.type_has_any(MAP|SEQ))
    _TEST_FAIL_(errtype, tree, node.type_has_all(MAP|SEQ))
    _TEST_FAIL_(errtype, tree, node.type_has_none(MAP|SEQ))
    _TEST_FAIL_(errtype, tree, node.is_container_styled())
    _TEST_FAIL_(errtype, tree, node.is_block())
    _TEST_FAIL_(errtype, tree, node.is_flow())
    _TEST_FAIL_(errtype, tree, node.is_flow_sl())
    _TEST_FAIL_(errtype, tree, node.is_flow_ml1())
    _TEST_FAIL_(errtype, tree, node.is_flow_mln())
    _TEST_FAIL_(errtype, tree, node.is_flow_mlx())
    _TEST_FAIL_(errtype, tree, node.has_flow_space())
    _TEST_FAIL_(errtype, tree, node.key_style())
    _TEST_FAIL_(errtype, tree, node.val_style())
    _TEST_FAIL_(errtype, tree, node.is_key_styled())
    _TEST_FAIL_(errtype, tree, node.is_val_styled())
    _TEST_FAIL_(errtype, tree, node.is_key_literal())
    _TEST_FAIL_(errtype, tree, node.is_val_literal())
    _TEST_FAIL_(errtype, tree, node.is_key_folded())
    _TEST_FAIL_(errtype, tree, node.is_val_folded())
    _TEST_FAIL_(errtype, tree, node.is_key_squo())
    _TEST_FAIL_(errtype, tree, node.is_val_squo())
    _TEST_FAIL_(errtype, tree, node.is_key_dquo())
    _TEST_FAIL_(errtype, tree, node.is_val_dquo())
    _TEST_FAIL_(errtype, tree, node.is_key_plain())
    _TEST_FAIL_(errtype, tree, node.is_val_plain())
    _TEST_FAIL_(errtype, tree, node.is_key_quoted())
    _TEST_FAIL_(errtype, tree, node.is_val_quoted())
    _TEST_FAIL_(errtype, tree, node.is_quoted())
    _TEST_FAIL_(errtype, tree, node.is_root())
    _TEST_FAIL_(errtype, tree, node.has_parent())
    _TEST_FAIL_(errtype, tree, node.has_child(0))
    _TEST_FAIL_(errtype, tree, node.has_child("key"))
    _TEST_FAIL_(errtype, tree, node.has_children())
    _TEST_FAIL_(errtype, tree, node.has_sibling("key"))
    _TEST_FAIL_(errtype, tree, node.has_other_siblings())
    _TEST_FAIL_(errtype, tree, node.doc(0))
    _TEST_FAIL_(errtype, tree, node.parent())
    _TEST_FAIL_(errtype, tree, node.num_children())
    _TEST_FAIL_(errtype, tree, node.first_child())
    _TEST_FAIL_(errtype, tree, node.last_child())
    _TEST_FAIL_(errtype, tree, node.child(0))
    _TEST_FAIL_(errtype, tree, node.find_child("key"))
    _TEST_FAIL_(errtype, tree, node.prev_sibling())
    _TEST_FAIL_(errtype, tree, node.next_sibling())
    _TEST_FAIL_(errtype, tree, node.first_sibling())
    _TEST_FAIL_(errtype, tree, node.last_sibling())
    _TEST_FAIL_(errtype, tree, node.sibling(0))
    _TEST_FAIL_(errtype, tree, node.find_sibling("key"))
    _TEST_FAIL_(errtype, tree, node.num_children())
    _TEST_FAIL_(errtype, tree, node.num_siblings())
    _TEST_FAIL_(errtype, tree, node.num_other_siblings())
    _TEST_FAIL_(errtype, tree, node.depth_asc())
    _TEST_FAIL_(errtype, tree, node.depth_desc())
    _TEST_FAIL_(errtype, tree, node["key"])
    _TEST_FAIL_(errtype, tree, node[0])
    _TEST_FAIL_(errtype, tree, node.at("key"))
    _TEST_FAIL_(errtype, tree, node.at(0))
    int val;
    _TEST_FAIL_(errtype, tree, node >> val)
    _TEST_FAIL_(errtype, tree, node >> key(val))
    _TEST_FAIL_(errtype, tree, node >> fmt::base64(val))
    _TEST_FAIL_(errtype, tree, node >> key(fmt::base64(val)))
    //_TEST_FAIL_(errtype, tree, node.deserialize_key(fmt::base64(val)))
    //_TEST_FAIL_(errtype, tree, node.deserialize(fmt::base64(val)))
    _TEST_FAIL_(errtype, tree, node.get_if("key", &val));
    _TEST_FAIL_(errtype, tree, node.get_if("key", &val, 0));
    const NodeT const_node = node;
    _TEST_FAIL_(errtype, tree, node.begin());
    _TEST_FAIL_(errtype, tree, node.cbegin());
    _TEST_FAIL_(errtype, tree, const_node.begin());
    _TEST_FAIL_(errtype, tree, const_node.cbegin());
    _TEST_FAIL_(errtype, tree, node.end());
    _TEST_FAIL_(errtype, tree, node.end());
    _TEST_FAIL_(errtype, tree, const_node.end());
    _TEST_FAIL_(errtype, tree, const_node.end());
    _TEST_FAIL_(errtype, tree, node.children());
    _TEST_FAIL_(errtype, tree, node.children());
    _TEST_FAIL_(errtype, tree, const_node.children());
    _TEST_FAIL_(errtype, tree, const_node.children());
    _TEST_FAIL_(errtype, tree, node.siblings());
    _TEST_FAIL_(errtype, tree, node.siblings());
    _TEST_FAIL_(errtype, tree, const_node.siblings());
    _TEST_FAIL_(errtype, tree, const_node.siblings());
    //_TEST_FAIL(tree, node.visit([](NodeT &n, size_t level){ (void)n; (void)level; return false; }));
    //_TEST_FAIL(tree, const_node.visit([](const NodeT &n, size_t level){ (void)n; (void)level; return false; }));
    _TEST_SUCCEED(tree, const_node == node);
    _TEST_SUCCEED(tree, const_node != node);
    if(std::is_same<NodeT, NodeRef>::value)
    {
        ConstNodeRef other;
        _TEST_SUCCEED(tree, node == other);
        _TEST_SUCCEED(tree, node != node);
    }
}
template<class NodeT>
C4_NO_INLINE void test_fail_read_subject(Tree *tree, NodeT node, NodeT subject, ExpectedErrorType errtype) noexcept
{
    SCOPED_TRACE("here");
    if(node.readable())
    {
        _TEST_SUCCEED(tree, node.has_child(subject))
        _TEST_SUCCEED(tree, node.has_sibling(subject))
        _TEST_SUCCEED(tree, node.is_ancestor(subject))
    }
    else
    {
        _TEST_FAIL_(errtype, tree, node.has_child(subject))
        _TEST_FAIL_(errtype, tree, node.has_sibling(subject))
        _TEST_FAIL_(errtype, tree, node.is_ancestor(subject))
    }
    _TEST_FAIL_(errtype, tree, node.child_pos(subject))
    _TEST_FAIL_(errtype, tree, node.sibling_pos(subject))
}
#undef _TEST_FAIL_
#undef _TEST_SUCCEED
} // namespace


TEST(NodeRef, cannot_read_from_invalid)
{
    NodeRef none;
    SCOPED_TRACE("here");
    ASSERT_EQ(none.tree(), nullptr);
    ASSERT_EQ(none.id(), NONE);
    EXPECT_TRUE(none.invalid());
    EXPECT_FALSE(none.is_seed());
    EXPECT_FALSE(none.readable());
    test_fail_read(nullptr, none, errbasic);
    test_fail_read_subject(nullptr, none, none, errbasic);
}

TEST(NodeRef, cannot_read_from_invalid_subject)
{
    SCOPED_TRACE("here");
    NodeRef none;
    Tree tree = parse_in_arena("foo: bar");
    test_fail_read_subject(&tree, tree["foo"], none, errany);
}

TEST(ConstNodeRef, cannot_read_from_invalid)
{
    SCOPED_TRACE("here");
    ConstNodeRef const_none;
    ASSERT_EQ(const_none.tree(), nullptr);
    ASSERT_EQ(const_none.id(), NONE);
    EXPECT_TRUE(const_none.invalid());
    EXPECT_FALSE(const_none.is_seed());
    EXPECT_FALSE(const_none.readable());
    test_fail_read(nullptr, const_none, errbasic);
    test_fail_read(nullptr, NodeRef{}, errbasic);
    const NodeRef nnone;
    test_fail_read<NodeRef>(nullptr, nnone, errbasic);
    test_fail_read<const NodeRef>(nullptr, nnone, errbasic);
    test_fail_read_subject(nullptr, const_none, const_none, errany);
    Tree tree = parse_in_arena("foo: bar");
    ConstNodeRef foo = tree["foo"];
    test_fail_read_subject(&tree, foo, const_none, errany);
}

TEST(ConstNodeRef, cannot_read_from_invalid_subject)
{
    SCOPED_TRACE("here");
    Tree tree = parse_in_arena("foo: bar");
    ConstNodeRef foo = tree["foo"];
    ConstNodeRef const_none;
    test_fail_read_subject(&tree, foo, const_none, errany);
}

TEST(NodeRef, cannot_read_from_seed)
{
    SCOPED_TRACE("here");
    Tree tree = parse_in_arena("foo: bar");
    NodeRef none = tree["none"];
    ASSERT_EQ(none.tree(), &tree);
    ASSERT_EQ(none.id(), 0);
    EXPECT_FALSE(none.invalid());
    EXPECT_TRUE(none.is_seed());
    EXPECT_FALSE(none.readable());
    test_fail_read(&tree, none, errvisit);
    test_fail_read_subject(&tree, none, none, errvisit);
}

TEST(NodeRef, cannot_read_from_seed_subject)
{
    SCOPED_TRACE("here");
    Tree tree = parse_in_arena("foo: bar");
    NodeRef none = tree["none"];
    ASSERT_EQ(none.tree(), &tree);
    ASSERT_EQ(none.id(), 0);
    EXPECT_FALSE(none.invalid());
    EXPECT_TRUE(none.is_seed());
    EXPECT_FALSE(none.readable());
    test_fail_read(&tree, none, errvisit);
    test_fail_read_subject(&tree, none, none, errvisit);
}

TEST(ConstNodeRef, cannot_read_from_seed_subject)
{
    SCOPED_TRACE("here");
    Tree tree = parse_in_arena("foo: bar");
    ConstNodeRef const_none = tree["none"];
    ASSERT_EQ(const_none.tree(), &tree);
    ASSERT_EQ(const_none.id(), NONE);
    EXPECT_TRUE(const_none.invalid());
    EXPECT_FALSE(const_none.is_seed());
    EXPECT_FALSE(const_none.readable());
    test_fail_read(&tree, const_none, errvisit);
    ConstNodeRef foo = tree["foo"];
    test_fail_read_subject(&tree, foo, const_none, errvisit);
}

void noderef_check_tree(ConstNodeRef const& root)
{
    test_invariants(*root.tree());

    EXPECT_EQ(root.tree()->size(), 7u);
    EXPECT_EQ(root.num_children(), 6u);
    EXPECT_EQ(root.is_container(), true);
    EXPECT_EQ(root.is_seq(), true);

    EXPECT_TRUE(root[0].type().is_val());
    EXPECT_EQ(  root[0].val(), "0");
    EXPECT_TRUE(root[1].type().is_val());
    EXPECT_EQ(  root[1].val(), "1");
    EXPECT_TRUE(root[2].type().is_val());
    EXPECT_EQ(  root[2].val(), "2");
    EXPECT_TRUE(root[3].type().is_val());
    EXPECT_EQ(  root[3].val(), "3");
    EXPECT_TRUE(root[4].type().is_val());
    EXPECT_EQ(  root[4].val(), "4");
    EXPECT_TRUE(root[5].type().is_val());
    EXPECT_EQ(  root[5].val(), "5");
}

TEST(NodeRef, append_child_1)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    root.append_child() = "0";
    root.append_child() = "1";
    root.append_child() = "2";
    root.append_child() = "3";
    root.append_child() = "4";
    root.append_child() = "5";
    noderef_check_tree(root);
}

TEST(NodeRef, append_child_2)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    root.append_child() << "0";
    root.append_child() << "1";
    root.append_child() << "2";
    root.append_child() << "3";
    root.append_child() << "4";
    root.append_child() << "5";
    noderef_check_tree(root);
}

TEST(NodeRef, append_sibling_1)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    NodeRef first = root.append_child() = "0";
    first.append_sibling() = "1";
    first.append_sibling() = "2";
    first.append_sibling() = "3";
    first.append_sibling() = "4";
    first.append_sibling() = "5";
    noderef_check_tree(root);
}

TEST(NodeRef, append_sibling_2)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    NodeRef first = root.append_child() << "0";
    first.append_sibling() << "1";
    first.append_sibling() << "2";
    first.append_sibling() << "3";
    first.append_sibling() << "4";
    first.append_sibling() << "5";
    noderef_check_tree(root);
}

TEST(NodeRef, prepend_child_1)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    root.prepend_child() << "5";
    root.prepend_child() << "4";
    root.prepend_child() << "3";
    root.prepend_child() << "2";
    root.prepend_child() << "1";
    root.prepend_child() << "0";
    noderef_check_tree(root);
}

TEST(NodeRef, prepend_child_2)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    root.prepend_child() << "5";
    root.prepend_child() << "4";
    root.prepend_child() << "3";
    root.prepend_child() << "2";
    root.prepend_child() << "1";
    root.prepend_child() << "0";
    noderef_check_tree(root);
}

TEST(NodeRef, prepend_sibling_1)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    NodeRef last = root.prepend_child() = "5";
    last.prepend_sibling() = "4";
    last.prepend_sibling() = "3";
    last.prepend_sibling() = "2";
    last.prepend_sibling() = "1";
    last.prepend_sibling() = "0";
    noderef_check_tree(root);
}

TEST(NodeRef, prepend_sibling_2)
{
    Tree t;
    NodeRef root(&t);
    root.set_seq();
    NodeRef last = root.prepend_child() << "5";
    last.prepend_sibling() << "4";
    last.prepend_sibling() << "3";
    last.prepend_sibling() << "2";
    last.prepend_sibling() << "1";
    last.prepend_sibling() << "0";
    noderef_check_tree(root);
}

TEST(NodeRef, insert_child_1)
{
    Tree t;
    NodeRef root(&t);
    NodeRef none(&t, NONE);
    root.set_seq();
    root.insert_child(none) = "3";
    root.insert_child(root[0]) = "4";
    root.insert_child(none) = "0";
    root.insert_child(root[2]) = "5";
    root.insert_child(root[0]) = "1";
    root.insert_child(root[1]) = "2";
    noderef_check_tree(root);
}

TEST(NodeRef, insert_child_2)
{
    Tree t;
    NodeRef root(&t);
    NodeRef none(&t, NONE);
    root.set_seq();
    root.insert_child(none) << "3";
    root.insert_child(root[0]) << "4";
    root.insert_child(none) << "0";
    root.insert_child(root[2]) << "5";
    root.insert_child(root[0]) << "1";
    root.insert_child(root[1]) << "2";
    noderef_check_tree(root);
}

TEST(NodeRef, insert_sibling_1)
{
    Tree t;
    NodeRef root(&t);
    NodeRef none(&t, NONE);
    root.set_seq();
    NodeRef first = root.insert_child(none) = "3";
    first.insert_sibling(root[0]) = "4";
    first.insert_sibling(none) = "0";
    first.insert_sibling(root[2]) = "5";
    first.insert_sibling(root[0]) = "1";
    first.insert_sibling(root[1]) = "2";
    noderef_check_tree(root);
}

TEST(NodeRef, insert_sibling_2)
{
    Tree t;
    NodeRef root(&t);
    NodeRef none(&t, NONE);
    root.set_seq();
    NodeRef first = root.insert_child(none) << "3";
    first.insert_sibling(root[0]) << "4";
    first.insert_sibling(none) << "0";
    first.insert_sibling(root[2]) << "5";
    first.insert_sibling(root[0]) << "1";
    first.insert_sibling(root[1]) << "2";
    noderef_check_tree(root);
}

TEST(NodeRef, remove_child)
{
    Tree t;

    NodeRef root(&t);
    NodeRef none(&t, NONE);

    root.set_seq();
    root.insert_child(none) = "3";
    root.insert_child(root[0]) = "4";
    root.insert_child(none) = "0";
    root.insert_child(root[2]) = "5";
    root.insert_child(root[0]) = "1";
    root.insert_child(root[1]) = "2";

    std::vector<int> vec({10, 20, 30, 40, 50, 60, 70, 80, 90});
    root.insert_child(root[0]) << vec; // 1
    root.insert_child(root[2]) << vec; // 3
    root.insert_child(root[4]) << vec; // 5
    root.insert_child(root[6]) << vec; // 7
    root.insert_child(root[8]) << vec; // 9
    root.append_child() << vec;        // 10

    root.remove_child(11);
    root.remove_child(9);
    root.remove_child(7);
    root.remove_child(5);
    root.remove_child(3);
    root.remove_child(1);

    noderef_check_tree(root);

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    root.prepend_child() << vec2; // 0
    root.insert_child(root[1]) << vec2; // 2
    root.insert_child(root[3]) << vec2; // 4
    root.insert_child(root[5]) << vec2; // 6
    root.insert_child(root[7]) << vec2; // 8
    root.insert_child(root[9]) << vec2; // 10
    root.append_child() << vec2;        // 12

    root.remove_child(12);
    root.remove_child(10);
    root.remove_child(8);
    root.remove_child(6);
    root.remove_child(4);
    root.remove_child(2);
    root.remove_child(0);

    noderef_check_tree(root);
}

TEST(NodeRef, clear_key)
{
    Tree tree = parse_in_arena("foo: bar");
    ASSERT_TRUE(tree[0].type().has_key());
    EXPECT_EQ(tree[0].key(), "foo");
    tree[0].clear_key();
    ASSERT_FALSE(tree[0].type().has_key());
    {
        NodeRef n;
        ExpectError::check_assert_basic(&tree, [&] { n.clear_key(); });
    }
    {
        NodeRef n = tree["not there"];
        ExpectError::check_assert_visit(&tree, [&] { n.clear_key(); });
    }
}

TEST(NodeRef, clear_val)
{
    Tree tree = parse_in_arena("foo: bar");
    ASSERT_TRUE(tree[0].type().has_val());
    EXPECT_EQ(tree[0].val(), "bar");
    tree[0].clear_val();
    ASSERT_FALSE(tree[0].type().has_val());
    {
        NodeRef n;
        ExpectError::check_assert_basic(&tree, [&] { n.clear_val(); });
    }
    {
        NodeRef n = tree["not there"];
        ExpectError::check_assert_visit(&tree, [&] { n.clear_val(); });
    }
}

TEST(NodeRef, clear_children)
{
    Tree tree = parse_in_arena("[0, 1, 2, 3, [4, 5, 6], 7]");
    ASSERT_TRUE(tree[4].type().is_seq());
    EXPECT_TRUE(tree[4].has_children());
    EXPECT_EQ(tree[4].num_children(), 3);
    tree[4].clear_children();
    ASSERT_TRUE(tree[4].type().is_seq());
    EXPECT_FALSE(tree[4].has_children());
    EXPECT_EQ(tree[4].num_children(), 0);
    {
        NodeRef n;
        ExpectError::check_assert_basic(&tree, [&] { n.clear_children(); });
    }
    {
        NodeRef n = tree[21];
        ExpectError::check_assert_visit(&tree, [&] { n.clear_children(); });
    }
}

TEST(NodeRef, to_arena)
{
    csubstr yaml = "foo: bar";
    Tree tree = parse_in_arena(yaml);
    EXPECT_EQ(tree.arena(), yaml);
    EXPECT_NE(tree.arena().str, yaml.str);
    {
        NodeRef n = tree;
        n.to_arena(123);
        EXPECT_EQ(tree.arena(), "foo: bar123");
    }
    {
        NodeRef n;
        ExpectError::check_assert_basic(&tree, [&] { n.to_arena(456); });
    }
    {
        NodeRef n = tree["not there"];
        ExpectError::check_success(&tree, [&] { n.to_arena(456); });
        EXPECT_EQ(tree.arena(), "foo: bar123456");
    }
}

TEST(NodeRef, remove_child__issue_356)
{
    csubstr yaml = R"(
numbers:
  one: true
  two: false
  three: false
  four: false
formats:
  rtt: json
)";
    Tree tree = parse_in_arena(yaml);
    NodeRef root = tree.rootref();
    const std::string expected = R"(formats:
  rtt: json
)";
    auto test_formats = [&](const char *desc){
        NodeRef formats = root["formats"];
        RYML_TRACE_FMT("desc={} formats.id={} ", desc, formats.id());
        EXPECT_TRUE(formats.readable());
        print_tree(tree);
        check_invariants(tree);
        EXPECT_EQ(emitrs_yaml<std::string>(formats), expected);
        EXPECT_EQ(emitrs_yaml<std::string>(tree, formats.id()), expected);
    };
    test_formats("0 after parse");
    NodeRef numbers = root["numbers"];
    numbers.remove_child("one");
    test_formats("1 after remove one");
    numbers.remove_child("two");
    test_formats("2 after remove two");
    numbers.remove_child("three");
    test_formats("3 after remove three");
}

TEST(NodeRef, move_in_same_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map<std::string, int> map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r.set_seq();
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    auto s = r[0];
    auto m = r[1];
    EXPECT_TRUE(s.is_seq());
    EXPECT_TRUE(m.is_map());
    EXPECT_EQ(s.num_children(), vec2.size());
    EXPECT_EQ(m.num_children(), map2.size());
    //printf("fonix"); print_tree(t); emit_yaml(r);
    r[0].move(r, r[1]);
    //printf("fonix"); print_tree(t); emit_yaml(r);
    EXPECT_EQ(r[0].get(), m.get());
    EXPECT_EQ(r[0].num_children(), map2.size());
    EXPECT_EQ(r[1].get(), s.get());
    EXPECT_EQ(r[1].num_children(), vec2.size());
    test_invariants(t);
}

TEST(NodeRef, move_in_same_parent_to_first_position)
{
    Tree t = parse_in_arena("[1, 2, 3, 0, 4]");
    NodeRef r = t;

    EXPECT_TRUE(r[0].val() == "1");
    EXPECT_TRUE(r[1].val() == "2");
    EXPECT_TRUE(r[2].val() == "3");
    EXPECT_TRUE(r[3].val() == "0");
    EXPECT_TRUE(r[4].val() == "4");
    r[3].move(r, {});
    EXPECT_TRUE(r[0].val() == "0");
    EXPECT_TRUE(r[1].val() == "1");
    EXPECT_TRUE(r[2].val() == "2");
    EXPECT_TRUE(r[3].val() == "3");
    EXPECT_TRUE(r[4].val() == "4");
    test_invariants(t);
    r[0].move(r, {}); // should have no effect
    EXPECT_TRUE(r[0].val() == "0");
    EXPECT_TRUE(r[1].val() == "1");
    EXPECT_TRUE(r[2].val() == "2");
    EXPECT_TRUE(r[3].val() == "3");
    EXPECT_TRUE(r[4].val() == "4");
    test_invariants(t);
    r[4].move(r, {});
    EXPECT_TRUE(r[0].val() == "4");
    EXPECT_TRUE(r[1].val() == "0");
    EXPECT_TRUE(r[2].val() == "1");
    EXPECT_TRUE(r[3].val() == "2");
    EXPECT_TRUE(r[4].val() == "3");
    test_invariants(t);
}

TEST(NodeRef, move_to_other_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector<std::vector<int>> vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map<std::string, int> map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r.set_seq();
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    NodeData *elm2 = r[2].get();
    EXPECT_EQ(r[2].val(), "elm2");
    //printf("fonix"); print_tree(t); emit_yaml(r);
    r[2].move(r[0], r[0][0]);
    EXPECT_EQ(r[0][1].get(), elm2);
    EXPECT_EQ(r[0][1].val(), "elm2");
    //printf("fonix"); print_tree(t); emit_yaml(r);
    test_invariants(t);
}

TEST(NodeRef, move_to_other_parent_to_first_position)
{
    Tree t = parse_in_arena("[[0, 1, 2, 3, 4], [00, 10, 20, 30, 40]]");
    NodeRef r = t;

    EXPECT_TRUE(r[0][0].val() == "0");
    EXPECT_TRUE(r[0][1].val() == "1");
    EXPECT_TRUE(r[0][2].val() == "2");
    EXPECT_TRUE(r[0][3].val() == "3");
    EXPECT_TRUE(r[0][4].val() == "4");
    EXPECT_TRUE(r[1][0].val() == "00");
    EXPECT_TRUE(r[1][1].val() == "10");
    EXPECT_TRUE(r[1][2].val() == "20");
    EXPECT_TRUE(r[1][3].val() == "30");
    EXPECT_TRUE(r[1][4].val() == "40");
    test_invariants(t);
    r[0][0].move(r[1], {});
    EXPECT_TRUE(r[0][0].val() == "1");
    EXPECT_TRUE(r[0][1].val() == "2");
    EXPECT_TRUE(r[0][2].val() == "3");
    EXPECT_TRUE(r[0][3].val() == "4");
    EXPECT_TRUE(r[1][0].val() == "0");
    EXPECT_TRUE(r[1][1].val() == "00");
    EXPECT_TRUE(r[1][2].val() == "10");
    EXPECT_TRUE(r[1][3].val() == "20");
    EXPECT_TRUE(r[1][4].val() == "30");
    EXPECT_TRUE(r[1][5].val() == "40");
    test_invariants(t);
    r[1][0].move(r[0], {});
    EXPECT_TRUE(r[0][0].val() == "0");
    EXPECT_TRUE(r[0][1].val() == "1");
    EXPECT_TRUE(r[0][2].val() == "2");
    EXPECT_TRUE(r[0][3].val() == "3");
    EXPECT_TRUE(r[0][4].val() == "4");
    EXPECT_TRUE(r[1][0].val() == "00");
    EXPECT_TRUE(r[1][1].val() == "10");
    EXPECT_TRUE(r[1][2].val() == "20");
    EXPECT_TRUE(r[1][3].val() == "30");
    EXPECT_TRUE(r[1][4].val() == "40");
    test_invariants(t);
}

TEST(NodeRef, move_to_other_tree)
{
    Tree t0 = parse_in_arena("[0, 1, 2, 3, 4]");
    Tree t1 = parse_in_arena("[00, 10, 20, 30, 40]");
    NodeRef r0 = t0;
    NodeRef r1 = t1;

    EXPECT_TRUE(r0[0].val() == "0");
    EXPECT_TRUE(r0[1].val() == "1");
    EXPECT_TRUE(r0[2].val() == "2");
    EXPECT_TRUE(r0[3].val() == "3");
    EXPECT_TRUE(r0[4].val() == "4");
    EXPECT_TRUE(r1[0].val() == "00");
    EXPECT_TRUE(r1[1].val() == "10");
    EXPECT_TRUE(r1[2].val() == "20");
    EXPECT_TRUE(r1[3].val() == "30");
    EXPECT_TRUE(r1[4].val() == "40");
    r0[0].move(r1, r1[0]);
    test_invariants(t0);
    test_invariants(t1);
    EXPECT_TRUE(r0[0].val() == "1");
    EXPECT_TRUE(r0[1].val() == "2");
    EXPECT_TRUE(r0[2].val() == "3");
    EXPECT_TRUE(r0[3].val() == "4");
    EXPECT_TRUE(r1[0].val() == "00");
    EXPECT_TRUE(r1[1].val() == "0");
    EXPECT_TRUE(r1[2].val() == "10");
    EXPECT_TRUE(r1[3].val() == "20");
    EXPECT_TRUE(r1[4].val() == "30");
    EXPECT_TRUE(r1[5].val() == "40");
    test_invariants(t0);
    test_invariants(t1);
    r1[1].move(r0, r0[0]);
    EXPECT_TRUE(r0[0].val() == "1");
    EXPECT_TRUE(r0[1].val() == "0");
    EXPECT_TRUE(r0[2].val() == "2");
    EXPECT_TRUE(r0[3].val() == "3");
    EXPECT_TRUE(r0[4].val() == "4");
    EXPECT_TRUE(r1[0].val() == "00");
    EXPECT_TRUE(r1[1].val() == "10");
    EXPECT_TRUE(r1[2].val() == "20");
    EXPECT_TRUE(r1[3].val() == "30");
    EXPECT_TRUE(r1[4].val() == "40");
    test_invariants(t0);
    test_invariants(t1);
}

TEST(NodeRef, move_to_other_tree_to_first_position)
{
    Tree t0 = parse_in_arena("[0, 1, 2, 3, 4]");
    Tree t1 = parse_in_arena("[00, 10, 20, 30, 40]");
    NodeRef r0 = t0;
    NodeRef r1 = t1;

    EXPECT_TRUE(r0[0].val() == "0");
    EXPECT_TRUE(r0[1].val() == "1");
    EXPECT_TRUE(r0[2].val() == "2");
    EXPECT_TRUE(r0[3].val() == "3");
    EXPECT_TRUE(r0[4].val() == "4");
    EXPECT_TRUE(r1[0].val() == "00");
    EXPECT_TRUE(r1[1].val() == "10");
    EXPECT_TRUE(r1[2].val() == "20");
    EXPECT_TRUE(r1[3].val() == "30");
    EXPECT_TRUE(r1[4].val() == "40");
    test_invariants(t0);
    test_invariants(t1);
    r0[0].move(r1, {});
    EXPECT_TRUE(r0[0].val() == "1");
    EXPECT_TRUE(r0[1].val() == "2");
    EXPECT_TRUE(r0[2].val() == "3");
    EXPECT_TRUE(r0[3].val() == "4");
    EXPECT_TRUE(r1[0].val() == "0");
    EXPECT_TRUE(r1[1].val() == "00");
    EXPECT_TRUE(r1[2].val() == "10");
    EXPECT_TRUE(r1[3].val() == "20");
    EXPECT_TRUE(r1[4].val() == "30");
    EXPECT_TRUE(r1[5].val() == "40");
    test_invariants(t0);
    test_invariants(t1);
    r1[0].move(r0, {});
    EXPECT_TRUE(r0[0].val() == "0");
    EXPECT_TRUE(r0[1].val() == "1");
    EXPECT_TRUE(r0[2].val() == "2");
    EXPECT_TRUE(r0[3].val() == "3");
    EXPECT_TRUE(r0[4].val() == "4");
    EXPECT_TRUE(r1[0].val() == "00");
    EXPECT_TRUE(r1[1].val() == "10");
    EXPECT_TRUE(r1[2].val() == "20");
    EXPECT_TRUE(r1[3].val() == "30");
    EXPECT_TRUE(r1[4].val() == "40");
    test_invariants(t0);
    test_invariants(t1);
}

namespace {
template<class Fn>
C4_NO_INLINE void test_duplicate(ConstNodeRef orig, Fn const& fn, std::string const& expected_yaml) noexcept
{
    ConstNodeRef copy = fn();
    test_compare(copy, orig, "copy", "orig");
    EXPECT_EQ(emitrs_yaml<std::string>(*copy.tree()), expected_yaml);
    if(testing::Test::HasFailure())
    {
        print_tree("orig node", *orig.tree(), orig.id());
        print_tree("copy node", *copy.tree(), copy.id());
        if(orig.tree() == copy.tree())
        {
            print_tree("full tree", *orig.tree());
        }
        else
        {
            print_tree("orig tree", *orig.tree());
            print_tree("copy tree", *copy.tree());
        }
    }
}
} // namespace


TEST(NodeRef, duplicate_to_same_tree)
{
    csubstr yaml = "[{a0: [b0,c0]},{a1: [b1,c1]}]";
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the beginning");
        {
            SCOPED_TRACE("seq");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(t, {}); },
                           "[[b0,c0],{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("seq, but tree");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return NodeRef{&t, t.duplicate(orig.id(), t.root_id(), NONE)}; },
                           "[[b0,c0],{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(t, {}); },
                           "[{a0: [b0,c0]},{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map, but tree");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0];
            test_duplicate(orig,
                           [&]{ return NodeRef{&t, t.duplicate(orig.id(), t.root_id(), NONE)}; },
                           "[{a0: [b0,c0]},{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the end");
        {
            SCOPED_TRACE("seq");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(t, t.rootref().last_child()); },
                           "[{a0: [b0,c0]},{a1: [b1,c1]},[b0,c0]]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("seq, but tree");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return NodeRef{&t, t.duplicate(orig.id(), t.root_id(), t.last_child(t.root_id()))}; },
                           "[{a0: [b0,c0]},{a1: [b1,c1]},[b0,c0]]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return t[0].duplicate(t, t.rootref().last_child()); },
                           "[{a0: [b0,c0]},{a1: [b1,c1]},{a0: [b0,c0]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map, but tree");
            Tree t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return NodeRef{&t, t.duplicate(t[0].id(), t.root_id(), t.last_child(t.root_id()))}; },
                           "[{a0: [b0,c0]},{a1: [b1,c1]},{a0: [b0,c0]}]");
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("in the middle");
        {
            SCOPED_TRACE("seq");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(t, t[0]); },
                           "[{a0: [b0,c0]},[b0,c0],{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("seq, but tree");
            Tree t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return NodeRef{&t, t.duplicate(orig.id(), t.root_id(), t[0].id())}; },
                           "[{a0: [b0,c0]},[b0,c0],{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return t[0].duplicate(t, t[0]); },
                           "[{a0: [b0,c0]},{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map, but tree");
            Tree t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return NodeRef{&t, t.duplicate(t[0].id(), t.root_id(), t[0].id())}; },
                           "[{a0: [b0,c0]},{a0: [b0,c0]},{a1: [b1,c1]}]");
        }
    }
}

TEST(NodeRef, duplicate_to_different_tree)
{
    csubstr yaml = "[{a0: [b0,c0]},{a1: [b1,c1]}]";
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the beginning");
        {
            SCOPED_TRACE("seq");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(dst, {}); },
                           "[[b0,c0],1,2]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            NodeRef orig = t[0];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(dst, {}); },
                           "[{a0: [b0,c0]},1,2]");
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the end");
        {
            SCOPED_TRACE("seq");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(dst, dst.rootref().last_child()); },
                           "[1,2,[b0,c0]]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return t[0].duplicate(dst, dst.rootref().last_child()); },
                           "[1,2,{a0: [b0,c0]}]");
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("in the middle");
        {
            SCOPED_TRACE("seq");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            NodeRef orig = t[0]["a0"];
            test_duplicate(orig,
                           [&]{ return orig.duplicate(dst, dst[0]); },
                           "[1,[b0,c0],2]");
        }
        if(!testing::Test::HasFailure())
        {
            SCOPED_TRACE("map");
            Tree dst = parse_in_arena("[1,2]"), t = parse_in_arena(yaml);
            test_duplicate(t[0],
                           [&]{ return t[0].duplicate(dst, dst[0]); },
                           "[1,{a0: [b0,c0]},2]");
        }
    }
}

namespace {
C4_NO_INLINE void test_duplicate_children(ConstNodeRef orig, ConstNodeRef dstparent, ConstNodeRef after, std::string const& expected_yaml) noexcept
{
    ASSERT_TRUE(after.readable() || dstparent.readable());
    for(id_type src_id = orig.first_child().id(),
            dst_id = after.readable() ? after.next_sibling().id() : dstparent.first_child().id();
        src_id != NONE && dst_id != NONE;
        src_id = orig.tree()->next_sibling(src_id),
        dst_id = dstparent.tree()->next_sibling(dst_id))
    {
        ConstNodeRef orig_child = {orig.tree(), src_id};
        ConstNodeRef copy = {dstparent.tree(), dst_id};
        test_compare(copy, orig_child, "copy", "orig");
    }
    EXPECT_EQ(emitrs_yaml<std::string>(*dstparent.tree()), expected_yaml);
    if(testing::Test::HasFailure())
    {
        if(orig.tree() == after.tree())
        {
            print_tree("full tree", *orig.tree());
        }
        else
        {
            print_tree("orig tree", *orig.tree());
            print_tree("copy tree", *dstparent.tree());
        }
    }
}
} // namespace


TEST(NodeRef, duplicate_children_to_same_tree)
{
    csubstr yaml = "[{a0: [b0,c0]},{a1: [b1,c1]}]";
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the beginning");
        ConstNodeRef after = {};
        Tree t = parse_in_arena(yaml);
        NodeRef orig = t[0]["a0"];
        orig.duplicate_children(t, after);
        test_duplicate_children(orig, t, after, "[b0,c0,{a0: [b0,c0]},{a1: [b1,c1]}]");
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("at the end");
        Tree t = parse_in_arena(yaml);
        NodeRef orig = t[0]["a0"];
        ConstNodeRef after = t.rootref().last_child();
        orig.duplicate_children(t, after);
        test_duplicate_children(orig, t, after, "[{a0: [b0,c0]},{a1: [b1,c1]},b0,c0]");
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("in the middle");
        Tree t = parse_in_arena(yaml);
        NodeRef orig = t[0]["a0"];
        ConstNodeRef after = t[0];
        orig.duplicate_children(t, after);
        test_duplicate_children(orig, t, after, "[{a0: [b0,c0]},b0,c0,{a1: [b1,c1]}]");
    }
}

TEST(NodeRef, intseq)
{
    Tree t = parse_in_arena("iseq: [8, 10]");
    NodeRef n = t["iseq"];
    int a, b;
    n[0] >> a;
    n[1] >> b;
    EXPECT_EQ(a, 8);
    EXPECT_EQ(b, 10);
    test_invariants(t);
}

TEST(NodeRef, vsConstNodeRef)
{
    Tree t = parse_in_arena("iseq: [8, 10]");
    Tree const& ct = t;
    NodeRef mseq = t["iseq"];
    ConstNodeRef seq = t["iseq"];
    EXPECT_EQ(mseq.tree(), seq.tree());
    EXPECT_EQ(mseq.id(), seq.id());
    EXPECT_TRUE(mseq == seq);
    EXPECT_FALSE(mseq != seq);
    EXPECT_TRUE(seq == mseq);
    EXPECT_FALSE(seq != mseq);
    // mseq = ct["iseq"]; // deliberate compile error
    seq = ct["iseq"]; // ok
    // mseq = seq; // deliberate compilation error
    seq = mseq; // ok
    {
        (void)mseq.get();
        //(void)seq.get(); // deliberate compile error
    }
    {
        (void)seq.get();
        (void)seq.get(); // ok
    }
    test_invariants(t);
}


namespace {
bool is_const_ref(NodeRef const&) { return false; }
bool is_const_ref(ConstNodeRef const&) { return true; }
} /// namespace


// see https://github.com/biojppm/rapidyaml/issues/294
TEST(NodeRef, overload_sets)
{
    // doc()
    {
        Tree t = parse_in_arena("a\n---\nb");
        NodeRef n = t;
        NodeRef const nc = t;
        ConstNodeRef const cn = t;
        EXPECT_EQ(n.doc(0), nc.doc(0));
        EXPECT_EQ(n.doc(0), cn.doc(0));
        EXPECT_EQ(is_const_ref(n.doc(0)), false);
        EXPECT_EQ(is_const_ref(cn.doc(0)), true);
    }
    Tree t = parse_in_arena("{iseq: [8, 10], imap: {a: b, c: d}}");
    NodeRef n = t;
    NodeRef const nc = t;
    ConstNodeRef const cn = t;
    NodeRef n_iseq = n["iseq"];
    NodeRef const nc_iseq = n["iseq"];
    ConstNodeRef const cn_iseq = n["iseq"];
    NodeRef n_imap = n["imap"];
    NodeRef const nc_imap = n["imap"];
    ConstNodeRef const cn_imap = n["imap"];
    // get()
    {
        EXPECT_EQ(n_iseq.get(), nc_iseq.get());
        EXPECT_EQ(n_iseq.get(), cn_iseq.get());
    }
    // parent()
    {
        EXPECT_EQ(n_iseq.parent(), nc_iseq.parent());
        EXPECT_EQ(n_iseq.parent(), cn_iseq.parent());
        EXPECT_EQ(is_const_ref(n_iseq.parent()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.parent()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.parent()), true);
    }
    // child_pos()
    {
        EXPECT_EQ(n_iseq.child_pos(n_iseq[0]), nc_iseq.child_pos(n_iseq[0]));
        EXPECT_EQ(n_iseq.child_pos(n_iseq[0]), cn_iseq.child_pos(n_iseq[0]));
    }
    // num_children()
    {
        EXPECT_EQ(n_iseq.num_children(), nc_iseq.num_children());
        EXPECT_EQ(n_iseq.num_children(), cn_iseq.num_children());
    }
    // first_child()
    {
        EXPECT_EQ(n_iseq.first_child(), nc_iseq.first_child());
        EXPECT_EQ(n_iseq.first_child(), cn_iseq.first_child());
        EXPECT_EQ(is_const_ref(n_iseq.first_child()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.first_child()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.first_child()), true);
    }
    // last_child()
    {
        EXPECT_EQ(n_iseq.last_child(), nc_iseq.last_child());
        EXPECT_EQ(n_iseq.last_child(), cn_iseq.last_child());
        EXPECT_EQ(is_const_ref(n_iseq.last_child()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.last_child()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.last_child()), true);
    }
    // child()
    {
        EXPECT_EQ(n_iseq.child(0), nc_iseq.child(0));
        EXPECT_EQ(n_iseq.child(0), cn_iseq.child(0));
        EXPECT_EQ(is_const_ref(n_iseq.child(0)), false);
        EXPECT_EQ(is_const_ref(cn_iseq.child(0)), true);
        EXPECT_EQ(is_const_ref(nc_iseq.child(0)), true);
    }
    // find_child()
    {
        EXPECT_EQ(n.find_child("iseq"), nc.find_child("iseq"));
        EXPECT_EQ(n.find_child("iseq"), cn.find_child("iseq"));
        EXPECT_EQ(n_imap.find_child("a"), nc_imap.find_child("a"));
        EXPECT_EQ(n_imap.find_child("a"), cn_imap.find_child("a"));
        EXPECT_EQ(is_const_ref(n_imap.find_child("a")), false);
        EXPECT_EQ(is_const_ref(cn_imap.find_child("a")), true);
        EXPECT_EQ(is_const_ref(nc_imap.find_child("a")), true);
    }
    // prev_sibling()
    {
        EXPECT_EQ(n_iseq[1].prev_sibling(), nc_iseq[1].prev_sibling());
        EXPECT_EQ(n_iseq[1].prev_sibling(), cn_iseq[1].prev_sibling());
        EXPECT_EQ(is_const_ref(n_iseq.prev_sibling()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.prev_sibling()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.prev_sibling()), true);
    }
    // next_sibling()
    {
        EXPECT_EQ(n_iseq[0].next_sibling(), nc_iseq[0].next_sibling());
        EXPECT_EQ(n_iseq[0].next_sibling(), cn_iseq[0].next_sibling());
        EXPECT_EQ(is_const_ref(n_iseq.next_sibling()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.next_sibling()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.next_sibling()), true);
    }
    // first_sibling()
    {
        EXPECT_EQ(n_iseq[1].first_sibling(), nc_iseq[1].first_sibling());
        EXPECT_EQ(n_iseq[1].first_sibling(), cn_iseq[1].first_sibling());
        EXPECT_EQ(is_const_ref(n_iseq.first_sibling()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.first_sibling()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.first_sibling()), true);
    }
    // last_sibling()
    {
        EXPECT_EQ(n_iseq[0].last_sibling(), nc_iseq[0].last_sibling());
        EXPECT_EQ(n_iseq[0].last_sibling(), cn_iseq[0].last_sibling());
        EXPECT_EQ(is_const_ref(n_iseq.last_sibling()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.last_sibling()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.last_sibling()), true);
    }
    // sibling()
    {
        EXPECT_EQ(n_iseq[1].sibling(0), nc_iseq[1].sibling(0));
        EXPECT_EQ(n_iseq[1].sibling(0), cn_iseq[1].sibling(0));
        EXPECT_EQ(is_const_ref(n_iseq.sibling(0)), false);
        EXPECT_EQ(is_const_ref(cn_iseq.sibling(0)), true);
        EXPECT_EQ(is_const_ref(nc_iseq.sibling(0)), true);
    }
    // find_sibling()
    {
        EXPECT_EQ(n_iseq.find_sibling("imap"), nc_iseq.find_sibling("imap"));
        EXPECT_EQ(n_iseq.find_sibling("imap"), cn_iseq.find_sibling("imap"));
        EXPECT_EQ(n_imap.find_sibling("a"), nc_imap.find_sibling("a"));
        EXPECT_EQ(n_imap.find_sibling("a"), cn_imap.find_sibling("a"));
        EXPECT_EQ(is_const_ref(n_imap.find_sibling("a")), false);
        EXPECT_EQ(is_const_ref(cn_imap.find_sibling("a")), true);
        EXPECT_EQ(is_const_ref(nc_imap.find_sibling("a")), true);
    }
    // ancestor_doc()
    {
        EXPECT_EQ(n_iseq[1].ancestor_doc(), nc_iseq[1].ancestor_doc());
        EXPECT_EQ(n_iseq[1].ancestor_doc(), cn_iseq[1].ancestor_doc());
        EXPECT_EQ(is_const_ref(n_iseq.ancestor_doc()), false);
        EXPECT_EQ(is_const_ref(cn_iseq.ancestor_doc()), true);
        EXPECT_EQ(is_const_ref(nc_iseq.ancestor_doc()), true);
    }
    // operator[](csubstr)
    {
        EXPECT_EQ(n["iseq"].id(), nc["iseq"].id());
        EXPECT_EQ(n["iseq"].id(), cn["iseq"].id());
        EXPECT_EQ(is_const_ref(n["iseq"]), false);
        EXPECT_EQ(is_const_ref(cn["iseq"]), true);
        EXPECT_EQ(is_const_ref(nc["iseq"]), true);
    }
    // operator[](size_t)
    {
        EXPECT_EQ(n_iseq[0].id(), nc_iseq[0].id());
        EXPECT_EQ(n_iseq[0].id(), cn_iseq[0].id());
        EXPECT_EQ(is_const_ref(n_iseq[0]), false);
        EXPECT_EQ(is_const_ref(cn_iseq[0]), true);
        EXPECT_EQ(is_const_ref(nc_iseq[0]), true);
    }
    // at(csubstr)
    {
        EXPECT_EQ(n.at("iseq").id(), nc.at("iseq").id());
        EXPECT_EQ(n.at("iseq").id(), cn.at("iseq").id());
        EXPECT_EQ(is_const_ref(n.at("iseq")), false);
        EXPECT_EQ(is_const_ref(cn.at("iseq")), true);
        EXPECT_EQ(is_const_ref(nc.at("iseq")), true);
    }
    // at(size_t)
    {
        EXPECT_EQ(n_iseq.at(0).id(), nc_iseq.at(0).id());
        EXPECT_EQ(n_iseq.at(0).id(), cn_iseq.at(0).id());
        EXPECT_EQ(is_const_ref(n_iseq.at(0)), false);
        EXPECT_EQ(is_const_ref(cn_iseq.at(0)), true);
        EXPECT_EQ(is_const_ref(nc_iseq.at(0)), true);
    }
    // begin()
    {
        EXPECT_EQ(n_iseq.begin().m_child_id, nc_iseq.begin().m_child_id);
        EXPECT_EQ(n_iseq.begin().m_child_id, cn_iseq.begin().m_child_id);
    }
    // end()
    {
        EXPECT_EQ(n_iseq.end().m_child_id, nc_iseq.end().m_child_id);
        EXPECT_EQ(n_iseq.end().m_child_id, cn_iseq.end().m_child_id);
    }
    // cbegin()
    {
        EXPECT_EQ(n_iseq.cbegin().m_child_id, nc_iseq.cbegin().m_child_id);
        EXPECT_EQ(n_iseq.cbegin().m_child_id, cn_iseq.cbegin().m_child_id);
        EXPECT_EQ(n_iseq.cbegin().m_child_id, n_iseq.begin().m_child_id);
        EXPECT_EQ(nc_iseq.cbegin().m_child_id, nc_iseq.begin().m_child_id);
        EXPECT_EQ(cn_iseq.cbegin().m_child_id, cn_iseq.begin().m_child_id);
    }
    // cend()
    {
        EXPECT_EQ(n_iseq.cend().m_child_id, nc_iseq.cend().m_child_id);
        EXPECT_EQ(n_iseq.cend().m_child_id, cn_iseq.cend().m_child_id);
        EXPECT_EQ(n_iseq.cend().m_child_id, n_iseq.end().m_child_id);
        EXPECT_EQ(nc_iseq.cend().m_child_id, nc_iseq.end().m_child_id);
        EXPECT_EQ(cn_iseq.cend().m_child_id, cn_iseq.end().m_child_id);
    }
    // children()
    {
        EXPECT_EQ(n["iseq"].children().b.m_child_id, nc["iseq"].children().b.m_child_id);
        EXPECT_EQ(n["iseq"].children().b.m_child_id, cn["iseq"].children().b.m_child_id);
        EXPECT_EQ(n["iseq"].children().e.m_child_id, nc["iseq"].children().e.m_child_id);
        EXPECT_EQ(n["iseq"].children().e.m_child_id, cn["iseq"].children().e.m_child_id);
    }
    // cchildren()
    {
        EXPECT_EQ(n["iseq"].cchildren().b.m_child_id, nc["iseq"].cchildren().b.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().b.m_child_id, cn["iseq"].cchildren().b.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().b.m_child_id, n["iseq"].children().b.m_child_id);
        EXPECT_EQ(nc["iseq"].cchildren().b.m_child_id, nc["iseq"].children().b.m_child_id);
        EXPECT_EQ(cn["iseq"].cchildren().b.m_child_id, cn["iseq"].children().b.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().e.m_child_id, nc["iseq"].cchildren().e.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().e.m_child_id, cn["iseq"].cchildren().e.m_child_id);
        EXPECT_EQ(n["iseq"].cchildren().e.m_child_id, n["iseq"].children().e.m_child_id);
        EXPECT_EQ(nc["iseq"].cchildren().e.m_child_id, nc["iseq"].children().e.m_child_id);
        EXPECT_EQ(cn["iseq"].cchildren().e.m_child_id, cn["iseq"].children().e.m_child_id);
    }
    // siblings()
    {
        EXPECT_EQ(n["iseq"][0].siblings().b.m_child_id, nc["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].siblings().b.m_child_id, cn["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(n.siblings().b.m_child_id, nc.siblings().b.m_child_id);
        EXPECT_EQ(n.siblings().b.m_child_id, cn.siblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].siblings().e.m_child_id, nc["iseq"][0].siblings().e.m_child_id);
        EXPECT_EQ(n["iseq"][0].siblings().e.m_child_id, cn["iseq"][0].siblings().e.m_child_id);
        EXPECT_EQ(n.siblings().e.m_child_id, nc.siblings().e.m_child_id);
        EXPECT_EQ(n.siblings().e.m_child_id, cn.siblings().e.m_child_id);
    }
    // csiblings()
    {
        EXPECT_EQ(n["iseq"][0].csiblings().b.m_child_id, nc["iseq"][0].csiblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().b.m_child_id, cn["iseq"][0].csiblings().b.m_child_id);
        EXPECT_EQ(n.csiblings().b.m_child_id, nc.csiblings().b.m_child_id);
        EXPECT_EQ(n.csiblings().b.m_child_id, cn.csiblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().e.m_child_id, nc["iseq"][0].csiblings().e.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().e.m_child_id, cn["iseq"][0].csiblings().e.m_child_id);
        EXPECT_EQ(n.csiblings().e.m_child_id, nc.csiblings().e.m_child_id);
        EXPECT_EQ(n.csiblings().e.m_child_id, cn.csiblings().e.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().b.m_child_id, n["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(cn["iseq"][0].csiblings().b.m_child_id, cn["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(nc["iseq"][0].csiblings().b.m_child_id, nc["iseq"][0].siblings().b.m_child_id);
        EXPECT_EQ(n.csiblings().b.m_child_id, n.siblings().b.m_child_id);
        EXPECT_EQ(nc.csiblings().b.m_child_id, nc.siblings().b.m_child_id);
        EXPECT_EQ(cn.csiblings().b.m_child_id, cn.siblings().b.m_child_id);
        EXPECT_EQ(n["iseq"][0].csiblings().e.m_child_id, n["iseq"][0].siblings().e.m_child_id);
        EXPECT_EQ(nc["iseq"][0].csiblings().e.m_child_id, nc["iseq"][0].siblings().e.m_child_id);
        EXPECT_EQ(cn["iseq"][0].csiblings().e.m_child_id, cn["iseq"][0].siblings().e.m_child_id);
        EXPECT_EQ(n.csiblings().e.m_child_id, n.siblings().e.m_child_id);
        EXPECT_EQ(nc.csiblings().e.m_child_id, nc.siblings().e.m_child_id);
        EXPECT_EQ(cn.csiblings().e.m_child_id, cn.siblings().e.m_child_id);
    }
    // iter begin-end
    const std::vector<csubstr> expected = {"8", "10"};
    {
        {
            std::vector<csubstr> actual;
            for(auto it = n_iseq.begin(); it != n_iseq.end(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto it = nc_iseq.begin(); it != nc_iseq.end(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto it = cn_iseq.begin(); it != cn_iseq.end(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
    }
    // iter cbegin-cend
    {
        {
            std::vector<csubstr> actual;
            for(auto it = n_iseq.cbegin(); it != n_iseq.cend(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto it = nc_iseq.cbegin(); it != nc_iseq.cend(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto it = cn_iseq.cbegin(); it != cn_iseq.cend(); ++it)
                actual.push_back((*it).val());
            EXPECT_EQ(expected, actual);
        }
    }
    // iter children
    {
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.children())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.children())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.children())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
    }
    // iter cchildren
    {
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.cchildren())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.cchildren())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
        {
            std::vector<csubstr> actual;
            for(auto r : n_iseq.cchildren())
                actual.push_back(r.val());
            EXPECT_EQ(expected, actual);
        }
    }
}

TEST(NodeRef, get_if)
{
    const Tree tree = parse_in_arena("{a: 1, b: 2}");
    ConstNodeRef root = tree.rootref();
    int val = 0;
    EXPECT_TRUE(root.get_if("a", &val));
    EXPECT_EQ(val, 1);
    EXPECT_TRUE(root.get_if("b", &val));
    EXPECT_EQ(val, 2);
    EXPECT_FALSE(root.get_if("c", &val));
    EXPECT_EQ(val, 2);
    int fallback = 3;
    EXPECT_TRUE(root.get_if("a", &val, fallback));
    EXPECT_EQ(val, 1);
    EXPECT_TRUE(root.get_if("b", &val, fallback));
    EXPECT_EQ(val, 2);
    EXPECT_FALSE(root.get_if("c", &val, fallback));
    EXPECT_EQ(val, 3);
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
