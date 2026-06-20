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

namespace foo_string {
struct MyStringType
{
    std::string s;
    template<class ...Args> MyStringType(Args&&...args) : s(std::forward<Args>(args)...) {}
};
csubstr to_csubstr(MyStringType const& s) { return c4::to_csubstr(s.s); }
substr to_substr(MyStringType & s) { return c4::to_substr(s.s); }
} // namespace foo_string
namespace c4 {
template<> struct is_string<foo_string::MyStringType> : std::true_type {};
} // namespace c4


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
template<class T> size_t scalar_serialize(substr buf, vec2<T> v) { return c4::format(buf, "({},{})", v.x, v.y); }
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
    n["x"].save(v.x);
    n["y"].save(v.y);
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    n["x"].load(&v->x);
    n["y"].load(&v->y);
    return true;
}
} // namespace foo5


namespace foo7_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_map();
    n["x"].save(v.x);
    n["y"].save(v.y);
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
    x.load(&v->x); // throws on error. to keep running, use x.deserialize() which returns bool
    y.load(&v->y); // throws on error. to keep running, use y.deserialize() which returns bool
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
    n[0].save(v.x);
    n[1].save(v.y);
}
template<class T> bool read(c4::yml::ConstNodeRef const& n, vec2<T> *v)
{
    n[0].load(&v->x);
    n[1].load(&v->y);
    return true;
}
} // namespace foo5


namespace foo9_pessimistic {
template<class T> struct vec2 { T x, y; };
template<class T> void write(c4::yml::NodeRef &n, vec2<T> v)
{
    n.set_seq();
    n[0].save(v.x);
    n[1].save(v.y);
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
    x.load(&v->x); // throws on error. to keep running, use x.deserialize() which returns bool
    y.load(&v->y); // throws on error. to keep running, use y.deserialize() which returns bool
    return true;
}
} // namespace foo9_pessimistic


//-----------------------------------------------------------------------------


namespace c4 {
namespace yml {

TEST(serialize, type_as_str__to_chars)
{
    c4::yml::Tree t;

    c4::yml::NodeRef r = t;
    r.set_map();

    foo::vec2<int> v2in{10, 11};
    foo::vec2<int> v2out{1, 2};
    r["v2"].save(v2in);
    r["v2"].load(&v2out);
    EXPECT_EQ(v2in.x, v2out.x);
    EXPECT_EQ(v2in.y, v2out.y);

    foo::vec3<int> v3in{100, 101, 102};
    foo::vec3<int> v3out{1, 2, 3};
    r["v3"].save(v3in);
    r["v3"].load(&v3out);
    EXPECT_EQ(v3in.x, v3out.x);
    EXPECT_EQ(v3in.y, v3out.y);
    EXPECT_EQ(v3in.z, v3out.z);

    foo::vec4<int> v4in{1000, 1001, 1002, 1003};
    foo::vec4<int> v4out{1, 2, 3, 4};
    r["v4"].save(v4in);
    r["v4"].load(&v4out);
    EXPECT_EQ(v4in.x, v4out.x);
    EXPECT_EQ(v4in.y, v4out.y);
    EXPECT_EQ(v4in.z, v4out.z);
    EXPECT_EQ(v4in.w, v4out.w);

    char buf[256];
    c4::csubstr ret = c4::yml::emit_yaml(t, buf);
    EXPECT_EQ(ret, R"(v2: (10,11)
v3: (100,101,102)
v4: (1000,1001,1002,1003)
)");
}


template<class StringType>
void test_serialization_roundtrip_string_type()
{
    c4::yml::Tree t;
    c4::yml::NodeRef r = t.rootref();
    r.set_map(FLOW_SL);
    {
        StringType k("key");
        StringType v("val");
        r[0].save_key(k);
        r[0].save(v);
    }
    EXPECT_EQ("{key: val}", emitrs_yaml<std::string>(t));
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
        r["v2"].save(v2in);
        r["v2"].load(&v2out);
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
        r["v2"].save(v2in);
        r["v2"].load(&v2out);
        EXPECT_EQ(v2in.x, v2out.x);
        EXPECT_EQ(v2in.y, v2out.y);
    }
    {
        Vec2T<int> v2in_key{1, 2};
        Vec2T<int> v2in_val{3, 4};
        Vec2T<int> v2out_val{10, 20};
        Vec2T<int> v2out_key{30, 40};
        NodeRef child = r.append_child();
        child.save_key(v2in_key);
        child.save(v2in_val);
        child.load_key(&v2out_key);
        child.load(&v2out_val);
        EXPECT_EQ(v2in_key.x, v2out_key.x);
        EXPECT_EQ(v2in_key.y, v2out_key.y);
        EXPECT_EQ(v2in_val.x, v2out_val.x);
        EXPECT_EQ(v2in_val.y, v2out_val.y);
    }
    EXPECT_EQ(expected, emitrs_yaml<std::string>(t));
}

TEST(serialize, type_as_str__foo_string)
{
    test_serialization_roundtrip_string_type<foo_string::MyStringType>();
}

TEST(serialize, type_as_str__foo2_serialize_scalar)
{
    test_serialization_roundtrip_vec2_with_key<foo2::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
        "");
}

TEST(serialize, type_as_str__foo3_serialize_to_arena)
{
    test_serialization_roundtrip_vec2_with_key<foo3::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
        "");
}

TEST(serialize, type_as_str__foo4_tree_read)
{
    test_serialization_roundtrip_vec2_with_key<foo4::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
        "");
}

TEST(serialize, type_as_str__foo5_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
        "");
}

TEST(serialize, type_as_str__foo5_ref_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5_ref::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
        "");
}

TEST(serialize, type_as_str__foo5_pass_by_value_node_read)
{
    test_serialization_roundtrip_vec2_with_key<foo5_pass_by_value::vec2>(""
        ""
        "v2: (10,11)\n"
        "(1,2): (3,4)\n"
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

TEST(serialize, auto_plain)
{
    Tree tree = parse_in_arena("{}");
    NodeRef n = tree[0];
    #define test_serialized_as_plain(T, val)                            \
        {                                                               \
            SCOPED_TRACE(#T " : " #val);                                \
            T scalar = val;                                             \
            tree._rem_flags(n.id(), VAL_PLAIN); n.save(scalar); EXPECT_TRUE(n.is_val_plain()); \
            tree._rem_flags(n.id(), KEY_PLAIN); n.save_key(scalar); EXPECT_TRUE(n.is_key_plain()); \
        }
    #define test_serialized_as_not_plain(T, val)                        \
        {                                                               \
            static_assert(!std::is_arithmetic<T>::value, "arithmetic?"); \
            SCOPED_TRACE(#T " : " #val);                                \
            T scalar = val;                                             \
            tree._rem_flags(n.id(), VAL_PLAIN); n.save(scalar); EXPECT_FALSE(n.is_val_plain()); \
            tree._rem_flags(n.id(), KEY_PLAIN); n.save_key(scalar); EXPECT_FALSE(n.is_key_plain()); \
        }
    test_serialized_as_plain(int, 0);
    test_serialized_as_plain(unsigned, 0);
    test_serialized_as_plain(int8_t, 0);
    test_serialized_as_plain(int16_t, 0);
    test_serialized_as_plain(int32_t, 0);
    test_serialized_as_plain(int64_t, 0);
    test_serialized_as_plain(uint8_t, 0);
    test_serialized_as_plain(uint16_t, 0);
    test_serialized_as_plain(uint32_t, 0);
    test_serialized_as_plain(uint64_t, 0);
    test_serialized_as_plain(float, 0);
    test_serialized_as_plain(double, 0);
    test_serialized_as_not_plain(std::string, ""); // NOLINT
    test_serialized_as_not_plain(const char*, "");
    test_serialized_as_not_plain(csubstr, "");
    char buf[10];
    test_serialized_as_not_plain(substr, buf);
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
