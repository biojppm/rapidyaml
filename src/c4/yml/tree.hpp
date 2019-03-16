#ifndef _C4_YML_TREE_HPP_
#define _C4_YML_TREE_HPP_


#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

#include <c4/substr.hpp>
#include <c4/to_chars.hpp>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
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
    KEYREF  = (1<<6),     ///< a *reference: the key references an &anchor
    VALREF  = (1<<7),     ///< a *reference: the val references an &anchor
    KEYANCH = (1<<8),     ///< the key has an &anchor
    VALANCH = (1<<9),     ///< the val has an &anchor
    _TYMASK = (1<<10)-1,
    KEYTAG  = (1<<10),    ///< the key has an explicit tag/type
    VALTAG  = (1<<11),    ///< the val has an explicit tag/type
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
} NodeType_e;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** wraps a NodeType_e element with some syntactic sugar and predicates */
struct NodeType
{
public:

    NodeType_e type;

public:

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

    bool is_stream() const { return ((type & STREAM) == STREAM) != 0; }
    bool is_doc() const { return (type & DOC) != 0; }
    bool is_container() const { return (type & (MAP|SEQ|STREAM|DOC)) != 0; }
    bool is_map() const { return (type & MAP) != 0; }
    bool is_seq() const { return (type & SEQ) != 0; }
    bool has_val() const { return (type & VAL) != 0; }
    bool has_key() const { return (type & KEY) != 0; }
    bool is_val() const { return (type & KEYVAL) == VAL; }
    bool is_keyval() const { return (type & KEYVAL) == KEYVAL; }
    bool has_key_tag() const { return (type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    bool has_val_tag() const { return ((type & (VALTAG)) && (type & (VAL|MAP|SEQ))); }
    bool has_key_anchor() const { return (type & KEYANCH) != 0; }
    bool has_val_anchor() const { return (type & VALANCH) != 0; }
    bool has_anchor() const { return (type & (KEYANCH|VALANCH)) != 0; }
    bool is_key_ref() const { return (type & KEYREF) != 0; }
    bool is_val_ref() const { return (type & VALREF) != 0; }
    bool is_ref() const { return (type & (KEYREF|VALREF)) != 0; }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** a node scalar is a csubstr, which may be tagged and anchored. */
struct NodeScalar
{
    csubstr tag;
    csubstr scalar;
    csubstr anchor;

    ~NodeScalar() = default;
    NodeScalar(NodeScalar &&) = default;
    NodeScalar(NodeScalar const&) = default;
    NodeScalar& operator= (NodeScalar &&) = default;
    NodeScalar& operator= (NodeScalar const&) = default;

    /// initialize as an empty scalar
    inline NodeScalar() noexcept : tag(), scalar(), anchor() {}

    /// initialize as an untagged scalar
    inline NodeScalar(csubstr s) noexcept : tag(), scalar(s), anchor() {}
    template<size_t N>
    inline NodeScalar(const char (&s)[N]) noexcept : tag(), scalar(s), anchor() {}

    /// initialize as a tagged scalar
    inline NodeScalar(csubstr t, csubstr s) noexcept : tag(t), scalar(s), anchor() {}
    template<size_t N, size_t M>
    inline NodeScalar(const char (&t)[N], const char (&s)[N]) noexcept : tag(t), scalar(s), anchor() {}

    bool empty() const noexcept { return tag.empty() && scalar.empty() && anchor.empty(); }

    void clear() noexcept { tag.clear(); scalar.clear(); anchor.clear(); }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** convenience class to initialize nodes */
struct NodeInit
{
public:

    NodeType   type;
    NodeScalar key;
    NodeScalar val;

public:

    /// initialize as an empty node
    NodeInit() : type(NOTYPE), key(), val() {}
    /// initialize as a typed node
    NodeInit(NodeType_e t) : type(t), key(), val() {}
    /// initialize as a sequence member
    NodeInit(NodeScalar const& v) : type(VAL), key(), val(v) { _add_flags(); }
    /// initialize as a mapping member
    NodeInit(              NodeScalar const& k, NodeScalar const& v) : type(KEYVAL), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }
    /// initialize as a mapping member with explicit type
    NodeInit(NodeType_e t, NodeScalar const& k, NodeScalar const& v) : type(t     ), key(k.tag, k.scalar), val(v.tag, v.scalar) { _add_flags(); }
    /// initialize as a mapping member with explicit type (eg SEQ or MAP)
    NodeInit(NodeType_e t, NodeScalar const& k                     ) : type(t     ), key(k.tag, k.scalar), val(               ) { _add_flags(KEY); }

public:

    void clear()
    {
        memset(this, 0, sizeof(*this));
    }

    void _add_flags(int more_flags=0)
    {
        type = (type|more_flags);
        if( ! key.tag.empty()) type = (type|KEYTAG);
        if( ! val.tag.empty()) type = (type|VALTAG);
        if( ! key.anchor.empty()) type = (type|KEYANCH);
        if( ! val.anchor.empty()) type = (type|VALANCH);
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

/** contains the data for each YAML node. */
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

    size_t     m_parent;
    size_t     m_first_child;
    size_t     m_last_child;
    size_t     m_next_sibling;
    size_t     m_prev_sibling;

public:

    NodeType_e type() const { return (NodeType_e)(m_type & _TYMASK); }
    const char* type_str() const { return type_str(m_type); }
    static const char* type_str(NodeType_e ty);

    csubstr const& key() const { C4_ASSERT(has_key()); return m_key.scalar; }
    csubstr const& key_tag() const { C4_ASSERT(has_key_tag()); return m_key.tag; }
    csubstr const& key_anchor() const { return m_key.anchor; }
    NodeScalar const& keysc() const { C4_ASSERT(has_key()); return m_key; }

    csubstr const& val() const { C4_ASSERT(has_val()); return m_val.scalar; }
    csubstr const& val_tag() const { C4_ASSERT(has_val_tag()); return m_val.tag; }
    csubstr const& val_anchor() const { C4_ASSERT(has_val_tag()); return m_val.anchor; }
    NodeScalar const& valsc() const { C4_ASSERT(has_val()); return m_val; }

public:

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
    bool   has_key_anchor() const { return ! m_type.has_key_anchor(); }
    bool   has_val_anchor() const { return ! m_type.has_val_anchor(); }
    bool   is_key_ref() const { return m_type.is_key_ref(); }
    bool   is_val_ref() const { return m_type.is_val_ref(); }

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

    size_t m_free_head;
    size_t m_free_tail;

    substr m_arena;
    size_t m_arena_pos;

    Allocator m_alloc;

    friend void check_invariants(Tree const&);

public:

    Tree(Allocator const& cb={});
    Tree(size_t node_capacity, size_t arena_capacity=0, Allocator const& cb={});

    ~Tree();

    Tree(Tree const& that);
    Tree(Tree     && that);

    Tree& operator= (Tree const& that);
    Tree& operator= (Tree     && that);

public:

    void reserve(size_t node_capacity, size_t arena_capacity=0);

    /** clear the tree and zero every node
     * @note does NOT clear the arena
     * @see clear_arena() */
    void clear();
    inline void clear_arena() { m_arena_pos = 0; }

    inline bool   empty() const { return m_size == 0; }

    inline size_t size () const { return m_size; }
    inline size_t capacity() const { return m_cap; }
    inline size_t slack() const { C4_ASSERT(m_cap >= m_size); return m_cap - m_size; }

    inline size_t arena_size() const { return m_arena_pos; }
    inline size_t arena_capacity() const { return m_arena.len; }
    inline size_t arena_slack() const { C4_ASSERT(m_arena.len >= m_arena_pos); return m_arena.len - m_arena_pos; }

    Allocator const& allocator() const { return m_alloc; }

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

    NodeType_e  type(size_t node) const { return (NodeType_e)(_p(node)->m_type & _TYMASK); }
    const char* type_str(size_t node) const { return NodeType::type_str(_p(node)->m_type); }

    csubstr    const& key       (size_t node) const { C4_ASSERT(has_key(node)); return _p(node)->m_key.scalar; }
    csubstr    const& key_tag   (size_t node) const { C4_ASSERT(has_key_tag(node)); return _p(node)->m_key.tag; }
    csubstr    const& key_ref   (size_t node) const { C4_ASSERT(is_key_ref(node) && ! has_key_anchor(node)); return _p(node)->m_key.anchor; }
    csubstr    const& key_anchor(size_t node) const { C4_ASSERT( ! is_key_ref(node) && has_key_anchor(node)); return _p(node)->m_key.anchor; }
    NodeScalar const& keysc     (size_t node) const { C4_ASSERT(has_key(node)); return _p(node)->m_key; }

    csubstr    const& val       (size_t node) const { C4_ASSERT(has_val(node)); return _p(node)->m_val.scalar; }
    csubstr    const& val_tag   (size_t node) const { C4_ASSERT(has_val_tag(node)); return _p(node)->m_val.tag; }
    csubstr    const& val_ref   (size_t node) const { C4_ASSERT(is_val_ref(node) && ! has_val_anchor(node)); return _p(node)->m_val.anchor; }
    csubstr    const& val_anchor(size_t node) const { C4_ASSERT( ! is_val_ref(node) && has_val_anchor(node)); return _p(node)->m_val.anchor; }
    NodeScalar const& valsc     (size_t node) const { C4_ASSERT(has_val(node)); return _p(node)->m_val; }

    bool has_anchor(size_t node, csubstr a) const { return _p(node)->m_key.anchor == a || _p(node)->m_val.anchor == a; }

public:

    // node predicates

    bool is_root(size_t node) const { C4_ASSERT(_p(node)->m_parent != NONE || node == 0); return _p(node)->m_parent == NONE; }
    bool is_stream(size_t node) const { return (_p(node)->m_type & STREAM) == STREAM; }
    bool is_doc(size_t node) const { return (_p(node)->m_type & DOC) != 0; }
    bool is_container(size_t node) const { return (_p(node)->m_type & (MAP|SEQ|STREAM|DOC)) != 0; }
    bool is_map(size_t node) const { return (_p(node)->m_type & MAP) != 0; }
    bool is_seq(size_t node) const { return (_p(node)->m_type & SEQ) != 0; }
    bool has_val(size_t node) const { return (_p(node)->m_type & VAL) != 0; }
    bool has_key(size_t node) const { return (_p(node)->m_type & KEY) != 0; }
    bool is_val(size_t node) const { return (_p(node)->m_type & KEYVAL) == VAL; }
    bool is_keyval(size_t node) const { return (_p(node)->m_type & KEYVAL) == KEYVAL; }
    bool has_key_tag(size_t node) const { return (_p(node)->m_type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    bool has_val_tag(size_t node) const { return ((_p(node)->m_type & (VALTAG)) && (_p(node)->m_type & (VAL|MAP|SEQ))); }
    bool has_key_anchor(size_t node) const { return (_p(node)->m_type & KEYANCH) != 0; }
    bool has_val_anchor(size_t node) const { return (_p(node)->m_type & VALANCH) != 0; }
    bool is_key_ref(size_t node) const { return (_p(node)->m_type & KEYREF) != 0; }
    bool is_val_ref(size_t node) const { return (_p(node)->m_type & VALREF) != 0; }
    bool is_ref(size_t node) const { return (_p(node)->m_type & (KEYREF|VALREF)) != 0; }
    bool is_anchor(size_t node) const { return (_p(node)->m_type & (KEYANCH|VALANCH)) != 0; }

    bool parent_is_seq(size_t node) const { C4_ASSERT(has_parent(node)); return is_seq(_p(node)->m_parent); }
    bool parent_is_map(size_t node) const { C4_ASSERT(has_parent(node)); return is_map(_p(node)->m_parent); }

    /** true when name and value are empty, and has no children */
    bool empty(size_t node) const { return ! has_children(node) && _p(node)->m_key.empty() && (( ! (_p(node)->m_type & VAL)) || _p(node)->m_val.empty()); }

public:

    // hierarchy predicates

    bool has_parent(size_t node) const { return _p(node)->m_parent != NONE; }

    bool has_child(size_t node, csubstr key) const { return find_child(node, key) != npos; }
    bool has_child(size_t node, size_t ch) const { return child_pos(node, ch) != npos; }
    bool has_children(size_t node) const { return _p(node)->m_first_child != NONE; }

    bool has_sibling(size_t node, size_t sib) const { return is_root(node) ? sib==node : child_pos(_p(node)->m_parent, sib) != npos; }
    bool has_sibling(size_t node, csubstr key) const { return find_sibling(node, key) != npos; }
    /** counts with this */
    bool has_siblings(size_t /*node*/) const { return true; }
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
    size_t find_child(size_t node, csubstr const& key) const;

    /** O(#num_siblings) */
    /** counts with this */
    size_t num_siblings(size_t node) const { return is_root(node) ? 1 : num_children(_p(node)->m_parent); }
    /** does not count with this */
    size_t num_other_siblings(size_t node) const { size_t ns = num_siblings(node); C4_ASSERT(ns > 0); return ns-1; }
    size_t sibling_pos(size_t node, size_t sib) const { C4_ASSERT( ! is_root(node) || node == root_id()); return child_pos(_p(node)->m_parent, sib); }
    size_t first_sibling(size_t node) const { return is_root(node) ? node : _p(_p(node)->m_parent)->m_first_child; }
    size_t last_sibling(size_t node) const { return is_root(node) ? node : _p(_p(node)->m_parent)->m_last_child; }
    size_t sibling(size_t node, size_t pos) const { return child(_p(node)->m_parent, pos); }
    size_t find_sibling(size_t node, csubstr const& key) const { return find_child(_p(node)->m_parent, key); }

public:

    void to_keyval(size_t node, csubstr const& key, csubstr const& val, int more_flags=0);
    void to_map(size_t node, csubstr const& key, int more_flags=0);
    void to_seq(size_t node, csubstr const& key, int more_flags=0);
    void to_val(size_t node, csubstr const& val, int more_flags=0);
    void to_stream(size_t node, int more_flags=0);
    void to_map(size_t node, int more_flags=0);
    void to_seq(size_t node, int more_flags=0);
    void to_doc(size_t node, int more_flags=0);

    void set_key_tag(size_t node, csubstr const& tag) { C4_ASSERT(has_key(node)); _p(node)->m_key.tag = tag; _add_flags(node, KEYTAG); }
    void set_val_tag(size_t node, csubstr const& tag) { C4_ASSERT(has_val(node) || is_container(node)); _p(node)->m_val.tag = tag; _add_flags(node, VALTAG); }

    void set_key_anchor(size_t node, csubstr anchor) { C4_ASSERT( ! is_key_ref(node)); _p(node)->m_key.anchor = anchor; _add_flags(node, KEYANCH); }
    void set_val_anchor(size_t node, csubstr anchor) { C4_ASSERT( ! is_val_ref(node)); _p(node)->m_val.anchor = anchor; _add_flags(node, VALANCH); }
    void set_key_ref   (size_t node, csubstr ref   ) { C4_ASSERT( ! has_key_anchor(node)); _p(node)->m_key.anchor = ref; _add_flags(node, KEYREF); }
    void set_val_ref   (size_t node, csubstr ref   ) { C4_ASSERT( ! has_val_anchor(node)); _p(node)->m_val.anchor = ref; _add_flags(node, VALREF); }

    void rem_key_anchor(size_t node) { C4_ASSERT( ! is_key_ref(node)); _p(node)->m_key.anchor.clear(); _rem_flags(node, KEYANCH); }
    void rem_val_anchor(size_t node) { C4_ASSERT( ! is_val_ref(node)); _p(node)->m_val.anchor.clear(); _rem_flags(node, VALANCH); }
    void rem_key_ref   (size_t node) { C4_ASSERT( ! has_key_anchor(node)); _p(node)->m_key.anchor.clear(); _rem_flags(node, KEYREF); }
    void rem_val_ref   (size_t node) { C4_ASSERT( ! has_val_anchor(node)); _p(node)->m_val.anchor.clear(); _rem_flags(node, VALREF); }
    void rem_anchor_ref(size_t node) { _p(node)->m_key.anchor.clear(); _p(node)->m_val.anchor.clear(); _rem_flags(node, KEYANCH|VALANCH|KEYREF|VALREF); }

public:

    inline void _add_flags(size_t node, NodeType_e f) { _p(node)->m_type = (f | _p(node)->m_type); }
    inline void _add_flags(size_t node, int        f) { _p(node)->m_type = (f | _p(node)->m_type); }

    inline void _rem_flags(size_t node, NodeType_e f) { _p(node)->m_type = ((~f) & _p(node)->m_type); }
    inline void _rem_flags(size_t node, int        f) { _p(node)->m_type = ((~f) & _p(node)->m_type); }

    inline void _set_flags(size_t node, NodeType_e f) { _p(node)->m_type = f; }
    inline void _set_flags(size_t node, int        f) { _p(node)->m_type = f; }

    void _set_key(size_t node, csubstr const& key, int more_flags=0)
    {
        _p(node)->m_key.scalar = key;
        _add_flags(node, KEY|more_flags);
    }
    void _set_key(size_t node, NodeScalar const& key, int more_flags=0)
    {
        _p(node)->m_key = key;
        _add_flags(node, KEY|more_flags);
    }

    void _set_val(size_t node, csubstr const& val, int more_flags=0)
    {
        C4_ASSERT(num_children(node) == 0);
        C4_ASSERT( ! is_container(node));
        _p(node)->m_val.scalar = val;
        _add_flags(node, VAL|more_flags);
    }
    void _set_val(size_t node, NodeScalar const& val, int more_flags=0)
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

    void _seq2map(size_t node)
    {
        C4_ASSERT(is_seq(node));
        for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
        {
            NodeData *C4_RESTRICT ch = _p(i);
            if(ch->m_type.is_keyval()) continue;
            ch->m_type.add(KEY);
            ch->m_key = ch->m_val;
        }
        auto *C4_RESTRICT n = _p(node);
        n->m_type.rem(SEQ);
        n->m_type.add(MAP);
    }

    void _copy_props(size_t node, size_t that_node)
    {
        _p(node)->m_key = _p(that_node)->m_key;
        _copy_props_wo_key(node, that_node);
    }

    void _copy_props(size_t node, Tree const* that_tree, size_t that_node)
    {
        _p(node)->m_key = that_tree->_p(that_node)->m_key;
        _copy_props_wo_key(node, that_tree, that_node);
    }

    void _copy_props_wo_key(size_t node, size_t that_node)
    {
        auto *n = _p(node), *t = _p(that_node);
        n->m_type = t->m_type;
        n->m_val = t->m_val;
    }

    void _copy_props_wo_key(size_t node, Tree const* that_tree, size_t that_node)
    {
        auto      * n = _p(node);
        auto const* t = that_tree->_p(that_node);
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
        C4_ASSERT(after == NONE || (has_sibling(node, after) && has_sibling(after, node)));
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
    void   move(size_t node, size_t new_parent, size_t after);
    /** change the node's parent and position */
    size_t move(Tree * src, size_t node, size_t new_parent, size_t after);

    /** recursively duplicate the node */
    size_t duplicate(size_t node, size_t new_parent, size_t after);
    /** recursively duplicate a node from a different tree */
    size_t duplicate(Tree const* src, size_t node, size_t new_parent, size_t after);

    /** recursively duplicate the node's children (but not the node) */
    void duplicate_children(size_t node, size_t parent, size_t after);
    /** recursively duplicate the node's children (but not the node), where the node is from a different tree */
    void duplicate_children(Tree const* src, size_t node, size_t parent, size_t after);

    void duplicate_contents(size_t node, size_t where);

    /** duplicate the node's children (but not the node) in a new parent, but
     * omit repetitions where a duplicated node has the same key (in maps) or
     * value (in seqs). If one of the duplicated children has the same key
     * (in maps) or value (in seqs) as one of the parent's children, the one
     * that is placed closest to the end will prevail. */
    void duplicate_children_no_rep(size_t node, size_t parent, size_t after);

private:

    void _clear_range(size_t first, size_t num);

    size_t _claim();
    void   _claim_root();
    void   _release(size_t node);

    void _set_hierarchy(size_t node, size_t parent, size_t after_sibling);
    void _rem_hierarchy(size_t node);

public:

    size_t root_id()       { if(m_cap == 0) { reserve(16); } C4_ASSERT(m_cap > 0 && m_size > 0); return 0; }
    size_t root_id() const {                                 C4_ASSERT(m_cap > 0 && m_size > 0); return 0; }

    NodeRef       rootref();
    NodeRef const rootref() const;

    NodeRef       operator[] (csubstr key);
    NodeRef const operator[] (csubstr key) const;

    NodeRef       operator[] (size_t i);
    NodeRef const operator[] (size_t i) const;

public:

    substr arena() const { return m_arena.range(0, m_arena_pos); }
    size_t arena_pos() const { return m_arena_pos; }

    template< class T >
    csubstr to_arena(T const& a)
    {
        substr rem(m_arena.sub(m_arena_pos));
        size_t num = to_chars(rem, a);
        if(num > rem.len)
        {
            rem = _grow_arena(num);
            num = to_chars(rem, a);
            C4_ASSERT(num <= rem.len);
        }
        rem = _request_span(num);
        return rem;
    }

    bool in_arena(csubstr      s  ) const
    {
        return m_arena.contains(s);
    }

    substr alloc_arena(size_t sz)
    {
        if(sz >= arena_slack())
        {
            _grow_arena(sz - arena_slack());
        }
        substr s = _request_span(sz);
        return s;
    }

    substr copy_to_arena(csubstr s)
    {
        substr cp = alloc_arena(s.len);
        C4_ASSERT(cp.len == s.len);
        memcpy(cp.str, s.str, s.len);
        return cp;
    }

private:

    substr _grow_arena(size_t more)
    {
        size_t cap = m_arena_pos + more;
        cap = cap < 2 * m_arena.len ? 2 * m_arena.len : cap;
        cap = cap < 64 ? 64 : cap;
        reserve(m_cap, cap);
        return m_arena.sub(m_arena_pos);
    }

    substr _request_span(size_t sz)
    {
        substr s;
        s = m_arena.sub(m_arena_pos, sz);
        m_arena_pos += sz;
        return s;
    }

    substr _relocated(csubstr s, substr next_arena) const
    {
        C4_ASSERT(m_arena.contains(s));
        C4_ASSERT(m_arena.sub(0, m_arena_pos).contains(s));
        auto pos = (s.str - m_arena.str);
        substr r(next_arena.str + pos, s.len);
        C4_ASSERT(r.str - next_arena.str == pos);
        C4_ASSERT(next_arena.sub(0, m_arena_pos).contains(r));
        return r;
    }

    void _free();
    void _copy(Tree const& that);
    void _move(Tree      & that);

    void _relocate(substr const& next_arena);

};

} // namespace yml
} // namespace c4


#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#endif /* _C4_YML_TREE_HPP_ */
