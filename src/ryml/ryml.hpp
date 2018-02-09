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

struct NodeScalar;
struct NodeInit;
class NodeData;
class NodeRef;
class Tree;
class Parser;

/** an index to none */
enum : size_t { NONE = size_t(-1) };


typedef enum {
    NOTYPE  = 0,          ///< no type is set
    VAL     = (1<<0),     ///< a leaf node, has a (possibly empty) value
    KEY     = (1<<1),     ///< is member of a map, must have non-empty key
    MAP     = (1<<2),     ///< a map: a parent of keyvals
    SEQ     = (1<<3),     ///< a seq: a parent of vals
    DOC     = (1<<4),     ///< a document
    STREAM  = (1<<5)|SEQ, ///< a stream: a seq of docs
    REF     = (1<<6),     ///< a *reference: references an &anchor
    _TYMASK = (1<<7)-1,
    KEYTAG  = (1<<7),     ///< the key has an explicit tag/type
    VALTAG  = (1<<8),     ///< the val has an explicit tag/type
    ANCHOR  = (1<<9),     ///< the node has an &anchor
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
} NodeType_e;



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct NodeType
{
    inline operator NodeType_e      & ()       { return type; }
    inline operator NodeType_e const& () const { return type; }

    NodeType() : type(NOTYPE) {}
    NodeType(int t) : type((NodeType_e)t) {}
    NodeType(NodeType_e t) : type(t) {}

    const char *type_str() const { return type_str(type); }
    static const char* type_str(NodeType_e t);

    void set(NodeType_e t) { type = (NodeType_e)t; }
    void set(int t) { type = (NodeType_e)t; }

    void add(NodeType_e t) { type = (NodeType_e)(type|t); }
    void add(int t) { type = (NodeType_e)(type|t); }

    void rem(NodeType_e t) { type = (NodeType_e)(type & ~t); }
    void rem(int t) { type = (NodeType_e)(type & ~t); }

public:

    bool   is_stream() const { return (type & STREAM) == STREAM; }
    bool   is_doc() const { return type & DOC; }
    bool   is_container() const { return type & (MAP|SEQ|STREAM|DOC); }
    bool   is_map() const { return type & MAP; }
    bool   is_seq() const { return type & SEQ; }
    bool   has_val() const { return type & VAL; }
    bool   has_key() const { return type & KEY; }
    bool   is_val() const { return (type & KEYVAL) == VAL; }
    bool   is_keyval() const { return (type & KEYVAL) == KEYVAL; }
    bool   has_key_tag() const { return (type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    bool   has_val_tag() const { return ((type & (VALTAG)) && (type & (VAL|MAP|SEQ))); }
    bool   is_ref() const { return type & REF; }

public:

    NodeType_e type;
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

    template< size_t N >
    inline NodeScalar(const char (&s)[N]      ) : tag(), scalar(    ) { /*avoid strlen call*/ scalar.assign<N>(s); }
    inline NodeScalar(cspan const& s          ) : tag(), scalar(s   ) {}
    inline NodeScalar(const char*  s          ) : tag(), scalar(s   ) {}
    inline NodeScalar(const char*  s, size_t n) : tag(), scalar(s, n) {}

    /// initialize as a tagged scalar

    template< size_t M >
    inline NodeScalar(cspan const& t           , const char (&s)[M]       ) : tag(t    ), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }
    inline NodeScalar(cspan const& t           , cspan const& s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(cspan const& t           , const char * s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(cspan const& t           , const char * s, size_t ns) : tag(t    ), scalar(s, ns) {}

    template< size_t M >
    inline NodeScalar(const char * t           , const char (&s)[M]       ) : tag(t    ), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }
    inline NodeScalar(const char * t           , cspan const& s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(const char * t           , const char * s           ) : tag(t    ), scalar(s    ) {}
    inline NodeScalar(const char * t           , const char * s, size_t ns) : tag(t    ), scalar(s, ns) {}

    template< size_t M >
    inline NodeScalar(const char * t, size_t nt, const char (&s)[M]       ) : tag(t, nt), scalar(     ) { /*avoid strlen call */scalar.assign<M>(s); }
    inline NodeScalar(const char * t, size_t nt, cspan const& s           ) : tag(t, nt), scalar(s    ) {}
    inline NodeScalar(const char * t, size_t nt, const char * s           ) : tag(t, nt), scalar(s    ) {}
    inline NodeScalar(const char * t, size_t nt, const char * s, size_t ns) : tag(t, nt), scalar(s, ns) {}

    template< size_t N, size_t M > inline NodeScalar(const char (&t)[N], const char (&s)[M]       ) : tag(     ), scalar(     ) { /*avoid strlen call */tag.assign<N>(t); scalar.assign<M>(s); }
    template< size_t N >           inline NodeScalar(const char (&t)[N], cspan const& s           ) : tag(     ), scalar(s    ) { /*avoid strlen call */tag.assign<N>(t); }
    template< size_t N >           inline NodeScalar(const char (&t)[N], const char * s           ) : tag(     ), scalar(s    ) { /*avoid strlen call */tag.assign<N>(t); }
    template< size_t N >           inline NodeScalar(const char (&t)[N], const char * s, size_t ns) : tag(     ), scalar(s, ns) { /*avoid strlen call */tag.assign<N>(t); }

    bool empty() const { return tag.empty() && scalar.empty(); }

    void clear() { tag.clear(); scalar.clear(); }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct NodeInit
{
    NodeType   type;
    NodeScalar key;
    NodeScalar val;

    /// initialize as an empty node
    NodeInit() : type(NOTYPE), key(), val() {}

    /// initialize as a typed node
    NodeInit(NodeType_e t) : type(t), key(), val() {}

    /// initialize as a sequence member
    NodeInit(NodeScalar const& v) : type(VAL), key(), val(v) { _add_flags(); }

    /// initialize as a mapping member
    NodeInit(              NodeScalar const& k, NodeScalar const& v) : type(KEYVAL), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }
    NodeInit(NodeType_e t, NodeScalar const& k, NodeScalar const& v) : type(t     ), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }

    /// initialize as a mapping member with explicit type (eg SEQ or MAP)
    NodeInit(NodeType_e t, NodeScalar const& k                     ) : type(t     ), key(k.tag, k.scalar), val(               ) { _add_flags(KEY); }

    void clear()
    {
        memset(this, 0, sizeof(*this));
    }

    void _add_flags(int more_flags=0)
    {
        type = (type|more_flags);
        if( ! key.tag.empty()) type = (type|KEYTAG);
        if( ! val.tag.empty()) type = (type|VALTAG);
    }

    bool _check() const
    {
        // key cannot be empty
        C4_ASSERT(key.scalar.empty() == ((type & KEY) == 0));
        // key tag cannot be empty
        C4_ASSERT(key.tag.empty() == ((type & KEYTAG) == 0));
        // val may be empty even though VAL is set. But when VAL is not set, val must be empty
        C4_ASSERT(((type & VAL) != 0) || val.scalar.empty());
        // val tag cannot be empty
        C4_ASSERT(val.tag.empty() == ((type & VALTAG) == 0));
        return true;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class NodeData
{
private:

    NodeData() = default;

    NodeData(NodeData const&) = delete;
    NodeData(NodeData     &&) = delete;
    NodeData& operator= (NodeData const&) = delete;
    NodeData& operator= (NodeData     &&) = delete;

public:

    NodeType   m_type;

    NodeScalar m_key;
    NodeScalar m_val;

    cspan      m_anchor;

    size_t     m_parent;
    size_t     m_first_child;
    size_t     m_last_child;
    size_t     m_next_sibling;
    size_t     m_prev_sibling;

public:

    NodeType_e type() const { return (NodeType_e)(m_type & _TYMASK); }
    const char* type_str() const { return type_str(m_type); }
    static const char* type_str(NodeType_e ty);

    cspan const& key() const { C4_ASSERT(has_key()); return m_key.scalar; }
    cspan const& key_tag() const { C4_ASSERT(has_key_tag()); return m_key.tag; }
    NodeScalar const& keysc() const { C4_ASSERT(has_key()); return m_key; }

    cspan const& val() const { C4_ASSERT(has_val()); return m_val.scalar; }
    cspan const& val_tag() const { C4_ASSERT(has_val_tag()); return m_val.tag; }
    NodeScalar const& valsc() const { C4_ASSERT(has_val()); return m_val; }

    cspan const& anchor() const { return m_anchor; }

public:

    bool   has_anchor() const { return ! m_anchor.empty(); }
    bool   is_root() const { return m_parent == NONE; }

    bool   is_stream() const { return m_type.is_stream(); }
    bool   is_doc() const { return m_type.is_doc(); }
    bool   is_container() const { return m_type.is_container(); }
    bool   is_map() const { return m_type.is_map(); }
    bool   is_seq() const { return m_type.is_seq(); }
    bool   has_val() const { return m_type.has_val(); }
    bool   has_key() const { return m_type.has_key(); }
    bool   is_val() const { return m_type.is_val(); }
    bool   is_keyval() const { return m_type.is_keyval(); }
    bool   has_key_tag() const { return m_type.has_key_tag(); }
    bool   has_val_tag() const { return m_type.has_val_tag(); }
    bool   is_ref() const { return m_type.is_ref(); }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Tree
{
private:

    NodeData * m_buf;
    size_t m_cap;

    size_t m_size;
    size_t m_head;
    size_t m_tail;

    size_t m_free_head;
    size_t m_free_tail;

    span   m_arena;
    size_t m_arena_pos;

    friend void check_invariants(Tree const&);

public:

    Tree();
    Tree(size_t node_capacity, size_t arena_capacity=0);
    ~Tree();

    Tree(Tree const& that);
    Tree(Tree     && that);

    Tree& operator= (Tree const& that);
    Tree& operator= (Tree     && that);

public:

    void reserve(size_t node_capacity, size_t arena_capacity=0);

    void clear();
    inline void clear_arena() { m_arena_pos = 0; }

    inline bool   empty() const { return m_size == 0; }

    inline size_t size () const { return m_size; }
    inline size_t capacity() const { return m_cap; }
    inline size_t slack() const { C4_ASSERT(m_cap >= m_size); return m_cap - m_size; }

    inline size_t arena_size() const { return m_arena_pos; }
    inline size_t arena_capacity() const { return m_arena.len; }
    inline size_t arena_slack() const { C4_ASSERT(m_arena.len >= m_arena_pos); return m_arena.len - m_arena_pos; }

public:

    size_t id(NodeData const* n)
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }
    size_t id(NodeData const* n) const
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }

    // with the get() method, i can be NONE, in which case a nullptr is returned
    inline NodeData *get(size_t i)
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }
    inline NodeData const *get(size_t i) const
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }

    // a shorter form of []
    inline NodeData       * _p(size_t i)       { C4_ASSERT(i != NONE && i >= 0 && i < m_cap); return m_buf + i; }
    inline NodeData const * _p(size_t i) const { C4_ASSERT(i != NONE && i >= 0 && i < m_cap); return m_buf + i; }

    /** resolve references in the tree */
    void resolve();

public:

    NodeType_e type(size_t node) const { return (NodeType_e)(_p(node)->m_type & _TYMASK); }
    const char* type_str(size_t node) const { return NodeType::type_str(_p(node)->m_type); }

    cspan const& key(size_t node) const { C4_ASSERT(has_key(node)); return _p(node)->m_key.scalar; }
    cspan const& key_tag(size_t node) const { C4_ASSERT(has_key_tag(node)); return _p(node)->m_key.tag; }
    NodeScalar const& keysc(size_t node) const { C4_ASSERT(has_key(node)); return _p(node)->m_key; }

    cspan const& val(size_t node) const { C4_ASSERT(has_val(node)); return _p(node)->m_val.scalar; }
    cspan const& val_tag(size_t node) const { C4_ASSERT(has_val_tag(node)); return _p(node)->m_val.tag; }
    NodeScalar const& valsc(size_t node) const { C4_ASSERT(has_val(node)); return _p(node)->m_val; }

    cspan const& anchor(size_t node) const { return _p(node)->m_anchor; }

public:

    // node predicates

    bool is_root(size_t node) const { return _p(node)->m_parent == NONE; }
    bool is_stream(size_t node) const { return (_p(node)->m_type & STREAM) == STREAM; }
    bool is_doc(size_t node) const { return _p(node)->m_type & DOC; }
    bool is_container(size_t node) const { return _p(node)->m_type & (MAP|SEQ|STREAM|DOC); }
    bool is_map(size_t node) const { return _p(node)->m_type & MAP; }
    bool is_seq(size_t node) const { return _p(node)->m_type & SEQ; }
    bool has_val(size_t node) const { return _p(node)->m_type & VAL; }
    bool has_key(size_t node) const { return _p(node)->m_type & KEY; }
    bool is_val(size_t node) const { return (_p(node)->m_type & KEYVAL) == VAL; }
    bool is_keyval(size_t node) const { return (_p(node)->m_type & KEYVAL) == KEYVAL; }
    bool has_key_tag(size_t node) const { return (_p(node)->m_type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    bool has_val_tag(size_t node) const { return ((_p(node)->m_type & (VALTAG)) && (_p(node)->m_type & (VAL|MAP|SEQ))); }
    bool is_ref(size_t node) const { return _p(node)->m_type & REF; }
    bool has_anchor(size_t node) const { return ! _p(node)->m_anchor.empty(); }

    bool parent_is_seq(size_t node) const { C4_ASSERT(has_parent(node)); return is_seq(_p(node)->m_parent); }
    bool parent_is_map(size_t node) const { C4_ASSERT(has_parent(node)); return is_map(_p(node)->m_parent); }

    /** true when name and value are empty, and has no children */
    bool empty(size_t node) const { return ! has_children(node) && _p(node)->m_key.empty() && (( ! (_p(node)->m_type & VAL)) || _p(node)->m_val.empty()); }

public:

    // hierarchy predicates

    bool has_parent(size_t node) const { return _p(node)->m_parent != NONE; }

    bool has_child(size_t node, size_t ch) const { return child_pos(node, ch) != npos; }
    bool has_children(size_t node) const { return _p(node)->m_first_child != NONE; }

    bool has_sibling(size_t node, size_t sib) const { return is_root(node) ? sib==node : child_pos(_p(node)->m_parent, sib) != npos; }
    /** counts with this */
    bool has_siblings(size_t node) const { return true; }
    /** does not count with this */
    bool has_other_siblings(size_t node) const { return is_root(node) ? false : (_p(_p(node)->m_parent)->m_first_child != _p(_p(node)->m_parent)->m_last_child); }

public:

    // hierarchy getters

    size_t parent(size_t node) const { return _p(node)->m_parent; }

    size_t prev_sibling(size_t node) const { return _p(node)->m_prev_sibling; }
    size_t next_sibling(size_t node) const { return _p(node)->m_next_sibling; }

    /** O(#num_children) */
    size_t num_children(size_t node) const;
    size_t child_pos(size_t node, size_t ch) const;
    size_t first_child(size_t node) const { return _p(node)->m_first_child; }
    size_t last_child(size_t node) const { return _p(node)->m_last_child; }
    size_t child(size_t node, size_t pos) const;
    size_t find_child(size_t node, cspan const& key) const;

    /** O(#num_siblings) */
    /** counts with this */
    size_t num_siblings(size_t node) const { return is_root(node) ? 1 : num_children(_p(node)->m_parent); }
    /** does not count with this */
    size_t num_other_siblings(size_t node) const { size_t ns = num_siblings(node); C4_ASSERT(ns > 0); return ns-1; }
    size_t sibling_pos(size_t node, size_t sib) const { C4_ASSERT( ! is_root(node) || node == root_id()); return child_pos(_p(node)->m_parent, sib); }
    size_t first_sibling(size_t node) const { return is_root(node) ? node : _p(_p(node)->m_parent)->m_first_child; }
    size_t last_sibling(size_t node) const { return is_root(node) ? node : _p(_p(node)->m_parent)->m_last_child; }
    size_t sibling(size_t node, size_t pos) const { return child(_p(node)->m_parent, pos); }
    size_t find_sibling(size_t node, cspan const& key) const { return find_child(_p(node)->m_parent, key); }

public:

    void to_val(size_t node, cspan const& val, int more_flags = 0);
    void to_keyval(size_t node, cspan const& key, cspan const& val, int more_flags = 0);
    void to_map(size_t node, int more_flags = 0);
    void to_map(size_t node, cspan const& key, int more_flags = 0);
    void to_seq(size_t node, int more_flags = 0);
    void to_seq(size_t node, cspan const& key, int more_flags = 0);
    void to_doc(size_t node, int more_flags = 0);
    void to_stream(size_t node, int more_flags = 0);

    void set_val_tag(size_t node, cspan const& tag);
    void set_key_tag(size_t node, cspan const& tag);

    void set_anchor(size_t node, cspan const& anchor);
    void rem_anchor(size_t node);

    inline void _add_flags(size_t node, NodeType_e f) { _p(node)->m_type = (f | _p(node)->m_type); }
    inline void _add_flags(size_t node, int        f) { _p(node)->m_type = (f | _p(node)->m_type); }

    inline void _rem_flags(size_t node, NodeType_e f) { _p(node)->m_type = ((~f) & _p(node)->m_type); }
    inline void _rem_flags(size_t node, int        f) { _p(node)->m_type = ((~f) & _p(node)->m_type); }

    inline void _set_flags(size_t node, NodeType_e f) { _p(node)->m_type = f; }
    inline void _set_flags(size_t node, int        f) { _p(node)->m_type = f; }

    void _set_key(size_t node, cspan const& key, int more_flags = 0)
    {
        _p(node)->m_key.scalar = key;
        _add_flags(node, KEY|more_flags);
    }
    void _set_key(size_t node, NodeScalar const& key, int more_flags = 0)
    {
        _p(node)->m_key = key;
        _add_flags(node, KEY|more_flags);
    }

    void _set_val(size_t node, cspan const& val, int more_flags = 0)
    {
        C4_ASSERT(num_children(node) == 0);
        C4_ASSERT( ! is_container(node));
        _p(node)->m_val.scalar = val;
        _add_flags(node, VAL|more_flags);
    }
    void _set_val(size_t node, NodeScalar const& val, int more_flags = 0)
    {
        C4_ASSERT(num_children(node) == 0);
        C4_ASSERT( ! is_container(node));
        _p(node)->m_val = val;
        _add_flags(node, VAL|more_flags);
    }

    void _set(size_t node, NodeInit const& i)
    {
        C4_ASSERT(i._check());
        NodeData *n = _p(node);
        C4_ASSERT(n->m_key.scalar.empty() || i.key.scalar.empty() || i.key.scalar == n->m_key.scalar);
        _add_flags(node, i.type);
        if(n->m_key.scalar.empty())
        {
            if( ! i.key.scalar.empty())
            {
                _set_key(node, i.key.scalar);
            }
        }
        n->m_key.tag = i.key.tag;
        n->m_val = i.val;
    }

    void _set_parent_as_container_if_needed(size_t in)
    {
        NodeData const* n = _p(in);
        size_t ip = parent(in);
        if(ip != NONE)
        {
            if( ! (is_seq(ip) || is_map(ip)))
            {
                if((in == first_child(ip)) && (in == last_child(ip)))
                {
                    if( ! n->m_key.empty() || n->has_key())
                    {
                        _add_flags(ip, MAP);
                    }
                    else
                    {
                        _add_flags(ip, SEQ);
                    }
                }
            }
        }
    }

    void _copy_props(size_t node, size_t that)
    {
        _p(node)->m_key = _p(that)->m_key;
        _copy_props_wo_key(node, that);
    }

    void _copy_props_wo_key(size_t node, size_t that)
    {
        auto *n = _p(node), *t = _p(that);
        n->m_type = t->m_type;
        n->m_val = t->m_val;
    }

    inline void _clear(size_t node)
    {
        auto *n = _p(node);
        n->m_type = NOTYPE;
        n->m_key.clear();
        n->m_val.clear();
        n->m_parent = NONE;
        n->m_first_child = NONE;
        n->m_last_child = NONE;
    }

    inline void _clear_key(size_t node)
    {
        _p(node)->m_key.clear();
        _rem_flags(node, KEY);
    }

    inline void _clear_val(size_t node)
    {
        _p(node)->m_key.clear();
        _rem_flags(node, VAL);
    }

public:

    /** create and insert a new child of "parent". insert after the (to-be)
     * sibling "after", which must be a child of "parent". To insert as the
     * first child, set after to NONE */
    inline size_t insert_child(size_t parent, size_t after)
    {
        C4_ASSERT(parent != NONE);
        C4_ASSERT(is_container(parent) || is_root(parent));
        C4_ASSERT(after == NONE || has_child(parent, after));
        size_t child = _claim();
        _set_hierarchy(child, parent, after);
        return child;
    }
    inline size_t prepend_child(size_t parent) { return insert_child(parent, NONE); }
    inline size_t  append_child(size_t parent) { return insert_child(parent, last_child(parent)); }

public:

    //! create and insert a new sibling of n. insert after "after"
    inline size_t insert_sibling(size_t node, size_t after)
    {
        C4_ASSERT(node != NONE);
        C4_ASSERT(parent(node) != NONE);
        C4_ASSERT(after == NONE || has_sibling(node, after) && has_sibling(after, node));
        return insert_child(get(node)->m_parent, after);
    }
    inline size_t prepend_sibling(size_t node) { return insert_sibling(node, NONE); }
    inline size_t  append_sibling(size_t node) { return insert_sibling(node, last_sibling(node)); }

public:

    //! remove an entire branch at once: ie remove the children and the node itself
    inline void remove(size_t node)
    {
        remove_children(node);
        _release(node);
    }

    //! remove all the node's children, but keep the node itself
    void remove_children(size_t node)
    {
        size_t ich = get(node)->m_first_child;
        while(ich != NONE)
        {
            remove_children(ich);
            size_t next = get(ich)->m_next_sibling;
            _release(ich);
            if(ich == get(node)->m_last_child) break;
            ich = next;
        }
    }

public:

    /** change the node's position in the parent */
    void move(size_t node, size_t after);

    /** change the node's parent and position */
    void move(size_t node, size_t new_parent, size_t after);

    /** duplicate the node (and its children) in a new parent */
    size_t duplicate(size_t node, size_t new_parent, size_t after);

    /** duplicate the node's children (but not the node) in a new parent */
    void duplicate_children(size_t node, size_t parent, size_t after);

    /** duplicate the node's children (but not the node) in a new parent, but
     * omit repetitions where a duplicated node has the same key (in maps) or
     * value (in seqs). If one of the duplicated children has the same key
     * (in maps) or value (in seqs) as one of the parent's children, the one
     * that is placed closer to the end will prevail. */
    void duplicate_children_no_rep(size_t node, size_t parent, size_t after);

    void duplicate_contents(size_t node, size_t where);

private:

    void _clear_range(size_t first, size_t num);

    size_t _claim();
    void   _release(size_t node);

    void _set_hierarchy(size_t node, size_t parent, size_t after_sibling);
    void _rem_hierarchy(size_t node);

public:

    size_t root_id()       { if(m_cap == 0) { reserve(16); } C4_ASSERT(m_head != NONE); C4_ASSERT(m_cap > 0 && m_size > 0); return 0; }
    size_t root_id() const {                                 C4_ASSERT(m_head != NONE); C4_ASSERT(m_cap > 0 && m_size > 0); return 0; }

    NodeRef       rootref();
    NodeRef const rootref() const;

public:

    template< class T >
    cspan to_arena(T const& a)
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

    inline bool in_arena(cspan const& s) const
    {
        return s.is_subspan(m_arena);
    }

private:

    span _grow_arena(size_t more)
    {
        size_t cap = m_arena_pos + more;
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

    inline span _relocated(cspan const& s, span const& next_arena) const
    {
        C4_ASSERT(s.is_subspan(m_arena));
        C4_ASSERT(s.is_subspan(m_arena.subspan(0, m_arena_pos)));
        auto pos = (s.str - m_arena.str);
        span r(next_arena.str + pos, s.len);
        C4_ASSERT(r.str - next_arena.str == pos);
        C4_ASSERT(r.is_subspan(next_arena.subspan(0, m_arena_pos)));
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

template< class T > void write(NodeRef *n, T const& v);
template< class T > bool read(NodeRef const& n, T *v);

template< class K >
struct Key
{
    K & k;
    inline Key(K & k_) : k(k_) {}
};
template< class K >
inline Key< K > key(K & k)
{
    return Key< K >{k};
}

class NodeRef
{
public:

    NodeRef() : m_tree(nullptr), m_id(NONE), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree &t) : m_tree(&t), m_id(t .root_id()), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t) : m_tree(t ), m_id(t->root_id()), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t, size_t id) : m_tree(t), m_id(id), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = NONE; }
    NodeRef(Tree *t, size_t id, size_t seed_pos) : m_tree(t), m_id(id), m_seed() { /*do this manually or an assert is triggered*/m_seed.str = nullptr; m_seed.len = seed_pos; }
    NodeRef(Tree *t, size_t id, cspan  seed_key) : m_tree(t), m_id(id), m_seed(seed_key) {}
    NodeRef(std::nullptr_t) : m_tree(nullptr), m_id(NONE), m_seed() {}

    NodeRef(NodeRef const&) = default;
    NodeRef(NodeRef     &&) = default;

    NodeRef& operator= (NodeRef const&) = default;
    NodeRef& operator= (NodeRef     &&) = default;

public:

    inline Tree      * tree()       { return m_tree; }
    inline Tree const* tree() const { return m_tree; }

    inline size_t id() const { return m_id; }

    inline NodeData      * get()       { return m_tree->get(m_id); }
    inline NodeData const* get() const { return m_tree->get(m_id); }

    inline bool valid() const { return m_tree != nullptr && m_id != NONE; }
    inline bool is_seed() const { return m_seed.str != nullptr || m_seed.len != NONE; }

    //inline operator bool () const { return m_tree == nullptr || m_id == NONE || is_seed(); }

    inline bool operator== (std::nullptr_t) const { return m_tree == nullptr || m_id == NONE || is_seed(); }
    inline bool operator!= (std::nullptr_t) const { return ! this->operator== (nullptr); }

#define _C4RV() C4_ASSERT(valid() && !is_seed()) // save some typing (and some reading too!)

    inline bool operator== (NodeRef const& that) const { _C4RV(); C4_ASSERT(that.valid() && !that.is_seed()); C4_ASSERT(that.m_tree == m_tree); return m_id == that.m_id; }
    inline bool operator!= (NodeRef const& that) const { return ! this->operator==(that); }

public:

    inline NodeType_e   type() const { _C4RV(); return m_tree->type(m_id); }
    inline const char*  type_str() const { _C4RV(); C4_ASSERT(valid() && ! is_seed()); return m_tree->type_str(m_id); }

    inline cspan      const& key    () const { _C4RV(); return m_tree->key(m_id); }
    inline cspan      const& key_tag() const { _C4RV(); return m_tree->key_tag(m_id); }
    inline NodeScalar const& keysc  () const { _C4RV(); return m_tree->keysc(m_id); }

    inline cspan      const& val    () const { _C4RV(); return m_tree->val(m_id); }
    inline cspan      const& val_tag() const { _C4RV(); return m_tree->val_tag(m_id); }
    inline NodeScalar const& valsc  () const { _C4RV(); return m_tree->valsc(m_id); }

    inline cspan const& anchor() const { _C4RV(); return m_tree->anchor(m_id); }

public:

    // node predicates

    inline bool is_root() const { _C4RV(); return m_tree->is_root(m_id); }
    inline bool is_stream() const { _C4RV(); return m_tree->is_stream(m_id); }
    inline bool is_doc() const { _C4RV(); return m_tree->is_doc(m_id); }
    inline bool is_container() const { _C4RV(); return m_tree->is_container(m_id); }
    inline bool is_map() const { _C4RV(); return m_tree->is_map(m_id); }
    inline bool is_seq() const { _C4RV(); return m_tree->is_seq(m_id); }
    inline bool has_val() const { _C4RV(); return m_tree->has_val(m_id); }
    inline bool has_key() const { _C4RV(); return m_tree->has_key(m_id); }
    inline bool is_val() const { _C4RV(); return m_tree->is_val(m_id); }
    inline bool is_keyval() const { _C4RV(); return m_tree->is_keyval(m_id); }
    inline bool has_key_tag() const { _C4RV(); return m_tree->has_key_tag(m_id); }
    inline bool has_val_tag() const { _C4RV(); return m_tree->has_val_tag(m_id); }
    inline bool is_ref() const { _C4RV(); return m_tree->is_ref(m_id); }
    inline bool has_anchor() const { _C4RV(); return m_tree->has_anchor(m_id); }

    inline bool parent_is_seq() const { _C4RV(); return m_tree->parent_is_seq(m_id); }
    inline bool parent_is_map() const { _C4RV(); return m_tree->parent_is_map(m_id); }

    /** true when name and value are empty, and has no children */
    inline bool empty() const { _C4RV(); return m_tree->empty(m_id); }

public:

    // hierarchy predicates

    inline bool has_parent() const { _C4RV(); return m_tree->has_parent(m_id); }

    inline bool has_child(NodeRef const& ch) const { _C4RV(); return m_tree->has_child(m_id, ch.m_id); }
    inline bool has_children() const { _C4RV(); return m_tree->has_children(m_id); }

    inline bool has_sibling(NodeRef const& n) const { _C4RV(); return m_tree->has_sibling(m_id, n.m_id); }
    /** counts with this */
    inline bool has_siblings() const { _C4RV(); return m_tree->has_siblings(m_id); }
    /** does not count with this */
    inline bool has_other_siblings() const { _C4RV(); return m_tree->has_other_siblings(m_id); }

public:

    // hierarchy getters

    NodeRef       parent()       { _C4RV(); return {m_tree, m_tree->parent(m_id)}; }
    NodeRef const parent() const { _C4RV(); return {m_tree, m_tree->parent(m_id)}; }

    NodeRef       prev_sibling()       { _C4RV(); return {m_tree, m_tree->prev_sibling(m_id)}; }
    NodeRef const prev_sibling() const { _C4RV(); return {m_tree, m_tree->prev_sibling(m_id)}; }

    NodeRef       next_sibling()       { _C4RV(); return {m_tree, m_tree->next_sibling(m_id)}; }
    NodeRef const next_sibling() const { _C4RV(); return {m_tree, m_tree->next_sibling(m_id)}; }

    /** O(#num_children) */
    size_t  num_children() const { _C4RV(); return m_tree->num_children(m_id); }
    size_t  child_pos(NodeRef const& n) const { _C4RV(); return m_tree->child_pos(m_id, n.m_id); }
    NodeRef       first_child()       { _C4RV(); return {m_tree, m_tree->first_child(m_id)}; }
    NodeRef const first_child() const { _C4RV(); return {m_tree, m_tree->first_child(m_id)}; }
    NodeRef       last_child ()       { _C4RV(); return {m_tree, m_tree->last_child (m_id)}; }
    NodeRef const last_child () const { _C4RV(); return {m_tree, m_tree->last_child (m_id)}; }
    NodeRef       child(size_t pos)       { _C4RV(); return {m_tree, m_tree->child(m_id, pos)}; }
    NodeRef const child(size_t pos) const { _C4RV(); return {m_tree, m_tree->child(m_id, pos)}; }
    NodeRef       find_child(cspan const& name)       { _C4RV(); return {m_tree, m_tree->find_child(m_id, name)}; }
    NodeRef const find_child(cspan const& name) const { _C4RV(); return {m_tree, m_tree->find_child(m_id, name)}; }

    /** O(#num_siblings) */
    size_t  num_siblings() const { _C4RV(); return m_tree->num_siblings(m_id); }
    size_t  num_other_siblings() const { _C4RV(); return m_tree->num_other_siblings(m_id); }
    size_t  sibling_pos(NodeRef const& n) const { _C4RV(); return m_tree->child_pos(m_tree->parent(m_id), n.m_id); }
    NodeRef       first_sibling()       { _C4RV(); return {m_tree, m_tree->first_sibling(m_id)}; }
    NodeRef const first_sibling() const { _C4RV(); return {m_tree, m_tree->first_sibling(m_id)}; }
    NodeRef       last_sibling()       { _C4RV(); return {m_tree, m_tree->last_sibling(m_id)}; }
    NodeRef const last_sibling() const { _C4RV(); return {m_tree, m_tree->last_sibling(m_id)}; }
    NodeRef       sibling(size_t pos)       { _C4RV(); return {m_tree, m_tree->sibling(m_id, pos)}; }
    NodeRef const sibling(size_t pos) const { _C4RV(); return {m_tree, m_tree->sibling(m_id, pos)}; }
    NodeRef       find_sibling(cspan      & name) const { _C4RV(); return {m_tree, m_tree->find_sibling(m_id, name)}; }
    NodeRef const find_sibling(cspan const& name) const { _C4RV(); return {m_tree, m_tree->find_sibling(m_id, name)}; }

public:

    inline void set_type(NodeType t)
    {
        _C4RV();
        m_tree->_set_flags(m_id, t);
    }

    inline void set_key(cspan const& key)
    {
        _C4RV();
        m_tree->_set_key(m_id, key);
    }

    template< class T >
    inline void set_key_serialized(T const& k)
    {
        _C4RV();
        auto sp = m_tree->to_arena(k);
        m_tree->_set_key(m_id, sp);
    }

    inline void set_key_tag(cspan const& key_tag)
    {
        _C4RV();
        m_tree->set_key_tag(m_id, key_tag);
    }

    inline void set_val(cspan const& val)
    {
        _C4RV();
        m_tree->_set_val(m_id, val);
    }

    template< class T >
    inline void set_val_serialized(T const& v)
    {
        _C4RV();
        auto sp = m_tree->to_arena(v);
        m_tree->_set_val(m_id, sp);
    }

    inline void set_val_tag(cspan const& val_tag) const
    {
        _C4RV();
        m_tree->set_val_tag(m_id, val_tag);
    }

    template< class T >
    inline cspan to_arena(T const& s) const
    {
        _C4RV();
        return m_tree->to_arena(s);
    }

public:

    /** O(num_children) */
    NodeRef operator[] (cspan const& k)
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        size_t ch = m_tree->find_child(m_id, k);
        NodeRef r = ch != NONE ? NodeRef(m_tree, ch) : NodeRef(m_tree, m_id, k);
        return r;
    }

    /** O(num_children) */
    NodeRef operator[] (size_t pos)
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        size_t ch = m_tree->child(m_id, pos);
        NodeRef r = ch != NONE ? NodeRef(m_tree, ch) : NodeRef(m_tree, m_id, pos);
        return r;
    }

public:

    /** O(num_children) */
    NodeRef const operator[] (cspan const& k) const
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        size_t ch = m_tree->find_child(m_id, k);
        C4_ASSERT(ch != NONE);
        NodeRef const r(m_tree, ch);
        return r;
    }

    /** O(num_children) */
    NodeRef const operator[] (size_t pos) const
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        size_t ch = m_tree->child(m_id, pos);
        C4_ASSERT(ch != NONE);
        NodeRef const r(m_tree, ch);
        return r;
    }

public:

    inline void clear()
    {
        if(is_seed()) return;
        m_tree->remove_children(m_id);
        m_tree->_clear(m_id);
    }

    inline void clear_key()
    {
        if(is_seed()) return;
        m_tree->_clear_key(m_id);
    }

    inline void clear_val()
    {
        if(is_seed()) return;
        m_tree->_clear_val(m_id);
    }

    inline void clear_children()
    {
        if(is_seed()) return;
        m_tree->remove_children(m_id);
    }

public:

    inline void operator= (NodeType_e t)
    {
        _apply_seed();
        m_tree->_set_flags(m_id, t);
    }
    inline void operator|= (NodeType_e t)
    {
        _apply_seed();
        m_tree->_add_flags(m_id, t);
    }

    inline void operator= (NodeInit const& v)
    {
        _apply_seed();
        _apply(v);
    }

    inline void operator= (NodeScalar const& v)
    {
        _apply_seed();
        _apply(v);
    }

    inline void operator= (cspan const& v)
    {
        _apply_seed();
        _apply(v);
    }

    template< size_t N >
    inline void operator= (const char (&v)[N])
    {
        _apply_seed();
        cspan sv;
        sv.assign<N>(v);
        _apply(sv);
    }

public:

    inline void operator<< (cspan const& s) // this overload is needed to prevent ambiguity (there's also << for writing a span to a stream)
    {
        _apply_seed();
        write(this, s);
        C4_ASSERT(get()->val() == s);
    }

    template< class T >
    inline void operator<< (T const& v)
    {
        _apply_seed();
        write(this, v);
    }

    template< class T >
    inline void operator<< (Key<const T> const& v)
    {
        _apply_seed();
        set_key_serialized(v.k);
    }

    template< class T >
    inline void operator>> (T &v) const
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        C4_ASSERT(get() != nullptr);
        if( ! read(*this, &v))
        {
            RymlCallbacks::error("could not parse value");
        }
    }

    template< class T >
    inline void operator>> (Key<T> v) const
    {
        C4_ASSERT( ! is_seed());
        C4_ASSERT(valid());
        C4_ASSERT(get() != nullptr);
        from_str(key(), &v.k);
    }

private:

    void _apply_seed()
    {
        if(m_seed.str) // we have a seed key: use it to create the new child
        {
            //C4_ASSERT(i.key.scalar.empty() || m_key == i.key.scalar || m_key.empty());
            m_id = m_tree->append_child(m_id);
            m_tree->_set_key(m_id, m_seed);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else if(m_seed.len != NONE) // we have a seed index: create a child at that position
        {
            C4_ASSERT(m_tree->num_children(m_id) == m_seed.len);
            m_id = m_tree->append_child(m_id);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else
        {
            C4_ASSERT(valid());
        }
    }

    inline void _apply(cspan const& v)
    {
        m_tree->_set_val(m_id, v);
    }

    inline void _apply(NodeScalar const& v)
    {
        m_tree->_set_val(m_id, v);
    }

    inline void _apply(NodeInit const& i)
    {
        m_tree->_set(m_id, i);
    }

public:

    inline NodeRef insert_child(NodeRef after)
    {
        _C4RV();
        C4_ASSERT(valid());
        size_t child_id = m_tree->insert_child(m_id, after.m_id);
        NodeRef r(m_tree, child_id);
        return r;
    }

    inline NodeRef insert_child(NodeInit const& i, NodeRef after)
    {
        _C4RV();
        C4_ASSERT(valid());
        size_t child_id = m_tree->insert_child(m_id, after.m_id);
        NodeRef r(m_tree, child_id);
        r._apply(i);
        return r;
    }

    inline NodeRef prepend_child()
    {
        _C4RV();
        NodeRef after(m_tree, NONE);
        return insert_child(after);
    }

    inline NodeRef prepend_child(NodeInit const& i)
    {
        _C4RV();
        NodeRef after(m_tree, NONE);
        return insert_child(i, after);
    }

    inline NodeRef append_child()
    {
        _C4RV();
        NodeRef after(m_tree, get()->m_last_child);
        return insert_child(after);
    }

    inline NodeRef append_child(NodeInit const& i)
    {
        _C4RV();
        NodeRef after(m_tree, get()->m_last_child);
        return insert_child(i, after);
    }

public:

    inline void remove_child(NodeRef & child)
    {
        _C4RV();
        C4_ASSERT(has_child(child));
        C4_ASSERT(child.parent().id() == id());
        m_tree->remove(child.id());
        child.clear();
    }

    //! remove the nth child of this node
    inline void remove_child(size_t pos)
    {
        _C4RV();
        C4_ASSERT(pos >= 0 && pos < num_children());
        size_t child = m_tree->child(m_id, pos);
        C4_ASSERT(child != NONE);
        m_tree->remove(child);
    }

    //! remove a child by name
    inline void remove_child(cspan key)
    {
        _C4RV();
        size_t child = m_tree->find_child(m_id, key);
        C4_ASSERT(child != NONE);
        m_tree->remove(child);
    }

public:

    inline void move(NodeRef const parent, NodeRef const after)
    {
        _C4RV();
        m_tree->move(m_id, parent.m_id, after.m_id);
    }

    inline void move(NodeRef const after)
    {
        _C4RV();
        m_tree->move(m_id, after.m_id);
    }

    inline NodeRef duplicate(NodeRef const parent, NodeRef const after)
    {
        _C4RV();
        size_t dup = m_tree->duplicate(m_id, parent.m_id, after.m_id);
        NodeRef r(m_tree, dup);
        return r;
    }

public:

    inline NodeRef insert_sibling(NodeRef const after)
    {
        _C4RV();
        C4_ASSERT( ! is_root());
        return parent().insert_child(after);
    }

    inline NodeRef insert_sibling(NodeInit const& i, NodeRef const after)
    {
        _C4RV();
        C4_ASSERT( ! is_root());
        return parent().insert_child(i, after);
    }

    inline NodeRef prepend_sibling()
    {
        _C4RV();
        NodeRef after(m_tree, NONE);
        return parent().insert_child(after);
    }

    inline NodeRef prepend_sibling(NodeInit const& i)
    {
        _C4RV();
        NodeRef after(m_tree, NONE);
        return parent().insert_child(i, after);
    }

    inline NodeRef append_sibling()
    {
        _C4RV();
        NodeRef after(m_tree, m_tree->last_child(m_id));
        return parent().insert_child(after);
    }

    inline NodeRef append_sibling(NodeInit const& i)
    {
        _C4RV();
        NodeRef after(m_tree, m_tree->last_child(m_id));
        return parent().insert_child(i, after);
    }

private:

    template< class N >
    struct child_iterator
    {
        Tree * m_tree;
        size_t m_child_id;

        using value_type = NodeRef;

        child_iterator(Tree * t, size_t id) : m_tree(t), m_child_id(id) {}

        child_iterator& operator++ () { C4_ASSERT(m_child_id != NONE); m_child_id = m_tree->next_sibling(m_child_id); return *this; }
        child_iterator& operator-- () { C4_ASSERT(m_child_id != NONE); m_child_id = m_tree->prev_sibling(m_child_id); return *this; }

        N operator*  () const { return N(m_tree, m_child_id); }
        N operator-> () const { return N(m_tree, m_child_id); }

        bool operator!= (child_iterator that) const { C4_ASSERT(m_tree == that.m_tree); return m_child_id != that.m_child_id; }
        bool operator== (child_iterator that) const { C4_ASSERT(m_tree == that.m_tree); return m_child_id == that.m_child_id; }
    };

public:

    using       iterator = child_iterator<       NodeRef >;
    using const_iterator = child_iterator< const NodeRef >;

    inline iterator begin() { return iterator(m_tree, m_tree->first_child(m_id)); }
    inline iterator end  () { return iterator(m_tree, NONE); }

    inline const_iterator begin() const { return const_iterator(m_tree, m_tree->first_child(m_id)); }
    inline const_iterator end  () const { return const_iterator(m_tree, NONE); }

private:

    template< class N >
    struct children_container_
    {
        using n_iterator = child_iterator< N >;

        n_iterator b, e;

        inline children_container_(n_iterator const& b_, n_iterator const& e_) : b(b_), e(e_) {}

        inline n_iterator begin() const { return b; }
        inline n_iterator end  () const { return e; }
    };

public:

    using children_container = children_container_< NodeRef >;
    using const_children_container = children_container_< const NodeRef >;

          children_container children()       { return       children_container(begin(), end()); }
    const_children_container children() const { return const_children_container(begin(), end()); }

          children_container siblings()       { if(is_root()) { return       children_container(end(), end()); } else { size_t p = get()->m_parent; return       children_container(iterator(m_tree, m_tree->get(p)->m_first_child), iterator(m_tree, NONE)); } }
    const_children_container siblings() const { if(is_root()) { return const_children_container(end(), end()); } else { size_t p = get()->m_parent; return const_children_container(const_iterator(m_tree, m_tree->get(p)->m_first_child), const_iterator(m_tree, NONE)); } }

public:

    template< class Visitor > bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true);
    template< class Visitor > bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true) const;

    template< class Visitor > bool visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true);
    template< class Visitor > bool visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true) const;

private:

    Tree * m_tree;
    size_t m_id;

    /** This member is used to enable lazy operator[] writing. When a child
     * is not found with a key or index, m_id is set to the id of the parent
     * and the asked-for key or index are stored in this member until a write
     * does happen. Then it is given as key or index for creating the child.
     * When a key is used, the span stores it (so the span's string is
     * non-null and the span's size is different from NONE). When an index is
     * used instead, the span's string is set to null, and only the span's
     * size is set to a value different from NONE. Otherwise, when operator[]
     * does find the child then this member is empty: the string is null and
     * the size is NONE. */
    cspan  m_seed;

#undef _C4RV
};

//-----------------------------------------------------------------------------

inline NodeRef       Tree::rootref()       { return NodeRef(this, root_id()); }
inline NodeRef const Tree::rootref() const { return NodeRef((Tree*)this, root_id()); }


//-----------------------------------------------------------------------------
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
template< class Visitor >
bool NodeRef::visit(Visitor fn, size_t indentation_level, bool skip_root)
{
    return const_cast< NodeRef const* >(this)->visit(fn, indentation_level, skip_root);
}

template< class Visitor >
bool NodeRef::visit(Visitor fn, size_t indentation_level, bool skip_root) const
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
        for(auto ch : children())
        {
            if(ch.visit(fn, indentation_level + increment)) // no need to forward skip_root as it won't be root
            {
                return true;
            }
        }
    }
    return false;
}


template< class Visitor >
bool NodeRef::visit_stacked(Visitor fn, size_t indentation_level, bool skip_root)
{
    return const_cast< NodeRef const* >(this)->visit_stacked(fn, indentation_level, skip_root);
}

template< class Visitor >
bool NodeRef::visit_stacked(Visitor fn, size_t indentation_level, bool skip_root) const
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
        for(auto ch : children())
        {
            if(ch.visit(fn, indentation_level + increment)) // no need to forward skip_root as it won't be root
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

struct RepC
{
    char c;
    size_t num_times;
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
    span emit(NodeRef const& n, bool error_on_excess=true);

    size_t tell() const;
    void   seek(size_t p);

private:

    size_t _visit(NodeRef const& n, size_t ilevel=0);
    void _do_visit(NodeRef const& n, size_t ilevel=0, bool indent=true);

private:

#define _c4this (static_cast< Writer * >(this))

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
    void _write_one(NodeScalar const& sc);

    struct AnchorScalar : public NodeScalar
    {
        cspan anchor;
        AnchorScalar(NodeRef const& n) : NodeScalar(n.valsc()), anchor(n.anchor()) {}
    };

    void _write_one(AnchorScalar const& sc);

    void _write_scalar(cspan const& s);

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

inline size_t emit(NodeRef const& r, FILE *f = nullptr)
{
    FileEmitter em(f);
    size_t num = em.emit(r).len;
    return num;
}

inline size_t emit(Tree const &t, FILE *f = nullptr)
{
    if(t.empty()) return 0;
    const NodeRef n(t.rootref());
    return emit(n, f);
}

//-----------------------------------------------------------------------------
/** emit YAML to the given buffer. Return a subspan of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */

inline span emit(NodeRef const& r, span const& sp, bool error_on_excess=true)
{
    SpanEmitter em(sp);
    span result = em.emit(r, error_on_excess);
    return result;
}

inline span emit(Tree const& t, span const& sp, bool error_on_excess=true)
{
    if(t.empty()) return span();
    return emit(t.rootref(), sp, error_on_excess);
}

//-----------------------------------------------------------------------------
/** emit YAML to the given container, resizing it as needed to fit the emitted YAML. */
template< class CharOwningContainer >
inline span emit_resize(NodeRef const& n, CharOwningContainer * cont)
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
    return emit_resize(t.rootref(), cont);
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
        NodeRef root(t);
        parse(filename, src, &root);
    }

    // This is very wrong. Use
    void parse(                       span src, NodeRef * root) { return parse({}, src, root); }
    void parse(cspan const& filename, span src, NodeRef * root);

private:

    typedef enum {
        BLOCK_LITERAL, ///< keep newlines (|)
        BLOCK_FOLD     ///< replace newline with single space (>)
    } BlockStyle_e;

    typedef enum {
        CHOMP_CLIP,    ///< single newline at end (default)
        CHOMP_STRIP,   ///< no newline at end     (-)
        CHOMP_KEEP     ///< all newlines from end (+)
    } BlockChomp_e;

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
    cspan _scan_ref();

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
    NodeData* _append_val(cspan const& val);
    NodeData* _append_key_val(cspan const& val);

    void  _store_scalar(cspan const& s);
    cspan _consume_scalar();
    void  _move_scalar_from_top();

    void  _save_indentation(size_t behind = 0);

    void  _resolve_references();

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

    inline NodeData * node(State const* s) const { return m_tree->get(s->node_id); }
    inline NodeData * node(State const& s) const { return m_tree->get(s .node_id); }
    inline NodeData * node(size_t node_id) const { return m_tree->get(   node_id); }

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

    cspan   m_anchor;
    size_t  m_num_anchors;
    size_t  m_num_references;
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

inline void parse(span buf, NodeRef * root)
{
    Parser np;
    np.parse(buf, root);
}
inline void parse(cspan const& filename, span buf, NodeRef * root)
{
    Parser np;
    np.parse(filename, buf, root);
}

} // namespace yml
} // namespace c4

//#include <ryml/ryml.cpp>
#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
