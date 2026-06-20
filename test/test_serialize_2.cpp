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
