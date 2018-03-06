#include "./test_case.hpp"

#include <gtest/gtest.h>

namespace c4 {
namespace yml {

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
            cspan const& kt = p.key_tag();
            cspan const& k  = p.key();
            printf(" '%.*s %.*s'", (int)kt.len, kt.str, (int)k.len, k.str);
        }
        else
        {
            cspan const& k  = p.key();
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
            cspan const& vt = p.val_tag();
            cspan const& v  = p.val();
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
        else
        {
            cspan const& v  = p.val();
            printf(" '%.*s'", (int)v.len, v.str);
        }
    }
    else
    {
        if(p.has_val_tag())
        {
            cspan const& vt = p.val_tag();
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
            cspan const& v  = p.key;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            cspan const& vt = p.key_tag;
            cspan const& v  = p.key;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    if(p.has_val())
    {
        if(p.val_tag.empty())
        {
            cspan const& v  = p.val;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            cspan const& vt = p.val_tag;
            cspan const& v  = p.val;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    else
    {
        if( ! p.val_tag.empty())
        {
            cspan const& vt = p.val_tag;
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
CaseData* get_data(cspan name)
{
    static std::map< cspan, CaseData > m;

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Case const* get_case(cspan name)
{

using N = CaseNode;
using L = CaseNode::iseqmap;
using TS = TaggedScalar;
using TL = CaseNode::TaggedList;
using AR = AnchorRef;

#define C(name, ...)                                    \
    std::pair< const cspan, Case >                      \
    (                                                   \
        std::piecewise_construct_t{},                   \
        std::forward_as_tuple(name),                    \
        std::forward_as_tuple(name, __VA_ARGS__)        \
    )

    static std::map< cspan, Case > cases({

//-----------------------------------------------------------------------------
#define EMPTY_FILE_CASES "empty0-nochars", "empty0-multiline"

C("empty0-nochars",
"",
NOTYPE
),

C("empty0-multiline", R"(



)", NOTYPE
),

//-----------------------------------------------------------------------------
#define EMPTY_DOC_CASES                                 \
    "one empty doc",                                    \
        "one empty doc, explicit termination",          \
        "two empty docs"                                \

C("one empty doc",
R"(---
)",
    N(STREAM, L{DOC})
),

C("one empty doc, explicit termination",
R"(---
...
)",
    N(STREAM, L{DOC})
),

C("two empty docs",
R"(---
---
)",
    N(STREAM, L{DOC, DOC})
),

//-----------------------------------------------------------------------------
#define SIMPLE_DOC_CASES                                        \
    "single scalar, implicit doc",                              \
        "multi scalar, implicit doc",                           \
        "single scalar, explicit doc, implicit termination",    \
        "single scalar, explicit doc, explicit termination"

C("single scalar, implicit doc",
R"(a scalar with some spaces inside
)",
    N(L{N("a scalar with some spaces inside")})
),

C("multi scalar, implicit doc",
R"(a scalar with some spaces inside,
and yet another one with more spaces inside,
and it doesn't really stop
)",
    N(L{
     N("a scalar with some spaces inside"),
     N("and yet another one with more spaces inside"),
     N("and it doesn't really stop"),
   })
),

C("single scalar, explicit doc, implicit termination",
R"(---
a scalar with some spaces inside
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),

C("single scalar, explicit doc, explicit termination",
R"(---
a scalar with some spaces inside
...
)",
    N(STREAM, L{N(DOCSEQ, L{N("a scalar with some spaces inside")})})
),


//-----------------------------------------------------------------------------
#define EMPTY_MAP_CASES                         \
    "empty map, explicit",                      \
        "empty map, multiline",                 \
        "empty map, multilines"

C("empty map, explicit",
"{}",
    MAP
),


C("empty map, multiline",
R"({

}
)",
    MAP
),

C("empty map, multilines",
R"({
# ksjdfkjhsdfkjhsdfkjh


}
)",
    MAP
),

//-----------------------------------------------------------------------------
#define EMPTY_SEQ_CASES                         \
    "empty seq, explicit",                      \
        "empty seq, multiline",                 \
        "empty seq, multilines"

C("empty seq, explicit",
"[]",
    SEQ
),


C("empty seq, multiline",
R"([
]
)",
    SEQ
),

C("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SEQ
),

//-----------------------------------------------------------------------------

#define SIMPLE_MAP_CASES                                \
    "empty map",                                        \
        "empty map, multiline",                         \
        "empty map, multilines",                        \
        "simple map, explicit, single line",            \
        "simple map, explicit, multiline, unindented",  \
        "simple map, explicit, multiline, indented",    \
        "simple map",                                   \
        "simple map, values on next line",              \
        "simple map, with comments",                    \
        "simple map, with comments interspersed",       \
        "simple map, with indented comments interspersed, before",\
        "simple map, with indented comments interspersed, after"

C("empty map",
"{}",
    MAP
),

C("empty map, multiline",
R"({
}
)",
    MAP
),

C("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MAP
),

C("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, values on next line",
R"(
foo:
  0
bar:
  1
baz:
  2
bat:
  3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a baz
bat: 3   # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments interspersed",
R"(
# this is a foo
foo: 0
# this is a bar
bar: 1
# this is a baz
baz: 2
# this is a bat
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with indented comments interspersed, before",
R"(
  # this is a foo
foo: 0
  # this is a bar
bar: 1
  # this is a baz
baz: 2
  # this is a bat
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with indented comments interspersed, after",
R"(
foo: 0
  # this is a foo
bar: 1
  # this is a bar
baz: 2
  # this is a baz
bat: 3
  # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),



//-----------------------------------------------------------------------------
#define SIMPLE_SEQ_CASES                                \
"simple seq",                                           \
"simple seq, explicit, single line",                    \
"simple seq, explicit, multiline, unindented",          \
"simple seq, explicit, multiline, comments inline",     \
"simple seq, explicit, multiline, comments prev line",  \
"simple seq, explicit, multiline, indented",            \
"simple seq, comments inline",                          \
"simple seq, comments prev line"


C("simple seq",
R"(- 0
- 1
- 2
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),


C("simple seq, explicit, single line",
"[0, 1, 2, 3]",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, unindented",
R"([
0,
1,
2,
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments prev line",
R"([
# bla0
0,
# bla1
1,
# bla2
2,
# bla3
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments inline",
R"(
- 0   # this is a foo
- 1   # this is a bar
- 2   # this is a bar
- 3   # this is a bar
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments prev line",
R"(
# this is a foo
- 0
# this is a bar
- 1
# this is a baz
- 2
# this is a bat
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),


//-----------------------------------------------------------------------------
#define SINGLE_QUOTED_CASES                                 \
            "squoted, only text",                           \
                "squoted, with double quotes",              \
                "squoted, with single quotes",              \
                "squoted, with single and double quotes",   \
                "squoted, with escapes",                    \
                "squoted, all",                             \
                "squoted, empty",                           \
                "squoted, 1 squote",                        \
                "squoted, 2 squotes",                       \
                "squoted, 3 squotes",                       \
                "squoted, 4 squotes",                       \
                "squoted, 5 squotes"

C("squoted, only text",
R"('Some text without any quotes.'
)",
  L{N("Some text without any quotes.")}
),

C("squoted, with double quotes",
R"('Some "text" "with double quotes"')",
  L{N("Some \"text\" \"with double quotes\"")}
),

C("squoted, with single quotes",
R"('Some text ''with single quotes''')",
  L{N("Some text 'with single quotes'")}
),

C("squoted, with single and double quotes",
R"('Some text ''with single quotes'' "and double quotes".')",
  L{N("Some text 'with single quotes' \"and double quotes\".")}
),

C("squoted, with escapes",
R"('Some text with escapes \n \r \t')",
  L{N("Some text with escapes \\n \\r \\t")}
),

C("squoted, all",
R"('Several lines of text,
containing ''single quotes'' and "double quotes". Escapes (like \n) don''t do anything.

Newlines can be added by leaving a blank line.
            Leading whitespace on lines is ignored.'
)",
  L{N("Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Leading whitespace on lines is ignored.")}
),

C("squoted, empty",
R"('')",
  L{N("")}
),

C("squoted, 1 squote",
R"('''')",
  L{N("'")}
),

C("squoted, 2 squotes",
R"('''''')",
  L{N("''")}
),

C("squoted, 3 squotes",
R"('''''''')",
  L{N("'''")}
),

C("squoted, 4 squotes",
R"('''''''''')",
  L{N("''''")}
),

C("squoted, 5 squotes",
R"('''''''''''')",
  L{N("'''''")}
),

/*
C("squoted, example 2",
R"('This is a key

that has multiple lines

': and this is its value
)",
  L{N("This is a key\nthat has multiple lines\n", "and this is its value")}
),
*/

//-----------------------------------------------------------------------------
#define DOUBLE_QUOTED_CASES                                 \
            "dquoted, only text",                           \
                "dquoted, with single quotes",              \
                "dquoted, with double quotes",              \
                "dquoted, with single and double quotes",   \
                "dquoted, with escapes",                    \
                "dquoted, with newline",                    \
                "dquoted, all",                             \
                "dquoted, empty",                           \
                "dquoted, 1 dquote",                        \
                "dquoted, 2 dquotes",                       \
                "dquoted, 3 dquotes",                       \
                "dquoted, 4 dquotes",                       \
                "dquoted, example 2",                       \
                "dquoted, example 2.1"

C("dquoted, only text",
R"("Some text without any quotes."
)",
  L{N("Some text without any quotes.")}
),

C("dquoted, with single quotes",
R"("Some text 'with single quotes'")",
  L{N("Some text 'with single quotes'")}
),

C("dquoted, with double quotes",
R"("Some \"text\" \"with double quotes\"")",
  L{N("Some \"text\" \"with double quotes\"")}
),

C("dquoted, with single and double quotes",
R"("Some text 'with single quotes' \"and double quotes\".")",
  L{N("Some text 'with single quotes' \"and double quotes\".")}
),

C("dquoted, with escapes",
R"("Some text with escapes \\n \\r \\t")",
  L{N("Some text with escapes \\n \\r \\t")}
),

C("dquoted, with newline",
R"("Some text with\nnewline")",
  L{N("Some text with\nnewline")}
),

C("dquoted, all",
R"("Several lines of text,
containing 'single quotes' and \"double quotes\". \
Escapes (like \\n) work.\nIn addition,
newlines can be esc\
aped to prevent them from being converted to a space.

Newlines can also be added by leaving a blank line.
    Leading whitespace on lines is ignored."
)",
  L{N("Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) work.\nIn addition, newlines can be escaped to prevent them from being converted to a space.\nNewlines can also be added by leaving a blank line. Leading whitespace on lines is ignored.")}
),

C("dquoted, empty",
R"("")",
  L{N("")}
),

C("dquoted, 1 dquote",
R"("\"")",
  L{N("\"")}
),

C("dquoted, 2 dquotes",
R"("\"\"")",
  L{N("\"\"")}
),

C("dquoted, 3 dquotes",
R"("\"\"\"")",
  L{N("\"\"\"")}
),

C("dquoted, 4 dquotes",
R"("\"\"\"\"")",
  L{N("\"\"\"\"")}
),

C("dquoted, 5 dquotes",
R"("\"\"\"\"\"")",
  L{N("\"\"\"\"\"")}
),

C("dquoted, example 2",
R"("This is a key\nthat has multiple lines\n": and this is its value
)",
  L{N("This is a key\nthat has multiple lines\n", "and this is its value")}
),

C("dquoted, example 2.1", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"("This is a key

that has multiple lines

": and this is its value
)",
  L{N("This is a key\nthat has multiple lines\n", "and this is its value")}
),


//-----------------------------------------------------------------------------
#define PLAIN_SCALAR_CASES                                              \
    "plain scalar, 1 word only",                                        \
        "plain scalar, 1 line with spaces",                             \
        "plain scalar, multiline",                                      \
        "plain scalar, multiline, quotes, escapes",                     \
        "plain scalar, multiline, quotes, escapes, blank lines middle", \
        "plain scalar, multiline, quotes, escapes, blank lines first",  \
        "plain scalar, multiline, quotes, escapes, blank lines last",   \
        "plain scalar, example",                                        \
        "plain scalar, map example 1"/*,                                  \
        "plain scalar, map example 2"*/,                                  \
        "plain scalar, seq example 1"/*,                                  \
        "plain scalar, seq example 2"*/


C("plain scalar, 1 word only",
R"(a_single_word_scalar_to_test)",
  L{N("a_single_word_scalar_to_test")}
),

C("plain scalar, 1 line with spaces",
R"(a scalar with spaces in it all in one line)",
  L{N("a scalar with spaces in it all in one line")}
),

C("plain scalar, multiline",
R"(
a scalar with several lines in it
  of course also with spaces but for now there are no quotes
  and also no blank lines to speak of)",
  L{N("a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")}
),

C("plain scalar, multiline, quotes, escapes",
R"(
a scalar with several lines in it and also 'single quotes'
  and "double quotes" and assorted escapes such as \r or \n)",
  L{N("a scalar with several lines in it and also 'single quotes' and \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines middle",
R"(
A scalar with several lines in it and also 'single quotes'.
  A blank line follows after this one.
  
  And "double quotes" and assorted escapes such as \r or \n)",
  L{N("A scalar with several lines in it and also 'single quotes'. A blank line follows after this one.\nAnd \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines first",
R"(
A scalar with several lines in it and also 'single quotes'.
  
  A blank line precedes this one.
  And "double quotes" and assorted escapes such as \r or \n)",
  L{N("A scalar with several lines in it and also 'single quotes'.\nA blank line precedes this one. And \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines last",
R"(
A scalar with several lines in it and also 'single quotes'.
  And "double quotes" and assorted escapes such as \r or \n.
  A blank line follows after this one.
  
  )",
  L{N("A scalar with several lines in it and also 'single quotes'. And \"double quotes\" and assorted escapes such as \\r or \\n. A blank line follows after this one.\n")}
),

C("plain scalar, example",
R"(
Several lines of text
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

C("plain scalar, map example 1",
R"(
example: Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("example", "Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

/*
C("plain scalar, map example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
example:
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("example", "Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),
*/

C("plain scalar, seq example 1",
R"(
- Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

/*
C("plain scalar, seq example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
-
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),
*/

//-----------------------------------------------------------------------------
#define BLOCK_LITERAL_CASES \
    "block literal as seq val, implicit indentation 2",\
    "block literal as seq val, implicit indentation 2, chomp=keep",\
    "block literal as seq val, implicit indentation 2, chomp=strip",\
    "block literal as seq val at eof, implicit indentation 2",\
    "block literal as seq val at eof, implicit indentation 4",\
    "block literal as map val, implicit indentation 2",\
    "block literal as map val, implicit indentation 3",\
    "block literal as map val, implicit indentation 4",\
    "block literal as map val, implicit indentation 9",\
    "block literal as map val, explicit indentation 2",\
    "block literal as map val, explicit indentation 2, chomp=keep",\
    "block literal as map val, explicit indentation 2, chomp=strip",\
    "block literal as map val at eof, implicit indentation 2",\
    "block literal as map val at eof, implicit indentation 4",\
    "block literal as map val, explicit indentation 3",\
    "block literal as map val, explicit indentation 4",\
    "block literal as map val, explicit indentation 9"

C("block literal as seq val, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N("Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another val")
  }
),

C("block literal as seq val, implicit indentation 2, chomp=keep",
R"(
- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N("Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another val")
  }
),

C("block literal as seq val, implicit indentation 2, chomp=strip",
R"(
- |-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N("Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another val")
  }
),

C("block literal as seq val at eof, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N("Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as seq val at eof, implicit indentation 4",
R"(
- |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N("Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2",
R"(
example: |2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2, chomp=keep",
R"(
example: |+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2, chomp=strip",
R"(
example: |-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another", "val")
  }
),

C("block literal as map val, implicit indentation 3",
R"(
example: |
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 3",
R"(
example: |3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 4",
R"(
example: |4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val at eof, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val at eof, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val, implicit indentation 9",
R"(
example: |
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 9",
R"(
example: |9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N("example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),


//-----------------------------------------------------------------------------
#define BLOCK_FOLDED_CASES \
    "block folded as seq val, implicit indentation 2", \
    "block folded as map val, implicit indentation 2",\
    "block folded as map val, implicit indentation 2, chomp=keep",\
    "block folded as map val, implicit indentation 2, chomp=strip",\
    "block folded as map val, implicit indentation 3",\
    "block folded as map val, implicit indentation 4",\
    "block folded as map val, implicit indentation 9",\
    "block folded as map val, explicit indentation 2",\
    "block folded as map val, explicit indentation 2, chomp=keep",\
    "block folded as map val, explicit indentation 2, chomp=strip",\
    "block folded as map val, explicit indentation 3",\
    "block folded as map val, explicit indentation 4",\
    "block folded as map val, explicit indentation 9"

C("block folded as seq val, implicit indentation 2",
R"(
- >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N("Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another val")
  }
),

C("block folded as map val, implicit indentation 2",
R"(
example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, implicit indentation 2, chomp=keep",
R"(
example: >+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
),

C("block folded as map val, implicit indentation 2, chomp=strip",
R"(
example: >-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 2",
R"(
example: >2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 2, chomp=keep",
R"(
example: >+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 2, chomp=strip",
R"(
example: >-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N("another", "val")
  }
),

C("block folded as map val, implicit indentation 3",
R"(
example: >
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 3",
R"(
example: >3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, implicit indentation 4",
R"(
example: >
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 4",
R"(
example: >4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, implicit indentation 9",
R"(
example: >
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 9",
R"(
example: >9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
),

//-----------------------------------------------------------------------------
#define TAG_PROPERTY_CASES \
    "tag property in implicit map",\
    "tag property in explicit map",\
    "tag property in implicit seq",\
    "tag property in explicit seq",\
    "tagged explicit sequence in map"

C("tag property in implicit map",
R"(ivar: !!int 0
svar: !!str 0
!!str key: !!int val
)",
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N(TS("!!str", "key"), TS("!!int", "val"))
    }
),

C("tag property in explicit map",
R"({
ivar: !!int 0,
svar: !!str 0,
!!str key: !!int val
}
)",
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N(TS("!!str", "key"), TS("!!int", "val"))
    }
),

C("tag property in implicit seq",
R"(- !!int 0
- !!str 0
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tag property in explicit seq",
R"([
!!int 0,
!!str 0
]
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tagged explicit sequence in map",
R"(some_seq: !its_type [
!!int 0,
!!str 0
]
)",
    L{N("some_seq", TL("!its_type", L{
              N(TS("!!int", "0")),
              N(TS("!!str", "0")),
                  }))
          }
),

//-----------------------------------------------------------------------------
#define NESTED_MAPX2_CASES \
    "nested map x2, explicit, same line", \
        "nested map x2, explicit", \
        "nested map x2",\
		"nested map x2, commented"

C("nested map x2, explicit, same line",
R"({foo: {foo0: 00, bar0: 01, baz0: 02}, bar: {foo1: 10, bar1: 11, baz1: 12}, baz: {foo2: 20, bar2: 21, baz2: 22}})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),

C("nested map x2, explicit",
R"({
foo: {foo0: 00, bar0: 01, baz0: 02},
bar: {foo1: 10, bar1: 11, baz1: 12},
baz: {foo2: 20, bar2: 21, baz2: 22}
})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),

C("nested map x2",
R"(
foo:
  foo0: 00
  bar0: 01
  baz0: 02
bar:
  foo1: 10
  bar1: 11
  baz1: 12
baz:
  foo2: 20
  bar2: 21
  baz2: 22
)",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
),


C("nested map x2, commented",
	R"(
send_to:
  #host: 192.168.1.100
  #port: 7000
  host: 192.168.1.101
  port: 7001
  #host: 192.168.1.102
  #port: 7002
)",
    L{
	  N("send_to", L{
	    N("host", "192.168.1.101"),
        N("port", "7001") })
	}
),

//-----------------------------------------------------------------------------
#define NESTED_SEQX2_CASES                                          \
"nested seq x2, empty, oneline",                                    \
"nested seq x2, explicit, same line",                               \
"nested seq x2, explicit first+last level, same line, no spaces",   \
"nested seq x2, explicit",                                          \
"nested seq x2",                                                    \
"nested seq x2, next line",                                         \
"nested seq x2, all next line",                                     \
"nested seq x2, implicit first, explicit last level"

C("nested seq x2, empty, oneline",
R"([[], [], []])",
    L{SEQ, SEQ, SEQ}
),

C("nested seq x2, explicit, same line",
R"([[00, 01, 02], [10, 11, 12], [20, 21, 22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, explicit first+last level, same line, no spaces",
R"([[00,01,02],[10,11,12],[20,21,22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, explicit",
R"([
[00, 01, 02],
[10, 11, 12],
[20, 21, 22],
])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2",
R"(
- - 00
  - 01
  - 02
- - 10
  - 11
  - 12
- - 20
  - 21
  - 22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, next line",
R"(
-
  - 00
  - 01
  - 02
-
  - 10
  - 11
  - 12
-
  - 20
  - 21
  - 22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, all next line",
R"(
-
  -
    00
  -
    01
  -
    02
-
  -
    10
  -
    11
  -
    12
-
  -
    20
  -
    21
  -
    22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

C("nested seq x2, implicit first, explicit last level",
R"(
- [00, 01, 02]
- [10, 11, 12]
- [20, 21, 22]
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
),

//-----------------------------------------------------------------------------

#define NESTED_MAPX3_CASES \
    "nested map x3, explicit",               \
        "nested map x3"


C("nested map x3, explicit",
R"({
  foo0: {
     foo1: {foo2: 000, bar2: 001, baz2: 002},
     bar1: {foo2: 010, bar2: 011, baz2: 012},
     baz1: {foo2: 020, bar2: 021, baz2: 022}
  },
  bar0: {
     foo1: {foo2: 100, bar2: 101, baz2: 102},
     bar1: {foo2: 110, bar2: 111, baz2: 112},
     baz1: {foo2: 120, bar2: 121, baz2: 122}
  },
  baz0: {
     foo1: {foo2: 200, bar2: 201, baz2: 202},
     bar1: {foo2: 210, bar2: 211, baz2: 212},
     baz1: {foo2: 220, bar2: 221, baz2: 222}
  }
})",
    L{
      N{"foo0", L{
         N{"foo1", L{N{"foo2", "000"}, N{"bar2", "001"}, N{"baz2", "002"}}},
         N{"bar1", L{N{"foo2", "010"}, N{"bar2", "011"}, N{"baz2", "012"}}},
         N{"baz1", L{N{"foo2", "020"}, N{"bar2", "021"}, N{"baz2", "022"}}} }},
      N{"bar0", L{
         N{"foo1", L{N{"foo2", "100"}, N{"bar2", "101"}, N{"baz2", "102"}}},
         N{"bar1", L{N{"foo2", "110"}, N{"bar2", "111"}, N{"baz2", "112"}}},
         N{"baz1", L{N{"foo2", "120"}, N{"bar2", "121"}, N{"baz2", "122"}}} }},
      N{"baz0", L{
         N{"foo1", L{N{"foo2", "200"}, N{"bar2", "201"}, N{"baz2", "202"}}},
         N{"bar1", L{N{"foo2", "210"}, N{"bar2", "211"}, N{"baz2", "212"}}},
         N{"baz1", L{N{"foo2", "220"}, N{"bar2", "221"}, N{"baz2", "222"}}} }},
          }
),

C("nested map x3",
R"(
foo0:
  foo1:
    foo2: 000
    bar2: 001
    baz2: 002
  bar1:
    foo2: 010
    bar2: 011
    baz2: 012
  baz1:
    foo2: 020
    bar2: 021
    baz2: 022
bar0:
  foo1:
    foo2: 100
    bar2: 101
    baz2: 102
  bar1:
    foo2: 110
    bar2: 111
    baz2: 112
  baz1:
    foo2: 120
    bar2: 121
    baz2: 122
baz0:
  foo1:
    foo2: 200
    bar2: 201
    baz2: 202
  bar1:
    foo2: 210
    bar2: 211
    baz2: 212
  baz1:
    foo2: 220
    bar2: 221
    baz2: 222
)",
    L{
      N{"foo0", L{
         N{"foo1", L{N{"foo2", "000"}, N{"bar2", "001"}, N{"baz2", "002"}}},
         N{"bar1", L{N{"foo2", "010"}, N{"bar2", "011"}, N{"baz2", "012"}}},
         N{"baz1", L{N{"foo2", "020"}, N{"bar2", "021"}, N{"baz2", "022"}}} }},
      N{"bar0", L{
         N{"foo1", L{N{"foo2", "100"}, N{"bar2", "101"}, N{"baz2", "102"}}},
         N{"bar1", L{N{"foo2", "110"}, N{"bar2", "111"}, N{"baz2", "112"}}},
         N{"baz1", L{N{"foo2", "120"}, N{"bar2", "121"}, N{"baz2", "122"}}} }},
      N{"baz0", L{
         N{"foo1", L{N{"foo2", "200"}, N{"bar2", "201"}, N{"baz2", "202"}}},
         N{"bar1", L{N{"foo2", "210"}, N{"bar2", "211"}, N{"baz2", "212"}}},
         N{"baz1", L{N{"foo2", "220"}, N{"bar2", "221"}, N{"baz2", "222"}}} }},
      }
),


//-----------------------------------------------------------------------------
#define NESTED_SEQX3_CASES \
"nested seq x3, explicit", \
"nested seq x3", \
"nested seq x3, continued on next line", \
"nested seq x3, all continued on next line"


C("nested seq x3, explicit",
R"([
[[000, 001, 002], [010, 011, 012], [020, 021, 022]],
[[100, 101, 102], [110, 111, 112], [120, 121, 122]],
[[200, 201, 202], [210, 211, 212], [220, 221, 222]],
])",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),

C("nested seq x3",
R"(
- - - 000
    - 001
    - 002
  - - 010
    - 011
    - 012
  - - 020
    - 021
    - 022
- - - 100
    - 101
    - 102
  - - 110
    - 111
    - 112
  - - 120
    - 121
    - 122
- - - 200
    - 201
    - 202
  - - 210
    - 211
    - 212
  - - 220
    - 221
    - 222
)",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),

C("nested seq x3, continued on next line",
R"(
-
  -
    - 000
    - 001
    - 002
  -
    - 010
    - 011
    - 012
  -
    - 020
    - 021
    - 022
-
  -
    - 100
    - 101
    - 102
  -
    - 110
    - 111
    - 112
  -
    - 120
    - 121
    - 122
-
  -
    - 200
    - 201
    - 202
  -
    - 210
    - 211
    - 212
  -
    - 220
    - 221
    - 222
)",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),

C("nested seq x3, all continued on next line",
R"(
-
  -
    -
      000
    -
      001
    -
      002
  -
    -
      010
    -
      011
    -
      012
  -
    -
      020
    -
      021
    -
      022
-
  -
    -
      100
    -
      101
    -
      102
  -
    -
      110
    -
      111
    -
      112
  -
    -
      120
    -
      121
    -
      122
-
  -
    -
      200
    -
      201
    -
      202
  -
    -
      210
    -
      211
    -
      212
  -
    -
      220
    -
      221
    -
      222
)",
    L{
      N{L{N{L{N{"000"}, N{"001"}, N{"002"}}}, N{L{N{"010"}, N{"011"}, N{"012"}}}, N{L{N{"020"}, N{"021"}, N{"022"}}}}},
      N{L{N{L{N{"100"}, N{"101"}, N{"102"}}}, N{L{N{"110"}, N{"111"}, N{"112"}}}, N{L{N{"120"}, N{"121"}, N{"122"}}}}},
      N{L{N{L{N{"200"}, N{"201"}, N{"202"}}}, N{L{N{"210"}, N{"211"}, N{"212"}}}, N{L{N{"220"}, N{"221"}, N{"222"}}}}},
          }
),

//-----------------------------------------------------------------------------
#define NESTED_MAPX4_CASES                      \
        "nested map x4, explicit",              \
        "nested map x4"

C("nested map x4, explicit",
R"({
  foo0: {
     foo1: { foo2: {foo3: 0000, bar3: 0001, baz3: 0002}, bar2: {foo3: 0010, bar3: 0011, baz3: 0012}, baz2: {foo3: 0020, bar3: 0021, baz3: 0022} },
     bar1: { foo2: {foo3: 0100, bar3: 0101, baz3: 0102}, bar2: {foo3: 0110, bar3: 0111, baz3: 0112}, baz2: {foo3: 0120, bar3: 0121, baz3: 0122} },
     baz1: { foo2: {foo3: 0200, bar3: 0201, baz3: 0202}, bar2: {foo3: 0210, bar3: 0211, baz3: 0212}, baz2: {foo3: 0220, bar3: 0221, baz3: 0222} },
  },
  bar0: {
     foo1: { foo2: {foo3: 1000, bar3: 1001, baz3: 1002}, bar2: {foo3: 1010, bar3: 1011, baz3: 1012}, baz2: {foo3: 1020, bar3: 1021, baz3: 1022} },
     bar1: { foo2: {foo3: 1100, bar3: 1101, baz3: 1102}, bar2: {foo3: 1110, bar3: 1111, baz3: 1112}, baz2: {foo3: 1120, bar3: 1121, baz3: 1122} },
     baz1: { foo2: {foo3: 1200, bar3: 1201, baz3: 1202}, bar2: {foo3: 1210, bar3: 1211, baz3: 1212}, baz2: {foo3: 1220, bar3: 1221, baz3: 1222} },
  },
  baz0: {
     foo1: { foo2: {foo3: 2000, bar3: 2001, baz3: 2002}, bar2: {foo3: 2010, bar3: 2011, baz3: 2012}, baz2: {foo3: 2020, bar3: 2021, baz3: 2022} },
     bar1: { foo2: {foo3: 2100, bar3: 2101, baz3: 2102}, bar2: {foo3: 2110, bar3: 2111, baz3: 2112}, baz2: {foo3: 2120, bar3: 2121, baz3: 2122} },
     baz1: { foo2: {foo3: 2200, bar3: 2201, baz3: 2202}, bar2: {foo3: 2210, bar3: 2211, baz3: 2212}, baz2: {foo3: 2220, bar3: 2221, baz3: 2222} },
  },
})",
    L{
      N("foo0", L{
         N("foo1", L{N("foo2", L{N("foo3", "0000"), N("bar3", "0001"), N("baz3", "0002")}),  N("bar2", L{N("foo3", "0010"), N("bar3", "0011"), N("baz3", "0012")}),  N("baz2", L{N("foo3", "0020"), N("bar3", "0021"), N("baz3", "0022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "0100"), N("bar3", "0101"), N("baz3", "0102")}),  N("bar2", L{N("foo3", "0110"), N("bar3", "0111"), N("baz3", "0112")}),  N("baz2", L{N("foo3", "0120"), N("bar3", "0121"), N("baz3", "0122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "0200"), N("bar3", "0201"), N("baz3", "0202")}),  N("bar2", L{N("foo3", "0210"), N("bar3", "0211"), N("baz3", "0212")}),  N("baz2", L{N("foo3", "0220"), N("bar3", "0221"), N("baz3", "0222")})}),
      }),
      N("bar0", L{
         N("foo1", L{N("foo2", L{N("foo3", "1000"), N("bar3", "1001"), N("baz3", "1002")}),  N("bar2", L{N("foo3", "1010"), N("bar3", "1011"), N("baz3", "1012")}),  N("baz2", L{N("foo3", "1020"), N("bar3", "1021"), N("baz3", "1022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "1100"), N("bar3", "1101"), N("baz3", "1102")}),  N("bar2", L{N("foo3", "1110"), N("bar3", "1111"), N("baz3", "1112")}),  N("baz2", L{N("foo3", "1120"), N("bar3", "1121"), N("baz3", "1122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "1200"), N("bar3", "1201"), N("baz3", "1202")}),  N("bar2", L{N("foo3", "1210"), N("bar3", "1211"), N("baz3", "1212")}),  N("baz2", L{N("foo3", "1220"), N("bar3", "1221"), N("baz3", "1222")})}),
      }),
      N("baz0", L{
         N("foo1", L{N("foo2", L{N("foo3", "2000"), N("bar3", "2001"), N("baz3", "2002")}),  N("bar2", L{N("foo3", "2010"), N("bar3", "2011"), N("baz3", "2012")}),  N("baz2", L{N("foo3", "2020"), N("bar3", "2021"), N("baz3", "2022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "2100"), N("bar3", "2101"), N("baz3", "2102")}),  N("bar2", L{N("foo3", "2110"), N("bar3", "2111"), N("baz3", "2112")}),  N("baz2", L{N("foo3", "2120"), N("bar3", "2121"), N("baz3", "2122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "2200"), N("bar3", "2201"), N("baz3", "2202")}),  N("bar2", L{N("foo3", "2210"), N("bar3", "2211"), N("baz3", "2212")}),  N("baz2", L{N("foo3", "2220"), N("bar3", "2221"), N("baz3", "2222")})}),
      })
    }
),

C("nested map x4",
R"(
foo0:
  foo1:
    foo2:
      foo3: 0000
      bar3: 0001
      baz3: 0002
    bar2:
      foo3: 0010
      bar3: 0011
      baz3: 0012
    baz2:
      foo3: 0020
      bar3: 0021
      baz3: 0022
  bar1:
    foo2:
      foo3: 0100
      bar3: 0101
      baz3: 0102
    bar2:
      foo3: 0110
      bar3: 0111
      baz3: 0112
    baz2:
      foo3: 0120
      bar3: 0121
      baz3: 0122
  baz1:
    foo2:
      foo3: 0200
      bar3: 0201
      baz3: 0202
    bar2:
      foo3: 0210
      bar3: 0211
      baz3: 0212
    baz2:
      foo3: 0220
      bar3: 0221
      baz3: 0222
bar0:
  foo1:
    foo2:
      foo3: 1000
      bar3: 1001
      baz3: 1002
    bar2:
      foo3: 1010
      bar3: 1011
      baz3: 1012
    baz2:
      foo3: 1020
      bar3: 1021
      baz3: 1022
  bar1:
    foo2:
      foo3: 1100
      bar3: 1101
      baz3: 1102
    bar2:
      foo3: 1110
      bar3: 1111
      baz3: 1112
    baz2:
      foo3: 1120
      bar3: 1121
      baz3: 1122
  baz1:
    foo2:
      foo3: 1200
      bar3: 1201
      baz3: 1202
    bar2:
      foo3: 1210
      bar3: 1211
      baz3: 1212
    baz2:
      foo3: 1220
      bar3: 1221
      baz3: 1222
baz0:
  foo1:
    foo2:
      foo3: 2000
      bar3: 2001
      baz3: 2002
    bar2:
      foo3: 2010
      bar3: 2011
      baz3: 2012
    baz2:
      foo3: 2020
      bar3: 2021
      baz3: 2022
  bar1:
    foo2:
      foo3: 2100
      bar3: 2101
      baz3: 2102
    bar2:
      foo3: 2110
      bar3: 2111
      baz3: 2112
    baz2:
      foo3: 2120
      bar3: 2121
      baz3: 2122
  baz1:
    foo2:
      foo3: 2200
      bar3: 2201
      baz3: 2202
    bar2:
      foo3: 2210
      bar3: 2211
      baz3: 2212
    baz2:
      foo3: 2220
      bar3: 2221
      baz3: 2222
)",
    L{
      N("foo0", L{
         N("foo1", L{N("foo2", L{N("foo3", "0000"), N("bar3", "0001"), N("baz3", "0002")}),  N("bar2", L{N("foo3", "0010"), N("bar3", "0011"), N("baz3", "0012")}),  N("baz2", L{N("foo3", "0020"), N("bar3", "0021"), N("baz3", "0022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "0100"), N("bar3", "0101"), N("baz3", "0102")}),  N("bar2", L{N("foo3", "0110"), N("bar3", "0111"), N("baz3", "0112")}),  N("baz2", L{N("foo3", "0120"), N("bar3", "0121"), N("baz3", "0122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "0200"), N("bar3", "0201"), N("baz3", "0202")}),  N("bar2", L{N("foo3", "0210"), N("bar3", "0211"), N("baz3", "0212")}),  N("baz2", L{N("foo3", "0220"), N("bar3", "0221"), N("baz3", "0222")})}),
      }),
      N("bar0", L{
         N("foo1", L{N("foo2", L{N("foo3", "1000"), N("bar3", "1001"), N("baz3", "1002")}),  N("bar2", L{N("foo3", "1010"), N("bar3", "1011"), N("baz3", "1012")}),  N("baz2", L{N("foo3", "1020"), N("bar3", "1021"), N("baz3", "1022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "1100"), N("bar3", "1101"), N("baz3", "1102")}),  N("bar2", L{N("foo3", "1110"), N("bar3", "1111"), N("baz3", "1112")}),  N("baz2", L{N("foo3", "1120"), N("bar3", "1121"), N("baz3", "1122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "1200"), N("bar3", "1201"), N("baz3", "1202")}),  N("bar2", L{N("foo3", "1210"), N("bar3", "1211"), N("baz3", "1212")}),  N("baz2", L{N("foo3", "1220"), N("bar3", "1221"), N("baz3", "1222")})}),
      }),
      N("baz0", L{
         N("foo1", L{N("foo2", L{N("foo3", "2000"), N("bar3", "2001"), N("baz3", "2002")}),  N("bar2", L{N("foo3", "2010"), N("bar3", "2011"), N("baz3", "2012")}),  N("baz2", L{N("foo3", "2020"), N("bar3", "2021"), N("baz3", "2022")})}),
         N("bar1", L{N("foo2", L{N("foo3", "2100"), N("bar3", "2101"), N("baz3", "2102")}),  N("bar2", L{N("foo3", "2110"), N("bar3", "2111"), N("baz3", "2112")}),  N("baz2", L{N("foo3", "2120"), N("bar3", "2121"), N("baz3", "2122")})}),
         N("baz1", L{N("foo2", L{N("foo3", "2200"), N("bar3", "2201"), N("baz3", "2202")}),  N("bar2", L{N("foo3", "2210"), N("bar3", "2211"), N("baz3", "2212")}),  N("baz2", L{N("foo3", "2220"), N("bar3", "2221"), N("baz3", "2222")})}),
      })
    }
),

//-----------------------------------------------------------------------------
#define NESTED_SEQX4_CASES \
    "nested seq x4, explicit", \
    "nested seq x4"


C("nested seq x4, explicit",
R"([
[[[0000, 0001, 0002], [0010, 0011, 0012], [0020, 0021, 0022]],
 [[0100, 0101, 0102], [0110, 0111, 0112], [0120, 0121, 0122]],
 [[0200, 0201, 0202], [0210, 0211, 0212], [0220, 0221, 0222]]],

[[[1000, 1001, 1002], [1010, 1011, 1012], [1020, 1021, 1022]],
 [[1100, 1101, 1102], [1110, 1111, 1112], [1120, 1121, 1122]],
 [[1200, 1201, 1202], [1210, 1211, 1212], [1220, 1221, 1222]]],

[[[2000, 2001, 2002], [2010, 2011, 2012], [2020, 2021, 2022]],
 [[2100, 2101, 2102], [2110, 2111, 2112], [2120, 2121, 2122]],
 [[2200, 2201, 2202], [2210, 2211, 2212], [2220, 2221, 2222]]],
])",
    L{
      N{L{N{L{N{L{N{"0000"}, N{"0001"}, N{"0002"}}}, N{L{N{"0010"}, N{"0011"}, N{"0012"}}}, N{L{N{"0020"}, N{"0021"}, N{"0022"}}}}}, N{L{N{L{N{"0100"}, N{"0101"}, N{"0102"}}}, N{L{N{"0110"}, N{"0111"}, N{"0112"}}}, N{L{N{"0120"}, N{"0121"}, N{"0122"}}}}}, N{L{N{L{N{"0200"}, N{"0201"}, N{"0202"}}}, N{L{N{"0210"}, N{"0211"}, N{"0212"}}}, N{L{N{"0220"}, N{"0221"}, N{"0222"}}}}}}},
      N{L{N{L{N{L{N{"1000"}, N{"1001"}, N{"1002"}}}, N{L{N{"1010"}, N{"1011"}, N{"1012"}}}, N{L{N{"1020"}, N{"1021"}, N{"1022"}}}}}, N{L{N{L{N{"1100"}, N{"1101"}, N{"1102"}}}, N{L{N{"1110"}, N{"1111"}, N{"1112"}}}, N{L{N{"1120"}, N{"1121"}, N{"1122"}}}}}, N{L{N{L{N{"1200"}, N{"1201"}, N{"1202"}}}, N{L{N{"1210"}, N{"1211"}, N{"1212"}}}, N{L{N{"1220"}, N{"1221"}, N{"1222"}}}}}}},
      N{L{N{L{N{L{N{"2000"}, N{"2001"}, N{"2002"}}}, N{L{N{"2010"}, N{"2011"}, N{"2012"}}}, N{L{N{"2020"}, N{"2021"}, N{"2022"}}}}}, N{L{N{L{N{"2100"}, N{"2101"}, N{"2102"}}}, N{L{N{"2110"}, N{"2111"}, N{"2112"}}}, N{L{N{"2120"}, N{"2121"}, N{"2122"}}}}}, N{L{N{L{N{"2200"}, N{"2201"}, N{"2202"}}}, N{L{N{"2210"}, N{"2211"}, N{"2212"}}}, N{L{N{"2220"}, N{"2221"}, N{"2222"}}}}}}},
          }
),

C("nested seq x4",
R"(
- - - - 0000
      - 0001
      - 0002
    - - 0010
      - 0011
      - 0012
    - - 0020
      - 0021
      - 0022
  - - - 0100
      - 0101
      - 0102
    - - 0110
      - 0111
      - 0112
    - - 0120
      - 0121
      - 0122
  - - - 0200
      - 0201
      - 0202
    - - 0210
      - 0211
      - 0212
    - - 0220
      - 0221
      - 0222
- - - - 1000
      - 1001
      - 1002
    - - 1010
      - 1011
      - 1012
    - - 1020
      - 1021
      - 1022
  - - - 1100
      - 1101
      - 1102
    - - 1110
      - 1111
      - 1112
    - - 1120
      - 1121
      - 1122
  - - - 1200
      - 1201
      - 1202
    - - 1210
      - 1211
      - 1212
    - - 1220
      - 1221
      - 1222
- - - - 2000
      - 2001
      - 2002
    - - 2010
      - 2011
      - 2012
    - - 2020
      - 2021
      - 2022
  - - - 2100
      - 2101
      - 2102
    - - 2110
      - 2111
      - 2112
    - - 2120
      - 2121
      - 2122
  - - - 2200
      - 2201
      - 2202
    - - 2210
      - 2211
      - 2212
    - - 2220
      - 2221
      - 2222
)",
    L{
      N{L{N{L{N{L{N{"0000"}, N{"0001"}, N{"0002"}}}, N{L{N{"0010"}, N{"0011"}, N{"0012"}}}, N{L{N{"0020"}, N{"0021"}, N{"0022"}}}}}, N{L{N{L{N{"0100"}, N{"0101"}, N{"0102"}}}, N{L{N{"0110"}, N{"0111"}, N{"0112"}}}, N{L{N{"0120"}, N{"0121"}, N{"0122"}}}}}, N{L{N{L{N{"0200"}, N{"0201"}, N{"0202"}}}, N{L{N{"0210"}, N{"0211"}, N{"0212"}}}, N{L{N{"0220"}, N{"0221"}, N{"0222"}}}}}}},
      N{L{N{L{N{L{N{"1000"}, N{"1001"}, N{"1002"}}}, N{L{N{"1010"}, N{"1011"}, N{"1012"}}}, N{L{N{"1020"}, N{"1021"}, N{"1022"}}}}}, N{L{N{L{N{"1100"}, N{"1101"}, N{"1102"}}}, N{L{N{"1110"}, N{"1111"}, N{"1112"}}}, N{L{N{"1120"}, N{"1121"}, N{"1122"}}}}}, N{L{N{L{N{"1200"}, N{"1201"}, N{"1202"}}}, N{L{N{"1210"}, N{"1211"}, N{"1212"}}}, N{L{N{"1220"}, N{"1221"}, N{"1222"}}}}}}},
      N{L{N{L{N{L{N{"2000"}, N{"2001"}, N{"2002"}}}, N{L{N{"2010"}, N{"2011"}, N{"2012"}}}, N{L{N{"2020"}, N{"2021"}, N{"2022"}}}}}, N{L{N{L{N{"2100"}, N{"2101"}, N{"2102"}}}, N{L{N{"2110"}, N{"2111"}, N{"2112"}}}, N{L{N{"2120"}, N{"2121"}, N{"2122"}}}}}, N{L{N{L{N{"2200"}, N{"2201"}, N{"2202"}}}, N{L{N{"2210"}, N{"2211"}, N{"2212"}}}, N{L{N{"2220"}, N{"2221"}, N{"2222"}}}}}}},
          }
),

//-----------------------------------------------------------------------------
#define COMPLEX_KEY_CASES                       \
"complex key with line break in between",       \
"complex key 2nd, inside explicit map",         \
"complex key 1st, inside explicit map",         \
"complex key 2nd",                              \
"complex key 1st",                              \
"complex key nested in a map, 1st",             \
"complex key nested in a seq, 1st",\
"complex block key, literal, clip",\
"complex block key, literal, keep",\
"complex block key, literal, strip",\
"complex block key, folded, clip",\
"complex block key, folded, keep",\
"complex block key, folded, strip"


C("complex key with line break in between",
R"(
? a complex key
: its value
)",
  L{N("a complex key", "its value")}
),

C("complex key 2nd, inside explicit map",
R"(
{
    a simple key: a value,
    ? a complex key: another value,
}
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
  }
),

C("complex key 1st, inside explicit map",
R"(
{
    ? a complex key: another value,
    a simple key: a value,
}
)",
  L{
      N("a complex key", "another value"),
      N("a simple key", "a value"),
  }
),

C("complex key 2nd",
R"(
a simple key: a value
? a complex key: another value
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
  }
),

C("complex key 1st",
R"(
? a complex key: another value
a simple key: a value
)",
  L{
      N("a complex key", "another value"),
      N("a simple key", "a value"),
  }
),

C("complex key nested in a map, 1st",
R"(
map:
  ? a complex key: another value
  a simple key: a value
? a complex key deindented: its value
)",
  L{
      N("map", L{
          N("a complex key", "another value"),
          N("a simple key", "a value"),
      }),
      N("a complex key deindented", "its value")
   }
),

C("complex key nested in a seq, 1st",
R"(
- ? a complex key: another value
  a simple key: a value
- ? another complex key: its value
)",
  L{
      N(L{
          N("a complex key", "another value"),
          N("a simple key", "a value"),
      }),
      N(L{N("another complex key", "its value")})
   }
),

C("complex block key, literal, clip",
R"(? |
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key\nthat has multiple lines\n", "and this is its value")
   }
),

C("complex block key, literal, keep",
R"(? |+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key\nthat has multiple lines\n\n", "and this is its value")
   }
),

C("complex block key, literal, strip",
R"(? |-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key\nthat has multiple lines", "and this is its value")
   }
),

C("complex block key, folded, clip",
R"(? >
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key that has multiple lines\n", "and this is its value")
   }
),

C("complex block key, folded, keep",
R"(? >+
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key that has multiple lines\n\n", "and this is its value")
   }
),

C("complex block key, folded, strip",
R"(? >-
    This is a key
    that has multiple lines
    
: and this is its value
)",
  L{
      N("This is a key that has multiple lines", "and this is its value")
   }
),


//-----------------------------------------------------------------------------
#define MAP_OF_SEQ_CASES \
    "map of empty seqs", \
    "map of seqs, one line", \
    "map of seqs",           \
    "map of seqs, next line"

C("map of empty seqs",
R"({foo: [], bar: [], baz: []})",
     L{
         N(KEYSEQ, "foo", L()),
         N(KEYSEQ, "bar", L()),
         N(KEYSEQ, "baz", L()),
     }
),

C("map of seqs, one line",
R"({men: [John Smith, Bill Jones], women: [Mary Smith, Susan Williams]})",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs",
R"(
men:
  - John Smith
  - Bill Jones
women:
  - Mary Smith
  - Susan Williams
)",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs, next line",
R"(
men:
  - 
    John Smith
  - 
    Bill Jones
women:
  - 
    Mary Smith
  - 
    Susan Williams
)",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

//-----------------------------------------------------------------------------
#define SEQ_OF_MAP_CASES                            \
    "seq of empty maps, one line",                  \
        "seq of maps, one line",                    \
        "seq of maps, implicit seq, explicit maps", \
        "seq of maps",                              \
        "seq of maps, next line"

C("seq of empty maps, one line",
R"([{}, {}, {}])",
  L{MAP, MAP, MAP}
),

C("seq of maps, one line",
R"([{name: John Smith, age: 33}, {name: Mary Smith, age: 27}])",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps, implicit seq, explicit maps",
R"(
- {name: John Smith, age: 33}
- {name: Mary Smith, age: 27}
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps",
R"(
- name: John Smith
  age: 33
- name: Mary Smith
  age: 27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps, next line",
R"(
- 
  name:
    John Smith
  age:
    33
- 
  name: 
    Mary Smith
  age:
    27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

//-----------------------------------------------------------------------------
#define GENERIC_SEQ_CASES                       \
    "generic seq v0",                              \
        "generic seq v1"

C("generic seq v0",
R"(
- item 1
- item 2
- - item 3.1
  - item 3.2
- key 1: value 1
  key 2: value 2
)",
  L{
      N("item 1"),
      N("item 2"),
      N(L{N("item 3.1"), N("item 3.2")}),
      N(L{N("key 1", "value 1"), N("key 2", "value 2")})
  }
),

C("generic seq v1",
R"(
- item 1
- item 2
-
  - item 3.1
  - item 3.2
-
  key 1: value 1
  key 2: value 2
)",
  L{
      N("item 1"),
      N("item 2"),
      N(L{N("item 3.1"), N("item 3.2")}),
      N(L{N("key 1", "value 1"), N("key 2", "value 2")})
  }
),

//-----------------------------------------------------------------------------
#define GENERIC_MAP_CASES                       \
        "generic map"

C("generic map",
R"(
a simple key: a value   # The KEY token is produced here.
? a complex key
: another value
a mapping:
  key 1: value 1
  key 2: value 2
a sequence:
  - item 1
  - item 2
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
      N("a mapping", L{N("key 1", "value 1"), N("key 2", "value 2")}),
      N("a sequence", L{N("item 1"), N("item 2")}),
  }
),

//-----------------------------------------------------------------------------
#define SIMPLE_ANCHOR_CASES                            \
    "simple anchor 1, implicit, unresolved",\
    "simple anchor 1, implicit, resolved",\
    "simple anchor 1, explicit, unresolved",\
    "simple anchor 1, explicit, resolved",\
    "anchor example 2, unresolved",   \
    "anchor example 2, resolved",\
    "anchor example 3, unresolved",   \
    "anchor example 3, resolved"

C("simple anchor 1, implicit, unresolved",
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(ANCHOR, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(REF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(ANCHOR, "anchor_in_seq")),
              N(REF, "*anchor_in_seq", AR(REF, "anchor_in_seq")),
          }, AR()),
      N("base", L{N("name", "Everyone has same name")}, AR(ANCHOR, "base")),
      N("foo", L{N(REF, "<<", "*base"), N("age", "10")}, AR(ANCHOR, "foo")),
      N("bar", L{N(REF, "<<", "*base"), N("age", "20")}, AR(ANCHOR, "bar")),
  }
),

C("simple anchor 1, explicit, unresolved",
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(ANCHOR, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(REF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(ANCHOR, "anchor_in_seq")),
              N(REF, "*anchor_in_seq", AR(REF, "anchor_in_seq")),
          }, AR()),
      N("base", L{N("name", "Everyone has same name")}, AR(ANCHOR, "base")),
      N("foo", L{N(REF, "<<", "*base"), N("age", "10")}, AR(ANCHOR, "foo")),
      N("bar", L{N(REF, "<<", "*base"), N("age", "20")}, AR(ANCHOR, "bar")),
  }
),

C("simple anchor 1, implicit, resolved", RESOLVE_REFS,
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),

C("simple anchor 1, explicit, resolved", RESOLVE_REFS,
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),


C("anchor example 2, unresolved",
R"(
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS
ship-to:  *id001
specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
)",
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}, AR(ANCHOR, "id001")},
   N{REF, "ship-to", "*id001", AR(REF, "id001")},
   N{"specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),


C("anchor example 2, resolved", RESOLVE_REFS,
R"(
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS
ship-to:  *id001
specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
)",
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{"ship-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{"specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),

C("anchor example 3, unresolved",
R"(
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm
- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
L{N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }, AR(ANCHOR, "id001")),
    }), N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }, AR(ANCHOR, "id002")),
    }), N(L{
N{REF, "step", "*id001"},
    }), N(L{
N{REF, "step", "*id002"},
    }), N(L{
N{"step", L{
    N{REF, "<<", "*id001"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{REF, "step", "*id002"},
    }),
    }
),

C("anchor example 3, resolved", RESOLVE_REFS,
R"(
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm
- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
  L{N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }),
    }
),


//-----------------------------------------------------------------------------
#define INDENTATION_CASES \
    "4 chars",\
    "2 chars + 4 chars, ex0",\
    "2 chars + 4 chars, ex1",\
    "2 chars + 4 chars, ex2"

C("4 chars",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),


C("2 chars + 4 chars, ex0",
R"(
key:
     value
another_key:
    sub_key0:
        - val0
        - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex1",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
        - val2
        - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex2",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
        - val4
        - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),


//-----------------------------------------------------------------------------
#define NUMBER_CASES \
    "integer numbers, expl",\
    "integer numbers, impl",\
    "floating point numbers, expl",\
    "floating point numbers, impl"

C("integer numbers, expl",
R"(translation: [-2, -2, 5])",
L{N("translation", L{N("-2"), N("-2"), N("5")})}
),

C("integer numbers, impl",
R"(translation:
  - -2
  - -2
  - -5
)",
L{N("translation", L{N("-2"), N("-2"), N("-5")})}
),

C("floating point numbers, expl",
R"([-2.0, -2.1, 0.1, .1, -.2, -2.e+6, -3e-6, 1.12345e+011])",
L{N("-2.0"), N("-2.1"), N("0.1"), N(".1"), N("-.2"), N("-2.e+6"), N("-3e-6"), N("1.12345e+011")}
),

C("floating point numbers, impl",
R"(
- -2.0
- -2.1
- 0.1
- .1
- -.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
L{N("-2.0"), N("-2.1"), N("0.1"), N(".1"), N("-.2"), N("-2.e+6"), N("-3e-6"), N("1.12345e+011")}
),


//-----------------------------------------------------------------------------
#define NULL_VAL_CASES \
    "null map vals, expl",\
    "null map vals, impl",\
    "null seq vals, impl",\
    "null seq vals in map, impl, mixed 1",\
    "null seq vals in map, impl, mixed 2",\
    "null seq vals in map, impl, mixed 3",\
    "null map vals in seq, impl, mixed 1",\
    "null map vals in seq, impl, mixed 2",\
    "null map vals in seq, impl, mixed 3"

C("null map vals, expl",
R"({foo: , bar: , baz: }
)",
L{N("foo", ""), N("bar", ""), N("baz", "")}
),

C("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
L{N("foo", ""), N("bar", ""), N("baz", "")}
),

C("null seq vals, impl",
R"(- 
- 
- 
)",
L{N(""), N(""), N("")}
),

C("null seq vals in map, impl, mixed 1",
R"(
foo:
  - 
  - 
  - 
bar: 
baz: 
)",
L{N("foo", L{N(""), N(""), N("")}), N("bar", ""), N("baz", "")}
),

C("null seq vals in map, impl, mixed 2",
R"(
foo:
bar: 
  - 
  - 
  - 
baz: 
)",
L{N("foo", ""), N("bar", L{N(""), N(""), N("")}), N("baz", "")}
),

C("null seq vals in map, impl, mixed 3",
R"(
foo:
bar: 
baz: 
  - 
  - 
  - 
)",
L{N("foo", ""), N("bar", ""), N("baz", L{N(""), N(""), N("")})}
),

C("null map vals in seq, impl, mixed 1",
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
L{N(L{N("foo", ""), N("bar", ""), N("baz", "")}), N(""), N("")}
),

C("null map vals in seq, impl, mixed 2",
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
L{N(""), N(L{N("foo", ""), N("bar", ""), N("baz", "")}), N("")}
),

C("null map vals in seq, impl, mixed 3",
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
L{N(""), N(""), N(L{N("foo", ""), N("bar", ""), N("baz", "")})}
),

//-----------------------------------------------------------------------------
#define GITHUB_ISSUE_CASES \
        "github3-problem1",\
        "github3-problem2-ex1",\
        "github3-problem2-ex2",\
        "github3-problem3",\
        "github3",\
        "github6"


C("github3-problem1",
R"(
translation: [-2, -2, 5])",
L{N("translation", L{N("-2"), N("-2"), N("5")})}
),

// these must work without quotes
C("github3-problem2-ex1",
R"(
audio resource: 
)",
L{N("audio resource", "")}
),
C("github3-problem2-ex2",
R"(
audio resource: 
more:
  example: y
)",
L{N("audio resource", ""), N("more", L{N("example", "y")})}
),

C("github3-problem3",
R"(component:
  type: perspective camera component
  some_data: {}  # this was working
  data:
    {}           # but this was not working
)",
L{N("component", L{N("type", "perspective camera component"), N(KEYMAP, "some_data", L{}), N(KEYMAP, "data", L{})})}
),

C("github3",
R"(
universe:
  objects:
    object:
      uuid: A7AB039C0EF3A74480A1B398247039A7
      components:
        - component:
            type: name component
            data:
              object name: Root Node
        - component:
            type: transform component
            data:
              translation: [-2, -2, 5]
              rotation: [0, 0, 0, 1]
              scaling: [1, 1, 1]
        - component:
            type: perspective camera component
            data:
             {}
        - component:
            type: mesh component
            data:
              mesh resource: TODO
        - component:
            type: lua script component
            data:
             {}
        - component:
            type: audio component
            data:
              audio resource: ''
              type: 0
              current sample: 184102
              spatialized: true
      children:
        - object:
            uuid: E1C364A925D649408E83C8EEF5179A87
            components:
              - component:
                  type: name component
                  data:
                    object name: Prepend
            children:
              []
        - object:
            uuid: 377DBA885AF4CD42B8A56BB3471F60E5
            components:
              - component:
                  type: name component
                  data:
                    object name: pivot
            children:
              []
        - object:
            uuid: 6DD1835797DADB4F95232CE7E9DE41BA
            components:
              - component:
                  type: name component
                  data:
                    object name: Append
            children:
              []
)",
  L{N("universe", L{
        N("objects", L{
            N("object", L{
                N("uuid", "A7AB039C0EF3A74480A1B398247039A7"),
                N("components", L{
                    N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Root Node")}), }), }),
                    N(L{N("component", L{N("type", "transform component"), N("data", L{N("translation", L{N("-2"), N("-2"), N("5")}), N("rotation", L{N("0"), N("0"), N("0"), N("1")}), N("scaling", L{N("1"), N("1"), N("1")}),}), }), }),
                    N(L{N("component", L{N("type", "perspective camera component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "mesh component"), N("data", L{N("mesh resource", "TODO")}), }), }),
                    N(L{N("component", L{N("type", "lua script component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "audio component"), N("data", L{N("audio resource", ""), N("type", "0"), N("current sample", "184102"), N("spatialized", "true"), }), }), }), // component
                  }), // components
                N("children", L{
                    N(L{N("object", L{
                        N("uuid", "E1C364A925D649408E83C8EEF5179A87"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Prepend")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "377DBA885AF4CD42B8A56BB3471F60E5"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "pivot")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "6DD1835797DADB4F95232CE7E9DE41BA"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Append")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                  }), // children
                }), // object
              }) // objects
          }) // universe
      }
),

C("github6",
R"(videos:
  - UQxRibHKEDI:
    - UQxRibHKEDI.mp4
    - UQxRibHKEDI.mp4
    - UQxRibHKEDI.mp4
    - UQxRibHKEDI.mp4
  - DcYsg8VFdC0:
    - DcYsg8VFdC0.mp4
    - DcYsg8VFdC0.mp4
    - DcYsg8VFdC0.mp4
    - DcYsg8VFdC0.mp4
  - Yt3ymqZXzLY:
    - Yt3ymqZXzLY.mp4
    - Yt3ymqZXzLY.mp4
    - Yt3ymqZXzLY.mp4
    - Yt3ymqZXzLY.mp4
)",
L{N("videos", L{
N("UQxRibHKEDI", L{N("UQxRibHKEDI.mp4"), N("UQxRibHKEDI.mp4"), N("UQxRibHKEDI.mp4"), N("UQxRibHKEDI.mp4")}),
N("DcYsg8VFdC0", L{N("DcYsg8VFdC0.mp4"), N("DcYsg8VFdC0.mp4"), N("DcYsg8VFdC0.mp4"), N("DcYsg8VFdC0.mp4")}),
N("Yt3ymqZXzLY", L{N("Yt3ymqZXzLY.mp4"), N("Yt3ymqZXzLY.mp4"), N("Yt3ymqZXzLY.mp4"), N("Yt3ymqZXzLY.mp4")}),
})}
),

//-----------------------------------------------------------------------------

    }); // ends the cases map

//-----------------------------------------------------------------------------

    auto it = cases.find(name);
    C4_ASSERT(it != cases.end());
    return &it->second;
}

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

INSTANTIATE_TEST_CASE_P(empty_files   , YmlTestCase, ::testing::Values(EMPTY_FILE_CASES));
INSTANTIATE_TEST_CASE_P(docs_empty    , YmlTestCase, ::testing::Values(EMPTY_DOC_CASES));
INSTANTIATE_TEST_CASE_P(docs_simple   , YmlTestCase, ::testing::Values(SIMPLE_DOC_CASES));

INSTANTIATE_TEST_CASE_P(maps_empty    , YmlTestCase, ::testing::Values(EMPTY_MAP_CASES));
INSTANTIATE_TEST_CASE_P(seqs_empty    , YmlTestCase, ::testing::Values(EMPTY_SEQ_CASES));

INSTANTIATE_TEST_CASE_P(maps_simple   , YmlTestCase, ::testing::Values(SIMPLE_MAP_CASES));
INSTANTIATE_TEST_CASE_P(seqs_simple   , YmlTestCase, ::testing::Values(SIMPLE_SEQ_CASES));

INSTANTIATE_TEST_CASE_P(double_quotes , YmlTestCase, ::testing::Values(DOUBLE_QUOTED_CASES));
INSTANTIATE_TEST_CASE_P(single_quotes , YmlTestCase, ::testing::Values(SINGLE_QUOTED_CASES));
INSTANTIATE_TEST_CASE_P(plain_scalars , YmlTestCase, ::testing::Values(PLAIN_SCALAR_CASES));

INSTANTIATE_TEST_CASE_P(literal_blocks, YmlTestCase, ::testing::Values(BLOCK_LITERAL_CASES));
INSTANTIATE_TEST_CASE_P(folded_blocks , YmlTestCase, ::testing::Values(BLOCK_FOLDED_CASES));

INSTANTIATE_TEST_CASE_P(tag_properties, YmlTestCase, ::testing::Values(TAG_PROPERTY_CASES));

INSTANTIATE_TEST_CASE_P(complex_keys  , YmlTestCase, ::testing::Values(COMPLEX_KEY_CASES));

INSTANTIATE_TEST_CASE_P(maps_nested2  , YmlTestCase, ::testing::Values(NESTED_MAPX2_CASES));
INSTANTIATE_TEST_CASE_P(seqs_nested2  , YmlTestCase, ::testing::Values(NESTED_SEQX2_CASES));

INSTANTIATE_TEST_CASE_P(maps_nested3  , YmlTestCase, ::testing::Values(NESTED_MAPX3_CASES));
INSTANTIATE_TEST_CASE_P(seqs_nested3  , YmlTestCase, ::testing::Values(NESTED_SEQX3_CASES));

INSTANTIATE_TEST_CASE_P(maps_nested4  , YmlTestCase, ::testing::Values(NESTED_MAPX4_CASES));
INSTANTIATE_TEST_CASE_P(seqs_nested4  , YmlTestCase, ::testing::Values(NESTED_SEQX4_CASES));

INSTANTIATE_TEST_CASE_P(map_of_seqs   , YmlTestCase, ::testing::Values(MAP_OF_SEQ_CASES));
INSTANTIATE_TEST_CASE_P(seq_of_maps   , YmlTestCase, ::testing::Values(SEQ_OF_MAP_CASES));

INSTANTIATE_TEST_CASE_P(maps_generic  , YmlTestCase, ::testing::Values(GENERIC_MAP_CASES));
INSTANTIATE_TEST_CASE_P(seqs_generic  , YmlTestCase, ::testing::Values(GENERIC_SEQ_CASES));

INSTANTIATE_TEST_CASE_P(simple_anchors, YmlTestCase, ::testing::Values(SIMPLE_ANCHOR_CASES));

INSTANTIATE_TEST_CASE_P(indentation   , YmlTestCase, ::testing::Values(INDENTATION_CASES));

INSTANTIATE_TEST_CASE_P(numbers       , YmlTestCase, ::testing::Values(NUMBER_CASES));
INSTANTIATE_TEST_CASE_P(null_vals     , YmlTestCase, ::testing::Values(NULL_VAL_CASES));

INSTANTIATE_TEST_CASE_P(github_issues , YmlTestCase, ::testing::Values(GITHUB_ISSUE_CASES));


#pragma GCC diagnostic pop

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

} // namespace yml
} // namespace c4
