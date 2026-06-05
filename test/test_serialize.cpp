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


//-------------------------------------------

namespace {
template<class Container, class testfn>
void test_roundtrip_container(Container const& s, testfn fn)
{
    Tree t;
    {
        // place the container in a different scope to ensure
        // valgrind will show which variable is referring to
        // uninitialized value in case of error
        Container out = {};
        fn(t.rootref(), &out);
        C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wmaybe-uninitialized") // gcc complains about the eq below when there's no exceptions
        EXPECT_EQ(s, out);
        C4_SUPPRESS_WARNING_GCC_POP
    }
    test_invariants(t);
    if(testing::Test::HasFailure())
    {
        print_tree(t);
        emit_yaml(t);
    }
}
template<class Scalar, class testfn_key, class testfn_val>
void test_roundtrip_scalar(Scalar const& s,
                           testfn_key fnk,
                           testfn_val fnv)
{
    {
        SCOPED_TRACE("key");
        Tree t;
        t.rootref().set_map();
        t[0].create();
        {
            // place the container in a different scope to ensure
            // valgrind will show which variable is referring to
            // uninitialized value in case of error
            Scalar out;
            fnk(t[0], &out);
            t[0].set_val("val");
            EXPECT_EQ(s, out);
        }
        test_invariants(t);
        if(testing::Test::HasFailure())
        {
            print_tree(t);
            emit_yaml(t);
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("val");
        Tree t;
        t.rootref().set_map();
        NodeRef n = t["key"];
        n.create();
        {
            // place the container in a different scope to ensure
            // valgrind will show which variable is referring to
            // uninitialized value in case of error
            Scalar out;
            fnv(n, &out);
            EXPECT_EQ(s, out);
        }
        test_invariants(t);
        if(testing::Test::HasFailure())
        {
            print_tree(t);
            emit_yaml(t);
        }
    }
}

template<class Container>
void do_test_serialize_container(Container const& s)
{
    {
        SCOPED_TRACE("Node <</>>");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            n.save(s);
            n.load(out);
        });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node <</>>, constread");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            n.save(s);
            ConstNodeRef cn = n;
            cn.load(&*out);
        });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node write/read");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            write(n, s);
            EXPECT_TRUE(read(n, out));
        });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node write/ const read");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            write(n, s);
            ConstNodeRef cn = n;
            EXPECT_TRUE(read(cn, out));
        });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Tree write/read");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            n.tree()->set_serialized(n.id(), s);
            EXPECT_TRUE(n.tree()->deserialize(n.id(), out));
        });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Tree write/read");
        test_roundtrip_container(s, [&](NodeRef n, Container *out){
            write(n.tree(), n.id(), s);
            EXPECT_TRUE(read(n.tree(), n.id(), out));
        });
    }
}

template<class Scalar>
void do_test_serialize_scalar(Scalar const& s)
{
    {
        SCOPED_TRACE("here");
        do_test_serialize_container<Scalar>(s);
    }
    {
        SCOPED_TRACE("Node save/load");
        test_roundtrip_scalar(s,
                              [&](NodeRef n, Scalar *out){
                                  n.save_key(s);
                                  n.load_key(out);
                              },
                              [&](NodeRef n, Scalar *out){
                                  n.save(s);
                                  n.load(out);
                              });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode save/load");
        test_roundtrip_scalar(s,
                              [&](NodeRef n, Scalar *out){
                                  n.save_key(s);
                                  ConstNodeRef cn = n;
                                  cn.load_key(out);
                              },
                              [&](NodeRef n, Scalar *out){
                                  n.save(s);
                                  ConstNodeRef cn = n;
                                  cn.load(out);
                              }
            );
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node serialize/deserialize");
        test_roundtrip_scalar(s,
                              [&](NodeRef n, Scalar *out){
                                  n.set_key_serialized(s);
                                  EXPECT_TRUE(n.deserialize_key(out));
                              },
                              [&](NodeRef n, Scalar *out){
                                  n.set_serialized(s);
                                  EXPECT_TRUE(n.deserialize(out));
                              });
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode serialize/deserialize");
        test_roundtrip_scalar(s,
                              [&](NodeRef n, Scalar *out){
                                  n.set_key_serialized(s);
                                  ConstNodeRef cn = n;
                                  EXPECT_TRUE(cn.deserialize_key(out));
                              },
                              [&](NodeRef n, Scalar *out){
                                  n.set_serialized(s);
                                  ConstNodeRef cn = n;
                                  EXPECT_TRUE(cn.deserialize(out));
                              }
            );
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Tree write/read");
        test_roundtrip_scalar(s,
                              [&](NodeRef n, Scalar *out){
                                  n.tree()->set_key_serialized(n.id(), s);
                                  EXPECT_TRUE(n.tree()->deserialize_key(n.id(), out));
                              },
                              [&](NodeRef n, Scalar *out){
                                  n.tree()->set_serialized(n.id(), s);
                                  EXPECT_TRUE(n.tree()->deserialize(n.id(), out));
                              });
    }
}


template<class Container>
void do_test_deserialize_err_container(NodeRef n)
{
    Container c;
    {
        SCOPED_TRACE("Node >>");
        RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
            n.load(&c);
        }));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode >>");
        ConstNodeRef cn = n;
        RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
            cn.load(&c);
        }));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node read");
        EXPECT_FALSE(n.deserialize(&c));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode read");
        ConstNodeRef cn = n;
        EXPECT_FALSE(cn.deserialize(&c));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstTree read");
        ConstNodeRef cn = n;
        EXPECT_FALSE(cn.tree()->deserialize(cn.id(), &c));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Tree read");
        EXPECT_FALSE(n.tree()->deserialize(n.id(), &c));
    }
}


template<class Scalar>
void do_test_deserialize_err_scalar(NodeRef n)
{
    {
        SCOPED_TRACE("container");
        do_test_deserialize_err_container<Scalar>(n);
    }
    {
        SCOPED_TRACE("Node >>");
        Scalar c;
        {
            SCOPED_TRACE("key");
            RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
                n.load_key(&c);
            }, n.id()));
        }
        {
            SCOPED_TRACE("val");
            RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
                n.load(&c);
            }, n.id()));
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode >>");
        Scalar c;
        ConstNodeRef cn = n;
        {
            SCOPED_TRACE("key");
            RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
                cn.load_key(&c);
            }, n.id()));
        }
        {
            SCOPED_TRACE("val");
            RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
                cn.load(&c);
            }, n.id()));
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode deserialize");
        Scalar c;
        ConstNodeRef cn = n;
        EXPECT_FALSE(cn.deserialize_key(&c));
        EXPECT_FALSE(cn.deserialize(&c));
    }
    {
        SCOPED_TRACE("Node deserialize");
        Scalar c;
        EXPECT_FALSE(n.deserialize_key(&c));
        EXPECT_FALSE(n.deserialize(&c));
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Node read");
        {
            Scalar c;
            EXPECT_FALSE(read_key(n, &c));
        }
        {
            Scalar c;
            EXPECT_FALSE(read(n, &c));
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("ConstNode read");
        ConstNodeRef cn = n;
        {
            Scalar c;
            EXPECT_FALSE(read_key(cn, &c));
        }
        {
            Scalar c;
            EXPECT_FALSE(read(cn, &c));
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Const Tree read");
        ConstNodeRef cn = n;
        {
            Scalar c;
            EXPECT_FALSE(n.tree()->deserialize_key(cn.id(), &c));
        }
        {
            Scalar c;
            EXPECT_FALSE(n.tree()->deserialize(cn.id(), &c));
        }
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("Const Tree read");
        ConstNodeRef cn = n;
        {
            Scalar c;
            EXPECT_FALSE(read_key(cn.tree(), cn.id(), &c));
        }
        {
            Scalar c;
            EXPECT_FALSE(read(cn.tree(), cn.id(), &c));
        }
    }
}
template<class T>
void do_test_deserialize_err_scalar(csubstr scalar)
{
    RYML_TRACE_FMT("scalar={}", scalar);
    std::string yaml = formatrs<std::string>("{}: {}", scalar, scalar);
    Tree t = parse_in_place(to_substr(yaml));
    do_test_deserialize_err_scalar<T>(t[0]);
}
} // namespace


TEST(serialize, int_err)
{
    do_test_deserialize_err_scalar<int>("not an int");
    do_test_deserialize_err_scalar<int>("1not an int");
    do_test_deserialize_err_scalar<int>("-1not an int");
    do_test_deserialize_err_scalar<int>("-1 not an int");
}
TEST(serialize, int)
{
    do_test_serialize_scalar<int>(0);
    do_test_serialize_scalar<int>(-1);
}

TEST(serialize, bool_err)
{
    do_test_deserialize_err_scalar<bool>("not a bool");
    do_test_deserialize_err_scalar<bool>("untrue");
    do_test_deserialize_err_scalar<bool>("unfalse");
    do_test_deserialize_err_scalar<bool>("truee");
    do_test_deserialize_err_scalar<bool>("falsee");
}
TEST(serialize, bool)
{
    do_test_serialize_scalar<bool>(true);
    do_test_serialize_scalar<bool>(false);
}

TEST(serialize, float_err)
{
    do_test_deserialize_err_scalar<float>(".notinf");
    do_test_deserialize_err_scalar<float>(".nannn");
    do_test_deserialize_err_scalar<float>(".Nannn");
    do_test_deserialize_err_scalar<float>(".NaNNN");
    do_test_deserialize_err_scalar<float>(".infff");
    do_test_deserialize_err_scalar<float>(".Infff");
    do_test_deserialize_err_scalar<float>(".INFF");
    do_test_deserialize_err_scalar<float>("-.infff");
    do_test_deserialize_err_scalar<float>("-.Infff");
    do_test_deserialize_err_scalar<float>("-.INFF");
    do_test_deserialize_err_scalar<float>("+.infff");
    do_test_deserialize_err_scalar<float>("+.Infff");
    do_test_deserialize_err_scalar<float>("+.INFF");
    //
    do_test_deserialize_err_scalar<double>(".notinf");
    do_test_deserialize_err_scalar<double>(".nannn");
    do_test_deserialize_err_scalar<double>(".Nannn");
    do_test_deserialize_err_scalar<double>(".NaNNN");
    do_test_deserialize_err_scalar<double>(".infff");
    do_test_deserialize_err_scalar<double>(".Infff");
    do_test_deserialize_err_scalar<double>(".INFF");
    do_test_deserialize_err_scalar<double>("-.infff");
    do_test_deserialize_err_scalar<double>("-.Infff");
    do_test_deserialize_err_scalar<double>("-.INFF");
    do_test_deserialize_err_scalar<double>("+.infff");
    do_test_deserialize_err_scalar<double>("+.Infff");
    do_test_deserialize_err_scalar<double>("+.INFF");
}

TEST(serialize, std_string0)
{
    do_test_serialize_scalar<std::string>("asdasdasd");
    do_test_serialize_scalar<std::string>("");
    do_test_serialize_scalar<std::string>({});
}
TEST(serialize, std_vector_int)
{
    do_test_serialize_container<std::vector<int>>({1,2,3,4,5});
}
TEST(serialize, std_vector_int_err)
{
    Tree t = parse_in_arena("[0, 1, 2, not an int, 3]");
    do_test_deserialize_err_container<std::vector<int>>(t.rootref());
}
TEST(serialize, std_vector_bool)
{
    do_test_serialize_container<std::vector<bool>>({
            true, false, true, false, true, true});
}
TEST(serialize, std_vector_bool_err)
{
    Tree t = parse_in_arena("[true, false, True, False, haha, true]");
    do_test_deserialize_err_container<std::vector<bool>>(t.rootref());
}
TEST(serialize, std_vector_string)
{
    do_test_serialize_container<std::vector<std::string>>({
            "0asdadk0", "1sdfkjdfgu1", "2fdfdjkhdfgkjhdfi2", "3e987dfgnfdg83", "4'd0fgºçdfg«4"});
}
TEST(serialize, std_vector_std_vector_int)
{
    do_test_serialize_container<std::vector<std::vector<int>>>({
            {1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}});
}


TEST(serialize, std_map__int_int)
{
    do_test_serialize_container<std::map<int, int>>({
            {10, 0}, {11, 1}, {22, 2}, {10001, 1000}, {20002, 2000}, {30003, 3000}});
}
TEST(serialize, std_map__int_int__err)
{
    {
        Tree t = parse_in_arena(R"({1: 10, 2: 20, not an int: 30})");
        do_test_deserialize_err_container<std::map<int, int>>(t.rootref());
    }
    {
        Tree t = parse_in_arena(R"({1: 10, 2: 20, 3: not an int})");
        do_test_deserialize_err_container<std::map<int, int>>(t.rootref());
    }
}

TEST(serialize, std_map__std_string_int)
{
    do_test_serialize_container<std::map<std::string, int>>({
            {"asdsdf", 0},
            {"dfgdfgdfg", 1},
            {"dfgjdfgkjh", 2}});
}
TEST(serialize, std_map__std_string_std_string)
{
    do_test_serialize_container<std::map<std::string, std::string>>({
            {"asdsdf", "bnoibjnlknmdfb"},
            {"dfgdfgdfg", "dvlkjdlkjd"},
            {"dfgjdfgkjh", "fglkjfghlk"}});
}
TEST(serialize, std_map__string_vectori)
{
    do_test_serialize_container<std::map<std::string, std::vector<int>>>({
            {"asdsdf", {0, 1, 2, 3}},
            {"dfgdfgdfg", {4, 5, 6, 7}},
            {"dfgjdfgkjh", {8, 9, 10, 11}}});
}
TEST(serialize, std_vector__map_string_int)
{
    using M = std::map<std::string, int>;
    do_test_serialize_container<std::vector<M>>({
            M{{"asdasf",  0}, {"dfgkjhdfg",  1}, {"fghffg",  2}, {"r5656kjnh9b'dfgwg+*",  3}},
            M{{"asdasf", 10}, {"dfgkjhdfg", 11}, {"fghffg", 12}, {"r5656kjnh9b'dfgwg+*", 13}},
            M{{"asdasf", 20}, {"dfgkjhdfg", 21}, {"fghffg", 22}, {"r5656kjnh9b'dfgwg+*", 23}},
            M{{"asdasf", 30}, {"dfgkjhdfg", 31}, {"fghffg", 32}, {"r5656kjnh9b'dfgwg+*", 33}},
    });
}


TEST(serialize, bool2)
{
    Tree t = parse_in_arena("{a: 0, b: false, c: 1, d: true}");
    bool v, w;
    t["a"].load(&v);
    EXPECT_EQ(v, false);
    t["b"].load(&v);
    EXPECT_EQ(v, false);
    t["c"].load(&v);
    EXPECT_EQ(v, true);
    t["d"].load(&v);
    EXPECT_EQ(v, true);

    t["e"].save(true);
    EXPECT_EQ(t["e"].val(), "1");
    t["e"].load(&w);
    EXPECT_EQ(w, true);

    t["e"].save(false);
    EXPECT_EQ(t["e"].val(), "0");
    t["e"].load(&w);
    EXPECT_EQ(w, false);

    t["e"].save(fmt::boolalpha(true));
    EXPECT_EQ(t["e"].val(), "true");
    t["e"].load(&w);
    EXPECT_EQ(w, true);

    t["e"].save(fmt::boolalpha(false));
    EXPECT_EQ(t["e"].val(), "false");
    t["e"].load(&w);
    EXPECT_EQ(w, false);
}


TEST(serialize, integral)
{
    auto makeyaml = [](std::string &s, csubstr fmt, csubstr number){
        size_t sz = 2 * fmt.len;
again:
        s.resize(sz);
        sz = fmt.replace_all(to_substr(s), "number", number);
        if(sz > s.size())
            goto again; // NOLINT
        s.resize(sz);
    };
    auto checkintegral = [](ConstNodeRef n, int expected){
        {
            int i = expected + 1;
            n.load(&i);
            EXPECT_EQ(i, expected);
        }
        {
            int i = expected + 1;
            n.load_key(&i);
            EXPECT_EQ(i, expected);
        }
    };
    csubstr fmt = R"(
plain: {number: number}
dq: {"number": "number"}
sq: {'number': 'number'}
literal:
  ? |-
    number
  : |-
    number
folded:
  ? >-
    number
  : >-
    number
)";
    std::string yaml;
    struct numberpair { int val; const char *str; };
    const numberpair numbers[] = {
        {0, "0"},
        {10, "10"},
        {+20, "+20"},
        {-30, "-30"},
        {100, "100"},
        {0xaf, "0xaf"},
        {-0xaf, "-0xaf"},
        {123, "0123"},
        {123, "00123"},
    };
    for(numberpair np : numbers)
    {
        makeyaml(yaml, fmt, np.str);
        RYML_TRACE_FMT("int={} str={} yaml={}", np.val, np.str, yaml);
        test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
            checkintegral(t["plain"][0], np.val);
            checkintegral(t["dq"][0], np.val);
            checkintegral(t["sq"][0], np.val);
            checkintegral(t["literal"][0], np.val);
            checkintegral(t["folded"][0], np.val);
        });
    }
}

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
        std::string s;
        s = {}; ASSERT_TRUE( r[0].deserialize_key(&s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(mr[0].deserialize_key(&s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE( r[0].deserialize(&s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].deserialize(&s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[1].deserialize_key(&s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[1].deserialize_key(&s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[1].deserialize(&s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].deserialize(&s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[2].deserialize_key(&s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[2].deserialize_key(&s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[2].deserialize(&s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].deserialize(&s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_key( r[0].id(), &s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[0].id(), &s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize( r[0].id(), &s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize(mr[0].id(), &s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_key( r[1].id(), &s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[1].id(), &s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize( r[1].id(), &s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize(mr[1].id(), &s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_key( r[2].id(), &s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[2].id(), &s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize( r[2].id(), &s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize(mr[2].id(), &s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
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
        EXPECT_FALSE(node.deserialize(w));
        EXPECT_FALSE(node.deserialize(&w));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf)));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf, &szk)));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf))));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf), &szk)));
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

TEST(serialize, base64)
{
    Tree t = parse_in_arena("{'Hello, World!': SGVsbG8sIFdvcmxkIQ==, not: base64}");
    print_tree(t);
    std::string result;
    t["Hello, World!"].load(fmt::base64(result));
    EXPECT_EQ("Hello, World!", result);
    ExpectError::check_error_visit(&t, [&]{ t["not"].load(fmt::base64(result)); }, t["not"].id());
}

// inf and nan are tested in test_number.cpp

TEST(serialize, get_if)
{
    Tree t = parse_in_arena(
        "map:\n"
        "  value: 10\n"
        "  badvalue: reallybad\n"
        "seq:\n"
        "  - 10\n"
        "  - reallybad\n"
        "");
    int val;
    val = 0; EXPECT_TRUE (t["seq"].get_if(0, &val)); EXPECT_EQ(val, 10);
    val = 0; EXPECT_FALSE(t["seq"].get_if(2, &val)); EXPECT_EQ(val, 0);
    val = 0; EXPECT_TRUE (t["seq"].get_if(0, &val, 11)); EXPECT_EQ(val, 10);
    val = 0; EXPECT_FALSE(t["seq"].get_if(2, &val, 11)); EXPECT_EQ(val, 11);
    val = 0; EXPECT_TRUE (t["map"].get_if("value"  , &val)); EXPECT_EQ(val, 10);
    val = 0; EXPECT_FALSE(t["map"].get_if("novalue", &val)); EXPECT_EQ(val, 0);
    val = 0; EXPECT_TRUE (t["map"].get_if("value"  , &val, 11)); EXPECT_EQ(val, 10);
    val = 0; EXPECT_FALSE(t["map"].get_if("novalue", &val, 11)); EXPECT_EQ(val, 11);
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t["seq"].get_if(1, &val); }, t["seq"][1].id()));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t["seq"].get_if(1, &val, 11); }, t["seq"][1].id()));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t["map"].get_if("badvalue", &val); }, t["map"]["badvalue"].id()));
    RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t["map"].get_if("badvalue", &val, 11); }, t["map"]["badvalue"].id()));
}

TEST(serialize, std_string)
{
    auto t = parse_in_arena("{foo: bar}");
    std::string s;
    EXPECT_NE(s, "bar");
    t["foo"].load(&s);
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
    tree["anchors"][0].set_val("val0");
    tree["anchors"][0].set_val_anchor("id0");
    tree["anchors"][1].set_map();
    tree["anchors"][1].set_val_anchor("id1");
    tree["anchors"][1]["key1"].set_val("val1");
    tree["anchors"][2].set_seq();
    tree["anchors"][2].set_val_anchor("id2");
    tree["anchors"][2][0].set_val("val2");
    auto setseq = [](NodeRef n, NodeType style){
        n.set_seq(style);
        n[0].set_val("id0");
        n[0].set_val_ref("id0");
        n[1].set_val("id1");
        n[1].set_val_ref("id1");
        n[2].set_val("id2");
        n[2].set_val_ref("id2");
    };
    auto setmap = [](NodeRef n, NodeType style){
        n.set_map(style);
        n["*id0"].set_val("id0");
        n["*id0"].set_key_ref("id0");
        n["*id0"].set_val_ref("id1");
        n["next"].set_val("id2");
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
    { int value = 0; EXPECT_FALSE(read(&tree, cnode.id(), &value)); }
    { int value = 0; EXPECT_FALSE(read(&tree, node.id(), &value)); }
    { int value = 0; EXPECT_FALSE(read(cnode, &value)); }
    { int value = 0; EXPECT_FALSE(read(node, &value)); }
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        cnode.load(&value);
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 0;
        node.load(&value);
    });
    { double value = 0; EXPECT_FALSE(read(&tree, cnode.id(), &value)); }
    { double value = 0; EXPECT_FALSE(read(&tree, node.id(), &value)); }
    { double value = 0; EXPECT_FALSE(read(cnode, &value)); }
    { double value = 0; EXPECT_FALSE(read(node, &value)); }
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        cnode.load(&value);
    });
    ExpectError::check_error_visit(&tree, [&]{
        double value = 0;
        node.load(&value);
    });
    { std::string value; EXPECT_TRUE(read(&tree, cnode.id(), &value)); EXPECT_TRUE(value.empty()); }
    { std::string value; EXPECT_TRUE(read(&tree, node.id(), &value)); EXPECT_TRUE(value.empty()); }
    { std::string value; EXPECT_TRUE(read(cnode, &value)); EXPECT_TRUE(value.empty()); }
    { std::string value; EXPECT_TRUE(read(node, &value)); EXPECT_TRUE(value.empty()); }
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
        cnode.load(&value);
    });
    ExpectError::check_error_visit(&tree, [&]{
        int value = 1;
        node.load(&value);
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
        cnode.load(&value);
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        float value = 1;
        node.load(&value);
        EXPECT_EQ(memcmp(&value, &fval, sizeof(fval)), 0);
    }
    {
        double value = 1;
        cnode.load(&value);
        EXPECT_EQ(memcmp(&value, &dval, sizeof(fval)), 0);
    }
    {
        double value = 1;
        node.load(&value);
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
        tree[0].load(&obj);
        Tree out_tree;
        out_tree.rootref().set_seq();
        out_tree[0].save(obj);
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
        tree["val"].load(&obj);
        Tree out_tree;
        out_tree.rootref().set_map();
        out_tree["val"].save(obj);
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
        tree.rootref().load(&obj);
        Tree out_tree;
        out_tree.rootref().save(obj);
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
    EXPECT_TRUE(scalar_style_query_plain_flow("123"));
    EXPECT_TRUE(scalar_style_query_plain_flow("-123"));
    EXPECT_TRUE(scalar_style_query_plain_flow("+123"));
    EXPECT_EQ(scalar_style_choose_flow("123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_flow("-123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_flow("+123"), SCALAR_PLAIN);
    EXPECT_TRUE(scalar_style_query_plain_block("123"));
    EXPECT_TRUE(scalar_style_query_plain_block("-123"));
    EXPECT_TRUE(scalar_style_query_plain_block("+123"));
    EXPECT_EQ(scalar_style_choose_block("123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_block("-123"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_block("+123"), SCALAR_PLAIN);
    {
        Tree tree;
        tree.rootref().save("123");
        EXPECT_EQ(std::string("123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save("-123");
        EXPECT_EQ(std::string("-123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save(123);
        EXPECT_EQ(std::string("123\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save(-123);
        EXPECT_EQ(std::string("-123\n"), emitrs_yaml<std::string>(tree));
    }
}
TEST(serialize, issue442_61)
{
    EXPECT_TRUE(scalar_style_query_plain_flow("2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain_flow("-2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain_flow("+2.35e-10"));
    EXPECT_EQ(scalar_style_choose_flow("2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_flow("-2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_flow("+2.35e-10"), SCALAR_PLAIN);
    EXPECT_TRUE(scalar_style_query_plain_block("2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain_block("-2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain_block("+2.35e-10"));
    EXPECT_EQ(scalar_style_choose_block("2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_block("-2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_block("+2.35e-10"), SCALAR_PLAIN);
    {
        Tree tree;
        tree.rootref().save(2.35e-10);
        EXPECT_EQ(std::string("2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save(-2.35e-10);
        EXPECT_EQ(std::string("-2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save("2.35e-10");
        EXPECT_EQ(std::string("2.35e-10\n"), emitrs_yaml<std::string>(tree));
    }
    {
        Tree tree;
        tree.rootref().save("-2.35e-10");
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
    t.rootref().load(&vec);
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
    t.rootref().load(&map);
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
