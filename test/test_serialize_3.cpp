#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/format_base64.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_helpers_tree.hpp"

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

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
