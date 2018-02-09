#ifndef _C4_YML_TREE_HPP_
#define _C4_YML_TREE_HPP_

#ifndef _C4_YML_SPAN_HPP_
#include "./span.hpp"
#endif

#ifndef _C4_YML_COMMON_HPP_
#include "./common.hpp"
#endif

#ifndef _C4_YML_TO_STR_HPP_
#include "./to_str.hpp"
#endif

namespace c4 {
namespace yml {

struct NodeScalar;
struct NodeInit;
class NodeData;
class NodeRef;
class Tree;

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
        C4_ASSERT( ! is_root(node));
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

} // namespace yml
} // namespace c4

#endif /* _C4_YML_TREE_HPP_ */
