#ifndef _CASE_HPP_
#define _CASE_HPP_


#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "./libyaml.hpp"

#include <ryml/ryml.hpp>


namespace c4 {
namespace yml {

struct Case;
class CaseNode;

void check_invariants(Node const& n);

void show_children(Node const& p);

void print_tree(Node const& p, int level = 0);
void print_tree(Tree const& t);

void print_tree(CaseNode const& p, int level);
void print_tree(CaseNode const& t);



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
            auto const& ch = children.front();
            if(ch.key.empty())
            {
                return SEQ;
            }
            else
            {
                return MAP;
            }
        }
    }

    bool is_root() const { return parent; }
    bool is_doc() const { return type & DOC; }
    bool is_val() const { return type & VAL; }
    bool is_map() const { return type & MAP; }
    bool is_seq() const { return type & SEQ; }

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

    void run() const;

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct CaseContainer
{
    std::vector< Case > tests;

    explicit CaseContainer(std::initializer_list< Case > il) : tests(il) {}

    bool run()
    {
        failed_tests.clear();

        for(auto &c : tests)
        {
            std::cout << "\n\n\n\n\n";
            std::cout << "---------------------------------------------------\n";
            std::cout << "Running test case: '" << c.name << "'\n";
            std::cout << "---------------------------------------------------\n";

            current_status = true;
            current_case = c.name;

            c.run();

            cspan stat = current_status ? "succeeded!!!! :-)" :  "failed.... :-( ";
            std::cout << "---------------------------------------------------\n";
            std::cout << "Test case '" << c.name << "' " << stat << "\n";
            std::cout << "---------------------------------------------------\n";
        }

        if( ! failed_tests.empty())
        {
            std::cout << "\n\n\n";
            std::cout << "===================================================\n";
            std::cout << "Failed tests:\n";
            for(auto const& t : failed_tests)
            {
                std::cout << "'" << t << "'\n";
            }
            std::cout << "===================================================\n";
        }

        return failed_tests.empty();
    }

    static void test_failed()
    {
        current_status = false;
        cspan back;
        if( ! failed_tests.empty())
        {
            back = failed_tests.back().c_str();
        }
        if(current_case != back)
        {
            failed_tests.emplace_back(current_case.str, current_case.len);
        }
    }
    static bool current_status;
    static cspan current_case;
    static std::vector< std::string > failed_tests;
};


bool CaseContainer::current_status;
cspan CaseContainer::current_case;
std::vector< std::string > CaseContainer::failed_tests;



#define C4_EXPECT_IMPL_(relname, val1, cmp, val2)                       \
    if( ! ((val1) cmp (val2)))                                          \
    {                                                                   \
        std::cout << "\n"                                               \
                  << __FILE__ ":" << __LINE__ << ": ERROR: [" << ::c4::yml::CaseContainer::current_case << "]:\n" \
                  << "    expected " #relname " (" #cmp "):\n"          \
                  << "    lhs: '" #val1 "'='" << val1 << "'\n"          \
                  << "    "#cmp"\n"                                     \
                  << "    rhs: '" #val2 "'='" << val2 << "'\n";         \
        ::c4::yml::CaseContainer::test_failed();                        \
    }

#define C4_EXPECT_EQ(val1, val2) C4_EXPECT_IMPL_(eq, val1, ==, val2)
#define C4_EXPECT_NE(val1, val2) C4_EXPECT_IMPL_(ne, val1, !=, val2)
#define C4_EXPECT_GE(val1, val2) C4_EXPECT_IMPL_(ge, val1, >=, val2)
#define C4_EXPECT_GT(val1, val2) C4_EXPECT_IMPL_(gt, val1, > , val2)
#define C4_EXPECT_LE(val1, val2) C4_EXPECT_IMPL_(le, val1, <=, val2)
#define C4_EXPECT_LT(val1, val2) C4_EXPECT_IMPL_(lt, val1, < , val2)

#define C4_EXPECT(expr)     C4_EXPECT_IMPL_(eq, expr, ==, true)
#define C4_EXPECT_NOT(expr) C4_EXPECT_IMPL_(eq, expr, ==, false)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void Case::run() const
{
    std::cout << "ref src:\n";
    std::cout << "--------------------------------------\n";
    std::cout << src << "\n";
    std::cout << "--------------------------------------\n";
    std::cout << "ref tree:\n";
    print_tree(root);

    std::cout << "parsing using libyaml to check if the YAML source is legal\n";
    LibyamlParser libyaml_parser;
    libyaml_parser.parse(src);
    std::cout << "parsing using libyaml: done\n";

    std::cout << "parsing using ryml\n";
    Tree t = parse(src);
    std::cout << "parsing using ryml: done\n";

    std::cout << "parsed tree:\n";
    print_tree(t);
    std::cout << "parsed tree: end.\n";

    std::cout << "emit yml:\n";
    emit(t);
    std::cout << "emit yml: end.\n";

    std::cout << "checking invariants...\n";
    check_invariants(*t.root());
    std::cout << "checking invariants: done\n";

    std::cout << "comparing parsed tree to ref tree...\n";
    C4_EXPECT_GE(t.capacity(), root.reccount());
    C4_EXPECT_EQ(t.size(), root.reccount());
    root.compare(*t.root());
    std::cout << "comparing parsed tree to ref tree: done\n";

    std::cout << "recreating a new tree from the ref tree\n";
    Tree dup;
    dup.reserve(t.size());
    root.recreate(dup.root());
    std::cout << "recreating a new tree from the ref tree: done\n";
}

void CaseNode::compare(yml::Node const& n) const
{
    C4_EXPECT_EQ(n.type(), type);
    C4_EXPECT_EQ(n.num_children(), children.size());
    if(n.has_key())
    {
        C4_EXPECT_EQ(n.key(), key);
    }
    if(n.is_val())
    {
        C4_EXPECT_EQ(n.val(), val);
    }

    // check that the children are in the same order
    {
        size_t ic = 0;
        for(auto const& ch : children)
        {
            if(type & MAP)
            {
                C4_EXPECT_EQ(&n[ic], &n[ch.key]);
                C4_EXPECT_EQ(n[ch.key].key(), ch.key);
                if(ch.type & VAL)
                {
                    C4_EXPECT_EQ(n[ch.key].val(), ch.val);
                }
            }
            if(type & SEQ)
            {
                C4_EXPECT_EQ(n[ic].m_key, children[ic].key);
                if(ch.type & VAL)
                {
                    C4_EXPECT_EQ(n[ic].val(), ch.val);
                }
            }
            if(ch.type & KEY)
            {
                C4_EXPECT(n[ic].has_key());
                C4_EXPECT_EQ(n[ic].key(), ch.key);
            }
            if(ch.type & VAL)
            {
                C4_EXPECT(n[ic].has_val());
                C4_EXPECT_EQ(n[ic].val(), ch.val);
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
    if( ! parent)
    {
        n->m_key = key;
        n->m_val = val;
    }
    for(auto const& ch : children)
    {
        size_t id = n->tree()->append_child(n->id());
        Node * chn = n->tree()->get(id);
        ch.recreate(chn);
    }
}

//-----------------------------------------------------------------------------



void print_node(Node const& p, int level=0)
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
    if(p.is_val())
    {
        printf(" '%.*s'", (int)p.val().len, p.val().str);
    }
    printf(" (%zd sibs)", p.num_siblings());
    if(p.is_container())
    {
        printf(" %zd children:", p.num_children());
    }
    printf("\n");
}



void print_node(CaseNode const& p, int level=0)
{
    printf("%*s%p", (2*level), "", (void*)&p);
    if( ! p.parent)
    {
        printf(" [ROOT]");
    }
    printf(" %s:", Node::type_str(p.type));
    if(p.type & KEY)
    {
        printf(" '%.*s'", (int)p.key.len, p.key.str);
    }
    if(p.is_val())
    {
        printf(" '%.*s'", (int)p.val.len, p.val.str);
    }
    printf(" (%zd sibs)", p.parent ? p.parent->children.size() : 0);
    if(p.type & (SEQ|MAP))
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
    C4_EXPECT_NE(n.is_root(), n.has_siblings());
    // keys or vals cannot be root
    if(n.has_key() || n.is_val() || n.is_keyval())
    {
        C4_EXPECT_NOT(n.is_root());
        C4_EXPECT_NOT(n.is_root());
        C4_EXPECT_NOT(n.is_root());
    }
    // vals cannot be containers
    C4_EXPECT_NE(n.has_val(), n.is_container());
    if(n.has_children())
    {
        C4_EXPECT(n.is_container());
        C4_EXPECT_NOT(n.is_val());
    }
    // check parent & sibling reciprocity
    for(Node const* s = n.first_sibling(); s; s = s->next_sibling())
    {
        C4_EXPECT(n.has_sibling(s));
        C4_EXPECT(s->has_sibling(&n));
        C4_EXPECT_EQ(s->parent(), n.parent());
    }
    if(n.parent())
    {
        C4_EXPECT(n.parent()->has_child(&n));
        C4_EXPECT_EQ(n.parent()->num_children(), n.num_siblings());
        // doc parent must be a seq and a stream
        if(n.is_doc())
        {
            C4_EXPECT(n.parent()->is_seq());
            C4_EXPECT(n.parent()->is_stream());
        }
    }
    else
    {
        C4_EXPECT(n.is_root());
    }
    if(n.is_seq())
    {
        C4_EXPECT(n.is_container());
        C4_EXPECT_NOT(n.is_map());
        for(Node const* ch = n.first_child(); ch; ch = ch->next_sibling())
        {
            C4_EXPECT_NOT(ch->is_keyval());
            C4_EXPECT_NOT(ch->has_key());
        }
    }
    if(n.is_map())
    {
        C4_EXPECT(n.is_container());
        C4_EXPECT_NOT(n.is_seq());
        for(Node const* ch = n.first_child(); ch; ch = ch->next_sibling())
        {
            C4_EXPECT(ch->has_key());
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
