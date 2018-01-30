
#include "./test_case.hpp"

#include <gtest/gtest.h>


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

    EXPECT_EQ(tl1.size(), tl2.size());
    N const& d1 = *tl1.begin();
    N const& d2 = *(tl1.begin() + 1);
    EXPECT_EQ(d1.reccount(), d2.reccount());
    EXPECT_EQ(d1.type, DOC);
    EXPECT_EQ(d2.type, DOC);

    N n1(tl1);
    N n2(tl2);
    EXPECT_EQ(n1.reccount(), n2.reccount());
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_libyaml_to_test_yml_correctness)
{
#ifdef RYML_DBG
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
#endif
    d->libyaml_parser.parse(c->src);
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
        EXPECT_GE(d->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->parsed_tree.size(), c->root.reccount());
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
