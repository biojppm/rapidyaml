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

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
#if __GNUC__ >= 6
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
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
    vec2<int> v2out;
    r["v2"] << v2in;
    r["v2"] >> v2out;
    EXPECT_EQ(v2in.x, v2out.x);
    EXPECT_EQ(v2in.y, v2out.y);

    vec3<int> v3in{100, 101, 102};
    vec3<int> v3out;
    r["v3"] << v3in;
    r["v3"] >> v3out;
    EXPECT_EQ(v3in.x, v3out.x);
    EXPECT_EQ(v3in.y, v3out.y);
    EXPECT_EQ(v3in.z, v3out.z);

    vec4<int> v4in{1000, 1001, 1002, 1003};
    vec4<int> v4out;
    r["v4"] << v4in;
    r["v4"] >> v4out;
    EXPECT_EQ(v4in.x, v4out.x);
    EXPECT_EQ(v4in.y, v4out.y);
    EXPECT_EQ(v4in.z, v4out.z);
    EXPECT_EQ(v4in.w, v4out.w);

    char buf[256];
    c4::csubstr interm = c4::yml::emit_json(t, buf);
    EXPECT_EQ(interm, R"_({"v2": "(10,11)","v3": "(100,101,102)","v4": "(1000,1001,1002,1003)"})_");
}
} // namespace foo

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(general, emitting)
{
    std::string cmpbuf;
    std::string cmpbuf2;

    Tree tree;
    auto r = tree.rootref();

    r |= MAP;  // this is needed to make the root a map

    r["foo"] = "1"; // ryml works only with strings.
    // Note that the tree will be __pointing__ at the
    // strings "foo" and "1" used here. You need
    // to make sure they have at least the same
    // lifetime as the tree.

    auto s = r["seq"]; // does not change the tree until s is written to.
    s |= SEQ;
    r["seq"].append_child() = "bar0"; // value of this child is now __pointing__ at "bar0"
    r["seq"].append_child() = "bar1";
    r["seq"].append_child() = "bar2";

    //print_tree(tree);

    // emit to stdout (can also emit to FILE* or ryml::span)
    emitrs_json(tree, &cmpbuf);
    EXPECT_EQ(cmpbuf, R"({"foo": 1,"seq": ["bar0","bar1","bar2"]})");

    // serializing: using operator<< instead of operator=
    // will make the tree serialize the value into a char
    // arena inside the tree. This arena can be reserved at will.
    int ch3 = 33, ch4 = 44;
    s.append_child() << ch3;
    s.append_child() << ch4;

    {
        std::string tmp = "child5";
        s.append_child() << tmp;
        // now tmp can go safely out of scope, as it was
        // serialized to the tree's internal string arena
    }

    emitrs_json(tree, &cmpbuf);
    EXPECT_EQ(cmpbuf, R"({"foo": 1,"seq": ["bar0","bar1","bar2",33,44,"child5"]})");

    // to serialize keys:
    int k = 66;
    r.append_child() << key(k) << 7;
    emitrs_json(tree, &cmpbuf);
    EXPECT_EQ(cmpbuf, R"({"foo": 1,"seq": ["bar0","bar1","bar2",33,44,"child5"],"66": 7})");
}

TEST(general, map_to_root)
{
    std::string cmpbuf; const char *exp;
    std::map<std::string, int> m({{"bar", 2}, {"foo", 1}});
    Tree t;
    t.rootref() << m;

    emitrs_json(t, &cmpbuf);
    exp = "{\"bar\": 2,\"foo\": 1}";
    EXPECT_EQ(cmpbuf, exp);

    t["foo"] << 10;
    t["bar"] << 20;

    m.clear();
    t.rootref() >> m;

    EXPECT_EQ(m["foo"], 10);
    EXPECT_EQ(m["bar"], 20);
}

TEST(general, json_stream_operator)
{
    std::map<std::string, int> out, m({{"bar", 2}, {"foo", 1}, {"foobar_barfoo:barfoo_foobar", 1001}, {"asdfjkl;", 42}, {"00000000000000000000000000000000000000000000000000000000000000", 1}});
    Tree t;
    t.rootref() << m;
    std::string str;
    {
        std::stringstream ss;
        ss << as_json(t);
        str = ss.str();
    }
    Tree res = c4::yml::parse_json_in_place(to_substr(str));
    EXPECT_EQ(res["foo"].val(), "1");
    EXPECT_EQ(res["bar"].val(), "2");
    EXPECT_EQ(res["foobar_barfoo:barfoo_foobar"].val(), "1001");
    EXPECT_EQ(res["asdfjkl;"].val(), "42");
    EXPECT_EQ(res["00000000000000000000000000000000000000000000000000000000000000"].val(), "1");
    res.rootref() >> out;
    EXPECT_EQ(out["foo"], 1);
    EXPECT_EQ(out["bar"], 2);
    EXPECT_EQ(out["foobar_barfoo:barfoo_foobar"], 1001);
    EXPECT_EQ(out["asdfjkl;"], 42);
    EXPECT_EQ(out["00000000000000000000000000000000000000000000000000000000000000"], 1);
}

TEST(emit_json, issue72)
{
    Tree t;
    NodeRef r = t.rootref();

    r |= MAP;
    r["1"] = "null";
    r["2"] = "true";
    r["3"] = "false";
    r["null"] = "1";
    r["true"] = "2";
    r["false"] = "3";

    std::string out;
    emitrs_json(t, &out);

    EXPECT_EQ(out, R"({"1": null,"2": true,"3": false,"null": 1,"true": 2,"false": 3})");
}


TEST(emit_json, issue121)
{
    csubstr json = R"({"string_value": "string","number_value": "9001","broken_value": "0.30.2"})";
    const Tree t = parse_json_in_arena(json);
    EXPECT_TRUE(t["string_value"].get()->m_type.type & VAL_DQUO);
    EXPECT_TRUE(t["number_value"].get()->m_type.type & VAL_DQUO);
    EXPECT_TRUE(t["broken_value"].get()->m_type.type & VAL_DQUO);
    std::string out;
    emitrs_json(t, &out);
    EXPECT_EQ(out, json);
    out.clear();
    emitrs_yaml(t, &out);
    EXPECT_EQ(out, json);
}

TEST(emit_json, issue291)
{
    Tree t = parse_json_in_arena("{}");
    t["james"] = "045";
    auto s = emitrs_json<std::string>(t);
    EXPECT_EQ(s, "{\"james\": \"045\"}");
}

TEST(emit_json, issue292)
{
    EXPECT_FALSE(csubstr("0.0.0").is_number());
    EXPECT_FALSE(csubstr("0.0.0").is_integer());
    EXPECT_FALSE(csubstr("0.0.0").is_real());
    EXPECT_FALSE(csubstr("0.1.0").is_number());
    EXPECT_FALSE(csubstr("0.1.0").is_integer());
    EXPECT_FALSE(csubstr("0.1.0").is_real());
    EXPECT_FALSE(csubstr("0.6.1").is_number());
    EXPECT_FALSE(csubstr("0.6.1").is_integer());
    EXPECT_FALSE(csubstr("0.6.1").is_real());
    EXPECT_FALSE(csubstr("1.1.9").is_number());
    EXPECT_FALSE(csubstr("1.1.9").is_integer());
    EXPECT_FALSE(csubstr("1.1.9").is_real());
    EXPECT_FALSE(csubstr("1.2.3").is_number());
    EXPECT_FALSE(csubstr("1.2.3").is_integer());
    EXPECT_FALSE(csubstr("1.2.3").is_real());
    Tree t = parse_json_in_arena("{}");
    t["james"] = "0.0.0";
    EXPECT_EQ(emitrs_json<std::string>(t), "{\"james\": \"0.0.0\"}");
    t["james"] = "0.1.0";
    EXPECT_EQ(emitrs_json<std::string>(t), "{\"james\": \"0.1.0\"}");
    t["james"] = "0.6.1";
    EXPECT_EQ(emitrs_json<std::string>(t), "{\"james\": \"0.6.1\"}");
    t["james"] = "1.1.9";
    EXPECT_EQ(emitrs_json<std::string>(t), "{\"james\": \"1.1.9\"}");
    t["james"] = "1.2.3";
    EXPECT_EQ(emitrs_json<std::string>(t), "{\"james\": \"1.2.3\"}");
}

TEST(emit_json, issue297)
{
    char yml_buf[] = R"(
comment: |
   abc
   def
)";
    Tree t = parse_in_place(yml_buf);
    auto s = emitrs_json<std::string>(t);
    EXPECT_EQ(s, "{\"comment\": \"abc\\ndef\\n\"}");
}

TEST(emit_json, issue297_escaped_chars)
{
    Tree t = parse_json_in_arena("{}");
    t["quote"] = "abc\"def";
    t["newline"] = "abc\ndef";
    t["tab"] = "abc\tdef";
    t["carriage"] = "abc\rdef";
    t["backslash"] = "abc\\def";
    t["backspace"] = "abc\bdef";
    t["formfeed"] = "abc\fdef";
    std::string expected = R"({"quote": "abc\"def","newline": "abc\ndef","tab": "abc\tdef","carriage": "abc\rdef","backslash": "abc\\def","backspace": "abc\bdef","formfeed": "abc\fdef"})";
    auto actual = emitrs_json<std::string>(t);
    EXPECT_EQ(actual, expected);
}

namespace {
std::string k(ConstNodeRef node) { return std::string(node.key().str, node.key().len); }
std::string v(ConstNodeRef node) { return std::string(node.val().str, node.val().len); }
}
TEST(emit_json, issue313_quoted_numbers__1)
{
    EXPECT_TRUE(csubstr("0.99356698989868164").is_number()); // [WEIRD0][0]
    EXPECT_TRUE(csubstr("0.99356698989868164").is_real()); // [WEIRD0][0]
    EXPECT_FALSE(csubstr("0.99356698989868164").is_integer()); // [WEIRD0][0]
    EXPECT_TRUE(csubstr("0.0064908224157989025").is_number()); // [WEIRD2][0]
    EXPECT_TRUE(csubstr("0.0064908224157989025").is_real()); // [WEIRD2][0]
    EXPECT_FALSE(csubstr("0.0064908224157989025").is_integer()); // [WEIRD2][0]
    EXPECT_TRUE(csubstr("0.0064917667768895626").is_number()); // [WEIRD2][1]
    EXPECT_TRUE(csubstr("0.0064917667768895626").is_real()); // [WEIRD2][1]
    EXPECT_FALSE(csubstr("0.0064917667768895626").is_integer()); // [WEIRD2][1]
    EXPECT_TRUE(csubstr("0.0064947893843054771").is_number()); // [WEIRD2][2]
    EXPECT_TRUE(csubstr("0.0064947893843054771").is_real()); // [WEIRD2][2]
    EXPECT_FALSE(csubstr("0.0064947893843054771").is_integer()); // [WEIRD2][2]
    EXPECT_TRUE(csubstr("0.91054189205169678").is_number()); // [WEIRD4][0]
    EXPECT_TRUE(csubstr("0.91054189205169678").is_real()); // [WEIRD4][0]
    EXPECT_FALSE(csubstr("0.91054189205169678").is_integer()); // [WEIRD4][0]
    EXPECT_TRUE(csubstr("0.13215841352939606").is_number()); // [REALLY_WEIRD5][9][0]
    EXPECT_TRUE(csubstr("0.13215841352939606").is_real()); // [REALLY_WEIRD5][9][0]
    EXPECT_FALSE(csubstr("0.13215841352939606").is_integer()); // [REALLY_WEIRD5][9][0]
    const Tree t0 = parse_json_in_arena(R"([
    0.99356698989868164,
    0.0064908224157989025,
    0.0064917667768895626,
    0.0064947893843054771,
    0.91054189205169678,
    0.13215841352939606,
])");
    std::string yaml = emitrs_json<std::string>(t0);
    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        for(ConstNodeRef number : t.rootref().children())
        {
            ASSERT_TRUE(number.is_val());
            EXPECT_FALSE(number.is_val_quoted()) << "tree[" << t.rootref().child_pos(number) << "]=" << v(number);
        }
    });
}


TEST(emit_json, issue313_quoted_numbers__2)
{
    Tree ti = parse_json_in_arena(R"({
"WEIRD0": [0.99356698989868164, 1.0605627298355103],
"OK1":    [0, 0, 0],
"WEIRD2": [0.0064908224157989025, 0.0064917667768895626, 0.0064947893843054771],
"OK3":    [6.6227097511291504, 6.8674740791320801, 7.0403199195861816, 7.5792555809020996, 7.9916787147521973, 8.136042594909668, 8.5505847930908203, 8.701807975769043, 8.926518440246582, 8.9484291076660156, 9.0740194320678711, 9.3788108825683594, 9.406926155090332],
"WEIRD4": [0.91054189205169678, 0.98725020885467529, 1.070807933807373],
"REALLY_WEIRD5": [
  [1.5158847570419312, 1.6361792087554932],  # 0
  [1.0741721391677856, 1.1791903972625732],  # 1
  [1.4423576593399048, 1.7063977718353271],  # 2
  [1.1791903972625732],                      # 3
  [1.1493504047393799, 1.1791903972625732],  # 4
  [1.1791903972625732, 1.3334760665893555],  # 5
  [1.0655292272567749, 1.4933452606201172],  # 6
  [1.0712906122207642, 1.1791903972625732],  # 7
  [1.1791903972625732, 1.830910325050354],   # 8
  [0.13215841352939606, 1.4161584377288818], # 9
  [1.1791903972625732, 1.5179581642150879],  # 10
  [1.1791903972625732, 1.2864601612091064],  # 11
  [1.1791903972625732, 1.6865267753601074],  # 12
  [1.1791903972625732, 1.2192368507385254],  # 13
  [1.1130030155181885, 1.5196701288223267],  # 14
  [1.0621790885925293, 1.1791903972625732]   # 15
]})");
    std::string yaml = emitrs_json<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        for(ConstNodeRef node : t.rootref().children())
        {
            ASSERT_TRUE(node.is_seq());
            ASSERT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_key_quoted()) << "tree[" << k(node) << "]";
            if(node.key() != "REALLY_WEIRD5")
            {
                for(ConstNodeRef number : node.children())
                {
                    ASSERT_TRUE(number.is_val());
                    EXPECT_FALSE(number.is_val_quoted()) << "tree[" << k(node) << "][" << node.child_pos(number) << "]=" << v(number);
                }
            }
            else
            {
                for(ConstNodeRef seq : node.children())
                {
                    ASSERT_TRUE(seq.is_seq());
                    for(ConstNodeRef number : seq.children())
                    {
                        ASSERT_TRUE(number.is_val());
                        EXPECT_FALSE(number.is_val_quoted()) << "tree[" << k(node) << "][" << node.child_pos(seq) << "][" << seq.child_pos(number) << "]=" << v(number);
                    }
                }
            }
        }
    });
}


#define _test(actual_src, expected_src)                           \
    {                                                             \
        SCOPED_TRACE(__LINE__);                                   \
        csubstr file = __FILE__ ":" C4_XQUOTE(__LINE__);          \
        const Tree actual = parse_json_in_arena(file, actual_src);     \
        const Tree expected = parse_json_in_arena(file, expected_src); \
        print_tree(actual);                                       \
        test_compare(actual, expected);                           \
    }


TEST(json, compact_map)
{
    _test("", "");
    _test("{}", "{}");

    _test(R"({"a":{"a":"b"}})", R"({"a": {"a": "b"}})");
}

TEST(json, compact_seq)
{
    _test(R"(["a",["a","b"]])", R"(["a", ["a", "b"]])");
}

TEST(json, github142)
{
    _test(R"({"A":"B}"})",
          R"({"A": "B}"})");
    _test(R"({"A":"{B"})",
          R"({"A": "{B"})");
    _test(R"({"A":"{B}"})",
          R"({"A": "{B}"})");
    _test(R"({  "A":"B}"  })",
          R"({  "A": "B}"  })");
    _test(R"({"A":["B]","[C","[D]"]})",
          R"({"A": ["B]","[C","[D]"]})");
    //_test(R"({"A":["B\"]","[\"C","\"[D]\""]})", // VS2019 chokes on this.
    //      R"({"A": ["B\"]","[\"C","\"[D]\""]})");
}

TEST(json, github52)
{
    _test(R"({"a": "b","c": 42,"d": "e"})",
          R"({"a": "b","c": 42,"d": "e"})");
    _test(R"({"aaaa": "bbbb","cccc": 424242,"dddddd": "eeeeeee"})",
          R"({"aaaa": "bbbb","cccc": 424242,"dddddd": "eeeeeee"})");

    _test(R"({"a":"b","c":42,"d":"e"})",
          R"({"a": "b","c": 42,"d": "e"})");
    _test(R"({"aaaaa":"bbbbb","ccccc":424242,"ddddd":"eeeee"})",
          R"({"aaaaa": "bbbbb","ccccc": 424242,"ddddd": "eeeee"})");
    _test(R"({"a":"b","c":{},"d":"e"})",
          R"({"a": "b","c": {},"d": "e"})");
    _test(R"({"aaaaa":"bbbbb","ccccc":{    },"ddddd":"eeeee"})",
          R"({"aaaaa": "bbbbb","ccccc": {    },"ddddd": "eeeee"})");
    _test(R"({"a":"b","c":true,"d":"e"})",
          R"({"a": "b","c": true,"d": "e"})");
    _test(R"({"a":"b","c":false,"d":"e"})",
          R"({"a": "b","c": false,"d": "e"})");
    _test(R"({"a":"b","c":true,"d":"e"})",
          R"({"a": "b","c": true,"d": "e"})");
    _test(R"({"a":"b","c":null,"d":"e"})",
          R"({"a": "b","c": null,"d": "e"})");
    _test(R"({"aaaaa":"bbbbb","ccccc":false,"ddddd":"eeeee"})",
          R"({"aaaaa": "bbbbb","ccccc": false,"ddddd": "eeeee"})");
    _test(R"({"a":"b","c":false,"d":"e"})",
          R"({"a": "b","c": false,"d": "e"})");
    _test(R"({"aaaaa":"bbbbb","ccccc":true,"ddddd":"eeeee"})",
          R"({"aaaaa": "bbbbb","ccccc": true,"ddddd": "eeeee"})");
}

TEST(json, nested)
{
    _test(R"({"a":"b","c":{"a":"b","c":{},"d":"e"},"d":"e"})",
          R"({"a": "b","c": {"a": "b","c": {},"d": "e"},"d": "e"})");
    _test(R"({"a":"b","c":{"a":"b","c":{"a":"b","c":{},"d":"e"},"d":"e"},"d":"e"})",
          R"({"a": "b","c": {"a": "b","c": {"a": "b","c": {},"d": "e"},"d": "e"},"d": "e"})");
    _test(R"({"a":"b","c":{"a":"b","c":{"a":"b","c":{"a":"b","c":{},"d":"e"},"d":"e"},"d":"e"},"d":"e"})",
          R"({"a": "b","c": {"a": "b","c": {"a": "b","c": {"a": "b","c": {},"d": "e"},"d": "e"},"d": "e"},"d": "e"})");
    _test(R"({"a":"b","c":{"a":"b","c":{"a":"b","c":{"a":"b","c":{"a":"b","c":{},"d":"e"},"d":"e"},"d":"e"},"d":"e"},"d":"e"})",
          R"({"a": "b","c": {"a": "b","c": {"a": "b","c": {"a": "b","c": {"a": "b","c": {},"d": "e"},"d": "e"},"d": "e"},"d": "e"},"d": "e"})");

    _test(R"({"a":"b","c":["a","c","d","e"],"d":"e"})",
          R"({"a": "b","c": ["a","c","d","e"],"d": "e"})");
}

TEST(json, nested_end)
{
    _test(R"({"a":"b","d":"e","c":{"a":"b","d":"e","c":{}}})",
          R"({"a": "b","d": "e","c": {"a": "b","d": "e","c": {}}})");
    _test(R"({"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{}}}})",
          R"({"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {}}}})");
    _test(R"({"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{}}}}})",
          R"({"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {}}}}})");
    _test(R"({"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{"a":"b","d":"e","c":{}}}}}})",
          R"({"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {"a": "b","d": "e","c": {}}}}}})");
}

#undef _test

TEST(json, issue390)
{
    const Tree tree = parse_in_arena(R"(quntity: 9.5e7
quntity2: 95000000)");
    EXPECT_TRUE(csubstr("9.5e7").is_number());
    EXPECT_EQ(emitrs_json<std::string>(tree), R"({"quntity": 9.5e7,"quntity2": 95000000})");
}

TEST(parse_json, error_on_missing_seq_val)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[foo, , ]");
    });
}

TEST(parse_json, error_on_double_seq_val)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[0 1, ]");
    });
}

TEST(parse_json, error_on_double_seq_val_quoted)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[\"0\" 1, ]");
    });
}

TEST(parse_json, error_on_double_seq_val_quoted_2)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[\"0\" \"1\", ]");
    });
}

TEST(parse_json, error_on_double_seq_val_quoted_3)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[0 \"1\", ]");
    });
}

TEST(parse_json, error_on_double_map_val)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"key\": 0 1}");
    });
}

TEST(parse_json, error_on_double_map_val_quoted)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"key\": 0 \"1\"}");
    });
}

TEST(parse_json, error_on_double_map_val_quoted_2)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"key\": \"0\" 1}");
    });
}

TEST(parse_json, error_on_double_map_val_quoted_3)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"key\": \"0\" \"1\"}");
    });
}

TEST(parse_json, error_on_missing_seq_term)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("[foo, ");
    });
}

TEST(parse_json, error_on_missing_map_val)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"foo\": }");
    });
}

TEST(parse_json, error_on_missing_map_term)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"foo\": 0");
    });
}

TEST(parse_json, error_on_missing_map_colon)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"foo\" }");
    });
}

TEST(parse_json, error_on_bad_map_val)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{\"foo\": , }");
    });
}

TEST(parse_json, error_on_wrong_key_character)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{'foo': 1}");
    });
}

TEST(parse_json, error_on_unquoted_key_character)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("{foo: 1}");
    });
}

TEST(parse_json, error_on_bare_keyval)
{
    ExpectError::check_error([]{
        Tree tree = parse_json_in_arena("\"fails\": true");
    });
}

TEST(parse_json, scalar_src_dquoted)
{
    Tree expected;
    expected.rootref() = "dquoted";
    Tree actual = parse_json_in_arena("\"dquoted\"");
    test_compare(expected, actual);
}

TEST(parse_json, scalar_src_plain)
{
    Tree expected;
    expected.rootref() = "true";
    Tree actual = parse_json_in_arena("true");
    test_compare(expected, actual);
}

TEST(parse_json, leading_whitespace_scalar_dquoted)
{
    csubstr json = "    \n   \"dquoted\"";
    Tree expected;
    expected.rootref() = "dquoted";
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}

TEST(parse_json, leading_whitespace_scalar_src_plain)
{
    csubstr json = "    \n   true";
    Tree expected;
    expected.rootref() = "true";
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}

TEST(parse_json, empty_lines_on_seq)
{
    csubstr json = R"([

   0,

1
,

2


   ,3

])";
    Tree expected;
    NodeRef root = expected.rootref();
    root |= SEQ|FLOW_SL;
    root.append_child() = "0";
    root.append_child() = "1";
    root.append_child() = "2";
    root.append_child() = "3";
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}

TEST(parse_json, empty_lines_on_map)
{
    csubstr json = R"({"0"

   :0,

    "1"
  :
   1
,

  "2":
   2


   ,"3":3

})";
    Tree expected;
    NodeRef root = expected.rootref();
    root |= MAP|FLOW_SL;
    root.append_child({"0", "0"});
    root.append_child({"1", "1"});
    root.append_child({"2", "2"});
    root.append_child({"3", "3"});
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}

TEST(parse_json, seq_nested_on_map)
{
    csubstr json = R"({"seq":[0,1],"key":val})";
    Tree expected;
    NodeRef root = expected.rootref();
    root |= MAP|FLOW_SL;
    NodeRef seq = root.append_child({KEYSEQ, "seq"});
    seq.append_child() = "0";
    seq.append_child() = "1";
    root.append_child({"key", "val"});
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}

TEST(parse_json, seq_nested_on_seq_with_trailing_comma)
{
    csubstr json = R"([[0,1,],2,3,])";
    Tree expected;
    NodeRef root = expected.rootref();
    root |= SEQ|FLOW_SL;
    NodeRef seq = root.append_child(SEQ);
    seq.append_child() = "0";
    seq.append_child() = "1";
    root.append_child() = "2";
    root.append_child() = "3";
    Tree actual = parse_json_in_arena(json);
    test_compare(expected, actual);
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP
