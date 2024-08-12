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

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389) // signed/unsigned mismatch
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

namespace foo {

template<class T>
struct vec2
{
    T x, y;
};
template<class T>
struct vec3
{
    T x, y, z;
};
template<class T>
struct vec4
{
    T x, y, z, w;
};

template<class T> size_t to_chars(c4::substr buf, vec2<T> v) { return c4::format(buf, "({},{})", v.x, v.y); }
template<class T> size_t to_chars(c4::substr buf, vec3<T> v) { return c4::format(buf, "({},{},{})", v.x, v.y, v.z); }
template<class T> size_t to_chars(c4::substr buf, vec4<T> v) { return c4::format(buf, "({},{},{},{})", v.x, v.y, v.z, v.w); }

template<class T> bool from_chars(c4::csubstr buf, vec2<T> *v) { size_t ret = c4::unformat(buf, "({},{})", v->x, v->y); return ret != c4::yml::npos; }
template<class T> bool from_chars(c4::csubstr buf, vec3<T> *v) { size_t ret = c4::unformat(buf, "({},{},{})", v->x, v->y, v->z); return ret != c4::yml::npos; }
template<class T> bool from_chars(c4::csubstr buf, vec4<T> *v) { size_t ret = c4::unformat(buf, "({},{},{},{})", v->x, v->y, v->z, v->w); return ret != c4::yml::npos; }

TEST(serialize, type_as_str)
{
    c4::yml::Tree t;

    auto r = t.rootref();
    r |= c4::yml::MAP;

    vec2<int> v2in{10, 11};
    vec2<int> v2out{1, 2};
    r["v2"] << v2in;
    r["v2"] >> v2out;
    EXPECT_EQ(v2in.x, v2out.x);
    EXPECT_EQ(v2in.y, v2out.y);

    vec3<int> v3in{100, 101, 102};
    vec3<int> v3out{1, 2, 3};
    r["v3"] << v3in;
    r["v3"] >> v3out;
    EXPECT_EQ(v3in.x, v3out.x);
    EXPECT_EQ(v3in.y, v3out.y);
    EXPECT_EQ(v3in.z, v3out.z);

    vec4<int> v4in{1000, 1001, 1002, 1003};
    vec4<int> v4out{1, 2, 3, 4};
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
} // namespace foo


namespace c4 {
namespace yml {

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

TEST(serialize, nan_0)
{
    Tree t;
    t.rootref() |= SEQ;
    t[0] << std::numeric_limits<float>::quiet_NaN();
    t[1] << std::numeric_limits<double>::quiet_NaN();
    EXPECT_EQ(t[0].val(), ".nan");
    EXPECT_EQ(t[1].val(), ".nan");
    EXPECT_EQ(emitrs_yaml<std::string>(t),
              R"(- .nan
- .nan
)");
}

TEST(serialize, nan_1)
{
    csubstr yaml = R"(
good:
  - .nan
  -   .nan
  - .NaN
  - .NAN
  - nan
  -
   .nan
set:
  - nothing
  - nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        EXPECT_EQ(t["good"][0].val(), ".nan");
        EXPECT_EQ(t["good"][1].val(), ".nan");
        EXPECT_EQ(t["good"][2].val(), ".NaN");
        EXPECT_EQ(t["good"][3].val(), ".NAN");
        EXPECT_EQ(t["good"][4].val(), "nan");
        EXPECT_EQ(t["good"][5].val(), ".nan");
        float f;
        double d;
        f = 0.f;
        d = 0.;
        t["good"][0] >> f;
        t["good"][0] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
        f = 0.f;
        d = 0.;
        t["good"][1] >> f;
        t["good"][1] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
        f = 0.f;
        d = 0.;
        t["good"][2] >> f;
        t["good"][2] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
        f = 0.f;
        d = 0.;
        t["good"][3] >> f;
        t["good"][3] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
        f = 0.f;
        d = 0.;
        t["good"][4] >> f;
        t["good"][4] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
        f = 0.f;
        d = 0.;
        t["good"][5] >> f;
        t["good"][5] >> d;
        EXPECT_TRUE(std::isnan(f));
        EXPECT_TRUE(std::isnan(d));
    });
}

TEST(serialize, inf_0)
{
    Tree t;
    t.rootref() |= SEQ;
    const float finf = std::numeric_limits<float>::infinity();
    const double dinf = std::numeric_limits<double>::infinity();
    t[0] << finf;
    t[1] << dinf;
    t[2] << -finf;
    t[3] << -dinf;
    EXPECT_EQ(t[0].val(), ".inf");
    EXPECT_EQ(t[1].val(), ".inf");
    EXPECT_EQ(t[2].val(), "-.inf");
    EXPECT_EQ(t[3].val(), "-.inf");
    EXPECT_EQ(scalar_style_choose("-.inf"), SCALAR_PLAIN);
    EXPECT_EQ(emitrs_yaml<std::string>(t),
              R"(- .inf
- .inf
- -.inf
- -.inf
)");
}

TEST(serialize, inf_1)
{
    C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wfloat-equal");
    csubstr yaml = R"(
good:
  - .inf
  -   .inf
  - .Inf
  - .INF
  - inf
  - infinity
  -
   .inf
set:
  - nothing
  - nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        float finf = std::numeric_limits<float>::infinity();
        double dinf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(t["good"][0].val(), ".inf");
        EXPECT_EQ(t["good"][1].val(), ".inf");
        EXPECT_EQ(t["good"][2].val(), ".Inf");
        EXPECT_EQ(t["good"][3].val(), ".INF");
        EXPECT_EQ(t["good"][4].val(), "inf");
        EXPECT_EQ(t["good"][5].val(), "infinity");
        EXPECT_EQ(t["good"][6].val(), ".inf");
        float f;
        double d;
        f = 0.f;
        d = 0.;
        t["good"][0] >> f;
        t["good"][0] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][1] >> f;
        t["good"][1] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][2] >> f;
        t["good"][2] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][3] >> f;
        t["good"][3] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][4] >> f;
        t["good"][4] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][5] >> f;
        t["good"][5] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
        f = 0.f;
        d = 0.;
        t["good"][6] >> f;
        t["good"][6] >> d;
        EXPECT_TRUE(f == finf);
        EXPECT_TRUE(d == dinf);
    });
    C4_SUPPRESS_WARNING_GCC_CLANG_POP
}

TEST(serialize, inf_2)
{
    C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wfloat-equal");
    csubstr yaml = R"(
good:
  - -.inf
  -   -.inf
  - -.Inf
  - -.INF
  - -inf
  - -infinity
  -
   -.inf
set:
  - nothing
  - nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        float finf = std::numeric_limits<float>::infinity();
        double dinf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(t["good"][0].val(), "-.inf");
        EXPECT_EQ(t["good"][1].val(), "-.inf");
        EXPECT_EQ(t["good"][2].val(), "-.Inf");
        EXPECT_EQ(t["good"][3].val(), "-.INF");
        EXPECT_EQ(t["good"][4].val(), "-inf");
        EXPECT_EQ(t["good"][5].val(), "-infinity");
        EXPECT_EQ(t["good"][6].val(), "-.inf");
        float f;
        double d;
        f = 0.f;
        d = 0.;
        t["good"][0] >> f;
        t["good"][0] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][1] >> f;
        t["good"][1] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][2] >> f;
        t["good"][2] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][3] >> f;
        t["good"][3] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][4] >> f;
        t["good"][4] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][5] >> f;
        t["good"][5] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
        f = 0.f;
        d = 0.;
        t["good"][6] >> f;
        t["good"][6] >> d;
        EXPECT_TRUE(f == -finf);
        EXPECT_TRUE(d == -dinf);
    });
    C4_SUPPRESS_WARNING_GCC_CLANG_POP
}

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
    const char expected_yaml[] = R"(anchor_objects:
  - &id001
    name: a_name
reference_list:
  - *id001
)";

    Tree tree;
    const id_type root_id = tree.root_id();
    tree.to_map(root_id);

    const id_type anchor_list_id = tree.append_child(root_id);
    tree.to_seq(anchor_list_id, "anchor_objects");

    const id_type anchor_map0 = tree.append_child(anchor_list_id);
    tree.to_map(anchor_map0);
    tree.set_val_anchor(anchor_map0, "id001");

    const id_type anchor_elem0 = tree.append_child(anchor_map0);
    tree.to_keyval(anchor_elem0, "name", "a_name");

    const id_type ref_list_id = tree.append_child(root_id);
    tree.to_seq(ref_list_id, "reference_list");

    const id_type elem0_id = tree.append_child(ref_list_id);
    tree.set_val_ref(elem0_id, "id001");

    EXPECT_EQ(emitrs_yaml<std::string>(tree), expected_yaml);
}

TEST(deserialize, issue434_0)
{
    Tree tree = parse_in_arena("{empty: }");
    ConstNodeRef cnode = tree["empty"];
    NodeRef node = tree["empty"];
    {
        int value = 0;
        EXPECT_FALSE(read(cnode, &value));
    }
    {
        int value = 0;
        EXPECT_FALSE(read(node, &value));
    }
    ExpectError::check_error(&tree, [&]{
        int value = 0;
        cnode >> value;
    });
    ExpectError::check_error(&tree, [&]{
        int value = 0;
        node >> value;
    });
    {
        double value = 0;
        EXPECT_FALSE(read(cnode, &value));
    }
    {
        double value = 0;
        EXPECT_FALSE(read(node, &value));
    }
    ExpectError::check_error(&tree, [&]{
        double value = 0;
        cnode >> value;
    });
    ExpectError::check_error(&tree, [&]{
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
    ExpectError::check_error(&tree, [&]{
        int value = 1;
        cnode >> value;
    });
    ExpectError::check_error(&tree, [&]{
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
        out_tree.rootref() |= SEQ;
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
        out_tree.rootref() |= MAP;
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
