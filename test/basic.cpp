
#include "./test_case.hpp"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


namespace c4 {
namespace yml {

using N = CaseNode;
using L = CaseNode::iseqmap;

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
    EXPECT_TRUE (cspan(": ").begins_with(":"));
    EXPECT_TRUE (cspan(": ").begins_with(':'));
    EXPECT_FALSE(cspan(":").begins_with(": "));

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
#ifdef RYML_DBG
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
#endif
    LibyamlParser libyaml_parser;
    libyaml_parser.parse(c->src);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_yaml_cpp)
{
#ifdef RYML_DBG
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
#endif
    std::string src(c->src.str, c->src.len);
    YAML::Node node = YAML::Load(src);
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
