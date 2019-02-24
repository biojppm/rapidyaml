#ifndef _TEST_CASE_HPP_
#define _TEST_CASE_HPP_

#include <vector>
#include <map>

#include "./libyaml.hpp"

#include <c4/yml/yml.hpp>
#include <gtest/gtest.h>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#   pragma warning(disable: 4800/*'int': forcing value to bool 'true' or 'false' (performance warning)*/)
#endif

namespace c4 {
namespace yml {

inline void PrintTo(substr  s, ::std::ostream* os) { *os << s; }
inline void PrintTo(csubstr s, ::std::ostream* os) { *os << s; }

struct Case;
struct CaseNode;
struct CaseData;

Case const* get_case(csubstr name);
CaseData* get_data(csubstr name);

void check_invariants(Tree const& t);
void check_invariants(NodeRef const& n);

void show_children(NodeRef const& p);

void print_node(NodeRef const& p, int level=0, bool print_children=false);
void print_tree(NodeRef const& p, int level=0);
void print_tree(Tree const& t);

void print_node(CaseNode const& t, int level = 0);
void print_tree(CaseNode const& p, int level = 0);


void print_path(NodeRef const& p);

struct TaggedScalar
{
    csubstr tag;
    csubstr scalar;
    template< size_t N, size_t M > TaggedScalar(const char (&t)[N], const char (&s)[M]) : tag(t), scalar(s) {}
};

struct AnchorRef
{
    NodeType_e type;
    csubstr val;
    AnchorRef() : type(NOTYPE), val() {}
    AnchorRef(NodeType_e t) : type(t), val() {}
    AnchorRef(NodeType_e t, csubstr v) : type(t), val(v) {}
};

/** a node class against which ryml structures are tested. Uses initializer
 * lists to facilitate minimal specification. */
struct CaseNode
{
public:

    using  seqmap = std::vector< CaseNode >;
    using iseqmap = std::initializer_list< CaseNode >;

    struct TaggedList
    {
        csubstr tag;
        iseqmap ilist;
        template< size_t N > TaggedList(const char (&t)[N], iseqmap l) : tag(t), ilist(l) {}
    };

public:

    NodeType   type;
    csubstr      key, key_tag;
    csubstr      val, val_tag;
    AnchorRef  ancref;
    seqmap     children;
    CaseNode * parent;

public:

    CaseNode() : CaseNode(NOTYPE) {}
    CaseNode(NodeType_e t) : type(t), key(), key_tag(), val(), val_tag(), ancref(), children(), parent(nullptr) { _set_parent(); }


    // val
    template< size_t N > explicit CaseNode(const char (&v)[N]   ) : type((VAL       )), key(), key_tag(), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(TaggedScalar const& v) : type((VAL|VALTAG)), key(), key_tag(), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    // val, with anchor/ref
    template< size_t N > explicit CaseNode(const char (&v)[N]   , AnchorRef const& ar) : type((ar.type|VAL       )), key(), key_tag(), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|VAL|VALTAG)), key(), key_tag(), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }


    // val, explicit type
    template< size_t N > explicit CaseNode(NodeType t, const char (&v)[N]   ) : type((VAL|t       )), key(), key_tag(), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t, TaggedScalar const& v) : type((VAL|VALTAG|t)), key(), key_tag(), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    // val, explicit type, with anchor/ref
    template< size_t N > explicit CaseNode(NodeType t, const char (&v)[N]   , AnchorRef const& ar) : type((ar.type|VAL|t       )), key(), key_tag(), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t, TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|VAL|VALTAG|t)), key(), key_tag(), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }


    // keyval
    template< size_t N, size_t M > explicit CaseNode(const char (&k)[N]   , const char (&v)[M]   ) : type((KEYVAL              )), key(k       ), key_tag(     ), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
    template< size_t N >           explicit CaseNode(const char (&k)[N]   , TaggedScalar const& v) : type((KEYVAL|VALTAG       )), key(k       ), key_tag(     ), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    template< size_t M >           explicit CaseNode(TaggedScalar const& k, const char (&v)[M]   ) : type((KEYVAL|KEYTAG       )), key(k.scalar), key_tag(k.tag), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
                                   explicit CaseNode(TaggedScalar const& k, TaggedScalar const& v) : type((KEYVAL|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    // keyval, with anchor/ref
    template< size_t N, size_t M > explicit CaseNode(const char (&k)[N]   , const char (&v)[M]   , AnchorRef const& ar) : type((ar.type|KEYVAL              )), key(k       ), key_tag(     ), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
    template< size_t N >           explicit CaseNode(const char (&k)[N]   , TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|KEYVAL|VALTAG       )), key(k       ), key_tag(     ), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }
    template< size_t M >           explicit CaseNode(TaggedScalar const& k, const char (&v)[M]   , AnchorRef const& ar) : type((ar.type|KEYVAL|KEYTAG       )), key(k.scalar), key_tag(k.tag), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
                                   explicit CaseNode(TaggedScalar const& k, TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|KEYVAL|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }


    // keyval, explicit type
    template< size_t N, size_t M > explicit CaseNode(NodeType t, const char (&k)[N]   , const char (&v)[M]   ) : type((KEYVAL|t              )), key(k       ), key_tag(     ), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
    template< size_t N >           explicit CaseNode(NodeType t, const char (&k)[N]   , TaggedScalar const& v) : type((KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    template< size_t M >           explicit CaseNode(NodeType t, TaggedScalar const& k, const char (&v)[M]   ) : type((KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), val(v       ), val_tag(     ), ancref(), children(), parent(nullptr) { _set_parent(); }
                                   explicit CaseNode(NodeType t, TaggedScalar const& k, TaggedScalar const& v) : type((KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), val(v.scalar), val_tag(v.tag), ancref(), children(), parent(nullptr) { _set_parent(); }
    // keyval, explicit type, with anchor/ref
    template< size_t N, size_t M > explicit CaseNode(NodeType t, const char (&k)[N]   , const char (&v)[M]   , AnchorRef const& ar) : type((ar.type|KEYVAL|t              )), key(k       ), key_tag(     ), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
    template< size_t N >           explicit CaseNode(NodeType t, const char (&k)[N]   , TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }
    template< size_t M >           explicit CaseNode(NodeType t, TaggedScalar const& k, const char (&v)[M]   , AnchorRef const& ar) : type((ar.type|KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), val(v       ), val_tag(     ), ancref(ar), children(), parent(nullptr) { _set_parent(); }
                                   explicit CaseNode(NodeType t, TaggedScalar const& k, TaggedScalar const& v, AnchorRef const& ar) : type((ar.type|KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), val(v.scalar), val_tag(v.tag), ancref(ar), children(), parent(nullptr) { _set_parent(); }


    // container
    template< size_t N > explicit CaseNode(const char (&k)[N]   , iseqmap    s) : type(), key(k       ), key_tag(     ), val(), val_tag(     ), ancref(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template< size_t N > explicit CaseNode(const char (&k)[N]   , TaggedList s) : type(), key(k       ), key_tag(     ), val(), val_tag(s.tag), ancref(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(TaggedScalar const& k, iseqmap    s) : type(), key(k.scalar), key_tag(k.tag), val(), val_tag(     ), ancref(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(TaggedScalar const& k, TaggedList s) : type(), key(k.scalar), key_tag(k.tag), val(), val_tag(s.tag), ancref(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(                       iseqmap    m) : CaseNode("", m) {}
                         explicit CaseNode(                       TaggedList m) : CaseNode("", m) {}
    // container, with anchor/ref
    template< size_t N > explicit CaseNode(const char (&k)[N]   , iseqmap    s, AnchorRef const& ar) : type(), key(k       ), key_tag(     ), val(), val_tag(     ), ancref(ar), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template< size_t N > explicit CaseNode(const char (&k)[N]   , TaggedList s, AnchorRef const& ar) : type(), key(k       ), key_tag(     ), val(), val_tag(s.tag), ancref(ar), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(TaggedScalar const& k, iseqmap    s, AnchorRef const& ar) : type(), key(k.scalar), key_tag(k.tag), val(), val_tag(     ), ancref(ar), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(TaggedScalar const& k, TaggedList s, AnchorRef const& ar) : type(), key(k.scalar), key_tag(k.tag), val(), val_tag(s.tag), ancref(ar), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                         explicit CaseNode(                       iseqmap    m, AnchorRef const& ar) : CaseNode("", m, ar) {}
                         explicit CaseNode(                       TaggedList m, AnchorRef const& ar) : CaseNode("", m, ar) {}


    // container, explicit type
    template< size_t N > explicit CaseNode(NodeType t, const char (&k)[N]   , iseqmap    s) : type((t       )), key(k       ), key_tag(     ), val(), val_tag(     ), ancref(), children(s      ), parent(nullptr) { _set_parent(); }
    template< size_t N > explicit CaseNode(NodeType t, const char (&k)[N]   , TaggedList s) : type((t       )), key(k       ), key_tag(     ), val(), val_tag(s.tag), ancref(), children(s.ilist), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t, TaggedScalar const& k, iseqmap    s) : type((t|KEYTAG)), key(k.scalar), key_tag(k.tag), val(), val_tag(     ), ancref(), children(s      ), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t,                        iseqmap    s) : CaseNode(t, "", s) {}
                         explicit CaseNode(NodeType t,                        TaggedList s) : CaseNode(t, "", s) {}
    // container, explicit type, with anchor/ref
    template< size_t N > explicit CaseNode(NodeType t, const char (&k)[N]   , iseqmap    s, AnchorRef const& ar) : type((ar.type|t       )), key(k       ), key_tag(     ), val(), val_tag(     ), ancref(ar), children(s      ), parent(nullptr) { _set_parent(); }
    template< size_t N > explicit CaseNode(NodeType t, const char (&k)[N]   , TaggedList s, AnchorRef const& ar) : type((ar.type|t       )), key(k       ), key_tag(     ), val(), val_tag(s.tag), ancref(ar), children(s.ilist), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t, TaggedScalar const& k, iseqmap    s, AnchorRef const& ar) : type((ar.type|t|KEYTAG)), key(k.scalar), key_tag(k.tag), val(), val_tag(     ), ancref(ar), children(s      ), parent(nullptr) { _set_parent(); }
                         explicit CaseNode(NodeType t,                        iseqmap    s, AnchorRef const& ar) : CaseNode(t, "", s, ar) {}
                         explicit CaseNode(NodeType t,                        TaggedList s, AnchorRef const& ar) : CaseNode(t, "", s, ar) {}


    CaseNode(CaseNode     && that) { _move(std::move(that)); }
    CaseNode(CaseNode const& that) { _copy(that); }

    CaseNode& operator= (CaseNode     && that) { _move(std::move(that)); return *this; }
    CaseNode& operator= (CaseNode const& that) { _copy(that); return *this; }

public:

    void _move(CaseNode&& that)
    {
        type = that.type;
        key = that.key;
        key_tag = that.key_tag;
        val = that.val;
        val_tag = that.val_tag;
        children = std::move(that.children);
        parent = nullptr;
        _set_parent();
    }
    void _copy(CaseNode const& that)
    {
        type = that.type;
        key = that.key;
        key_tag = that.key_tag;
        val = that.val;
        val_tag = that.val_tag;
        children = that.children;
        parent = nullptr;
        _set_parent();
    }

    void _set_parent()
    {
        for(auto &ch : children)
        {
            ch.parent = this;
        }
    }

    NodeType_e _guess() const
    {
        NodeType t;
        C4_ASSERT(!val.empty() != !children.empty() || (val.empty() && children.empty()));
        if(children.empty())
        {
            C4_ASSERT(parent);
            if(key.empty())
            {
                t = VAL;
            }
            else
            {
                t = KEYVAL;
            }
        }
        else
        {
            NodeType_e has_key = key.empty() ? NOTYPE : KEY;
            auto const& ch = children.front();
            if(ch.key.empty())
            {
                t = (has_key|SEQ);
            }
            else
            {
                t = (has_key|MAP);
            }
        }
        if( ! key_tag.empty())
        {
            C4_ASSERT( ! key.empty());
            t.add(KEYTAG);
        }
        if( ! val_tag.empty())
        {
            C4_ASSERT( ! val.empty() || ! children.empty());
            t.add(VALTAG);
        }
        t.add(ancref.type);
        return t;
    }

    bool is_root() const { return parent; }
    bool is_doc() const { return type & DOC; }
    bool is_map() const { return type & MAP; }
    bool is_seq() const { return type & SEQ; }
    bool has_val() const { return type & VAL; }
    bool has_key() const { return type & KEY; }
    bool is_container() const { return type & (SEQ|MAP); }
    bool has_anchor() const { return type & ANCHOR; }

public:

    CaseNode const& operator[] (size_t i) const
    {
        C4_ASSERT(i >= 0 && i < children.size());
        return children[i];
    }

    CaseNode const& operator[] (csubstr const& name) const
    {
        auto ch = lookup(name);
        C4_ASSERT(ch != nullptr);
        return *ch;
    }

    CaseNode const* lookup(csubstr const& name) const
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

    void compare(yml::NodeRef const& n) const;
    void compare_child(yml::NodeRef const& n, size_t pos) const;

    size_t reccount() const
    {
        size_t c = 1;
        for(auto const& ch : children)
        {
            c += ch.reccount();
        }
        return c;
    }

    void recreate(yml::NodeRef *n) const;

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum {
    IGNORE_LIBYAML_PARSE_FAIL = (1<<0),
    IGNORE_YAMLCPP_PARSE_FAIL = (1<<1),
    RESOLVE_REFS = (1<<2),
} TestCaseFlags_e;

struct Case
{
    csubstr name;
    csubstr src;
    CaseNode root;
    TestCaseFlags_e flags;

    template< size_t N, class... Args >
    Case(csubstr const& n, const char (&s)[N], Args&& ...args)
        : name(n), src(s), root(std::forward< Args >(args)...), flags()
    {
    }

    template< size_t N, class... Args >
    Case(csubstr const& n, int f_, const char (&s)[N], Args&& ...args)
        : name(n), src(s), root(std::forward< Args >(args)...), flags((TestCaseFlags_e)f_)
    {
    }

};

//-----------------------------------------------------------------------------

// a persistent data store to avoid repeating operations on every test
struct CaseData
{
    std::vector< char > src_buf;
    substr src;

    Tree parsed_tree;

    size_t numbytes_stdout;
    std::vector< char > emit_buf;
    csubstr emitted_yml;
    std::vector< char > parse_buf;
    substr parsed_yml;

    Tree emitted_tree;

    Tree recreated;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// a fixture for running the tests
struct YmlTestCase : public ::testing::TestWithParam< const char* >
{
    csubstr const name;
    Case const* c;
    CaseData * d;

    YmlTestCase() : name(to_csubstr(GetParam())), c(get_case(to_csubstr(GetParam()))), d(get_data(to_csubstr(GetParam()))) {}

    void SetUp() override
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
        std::cout << "-------------------------------------------\n";
        std::cout << "running test case '" << name << "'\n";
        std::cout << "-------------------------------------------\n";
    }
};


} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#endif /* _TEST_CASE_HPP_ */
