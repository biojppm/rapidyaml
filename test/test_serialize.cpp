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


//-----------------------------------------------------------------------------

namespace foo {
template<class T> struct vec2 { T x, y; };
template<class T> size_t to_chars(substr buf, vec2<T> v) { return c4::format(buf, "({},{})", v.x, v.y); }
template<class T> bool from_chars(csubstr buf, vec2<T> *v) { return c4::yml::npos != c4::unformat(buf, "({},{})", v->x, v->y); }

template<class T> struct vec3 { T x, y, z; };
template<class T> size_t to_chars(substr buf, vec3<T> v) { return c4::format(buf, "({},{},{})", v.x, v.y, v.z); }
template<class T> bool from_chars(csubstr buf, vec3<T> *v) { return c4::yml::npos != c4::unformat(buf, "({},{},{})", v->x, v->y, v->z); }

template<class T> struct vec4 { T x, y, z, w; };
template<class T> size_t to_chars(substr buf, vec4<T> v) { return c4::format(buf, "({},{},{},{})", v.x, v.y, v.z, v.w); }
template<class T> bool from_chars(csubstr buf, vec4<T> *v) { return c4::yml::npos != c4::unformat(buf, "({},{},{},{})", v->x, v->y, v->z, v->w); }
} // namespace foo


//-----------------------------------------------------------------------------

namespace foo2 {
template<class T> struct vec2 { T x, y; };
template<class T> size_t serialize_scalar(substr buf, vec2<T> v) { return c4::format(buf, "({},{})", v.x, v.y); }
template<class T> bool from_chars(csubstr buf, vec2<T> *v) { return c4::yml::npos != c4::unformat(buf, "({},{})", v->x, v->y); }
} // namespace foo2


//-----------------------------------------------------------------------------

template<class Vec2>
csubstr arena_helper_vec2(c4::yml::Tree * tree, Vec2 v)
{
    substr buf(tree->arena_rem()); // get the free part of the tree's arena.
    size_t num = c4::format(buf, "({},{})", v.x, v.y); // try to write into it
    substr ret = tree->alloc_arena(num); // consume the required size
    if(num > buf.len) // was the buffer too small?
        c4::format(ret, "({},{})", v.x, v.y); // do it again with the new buffer
    return ret;
}

template<class Vec2>
bool from_chars_helper_vec2(csubstr buf, Vec2 *v)
{
    return c4::yml::npos != c4::unformat(buf, "({},{})", v->x, v->y);
}


namespace foo3 {
template<class T> struct vec2 { T x, y; };
template<class T> csubstr serialize_to_arena(c4::yml::Tree * tree, vec2<T> v)
{
    return arena_helper_vec2(tree, v);
}
template<class T> bool from_chars(csubstr buf, vec2<T> *v)
{
    return from_chars_helper_vec2(buf, v);
}
} // namespace foo3


//-----------------------------------------------------------------------------

namespace foo4 {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::Tree * tree, c4::yml::id_type id, vec2<T> const& v)
{
    tree->set_val(id, arena_helper_vec2(tree, v));
}
template<class T> void write_key(c4::yml::Tree *tree, c4::yml::id_type id, vec2<T> v)
{
    tree->set_key(id, arena_helper_vec2(tree, v));
}
template<class T> bool read(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    return from_chars_helper_vec2(tree->val(id), v);
}
template<class T> bool read_key(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    return from_chars_helper_vec2(tree->key(id), v);
}
} // namespace foo4


//-----------------------------------------------------------------------------

namespace foo5 {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef *n, vec2<T> v)
{
    n->set_val(arena_helper_vec2(n->tree(), v));
}
template<class T> void write_key(c4::yml::NodeRef *n, vec2<T> v)
{
    n->set_key(arena_helper_vec2(n->tree(), v));
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.val(), v);
}
template<class T> bool read_key(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.key(), v);
}
} // namespace foo5


namespace foo5_ref {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_val(arena_helper_vec2(n.tree(), v));
}
template<class T> void write_key(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_key(arena_helper_vec2(n.tree(), v));
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.val(), v);
}
template<class T> bool read_key(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.key(), v);
}
} // namespace foo5_ref


namespace foo5_pass_by_value {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef n, vec2<T> v)
{
    n.set_val(arena_helper_vec2(n.tree(), v));
}
template<class T> void write_key(c4::yml::NodeRef n, vec2<T> v)
{
    n.set_key(arena_helper_vec2(n.tree(), v));
}
template<class T> bool read(c4::yml::ConstNodeRef n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.val(), v);
}
template<class T> bool read_key(c4::yml::ConstNodeRef n, vec2<T> *v)
{
    return from_chars_helper_vec2<vec2<T>>(n.key(), v);
}
} // namespace foo5


//-----------------------------------------------------------------------------

namespace foo6_optimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::Tree *tree, c4::yml::id_type id, vec2<T> v)
{
    tree->set_map(id);
    c4::yml::id_type x = tree->append_child(id);
    c4::yml::id_type y = tree->append_child(id);
    write_key(tree, x, "x"); // copies "x" to the arena. to avoid copying, use tree->set_key(x, "x")
    write_key(tree, y, "y"); // copies "y" to the arena. to avoid copying, use tree->set_key(y, "y")
    write(tree, x, v.x); // serialize the value of x to the arena
    write(tree, y, v.y); // serialize the value of y to the arena
}
template<class T> bool read(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    return read(tree, tree->find_child(id, "x"), &v->x)
        && read(tree, tree->find_child(id, "y"), &v->y);
}
} // namespace foo6_optimistic


namespace foo6_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::Tree *tree, c4::yml::id_type id, vec2<T> v)
{
    tree->set_map(id);
    c4::yml::id_type x = tree->append_child(id);
    c4::yml::id_type y = tree->append_child(id);
    write_key(tree, x, "x"); // copies "x" to the arena. to avoid copying, use tree->set_key(x, "x")
    write_key(tree, y, "y"); // copies "y" to the arena. to avoid copying, use tree->set_key(y, "y")
    write(tree, x, v.x); // serialize the value of x to the arena
    write(tree, y, v.y); // serialize the value of y to the arena
}
template<class T> bool read(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    using namespace c4::yml;
    if(!tree->is_map(id))
        return false;
    id_type x = tree->find_child(id, "x");
    id_type y = tree->find_child(id, "y");
    if(x == NONE || y == NONE)
        return false;
    return read(tree, x, &v->x) && read(tree, y, &v->y);
}
} // namespace foo6


//-----------------------------------------------------------------------------

namespace foo7_optimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_map();
    n["x"] << v.x;
    n["y"] << v.y;
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    n["x"] >> v->x;
    n["y"] >> v->y;
    return true;
}
} // namespace foo5


namespace foo7_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_map();
    n["x"] << v.x;
    n["y"] << v.y;
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    using namespace c4::yml;
    if(!n.readable() || !n.is_map())
        return false;
    ConstNodeRef x = n["x"];
    ConstNodeRef y = n["y"];
    if(!x.readable() || !y.readable())
        return false;
    x >> v->x; // throws on error. to keep running, use x.deserialize_val() which returns bool
    y >> v->y; // throws on error. to keep running, use y.deserialize_val() which returns bool
    return true;
}
} // namespace foo7_pessimistic


//-----------------------------------------------------------------------------

namespace foo8_optimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::Tree *tree, c4::yml::id_type id, vec2<T> v)
{
    tree->set_seq(id);
    write(tree, tree->append_child(id), v.x); // serialize the value of x to the arena
    write(tree, tree->append_child(id), v.y); // serialize the value of y to the arena
}
template<class T> bool read(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    c4::yml::id_type x = tree->first_child(id);
    c4::yml::id_type y = tree->next_sibling(x); // of x!
    return read(tree, x, &v->x) && read(tree, y, &v->y);
}
} // namespace foo8_optimistic


namespace foo8_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::Tree *tree, c4::yml::id_type id, vec2<T> v)
{
    tree->set_seq(id);
    write(tree, tree->append_child(id), v.x); // serialize the value of x to the arena
    write(tree, tree->append_child(id), v.y); // serialize the value of y to the arena
}
template<class T> bool read(c4::yml::Tree const* tree, c4::yml::id_type id, vec2<T> *v)
{
    using namespace c4::yml;
    if(!tree->is_seq(id))
        return false;
    c4::yml::id_type x = tree->first_child(id);
    if(x == NONE)
        return false;
    c4::yml::id_type y = tree->next_sibling(x); // of x!
    if(y == NONE)
        return false;
    return read(tree, x, &v->x) && read(tree, y, &v->y);
}
} // namespace foo8


//-----------------------------------------------------------------------------

namespace foo9_optimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_seq();
    n[0] << v.x;
    n[1] << v.y;
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    n[0] >> v->x;
    n[1] >> v->y;
    return true;
}
} // namespace foo5


namespace foo9_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_seq();
    n[0] << v.x;
    n[1] << v.y;
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    using namespace c4::yml;
    if(!n.readable() || !n.is_seq())
        return false;
    ConstNodeRef x = n[0];
    ConstNodeRef y = n[1];
    if(!x.readable() || !y.readable())
        return false;
    x >> v->x; // throws on error. to keep running, use x.deserialize_val() which returns bool
    y >> v->y; // throws on error. to keep running, use y.deserialize_val() which returns bool
    return true;
}
} // namespace foo9_pessimistic


//-----------------------------------------------------------------------------


namespace c4 {
namespace yml {

TEST(serialize, type_as_str__to_chars)
{
    c4::yml::Tree t;

    NodeRef r = t.rootref();
    r.set_map();

    foo::vec2<int> v2in{10, 11};
    foo::vec2<int> v2out{1, 2};
    r["v2"] << v2in;
    r["v2"] >> v2out;
    EXPECT_EQ(v2in.x, v2out.x);
    EXPECT_EQ(v2in.y, v2out.y);

    foo::vec3<int> v3in{100, 101, 102};
    foo::vec3<int> v3out{1, 2, 3};
    r["v3"] << v3in;
    r["v3"] >> v3out;
    EXPECT_EQ(v3in.x, v3out.x);
    EXPECT_EQ(v3in.y, v3out.y);
    EXPECT_EQ(v3in.z, v3out.z);

    foo::vec4<int> v4in{1000, 1001, 1002, 1003};
    foo::vec4<int> v4out{1, 2, 3, 4};
    r["v4"] << v4in;
    r["v4"] >> v4out;
    EXPECT_EQ(v4in.x, v4out.x);
    EXPECT_EQ(v4in.y, v4out.y);
    EXPECT_EQ(v4in.z, v4out.z);
    EXPECT_EQ(v4in.w, v4out.w);

    char buf[256];
    c4::csubstr ret = c4::yml::emit_yaml(t, buf);
    EXPECT_EQ(ret, R"(v2: '(10,11)'
v3: '(100,101,102)'
v4: '(1000,1001,1002,1003)'
)");
}

template<template<class> class Vec2T>
void test_serialization_roundtrip_vec2(std::string const& expected)
{
    c4::yml::Tree t;
    NodeRef r = t.rootref();
    r.set_map();
    {
        Vec2T<int> v2in{10, 11};
        Vec2T<int> v2out{1, 2};
        r["v2"] << v2in;
        r["v2"] >> v2out;
        EXPECT_EQ(v2in.x, v2out.x);
        EXPECT_EQ(v2in.y, v2out.y);
    }
    EXPECT_EQ(expected, emitrs_yaml<std::string>(t));
}
template<template<class> class Vec2T>
void test_serialization_roundtrip_vec2_with_key(std::string const& expected)
{
    c4::yml::Tree t;
    NodeRef r = t.rootref();
    r.set_map();
    {
        Vec2T<int> v2in{10, 11};
        Vec2T<int> v2out{1, 2};
        r["v2"] << v2in;
        r["v2"] >> v2out;
        EXPECT_EQ(v2in.x, v2out.x);
        EXPECT_EQ(v2in.y, v2out.y);
    }
    {
        Vec2T<int> v2in_key{1, 2};
        Vec2T<int> v2in_val{3, 4};
        Vec2T<int> v2out_val{10, 20};
        Vec2T<int> v2out_key{30, 40};
        NodeRef child = r.append_child();
        child << key(v2in_key) << v2in_val;
        child >> key(v2out_key) >> v2out_val;
        EXPECT_EQ(v2in_key.x, v2out_key.x);
        EXPECT_EQ(v2in_key.y, v2out_key.y);
        EXPECT_EQ(v2in_val.x, v2out_val.x);
        EXPECT_EQ(v2in_val.y, v2out_val.y);
    }
    EXPECT_EQ(expected, emitrs_yaml<std::string>(t));
}

TEST(serialize, type_as_str__foo2_serialize_scalar)
{
    test_serialization_roundtrip_vec2_with_key<foo2::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}

TEST(serialize, type_as_str__foo3_serialize_to_arena)
{
    test_serialization_roundtrip_vec2_with_key<foo3::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}

TEST(serialize, type_as_str__foo4_tree_read)
{
    test_serialization_roundtrip_vec2_with_key<foo4::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}

TEST(serialize, type_as_str__foo5_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}

TEST(serialize, type_as_str__foo5_ref_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5_ref::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}

TEST(serialize, type_as_str__foo5_pass_by_value_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5_pass_by_value::vec2>(""
        ""
        "v2: '(10,11)'\n"
        "'(1,2)': '(3,4)'\n"
        "");
}


TEST(serialize, tree__foo6_optimistic)
{
    test_serialization_roundtrip_vec2<foo6_optimistic::vec2>(""
        "v2:\n"
        "  x: 10\n"
        "  y: 11\n"
        "");
}

TEST(serialize, tree__foo6_pessimistic)
{
    test_serialization_roundtrip_vec2<foo6_pessimistic::vec2>(""
        "v2:\n"
        "  x: 10\n"
        "  y: 11\n"
        "");
}

TEST(serialize, tree__foo7_optimistic)
{
    test_serialization_roundtrip_vec2<foo7_optimistic::vec2>(""
        "v2:\n"
        "  x: 10\n"
        "  y: 11\n"
        "");
}

TEST(serialize, tree__foo7_pessimistic)
{
    test_serialization_roundtrip_vec2<foo7_pessimistic::vec2>(""
        "v2:\n"
        "  x: 10\n"
        "  y: 11\n"
        "");
}


TEST(serialize, tree__foo8_optimistic)
{
    test_serialization_roundtrip_vec2<foo8_optimistic::vec2>(""
        "v2:\n"
        "  - 10\n"
        "  - 11\n"
        "");
}

TEST(serialize, tree__foo8_pessimistic)
{
    test_serialization_roundtrip_vec2<foo8_pessimistic::vec2>(""
        "v2:\n"
        "  - 10\n"
        "  - 11\n"
        "");
}

TEST(serialize, tree__foo9_optimistic)
{
    test_serialization_roundtrip_vec2<foo9_optimistic::vec2>(""
        "v2:\n"
        "  - 10\n"
        "  - 11\n"
        "");
}

TEST(serialize, tree__foo9_pessimistic)
{
    test_serialization_roundtrip_vec2<foo9_pessimistic::vec2>(""
        "v2:\n"
        "  - 10\n"
        "  - 11\n"
        "");
}


//-------------------------------------------
template<class Container, class... Args>
void do_test_serialize(Args&& ...args)
{
    using namespace c4::yml;
    Container s(std::forward<Args>(args)...);
    Container out;

    Tree t;
    NodeRef n(&t);

    n << s;
    //print_tree(t);
    emit_yaml(t);
    c4::yml::check_invariants(t);
    n >> out;
    EXPECT_EQ(s, out);
}


TEST(serialize, std_vector_int)
{
    using T = int;
    using L = std::initializer_list<T>;
    do_test_serialize<std::vector<T>>(L{1, 2, 3, 4, 5});
}
TEST(serialize, std_vector_bool)
{
    using T = bool;
    using L = std::initializer_list<T>;
    do_test_serialize<std::vector<T>>(L{true, false, true, false, true, true});
}
TEST(serialize, std_vector_string)
{
    using T = std::string;
    using L = std::initializer_list<T>;
    do_test_serialize<std::vector<T>>(L{"0asdadk0", "1sdfkjdfgu1", "2fdfdjkhdfgkjhdfi2", "3e987dfgnfdg83", "4'd0fgºçdfg«4"});
}
TEST(serialize, std_vector_std_vector_int)
{
    using T = std::vector<int>;
    using L = std::initializer_list<T>;
    do_test_serialize<std::vector<T>>(L{{1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}});
}


TEST(serialize, std_map__int_int)
{
    using M = std::map<int, int>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize<M>(L{{10, 0}, {11, 1}, {22, 2}, {10001, 1000}, {20002, 2000}, {30003, 3000}});
}
TEST(serialize, std_map__std_string_int)
{
    using M = std::map<std::string, int>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize<M>(L{{"asdsdf", 0}, {"dfgdfgdfg", 1}, {"dfgjdfgkjh", 2}});
}
TEST(serialize, std_map__string_vectori)
{
    using M = std::map<std::string, std::vector<int>>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize<M>(L{{"asdsdf", {0, 1, 2, 3}}, {"dfgdfgdfg", {4, 5, 6, 7}}, {"dfgjdfgkjh", {8, 9, 10, 11}}});
}
TEST(serialize, std_vector__map_string_int)
{
    using V = std::vector< std::map<std::string, int>>;
    using M = typename V::value_type;
    using L = std::initializer_list<M>;
    do_test_serialize<V>(L{
            M{{"asdasf",  0}, {"dfgkjhdfg",  1}, {"fghffg",  2}, {"r5656kjnh9b'dfgwg+*",  3}},
            M{{"asdasf", 10}, {"dfgkjhdfg", 11}, {"fghffg", 12}, {"r5656kjnh9b'dfgwg+*", 13}},
            M{{"asdasf", 20}, {"dfgkjhdfg", 21}, {"fghffg", 22}, {"r5656kjnh9b'dfgwg+*", 23}},
            M{{"asdasf", 30}, {"dfgkjhdfg", 31}, {"fghffg", 32}, {"r5656kjnh9b'dfgwg+*", 33}},
    });
}


TEST(serialize, bool)
{
    Tree t = parse_in_arena("{a: 0, b: false, c: 1, d: true}");
    bool v, w;
    t["a"] >> v;
    EXPECT_EQ(v, false);
    t["b"] >> v;
    EXPECT_EQ(v, false);
    t["c"] >> v;
    EXPECT_EQ(v, true);
    t["d"] >> v;
    EXPECT_EQ(v, true);

    t["e"] << true;
    EXPECT_EQ(t["e"].val(), "1");
    t["e"] >> w;
    EXPECT_EQ(w, true);

    t["e"] << false;
    EXPECT_EQ(t["e"].val(), "0");
    t["e"] >> w;
    EXPECT_EQ(w, false);

    t["e"] << fmt::boolalpha(true);
    EXPECT_EQ(t["e"].val(), "true");
    t["e"] >> w;
    EXPECT_EQ(w, true);

    t["e"] << fmt::boolalpha(false);
    EXPECT_EQ(t["e"].val(), "false");
    t["e"] >> w;
    EXPECT_EQ(w, false);
}

TEST(serialize, integral)
{
    csubstr yaml = R"({
0: 0,
10: 10,
+20: +20,
-30: -30,
"0xaf": "0xaf",
"100": "100",
 :
})";
    test_check_emit_check(yaml, [](Tree const& t){
        int i = 1;
        i = 1; t[0] >> i; EXPECT_EQ(i, 0);
        i = 1; t[1] >> i; EXPECT_EQ(i, 10);
        i = 1; t[2] >> i; EXPECT_EQ(i, 20);
        i = 1; t[3] >> i; EXPECT_EQ(i, -30);
        i = 1; t[4] >> i; EXPECT_EQ(i, 0xaf);
        ExpectError::check_error_visit(&t, [&]{ t[5] >> i; });
        ExpectError::check_error_visit(&t, [&]{ t[6] >> i; });
        i = 1; t[0] >> key(i); EXPECT_EQ(i, 0);
        i = 1; t[1] >> key(i); EXPECT_EQ(i, 10);
        i = 1; t[2] >> key(i); EXPECT_EQ(i, 20);
        i = 1; t[3] >> key(i); EXPECT_EQ(i, -30);
        i = 1; t[4] >> key(i); EXPECT_EQ(i, 0xaf);
        ExpectError::check_error_visit(&t, [&]{ t[5] >> key(i); });
        ExpectError::check_error_visit(&t, [&]{ t[6] >> key(i); });
    });
}

TEST(serialize, error_deserializing_containers)
{
    csubstr yaml = R"(--- {}
--- []
)";
    const Tree t = parse_in_arena(yaml);
    ASSERT_TRUE(t.rootref().type().is_stream());
    ASSERT_TRUE(t.docref(0).type().is_map());
    ASSERT_TRUE(t.docref(1).type().is_seq());
    {
        int i;
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> i; }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> i; }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> i; }, t.docref(1).id());
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> key(i); }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> key(i); }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> key(i); }, t.docref(1).id());
    }
    {
        csubstr s;
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> s; }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> s; }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> s; }, t.docref(1).id());
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> key(s); }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> key(s); }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> key(s); }, t.docref(1).id());
    }
    {
        // base64
        char buf_[256];
        substr s = buf_;
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> fmt::base64(s); }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> fmt::base64(s); }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> fmt::base64(s); }, t.docref(1).id());
        ExpectError::check_error_visit(&t, [&]{ t.rootref() >> key(fmt::base64(s)); }, t.rootref().id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(0) >> key(fmt::base64(s)); }, t.docref(0).id());
        ExpectError::check_error_visit(&t, [&]{ t.docref(1) >> key(fmt::base64(s)); }, t.docref(1).id());
    }
}

TEST(serialize, deserializing_to_csubstr_and_base64)
{
    csubstr yaml = R"(str: Brevity is the soul of wit.
base64: QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu
QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu: base64
)";
    const Tree t = parse_in_arena(yaml);
    ConstNodeRef r = t.rootref();
    ASSERT_TRUE(r.type().is_map());
    csubstr s;
    ASSERT_EQ(r[0].key(), "str");
    ASSERT_EQ(r[0].val(), "Brevity is the soul of wit.");
    ASSERT_EQ(r[1].key(), "base64");
    ASSERT_EQ(r[1].val(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].key(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].val(), "base64");
    r[0] >> s; EXPECT_EQ(s, "Brevity is the soul of wit."); EXPECT_EQ(s, r[0].val());
    r[1] >> s; EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu"); EXPECT_EQ(s, r[1].val());
    r[2] >> s; EXPECT_EQ(s, "base64"); EXPECT_EQ(s, r[2].val());
    r[0] >> key(s); EXPECT_EQ(s, "str"); EXPECT_EQ(s, r[0].key());
    r[1] >> key(s); EXPECT_EQ(s, "base64"); EXPECT_EQ(s, r[1].key());
    r[2] >> key(s); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu"); EXPECT_EQ(s, r[2].key());
    {
        std::string buf;
        size_t szk = 0;
        EXPECT_TRUE(r[1].deserialize_val(fmt::base64(buf, &szk)));
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        to_substr(buf).fill('a'); EXPECT_NE(buf, "Brevity is the soul of wit.");
        //
        EXPECT_TRUE(r[2].deserialize_key(fmt::base64(buf, &szk)));
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        to_substr(buf).fill('a'); EXPECT_NE(buf, "Brevity is the soul of wit.");
        //
        r[1] >> fmt::base64(buf, &szk);
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        to_substr(buf).fill('a'); EXPECT_NE(buf, "Brevity is the soul of wit.");
        //
        r[2] >> key(fmt::base64(buf, &szk));
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        to_substr(buf).fill('a'); EXPECT_NE(buf, "Brevity is the soul of wit.");
        //
        r[1] >> fmt::base64(to_substr(buf), &szk);
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        //
        r[2] >> key(fmt::base64(to_substr(buf), &szk));
        EXPECT_EQ(buf.size(), szk); EXPECT_EQ(buf, "Brevity is the soul of wit.");
        //
        {
            SCOPED_TRACE("r[1] val");
            EXPECT_FALSE(base64_valid(r[1].key().str, r[1].key().len));
            EXPECT_FALSE(r[1].deserialize_key(fmt::base64(buf)));
            EXPECT_FALSE(r[1].deserialize_key(fmt::base64(buf, &szk)));
            EXPECT_FALSE(r[1].deserialize_key(fmt::base64(to_substr(buf))));
            EXPECT_FALSE(r[1].deserialize_key(fmt::base64(to_substr(buf), &szk)));
            ExpectError::check_error_visit(&t, [&]{ r[1] >> key(fmt::base64(buf)); }, r[1].id());
            ExpectError::check_error_visit(&t, [&]{ r[1] >> key(fmt::base64(buf, &szk)); }, r[1].id());
            ExpectError::check_error_visit(&t, [&]{ r[1] >> key(fmt::base64(to_substr(buf))); }, r[1].id());
            ExpectError::check_error_visit(&t, [&]{ r[1] >> key(fmt::base64(to_substr(buf), &szk)); }, r[1].id());
        }
        //
        {
            SCOPED_TRACE("r[2] key");
            EXPECT_FALSE(base64_valid(r[2].val().str, r[2].val().len));
            EXPECT_FALSE(r[2].deserialize_val(fmt::base64(buf)));
            EXPECT_FALSE(r[2].deserialize_val(fmt::base64(buf, &szk)));
            EXPECT_FALSE(r[2].deserialize_val(fmt::base64(to_substr(buf))));
            EXPECT_FALSE(r[2].deserialize_val(fmt::base64(to_substr(buf), &szk)));
            ExpectError::check_error_visit(&t, [&]{ r[2] >> fmt::base64(buf); }, r[2].id());
            ExpectError::check_error_visit(&t, [&]{ r[2] >> fmt::base64(buf, &szk); }, r[2].id());
            ExpectError::check_error_visit(&t, [&]{ r[2] >> fmt::base64(to_substr(buf)); }, r[2].id());
            ExpectError::check_error_visit(&t, [&]{ r[2] >> fmt::base64(to_substr(buf), &szk); }, r[2].id());
        }
    }
}

// inf and nan are tested in test_number.cpp

TEST(serialize, std_string)
{
    auto t = parse_in_arena("{foo: bar}");
    std::string s;
    EXPECT_NE(s, "bar");
    t["foo"] >> s;
    EXPECT_EQ(s, "bar");
}

TEST(serialize, anchor_and_ref_round_trip)
{
    const char yaml[] = R"(anchor_objects:
  - &id001
    name: id001
  - &id002
    name: id002
  - name: id003
  - &id004
    name: id004
references:
  reference_key: *id001
  reference_list:
    - *id002
    - *id004
)";

    Tree t = parse_in_arena(yaml);
    std::string cmpbuf;
    emitrs_yaml(t, &cmpbuf);
    EXPECT_EQ(cmpbuf, yaml);
}

TEST(serialize, create_anchor_ref_trip)
{
    const char expected_yaml[] = R"(anchors:
  - &id0 val0
  - &id1
    key1: val1
  - &id2
    - val2
seq:
  - *id0
  - *id1
  - *id2
seqflow: [*id0,*id1,*id2]
map:
  *id0 : *id1
  next: *id2
mapflow: {*id0 : *id1,next: *id2}
)";

    Tree tree;
    tree.rootref().set_map();
    tree["anchors"].set_seq();
    tree["anchors"][0] = "val0";
    tree["anchors"][0].set_val_anchor("id0");
    tree["anchors"][1].set_map();
    tree["anchors"][1].set_val_anchor("id1");
    tree["anchors"][1]["key1"] = "val1";
    tree["anchors"][2].set_seq();
    tree["anchors"][2].set_val_anchor("id2");
    tree["anchors"][2][0] = "val2";
    auto setseq = [](NodeRef n, NodeType style){
        n.set_seq(style);
        n[0] = "id0";
        n[0].set_val_ref("id0");
        n[1] = "id1";
        n[1].set_val_ref("id1");
        n[2] = "id2";
        n[2].set_val_ref("id2");
    };
    auto setmap = [](NodeRef n, NodeType style){
        n.set_map(style);
        n["*id0"] = "id0";
        n["*id0"].set_key_ref("id0");
        n["*id0"].set_val_ref("id1");
        n["next"] = "id2";
        n["next"].set_val_ref("id2");
    };
    setseq(tree["seq"], BLOCK);
    setseq(tree["seqflow"], FLOW_SL);
    setmap(tree["map"], BLOCK);
    setmap(tree["mapflow"], FLOW_SL);

    EXPECT_EQ(emitrs_yaml<std::string>(tree), expected_yaml);
}

TEST(deserialize, issue434_0)
{
    Tree tree = parse_in_arena("{empty: }");
    ConstNodeRef cnode = tree["empty"];
    NodeRef node = tree["empty"];
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        bool ret = read(cnode, &value); (void)ret;
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        bool ret = read(node, &value); (void)ret;
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        cnode >> value;
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        node >> value;
    });
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        bool ret = read(cnode, &value); (void)ret;
    });
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        bool ret = read(node, &value); (void)ret;
    });
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        cnode >> value;
    });
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        node >> value;
    });
}

void test_deserialize_trailing_434(csubstr yaml, csubstr val, csubstr first, double dval)
{
    Tree tree = parse_in_arena(yaml);
    ASSERT_EQ(tree["val"].val(), val);
    EXPECT_EQ(tree["val"].val().first_int_span(), first);
    EXPECT_EQ(tree["val"].val().first_real_span(), first);
    ConstNodeRef cnode = tree["val"];
    NodeRef node = tree["val"];
    {
        int value = {};
        EXPECT_FALSE(read(cnode, &value));
    }
    {
        int value = {};
        EXPECT_FALSE(read(node, &value));
    }
    ExpectError::check_error_visit(&tree, [&]{
        int value = 1;
        cnode >> value;
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 1;
        node >> value;
    });
    float fval = (float)dval;
    {
        float value = {};
        EXPECT_TRUE(read(cnode, &value));
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        float value = {};
        EXPECT_TRUE(read(node, &value));
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        double value = {};
        EXPECT_TRUE(read(cnode, &value));
        EXPECT_EQ(memcmp(&value, &dval, sizeof(dval)), 0);
    }
    {
        double value = {};
        EXPECT_TRUE(read(node, &value));
        EXPECT_EQ(memcmp(&value, &dval, sizeof(dval)), 0);
    }
    {
        float value = 1;
        cnode >> value;
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        float value = 1;
        node >> value;
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        double value = 1;
        cnode >> value;
        EXPECT_EQ(memcmp(&value, &dval, sizeof(fval)), 0);
    }
    {
        double value = 1;
        node >> value;
        EXPECT_EQ(memcmp(&value, &dval, sizeof(fval)), 0);
    }
}
TEST(deserialize, issue434_1)
{
    test_deserialize_trailing_434("{val: 0.r3}", "0.r3", "", 0.0);
}

TEST(deserialize, issue434_2)
{
    test_deserialize_trailing_434("{val: 34gf3}", "34gf3", "", 34.0);
}

TEST(deserialize, issue434_3)
{
    test_deserialize_trailing_434("{val: 34 gf3}", "34 gf3", "34", 34.0);
}


template<class T>
void test442(csubstr input, csubstr expected, NodeType style_flag)
{
    // as a seq member
    {
        const std::string input_str = formatrs<std::string>("- {}", input);
        const std::string expected_input_str = formatrs<std::string>("- {}\n", input);
        const std::string expected_str = formatrs<std::string>("- {}\n", expected);
        const Tree tree = parse_in_arena(to_csubstr(input_str));
        EXPECT_TRUE(tree[0].type_has_all(style_flag));
        EXPECT_EQ(expected_input_str, emitrs_yaml<std::string>(tree));
        T obj = {};  // T is a scalar type like int, char, double, etc.
        tree[0] >> obj;
        Tree out_tree;
        out_tree.rootref().set_seq();
        out_tree[0] << obj;
        out_tree[0].set_val_style(style_flag);
        EXPECT_EQ(expected_str, emitrs_yaml<std::string>(out_tree));
    }
    // as a map member
    {
        const std::string input_str = formatrs<std::string>("val: {}", input);
        const std::string expected_input_str = formatrs<std::string>("val: {}\n", input);
        const std::string expected_str = formatrs<std::string>("val: {}\n", expected);
        const Tree tree = parse_in_arena(to_csubstr(input_str));
        EXPECT_TRUE(tree["val"].type_has_all(style_flag));
        EXPECT_EQ(expected_input_str, emitrs_yaml<std::string>(tree));
        T obj = {};  // T is a scalar type like int, char, double, etc.
        tree["val"] >> obj;
        Tree out_tree;
        out_tree.rootref().set_map();
        out_tree["val"] << obj;
        out_tree["val"].set_val_style(style_flag);
        EXPECT_EQ(expected_str, emitrs_yaml<std::string>(out_tree));
    }
    // as a doc scalar
    {
        const std::string expected_input_str = formatrs<std::string>("{}\n", input);
        const std::string expected_str = formatrs<std::string>("{}\n", expected);
        const Tree tree = parse_in_arena(input);
        EXPECT_TRUE(tree.rootref().type_has_all(style_flag));
        EXPECT_EQ(expected_input_str, emitrs_yaml<std::string>(tree));
        T obj = {};  // T is a scalar type like int, char, double, etc.
        tree.rootref() >> obj;
        Tree out_tree;
        out_tree.rootref() << obj;
        out_tree.rootref().set_val_style(style_flag);
        EXPECT_EQ(expected_str, emitrs_yaml<std::string>(out_tree));
    }
}
TEST(serialize, issue442_00)
{
    test442<int>("123", "123", VAL_PLAIN);
}
TEST(serialize, issue442_01)
{
    test442<int>("-123", "-123", VAL_PLAIN);
}
TEST(serialize, issue442_02)
{
    test442<int>("+123", "123", VAL_PLAIN);
}
TEST(serialize, issue442_10)
{
    test442<float>("2.35e-10", "2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_11)
{
    test442<float>("-2.35e-10", "-2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_12)
{
    test442<float>("+2.35e-10", "2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_20)
{
    test442<double>("2.35e-10", "2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_21)
{
    test442<double>("-2.35e-10", "-2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_22)
{
    test442<double>("+2.35e-10", "2.35e-10", VAL_PLAIN);
}
TEST(serialize, issue442_30)
{
    test442<char>("'a'", "'a'", VAL_SQUO);
}
TEST(serialize, issue442_31)
{
    test442<char>("' '", "' '", VAL_SQUO);
}
TEST(serialize, issue442_40)
{
    test442<char>("\"a\"", "\"a\"", VAL_DQUO);
}
TEST(serialize, issue442_41)
{
    test442<char>("\" \"", "\" \"", VAL_DQUO);
}
TEST(serialize, issue442_50)
{
    test442<char>("a", "a", VAL_PLAIN);
}
TEST(serialize, issue442_60)
{
    EXPECT_TRUE(scalar_style_query_plain("123"));
    EXPECT_TRUE(scalar_style_query_plain("-123"));
    EXPECT_TRUE(scalar_style_query_plain("+123"));
    EXPECT_EQ(scalar_style_choose("123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+123"), SCALAR_PLAIN);
    {
        Tree tree;
        tree.rootref() << "123";
        EXPECT_EQ(std::string("123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << "-123";
        EXPECT_EQ(std::string("-123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << 123;
        EXPECT_EQ(std::string("123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << -123;
        EXPECT_EQ(std::string("-123\n"), emitrs_yaml<std::string>(tree));
    }
}
TEST(serialize, issue442_61)
{
    EXPECT_TRUE(scalar_style_query_plain("2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain("-2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain("+2.35e-10"));
    EXPECT_EQ(scalar_style_choose("2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+2.35e-10"), SCALAR_PLAIN);
    {
        Tree tree;
        tree.rootref() << 2.35e-10;
        EXPECT_EQ(std::string("2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << -2.35e-10;
        EXPECT_EQ(std::string("-2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << "2.35e-10";
        EXPECT_EQ(std::string("2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref() << "-2.35e-10";
        EXPECT_EQ(std::string("-2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
}


TEST(serialize, issue477_vec)
{
    const Tree t = parse_in_arena(R"([0, 10, 20, 30, 40])");
    std::vector<int> vec = {100, 1, 2, 3};
    ASSERT_EQ(vec.size(), 4);
    EXPECT_EQ(vec[0], 100);
    EXPECT_EQ(vec[1], 1);
    EXPECT_EQ(vec[2], 2);
    EXPECT_EQ(vec[3], 3);
    t.rootref() >> vec;
    ASSERT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0], 0);
    EXPECT_EQ(vec[1], 10);
    EXPECT_EQ(vec[2], 20);
    EXPECT_EQ(vec[3], 30);
    EXPECT_EQ(vec[4], 40);
}

TEST(serialize, issue477_map)
{
    const Tree t = parse_in_arena(R"({0: 10,  2: 30,  4: 50})");
    std::map<int,int> map = {{0, 1}, {2, 3}};
    ASSERT_EQ(map.size(), 2);
    EXPECT_EQ(map[0], 1);
    EXPECT_EQ(map[2], 3);
    t.rootref() >> map;
    ASSERT_EQ(map.size(), 3); // added a new member
    EXPECT_EQ(map[0], 10); // modified
    EXPECT_EQ(map[2], 30); // modified
    EXPECT_EQ(map[4], 50);
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
