#include "./test_case.hpp"
#include "c4/yml/std/std.hpp"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4127/*conditional expression is constant*/)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#endif

#include <yaml-cpp/yaml.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	return result;
}


namespace foo {

template< class T >
struct vec2
{
    T x, y;
};
template< class T >
struct vec3
{
    T x, y, z;
};
template< class T >
struct vec4
{
    T x, y, z, w;
};

template< class T > size_t to_str(c4::yml::span buf, vec2<T> v) { return c4::yml::cat(buf, '(', v.x, ',', v.y, ')'); }
template< class T > size_t to_str(c4::yml::span buf, vec3<T> v) { return c4::yml::cat(buf, '(', v.x, ',', v.y, ',', v.z, ')'); }
template< class T > size_t to_str(c4::yml::span buf, vec4<T> v) { return c4::yml::cat(buf, '(', v.x, ',', v.y, ',', v.z, ',', v.w, ')'); }

template< class T > size_t from_str(c4::yml::cspan buf, vec2<T> *v) { char c; return c4::yml::uncat(buf, c, v->x, c, v->y, c); }
template< class T > size_t from_str(c4::yml::cspan buf, vec3<T> *v) { char c; return c4::yml::uncat(buf, c, v->x, c, v->y, c, v->z, c); }
template< class T > size_t from_str(c4::yml::cspan buf, vec4<T> *v) { char c; return c4::yml::uncat(buf, c, v->x, c, v->y, c, v->z, c, v->w, c); }

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
    c4::yml::cspan ret = c4::yml::emit(t, buf);
    EXPECT_EQ(ret, R"(v2: (10,11)
v3: (100,101,102)
v4: (1000,1001,1002,1003)
)");
}

template< class Container, class... Args >
void do_test_serialize(Args&& ...args)
{
    using namespace c4::yml;
    Container s(std::forward< Args >(args)...);
    Container out;

    Tree t;
    NodeRef n(&t);

    n << s;
    print_tree(t);
    emit(t);
    check_invariants(t);
    n >> out;
    EXPECT_EQ(s, out);
}

TEST(serialize, std_vector_int)
{
    using T = int;
    using L = std::initializer_list<T>;
    do_test_serialize< std::vector<T> >(L{1, 2, 3, 4, 5});
}
TEST(serialize, std_vector_string)
{
    using T = std::string;
    using L = std::initializer_list<T>;
    do_test_serialize< std::vector<T> >(L{"asdadk", "sdfkjdfgu", "fdfdjkhdfgkjhdfi", "e987dfgnfdg8", "'d0fgºçdfg«"});
}
TEST(serialize, std_vector_std_vector_int)
{
    using T = std::vector<int>;
    using L = std::initializer_list<T>;
    do_test_serialize< std::vector<T> >(L{{1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}});
}


TEST(serialize, std_map__int_int)
{
    using M = std::map<int, int>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize< M >(L{{10, 0}, {11, 1}, {22, 2}, {10001, 1000}, {20002, 2000}, {30003, 3000}});
}
TEST(serialize, std_map__std_string_int)
{
    using M = std::map<std::string, int>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize< M >(L{{"asdsdf", 0}, {"dfgdfgdfg", 1}, {"dfgjdfgkjh", 2}});
}
TEST(serialize, std_map__string_vectori)
{
    using M = std::map<std::string, std::vector<int>>;
    using L = std::initializer_list<typename M::value_type>;
    do_test_serialize< M >(L{{"asdsdf", {0, 1, 2, 3}}, {"dfgdfgdfg", {4, 5, 6, 7}}, {"dfgjdfgkjh", {8, 9, 10, 11}}});
}
TEST(serialize, std_vector__map_string_int)
{
    using V = std::vector< std::map<std::string, int>>;
    using M = typename V::value_type;
    using L = std::initializer_list<M>;
    do_test_serialize< V >(L{
            M{{"asdasf",  0}, {"dfgkjhdfg",  1}, {"fghffg",  2}, {"r5656kjnh9b'dfgwg+*",  3}},
            M{{"asdasf", 10}, {"dfgkjhdfg", 11}, {"fghffg", 12}, {"r5656kjnh9b'dfgwg+*", 13}},
            M{{"asdasf", 20}, {"dfgkjhdfg", 21}, {"fghffg", 22}, {"r5656kjnh9b'dfgwg+*", 23}},
            M{{"asdasf", 30}, {"dfgkjhdfg", 31}, {"fghffg", 32}, {"r5656kjnh9b'dfgwg+*", 33}},
    });
}

} // namespace foo

namespace c4 {
namespace yml {

TEST(atoi, basic)
{
    char bufc[100] = {0};
    span buf(bufc);
    C4_ASSERT(buf.len == sizeof(bufc)-1);

    size_t ret;

#define _woof(val) \
    ret = itoa(buf, val); EXPECT_LT(ret, buf.len); EXPECT_EQ(buf.subspan(0, ret), #val)
    _woof(0);
    _woof(1);
    _woof(2);
    _woof(3);
    _woof(4);
    _woof(5);
    _woof(6);
    _woof(7);
    _woof(8);
    _woof(9);
    _woof(10);
    _woof(11);
    _woof(12);
    _woof(13);
    _woof(14);
    _woof(15);
    _woof(16);
    _woof(17);
    _woof(18);
    _woof(19);
    _woof(20);
    _woof(21);
    _woof(100);
    _woof(1000);
    _woof(11);
    _woof(101);
    _woof(1001);
    _woof(10001);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(span, ctor_from_char)
{
    char buf1[] = "{foo: 1}";
    char buf2[] = "{foo: 2}";
    span s(buf1);
    EXPECT_EQ(s, "{foo: 1}");
    s = buf2;
    EXPECT_EQ(s, "{foo: 2}");
}

TEST(cspan, ctor_from_char)
{
    char buf1[] = "{foo: 1}";
    char buf2[] = "{foo: 2}";
    span s(buf1);
    EXPECT_EQ(s, "{foo: 1}");
    s = buf2;
    EXPECT_EQ(s, "{foo: 2}");
}

TEST(span, begins_with)
{
    EXPECT_TRUE (cspan(": ").begins_with(":" ));
    EXPECT_TRUE (cspan(": ").begins_with(':' ));
    EXPECT_FALSE(cspan(":") .begins_with(": "));

    EXPECT_TRUE (cspan(    "1234").begins_with('0', 0));
    EXPECT_TRUE (cspan(   "01234").begins_with('0', 1));
    EXPECT_FALSE(cspan(   "01234").begins_with('0', 2));
    EXPECT_TRUE (cspan(  "001234").begins_with('0', 1));
    EXPECT_TRUE (cspan(  "001234").begins_with('0', 2));
    EXPECT_FALSE(cspan(  "001234").begins_with('0', 3));
    EXPECT_TRUE (cspan( "0001234").begins_with('0', 1));
    EXPECT_TRUE (cspan( "0001234").begins_with('0', 2));
    EXPECT_TRUE (cspan( "0001234").begins_with('0', 3));
    EXPECT_FALSE(cspan( "0001234").begins_with('0', 4));
    EXPECT_TRUE (cspan("00001234").begins_with('0', 1));
    EXPECT_TRUE (cspan("00001234").begins_with('0', 2));
    EXPECT_TRUE (cspan("00001234").begins_with('0', 3));
    EXPECT_TRUE (cspan("00001234").begins_with('0', 4));
    EXPECT_FALSE(cspan("00001234").begins_with('0', 5));
}

TEST(span, ends_with)
{
    EXPECT_TRUE(cspan("{% if foo %}bar{% endif %}").ends_with("{% endif %}"));

    EXPECT_TRUE (cspan("1234"    ).ends_with('0', 0));
    EXPECT_TRUE (cspan("12340"   ).ends_with('0', 1));
    EXPECT_FALSE(cspan("12340"   ).ends_with('0', 2));
    EXPECT_TRUE (cspan("123400"  ).ends_with('0', 1));
    EXPECT_TRUE (cspan("123400"  ).ends_with('0', 2));
    EXPECT_FALSE(cspan("123400"  ).ends_with('0', 3));
    EXPECT_TRUE (cspan("1234000" ).ends_with('0', 1));
    EXPECT_TRUE (cspan("1234000" ).ends_with('0', 2));
    EXPECT_TRUE (cspan("1234000" ).ends_with('0', 3));
    EXPECT_FALSE(cspan("1234000" ).ends_with('0', 4));
    EXPECT_TRUE (cspan("12340000").ends_with('0', 1));
    EXPECT_TRUE (cspan("12340000").ends_with('0', 2));
    EXPECT_TRUE (cspan("12340000").ends_with('0', 3));
    EXPECT_TRUE (cspan("12340000").ends_with('0', 4));
    EXPECT_FALSE(cspan("12340000").ends_with('0', 5));
}

TEST(span, first_of)
{
    EXPECT_EQ(cspan("012345").first_of('a'), npos);
    EXPECT_EQ(cspan("012345").first_of("ab"), npos);

    EXPECT_EQ(cspan("012345").first_of('0'), 0u);
    EXPECT_EQ(cspan("012345").first_of("0"), 0u);
    EXPECT_EQ(cspan("012345").first_of("01"), 0u);
    EXPECT_EQ(cspan("012345").first_of("10"), 0u);
    EXPECT_EQ(cspan("012345").first_of("012"), 0u);
    EXPECT_EQ(cspan("012345").first_of("210"), 0u);
    EXPECT_EQ(cspan("012345").first_of("0123"), 0u);
    EXPECT_EQ(cspan("012345").first_of("3210"), 0u);
    EXPECT_EQ(cspan("012345").first_of("01234"), 0u);
    EXPECT_EQ(cspan("012345").first_of("43210"), 0u);
    EXPECT_EQ(cspan("012345").first_of("012345"), 0u);
    EXPECT_EQ(cspan("012345").first_of("543210"), 0u);

    EXPECT_EQ(cspan("012345").first_of('5'), 5u);
    EXPECT_EQ(cspan("012345").first_of("5"), 5u);
    EXPECT_EQ(cspan("012345").first_of("45"), 4u);
    EXPECT_EQ(cspan("012345").first_of("54"), 4u);
    EXPECT_EQ(cspan("012345").first_of("345"), 3u);
    EXPECT_EQ(cspan("012345").first_of("543"), 3u);
    EXPECT_EQ(cspan("012345").first_of("2345"), 2u);
    EXPECT_EQ(cspan("012345").first_of("5432"), 2u);
    EXPECT_EQ(cspan("012345").first_of("12345"), 1u);
    EXPECT_EQ(cspan("012345").first_of("54321"), 1u);
    EXPECT_EQ(cspan("012345").first_of("012345"), 0u);
    EXPECT_EQ(cspan("012345").first_of("543210"), 0u);
}

TEST(span, last_of)
{
    EXPECT_EQ(cspan("012345").last_of('a'), npos);
    EXPECT_EQ(cspan("012345").last_of("ab"), npos);

    EXPECT_EQ(cspan("012345").last_of('0'), 0u);
    EXPECT_EQ(cspan("012345").last_of("0"), 0u);
    EXPECT_EQ(cspan("012345").last_of("01"), 1u);
    EXPECT_EQ(cspan("012345").last_of("10"), 1u);
    EXPECT_EQ(cspan("012345").last_of("012"), 2u);
    EXPECT_EQ(cspan("012345").last_of("210"), 2u);
    EXPECT_EQ(cspan("012345").last_of("0123"), 3u);
    EXPECT_EQ(cspan("012345").last_of("3210"), 3u);
    EXPECT_EQ(cspan("012345").last_of("01234"), 4u);
    EXPECT_EQ(cspan("012345").last_of("43210"), 4u);
    EXPECT_EQ(cspan("012345").last_of("012345"), 5u);
    EXPECT_EQ(cspan("012345").last_of("543210"), 5u);

    EXPECT_EQ(cspan("012345").last_of('5'), 5u);
    EXPECT_EQ(cspan("012345").last_of("5"), 5u);
    EXPECT_EQ(cspan("012345").last_of("45"), 5u);
    EXPECT_EQ(cspan("012345").last_of("54"), 5u);
    EXPECT_EQ(cspan("012345").last_of("345"), 5u);
    EXPECT_EQ(cspan("012345").last_of("543"), 5u);
    EXPECT_EQ(cspan("012345").last_of("2345"), 5u);
    EXPECT_EQ(cspan("012345").last_of("5432"), 5u);
    EXPECT_EQ(cspan("012345").last_of("12345"), 5u);
    EXPECT_EQ(cspan("012345").last_of("54321"), 5u);
    EXPECT_EQ(cspan("012345").last_of("012345"), 5u);
    EXPECT_EQ(cspan("012345").last_of("543210"), 5u);
}

TEST(span, first_not_of)
{
    EXPECT_EQ(cspan("012345").first_not_of('a'), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("ab"), 0u);

    EXPECT_EQ(cspan("012345").first_not_of('0'), 1u);
    EXPECT_EQ(cspan("012345").first_not_of("0"), 1u);
    EXPECT_EQ(cspan("012345").first_not_of("01"), 2u);
    EXPECT_EQ(cspan("012345").first_not_of("10"), 2u);
    EXPECT_EQ(cspan("012345").first_not_of("012"), 3u);
    EXPECT_EQ(cspan("012345").first_not_of("210"), 3u);
    EXPECT_EQ(cspan("012345").first_not_of("0123"), 4u);
    EXPECT_EQ(cspan("012345").first_not_of("3210"), 4u);
    EXPECT_EQ(cspan("012345").first_not_of("01234"), 5u);
    EXPECT_EQ(cspan("012345").first_not_of("43210"), 5u);
    EXPECT_EQ(cspan("012345").first_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").first_not_of("543210"), npos);

    EXPECT_EQ(cspan("012345").first_not_of('5'), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("5"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("45"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("54"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("345"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("543"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("2345"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("5432"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("12345"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("54321"), 0u);
    EXPECT_EQ(cspan("012345").first_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").first_not_of("543210"), npos);
}

TEST(span, last_not_of)
{
    EXPECT_EQ(cspan("012345").last_not_of('a'), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("ab"), 5u);

    EXPECT_EQ(cspan("012345").last_not_of('5'), 4u);
    EXPECT_EQ(cspan("012345").last_not_of("5"), 4u);
    EXPECT_EQ(cspan("012345").last_not_of("45"), 3u);
    EXPECT_EQ(cspan("012345").last_not_of("54"), 3u);
    EXPECT_EQ(cspan("012345").last_not_of("345"), 2u);
    EXPECT_EQ(cspan("012345").last_not_of("543"), 2u);
    EXPECT_EQ(cspan("012345").last_not_of("2345"), 1u);
    EXPECT_EQ(cspan("012345").last_not_of("5432"), 1u);
    EXPECT_EQ(cspan("012345").last_not_of("12345"), 0u);
    EXPECT_EQ(cspan("012345").last_not_of("54321"), 0u);
    EXPECT_EQ(cspan("012345").last_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").last_not_of("543210"), npos);

    EXPECT_EQ(cspan("012345").last_not_of('0'), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("0"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("01"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("10"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("012"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("210"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("0123"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("3210"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("01234"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("43210"), 5u);
    EXPECT_EQ(cspan("012345").last_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").last_not_of("543210"), npos);
}

TEST(span, compare)
{
    const char s1[] = "one empty doc";
    const char s2[] = "one empty doc, explicit termination";
    cspan c1(s1), c2(s2);
    EXPECT_NE(c1, c2);
    EXPECT_GT(c2, c1);
    EXPECT_TRUE((c1 > c2) != (c1 < c2));
}

TEST(span, span2cspan)
{
    char b[] = "some string";
    span s(b);
    cspan sc = s;
    EXPECT_EQ(sc, s);
    const span cs(b);
    const cspan csc(b);

}

TEST(span, first_of_any)
{
    EXPECT_EQ(cspan("baz{% endif %}").first_of_any("{% endif %}", "{% if "         , "{% elif bar %}" , "{% else %}" ).which, 0u);
    EXPECT_EQ(cspan("baz{% endif %}").first_of_any("{% if "     , "{% endif %}"    , "{% elif bar %}" , "{% else %}" ).which, 1u);
    EXPECT_EQ(cspan("baz{% endif %}").first_of_any("{% if "     , "{% elif bar %}" , "{% endif %}"    , "{% else %}" ).which, 2u);
    EXPECT_EQ(cspan("baz{% endif %}").first_of_any("{% if "     , "{% elif bar %}" , "{% else %}"     , "{% endif %}").which, 3u);

    EXPECT_EQ(cspan("bar{% else %}baz{% endif %}").first_of_any("{% else %}" , "{% if "         , "{% elif bar %}" , "{% endif %}").which, 0u);
    EXPECT_EQ(cspan("bar{% else %}baz{% endif %}").first_of_any("{% if "     , "{% else %}"     , "{% elif bar %}" , "{% endif %}").which, 1u);
    EXPECT_EQ(cspan("bar{% else %}baz{% endif %}").first_of_any("{% if "     , "{% elif bar %}" , "{% else %}"     , "{% endif %}").which, 2u);
    EXPECT_EQ(cspan("bar{% else %}baz{% endif %}").first_of_any("{% if "     , "{% elif bar %}" , "{% endif %}"    , "{% else %}" ).which, 3u);

    EXPECT_EQ(cspan("foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% elif bar %}" , "{% if "         , "{% else %}"     , "{% endif %}"   ).which, 0u);
    EXPECT_EQ(cspan("foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% if "         , "{% elif bar %}" , "{% else %}"     , "{% endif %}"   ).which, 1u);
    EXPECT_EQ(cspan("foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% if "         , "{% else %}"     , "{% elif bar %}" , "{% endif %}"   ).which, 2u);
    EXPECT_EQ(cspan("foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% if "         , "{% else %}"     , "{% endif %}"    , "{% elif bar %}").which, 3u);

    EXPECT_EQ(cspan("{% if foo %}foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% if "         , "{% elif bar %}" , "{% else %}" , "{% endif %}" ).which, 0u);
    EXPECT_EQ(cspan("{% if foo %}foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% elif bar %}" , "{% if "         , "{% else %}" , "{% endif %}" ).which, 1u);
    EXPECT_EQ(cspan("{% if foo %}foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% elif bar %}" , "{% else %}"     , "{% if "     , "{% endif %}" ).which, 2u);
    EXPECT_EQ(cspan("{% if foo %}foo{% elif bar %}bar{% else %}baz{% endif %}").first_of_any("{% elif bar %}" , "{% else %}"     , "{% endif %}", "{% if "      ).which, 3u);
}

TEST(span, first_non_empty_span)
{
    EXPECT_EQ(cspan("foo bar").first_non_empty_span(), "foo");
    EXPECT_EQ(cspan("       foo bar").first_non_empty_span(), "foo");
    EXPECT_EQ(cspan("\n   \r  \t  foo bar").first_non_empty_span(), "foo");
    EXPECT_EQ(cspan("\n   \r  \t  foo\n\r\t bar").first_non_empty_span(), "foo");
    EXPECT_EQ(cspan("\n   \r  \t  foo\n\r\t bar").first_non_empty_span(), "foo");
    EXPECT_EQ(cspan(",\n   \r  \t  foo\n\r\t bar").first_non_empty_span(), ",");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void node_scalar_test_empty(NodeScalar const& s)
{
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.tag, "");
    EXPECT_EQ(s.tag.len, 0u);
    EXPECT_TRUE(s.tag.empty());
    EXPECT_EQ(s.scalar, "");
    EXPECT_EQ(s.scalar.len, 0u);
    EXPECT_TRUE(s.scalar.empty());
}

void node_scalar_test_foo(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str");
        EXPECT_EQ(s.tag.len, 5u);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo");
    EXPECT_EQ(s.scalar.len, 3u);
    EXPECT_FALSE(s.scalar.empty());
}

void node_scalar_test_foo3(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str+++");
        EXPECT_EQ(s.tag.len, 8u);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0u);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo3");
    EXPECT_EQ(s.scalar.len, 4u);
    EXPECT_FALSE(s.scalar.empty());
}

TEST(NodeScalar, ctor_empty)
{
    NodeScalar s;
    node_scalar_test_empty(s);
}

TEST(NodeScalar, ctor__untagged)
{
    {
        const char sarr[] = "foo";
        const char *sptr = "foo"; size_t sptrlen = 3;
        cspan ssp = "foo";

        for(auto s : {NodeScalar(sarr), NodeScalar(sptr), NodeScalar(sptr, sptrlen), NodeScalar(ssp)})
        {
            node_scalar_test_foo(s);
        }

        NodeScalar s;
        s = {sarr};
        node_scalar_test_foo(s);
        s = {sptr};
        node_scalar_test_foo(s);
        s = {sptr, sptrlen};
        node_scalar_test_foo(s);
        s = {ssp};
        node_scalar_test_foo(s);
    }

    {
        const char sarr[] = "foo3";
        const char *sptr = "foo3"; size_t sptrlen = 4;
        cspan ssp = "foo3";

        for(auto s : {NodeScalar(sarr), NodeScalar(sptr), NodeScalar(sptr, sptrlen), NodeScalar(ssp)})
        {
            node_scalar_test_foo3(s);
        }

        NodeScalar s;
        {
            SCOPED_TRACE("here 1");
            s = {sarr};
            node_scalar_test_foo3(s);
        }
        {
            SCOPED_TRACE("here 2");
            s = {sptr};
            node_scalar_test_foo3(s);
        }
        {
            SCOPED_TRACE("here 3");
            s = {sptr, sptrlen};
            node_scalar_test_foo3(s);
        }
        {
            SCOPED_TRACE("here 4");
            s = {ssp};
            node_scalar_test_foo3(s);
        }
    }
}

TEST(NodeScalar, ctor__tagged)
{
    using ilist = std::initializer_list< NodeScalar >;

    {
        const char sarr[] = "foo", tarr[] = "!!str";
        const char *sptr = "foo"; size_t sptrlen = 3;
        const char *tptr = "!!str"; size_t tptrlen = 5;
        cspan ssp = "foo", tsp = "!!str";

        for(auto s : ilist{
                {tsp, ssp},
                    {tsp, sptr},
                    {tsp, sptr, sptrlen},
                    {tsp, sarr},
                {tptr, ssp},
                    {tptr, sptr},
                    {tptr, sptr, sptrlen},
                    {tptr, sarr},
                {tptr, tptrlen, ssp},
                    {tptr, tptrlen, sptr},
                    {tptr, tptrlen, sptr, sptrlen},
                    {tptr, tptrlen, sarr},
                {tarr, ssp},
                    {tarr, sptr},
                    {tarr, sptr, sptrlen},
                    {tarr, sarr},
                    })
        {
            node_scalar_test_foo(s, true);
        }

        NodeScalar s;

        {
            SCOPED_TRACE("here 0.0");
            s = {tsp, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 0.1");
            s = {tsp, sptr};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 0.2");
            s = {tsp, sptr, sptrlen};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 0.3");
            s = {tsp, sarr};
            node_scalar_test_foo(s, true);
        }

        {
            SCOPED_TRACE("here 1.0");
            s = {tptr, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 1.1");
            s = {tptr, sptr};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 1.2");
            s = {tptr, sptr, sptrlen};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 1.3");
            s = {tptr, sarr};
            node_scalar_test_foo(s, true);
        }

        {
            SCOPED_TRACE("here 2.0");
            s = {tptr, tptrlen, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 2.1");
            s = {tptr, tptrlen, sptr};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 2.2");
            s = {tptr, tptrlen, sptr, sptrlen};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 2.3");
            s = {tptr, tptrlen, sarr};
            node_scalar_test_foo(s, true);
        }

        {
            SCOPED_TRACE("here 3.0");
            s = {tarr, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 3.1");
            s = {tarr, sptr};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 3.2");
            s = {tarr, sptr, sptrlen};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 3.3");
            s = {tarr, sarr};
            node_scalar_test_foo(s, true);
        }

    }

    {
        const char sarr[] = "foo3", tarr[] = "!!str+++";
        const char *sptr = "foo3"; size_t sptrlen = 4;
        const char *tptr = "!!str+++"; size_t tptrlen = 8;
        cspan ssp = "foo3", tsp = "!!str+++";

        for(auto s : ilist{
                {tsp, ssp},
                    {tsp, sptr},
                    {tsp, sptr, sptrlen},
                    {tsp, sarr},
                {tptr, ssp},
                    {tptr, sptr},
                    {tptr, sptr, sptrlen},
                    {tptr, sarr},
                {tptr, tptrlen, ssp},
                    {tptr, tptrlen, sptr},
                    {tptr, tptrlen, sptr, sptrlen},
                    {tptr, tptrlen, sarr},
                {tarr, ssp},
                    {tarr, sptr},
                    {tarr, sptr, sptrlen},
                    {tarr, sarr},
                    })
        {
            node_scalar_test_foo3(s, true);
        }

        NodeScalar s;

        {
            SCOPED_TRACE("here 0.0");
            s = {tsp, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 0.1");
            s = {tsp, sptr};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 0.2");
            s = {tsp, sptr, sptrlen};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 0.3");
            s = {tsp, sarr};
            node_scalar_test_foo3(s, true);
        }

        {
            SCOPED_TRACE("here 1.0");
            s = {tptr, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 1.1");
            s = {tptr, sptr};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 1.2");
            s = {tptr, sptr, sptrlen};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 1.3");
            s = {tptr, sarr};
            node_scalar_test_foo3(s, true);
        }

        {
            SCOPED_TRACE("here 2.0");
            s = {tptr, tptrlen, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 2.1");
            s = {tptr, tptrlen, sptr};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 2.2");
            s = {tptr, tptrlen, sptr, sptrlen};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 2.3");
            s = {tptr, tptrlen, sarr};
            node_scalar_test_foo3(s, true);
        }

        {
            SCOPED_TRACE("here 3.0");
            s = {tarr, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 3.1");
            s = {tarr, sptr};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 3.2");
            s = {tarr, sptr, sptrlen};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 3.3");
            s = {tarr, sarr};
            node_scalar_test_foo3(s, true);
        }

    }

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(NodeInit, ctor__empty)
{
    NodeInit n;
    EXPECT_EQ(n.type, NOTYPE);
    EXPECT_EQ(n.key.scalar, "");
    EXPECT_EQ(n.key.tag, "");
    EXPECT_EQ(n.val.scalar, "");
    EXPECT_EQ(n.val.tag, "");
}

TEST(NodeInit, ctor__type_only)
{
    for(auto k : {KEY, KEYVAL, MAP, KEYMAP, SEQ, KEYSEQ})
    {
        SCOPED_TRACE(NodeType::type_str(k));
        NodeInit n(k);
        EXPECT_EQ(n.type, k);
        EXPECT_EQ(n.key.scalar, "");
        EXPECT_EQ(n.key.tag, "");
        EXPECT_EQ(n.val.scalar, "");
        EXPECT_EQ(n.val.tag, "");
    }
}

TEST(NodeInit, ctor__val_only)
{
    using ilist = std::initializer_list< NodeInit >;
    {
        const char sarr[] = "foo";
        const char *sptr = "foo"; size_t sptrlen = 3;
        cspan ssp = "foo";

        {
            SCOPED_TRACE("here 0");
            {
                NodeInit s{sarr};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s{sptr};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s{{sptr, sptrlen}};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s{sarr};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 1");
            {
                NodeInit s(sarr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s(sptr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s({sptr, sptrlen});
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s(sarr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 2");
            NodeInit s;
            s = {sarr};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
            s = {sptr};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
            //s = {sptr, sptrlen}; // fails to compile
            //node_scalar_test_foo(s.val);
            //node_scalar_test_empty(s.key);
            //s.clear();
            s = {ssp};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
        }

        for(auto s : ilist{{sarr}, {sptr}, {{sptr, sptrlen}}, {ssp}})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
        }
    }

    {
        const char sarr[] = "foo3";
        const char *sptr = "foo3"; size_t sptrlen = 4;
        cspan ssp = "foo3";

        {
            SCOPED_TRACE("here 0");
            NodeInit s = {sarr};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
        {   // FAILS
            SCOPED_TRACE("here 1");
            //NodeInit s = sarr;
            //node_scalar_test_foo3(s.val);
            //node_scalar_test_empty(s.key);
        }
        {
            SCOPED_TRACE("here 2");
            NodeInit s{sarr};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
        {
            SCOPED_TRACE("here 3");
            NodeInit s(sarr);
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }

        for(auto s : ilist{{sarr}, {sptr}, {{sptr, sptrlen}}, {ssp}})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(NodeRef, 0_general)
{
    Tree t;

    NodeRef root(&t);

    //using S = cspan;
    //using V = NodeScalar;
    using N = NodeInit;

    root = N{MAP};
    root.append_child({"a", "0"});
    root.append_child({MAP, "b"});
    root["b"].append_child({SEQ, "seq"});
    root["b"]["seq"].append_child({"0"});
    root["b"]["seq"].append_child({"1"});
    root["b"]["seq"].append_child({"2"});
    root["b"]["seq"].append_child({NodeScalar{"!!str", "3"}});
    auto ch4 = root["b"]["seq"][3].append_sibling({"4"});
    EXPECT_EQ(ch4.id(), root["b"]["seq"][4].id());
    EXPECT_EQ(ch4.get(), root["b"]["seq"][4].get());
    EXPECT_EQ(root["b"]["seq"][4].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");
    root["b"]["seq"].append_sibling({NodeScalar{"!!str", "aaa"}, NodeScalar{"!!int", "0"}});
    EXPECT_EQ(root["b"]["seq"][4].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");

    root["b"]["key"] = "val";
    auto seq = root["b"]["seq"];
    auto seq2 = root["b"]["seq2"];
    EXPECT_TRUE(seq2.is_seed());
    root["b"]["seq2"] = N(SEQ);
    seq2 = root["b"]["seq2"];
    EXPECT_FALSE(seq2.is_seed());
    auto seq20 = root["b"]["seq2"][0];
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
    root["b"]["seq2"][4] << 55; root["b"]["seq2"][4] >> iv;
    int zv = 0;
    root["b"]["seq2"][5] << size_t(55); root["b"]["seq2"][5] >> zv;
    float fv = 0;
    root["b"]["seq2"][6] << 2.0f; root["b"]["seq2"][6] >> fv;
    float dv = 0;
    root["b"]["seq2"][7] << 2.0; root["b"]["seq2"][7] >> dv;

    EXPECT_EQ(root["b"]["key"].key(), "key");
    EXPECT_EQ(root["b"]["key"].val(), "val");


    emit(t);

    EXPECT_EQ(root.type(), MAP);
    EXPECT_EQ(root["a"].type(), KEYVAL);
    EXPECT_EQ(root["a"].key(), "a");
    EXPECT_EQ(root["a"].val(), "0");

    EXPECT_EQ(root["b"].type(), KEYMAP);

    EXPECT_EQ(root["b"]["seq"].type(), KEYSEQ);
    EXPECT_EQ(root["b"]["seq"].key(), "seq");
    EXPECT_EQ(root["b"]["seq"][0].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][0].val(), "0");
    EXPECT_EQ(root["b"]["seq"][1].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][1].val(), "1");
    EXPECT_EQ(root["b"]["seq"][2].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][2].val(), "2");
    EXPECT_EQ(root["b"]["seq"][3].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][3].val(), "3");
    EXPECT_EQ(root["b"]["seq"][3].val_tag(), "!!str");
    EXPECT_EQ(root["b"]["seq"][4].type(), VAL);
    EXPECT_EQ(root["b"]["seq"][4].val(), "4");

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

    emit(t);

    EXPECT_EQ(root["b"]["aaa"].type(), KEYVAL);
    EXPECT_EQ(root["b"]["aaa"].key_tag(), "!!str");
    EXPECT_EQ(root["b"]["aaa"].key(), "aaa");
    EXPECT_EQ(root["b"]["aaa"].val_tag(), "!!int");
    EXPECT_EQ(root["b"]["aaa"].val(), "0");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void noderef_check_tree(NodeRef const& root)
{
    check_invariants(*root.tree());

    EXPECT_EQ(root.tree()->size(), 7);
    EXPECT_EQ(root.num_children(), 6);
    EXPECT_EQ(root.is_container(), true);
    EXPECT_EQ(root.is_seq(), true);

    EXPECT_EQ(root[0].type(), VAL);
    EXPECT_EQ(root[0].val(), "0");
    EXPECT_EQ(root[1].type(), VAL);
    EXPECT_EQ(root[1].val(), "1");
    EXPECT_EQ(root[2].type(), VAL);
    EXPECT_EQ(root[2].val(), "2");
    EXPECT_EQ(root[3].type(), VAL);
    EXPECT_EQ(root[3].val(), "3");
    EXPECT_EQ(root[4].type(), VAL);
    EXPECT_EQ(root[4].val(), "4");
    EXPECT_EQ(root[5].type(), VAL);
    EXPECT_EQ(root[5].val(), "5");
}

TEST(NodeRef, 1_append_child)
{
    Tree t;

    NodeRef root(&t);

    root |= SEQ;
    root.append_child({"0"});
    root.append_child({"1"});
    root.append_child({"2"});
    root.append_child({"3"});
    root.append_child({"4"});
    root.append_child({"5"});

    noderef_check_tree(root);
}

TEST(NodeRef, 2_prepend_child)
{
    Tree t;

    NodeRef root(&t);

    root |= SEQ;
    root.prepend_child({"5"});
    root.prepend_child({"4"});
    root.prepend_child({"3"});
    root.prepend_child({"2"});
    root.prepend_child({"1"});
    root.prepend_child({"0"});

    noderef_check_tree(root);
}

TEST(NodeRef, 3_insert_child)
{
    Tree t;

    NodeRef root(&t);
    NodeRef none(&t, NONE);

    root |= SEQ;
    root.insert_child({"3"}, none);
    root.insert_child({"4"}, root[0]);
    root.insert_child({"0"}, none);
    root.insert_child({"5"}, root[2]);
    root.insert_child({"1"}, root[0]);
    root.insert_child({"2"}, root[1]);

    noderef_check_tree(root);
}

TEST(NodeRef, 4_remove_child)
{
    Tree t;

    NodeRef root(&t);
    NodeRef none(&t, NONE);

    root |= SEQ;
    root.insert_child({"3"}, none);
    root.insert_child({"4"}, root[0]);
    root.insert_child({"0"}, none);
    root.insert_child({"5"}, root[2]);
    root.insert_child({"1"}, root[0]);
    root.insert_child({"2"}, root[1]);

    std::vector< int > vec({10, 20, 30, 40, 50, 60, 70, 80, 90});
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

    std::vector< std::vector<int> > vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
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

TEST(NodeRef, 5_move_in_same_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector< std::vector<int> > vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map< std::string, int > map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r |= SEQ;
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
    //printf("fonix"); print_tree(t); emit(r);
    r[0].move(r[1]);
    //printf("fonix"); print_tree(t); emit(r);
    EXPECT_EQ(r[0], m);
    EXPECT_EQ(r[0].num_children(), map2.size());
    EXPECT_EQ(r[1], s);
    EXPECT_EQ(r[1].num_children(), vec2.size());
}

TEST(NodeRef, 6_move_to_other_parent)
{
    Tree t;
    NodeRef r = t;

    std::vector< std::vector<int> > vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map< std::string, int > map2({{"foo", 100}, {"bar", 200}, {"baz", 300}});

    r |= SEQ;
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    NodeData *elm2 = r[2].get();
    EXPECT_EQ(r[2].val(), "elm2");
    //printf("fonix"); print_tree(t); emit(r);
    r[2].move(r[0], r[0][0]);
    EXPECT_EQ(r[0][1].get(), elm2);
    EXPECT_EQ(r[0][1].val(), "elm2");
    //printf("fonix"); print_tree(t); emit(r);
}

TEST(NodeRef, 7_duplicate)
{
    Tree t;
    NodeRef r = t;

    std::vector< std::vector<int> > vec2({{100, 200}, {300, 400}, {500, 600}, {700, 800, 900}});
    std::map< std::string, int > map2({{"bar", 200}, {"baz", 300}, {"foo", 100}});

    r |= SEQ;
    r.append_child() << vec2;
    r.append_child() << map2;
    r.append_child() << "elm2";
    r.append_child() << "elm3";

    EXPECT_EQ(r[0][0].num_children(), 2u);
    printf("fonix"); print_tree(t); emit(r);
    NodeRef dup = r[1].duplicate(r[0][0], r[0][0][1]);
    printf("fonix"); print_tree(t); emit(r);
    EXPECT_EQ(r[0][0].num_children(), 3u);
    EXPECT_EQ(r[0][0][2].num_children(), map2.size());
    EXPECT_NE(dup.get(), r[1].get());
    EXPECT_EQ(dup[0].key(), "bar");
    EXPECT_EQ(dup[0].val(), "200");
    EXPECT_EQ(dup[1].key(), "baz");
    EXPECT_EQ(dup[1].val(), "300");
    EXPECT_EQ(dup[2].key(), "foo");
    EXPECT_EQ(dup[2].val(), "100");
    EXPECT_EQ(dup[0].key().str, r[1][0].key().str);
    EXPECT_EQ(dup[0].val().str, r[1][0].val().str);
    EXPECT_EQ(dup[0].key().len, r[1][0].key().len);
    EXPECT_EQ(dup[0].val().len, r[1][0].val().len);
    EXPECT_EQ(dup[1].key().str, r[1][1].key().str);
    EXPECT_EQ(dup[1].val().str, r[1][1].val().str);
    EXPECT_EQ(dup[1].key().len, r[1][1].key().len);
    EXPECT_EQ(dup[1].val().len, r[1][1].val().len);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(general, parsing)
{
    char src[] = "{foo: 1}"; // needs to be writable
    auto tree = parse(src);

    char cmpbuf[128] = {0};
    span cmp(cmpbuf);

    cat(cmp, tree["foo"].val());
    EXPECT_EQ(cmp, "1");

    cat(cmp, tree["foo"].key());
    EXPECT_EQ(cmp, "foo");
}

TEST(general, emitting)
{
    std::string cmpbuf;

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

    print_tree(tree);

    // emit to stdout (can also emit to FILE* or ryml::span)
    emit_resize(tree, &cmpbuf);
    const char* exp = R"(foo: 1
seq:
  - bar0
  - bar1
  - bar2
)";
    EXPECT_EQ(cmpbuf, exp);

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

    emit_resize(tree, &cmpbuf);
    exp = R"(foo: 1
seq:
  - bar0
  - bar1
  - bar2
  - 33
  - 44
  - child5
)";
    EXPECT_EQ(cmpbuf, exp);

    // to serialize keys:
    int k=66;
    r.append_child() << key(k) << 7;

    emit_resize(tree, &cmpbuf);
    exp = R"(foo: 1
seq:
  - bar0
  - bar1
  - bar2
  - 33
  - 44
  - child5
66: 7
)";
    EXPECT_EQ(cmpbuf, exp);
}

TEST(general, map_to_root)
{
    std::string cmpbuf; const char *exp;
    std::map< std::string, int > m({{"bar", 2}, {"foo", 1}});
    Tree t;
    t.rootref() << m;

    emit_resize(t, &cmpbuf);
    exp = R"(bar: 2
foo: 1
)";
    EXPECT_EQ(cmpbuf, exp);

    t["foo"] << 10;
    t["bar"] << 20;

    m.clear();
    t.rootref() >> m;

    EXPECT_EQ(m["foo"], 10);
    EXPECT_EQ(m["bar"], 20);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

using N = CaseNode;
using L = CaseNode::iseqmap;

TEST(CaseNode, setting_up)
{
    L tl1 = {DOC, DOC};
    L tl2 = {(DOC), (DOC)};

    ASSERT_EQ(tl1.size(), tl2.size());
    N const& d1 = *tl1.begin();
    N const& d2 = *(tl1.begin() + 1);
    ASSERT_EQ(d1.reccount(), d2.reccount());
    ASSERT_EQ(d1.type, DOC);
    ASSERT_EQ(d2.type, DOC);

    N n1(tl1);
    N n2(tl2);
    ASSERT_EQ(n1.reccount(), n2.reccount());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_libyaml)
{
    try
    {
        LibyamlParser libyaml_parser;
        libyaml_parser.parse(c->src);
    }
    catch(...)
    {
        if(c->flags & IGNORE_LIBYAML_PARSE_FAIL)
        {
            std::cout << "libyaml failed parsing; ignoring\n";
        }
        else
        {
            std::cout << "libyaml failed parsing the following source:\n";
            std::cout << "---------------\n";
            std::cout << c->src;
            std::cout << "---------------\n";
            throw;
        }
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_yaml_cpp)
{
    try
    {
        std::string src(c->src.str, c->src.len);
        YAML::Node node = YAML::Load(src);
    }
    catch(...)
    {
        if(c->flags & IGNORE_YAMLCPP_PARSE_FAIL)
        {
            std::cout << "yamlcpp failed parsing the following source:\n";
        }
        else
        {
            std::cout << "yamlcpp failed parsing the following source:\n";
            std::cout << "---------------\n";
            std::cout << c->src;
            std::cout << "---------------\n";
            throw;
        }
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_ryml)
{
#ifdef RYML_DBG
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
#endif
    parse(d->src, &d->parsed_tree);
    {
        SCOPED_TRACE("checking tree invariants of unresolved parsed tree");
        check_invariants(d->parsed_tree);
    }
#ifdef RYML_DBG
    print_tree(c->root);
    print_tree(d->parsed_tree);
#endif
    {
        SCOPED_TRACE("checking node invariants of unresolved parsed tree");
        check_invariants(d->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        d->parsed_tree.resolve();
#ifdef RYML_DBG
        std::cout << "resolved tree!!!\n";
        print_tree(d->parsed_tree);
#endif
        {
            SCOPED_TRACE("checking tree invariants of resolved parsed tree");
            check_invariants(d->parsed_tree);
        }
        {
            SCOPED_TRACE("checking node invariants of resolved parsed tree");
            check_invariants(d->parsed_tree.rootref());
        }
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->parsed_tree.size(), c->root.reccount());
        c->root.compare(d->parsed_tree.rootref());
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_stdout)
{
    d->numbytes_stdout = emit(d->parsed_tree);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_string)
{
    auto em = emit_resize(d->parsed_tree, &d->emit_buf);
    EXPECT_EQ(em.len, d->emit_buf.size());
    EXPECT_EQ(em.len, d->numbytes_stdout);
    d->emitted_yml = em;

#ifdef RYML_DBG
    std::cout << em;
#endif
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, complete_round_trip)
{
#ifdef RYML_DBG
    print_tree(d->parsed_tree);
    std::cout << d->emitted_yml;
#endif

    {
        SCOPED_TRACE("parsing emitted yml");
        d->parse_buf = d->emit_buf;
        d->parsed_yml.assign(d->parse_buf.data(), d->parse_buf.size());
        parse(d->parsed_yml, &d->emitted_tree);
#ifdef RYML_DBG
        print_tree(d->emitted_tree);
#endif
    }

    {
        SCOPED_TRACE("checking node invariants of parsed tree");
        check_invariants(d->emitted_tree.rootref());
    }

    {
        SCOPED_TRACE("checking tree invariants of parsed tree");
        check_invariants(d->emitted_tree);
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->emitted_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->emitted_tree.size(), c->root.reccount());
        c->root.compare(d->emitted_tree.rootref());
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, recreate_from_ref)
{
    {
        SCOPED_TRACE("recreating a new tree from the ref tree");
        d->recreated.reserve(d->parsed_tree.size());
        NodeRef r = d->recreated.rootref();
        c->root.recreate(&r);
    }

    {
        SCOPED_TRACE("checking node invariants of recreated tree");
        check_invariants(d->recreated.rootref());
    }

    {
        SCOPED_TRACE("checking tree invariants of recreated tree");
        check_invariants(d->recreated);
    }

    {
        SCOPED_TRACE("comparing recreated tree to ref tree");
        c->root.compare(d->recreated.rootref());
    }
}

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
