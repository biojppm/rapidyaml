#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#define RYML_DBG // remove this
#define RYML_ERRMSG_SIZE 1024

#include "./common.hpp"
#include "./span.hpp"
#include "./stack.hpp"
#include "./to_str.hpp"

namespace c4 {
namespace yml {

class Node;
class Tree;
class Parser;


typedef enum {
    NOTYPE  = 0,       ///< no type is set
    VAL     = (1<<0),  ///< a leaf node, has a (possibly empty) value
    KEY     = (1<<1),  ///< is member of a map, must have non-empty key
    MAP     = (1<<2),  ///< a map: a parent of keyvals
    SEQ     = (1<<3),  ///< a seq: a parent of vals
    DOC     = (1<<4),  ///< a document
    STREAM  = (1<<5)|SEQ,  ///< a stream: a seq of docs
    _TYMASK = (1<<6)-1,
    KEYTAG  = (1<<6),  ///< the key has an explicit tag/type
    VALTAG  = (1<<7),  ///< the val has an explicit tag/type
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
} NodeType_e;


typedef enum {
    BLOCK_LITERAL, ///< keep newlines (|)
    BLOCK_FOLD     ///< replace newline with single space (>)
} BlockStyle_e;

typedef enum {
    CHOMP_CLIP,    ///< single newline at end (default)
    CHOMP_STRIP,   ///< no newline at end     (-)
    CHOMP_KEEP     ///< all newlines from end (+)
} BlockChomp_e;


/** an index to none */
enum : size_t { NONE = size_t(-1) };


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Node
{
private:

    Node() = default;

    Node(Node const&) = delete;
    Node(Node     &&) = delete;
    Node& operator= (Node const&) = delete;
    Node& operator= (Node     &&) = delete;

public:

    struct listseq
    {
        size_t prev;
        size_t next;
    };
    struct childrenfl
    {
        size_t first;
        size_t last;
    };

public:

    mutable Tree * m_s;
    NodeType_e     m_type;
    cspan          m_key, m_key_tag;
    cspan          m_val, m_val_tag;

    size_t         m_parent;
    childrenfl     m_children;

    listseq        m_list;

public:

    Tree *tree() const { return m_s; }
    size_t id() const;

    NodeType_e type() const { return (NodeType_e)(m_type & _TYMASK); }
    const char* type_str() const { return type_str(m_type); }
    static const char* type_str(NodeType_e ty);

    cspan const& key() const { C4_ASSERT(has_key()); return m_key; }
    cspan const& key_tag() const { C4_ASSERT(has_key_tag()); return m_key_tag; }

    cspan const& val() const { C4_ASSERT(has_val()); return m_val; }
    cspan const& val_tag() const { C4_ASSERT(has_val_tag()); return m_val_tag; }

    bool operator== (cspan const& cmp) const
    {
        C4_ASSERT(is_val());
        return m_val == cmp;
    }

    bool   is_root() const { return m_parent == NONE; }
    bool   is_stream() const { return (m_type & STREAM) == STREAM; }
    bool   is_doc() const { return m_type & DOC; }
    bool   is_container() const { return m_type & (MAP|SEQ|STREAM|DOC); }
    bool   is_map() const { return m_type & MAP; }
    bool   is_seq() const { return m_type & SEQ; }
    bool   has_val() const { return m_type & VAL; }
    bool   has_key() const { return m_type & KEY; }
    bool   is_val() const { return (m_type & KEYVAL) == VAL; }
    bool   is_keyval() const { return (m_type & KEYVAL) == KEYVAL; }
    bool   has_key_tag() const { return (m_type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    bool   has_val_tag() const { return (m_type & (VAL|VALTAG)) == (VAL|VALTAG); }

    bool   parent_is_seq() const { C4_ASSERT(parent()); return parent()->is_seq(); }
    bool   parent_is_map() const { C4_ASSERT(parent()); return parent()->is_map(); }

    /** true when name and value are empty, and has no children */
    bool   empty() const { return ! has_children() && m_key.empty() && (( ! (m_type & VAL)) || m_val.empty()); }
    bool   has_children() const { return m_children.first != NONE; }
    /** counts with this */
    bool   has_siblings() const;
    /** does not count with this */
    bool   has_other_siblings() const;

    bool   has_parent() const { return m_parent != NONE; }
    bool   has_sibling(Node const* n) const;
    bool   has_child(Node const* ch) const;

public:

    Node * parent() const;

    /** O(#num_children) */
    size_t num_children() const;
    Node * child(size_t i) const;
    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;
    size_t child_pos(Node const* n) const;

    /** O(#num_siblings) */
    size_t num_siblings() const;
    size_t num_other_siblings() const;
    Node * sibling(size_t i) const;
    Node * first_sibling() const;
    Node * last_sibling() const;
    Node * find_sibling(cspan const& name) const;

    Node * prev_sibling() const;
    Node * next_sibling() const;

public:

    Node & operator[] (size_t i) const
    {
        Node *c = child(i);
        if( ! c) { RymlCallbacks::error("could not find node");}
        return *c;
    }

    Node & operator[] (cspan const& name) const
    {
        C4_ASSERT(is_map());
        Node *c = find_child(name);
        if( ! c) { RymlCallbacks::error("could not find node"); }
        return *c;
    }

public:

    template< class Visitor > bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true);
    template< class Visitor > bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true) const;

    template< class Visitor > bool visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true);
    template< class Visitor > bool visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true) const;

public:

    void  remove_sibling(cspan const& name); ///< remove a sibling by name
    void  remove_sibling(size_t i); ///< remove a sibling by index
    void  remove_sibling(Node *n);  ///< remove a sibling

    void  remove_child  (cspan const& name); ///< remove a child by name
    void  remove_child  (size_t i); ///< remove a child by index
    void  remove_child  (Node *n);  ///< remove a child

public:

    void to_val(cspan const& val, int more_flags = 0);
    void to_keyval(cspan const& key, cspan const& val, int more_flags = 0);
    void to_map(int more_flags = 0);
    void to_map(cspan const& key, int more_flags = 0);
    void to_seq(int more_flags = 0);
    void to_seq(cspan const& key, int more_flags = 0);
    void to_doc(int more_flags = 0);
    void to_stream(int more_flags = 0);

    void set_val_tag(cspan const& tag);
    void set_key_tag(cspan const& tag);

public:

    void _add_flags(NodeType_e f) { m_type = (NodeType_e)(f | m_type); }
    void _add_flags(int        f) { m_type = (NodeType_e)(f | m_type); }

    void _rem_flags(NodeType_e f) { m_type = (NodeType_e)((~f) & m_type); }
    void _rem_flags(int        f) { m_type = (NodeType_e)((~f) & m_type); }

    void _set_flags(NodeType_e f) { m_type = f; }
    void _set_flags(int        f) { m_type = (NodeType_e)f; }

private:

    struct child_iterator
    {
        Node const *child;

        child_iterator(Node const* c) : child(c) {}

        child_iterator& operator++ (   ) { child = child->next_sibling(); return *this; }
        child_iterator  operator++ (int) { child_iterator ret = *this; child = child->next_sibling(); return ret; }

        child_iterator& operator-- (   ) { child = child->prev_sibling(); return *this; }
        child_iterator  operator-- (int) { child_iterator ret = *this; child = child->prev_sibling(); return ret; }

        Node const& operator*  () const { return *child; }
        Node const* operator-> () const { return  child; }

        bool operator!= (child_iterator that) const { return child != that.child; }
        bool operator== (child_iterator that) const { return child == that.child; }

    };

    struct children_container
    {
        Node const* parent;

        using const_iterator = child_iterator;

        children_container(Node const* p) : parent(p) {}

        const_iterator begin() const { return const_iterator(parent->first_child()); }
        const_iterator end  () const { return nullptr; }
    };

public:

    children_container children() const {                        return children_container(    this); }
    children_container siblings() const { C4_ASSERT(!is_root()); return children_container(parent()); }

};


template< class Visitor >
bool Node::visit(Visitor fn, size_t indentation_level, bool skip_root)
{
    return const_cast< Node const* >(this)->visit(fn, indentation_level, skip_root);
}

template< class Visitor >
bool Node::visit(Visitor fn, size_t indentation_level, bool skip_root) const
{
    size_t increment = 0;
    if( ! (is_root() && skip_root))
    {
        if(fn(this, indentation_level))
        {
            return true;
        }
        ++increment;
    }
    if(has_children())
    {
        for(Node *ch = first_child(); ch; ch = ch->next_sibling())
        {
            // no need to forward skip_root as it won't be root
            if(ch->visit(fn, indentation_level + increment))
            {
                return true;
            }
        }
    }
    return false;
}


template< class Visitor >
bool Node::visit_stacked(Visitor fn, size_t indentation_level, bool skip_root)
{
    return const_cast< Node const* >(this)->visit_stacked(fn, indentation_level, skip_root);
}

template< class Visitor >
bool Node::visit_stacked(Visitor fn, size_t indentation_level, bool skip_root) const
{
    size_t increment = 0;
    if( ! (is_root() && skip_root))
    {
        if(fn(this, indentation_level))
        {
            return true;
        }
        ++increment;
    }
    if(has_children())
    {
        fn.push(this, indentation_level);
        for(Node *ch = first_child(); ch; ch = ch->next_sibling())
        {
            // no need to forward skip_root as it won't be root
            if(ch->visit(fn, indentation_level + increment))
            {
                fn.pop(this, indentation_level);
                return true;
            }
        }
        fn.pop(this, indentation_level);
    }
    return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Tree
{
private:

    Node * m_buf;
    size_t m_cap;

    size_t m_size;
    size_t m_head;
    size_t m_tail;

    size_t m_free_head;
    size_t m_free_tail;

    span   m_arena;
    size_t m_arena_pos;

public:

    Tree(size_t node_capacity=16, size_t arena_capacity=0);
    ~Tree();

    Tree(Tree const& that);
    Tree(Tree     && that);

    Tree& operator= (Tree const& that);
    Tree& operator= (Tree     && that);

public:

    void reserve(size_t node_capacity, size_t arena_capacity=0);

    void clear();

    inline bool   empty() const { return m_size == 0; }

    inline size_t size () const { return m_size; }
    inline size_t capacity() const { return m_cap; }
    inline size_t slack() const { C4_ASSERT(m_cap >= m_size); return m_cap - m_size; }

    inline size_t arena_size() const { return m_arena_pos; }
    inline size_t arena_capacity() const { return m_arena.len; }
    inline size_t arena_slack() const { C4_ASSERT(m_arena.len >= m_arena_pos); return m_arena.len - m_arena_pos; }

public:

    size_t id(Node const* n)
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }
    size_t id(Node const* n) const
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }

    Node *get(size_t i)
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }
    Node const *get(size_t i) const
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }

public:

    //! create and insert a new sibling of n. insert after "after"
    Node * insert_sibling(Node *n, Node *after)
    {
        C4_ASSERT(n->parent() != nullptr);
        return insert_child(get(n->m_parent), after);
    }
    Node * prepend_sibling(Node *n) { return insert_sibling(n, nullptr); }
    Node *  append_sibling(Node *n) { return insert_sibling(n, n->last_child()); }

    //! create and insert a new sibling of n. insert after "after"
    size_t insert_sibling(size_t n, size_t after)
    {
        C4_ASSERT(get(n)->parent() != nullptr);
        C4_ASSERT( ! get(after) || (get(n)->has_sibling(get(after)) && get(after)->has_sibling(get(n))));
        return insert_child(get(n)->m_parent, after);
    }
    size_t prepend_sibling(size_t n) { return insert_sibling(n, NONE); }
    size_t  append_sibling(size_t n) { return insert_sibling(n, id(get(n)->last_child())); }

public:

    //! create and insert a new child of "n". insert after "after"
    Node * insert_child(Node *n, Node *after)
    {
        size_t cap = m_cap;
        size_t ich = insert_child(id(n), id(after));
        if(m_cap != cap)
        {
            C4_ERROR("pointers were invalidated");
        }
        Node *ch = get(ich);
        return ch;
    }
    Node * prepend_child(Node *n) { return insert_child(n, nullptr); }
    Node *  append_child(Node *n) { return insert_child(n, n->last_child()); }

    //! create and insert a new child of "parent". insert after "after"
    size_t insert_child(size_t parent, size_t after)
    {
        C4_ASSERT(get(parent)->is_container() || get(parent)->is_root());
        C4_ASSERT(get(parent)->has_child(get(after)) || after == NONE);
        size_t child = claim(after);
        set_parent(parent, child, after);
        return child;
    }
    size_t prepend_child(size_t n) { return insert_child(n, NONE); }
    size_t  append_child(size_t n) { return insert_child(n, id(get(n)->last_child())); }

public:

    //! remove a child by name, pointer version
    void remove_child(Node * parent, Node * child)
    {
        remove_child(id(parent), id(child));
    }
    //! remove a child by name, index version
    void remove_child(size_t parent, size_t child)
    {
        C4_ASSERT(get(parent)->is_container());
        C4_ASSERT(get(parent)->has_children());
        release(child);
        C4_ERROR("not implemented");
    }

    //! remove an entire branch at once
    void remove_branch(Node *first)
    {
        remove_branch(id(first));
    }
    //! remove an entire branch at once
    void remove_branch(size_t first)
    {
        C4_ERROR("not implemented");
    }

public:

    void move_branch(Node * branch, Node * new_parent, Node * after)
    {
        move_branch(id(branch), id(new_parent), id(after));
    }
    void move_branch(size_t branch, size_t new_parent, size_t after)
    {
        C4_ERROR("not implemented");
    }

private:

    void set_parent(size_t parent, size_t child, size_t prev_sibling, size_t next_sibling=NONE);
    void clear_range(size_t first, size_t num);

    size_t claim(size_t after);
    size_t claim(size_t prev, size_t next);

    void release(size_t i);

    void _clear(size_t i)
    {
        Node *n = get(i);
        n->m_s = nullptr;
        n->m_type = NOTYPE;
        n->m_key.clear();
        n->m_key_tag.clear();
        n->m_val.clear();
        n->m_val_tag.clear();
        n->m_parent = NONE;
        n->m_children.first = NONE;
        n->m_children.last = NONE;
    }

    Node      * head()       { return m_buf + m_head; }
    Node const* head() const { return m_buf + m_head; }

    Node      * tail()       { return m_buf + m_tail; }
    Node const* tail() const { return m_buf + m_tail; }

public:

    Node      * root()       { C4_ASSERT(m_head != NONE); C4_ASSERT(m_cap > 0 && m_size > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }
    Node const* root() const { C4_ASSERT(m_head != NONE); C4_ASSERT(m_cap > 0 && m_size > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }

    Node      * first_doc()         { Node *n = root()->child(0); C4_ASSERT(n && n->is_doc()); return n; }
    Node const* first_doc() const   { Node *n = root()->child(0); C4_ASSERT(n && n->is_doc()); return n; }

    Node      * doc(size_t i)       { Node *n = root()->child(i); C4_ASSERT(n && n->is_doc()); return n; }
    Node const* doc(size_t i) const { Node *n = root()->child(i); C4_ASSERT(n && n->is_doc()); return n; }

public:

    template< class T >
    span to_str_arena(T const& a)
    {
        span rem(m_arena.subspan(m_arena_pos));
        size_t num = to_str(rem, a);
        if(num > rem.len)
        {
            rem = _grow_arena(num);
            num = to_str(rem, a);
            C4_ASSERT(num <= rem.len);
        }
        rem = _request_span(num);
        return rem;
    }

    span _grow_arena(size_t num)
    {
        size_t cap = m_arena_pos + num;
        cap = cap < 2 * m_arena.len ? 2 * m_arena.len : cap;
        cap = cap < 64 ? 64 : cap;
        reserve(m_cap, cap);
        return m_arena.subspan(m_arena_pos);
    }

    span _request_span(size_t sz)
    {
        span s;
        s = m_arena.subspan(m_arena_pos, sz);
        m_arena_pos += sz;
        return s;
    }

public:

    inline bool in_arena(cspan const& s) const
    {
        return s.begin() >= m_arena.begin() && s.end() <= m_arena.end();
    }

private:

    inline span _relocate(cspan const& s, span const& next_arena) const
    {
        C4_ASSERT(in_arena(s));
        C4_ASSERT(s.begin() >= m_arena.begin() && s.end() <= m_arena.begin() + m_arena_pos);
        span r(next_arena.str + (s.str - m_arena.str), s.len);
        C4_ASSERT(r.begin() >= next_arena.begin() && r.end() <= next_arena.begin() + m_arena_pos);
        return r;
    }

    void _free();
    void _copy(Tree const& that);
    void _move(Tree      & that);

    void _relocate(span const& next_arena);

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct NodeScalar
{
    cspan tag;
    cspan scalar;

    /// initialize as an empty scalar

    inline NodeScalar() : tag(), scalar() {}

    /// initialize as an untagged scalar

    inline NodeScalar(cspan const& s          ) : tag(), scalar(s   ) {}
    inline NodeScalar(const char*  s          ) : tag(), scalar(s   ) {}
    inline NodeScalar(const char*  s, size_t n) : tag(), scalar(s, n) {}
    template< size_t N >
    inline NodeScalar(const char (&s)[N]      ) : tag(), scalar(    ) { /*avoid strlen call*/ scalar.assign<N>(s); }

    /// initialize as a tagged scalar

    inline NodeScalar(cspan const& t           , cspan const& s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(cspan const& t           , const char * s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(cspan const& t           , const char * s, size_t ns) : tag(t    ), scalar(s, ns) {}
    template< size_t M >
    inline NodeScalar(cspan const& t           , const char (&s)[M]       ) : tag(t    ), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }

    inline NodeScalar(const char * t           , cspan const& s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(const char * t           , const char * s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(const char * t           , const char * s, size_t ns) : tag(t    ), scalar(s, ns) {}
    template< size_t M >
    inline NodeScalar(const char * t           , const char (&s)[M]       ) : tag(t    ), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }

    inline NodeScalar(const char  *t, size_t nt, cspan const& s           ) : tag(t, nt), scalar(s    ) {}
    inline NodeScalar(const char  *t, size_t nt, const char * s           ) : tag(t, nt), scalar(s    ) {}
    inline NodeScalar(const char  *t, size_t nt, const char * s, size_t ns) : tag(t, nt), scalar(s, ns) {}
    template< size_t M >
    inline NodeScalar(const char  *t, size_t nt, const char (&s)[M]       ) : tag(t, nt), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }

    template< size_t N >
    inline NodeScalar(const char (&t)[N]       , cspan const& s           ) : tag(     ), scalar(s    ) {}
    template< size_t N >
    inline NodeScalar(const char (&t)[N]       , const char * s           ) : tag(     ), scalar(s    ) { /*avoid strlen call */tag.assign<N>(t); }
    template< size_t N >
    inline NodeScalar(const char (&t)[N]       , const char * s, size_t ns) : tag(     ), scalar(s, ns) { /*avoid strlen call */tag.assign<N>(t); }
    template< size_t N, size_t M >
    inline NodeScalar(const char (&t)[N]       , const char (&s)[M]       ) : tag(     ), scalar(     ) { /*avoid strlen call */tag.assign<N>(t); scalar.assign<M>(s); }

    bool empty() const { return scalar.empty(); }

};

struct NodeInit
{
    NodeType_e type;
    NodeScalar key;
    NodeScalar val;

    /// initialize as an empty node
    NodeInit() : type(NOTYPE), key(), val() {}

    /// initialize as a typed node
    NodeInit(NodeType_e t) : type(t), key(), val() {}

    /// initialize as a sequence member
    //NodeInit(cspan      const& v) : type(VAL), key(), val(v              ) {}
    NodeInit(NodeScalar const& v) : type(VAL), key(), val(v) { _add_flags(); }

    /// initialize as a mapping member
    //NodeInit(              cspan      const& k, cspan      const& v) : type(KEYVAL), key(k              ), val(v              ) { _add_flags(); }
    //NodeInit(              cspan      const& k, NodeScalar const& v) : type(KEYVAL), key(k              ), val(v.tag, v.scalar) { _add_flags(); }
    //NodeInit(              NodeScalar const& k, cspan      const& v) : type(KEYVAL), key(k.tag, k.scalar), val(v              ) { _add_flags(); }
    NodeInit(              NodeScalar const& k, NodeScalar const& v) : type(KEYVAL), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }
    //NodeInit(NodeType_e t, cspan      const& k, cspan      const& v) : type(t     ), key(k              ), val(v              ) {}
    //NodeInit(NodeType_e t, cspan      const& k, NodeScalar const& v) : type(t     ), key(k              ), val(v.tag, v.scalar) { _add_flags(); }
    //NodeInit(NodeType_e t, NodeScalar const& k, cspan      const& v) : type(t     ), key(k.tag, k.scalar), val(v              ) { _add_flags(); }
    NodeInit(NodeType_e t, NodeScalar const& k, NodeScalar const& v) : type(t     ), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }

    /// initialize as a mapping member with explicit type (eg SEQ or MAP)
    //NodeInit(NodeType_e t, cspan      const& k                     ) : type(t     ), key(k              ), val(               ) { _add_flags(KEY); }
    NodeInit(NodeType_e t, NodeScalar const& k                     ) : type(t     ), key(k.tag, k.scalar), val(               ) { _add_flags(KEY); }

    void clear()
    {
        memset(this, 0, sizeof(*this));
    }
    void _add_flags(int more_flags=0)
    {
        type = (NodeType_e)(type|more_flags);
        if( ! key.tag.empty()) type = (NodeType_e)(type|KEYTAG);
        if( ! val.tag.empty()) type = (NodeType_e)(type|VALTAG);
    }
};

class NodeRef;

template< class T > void write(NodeRef *n, T const& v);
template< class T > bool read(NodeRef const& n, T *v);

template< class K >
struct Key
{
    K const& k;
    inline Key(K const& k_) : k(k_) {}
};
template< class K >
Key< K > key(K const& k)
{
    return Key< K >{k};
}

class NodeRef
{
public://private:

    friend class Tree;


    NodeRef() : m_tree(nullptr), m_id(NONE), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t) : m_tree(t), m_id(t->root()->id()), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t, size_t id) : m_tree(t), m_id(id), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t, size_t id, size_t pos) : m_tree(t), m_id(id), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = pos; }
    NodeRef(Tree *t, size_t id, cspan  key) : m_tree(t), m_id(id), m_seed(key) {}

    NodeRef(Node *n) : m_tree(n->tree()), m_id(n->id()) {}

public:

    NodeRef(NodeRef const&) = default;
    NodeRef(NodeRef     &&) = default;

    NodeRef& operator= (NodeRef const&) = default;
    NodeRef& operator= (NodeRef     &&) = default;

public:

    inline Tree      * tree()       { return m_tree; }
    inline Tree const* tree() const { return m_tree; }

    inline size_t id() const { return m_id; }

    inline Node      * get()       { return m_tree->get(m_id); }
    inline Node const* get() const { return m_tree->get(m_id); }

    inline bool valid() const { return m_tree != nullptr && m_id != NONE; }
    inline bool is_seed() const { return m_seed.str != nullptr || m_seed.len != NONE; }

public:

    inline NodeType_e type() const { return get()->type(); }

    const char* type_str() const { return get()->type_str(); }
    static const char* type_str(NodeType_e ty);

    inline cspan const& key    () const { return get()->key(); }
    inline cspan const& key_tag() const { return get()->key_tag(); }

    inline cspan const& val    () const { return get()->val(); }
    inline cspan const& val_tag() const { return get()->val_tag(); }

public:

    inline bool is_map() const { return get()->is_map(); }
    inline bool is_seq() const { return get()->is_seq(); }

    inline bool is_root() const { return get()->is_root(); }
    inline bool is_stream() const { return get()->is_stream(); }
    inline bool is_doc() const { return get()->is_doc(); }
    inline bool is_container() const { return get()->is_container(); }
    inline bool has_val() const { return get()->has_val(); }
    inline bool has_key() const { return get()->has_key(); }
    inline bool is_val() const { return get()->is_val(); }
    inline bool is_keyval() const { return get()->is_keyval(); }
    inline bool has_key_tag() const { return get()->has_key_tag(); }
    inline bool has_val_tag() const { return get()->has_val_tag(); }

    inline bool parent_is_seq() const { return get()->parent_is_seq(); }
    inline bool parent_is_map() const { return get()->parent_is_map(); }

    /** true when name and value are empty, and has no children */
    inline bool empty() const { return get()->empty(); }
    inline bool has_children() const { return get()->has_children(); }
    /** counts with this */
    inline bool has_siblings() const { return get()->has_siblings(); }
    /** does not count with this */
    inline bool has_other_siblings() const { return get()->has_other_siblings(); }

    inline bool has_parent() const { return get()->has_parent(); }
    inline bool has_sibling(NodeRef const& n) const { return get()->has_sibling(n.get()); }
    inline bool has_child(NodeRef const& ch) const { return get()->has_child(ch.get()); }

public:

    inline NodeRef       parent()       { return {m_tree, m_tree->get(m_id)->m_parent}; }
    inline NodeRef const parent() const { return {m_tree, m_tree->get(m_id)->m_parent}; }

    /** O(#num_children) */
    size_t  num_children() const { return get()->num_children(); }
    NodeRef child(size_t i) const { return NodeRef(get()->child(i)); }
    NodeRef first_child() const { return NodeRef(get()->first_child()); }
    NodeRef last_child() const { return NodeRef(get()->last_child()); }
    NodeRef find_child(cspan const& name) const { return NodeRef(get()->find_child(name)); }
    size_t  child_pos(NodeRef const& n) const { return get()->child_pos(n.get()); }

    /** O(#num_siblings) */
    size_t  num_siblings() const { return get()->num_siblings(); }
    size_t  num_other_siblings() const { return get()->num_other_siblings(); }
    NodeRef sibling(size_t i) const { return NodeRef(get()->sibling(i)); }
    NodeRef first_sibling() const { return NodeRef(get()->first_sibling()); }
    NodeRef last_sibling() const { return NodeRef(get()->last_sibling()); }
    NodeRef find_sibling(cspan const& name) const { return NodeRef(get()->find_sibling(name)); }

    NodeRef prev_sibling() const { return NodeRef(get()->prev_sibling()); }
    NodeRef next_sibling() const { return NodeRef(get()->next_sibling()); }

public:

    template< class T >
    inline void set_key_serialized(T const& k)
    {
        Node *n = get();
        n->m_key = m_tree->to_str_arena(k);
        n->_add_flags(KEY);
    }

    template< class T >
    inline void set_val_serialized(T const& v)
    {
        C4_ASSERT(num_children() == 0);
        C4_ASSERT( ! is_container());
        Node *n = get();
        n->m_val = m_tree->to_str_arena(v);
        n->_add_flags(VAL);
    }

public:

    NodeRef operator[] (cspan const& k)
    {
        C4_ASSERT(valid());
        Node *n = get()->find_child(k);
        NodeRef r = n ? NodeRef(m_tree, n->id()) : NodeRef(m_tree, m_id, k);
        return r;
    }

    NodeRef operator[] (size_t i)
    {
        C4_ASSERT(valid());
        Node *n = get()->child(i);
        NodeRef r = n ? NodeRef(m_tree, n->id()) : NodeRef(m_tree, m_id, i);
        return r;
    }

public:

    NodeRef const operator[] (cspan const& k) const
    {
        C4_ASSERT(valid());
        Node const& n = *get();
        Node const& ch = n[k];
        NodeRef r(m_tree, ch.id());
        return r;
    }

    NodeRef const operator[] (size_t i) const
    {
        C4_ASSERT(valid());
        Node const& n = *get();
        Node const& ch = n[i];
        NodeRef r(m_tree, ch.id());
        return r;
    }

public:

    inline void operator= (NodeType_e t)
    {
        _clear_all();
        get()->m_type = t;
    }

    inline void operator= (NodeInit const& i)
    {
        _clear_all();
        _apply(i);
    }

    inline void operator= (NodeScalar const& v)
    {
        _clear_all();
        _apply(v);
    }

    inline void operator= (cspan const& v)
    {
        _clear_all();
        _apply(v);
    }

    template< size_t N >
    inline void operator= (const char (&v)[N])
    {
        _clear_all();
        cspan sv;
        sv.assign<N>(v);
        _apply(sv);
    }

public:

    inline void _clear_all()
    {
        if(is_seed()) return;
        Node *n = get();
        n->m_key.clear();
        n->m_val.clear();
        C4_ASSERT(num_children() == 0 && "not implemented");
    }

    inline void operator<< (cspan const& s) // this overload is needed to prevent ambiguity (there's also << for writing a span to a stream)
    {
        _clear_all();
        write(this, s);
        C4_ASSERT(get()->m_val == s);
        _apply();
    }

    template< class T >
    inline void operator<< (T const& v)
    {
        _clear_all();
        write(this, v);
        _apply();
    }

    template< class T >
    inline void operator>> (T &v) const
    {
        C4_ASSERT(valid());
        C4_ASSERT(get() != nullptr);
        if( ! read(*this, &v))
        {
            RymlCallbacks::error("could not parse value");
        }
    }

    template< class T >
    inline void operator<< (Key<const T> v)
    {
        set_key_serialized(v);
    }
    template< class T >
    inline void operator>> (Key<T> v) const
    {
        from_str(key(), &v);
    }

private:

    void _apply()
    {
        if(m_seed.str) // we have a seed key: use it to create the new child
        {
            //C4_ASSERT(i.key.scalar.empty() || m_key == i.key.scalar || m_key.empty());
            m_id = m_tree->append_child(m_id);
            Node *n = get();
            n->m_key = m_seed;
            n->_add_flags(KEY);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else if(m_seed.len != NONE) // we have a seed index: create a child at that position
        {
            C4_ASSERT(get()->num_children() == m_seed.len);
            m_id = m_tree->append_child(m_id);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else
        {
            C4_ASSERT(valid());
        }
    }

    template< class T >
    inline void _apply(T const& x)
    {
        _apply();
        _do_apply(get(), x);
    }

    static void _do_apply(Node *n, cspan const& v)
    {
        n->m_val = v;
        n->m_val_tag = 0;
        n->_add_flags(VAL);
    }

    static void _do_apply(Node *n, NodeScalar const& v)
    {
        n->m_val = v.scalar;
        n->m_val_tag = v.tag;
        n->_add_flags(VAL);
    }

    static void _do_apply(Node *n, NodeInit const& i)
    {
        n->m_type = i.type;
        n->m_key = i.key.scalar;
        n->m_key_tag = i.key.tag;
        n->m_val = i.val.scalar;
        n->m_val_tag = i.val.tag;
    }

public:

    NodeRef insert_child(NodeInit const& i, NodeRef after)
    {
        C4_ASSERT(valid());
        size_t child_id = m_tree->insert_child(m_id, after.m_id);
        NodeRef r(m_tree, child_id);
        _do_apply(r.get(), i);
        return r;
    }

    NodeRef prepend_child(NodeInit const& i)
    {
        NodeRef after(m_tree, NONE);
        return insert_child(i, after);
    }

    NodeRef append_child(NodeInit const& i)
    {
        NodeRef after(m_tree, get()->m_children.last);
        return insert_child(i, after);
    }

public:

    NodeRef insert_sibling(NodeInit const& i, NodeRef after)
    {
        return parent().insert_child(i, after);
    }

    NodeRef prepend_sibling(NodeInit const& i)
    {
        return parent().prepend_child(i);
    }

    NodeRef append_sibling(NodeInit const& i)
    {
        return parent().append_child(i);
    }

private:

    template< class N >
    struct child_iterator
    {
        Tree * m_tree;
        size_t m_child_id;

        child_iterator(Tree * t, size_t id) : m_tree(t), m_child_id(id) {}

        child_iterator& operator++ () { Node *s = m_tree->get(m_child_id)->next_sibling(); m_child_id = s ? s->id() : NONE; return *this; }
        child_iterator& operator-- () { Node *s = m_tree->get(m_child_id)->prev_sibling(); m_child_id = s ? s->id() : NONE; return *this; }

        N operator*  () const { return N(m_tree, m_child_id); }
        N operator-> () const { return N(m_tree, m_child_id); }

        bool operator!= (child_iterator that) const { C4_ASSERT(m_tree == that.m_tree); return m_child_id != that.m_child_id; }
        bool operator== (child_iterator that) const { C4_ASSERT(m_tree == that.m_tree); return m_child_id == that.m_child_id; }
    };

public:

    using       iterator = child_iterator<       NodeRef >;
    using const_iterator = child_iterator< const NodeRef >;

    inline iterator begin() { return iterator(m_tree, m_tree->get(m_id)->first_child()->id()); }
    inline iterator end  () { return iterator(m_tree, NONE); }

    inline const_iterator begin() const { return const_iterator(m_tree, m_tree->get(m_id)->first_child()->id()); }
    inline const_iterator end  () const { return const_iterator(m_tree, NONE); }

private:

    template< class N >
    struct children_container_
    {
        using n_iterator = child_iterator< N >;

        n_iterator b, e;

        inline children_container_(n_iterator const& b_, n_iterator const& e_) : b(b_), e(e_) {}

        inline iterator begin() const { return b; }
        inline iterator end  () const { return e; }
    };

public:

    using children_container = children_container_< NodeRef >;
    using const_children_container = children_container_< const NodeRef >;

          children_container children()       { return       children_container(begin(), end()); }
    const_children_container children() const { return const_children_container(begin(), end()); }

          children_container siblings()       { if(is_root()) { return       children_container(end(), end()); } else { size_t p = get()->m_parent; return       children_container(iterator(m_tree, m_tree->get(p)->m_children.first), iterator(m_tree, NONE)); } }
    const_children_container siblings() const { if(is_root()) { return const_children_container(end(), end()); } else { size_t p = get()->m_parent; return const_children_container(const_iterator(m_tree, m_tree->get(p)->m_children.first), const_iterator(m_tree, NONE)); } }

private:

    Tree * m_tree;
    size_t m_id;

    /** This member is used to enable lazy operator[] writing. When a child
     * is not found with a key or index, m_id is set to the id of the parent
     * and the key or index are stored in this member until a write does
     * happen. Then it is given as key or index for creating the child.
     *
     * When a key is used, the span stores it (so the span's string is
     * non-null and the span's size is different from NONE). When an index is
     * used instead, the span's string is set to null, and only the span's size is
     * set to a value different from NONE. Otherwise, when operator[] does find the
     * child then this member is empty: the string is null and the size is NONE. */
    cspan  m_seed;

};


template< class T >
inline void write(NodeRef *n, T const& v)
{
    n->set_val_serialized(v);
}

template< class T >
inline bool read(NodeRef const& n, T *v)
{
    return from_str(n.val(), v);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct RepC
{
    char c;
    size_t num_times;
};

struct Scalar
{
    cspan s;
    cspan tag;
    inline Scalar(cspan const& s_) : s(s_) {}
    inline Scalar(cspan const& s_, cspan const& t_) : s(s_), tag(t_) {}
};

template< class Writer >
class Emitter : public Writer
{
public:

    using Writer::Writer;

    /** emit YAML.
     *
     * When writing to a buffer, returns a subspan of the emitted YAML.
     * If the given buffer has insufficient space, the returned span will
     * be null and its size will be the needed space. No writes are done
     * after the end of the buffer.
     *
     * When writing to a file, the returned span will be null, but its
     * length is set the number of bytes written. */
    span emit(Node const* n, bool error_on_excess=true);

    size_t tell() const;
    void   seek(size_t p);

private:

    size_t _visit(Node const* n, size_t ilevel=0);
    void _do_visit(Node const* n, size_t ilevel=0, bool indent=true);

private:

#define _c4this (static_cast< Writer * >(this))

    static inline Scalar keysc(Node const* n) { return n->has_key_tag() ? Scalar(n->key(), n->key_tag()) : Scalar(n->key()); }
    static inline Scalar valsc(Node const* n) { return n->has_val_tag() ? Scalar(n->val(), n->val_tag()) : Scalar(n->val()); }

    template< class T, class... Args >
    inline void _write(T a, Args... more)
    {
        _write_one(a);
        _write(more...);
    }
    inline void _write() {}

    template< class T >
    inline void _write_one(T a)
    {
        _c4this->_do_write(a);
    }
    template< size_t N >
    inline void _write_one(const char (&a)[N])
    {
        // a decays into const char*, so explicitly create using the array
        cspan s;
        s.assign<N>(a);
        _c4this->_do_write(s);
    }
    void _write_one(Scalar const& sc);

#undef _c4this

};

//-----------------------------------------------------------------------------
/** A writer that outputs to a file. Defaults to stdout. */
struct FileWriter
{
    FILE *m_file;
    size_t m_pos;

    FileWriter(FILE *f = nullptr) : m_file(f ? f : stdout), m_pos(0) {}

    inline span _get(bool /*error_on_excess*/)
    {
        span sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    inline void _do_write(const char c)
    {
        fwrite(&c, sizeof(char), 1, m_file);
        ++m_pos;
    }

    inline void _do_write(RepC const rc)
    {
        //fwrite(&rc.c, sizeof(char), rc.num_times, m_file); // this fails... need to investigate
        for(size_t i = 0; i < rc.num_times; ++i)
        {
            fputc(rc.c, m_file);
        }
        m_pos += rc.num_times;
    }

    inline void _do_write(cspan const& sp)
    {
        if(sp.empty()) return;
        if(m_file)
        {
            fwrite(sp.str, sp.len, 1, m_file);
        }
        m_pos += sp.len;
    }

};

//-----------------------------------------------------------------------------
/** a writer to a string span */
struct SpanWriter
{
    span   m_span;
    size_t m_pos;

    SpanWriter(span const& sp) : m_span(sp), m_pos(0) {}

    inline span _get(bool error_on_excess)
    {
        span sp = m_span;
        if(m_pos > sp.len)
        {
            sp.len = m_pos;
            sp.str = nullptr;
            if(error_on_excess)
            {
                RymlCallbacks::error("not enough space in the given span");
            }
        }
        return sp;
    }

    inline void _do_write(const char c)
    {
        if(m_pos + 1 <= m_span.len)
        {
            m_span[m_pos] = c;
        }
        ++m_pos;
    }

    inline void _do_write(RepC const rc)
    {
        if(m_pos + rc.num_times <= m_span.len)
        {
            for(size_t i = 0; i < rc.num_times; ++i)
            {
                m_span[m_pos + i] = rc.c;
            }
        }
        m_pos += rc.num_times;
    }

    inline void _do_write(cspan const& sp)
    {
        if(sp.empty()) return;
        if(m_pos + sp.len <= m_span.len)
        {
            memcpy(&(m_span[m_pos]), sp.str, sp.len);
        }
        m_pos += sp.len;
    }

};

using FileEmitter = Emitter< FileWriter >;
using SpanEmitter = Emitter< SpanWriter >;

//-----------------------------------------------------------------------------
/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written. */

inline size_t emit(Node const* n, FILE *f = nullptr)
{
    FileEmitter em(f);
    size_t num = em.emit(n).len;
    return num;
}

inline size_t emit(Tree const &t, FILE *f = nullptr)
{
    if(t.empty()) return 0;
    return emit(t.root(), f);
}

//-----------------------------------------------------------------------------
/** emit YAML to the given buffer. Return a subspan of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline span emit(Node const* n, span const& sp, bool error_on_excess=true)
{
    SpanEmitter em(sp);
    span result = em.emit(n, error_on_excess);
    return result;
}

inline span emit(Tree const& t, span const& sp, bool error_on_excess=true)
{
    if(t.empty()) return span();
    return emit(t.root(), sp, error_on_excess);
}

//-----------------------------------------------------------------------------
/** emit YAML to the given container, resizing it as needed to fit the emitted YAML. */
template< class CharOwningContainer >
inline span emit_resize(Node const* n, CharOwningContainer * cont)
{
    span buf(cont->data(), cont->size());
    span ret = emit(n, buf, /*error_on_excess*/false);
    if(ret.str == nullptr)
    {
        cont->resize(ret.len);
        buf.assign(cont->data(), cont->size());
        ret = emit(n, buf, /*error_on_excess*/true);
    }
    return ret;
}

template< class CharOwningContainer >
inline span emit_resize(Tree const& t, CharOwningContainer * cont)
{
    if(t.empty()) return span();
    return emit_resize(t.root(), cont);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class Parser
{
public:

    Parser();

    Tree parse(                       span src) { return parse({}, src); }
    Tree parse(cspan const& filename, span src)
    {
        Tree t;
        size_t capacity = _count_nlines(src);
        capacity = capacity >= 16 ? capacity : 16;
        t.reserve(capacity);
        parse(filename, src, &t);
        return t;
    }

    void parse(                       span src, Tree *t) { return parse({}, src, t); }
    void parse(cspan const& filename, span src, Tree *t)
    {
        if(t->empty())
        {
            t->reserve(16);
        }
        parse(filename, src, t->root());
    }

    void parse(                       span src, Node *root) { return parse({}, src, root); }
    void parse(cspan const& filename, span src, Node *root);

private:

    void  _reset();

    bool  _finished_file() const;
    bool  _finished_line() const;

    cspan _peek_next_line(size_t pos=npos) const;
    void  _scan_line();
    void  _next_line() { _line_ended(); }

    bool  _is_scalar_next() const;
    cspan _scan_scalar();
    cspan _scan_comment();
    cspan _scan_quoted_scalar(const char q);
    cspan _scan_block();

    cspan _filter_squot_scalar(span s);
    cspan _filter_dquot_scalar(span s);
    cspan _filter_plain_scalar(span s, size_t indentation);
    cspan _filter_block_scalar(span s, BlockStyle_e style, BlockChomp_e chomp, size_t indentation);
    span  _filter_whitespace(span s, size_t indentation=0, bool leading_whitespace=true);

    void  _handle_finished_file();
    void  _handle_line();

    bool  _handle_indentation();

    bool  _handle_unk();
    bool  _handle_map_expl();
    bool  _handle_map_impl();
    bool  _handle_seq_expl();
    bool  _handle_seq_impl();
    bool  _handle_top();
    bool  _handle_anchors_and_refs();
    bool  _handle_types();

    void  _push_level(bool explicit_flow_chars = false);
    void  _pop_level();

    void  _start_unk(bool as_child=true);

    void  _start_map(bool as_child=true);
    void  _stop_map();

    void  _start_seq(bool as_child=true);
    void  _stop_seq();

    void  _start_doc(bool as_child=true);
    void  _stop_doc();

    void  _append_comment(cspan const& cmt);
    void  _append_val(cspan const& val);
    void  _append_key_val(cspan const& val);

    void  _store_scalar(cspan const& s);
    cspan _consume_scalar();
    void  _move_scalar_from_top();

    void  _save_indentation(size_t behind = 0);

private:

    static bool   _read_decimal(cspan const& str, size_t *decimal);
    static size_t _count_nlines(cspan src);

private:

    typedef enum {
        RTOP = 0x01 <<  0,   ///< reading at top level
        RUNK = 0x01 <<  1,   ///< reading an unknown: must determine whether scalar, map or seq
        RMAP = 0x01 <<  2,   ///< reading a map
        RSEQ = 0x01 <<  3,   ///< reading a seq
        EXPL = 0x01 <<  4,   ///< reading is inside explicit flow chars: [] or {}
        CPLX = 0x01 <<  5,   ///< reading a complex key
        RKEY = 0x01 <<  6,   ///< reading a scalar as key
        RVAL = 0x01 <<  7,   ///< reading a scalar as val
        RNXT = 0x01 <<  8,   ///< read next val or keyval
        SSCL = 0x01 <<  9,   ///< there's a scalar stored
    } State_e;

    struct LineContents
    {
        cspan  full;        ///< the full line, including newlines on the right
        cspan  stripped;    ///< the stripped line, excluding newlines on the right
        cspan  rem;         ///< the stripped line remainder; initially starts at the first non-space character
        int    indentation; ///< the number of spaces on the beginning of the line

        void reset(cspan const& full_, cspan const& stripped_)
        {
            full = full_;
            stripped = stripped_;
            rem = stripped_;
            // find the first column where the character is not a space
            indentation = (int)full.first_not_of(' ');
        }
    };

    struct State
    {
        size_t       flags;
        size_t       level;
        size_t       node_id; // don't hold a pointer to the node as it will be relocated during tree resizes
        cspan        scalar;

        Location     pos;
        LineContents line_contents;
        int          indref;

        void reset(const char *file, size_t node_id_)
        {
            flags = RUNK|RTOP;
            level = 0;
            pos.name = file;
            pos.offset = 0;
            pos.line = 1;
            pos.col = 1;
            node_id = node_id_;
            scalar.clear();
            indref = 0;
        }
    };

    void _line_progressed(size_t ahead);
    void _line_ended();

    void _prepare_pop()
    {
        C4_ASSERT(m_stack.size() > 1);
        State const& curr = m_stack.top();
        State      & next = m_stack.top(1);
        next.pos = curr.pos;
        next.line_contents = curr.line_contents;
        next.scalar = curr.scalar;
    }

    inline bool _at_line_begin() const
    {
        return m_state->line_contents.rem.begin() == m_state->line_contents.full.begin();
    }
    inline bool _at_line_end() const
    {
        cspan r = m_state->line_contents.rem;
        return r.empty() || r.begins_with(' ', r.len);
    }

    inline Node * node(State const* s) const { return m_tree->get(s->node_id); }
    inline Node * node(State const& s) const { return m_tree->get(s .node_id); }
    inline Node * node(size_t node_id) const { return m_tree->get(   node_id); }

    inline bool has_all(size_t f) const { return has_all(f, m_state); }
    inline bool has_any(size_t f) const { return has_any(f, m_state); }
    inline bool has_none(size_t f) const { return has_none(f, m_state); }

    inline bool has_all(size_t f, State const* s) const { return (s->flags & f) == f; }
    inline bool has_any(size_t f, State const* s) const { return (s->flags & f) != 0; }
    inline bool has_none(size_t f, State const* s) const { return (s->flags & f) == 0; }

    inline void set_flags(size_t f) { set_flags(f, m_state); }
    inline void add_flags(size_t on) { add_flags(on, m_state); }
    inline void addrem_flags(size_t on, size_t off) { addrem_flags(on, off, m_state); }
    inline void rem_flags(size_t off) { rem_flags(off, m_state); }

    void set_flags(size_t f, State * s);
    void add_flags(size_t on, State * s);
    void addrem_flags(size_t on, size_t off, State * s);
    void rem_flags(size_t off, State * s);

private:

#ifdef RYML_DBG
    void _dbg(const char *msg, ...) const;
#endif
    void _err(const char *msg, ...) const;
    int  _fmt_msg(char *buf, int buflen, const char *msg, va_list args) const;
    static int  _prfl(char *buf, int buflen, size_t v);

private:

    cspan   m_file;
     span   m_buf;

    size_t  m_root_id;
    Tree *  m_tree;

    detail::stack< State > m_stack;
    State * m_state;

    cspan   m_key_tag;
    cspan   m_val_tag;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline Tree parse(span buf)
{
    Parser np;
    return np.parse(buf);
}
inline Tree parse(cspan const& filename, span buf)
{
    Parser np;
    return np.parse(filename, buf);
}

inline void parse(span buf, Tree *t)
{
    Parser np;
    np.parse(buf, t);
}
inline void parse(cspan const& filename, span buf, Tree *t)
{
    Parser np;
    np.parse(filename, buf, t);
}

inline void parse(span buf, Node *root)
{
    Parser np;
    np.parse(buf, root);
}
inline void parse(cspan const& filename, span buf, Node *root)
{
    Parser np;
    np.parse(filename, buf, root);
}

} // namespace yml
} // namespace c4

//#include <ryml/ryml.cpp>
#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
