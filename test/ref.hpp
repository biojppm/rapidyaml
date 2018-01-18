#ifndef _CASE_HPP_
#define _CASE_HPP_


#include <vector>
#include <map>
#include <iostream>

#include <ryml/ryml.hpp>



namespace c4 {
namespace yml {

struct Case;

/** a node class against which ryml structures are tested. Uses initializer
 * lists to facilitate minimal specification. */
class CaseNode
{
public:

    using children_type = std::vector< CaseNode >;
    using children_init_type = std::initializer_list< CaseNode >;

public:

    NodeType_e type;
    cspan key;
    cspan val;
    children_type children;
    CaseNode *parent;

public:

    CaseNode() : type(NOTYPE), key(), val(), children(), parent() {}
    CaseNode(NodeType_e t) : type(t), key(), val(), children(), parent() {}

    template< size_t N >
    explicit CaseNode(const char (&v)[N]) : type(VAL), key(), val(v), children() {}

    template< size_t N, size_t M >
    explicit CaseNode(const char (&k)[N], const char (&v)[M]) : type(VAL), key(k), val(v), children() {}

    template< size_t N >
    explicit CaseNode(const char (&k)[N], children_init_type s) : type(), key(k), val(), children(s) { _set_parent(); type = _guess(); }
    explicit CaseNode(                    children_init_type m) : CaseNode("", m) {}

    template< size_t N >
    explicit CaseNode(NodeType_e t, const char (&k)[N], children_init_type s) : type(t), key(k), val(), children(s) { _set_parent(); }
    explicit CaseNode(NodeType_e t,                     children_init_type m) : CaseNode(t, "", m) {}

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
            return VAL;
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

    /*
    void print(int level = 0) const
    {
        if(is_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        else if(is_key_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        ++level;
        if(is_seq())
        {
            printf("\n");
            for(auto const& c : seq)
            {
                printf("%*s", level, "  ");
                printf("%.*s", (int)key.len, key.str);
                c.print(level);
            }
        }
        else if(is_map())
        {
            ++level;
            for(auto const& c : map)
            {
                c.print(level);
            }
        }
    }
*/

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

    void run()
    {
        failed_tests.clear();

        for(auto &c : tests)
        {
            std::cout << "\n\n\n\n\n";
            std::cout << "---------------------------------------------------\n";
            std::cout << "Running test case: '" << c.name << "'\n";
            std::cout << "---------------------------------------------------\n";
            std::cout << "input yml:\n'" << c.src << "'\n";
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
                std::cout << t << "\n";
            }
            std::cout << "===================================================\n";
        }
    }

    static void test_failed()
    {
        current_status = false;
        if(failed_tests.empty() || failed_tests.back() != current_case)
        {
            failed_tests.push_back(current_case);
        }
    }
    static bool current_status;
    static cspan current_case;
    static std::vector< cspan > failed_tests;
};


bool CaseContainer::current_status;
cspan CaseContainer::current_case;
std::vector< cspan > CaseContainer::failed_tests;



#define C4_EXPECT_IMPL_(relname, val1, cmp, val2)                       \
    if( ! ((val1) cmp (val2)))                                          \
    {                                                                   \
        std::cout << "\n"                                               \
                  << __FILE__ ":" << __LINE__ << ": ERROR: [" << ::c4::yml::CaseContainer::current_case << "]:\n" \
                  << "    expected " #relname " (" #cmp "):\n"          \
                  << "    lhs: '" #val1 "'=" << val1 << "\n"            \
                  << "    "#cmp"\n"                                     \
                  << "    rhs: '" #val2 "'=" << val2 << "\n";           \
        ::c4::yml::CaseContainer::test_failed();                        \
    }

#define C4_EXPECT_EQ(val1, val2) C4_EXPECT_IMPL_(eq, val1, ==, val2)
#define C4_EXPECT_GE(val1, val2) C4_EXPECT_IMPL_(ge, val1, >=, val2)
#define C4_EXPECT_GT(val1, val2) C4_EXPECT_IMPL_(gt, val1, > , val2)
#define C4_EXPECT_LE(val1, val2) C4_EXPECT_IMPL_(le, val1, <=, val2)
#define C4_EXPECT_LT(val1, val2) C4_EXPECT_IMPL_(lt, val1, < , val2)


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void Case::run() const
{
    Parser libyaml_parser;
    Tree libyaml_tree;

    std::cout << "parsing using libyaml\n";
    libyaml_parser.parse(&libyaml_tree, src);
    emit(libyaml_tree);
    std::cout << "parsing using libyaml: done\n";

    std::cout << "parsing using ryml\n";
    Tree t = parse(src);
    emit(t);
    std::cout << "parsing using ryml: done\n";

    std::cout << "comparing trees...\n";
    C4_EXPECT_GE(t.capacity(), root.reccount());
    C4_EXPECT_EQ(t.size(), root.reccount());
    root.compare(*t.root());
    std::cout << "comparing trees: done\n";
}

void CaseNode::compare(yml::Node const& n) const
{
    C4_EXPECT_EQ(n.type(), type);
    C4_EXPECT_EQ(n.num_children(), children.size());
    C4_EXPECT_EQ(n.name(), key);
    if(n.is_val())
    {
        C4_EXPECT_EQ(n.val(), val);
    }
    for(size_t i = 0, ei = n.num_children(), j = 0, ej = children.size(); i < ei && j < ej; ++i, ++j)
    {
        children[j].compare(n[i]);
    }
}

} // namespace yml
} // namespace c4

#endif /* _CASE_HPP_ */
