#ifndef _CASE_HPP_
#define _CASE_HPP_


#include <vector>
#include <map>
#include <iostream>

#include "./libyaml.hpp"

#include <ryml/ryml.hpp>
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

struct Case;
class CaseNode;

void check_invariants(Node const& n);

void show_children(Node const& p);

void print_node(Node const& p, int level=0, bool print_children=false);
void print_tree(Node const& p, int level=0);
void print_tree(Tree const& t);

void print_node(CaseNode const& t, int level = 0);
void print_tree(CaseNode const& p, int level = 0);


void print_path(Node const& p);



/** a node class against which ryml structures are tested. Uses initializer
 * lists to facilitate minimal specification. */
class CaseNode
{
public:

    using  seqmap = std::vector< CaseNode >;
    using iseqmap = std::initializer_list< CaseNode >;

public:

    NodeType_e type;
    cspan      key;
    cspan      val;
    seqmap     children;
    CaseNode * parent;

public:

    CaseNode() : CaseNode(NOTYPE) {}
    CaseNode(NodeType_e t) : type(t), key(), val(), children(), parent(nullptr) { _set_parent(); }

    template< size_t N >
    explicit CaseNode(const char (&v)[N]) : type(VAL), key(), val(v), children(), parent(nullptr) { _set_parent(); }

    template< size_t N, size_t M >
    explicit CaseNode(const char (&k)[N], const char (&v)[M]) : type(KEYVAL), key(k), val(v), children(), parent(nullptr) { _set_parent(); }

    template< size_t N >
    explicit CaseNode(const char (&k)[N], iseqmap s) : type(), key(k), val(), children(s), parent(nullptr) { _set_parent(); type = _guess(); }
    explicit CaseNode(                    iseqmap m) : CaseNode("", m) {}

    template< size_t N >
    explicit CaseNode(NodeType_e t, const char (&k)[N], iseqmap s) : type(t), key(k), val(), children(s), parent(nullptr) { _set_parent(); }
    explicit CaseNode(NodeType_e t,                     iseqmap m) : CaseNode(t, "", m) {}

    CaseNode(CaseNode     &&) = default;
    CaseNode(CaseNode const&) = default;

    CaseNode& operator= (CaseNode     &&) = default;
    CaseNode& operator= (CaseNode const&) = default;

public:

    void _set_parent()
    {
        for(auto &ch : children)
        {
            ch.parent = this;
        }
    }

    NodeType_e _guess() const
    {
        C4_ASSERT(val.empty() != children.empty());
        if(children.empty())
        {
            C4_ASSERT(parent);
            if(key.empty())
            {
                return VAL;
            }
            else
            {
                return KEYVAL;
            }
        }
        else
        {
            NodeType_e has_key = key.empty() ? NOTYPE : KEY;
            auto const& ch = children.front();
            if(ch.key.empty())
            {
                return ((NodeType_e)(has_key|SEQ));
            }
            else
            {
                return ((NodeType_e)(has_key|MAP));
            }
        }
    }

    bool is_root() const { return parent; }
    bool is_doc() const { return type & DOC; }
    bool is_map() const { return type & MAP; }
    bool is_seq() const { return type & SEQ; }
    bool has_val() const { return type & VAL; }
    bool has_key() const { return type & KEY; }
    bool is_container() const { return type & (SEQ|MAP); }

public:

    CaseNode const& operator[] (size_t i) const
    {
        C4_ASSERT(i >= 0 && i < children.size());
        return children[i];
    }

    CaseNode const& operator[] (cspan const& name) const
    {
        auto ch = lookup(name);
        C4_ASSERT(ch != nullptr);
        return *ch;
    }

    CaseNode const* lookup(cspan const& name) const
    {
        C4_ASSERT( ! children.empty());
        for(auto const& ch : children)
        {
            if(ch.key == name)
            {
                return &ch;
            }
        }
        return nullptr;
    }

public:

    void compare(yml::Node const& n) const;

    size_t reccount() const
    {
        size_t c = 1;
        for(auto const& ch : children)
        {
            c += ch.reccount();
        }
        return c;
    }

    void recreate(yml::Node *n) const;

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct Case
{
    cspan name;
    cspan src;
    CaseNode root;

    template< size_t N, class... Args >
    Case(cspan const& n, const char (&s)[N], Args&& ...args)
        : name(n), src(s), root(std::forward< Args >(args)...)
    {
    }

};


void CaseNode::compare(yml::Node const& n) const
{
    EXPECT_EQ(n.type(), type);
    EXPECT_EQ(n.num_children(), children.size());
    if(n.has_key())
    {
        EXPECT_EQ(n.key(), key);
    }
    if(n.is_val())
    {
        EXPECT_EQ(n.val(), val);
    }

    // check that the children are in the same order
    {
        EXPECT_EQ(children.size(), n.num_children());
        size_t ic = 0;
        for(auto const& ch : children)
        {
            EXPECT_TRUE(ic < n.num_children());
            if(ic >= n.num_children()) break;
            EXPECT_TRUE(n.child(ic) != nullptr);
            if(type & MAP)
            {
                EXPECT_NE(n.find_child(ch.key), nullptr);
                auto fch = n.find_child(ch.key);
                if(fch)
                {
                    EXPECT_EQ(&n[ic], &n[ch.key]);
                    EXPECT_EQ(n[ch.key].key(), ch.key);
                    if(ch.type & VAL)
                    {
                        EXPECT_EQ(n[ch.key].val(), ch.val);
                    }
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
                EXPECT_EQ(n[ic].m_key, children[ic].key);
                if(ch.type & VAL)
                {
                    EXPECT_EQ(n[ic].val(), ch.val);
                }
            }
            if(ch.type & KEY)
            {
                EXPECT_TRUE(n[ic].has_key());
                EXPECT_EQ(n[ic].key(), ch.key);
            }
            if(ch.type & VAL)
            {
                EXPECT_TRUE(n[ic].has_val());
                EXPECT_EQ(n[ic].val(), ch.val);
            }
            ++ic;
        }
    }

    for(size_t i = 0, ei = n.num_children(), j = 0, ej = children.size(); i < ei && j < ej; ++i, ++j)
    {
        children[j].compare(n[i]);
    }
}

void CaseNode::recreate(yml::Node *n) const
{
    C4_ASSERT( ! n->has_children());
    n->m_type = type;
    n->m_key = key;
    n->m_val = val;
    auto &tree = *n->tree();
    size_t nid = n->id(); // don't use node from now on
    for(auto const& ch : children)
    {
        size_t id = tree.append_child(nid);
        Node * chn = tree.get(id);
        ch.recreate(chn);
    }
}

//-----------------------------------------------------------------------------

void print_path(Node const& n)
{
    size_t len = 0;
    char buf[1024];
    Node const *p = &n;
    while(p)
    {
        if(p->has_key())
        {
            len += 1 + p->key().len;
        }
        else
        {
            len += snprintf(buf, sizeof(buf), "/%zd", p->parent() ? p->parent()->child_pos(p) : 0);
        }
        p = p->parent();
    };
    C4_ASSERT(len < sizeof(buf));
    size_t pos = len;
    while(p)
    {
        if(p->has_key())
        {
            size_t tl = p->key().len;
            pos -= snprintf(buf + pos - tl, tl, "%.*s", (int)tl, p->key().str);
        }
        else
        {
            size_t pos = p->parent()->child_pos(p);
            size_t tl = snprintf(buf, 0, "/%zd", pos);
            pos -= snprintf(buf + pos - tl, tl, "/%zd", pos);
        }
        p = p->parent();
    };
    printf("%.*s", (int)len, buf);
}


void print_node(Node const& p, int level, bool print_children)
{
    printf("%*s[%zd] %p", (2*level), "", p.id(), (void*)&p);
    if( ! p.parent())
    {
        printf(" [ROOT]");
    }
    printf(" %s:", p.type_str());
    if(p.has_key())
    {
        printf(" '%.*s'", (int)p.key().len, p.key().str);
    }
    if(p.has_val())
    {
        printf(" '%.*s'", (int)p.val().len, p.val().str);
    }
    printf(" (%zd sibs)", p.num_siblings());
    if(p.is_container())
    {
        printf(" %zd children:", p.num_children());
        if(print_children)
        {
            for(Node const* ch = p.first_child(); ch; ch = ch->next_sibling())
            {
                print_node(*ch, level+1);
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
    printf(" %s:", Node::type_str(p.type));
    if(p.has_key())
    {
        printf(" '%.*s'", (int)p.key.len, p.key.str);
    }
    if(p.has_val())
    {
        printf(" '%.*s'", (int)p.val.len, p.val.str);
    }
    printf(" (%zd sibs)", p.parent ? p.parent->children.size() : 0);
    if(p.is_container())
    {
        printf(" %zd children:", p.children.size());
    }
    printf("\n");
}


void print_tree(Node const& p, int level)
{
    print_node(p, level);
    for(Node const* ch = p.first_child(); ch; ch = ch->next_sibling())
    {
        print_tree(*ch, level+1);
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
    print_tree(*t.root());
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

void check_invariants(Node const& n)
{
    EXPECT_NE(n.is_root(), n.has_siblings());
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
    for(Node const* s = n.first_sibling(); s; s = s->next_sibling())
    {
        EXPECT_TRUE(n.has_sibling(s));
        EXPECT_TRUE(s->has_sibling(&n));
        EXPECT_EQ(s->parent(), n.parent());
    }
    if(n.parent())
    {
        EXPECT_TRUE(n.parent()->has_child(&n));
        EXPECT_EQ(n.parent()->num_children(), n.num_siblings());
        // doc parent must be a seq and a stream
        if(n.is_doc())
        {
            EXPECT_TRUE(n.parent()->is_seq());
            EXPECT_TRUE(n.parent()->is_stream());
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
        for(Node const* ch = n.first_child(); ch; ch = ch->next_sibling())
        {
            EXPECT_FALSE(ch->is_keyval());
            EXPECT_FALSE(ch->has_key());
        }
    }
    if(n.is_map())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_seq());
        for(Node const* ch = n.first_child(); ch; ch = ch->next_sibling())
        {
            EXPECT_TRUE(ch->has_key());
        }
    }
    // ... add more tests here

    // now recurse into the children
    for(Node const* ch = n.first_child(); ch; ch = ch->next_sibling())
    {
        check_invariants(*ch);
    }
}

} // namespace yml
} // namespace c4

#endif /* _CASE_HPP_ */
