#ifndef _TEST_CASE_HPP_
#define _TEST_CASE_HPP_


#include <vector>
#include <map>

#include "./libyaml.hpp"

#include <ryml/ryml.hpp>


namespace c4 {
namespace yml {

struct Case;
struct CaseNode;
struct CaseData;

Case const* get_case(cspan name);
CaseData* get_data(cspan name);

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
struct CaseNode
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

//-----------------------------------------------------------------------------

// a persistent data store to avoid repeating operations on every test
struct CaseData
{
    LibyamlParser libyaml_parser;
    Tree parsed_tree;

    size_t numbytes_stdout;
    std::vector< char > emit_buf;
    cspan emitted_yml;

    Tree emitted_tree;

    Tree recreated;
};

} // namespace yml
} // namespace c4

#endif /* _TEST_CASE_HPP_ */
