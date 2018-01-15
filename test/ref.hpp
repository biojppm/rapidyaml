#ifndef _CASE_HPP_
#define _CASE_HPP_


#include <vector>
#include <map>
#include <iostream>

#include <ryml/ryml.hpp>



namespace c4 {
namespace yml {

#define C4_EXPECT_IMPL_(relname, val1, cmp, val2) \
    if( ! ((val1) cmp (val2))) \
    {\
        std::cout << "\n"                                               \
                  << __FILE__ ":" << __LINE__ << ": ERROR: [" << current_case << "]:\n" \
                  <<                             ":          expected " #relname " (" #cmp "):\n" \
                  <<                             ":          lhs: '" #val1 "'=" << val1 << "\n" \
                  <<                             ":          rhs: '" #val2 "'=" << val2 << "\n"; \
        current_status = false;                                         \
    }

#define C4_EXPECT_EQ(val1, val2) C4_EXPECT_IMPL_(eq, val1, ==, val2)


bool current_status;
cspan current_case;

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

public:

    CaseNode(CaseNode     &&) = default;
    CaseNode(CaseNode const&) = default;

    CaseNode& operator= (CaseNode     &&) = default;
    CaseNode& operator= (CaseNode const&) = default;

public:

    CaseNode() : type(TYPE_NONE), key(), val(), children() {}

    template< size_t N >
    explicit CaseNode(const char (&v)[N]) : type(TYPE_VAL), key(), val(v), children() {}

    template< size_t N, size_t M >
    explicit CaseNode(const char (&k)[N], const char (&v)[M]) : type(TYPE_VAL), key(k), val(v), children() {}

    template< size_t N >
    CaseNode(const char (&k)[N], children_init_type s) : type(), key(k), val(), children(s) { type = guess(); }
    explicit CaseNode(children_init_type m) : CaseNode("", m) {}

    template< size_t N >
    explicit CaseNode(NodeType_e t, const char (&k)[N], children_init_type s) : type(t), key(k), val(), children(s) {}
    explicit CaseNode(NodeType_e t, children_init_type m) : CaseNode(t, "", m) {}

public:

    NodeType_e guess() const
    {
        C4_ASSERT(val.empty() != children.empty());
        if(children.empty())
        {
            return TYPE_VAL;
        }
        else
        {
            auto const& ch = children.front();
            if(ch.key.empty())
            {
                return TYPE_SEQ;
            }
            else
            {
                return TYPE_MAP;
            }
        }
    }

    bool is_doc() const { return type == TYPE_DOC; }
    bool is_val() const { return type == TYPE_VAL && key.empty(); }
    bool is_key_val() const { return type == TYPE_VAL && ! key.empty(); }
    bool is_map() const { return type == TYPE_MAP || (type == TYPE_DOC && ! children.empty() && ! children.front().key.empty() ); }
    bool is_seq() const { return type == TYPE_MAP || (type == TYPE_DOC && ! children.empty() &&   children.front().key.empty() ); }

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

    void compare(yml::Node const& n) const
    {
        C4_EXPECT_EQ(n.type(), type);
        C4_EXPECT_EQ(n.num_children(), children.size());
        for(size_t i = 0, ei = n.num_children(), j = 0, ej = children.size(); i < ei && j < ej; ++i, ++j)
        {
             children[j].compare(n[i]);
        }
    }
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

    void run() const
    {
        std::cout << "\n\n\n\n\n";
        std::cout << "---------------------------------------------------\n";
        std::cout << "Running test case: '" << name << "'\n";
        std::cout << "---------------------------------------------------\n";
        NextParser np;
        Tree t = np.parse(src);

        current_status = true;
        current_case = name;

        root.compare(*t.root());

        cspan stat = current_status ? "succeeded!!!! :-)" :  "failed.... :-( ";
        std::cout << "Test case '" << name << "' " << stat << "\n";
        std::cout << "---------------------------------------------------\n";
        C4_ASSERT(true);
    }
};


struct CaseContainer : public std::vector< Case > // yeah I know.
{

};

} // namespace yml
} // namespace c4

#endif /* _CASE_HPP_ */
