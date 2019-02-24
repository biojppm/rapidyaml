#include "./test_case.hpp"
#include "c4/span.hpp"
#include "c4/yml/std/std.hpp"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4127/*conditional expression is constant*/)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#endif

#include <yaml-cpp/yaml.h>

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

namespace c4 {
namespace yml {

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CaseNode::compare_child(yml::NodeRef const& n, size_t pos) const
{
    EXPECT_TRUE(pos < n.num_children());
    EXPECT_TRUE(pos < children.size());

    if(pos >= n.num_children() || pos >= children.size()) return;

    ASSERT_GT(n.num_children(), pos);
    auto const& ch = children[pos];

    if(type & MAP)
    {
        EXPECT_NE(n.find_child(ch.key), nullptr);
        auto fch = n.find_child(ch.key);
        if(fch != nullptr)
        {
            EXPECT_EQ(fch, n[ch.key]);
            EXPECT_EQ(fch, n[pos]);
            EXPECT_EQ(n[pos], n[ch.key]);
            EXPECT_EQ(n[ch.key].key(), ch.key);
        }
        else
        {
            printf("error: node should have child %.*s: ", (int)ch.key.len, ch.key.str);
            print_path(n);
            printf("\n");
            print_node(n);
        }
    }

    if(type & SEQ)
    {
        EXPECT_FALSE(n[pos].has_key());
        EXPECT_EQ(n[pos].get()->m_key.scalar, children[pos].key);
        auto fch = n.child(pos);
        EXPECT_EQ(fch, n[pos]);
    }

    if(ch.type & KEY)
    {
        auto fch = n[pos];
        EXPECT_TRUE(fch.has_key());
        EXPECT_EQ(fch.key(), ch.key);

        if( ! ch.key_tag.empty())
        {
            EXPECT_TRUE(fch.has_key_tag());
            EXPECT_EQ(fch.key_tag(), ch.key_tag);
        }
    }

    if(ch.type & VAL)
    {
        auto fch = n[pos];
        EXPECT_TRUE(fch.has_val());
        EXPECT_EQ(fch.val(), ch.val);

        if( ! ch.val_tag.empty())
        {
            EXPECT_TRUE(fch.has_val_tag());
            EXPECT_EQ(fch.val_tag(), ch.val_tag);
        }
    }
}

void CaseNode::compare(yml::NodeRef const& n) const
{
    EXPECT_EQ(n.get()->m_type, type); // the type() method masks the type, and thus tag flags are omitted on its return value
    EXPECT_EQ(n.num_children(), children.size());

    if(n.has_key())
    {
        EXPECT_EQ(n.key(), key);
    }

    if(n.has_val())
    {
        EXPECT_EQ(n.val(), val);
    }

    // check that the children are in the same order
    {
        EXPECT_EQ(children.size(), n.num_children());

        size_t ic = 0;
        for(auto const& ch : children)
        {
            SCOPED_TRACE("comparing: iteration based on the ref children");
            (void)ch; // unused
            compare_child(n, ic++);
        }

        ic = 0;
        for(auto const& ch : n.children())
        {
            SCOPED_TRACE("comparing: iteration based on the yml::Node children");
            (void)ch; // unused
            compare_child(n, ic++);
        }

        if(n.first_child() != nullptr)
        {
            ic = 0;
            for(auto const& ch : n.first_child().siblings())
            {
                SCOPED_TRACE("comparing: iteration based on the yml::Node siblings");
                (void)ch; // unused
                compare_child(n, ic++);
            }
        }
    }

    for(size_t i = 0, ei = n.num_children(), j = 0, ej = children.size(); i < ei && j < ej; ++i, ++j)
    {
        children[j].compare(n[i]);
    }
}

void CaseNode::recreate(yml::NodeRef *n) const
{
    C4_ASSERT( ! n->has_children());
    auto *nd = n->get();
    nd->m_type = type;
    nd->m_key.scalar = key;
    nd->m_key.tag = (key_tag);
    nd->m_val.scalar = val;
    nd->m_val.tag = (val_tag);
    auto &tree = *n->tree();
    size_t nid = n->id(); // don't use node from now on
    for(auto const& ch : children)
    {
        size_t id = tree.append_child(nid);
        NodeRef chn(n->tree(), id);
        ch.recreate(&chn);
    }
}

//-----------------------------------------------------------------------------

void print_path(NodeRef const& n)
{
    size_t len = 0;
    char buf[1024];
    NodeRef p = n;
    while(p != nullptr)
    {
        if(p.has_key())
        {
            len += 1 + p.key().len;
        }
        else
        {
            len += snprintf(buf, sizeof(buf), "/%zd", p.has_parent() ? p.parent().child_pos(p) : 0);
        }
        p = p.parent();
    };
    C4_ASSERT(len < sizeof(buf));
    size_t pos = len;
    p = n;
    while(p != nullptr)
    {
        if(p.has_key())
        {
            size_t tl = p.key().len;
            pos -= snprintf(buf + pos - tl, tl, "%.*s", (int)tl, p.key().str);
        }
        else
        {
            pos = p.parent().child_pos(p);
            size_t tl = snprintf(buf, 0, "/%zd", pos);
            pos -= snprintf(buf + pos - tl, tl, "/%zd", pos);
        }
        p = p.parent();
    };
    printf("%.*s", (int)len, buf);
}


void print_node(NodeRef const& p, int level, bool print_children)
{
    printf("%*s[%zd] %p", (2*level), "", p.id(), (void*)p.get());
    if(p.is_root())
    {
        printf(" [ROOT]");
    }
    printf(" %s:", p.type_str());
    if(p.has_key())
    {
        if(p.has_key_tag())
        {
            csubstr const& kt = p.key_tag();
            csubstr const& k  = p.key();
            printf(" '%.*s %.*s'", (int)kt.len, kt.str, (int)k.len, k.str);
        }
        else
        {
            csubstr const& k  = p.key();
            printf(" '%.*s'", (int)k.len, k.str);
        }
    }
    else
    {
        C4_ASSERT( ! p.has_key_tag());
    }
    if(p.has_val())
    {
        if(p.has_val_tag())
        {
            csubstr const& vt = p.val_tag();
            csubstr const& v  = p.val();
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
        else
        {
            csubstr const& v  = p.val();
            printf(" '%.*s'", (int)v.len, v.str);
        }
    }
    else
    {
        if(p.has_val_tag())
        {
            csubstr const& vt = p.val_tag();
            printf(" %.*s", (int)vt.len, vt.str);
        }
    }
    if(p.has_anchor())
    {
        auto &a = p.anchor();
        printf(" anchor='&%.*s'", (int)a.len, a.str);
    }
    printf(" (%zd sibs)", p.num_siblings());
    if(p.is_container())
    {
        printf(" %zd children:", p.num_children());
        if(print_children)
        {
            for(NodeRef const& ch : p.children())
            {
                print_node(ch, level+1);
            }
        }
    }
    printf("\n");
}



void print_node(CaseNode const& p, int level)
{
    printf("%*s%p", (2*level), "", (void*)&p);
    if( ! p.parent)
    {
        printf(" [ROOT]");
    }
    printf(" %s:", NodeType::type_str(p.type));
    if(p.has_key())
    {
        if(p.key_tag.empty())
        {
            csubstr const& v  = p.key;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            csubstr const& vt = p.key_tag;
            csubstr const& v  = p.key;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    if(p.has_val())
    {
        if(p.val_tag.empty())
        {
            csubstr const& v  = p.val;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            csubstr const& vt = p.val_tag;
            csubstr const& v  = p.val;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    else
    {
        if( ! p.val_tag.empty())
        {
            csubstr const& vt = p.val_tag;
            printf(" %.*s", (int)vt.len, vt.str);
        }
    }
    if(p.has_anchor())
    {
        auto &a = p.ancref.val;
        printf(" anchor='&%.*s'", (int)a.len, a.str);
    }
    printf(" (%zd sibs)", p.parent ? p.parent->children.size() : 0);
    if(p.is_container())
    {
        printf(" %zd children:", p.children.size());
    }
    printf("\n");
}


void print_tree(NodeRef const& p, int level)
{
    print_node(p, level);
    for(NodeRef const& ch : p.children())
    {
        print_tree(ch, level+1);
    }
}

void print_tree(CaseNode const& p, int level)
{
    print_node(p, level);
    for(auto const& ch : p.children)
    {
        print_tree(ch, level+1);
    }
}

void print_tree(Tree const& t)
{
    printf("--------------------------------------\n");
    print_tree(t.rootref());
    printf("#nodes: %zd\n", t.size());
    printf("--------------------------------------\n");
}

void print_tree(CaseNode const& t)
{
    printf("--------------------------------------\n");
    print_tree(t, 0);
    printf("#nodes: %zd\n", t.reccount());
    printf("--------------------------------------\n");
}

void check_invariants(NodeRef const& n)
{
    if(n.is_root())
    {
        EXPECT_FALSE(n.has_other_siblings());
    }
    // keys or vals cannot be root
    if(n.has_key() || n.is_val() || n.is_keyval())
    {
        EXPECT_FALSE(n.is_root());
        EXPECT_FALSE(n.is_root());
        EXPECT_FALSE(n.is_root());
    }
    // vals cannot be containers
    if( ! n.empty())
    {
        EXPECT_NE(n.has_val(), n.is_container());
    }
    if(n.has_children())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_val());
    }
    // check parent & sibling reciprocity
    for(NodeRef const& s : n.siblings())
    {
        EXPECT_TRUE(n.has_sibling(s));
        EXPECT_TRUE(s.has_sibling(n));
        EXPECT_EQ(s.parent(), n.parent());
    }
    if(n.parent() != nullptr)
    {
        EXPECT_TRUE(n.parent().has_child(n));
        EXPECT_EQ(n.parent().num_children(), n.num_siblings());
        // doc parent must be a seq and a stream
        if(n.is_doc())
        {
            EXPECT_TRUE(n.parent().is_seq());
            EXPECT_TRUE(n.parent().is_stream());
        }
    }
    else
    {
        EXPECT_TRUE(n.is_root());
    }
    if(n.is_seq())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_map());
        for(NodeRef const& ch : n.children())
        {
            EXPECT_FALSE(ch.is_keyval());
            EXPECT_FALSE(ch.has_key());
        }
    }
    if(n.is_map())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_seq());
        for(NodeRef const& ch : n.children())
        {
            EXPECT_TRUE(ch.has_key());
        }
    }
    // ... add more tests here

    // now recurse into the children
    for(NodeRef const& ch : n.children())
    {
        check_invariants(ch);
    }
}

size_t check_tree_invariants(NodeRef const& n)
{
    auto parent = n.parent();

    if(n.get()->m_prev_sibling == NONE)
    {
        if(parent != nullptr)
        {
            EXPECT_EQ(parent.first_child(), n);
            EXPECT_EQ(parent.first_child().id(), n.id());
        }
    }

    if(n.get()->m_next_sibling == NONE)
    {
        if(parent != nullptr)
        {
            EXPECT_EQ(parent.last_child(), n);
            EXPECT_EQ(parent.last_child().id(), n.id());
        }
    }

    if(parent == nullptr)
    {
        EXPECT_TRUE(n.is_root());
        EXPECT_EQ(n.prev_sibling(), nullptr);
        EXPECT_EQ(n.next_sibling(), nullptr);
    }

    size_t count = 1, num = 0;
    for(NodeRef const& ch : n.children())
    {
        EXPECT_NE(ch, n);
        count += check_tree_invariants(ch);
        ++num;
    }

    EXPECT_EQ(num, n.num_children());

    return count;
}

void check_invariants(Tree const& t)
{

    EXPECT_LE(t.size(), t.capacity());
    EXPECT_EQ(t.size() + t.slack(), t.capacity());

    size_t count = check_tree_invariants(t.rootref());
    EXPECT_EQ(count, t.size());

    return;
#if 0 == 1
    for(size_t i = 0; i < t.m_size; ++i)
    {
        auto n = t.get(i);
        if(n->m_prev_sibling == NONE)
        {
            EXPECT_TRUE(i == t.m_head || i == t.m_free_head);
        }
        if(n->m_next_sibling == NONE)
        {
            EXPECT_TRUE(i == t.m_tail || i == t.m_free_tail);
        }
    }

    std::vector<bool> touched(t.capacity());

    for(size_t i = t.m_head; i != NONE; i = t.get(i)->m_next_sibling)
    {
        touched[i] = true;
    }

    size_t size = 0;
    for(auto v : touched)
    {
        size += v;
    }

    EXPECT_EQ(size, t.size());

    touched.clear();
    touched.resize(t.capacity());

    for(size_t i = t.m_free_head; i != NONE; i = t.get(i)->m_next_sibling)
    {
        touched[i] = true;
    }

    size_t slack = 0;
    for(auto v : touched)
    {
        slack += v;
    }

    EXPECT_EQ(slack, t.slack());
    EXPECT_EQ(size+slack, t.capacity());

    // there are more checks to be done
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef CRL

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef JAVAI
int do_test()
{
    using namespace c4::yml;

    using C = Case;
    using N = CaseNode;
    using L = CaseNode::iseqmap;



    CaseContainer tests({
//-----------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/YAML

//-----------------------------------------------------------------------------
C("literal block scalar as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       \"Please don't spit on the floor\"
   So he carefully spat on the ceiling
)",
     N{"data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n"}
),

//-----------------------------------------------------------------------------
C("folded block scalar as map entry",
R"(
data: >
   Wrapped text
   will be folded
   into a single
   paragraph

   Blank lines denote
   paragraph breaks
)",
  N{"data", "Wrapped text will be folded into a single paragraph\nBlank lines denote paragraph breaks\n"}
),

//-----------------------------------------------------------------------------
C("two scalars in a block, html example",
R"(
---
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style=\"font: italic 12pt Times\">
        <p>\"Three is always greater than two,
           even for large values of two\"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     N{DOC, L{
          N{"example", "HTML goes into YAML without modification"},
          N{"message", R"(<blockquote style=\"font: italic 12pt Times\">
<p>\"Three is always greater than two,
   even for large values of two\"</p>
<p>--Author Unknown</p>
</blockquote>
)"},
          N{"date","2007-06-01"},
              }}
),



//-----------------------------------------------------------------------------
C("scalar block, literal, no chomp, no indentation",
R"(example: |
  Several lines of text,
  with some \"quotes\" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     L{
      N{"example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"},
      N{"another", "text"},
          }
),

//-----------------------------------------------------------------------------
C("scalar block, folded, no chomp, no indentation",
R"(example: >
  Several lines of text,
  with some \"quotes\" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     L{
      N{"example", "Several lines of text,  with some \"quotes\" of various 'types',  and also a blank line:\nplus another line at the end.\n"},
      N{"another", "text"},
          }
),
    }); // end examples

    return tests.run();
}
#endif


#ifdef MORE_EXAMPLES
char const* const structure_examples[] = {
// https://en.wikipedia.org/wiki/YAML
R"(
---
a: 123                     # an integer
b: "123"                   # a string, disambiguated by quotes
c: 123.0                   # a float
d: !!float 123             # also a float via explicit data type prefixed by (!!)
e: !!str 123               # a string, disambiguated by explicit type
f: !!str Yes               # a string via explicit type
g: Yes                     # a boolean True (yaml1.1), string \"Yes\" (yaml1.2)
h: Yes we have No bananas  # a string, \"Yes\" and \"No\" disambiguated by context.
)",
R"(
---
picture: !!binary |
 R0lGODdhDQAIAIAAAAAAANn
 Z2SwAAAAADQAIAAACF4SDGQ
 ar3xxbJ9p0qa7R0YxwzaFME
 1IAADs=
)",
R"(
---
myObject:  !myClass { name: Joe, age: 15 }
)",
R"(

)",
R"(
)",
R"(
)",
R"(
)",
R"(
)",
};
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//#pragma GCC diagnostic ignored "-Wpragma-system-header-outside-header"

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

INSTANTIATE_TEST_SUITE_P(docs_empty    , YmlTestCase, ::testing::Values(EMPTY_DOC_CASES));
INSTANTIATE_TEST_SUITE_P(docs_simple   , YmlTestCase, ::testing::Values(SIMPLE_DOC_CASES));

INSTANTIATE_TEST_SUITE_P(maps_empty    , YmlTestCase, ::testing::Values(EMPTY_MAP_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_empty    , YmlTestCase, ::testing::Values(EMPTY_SEQ_CASES));

INSTANTIATE_TEST_SUITE_P(maps_simple   , YmlTestCase, ::testing::Values(SIMPLE_MAP_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_simple   , YmlTestCase, ::testing::Values(SIMPLE_SEQ_CASES));

INSTANTIATE_TEST_SUITE_P(double_quotes , YmlTestCase, ::testing::Values(DOUBLE_QUOTED_CASES));
INSTANTIATE_TEST_SUITE_P(single_quotes , YmlTestCase, ::testing::Values(SINGLE_QUOTED_CASES));
INSTANTIATE_TEST_SUITE_P(plain_scalars , YmlTestCase, ::testing::Values(PLAIN_SCALAR_CASES));

INSTANTIATE_TEST_SUITE_P(literal_blocks, YmlTestCase, ::testing::Values(BLOCK_LITERAL_CASES));
INSTANTIATE_TEST_SUITE_P(folded_blocks , YmlTestCase, ::testing::Values(BLOCK_FOLDED_CASES));

INSTANTIATE_TEST_SUITE_P(tag_properties, YmlTestCase, ::testing::Values(TAG_PROPERTY_CASES));

INSTANTIATE_TEST_SUITE_P(complex_keys  , YmlTestCase, ::testing::Values(COMPLEX_KEY_CASES));

INSTANTIATE_TEST_SUITE_P(maps_nested2  , YmlTestCase, ::testing::Values(NESTED_MAPX2_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_nested2  , YmlTestCase, ::testing::Values(NESTED_SEQX2_CASES));

INSTANTIATE_TEST_SUITE_P(maps_nested3  , YmlTestCase, ::testing::Values(NESTED_MAPX3_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_nested3  , YmlTestCase, ::testing::Values(NESTED_SEQX3_CASES));

INSTANTIATE_TEST_SUITE_P(maps_nested4  , YmlTestCase, ::testing::Values(NESTED_MAPX4_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_nested4  , YmlTestCase, ::testing::Values(NESTED_SEQX4_CASES));

INSTANTIATE_TEST_SUITE_P(map_of_seqs   , YmlTestCase, ::testing::Values(MAP_OF_SEQ_CASES));
INSTANTIATE_TEST_SUITE_P(seq_of_maps   , YmlTestCase, ::testing::Values(SEQ_OF_MAP_CASES));

INSTANTIATE_TEST_SUITE_P(maps_generic  , YmlTestCase, ::testing::Values(GENERIC_MAP_CASES));
INSTANTIATE_TEST_SUITE_P(seqs_generic  , YmlTestCase, ::testing::Values(GENERIC_SEQ_CASES));

INSTANTIATE_TEST_SUITE_P(simple_anchors, YmlTestCase, ::testing::Values(SIMPLE_ANCHOR_CASES));

INSTANTIATE_TEST_SUITE_P(indentation   , YmlTestCase, ::testing::Values(INDENTATION_CASES));

INSTANTIATE_TEST_SUITE_P(numbers       , YmlTestCase, ::testing::Values(NUMBER_CASES));
INSTANTIATE_TEST_SUITE_P(null_vals     , YmlTestCase, ::testing::Values(NULL_VAL_CASES));
#ifdef CRL
INSTANTIATE_TEST_SUITE_P(github_issues , YmlTestCase, ::testing::Values(GITHUB_ISSUE_CASES));
#endif

#pragma GCC diagnostic pop

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif // CRL

CaseData* get_data(csubstr name)
{
    static std::map< csubstr, CaseData > m;

    auto it = m.find(name);
    CaseData *cd;
    if(it == m.end())
    {
        cd = &m[name];
        Case const* c = get_case(name);
        cd->src_buf.assign(c->src.begin(), c->src.end());
        cd->src.assign(cd->src_buf.data(), cd->src_buf.size());
    }
    else
    {
        cd = &it->second;
    }
    return cd;
}

} // namespace yml
} // namespace c4
