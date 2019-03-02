#include "./test_case.hpp"
#include "c4/span.hpp"
#include "c4/yml/std/std.hpp"

#include <gtest/gtest.h>

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
    nd->m_type = type|key_anchor.type|val_anchor.type;
    nd->m_key.scalar = key;
    nd->m_key.tag = (key_tag);
    nd->m_key.anchor = key_anchor.str;
    nd->m_val.scalar = val;
    nd->m_val.tag = (val_tag);
    nd->m_val.anchor = val_anchor.str;
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
        if(p.has_key_anchor())
        {
            csubstr const& ka = p.key_anchor();
            printf(" &%.*s", (int)ka.len, ka.str);
        }
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
    if(p.has_val_anchor())
    {
        auto &a = p.val_anchor();
        printf(" valanchor='&%.*s'", (int)a.len, a.str);
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
        if(p.has_key_anchor())
        {
            csubstr const& ka = p.key_anchor.str;
            printf(" &%.*s", (int)ka.len, ka.str);
        }
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
    if(p.has_val_anchor())
    {
        auto &a = p.val_anchor.str;
        printf(" valanchor='&%.*s'", (int)a.len, a.str);
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
    if(n.has_key_anchor())
    {
        EXPECT_FALSE(n.key_anchor().empty());
        EXPECT_FALSE(n.is_key_ref());
    }
    if(n.has_val_anchor())
    {
        EXPECT_FALSE(n.val_anchor().empty());
        EXPECT_FALSE(n.is_val_ref());
    }
    if(n.is_key_ref())
    {
        EXPECT_FALSE(n.key_ref().empty());
        EXPECT_FALSE(n.has_key_anchor());
    }
    if(n.is_val_ref())
    {
        EXPECT_FALSE(n.val_ref().empty());
        EXPECT_FALSE(n.has_val_anchor());
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
