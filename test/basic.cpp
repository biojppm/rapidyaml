
#include "./test_case.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


namespace std {

using namespace c4::yml;

inline size_t to_str(span buf, std::string const& s)
{
    return to_str(buf, cspan(s.data(), s.size()));
}
inline size_t from_str(cspan buf, std::string * s)
{
    s->resize(buf.len);
    span v(&(*s)[0], buf.len);
    return from_str(buf, &v);
}

void write(NodeRef *n, std::string const& s)
{
    *n << cspan(s.data(), s.size());
}
bool read(NodeRef const& n, std::string *s)
{
    s->resize(n.val().len);
    span sp(&(*s)[0], s->size());
    n >> sp;
    return true;
}

template< class V, class Alloc >
void write(NodeRef *n, std::vector< V, Alloc > const& vec)
{
    *n |= SEQ;
    for(auto const& v : vec)
    {
        auto ch = n->append_child();
        ch << v;
    }
}
template< class V, class Alloc >
bool read(NodeRef const& n, std::vector< V, Alloc > *vec)
{
    vec->resize(n.num_children());
    size_t pos = 0;
    for(auto const& ch : n)
    {
        ch >> (*vec)[pos++];
    }
    return true;
}

template< class K, class V, class Less, class Alloc >
void write(NodeRef *n, std::map< K, V, Less, Alloc > const& m)
{
    *n |= MAP;
    for(auto const& p : m)
    {
        auto ch = n->append_child(KEY);
        ch << key(p.first);
        ch << p.second;
    }
}
template< class K, class V, class Less, class Alloc >
bool read(NodeRef const& n, std::map< K, V, Less, Alloc > * m)
{
    K k{};
    V v;
    for(auto const& ch : n)
    {
        ch >> key(k);
        ch >> v;
        m->emplace(make_pair(move(k), move(v)));
    }
    return true;
}

} // namespace std

namespace foo {

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
    do_test_serialize< M >(L{{10, 0}, {11, 1}, {22, 2}});
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
    using L = std::initializer_list<typename V::value_type>;
    do_test_serialize< V >(L{
            {{"asdasf", 0}, {"dfgkjhdfg", 1}, {"dfgkjhdfg", 2}},
            {{"asdasf", 10}, {"dfgkjhdfg", 11}, {"dfgkjhdfg", 12}},
            {{"asdasf", 20}, {"dfgkjhdfg", 21}, {"dfgkjhdfg", 22}},
    });
}

} // namespace foo

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void PrintTo(const span& bar, ::std::ostream* os) { *os << bar; }
void PrintTo(const cspan& bar, ::std::ostream* os) { *os << bar; }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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

    EXPECT_EQ(cspan("012345").first_of('0'), 0);
    EXPECT_EQ(cspan("012345").first_of("0"), 0);
    EXPECT_EQ(cspan("012345").first_of("01"), 0);
    EXPECT_EQ(cspan("012345").first_of("10"), 0);
    EXPECT_EQ(cspan("012345").first_of("012"), 0);
    EXPECT_EQ(cspan("012345").first_of("210"), 0);
    EXPECT_EQ(cspan("012345").first_of("0123"), 0);
    EXPECT_EQ(cspan("012345").first_of("3210"), 0);
    EXPECT_EQ(cspan("012345").first_of("01234"), 0);
    EXPECT_EQ(cspan("012345").first_of("43210"), 0);
    EXPECT_EQ(cspan("012345").first_of("012345"), 0);
    EXPECT_EQ(cspan("012345").first_of("543210"), 0);

    EXPECT_EQ(cspan("012345").first_of('5'), 5);
    EXPECT_EQ(cspan("012345").first_of("5"), 5);
    EXPECT_EQ(cspan("012345").first_of("45"), 4);
    EXPECT_EQ(cspan("012345").first_of("54"), 4);
    EXPECT_EQ(cspan("012345").first_of("345"), 3);
    EXPECT_EQ(cspan("012345").first_of("543"), 3);
    EXPECT_EQ(cspan("012345").first_of("2345"), 2);
    EXPECT_EQ(cspan("012345").first_of("5432"), 2);
    EXPECT_EQ(cspan("012345").first_of("12345"), 1);
    EXPECT_EQ(cspan("012345").first_of("54321"), 1);
    EXPECT_EQ(cspan("012345").first_of("012345"), 0);
    EXPECT_EQ(cspan("012345").first_of("543210"), 0);
}

TEST(span, last_of)
{
    EXPECT_EQ(cspan("012345").last_of('a'), npos);
    EXPECT_EQ(cspan("012345").last_of("ab"), npos);

    EXPECT_EQ(cspan("012345").last_of('0'), 0);
    EXPECT_EQ(cspan("012345").last_of("0"), 0);
    EXPECT_EQ(cspan("012345").last_of("01"), 1);
    EXPECT_EQ(cspan("012345").last_of("10"), 1);
    EXPECT_EQ(cspan("012345").last_of("012"), 2);
    EXPECT_EQ(cspan("012345").last_of("210"), 2);
    EXPECT_EQ(cspan("012345").last_of("0123"), 3);
    EXPECT_EQ(cspan("012345").last_of("3210"), 3);
    EXPECT_EQ(cspan("012345").last_of("01234"), 4);
    EXPECT_EQ(cspan("012345").last_of("43210"), 4);
    EXPECT_EQ(cspan("012345").last_of("012345"), 5);
    EXPECT_EQ(cspan("012345").last_of("543210"), 5);

    EXPECT_EQ(cspan("012345").last_of('5'), 5);
    EXPECT_EQ(cspan("012345").last_of("5"), 5);
    EXPECT_EQ(cspan("012345").last_of("45"), 5);
    EXPECT_EQ(cspan("012345").last_of("54"), 5);
    EXPECT_EQ(cspan("012345").last_of("345"), 5);
    EXPECT_EQ(cspan("012345").last_of("543"), 5);
    EXPECT_EQ(cspan("012345").last_of("2345"), 5);
    EXPECT_EQ(cspan("012345").last_of("5432"), 5);
    EXPECT_EQ(cspan("012345").last_of("12345"), 5);
    EXPECT_EQ(cspan("012345").last_of("54321"), 5);
    EXPECT_EQ(cspan("012345").last_of("012345"), 5);
    EXPECT_EQ(cspan("012345").last_of("543210"), 5);
}

TEST(span, first_not_of)
{
    EXPECT_EQ(cspan("012345").first_not_of('a'), 0);
    EXPECT_EQ(cspan("012345").first_not_of("ab"), 0);

    EXPECT_EQ(cspan("012345").first_not_of('0'), 1);
    EXPECT_EQ(cspan("012345").first_not_of("0"), 1);
    EXPECT_EQ(cspan("012345").first_not_of("01"), 2);
    EXPECT_EQ(cspan("012345").first_not_of("10"), 2);
    EXPECT_EQ(cspan("012345").first_not_of("012"), 3);
    EXPECT_EQ(cspan("012345").first_not_of("210"), 3);
    EXPECT_EQ(cspan("012345").first_not_of("0123"), 4);
    EXPECT_EQ(cspan("012345").first_not_of("3210"), 4);
    EXPECT_EQ(cspan("012345").first_not_of("01234"), 5);
    EXPECT_EQ(cspan("012345").first_not_of("43210"), 5);
    EXPECT_EQ(cspan("012345").first_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").first_not_of("543210"), npos);

    EXPECT_EQ(cspan("012345").first_not_of('5'), 0);
    EXPECT_EQ(cspan("012345").first_not_of("5"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("45"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("54"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("345"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("543"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("2345"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("5432"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("12345"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("54321"), 0);
    EXPECT_EQ(cspan("012345").first_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").first_not_of("543210"), npos);
}

TEST(span, last_not_of)
{
    EXPECT_EQ(cspan("012345").last_not_of('a'), 5);
    EXPECT_EQ(cspan("012345").last_not_of("ab"), 5);

    EXPECT_EQ(cspan("012345").last_not_of('5'), 4);
    EXPECT_EQ(cspan("012345").last_not_of("5"), 4);
    EXPECT_EQ(cspan("012345").last_not_of("45"), 3);
    EXPECT_EQ(cspan("012345").last_not_of("54"), 3);
    EXPECT_EQ(cspan("012345").last_not_of("345"), 2);
    EXPECT_EQ(cspan("012345").last_not_of("543"), 2);
    EXPECT_EQ(cspan("012345").last_not_of("2345"), 1);
    EXPECT_EQ(cspan("012345").last_not_of("5432"), 1);
    EXPECT_EQ(cspan("012345").last_not_of("12345"), 0);
    EXPECT_EQ(cspan("012345").last_not_of("54321"), 0);
    EXPECT_EQ(cspan("012345").last_not_of("012345"), npos);
    EXPECT_EQ(cspan("012345").last_not_of("543210"), npos);

    EXPECT_EQ(cspan("012345").last_not_of('0'), 5);
    EXPECT_EQ(cspan("012345").last_not_of("0"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("01"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("10"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("012"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("210"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("0123"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("3210"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("01234"), 5);
    EXPECT_EQ(cspan("012345").last_not_of("43210"), 5);
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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void node_scalar_test_empty(NodeScalar const& s)
{
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.tag, "");
    EXPECT_EQ(s.tag.len, 0);
    EXPECT_TRUE(s.tag.empty());
    EXPECT_EQ(s.scalar, "");
    EXPECT_EQ(s.scalar.len, 0);
    EXPECT_TRUE(s.scalar.empty());
}

void node_scalar_test_foo(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str");
        EXPECT_EQ(s.tag.len, 5);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo");
    EXPECT_EQ(s.scalar.len, 3);
    EXPECT_FALSE(s.scalar.empty());
}

void node_scalar_test_foo3(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str+++");
        EXPECT_EQ(s.tag.len, 8);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo3");
    EXPECT_EQ(s.scalar.len, 4);
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
        SCOPED_TRACE(Node::type_str(k));
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

        using ilist = std::initializer_list< NodeInit >;
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
TEST(NodeRef, setting_up)
{
    Tree t;
    t.reserve(32);

    NodeRef root(&t);

    using S = cspan;
    using V = NodeScalar;
    using N = NodeInit;

    root = N{MAP};
    root.append_child({"a", "0"});
    root.append_child({MAP, "b"});
    root["b"].append_child({SEQ, "seq"});
    root["b"]["seq"].append_child({"0"});
    root["b"]["seq"].append_child({"1"});
    root["b"]["seq"].append_child({"2"});
    root["b"]["seq"].append_child({NodeScalar{"!!str", "3"}});
    //BUG!root["b"]["seq"][3].append_sibling({"4"});
    //BUG!root["b"]["seq"].append_sibling({NodeScalar{"!!str", "aaaa"}, NodeScalar{"!!int", "0"}});

    root["b"]["key"] = "val";
    root["b"]["seq2"] = N(SEQ);
    root["b"]["seq2"][0] = "00";
    root["b"]["seq2"][1] = "01";
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
    //BUG!EXPECT_EQ(root["b"]["seq"][4].val(), VAL);
    //BUG!EXPECT_EQ(root["b"]["seq"][4].val(), "4");

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

    //BUG!EXPECT_EQ(root["b"]["aaa"].type(), KEYVAL);
    //BUG!EXPECT_EQ(root["b"]["aaa"].key_tag(), "!!str");
    //BUG!EXPECT_EQ(root["b"]["aaa"].key(), "aaa");
    //BUG!EXPECT_EQ(root["b"]["aaa"].val_tag(), "!!int");
    //BUG!EXPECT_EQ(root["b"]["aaa"].val(), "0");
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
    parse(d->src, &d->parsed_tree);
#ifdef RYML_DBG
    print_tree(c->root);
    print_tree(d->parsed_tree);
#endif
    {
        SCOPED_TRACE("checking invariants of parsed tree");
        check_invariants(*d->parsed_tree.root());
    }
    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->parsed_tree.size(), c->root.reccount());
        c->root.compare(*d->parsed_tree.root());
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
        SCOPED_TRACE("checking invariants of parsed tree");
        check_invariants(*d->emitted_tree.root());
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->emitted_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->emitted_tree.size(), c->root.reccount());
        c->root.compare(*d->emitted_tree.root());
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, recreate_from_ref)
{
    {
        SCOPED_TRACE("recreating a new tree from the ref tree");
        d->recreated.reserve(d->parsed_tree.size());
        c->root.recreate(d->recreated.root());
    }

    {
        SCOPED_TRACE("comparing recreated tree to ref tree");
        c->root.compare(*d->recreated.root());
    }
}

} // namespace yml
} // namespace c4
