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

// inf and nan are tested in test_number.cpp

RYML_DEFINE_TEST_MAIN()

using substr = c4::substr;
using csubstr = c4::csubstr;

//-------------------------------------------

namespace c4 {
namespace yml {

namespace {
constexpr bool nocheck = true;
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_notree_impl(Tree &t, T var)
{
    NodeRef notree;
    ConstNodeRef cnotree = notree;
    id_type id = notree.id();
    ASSERT_EQ(cnotree.id(), notree.id());
    RYML_EXPECT_ERROR(check_error_basic(&t, [&]{  notree.load(var); }));
    RYML_EXPECT_ERROR(check_error_basic(&t, [&]{ cnotree.load(var); }));
    RYML_EXPECT_ERROR(check_error_basic(&t, [&]{  notree.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_basic(&t, [&]{ cnotree.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(id, var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(id, var); }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{  notree.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ cnotree.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{  notree.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ cnotree.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ t.load(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ t.load_key(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ bool ok =  notree.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ bool ok = cnotree.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ bool ok =  notree.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_basic(&t, [&]{ bool ok = cnotree.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize(id, var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize_key(id, var); (void)ok; }));
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_seednode_impl(Tree &t, T var, NodeRef seednode)
{
    ConstNodeRef cseednode = seednode;
    id_type id = NONE;
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  seednode.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ cseednode.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  seednode.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ cseednode.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(id, var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(id, var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  seednode.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cseednode.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  seednode.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cseednode.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ t.load(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ t.load_key(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  seednode.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cseednode.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  seednode.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cseednode.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize(id, var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize_key(id, var); (void)ok; }));
}

template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_container(Tree &t, T var, NodeRef container)
{
    ConstNodeRef ccontainer = container;
    id_type id = container.id();
    ASSERT_EQ(ccontainer.id(), container.id());
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  container.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  ccontainer.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  container.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ ccontainer.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(id, var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(id, var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  container.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ ccontainer.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  container.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ ccontainer.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(id, var, nocheck); }));
    EXPECT_FALSE( container.deserialize(var));
    EXPECT_FALSE(ccontainer.deserialize(var));
    EXPECT_FALSE( container.deserialize_key(var));
    EXPECT_FALSE(ccontainer.deserialize_key(var));
    EXPECT_FALSE(t.deserialize(id, var));
    EXPECT_FALSE(t.deserialize_key(id, var));
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_noval(Tree &t, T var, NodeRef noval)
{
    ConstNodeRef cnoval = noval;
    id_type id = cnoval.id();
    ASSERT_EQ(cnoval.id(), noval.id());
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{   noval.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  cnoval.load(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  t.load(id, var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{   noval.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  cnoval.load(var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  t.load(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  noval.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cnoval.deserialize(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize(noval.id(), var); (void)ok; }));
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_impl_nokey(Tree &t, T var, NodeRef nokey)
{
    ConstNodeRef cnokey = nokey;
    id_type id = cnokey.id();
    ASSERT_EQ(cnokey.id(), nokey.id());
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{   nokey.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  cnokey.load_key(var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  t.load_key(id, var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{   nokey.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  cnokey.load_key(var, nocheck); }));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{  t.load_key(id, var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  nokey.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cnokey.deserialize_key(var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = t.deserialize_key(nokey.id(), var); (void)ok; }));
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_impl(Tree &t, T var, NodeRef seednode, NodeRef nokey, NodeRef noval)
{
    do_test_error_deserializing_unreadable_nodes_notree_impl(t, var);
    do_test_error_deserializing_unreadable_nodes_seednode_impl(t, var, seednode);
    do_test_error_deserializing_unreadable_nodes_noval(t, var, noval);
    do_test_error_deserializing_unreadable_nodes_impl_nokey(t, var, nokey);
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes(Tree &t, T &var, NodeRef nonode, NodeRef nokey, NodeRef noval)
{
    do_test_error_deserializing_unreadable_nodes_impl<T*>(t, &var, nonode, nokey, noval);
}
template<class T>
C4_NO_INLINE void do_test_error_deserializing_unreadable_nodes_wrapper(Tree &t, T const& var, NodeRef nonode, NodeRef nokey, NodeRef noval)
{
    do_test_error_deserializing_unreadable_nodes_impl<T const&>(t, var, nonode, nokey, noval);
}
template<class T>
C4_NO_INLINE void check_err_common(NodeRef n)
{
    Tree &t = *n.tree();
    T var;
    ConstNodeRef cn = n;
    id_type id = n.id();
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  n.load(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{ cn.load(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  t.load(id, &var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  n.load(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cn.load(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  t.load(id, &var, nocheck); }));
    EXPECT_FALSE( n.deserialize(&var));
    EXPECT_FALSE(cn.deserialize(&var));
    EXPECT_FALSE( t.deserialize(id, &var));
}
template<class T>
C4_NO_INLINE void check_err_common_noval(NodeRef n)
{
    Tree &t = *n.tree();
    T var;
    ConstNodeRef cn = n;
    id_type id = n.id();
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  n.load(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{ cn.load(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  t.load(id, &var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  n.load(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cn.load(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  t.load(id, &var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  n.deserialize(&var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cn.deserialize(&var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  t.deserialize(id, &var); (void)ok; }));
}
template<class T>
C4_NO_INLINE void check_err_common_key(NodeRef n)
{
    Tree &t = *n.tree();
    T var;
    ConstNodeRef cn = n;
    id_type id = n.id();
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  n.load_key(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{ cn.load_key(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  t.load_key(id, &var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  n.load_key(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cn.load_key(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  t.load_key(id, &var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  n.deserialize_key(&var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok = cn.deserialize_key(&var); (void)ok; }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ bool ok =  t.deserialize_key(id, &var); (void)ok; }));
}
template<class T>
C4_NO_INLINE void check_err_common_key_but_empty(NodeRef n)
{
    Tree &t = *n.tree();
    T var;
    ConstNodeRef cn = n;
    id_type id = n.id();
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  n.load_key(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{ cn.load_key(&var); }));
    RYML_EXPECT_ERROR(check_error_visit(&t,  [&]{  t.load_key(id, &var); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  n.load_key(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{ cn.load_key(&var, nocheck); }));
    RYML_EXPECT_ERROR(check_assert_visit(&t, [&]{  t.load_key(id, &var, nocheck); }));
    EXPECT_FALSE( n.deserialize_key(&var));
    EXPECT_FALSE(cn.deserialize_key(&var));
    EXPECT_FALSE(t.deserialize_key(id, &var));
}
template<class T, class Cmp>
C4_NO_INLINE void check_deserialize_scalar_key(NodeRef n, T const& expected, Cmp const& fn)
{
    Tree &t = *n.tree();
    T v;
    ConstNodeRef cn = n;
    v = {}; ASSERT_TRUE( n.deserialize_key(&v));         EXPECT_TRUE(fn(expected, v));
    v = {}; ASSERT_TRUE(cn.deserialize_key(&v));         EXPECT_TRUE(fn(expected, v));
    v = {}; ASSERT_TRUE( t.deserialize_key(n.id(), &v)); EXPECT_TRUE(fn(expected, v));
    v = {};  n.load_key(&v);                             EXPECT_TRUE(fn(expected, v));
    v = {};  n.load_key(&v, nocheck);                    EXPECT_TRUE(fn(expected, v));
    v = {}; cn.load_key(&v);                             EXPECT_TRUE(fn(expected, v));
    v = {}; cn.load_key(&v, nocheck);                    EXPECT_TRUE(fn(expected, v));
    v = {};  t.load_key(n.id(), &v);                     EXPECT_TRUE(fn(expected, v));
    v = {};  t.load_key(n.id(), &v);                     EXPECT_TRUE(fn(expected, v));
}
template<class T, class Cmp>
C4_NO_INLINE void check_deserialize_scalar_val(NodeRef n, T const& expected, Cmp const& fn)
{
    Tree &t = *n.tree();
    T v;
    ConstNodeRef cn = n;
    v = {}; ASSERT_TRUE( n.deserialize(&v));         EXPECT_TRUE(fn(expected, v));
    v = {}; ASSERT_TRUE(cn.deserialize(&v));         EXPECT_TRUE(fn(expected, v));
    v = {}; ASSERT_TRUE( t.deserialize(n.id(), &v)); EXPECT_TRUE(fn(expected, v));
    v = {};  n.load(&v);                             EXPECT_TRUE(fn(expected, v));
    v = {};  n.load(&v, nocheck);                    EXPECT_TRUE(fn(expected, v));
    v = {}; cn.load(&v);                             EXPECT_TRUE(fn(expected, v));
    v = {}; cn.load(&v, nocheck);                    EXPECT_TRUE(fn(expected, v));
    v = {};  t.load(n.id(), &v);                     EXPECT_TRUE(fn(expected, v));
    v = {};  t.load(n.id(), &v);                     EXPECT_TRUE(fn(expected, v));
}
} // namespace


TEST(serialize, scalar_bool)
{
    SCOPED_TRACE("scalar bool");
    csubstr yaml = R"(
map: {true: false, 0: 1, True: False}
seq: [true, false, 0, 1, True, False]
noval:
: nokey
)";
    auto cmp = [](bool lhs, bool rhs) { return lhs == rhs; };
    Tree t = parse_in_arena(yaml);
    check_err_common<bool>(t);
    check_err_common<bool>(t["map"]);
    check_deserialize_scalar_key(t["map"][0], true,  cmp);
    check_deserialize_scalar_val(t["map"][0], false, cmp);
    check_deserialize_scalar_key(t["map"][1], false, cmp);
    check_deserialize_scalar_val(t["map"][1], true,  cmp);
    check_deserialize_scalar_key(t["map"][2], true,  cmp);
    check_deserialize_scalar_val(t["map"][2], false, cmp);
    check_err_common<bool>      (t["seq"]);
    check_err_common_key<bool>  (t["seq"][0]);
    check_deserialize_scalar_val(t["seq"][0], true, cmp);
    check_err_common_key<bool>  (t["seq"][1]);
    check_deserialize_scalar_val(t["seq"][1], false, cmp);
    check_err_common_key<bool>  (t["seq"][2]);
    check_deserialize_scalar_val(t["seq"][2], false, cmp);
    check_err_common_key<bool>  (t["seq"][3]);
    check_deserialize_scalar_val(t["seq"][3], true, cmp);
    check_err_common_key<bool>  (t["seq"][4]);
    check_deserialize_scalar_val(t["seq"][4], true, cmp);
    check_err_common_key<bool>  (t["seq"][5]);
    check_deserialize_scalar_val(t["seq"][5], false, cmp);
    check_err_common<bool>      (t["noval"]);
    check_err_common_key_but_empty<bool>(t[""]);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<bool>(empty_on_map);
    check_err_common_key<bool>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<bool>(empty_on_seq);
    check_err_common_key<bool>(empty_on_seq);
}
template<class T, class Cmp>
C4_NO_INLINE static void test_scalar_arithmetic(Cmp const& cmp)
{
    SCOPED_TRACE("scalar arithmetic");
    csubstr yaml = R"(
map: {0: 10, 1: 11, 2: 12}
seq: [0, 1, 2, 3]
noval:
: nokey
)";
    Tree t = parse_in_arena(yaml);
    check_err_common<T>(t);
    check_err_common<T>(t["map"]);
    check_deserialize_scalar_key(t["map"][0], T(0), cmp);
    check_deserialize_scalar_val(t["map"][0], T(10), cmp);
    check_deserialize_scalar_key(t["map"][1], T(1), cmp);
    check_deserialize_scalar_val(t["map"][1], T(11), cmp);
    check_deserialize_scalar_key(t["map"][2], T(2), cmp);
    check_deserialize_scalar_val(t["map"][2], T(12), cmp);
    check_err_common<T>         (t["seq"]);
    check_err_common_key<T>     (t["seq"][0]);
    check_deserialize_scalar_val(t["seq"][0], T(0), cmp);
    check_err_common_key<T>     (t["seq"][1]);
    check_deserialize_scalar_val(t["seq"][1], T(1), cmp);
    check_err_common_key<T>     (t["seq"][2]);
    check_deserialize_scalar_val(t["seq"][2], T(2), cmp);
    check_err_common_key<T>     (t["seq"][3]);
    check_deserialize_scalar_val(t["seq"][3], T(3), cmp);
    check_err_common<T>         (t["noval"]);
    check_err_common_key_but_empty<T>(t[""]);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<T>(empty_on_map);
    check_err_common_key<T>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<T>(empty_on_seq);
    check_err_common_key<T>(empty_on_seq);
}
template<class T, class Cmp>
C4_NO_INLINE static void test_scalar_str(Cmp const& cmp)
{
    SCOPED_TRACE("fundamental");
    csubstr yaml = R"(
map: {0: 10, 1: 11, 2: 12}
seq: [0, 1, 2, 3]
noval:
: nokey
)";
    Tree t = parse_in_arena(yaml);
    check_err_common<T>(t);
    check_err_common<T>(t["map"]);
    check_deserialize_scalar_key(t["map"], T("map"), cmp);
    check_deserialize_scalar_key(t["map"][0], T("0"), cmp);
    check_deserialize_scalar_val(t["map"][0], T("10"), cmp);
    check_deserialize_scalar_key(t["map"][1], T("1"), cmp);
    check_deserialize_scalar_val(t["map"][1], T("11"), cmp);
    check_deserialize_scalar_key(t["map"][2], T("2"), cmp);
    check_deserialize_scalar_val(t["map"][2], T("12"), cmp);
    check_err_common<T>         (t["seq"]);
    check_err_common_key<T>     (t["seq"][0]);
    check_deserialize_scalar_val(t["seq"][0], T("0"), cmp);
    check_err_common_key<T>     (t["seq"][1]);
    check_deserialize_scalar_val(t["seq"][1], T("1"), cmp);
    check_err_common_key<T>     (t["seq"][2]);
    check_deserialize_scalar_val(t["seq"][2], T("2"), cmp);
    check_err_common_key<T>     (t["seq"][3]);
    check_deserialize_scalar_val(t["seq"][3], T("3"), cmp);
    check_deserialize_scalar_key(t["noval"], T("noval"), cmp);
    check_deserialize_scalar_val(t["noval"], T(""), cmp);
    check_deserialize_scalar_key(t[""], T(""), cmp);
    check_deserialize_scalar_val(t[""], T("nokey"), cmp);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<T>(empty_on_map);
    check_err_common_key<T>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<T>(empty_on_seq);
    check_err_common_key<T>(empty_on_seq);
}
TEST(serialize, scalar_int)
{
    using T = int;
    test_scalar_arithmetic<T>([](T a, T b) { return a == b; });
}
TEST(serialize, scalar_uint)
{
    using T = uint32_t;
    test_scalar_arithmetic<T>([](T a, T b) { return a == b; });
}
TEST(serialize, scalar_float)
{
    using T = float;
    test_scalar_arithmetic<T>([](T a, T b) { return 0 == memcmp(&a, &b, sizeof(T)); });
}
TEST(serialize, scalar_double)
{
    using T = double;
    test_scalar_arithmetic<T>([](T a, T b) { return 0 == memcmp(&a, &b, sizeof(T)); });
}
TEST(serialize, scalar_std_string)
{
    using T = std::string;
    test_scalar_str<T>([](T const& a, T const& b) { return a == b; });
}
TEST(serialize, scalar_csubstr)
{
    using T = csubstr;
    test_scalar_str<T>([](T a, T b) { return a == b; });
}

TEST(serialize, vecbool)
{
    SCOPED_TRACE("test");
    csubstr yaml = R"(
map: {true: false, 0: 1, True: False}
map empty: {}
seq: [true, false, 0, 1, True, False]
seq empty: []
noval:
: nokey
)";
    Tree t = parse_in_arena(yaml);
    using Vec = std::vector<bool>;
    Vec expected{{true, false, false, true, true, false}};
    auto cmp = [](Vec const& lhs, Vec const& rhs){
        return lhs == rhs;
    };
    check_err_common<Vec>(t);
    check_err_common<Vec>(t["map"]);
    check_err_common<Vec>(t["map"][0]);
    check_err_common<Vec>(t["map"][1]);
    check_err_common<Vec>(t["map"][2]);
    check_err_common<Vec>(t["map empty"]);
    check_deserialize_scalar_val(t["seq"], expected, cmp);
    check_err_common<Vec>(t["seq"][0]);
    check_err_common<Vec>(t["seq"][1]);
    check_err_common<Vec>(t["seq"][2]);
    check_err_common<Vec>(t["seq"][3]);
    check_err_common<Vec>(t["seq"][4]);
    check_err_common<Vec>(t["seq"][5]);
    check_deserialize_scalar_val(t["seq empty"], Vec{}, cmp);
    check_err_common<Vec>(t["noval"]);
    check_err_common<Vec>(t[""]);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<Vec>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<Vec>(empty_on_seq);
}

template<class Vec, class Cmp>
C4_NO_INLINE static void test_container_vec(Vec const& expected, Cmp const& cmp)
{
    SCOPED_TRACE("fundamental");
    csubstr yaml = R"(
map: {0: 10, 1: 11, 2: 12}
map empty: {}
seq: [0, 1, 2, 3]
seq empty: []
null:
)";
    Tree t = parse_in_arena(yaml);
    check_err_common<Vec>(t);
    check_err_common<Vec>(t["map"]);
    check_err_common<Vec>(t["map"]["0"]);
    check_err_common<Vec>(t["map"]["1"]);
    check_err_common<Vec>(t["map"]["2"]);
    check_err_common<Vec>(t["map empty"]);
    check_deserialize_scalar_val(t["seq"], expected, cmp);
    check_deserialize_scalar_val(t["seq empty"], Vec{}, cmp);
    check_err_common<Vec>(t["null"]);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<Vec>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<Vec>(empty_on_seq);
}
template<class T>
C4_NO_INLINE static void test_vector_int()
{
    using Vec = std::vector<T>;
    Vec expected{{T(0), T(1), T(2), T(3)}};
    test_container_vec<Vec>(expected, [](Vec const& lhs, Vec const& rhs){
        return lhs == rhs;
    });
}
template<class T>
C4_NO_INLINE static void test_vector_str()
{
    using Vec = std::vector<T>;
    Vec expected{{T("0"), T("1"), T("2"), T("3")}};
    test_container_vec<Vec>(expected, [](Vec const& lhs, Vec const& rhs){
        return lhs == rhs;
    });
}
template<class T>
C4_NO_INLINE static void test_vector_float()
{
    using Vec = std::vector<T>;
    Vec expected{{T(0), T(1), T(2), T(3)}};
    test_container_vec<Vec>(expected, [](Vec const& lhs, Vec const& rhs){
        if(lhs.size() != rhs.size())
            return false;
        if(lhs.empty())
            return true;
        return 0 == memcmp(lhs.data(), rhs.data(), sizeof(T) * lhs.size());
    });
}
TEST(serialize, container_vector_int)
{
    test_vector_int<int>();
}
TEST(serialize, container_vector_uint)
{
    test_vector_int<uint32_t>();
}
TEST(serialize, container_vector_float)
{
    test_vector_float<float>();
}
TEST(serialize, container_vector_double)
{
    test_vector_float<double>();
}
TEST(serialize, container_vector_std_string)
{
    test_vector_str<std::string>();
}
TEST(serialize, container_vector_csubstr)
{
    test_vector_str<csubstr>();
}

template<class Map, class Cmp>
C4_NO_INLINE static void test_container_map(Map const& expected, Cmp const& cmp)
{
    SCOPED_TRACE("fundamental");
    csubstr yaml = R"(
map: {0: 10, 1: 11, 2: 12}
map empty: {}
seq: [0, 1, 2, 3]
seq empty: []
null:
)";
    Tree t = parse_in_arena(yaml);
    check_err_common<Map>(t);
    check_deserialize_scalar_val(t["map"], expected, cmp);
    check_err_common<Map>(t["map"]["0"]);
    check_err_common<Map>(t["map"]["1"]);
    check_err_common<Map>(t["map"]["2"]);
    check_deserialize_scalar_val(t["map empty"], Map{}, cmp);
    check_err_common<Map>(t["seq"]);
    check_err_common<Map>(t["seq"][0]);
    check_err_common<Map>(t["seq"][1]);
    check_err_common<Map>(t["seq"][2]);
    check_err_common<Map>(t["seq"][3]);
    check_err_common<Map>(t["seq empty"]);
    check_err_common<Map>(t["null"]);
    NodeRef empty_on_map = t["map"].append_child();
    check_err_common_noval<Map>(empty_on_map);
    NodeRef empty_on_seq = t["seq"].append_child();
    check_err_common_noval<Map>(empty_on_seq);
}
template<class K, class V=K>
C4_NO_INLINE static void test_map_int()
{
    using Map = std::map<K, V>;
    Map expected{{{K(0), V(10)}, {K(1), V(11)}, {K(2), V(12)}}};
    test_container_map<Map>(expected, [](Map const& lhs, Map const& rhs){
        return lhs == rhs;
    });
}
template<class K, class V=K>
C4_NO_INLINE static void test_map_str()
{
    using Map = std::map<K, K>;
    Map expected{{{K("0"), V("10")}, {K("1"), V("11")}, {K("2"), V("12")}}};
    test_container_map<Map>(expected, [](Map const& lhs, Map const& rhs){
        return lhs == rhs;
    });
}
template<class K, class V=K>
C4_NO_INLINE static void test_map_float()
{
    using Map = std::map<K, V>;
    Map expected{{{K(0), V(10)}, {K(1), V(11)}, {K(2), V(12)}}};
    test_container_map<Map>(expected, [](Map const& lhs, Map const& rhs){
        if(lhs.size() == rhs.size())
        {
            for(auto const& p : lhs)
            {
                auto it = rhs.find(p.first);
                if(it != rhs.end())
                    if(0 == memcmp(&p.second, &it->second, sizeof(V))) // NOLINT
                        continue;
                return false;
            }
        }
        return true;
    });
}
TEST(serialize, container_map_int)
{
    test_map_int<int>();
}
TEST(serialize, container_map_uint)
{
    test_map_int<uint32_t>();
}
TEST(serialize, container_map_float)
{
    test_map_float<int, float>();
}
TEST(serialize, container_map_double)
{
    test_map_float<int, double>();
}
TEST(serialize, container_map_std_string)
{
    test_map_str<std::string>();
}
TEST(serialize, container_map_csubstr)
{
    test_map_str<csubstr>();
}

TEST(serialize, base64_2)
{
    csubstr yaml = R"(str: Brevity is the soul of wit.
base64: QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu
QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu: base64
)";
    Tree t = parse_in_arena(yaml);
    NodeRef mr = t;
    ConstNodeRef r = t;
    ASSERT_TRUE(r.type().is_map());
    ASSERT_EQ(r[0].key(), "str");
    ASSERT_EQ(r[0].val(), "Brevity is the soul of wit.");
    ASSERT_EQ(r[1].key(), "base64");
    ASSERT_EQ(r[1].val(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].key(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].val(), "base64");
    {
        std::string s, fallback = "fallback";
        s = {}; ASSERT_TRUE( r[0].deserialize_key(&s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(mr[0].deserialize_key(&s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE( r[0].deserialize(&s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].deserialize(&s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(0, &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(0, &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(0, &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(0, &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("str", &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("str", &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("str", &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("str", &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("!!!", &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("!!!", &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[1].deserialize_key(&s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[1].deserialize_key(&s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[1].deserialize(&s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].deserialize(&s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(1, &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(1, &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(1, &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(1, &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("base64", &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("base64", &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("base64", &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("base64", &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("!!!!!!", &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("!!!!!!", &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[2].deserialize_key(&s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[2].deserialize_key(&s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[2].deserialize(&s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].deserialize(&s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(2, &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(2, &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(2, &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(2, &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[0].id(), &s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[0].id(), &s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize( r[0].id(), &s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize(mr[0].id(), &s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 0, &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 0, &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 0, &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 0, &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 20, &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 20, &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "str", &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "str", &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "str", &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "str", &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "!!!", &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "!!!", &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[1].id(), &s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[1].id(), &s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize( r[1].id(), &s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize(mr[1].id(), &s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 1, &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 1, &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 1, &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 1, &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 20, &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 20, &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "base64", &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "base64", &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "base64", &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "base64", &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "!!!!!!", &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "!!!!!!", &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[2].id(), &s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[2].id(), &s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize( r[2].id(), &s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize(mr[2].id(), &s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 2, &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 2, &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 2, &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 2, &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 21, &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 21, &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        //
        s = {};  r[0].load_key(&s); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; mr[0].load_key(&s); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {};  r[0].load(&s);     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; mr[0].load(&s);     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {};  r[1].load_key(&s); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; mr[1].load_key(&s); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {};  r[1].load(&s);     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; mr[1].load(&s);     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {};  r[2].load_key(&s); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; mr[2].load_key(&s); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {};  r[2].load(&s);     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; mr[2].load(&s);     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; t.load_key( r[0].id(), &s); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; t.load_key(mr[0].id(), &s); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; t.load( r[0].id(), &s);     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; t.load(mr[0].id(), &s);     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; t.load_key( r[1].id(), &s); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; t.load_key(mr[1].id(), &s); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; t.load( r[1].id(), &s);     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load(mr[1].id(), &s);     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load_key( r[2].id(), &s); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load_key(mr[2].id(), &s); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load( r[2].id(), &s);     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; t.load(mr[2].id(), &s);     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
    }
    std::string buf;
    size_t szk = 0;
    #define testonbuf(...) { SCOPED_TRACE("here"); testonbuf_(__VA_ARGS__); }
    auto testonbuf_ = [&](std::function<void()> const& fn){
        to_substr(buf).fill('a');
        EXPECT_NE(buf, "Brevity is the soul of wit.");
        fn();
        EXPECT_TRUE(r[1].deserialize(fmt::base64(buf, &szk)));
        EXPECT_EQ(buf, "Brevity is the soul of wit.");
    };
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_child(r[1].parent().id(), 1, fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_child(r[1].parent().id(), r[1].key(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize(r[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_key(r[2].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ t.load(r[1].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load(r[1].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load(mr[1].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load(mr[1].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load_key(r[2].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load_key(r[2].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load_key(mr[2].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load_key(mr[2].id(), fmt::base64(to_substr(buf), &szk)); });
    //
    testonbuf([&]{ EXPECT_TRUE(r[1].parent().deserialize_child(1, fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[1].parent().deserialize_child(r[1].key(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[1].deserialize(fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[2].deserialize_key(fmt::base64(buf, &szk))); });
    testonbuf([&]{ r[1].load(fmt::base64(buf, &szk)); });
    testonbuf([&]{ r[1].load(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ mr[1].load(fmt::base64(buf, &szk)); });
    testonbuf([&]{ mr[1].load(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ r[2].load_key(fmt::base64(buf, &szk)); });
    testonbuf([&]{ r[2].load_key(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ mr[2].load_key(fmt::base64(buf, &szk)); });
    testonbuf([&]{ mr[2].load_key(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ EXPECT_TRUE(read(r[1], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(mr[1], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(r[2], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), fmt::base64(buf, &szk))); });
    {
        auto w = fmt::base64(buf, &szk);
        testonbuf([&]{ EXPECT_TRUE(read(r[1], w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1], w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2], w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), &w)); });
    }
    #undef testonbuf
    //
    auto checkerr_key = [&](ConstNodeRef node){
        ASSERT_EQ(node.tree(), &t);
        ASSERT_TRUE(node.type().has_key());
        id_type id = node.id();
        auto w = fmt::base64(buf, &szk);
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(to_substr(buf), &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(to_substr(buf), &szk)); }, id));
        EXPECT_FALSE(t.deserialize_key(id, w));
        EXPECT_FALSE(t.deserialize_key(id, &w));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(buf)));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(node.deserialize_key(w));
        EXPECT_FALSE(node.deserialize_key(&w));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(buf)));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(buf, &szk)));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(to_substr(buf))));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read_key(node, w));
        EXPECT_FALSE(read_key(node, &w));
        EXPECT_FALSE(read_key(node, fmt::base64(buf)));
        EXPECT_FALSE(read_key(node, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read_key(node, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read_key(node, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read_key(&t, id, w));
        EXPECT_FALSE(read_key(&t, id, &w));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(buf)));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(to_substr(buf), &szk)));
    };
    auto checkerr_val = [&](ConstNodeRef node){
        ASSERT_EQ(node.tree(), &t);
        id_type id = node.id();
        auto w = fmt::base64(buf, &szk);
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(to_substr(buf), &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(to_substr(buf), &szk)); }, id));
        EXPECT_FALSE(t.deserialize(id, w));
        EXPECT_FALSE(t.deserialize(id, &w));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(buf)));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(to_substr(buf), &szk)));
        if(t.has_parent(id))
        {
            id_type parent = t.parent(id), pos = t.child_pos(parent, id);
            csubstr key = t.key(id);
            EXPECT_FALSE(t.deserialize_child(parent, pos, w));
            EXPECT_FALSE(t.deserialize_child(parent, key, w));
            EXPECT_FALSE(t.deserialize_child(parent, pos, &w));
            EXPECT_FALSE(t.deserialize_child(parent, key, &w));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(buf)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(buf)));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(buf, &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(buf, &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(to_substr(buf), &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(to_substr(buf), &szk)));
        }
        EXPECT_FALSE(node.deserialize(w));
        EXPECT_FALSE(node.deserialize(&w));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf)));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf, &szk)));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf))));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf), &szk)));
        if(t.has_parent(id))
        {
            ConstNodeRef nparent = node.parent();
            id_type pos = nparent.child_pos(node);
            csubstr key = t.key(id);
            EXPECT_FALSE(nparent.deserialize_child(pos, w));
            EXPECT_FALSE(nparent.deserialize_child(key, w));
            EXPECT_FALSE(nparent.deserialize_child(pos, &w));
            EXPECT_FALSE(nparent.deserialize_child(key, &w));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(buf)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(buf)));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(buf, &szk)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(buf, &szk)));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(to_substr(buf), &szk)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(to_substr(buf), &szk)));
        }
        EXPECT_FALSE(read(node, w));
        EXPECT_FALSE(read(node, &w));
        EXPECT_FALSE(read(node, fmt::base64(buf)));
        EXPECT_FALSE(read(node, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read(node, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read(node, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read(node.tree(), id, w));
        EXPECT_FALSE(read(node.tree(), id, &w));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(buf)));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(to_substr(buf), &szk)));
    };
    {
        SCOPED_TRACE("r[1] key is not base64");
        EXPECT_FALSE(base64_valid(r[1].key().str, r[1].key().len));
        checkerr_key(r[1]);
    }
    {
        SCOPED_TRACE("r[2] val is not base64");
        EXPECT_FALSE(base64_valid(r[2].val().str, r[2].val().len));
        checkerr_val(r[2]);
    }
    {
        SCOPED_TRACE("container as base64");
        checkerr_val(r);
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

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
