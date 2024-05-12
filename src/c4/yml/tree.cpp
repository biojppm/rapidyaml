#include "c4/yml/tree.hpp"
#include "c4/yml/detail/parser_dbg.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/reference_resolver.hpp"


C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4296/*expression is always 'boolean_value'*/)
C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wtype-limits")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace c4 {
namespace yml {

#define _ryml_num_val_arrays (size_t(6u))

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

NodeRef Tree::rootref()
{
    return NodeRef(this, root_id());
}
ConstNodeRef Tree::rootref() const
{
    return ConstNodeRef(this, root_id());
}

ConstNodeRef Tree::crootref() const
{
    return ConstNodeRef(this, root_id());
}

NodeRef Tree::ref(id_type id)
{
    _RYML_CB_ASSERT(m_callbacks, id != NONE && id >= 0 && id < m_cap);
    return NodeRef(this, id);
}
ConstNodeRef Tree::ref(id_type id) const
{
    _RYML_CB_ASSERT(m_callbacks, id != NONE && id >= 0 && id < m_cap);
    return ConstNodeRef(this, id);
}
ConstNodeRef Tree::cref(id_type id) const
{
    _RYML_CB_ASSERT(m_callbacks, id != NONE && id >= 0 && id < m_cap);
    return ConstNodeRef(this, id);
}

NodeRef Tree::operator[] (csubstr key)
{
    return rootref()[key];
}
ConstNodeRef Tree::operator[] (csubstr key) const
{
    return rootref()[key];
}

NodeRef Tree::operator[] (id_type i)
{
    return rootref()[i];
}
ConstNodeRef Tree::operator[] (id_type i) const
{
    return rootref()[i];
}

NodeRef Tree::docref(id_type i)
{
    return ref(doc(i));
}
ConstNodeRef Tree::docref(id_type i) const
{
    return cref(doc(i));
}
ConstNodeRef Tree::cdocref(id_type i) const
{
    return cref(doc(i));
}


//-----------------------------------------------------------------------------

// the tree's node data arrays are allocated all into a single buffer.
// these utilities help doing that.
namespace {
template<class T>
bool _is_aligned(T const* ptr)
{
    return (reinterpret_cast<uintptr_t>(ptr) & (alignof(T) - size_t(1))) == 0;
}
size_t _next_multiple(size_t val, size_t base)
{
    return base * ((val + base - size_t(1)) / base);
}
size_t _get_padding(size_t start, size_t align)
{
    if(start & (align - size_t(1)))
        return _next_multiple(start, align) - start;
    return 0;
}
struct node_buf_data
{
    C4_NO_INLINE node_buf_data(size_t new_cap)
    {
        size_for_type = new_cap * sizeof(NodeType);
        size_for_val_align = _get_padding(size_for_type, alignof(csubstr));
        val_start = size_for_type + size_for_val_align;
        size_for_val = _ryml_num_val_arrays * new_cap * sizeof(csubstr);
        size_for_rel_align = _get_padding(val_start + size_for_val, alignof(NodeRelation));
        rel_start = val_start + size_for_val + size_for_rel_align;
        size_for_rel = new_cap * sizeof(NodeRelation);
        total_size = rel_start + size_for_rel;
    }
    size_t size_for_type;
    size_t size_for_val_align;
    size_t val_start;
    size_t size_for_val;
    size_t size_for_rel_align;
    size_t rel_start;
    size_t size_for_rel;
    size_t total_size;
};
C4_NO_INLINE void _resize_node_data(Callbacks const& cb,
                                    size_t old_cap,
                                    size_t new_cap,
                                    NodeType     *C4_RESTRICT * m_type,
                                    csubstr      *C4_RESTRICT * m_val,
                                    csubstr      *C4_RESTRICT * m_val_tag,
                                    csubstr      *C4_RESTRICT * m_val_anchor,
                                    csubstr      *C4_RESTRICT * m_key,
                                    csubstr      *C4_RESTRICT * m_key_tag,
                                    csubstr      *C4_RESTRICT * m_key_anchor,
                                    NodeRelation *C4_RESTRICT * m_relation)
{
    _RYML_CB_ASSERT(cb, new_cap > 0);
    const node_buf_data newsz(new_cap);
    uint8_t *buf = _RYML_CB_ALLOC_HINT(cb, uint8_t, (size_t)newsz.total_size, *m_type);
    _RYML_CB_CHECK(cb, buf != nullptr);
    NodeType *type      = reinterpret_cast<NodeType*>(buf);
    csubstr *val        = reinterpret_cast<csubstr*>(buf + newsz.val_start);
    csubstr *val_tag    = reinterpret_cast<csubstr*>(buf + newsz.val_start + 1 * new_cap * sizeof(csubstr));
    csubstr *val_anchor = reinterpret_cast<csubstr*>(buf + newsz.val_start + 2 * new_cap * sizeof(csubstr));
    csubstr *key        = reinterpret_cast<csubstr*>(buf + newsz.val_start + 3 * new_cap * sizeof(csubstr));
    csubstr *key_tag    = reinterpret_cast<csubstr*>(buf + newsz.val_start + 4 * new_cap * sizeof(csubstr));
    csubstr *key_anchor = reinterpret_cast<csubstr*>(buf + newsz.val_start + 5 * new_cap * sizeof(csubstr));
    NodeRelation *rel   = reinterpret_cast<NodeRelation*>(buf + newsz.rel_start);
    _RYML_CB_ASSERT(cb, _is_aligned(type));
    _RYML_CB_ASSERT(cb, _is_aligned(val));
    _RYML_CB_ASSERT(cb, _is_aligned(val_tag));
    _RYML_CB_ASSERT(cb, _is_aligned(val_anchor));
    _RYML_CB_ASSERT(cb, _is_aligned(key));
    _RYML_CB_ASSERT(cb, _is_aligned(key_tag));
    _RYML_CB_ASSERT(cb, _is_aligned(key_anchor));
    _RYML_CB_ASSERT(cb, _is_aligned(rel));
    if(old_cap)
    {
        memcpy(type, *m_type, old_cap * sizeof(NodeType));
        memcpy(val, *m_val, old_cap * sizeof(csubstr));
        memcpy(val_tag, *m_val_tag, old_cap * sizeof(csubstr));
        memcpy(val_anchor, *m_val_anchor, old_cap * sizeof(csubstr));
        memcpy(key, *m_key, old_cap * sizeof(csubstr));
        memcpy(key_tag, *m_key_tag, old_cap * sizeof(csubstr));
        memcpy(key_anchor, *m_key_anchor, old_cap * sizeof(csubstr));
        memcpy(rel, *m_relation, old_cap * sizeof(NodeRelation));
        const node_buf_data oldsz(old_cap);
        _RYML_CB_FREE(cb, *m_type, uint8_t, oldsz.total_size);
    }
    *m_type = type;
    *m_val = val;
    *m_val_tag = val_tag;
    *m_val_anchor = val_anchor;
    *m_key = key;
    *m_key_tag = key_tag;
    *m_key_anchor = key_anchor;
    *m_relation = rel;
}
} // namespace


//-----------------------------------------------------------------------------
Tree::Tree(Callbacks const& cb)
    : m_type(nullptr)
    , m_val(nullptr)
    , m_val_tag(nullptr)
    , m_val_anchor(nullptr)
    , m_key(nullptr)
    , m_key_tag(nullptr)
    , m_key_anchor(nullptr)
    , m_relation(nullptr)
    , m_cap(0)
    , m_size(0)
    , m_free_head(NONE)
    , m_free_tail(NONE)
    , m_arena()
    , m_arena_pos(0)
    , m_callbacks(cb)
    , m_tag_directives()
{
}

Tree::Tree(id_type node_capacity, size_t arena_capacity, Callbacks const& cb)
    : Tree(cb)
{
    reserve(node_capacity);
    reserve_arena(arena_capacity);
}

Tree::~Tree()
{
    _free();
}


Tree::Tree(Tree const& that) : Tree(that.m_callbacks)
{
    _copy(that);
}

Tree& Tree::operator= (Tree const& that)
{
    _free();
    m_callbacks = that.m_callbacks;
    _copy(that);
    return *this;
}

Tree::Tree(Tree && that) noexcept : Tree(that.m_callbacks)
{
    _move(that);
}

Tree& Tree::operator= (Tree && that) RYML_NOEXCEPT
{
    _free();
    m_callbacks = that.m_callbacks;
    _move(that);
    return *this;
}

void Tree::_free()
{
    if(m_type) // allocated for everything
    {
        _RYML_CB_ASSERT(m_callbacks, m_cap > 0);
        const node_buf_data oldsz(m_cap);
        _RYML_CB_FREE(m_callbacks, m_type, uint8_t, oldsz.total_size);
    }
    if(m_arena.str)
    {
        _RYML_CB_ASSERT(m_callbacks, m_arena.len > 0);
        _RYML_CB_FREE(m_callbacks, m_arena.str, char, m_arena.len);
    }
    _clear();
}

void Tree::_clear()
{
    m_type = nullptr;
    m_val = nullptr;
    m_val_tag = nullptr;
    m_val_anchor = nullptr;
    m_key = nullptr;
    m_key_tag = nullptr;
    m_key_anchor = nullptr;
    m_relation = nullptr;
    m_cap = 0;
    m_size = 0;
    m_free_head = 0;
    m_free_tail = 0;
    m_arena = {};
    m_arena_pos = 0;
    for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
        m_tag_directives[i] = {};
}

void Tree::_copy(Tree const& that)
{
    _RYML_CB_ASSERT(m_callbacks, m_type == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val_tag == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val_anchor == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key_tag == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key_anchor == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_relation == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_arena.str == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_arena.len == 0);
    if(that.m_cap)
    {
        _resize_node_data(m_callbacks,
                          0,
                          that.m_cap,
                          &m_type,
                          &m_val,
                          &m_val_tag,
                          &m_val_anchor,
                          &m_key,
                          &m_key_tag,
                          &m_key_anchor,
                          &m_relation);
        memcpy(m_type, that.m_type, (size_t)that.m_cap * sizeof(NodeType));
        memcpy(m_val, that.m_val, (size_t)that.m_cap * _ryml_num_val_arrays * sizeof(csubstr));
        memcpy(m_relation, that.m_relation, (size_t)that.m_cap * sizeof(NodeRelation));
        m_cap = that.m_cap;
        m_size = that.m_size;
        m_free_head = that.m_free_head;
        m_free_tail = that.m_free_tail;
        m_arena_pos = that.m_arena_pos;
        m_arena = that.m_arena;
    }
    if(that.m_arena.str)
    {
        _RYML_CB_ASSERT(m_callbacks, that.m_arena.len > 0);
        substr arena;
        arena.str = _RYML_CB_ALLOC_HINT(m_callbacks, char, that.m_arena.len, that.m_arena.str);
        arena.len = that.m_arena.len;
        _relocate(arena); // does a memcpy of the arena and updates nodes using the old arena
        m_arena = arena;
    }
    for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
        m_tag_directives[i] = that.m_tag_directives[i];
}

void Tree::_move(Tree & that) noexcept
{
    _RYML_CB_ASSERT(m_callbacks, m_type == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val_tag == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_val_anchor == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key_tag == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_key_anchor == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_relation == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_arena.str == nullptr);
    _RYML_CB_ASSERT(m_callbacks, m_arena.len == 0);
    m_type = that.m_type;
    m_val = that.m_val;
    m_val_tag = that.m_val_tag;
    m_val_anchor = that.m_val_anchor;
    m_key = that.m_key;
    m_key_tag = that.m_key_tag;
    m_key_anchor = that.m_key_anchor;
    m_relation = that.m_relation;
    m_cap = that.m_cap;
    m_size = that.m_size;
    m_free_head = that.m_free_head;
    m_free_tail = that.m_free_tail;
    m_arena = that.m_arena;
    m_arena_pos = that.m_arena_pos;
    for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
        m_tag_directives[i] = that.m_tag_directives[i];
    that._clear();
}

void Tree::_relocate(substr next_arena)
{
    _RYML_CB_ASSERT(m_callbacks, next_arena.not_empty());
    _RYML_CB_ASSERT(m_callbacks, next_arena.len >= m_arena.len);
    if(m_arena_pos)
        memcpy(next_arena.str, m_arena.str, m_arena_pos);
    for(id_type i = 0; i < m_cap; ++i)
    {
        const NodeType nty = m_type[i];
        if(nty & VAL)
            if(in_arena(m_val[i]))
                m_val[i] = _relocated(m_val[i], next_arena);
        if(nty & VALTAG)
            if(in_arena(m_val_tag[i]))
                m_val_tag[i] = _relocated(m_val_tag[i], next_arena);
        if(nty & (VALANCH|VALREF))
            if(in_arena(m_val_anchor[i]))
                m_val_anchor[i] = _relocated(m_val_anchor[i], next_arena);
        if(nty & KEY)
            if(in_arena(m_key[i]))
                m_key[i] = _relocated(m_key[i], next_arena);
        if(nty & KEYTAG)
            if(in_arena(m_key_tag[i]))
                m_key_tag[i] = _relocated(m_key_tag[i], next_arena);
        if(nty & (KEYANCH|KEYREF))
            if(in_arena(m_key_anchor[i]))
                m_key_anchor[i] = _relocated(m_key_anchor[i], next_arena);
    }
    for(TagDirective &C4_RESTRICT td : m_tag_directives)
    {
        if(in_arena(td.prefix))
            td.prefix = _relocated(td.prefix, next_arena);
        if(in_arena(td.handle))
            td.handle = _relocated(td.handle, next_arena);
    }
}


//-----------------------------------------------------------------------------
void Tree::reserve(id_type cap)
{
    if(cap > m_cap)
    {
        _resize_node_data(m_callbacks,
                          (size_t)m_cap,
                          (size_t)cap,
                          &m_type,
                          &m_val,
                          &m_val_tag,
                          &m_val_anchor,
                          &m_key,
                          &m_key_tag,
                          &m_key_anchor,
                          &m_relation);
        id_type first = m_cap;
        id_type del   = cap - m_cap;
        m_cap         = cap;
        _clear_range(first, del);
        if(m_free_head != NONE)
        {
            _RYML_CB_ASSERT(m_callbacks, m_val != nullptr);
            _RYML_CB_ASSERT(m_callbacks, m_free_tail != NONE);
            m_relation[m_free_tail].m_next_sibling = first;
            m_relation[first].m_prev_sibling = m_free_tail;
            m_free_tail = cap-1;
        }
        else
        {
            _RYML_CB_ASSERT(m_callbacks, m_free_tail == NONE);
            m_free_head = first;
            m_free_tail = cap-1;
        }
        _RYML_CB_ASSERT(m_callbacks, m_free_head == NONE || (m_free_head >= 0 && m_free_head < cap));
        _RYML_CB_ASSERT(m_callbacks, m_free_tail == NONE || (m_free_tail >= 0 && m_free_tail < cap));
        if( ! m_size)
            _claim_root();
    }
}


//-----------------------------------------------------------------------------
void Tree::clear()
{
    _clear_range(0, m_cap);
    m_size = 0;
    if(m_val)
    {
        _RYML_CB_ASSERT(m_callbacks, m_cap >= 0);
        m_free_head = 0;
        m_free_tail = m_cap-1;
        _claim_root();
    }
    else
    {
        m_free_head = NONE;
        m_free_tail = NONE;
    }
    for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
        m_tag_directives[i] = {};
}

void Tree::_claim_root()
{
    id_type r = _claim();
    _RYML_CB_ASSERT(m_callbacks, r == 0);
    _set_hierarchy(r, NONE, NONE);
}


//-----------------------------------------------------------------------------
void Tree::_clear_range(id_type first, id_type num)
{
    if(num == 0)
        return; // prevent overflow when subtracting
    _RYML_CB_ASSERT(m_callbacks, first >= 0 && first + num <= m_cap);
    for(id_type i = first, e = first + num; i < e; ++i)
    {
        _clear(i);
        m_relation[i].m_prev_sibling = i - 1;
        m_relation[i].m_next_sibling = i + 1;
    }
    m_relation[first + num - 1].m_next_sibling = NONE;
    // we don't need to clear the node data
}


//-----------------------------------------------------------------------------
void Tree::_release(id_type i)
{
    _RYML_CB_ASSERT(m_callbacks, i >= 0 && i < m_cap);
    _rem_hierarchy(i);
    _free_list_add(i);
    _clear(i);
    --m_size;
}

//-----------------------------------------------------------------------------
// add to the front of the free list
void Tree::_free_list_add(id_type i)
{
    _RYML_CB_ASSERT(m_callbacks, i >= 0 && i < m_cap);
    NodeRelation &C4_RESTRICT w = m_relation[i];
    w.m_parent = NONE;
    w.m_next_sibling = m_free_head;
    w.m_prev_sibling = NONE;
    if(m_free_head != NONE)
        m_relation[m_free_head].m_prev_sibling = i;
    m_free_head = i;
    if(m_free_tail == NONE)
        m_free_tail = m_free_head;
}

void Tree::_free_list_rem(id_type i)
{
    _RYML_CB_ASSERT(m_callbacks, i >= 0 && i < m_cap);
    if(m_free_head == i)
        m_free_head = m_relation[i].m_next_sibling;
    _rem_hierarchy(i);
}

//-----------------------------------------------------------------------------
id_type Tree::_claim()
{
    if(m_free_head == NONE || m_val == nullptr)
    {
        id_type sz = 2 * m_cap;
        sz = sz ? sz : 16;
        reserve(sz);
        _RYML_CB_ASSERT(m_callbacks, m_free_head != NONE);
    }
    _RYML_CB_ASSERT(m_callbacks, m_size < m_cap);
    _RYML_CB_ASSERT(m_callbacks, m_free_head >= 0 && m_free_head < m_cap);
    const id_type ichild = m_free_head;
    ++m_size;
    m_free_head = m_relation[ichild].m_next_sibling;
    if(m_free_head == NONE)
    {
        m_free_tail = NONE;
        _RYML_CB_ASSERT(m_callbacks, m_size == m_cap);
    }
    _clear(ichild);
    return ichild;
}


//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_GCC_PUSH
C4_SUPPRESS_WARNING_CLANG_PUSH
C4_SUPPRESS_WARNING_CLANG("-Wnull-dereference")
#if defined(__GNUC__)
#if (__GNUC__ >= 6)
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif
#if (__GNUC__ > 9)
C4_SUPPRESS_WARNING_GCC("-Wanalyzer-fd-leak")
#endif
#endif

void Tree::_set_hierarchy(id_type ichild, id_type iparent, id_type iprev_sibling)
{
    _RYML_CB_ASSERT(m_callbacks, m_relation != nullptr);
    _RYML_CB_ASSERT(m_callbacks, ichild >= 0 && ichild < m_cap);
    _RYML_CB_ASSERT(m_callbacks, iparent == NONE || (iparent >= 0 && iparent < m_cap));
    _RYML_CB_ASSERT(m_callbacks, iprev_sibling == NONE || (iprev_sibling >= 0 && iprev_sibling < m_cap));
    NodeRelation *C4_RESTRICT child = &m_relation[ichild];
    child->m_parent = iparent;
    child->m_prev_sibling = NONE;
    child->m_next_sibling = NONE;
    _RYML_CB_ASSERT(m_callbacks, iparent != NONE || ichild == 0);
    _RYML_CB_ASSERT(m_callbacks, iparent != NONE || iprev_sibling == NONE);
    if(iparent == NONE)
        return;
    id_type inext_sibling = iprev_sibling != NONE ? next_sibling(iprev_sibling) : first_child(iparent);
    NodeRelation *C4_RESTRICT psib = nullptr;
    NodeRelation *C4_RESTRICT nsib = nullptr;
    if(iprev_sibling != NONE)
    {
        psib = &m_relation[iprev_sibling];
        _RYML_CB_ASSERT(m_callbacks, next_sibling(iprev_sibling) == inext_sibling);
        child->m_prev_sibling = iprev_sibling;
        psib->m_next_sibling = ichild;
        _RYML_CB_ASSERT(m_callbacks, psib->m_prev_sibling != psib->m_next_sibling || psib->m_prev_sibling == NONE);
    }
    if(inext_sibling != NONE)
    {
        nsib = &m_relation[inext_sibling];
        _RYML_CB_ASSERT(m_callbacks, prev_sibling(inext_sibling) == iprev_sibling);
        child->m_next_sibling = inext_sibling;
        nsib->m_prev_sibling = ichild;
        _RYML_CB_ASSERT(m_callbacks, nsib->m_prev_sibling != nsib->m_next_sibling || nsib->m_prev_sibling == NONE);
    }
    NodeRelation *C4_RESTRICT parent = &m_relation[iparent];
    if(parent->m_first_child != NONE)
    {
        if(child->m_next_sibling == parent->m_first_child)
            parent->m_first_child = ichild;
        if(child->m_prev_sibling == parent->m_last_child)
            parent->m_last_child = ichild;
    }
    else
    {
        _RYML_CB_ASSERT(m_callbacks, parent->m_last_child == NONE);
        parent->m_first_child = ichild;
        parent->m_last_child = ichild;
    }
}

C4_SUPPRESS_WARNING_GCC_POP
C4_SUPPRESS_WARNING_CLANG_POP


//-----------------------------------------------------------------------------
void Tree::_rem_hierarchy(id_type i)
{
    _RYML_CB_ASSERT(m_callbacks, i >= 0 && i < m_cap);
    NodeRelation &C4_RESTRICT w = m_relation[i];
    // remove from the parent
    if(w.m_parent != NONE)
    {
        NodeRelation &C4_RESTRICT p = m_relation[w.m_parent];
        if(p.m_first_child == i)
        {
            p.m_first_child = w.m_next_sibling;
        }
        if(p.m_last_child == i)
        {
            p.m_last_child = w.m_prev_sibling;
        }
    }

    // remove from the used list
    if(w.m_prev_sibling != NONE)
    {
        NodeRelation *C4_RESTRICT prev = &m_relation[w.m_prev_sibling];
        prev->m_next_sibling = w.m_next_sibling;
    }
    if(w.m_next_sibling != NONE)
    {
        NodeRelation *C4_RESTRICT next = &m_relation[w.m_next_sibling];
        next->m_prev_sibling = w.m_prev_sibling;
    }
}

//-----------------------------------------------------------------------------
id_type Tree::_do_reorder(id_type *node, id_type count)
{
    // swap this node if it's not in place
    if(*node != count)
    {
        _swap(*node, count);
        *node = count;
    }
    ++count; // bump the count from this node

    // now descend in the hierarchy
    for(id_type i = first_child(*node); i != NONE; i = next_sibling(i))
    {
        // this child may have been relocated to a different index,
        // so get an updated version
        count = _do_reorder(&i, count);
    }
    return count;
}

void Tree::reorder()
{
    id_type r = root_id();
    _do_reorder(&r, 0);
}


//-----------------------------------------------------------------------------
void Tree::_swap(id_type n_, id_type m_)
{
    _RYML_CB_ASSERT(m_callbacks, (parent(n_) != NONE) || type(n_) == NOTYPE);
    _RYML_CB_ASSERT(m_callbacks, (parent(m_) != NONE) || type(m_) == NOTYPE);
    NodeType tn = type(n_);
    NodeType tm = type(m_);
    if(tn != NOTYPE && tm != NOTYPE)
    {
        _swap_props(n_, m_);
        _swap_hierarchy(n_, m_);
    }
    else if(tn == NOTYPE && tm != NOTYPE)
    {
        _copy_props(n_, m_);
        _free_list_rem(n_);
        _copy_hierarchy(n_, m_);
        _clear(m_);
        _free_list_add(m_);
    }
    else if(tn != NOTYPE && tm == NOTYPE)
    {
        _copy_props(m_, n_);
        _free_list_rem(m_);
        _copy_hierarchy(m_, n_);
        _clear(n_);
        _free_list_add(n_);
    }
    else
    {
        C4_NEVER_REACH();
    }
}

//-----------------------------------------------------------------------------
void Tree::_swap_hierarchy(id_type ia, id_type ib)
{
    if(ia == ib) return;

    for(id_type i = first_child(ia); i != NONE; i = next_sibling(i))
    {
        if(i == ib || i == ia)
            continue;
        m_relation[i].m_parent = ib;
    }

    for(id_type i = first_child(ib); i != NONE; i = next_sibling(i))
    {
        if(i == ib || i == ia)
            continue;
        m_relation[i].m_parent = ia;
    }

    auto & C4_RESTRICT a  = m_relation[ia];
    auto & C4_RESTRICT b  = m_relation[ib];
    auto & C4_RESTRICT pa = m_relation[a.m_parent];
    auto & C4_RESTRICT pb = m_relation[b.m_parent];

    if(&pa == &pb)
    {
        if((pa.m_first_child == ib && pa.m_last_child == ia)
            ||
           (pa.m_first_child == ia && pa.m_last_child == ib))
        {
            std::swap(pa.m_first_child, pa.m_last_child);
        }
        else
        {
            bool changed = false;
            if(pa.m_first_child == ia)
            {
                pa.m_first_child = ib;
                changed = true;
            }
            if(pa.m_last_child  == ia)
            {
                pa.m_last_child = ib;
                changed = true;
            }
            if(pb.m_first_child == ib && !changed)
            {
                pb.m_first_child = ia;
            }
            if(pb.m_last_child  == ib && !changed)
            {
                pb.m_last_child  = ia;
            }
        }
    }
    else
    {
        if(pa.m_first_child == ia)
            pa.m_first_child = ib;
        if(pa.m_last_child  == ia)
            pa.m_last_child  = ib;
        if(pb.m_first_child == ib)
            pb.m_first_child = ia;
        if(pb.m_last_child  == ib)
            pb.m_last_child  = ia;
    }
    std::swap(a.m_first_child , b.m_first_child);
    std::swap(a.m_last_child  , b.m_last_child);

    if(a.m_prev_sibling != ib && b.m_prev_sibling != ia &&
       a.m_next_sibling != ib && b.m_next_sibling != ia)
    {
        if(a.m_prev_sibling != NONE && a.m_prev_sibling != ib)
            m_relation[a.m_prev_sibling].m_next_sibling = ib;
        if(a.m_next_sibling != NONE && a.m_next_sibling != ib)
            m_relation[a.m_next_sibling].m_prev_sibling = ib;
        if(b.m_prev_sibling != NONE && b.m_prev_sibling != ia)
            m_relation[b.m_prev_sibling].m_next_sibling = ia;
        if(b.m_next_sibling != NONE && b.m_next_sibling != ia)
            m_relation[b.m_next_sibling].m_prev_sibling = ia;
        std::swap(a.m_prev_sibling, b.m_prev_sibling);
        std::swap(a.m_next_sibling, b.m_next_sibling);
    }
    else
    {
        if(a.m_next_sibling == ib) // n will go after m
        {
            _RYML_CB_ASSERT(m_callbacks, b.m_prev_sibling == ia);
            if(a.m_prev_sibling != NONE)
            {
                _RYML_CB_ASSERT(m_callbacks, a.m_prev_sibling != ib);
                m_relation[a.m_prev_sibling].m_next_sibling = ib;
            }
            if(b.m_next_sibling != NONE)
            {
                _RYML_CB_ASSERT(m_callbacks, b.m_next_sibling != ia);
                m_relation[b.m_next_sibling].m_prev_sibling = ia;
            }
            id_type ns = b.m_next_sibling;
            b.m_prev_sibling = a.m_prev_sibling;
            b.m_next_sibling = ia;
            a.m_prev_sibling = ib;
            a.m_next_sibling = ns;
        }
        else if(a.m_prev_sibling == ib) // m will go after n
        {
            _RYML_CB_ASSERT(m_callbacks, b.m_next_sibling == ia);
            if(b.m_prev_sibling != NONE)
            {
                _RYML_CB_ASSERT(m_callbacks, b.m_prev_sibling != ia);
                m_relation[b.m_prev_sibling].m_next_sibling = ia;
            }
            if(a.m_next_sibling != NONE)
            {
                _RYML_CB_ASSERT(m_callbacks, a.m_next_sibling != ib);
                m_relation[a.m_next_sibling].m_prev_sibling = ib;
            }
            id_type ns = b.m_prev_sibling;
            a.m_prev_sibling = b.m_prev_sibling;
            a.m_next_sibling = ib;
            b.m_prev_sibling = ia;
            b.m_next_sibling = ns;
        }
        else
        {
            C4_NEVER_REACH();
        }
    }
    _RYML_CB_ASSERT(m_callbacks, a.m_next_sibling != ia);
    _RYML_CB_ASSERT(m_callbacks, a.m_prev_sibling != ia);
    _RYML_CB_ASSERT(m_callbacks, b.m_next_sibling != ib);
    _RYML_CB_ASSERT(m_callbacks, b.m_prev_sibling != ib);

    if(a.m_parent != ib && b.m_parent != ia)
    {
        std::swap(a.m_parent, b.m_parent);
    }
    else
    {
        if(a.m_parent == ib && b.m_parent != ia)
        {
            a.m_parent = b.m_parent;
            b.m_parent = ia;
        }
        else if(a.m_parent != ib && b.m_parent == ia)
        {
            b.m_parent = a.m_parent;
            a.m_parent = ib;
        }
        else
        {
            C4_NEVER_REACH();
        }
    }
}

//-----------------------------------------------------------------------------
void Tree::_copy_hierarchy(id_type dst_, id_type src_)
{
    auto const& C4_RESTRICT src = m_relation[src_];
    auto      & C4_RESTRICT dst = m_relation[dst_];
    auto      & C4_RESTRICT prt = m_relation[src.m_parent];
    for(id_type i = src.m_first_child; i != NONE; i = next_sibling(i))
    {
        m_relation[i].m_parent = dst_;
    }
    if(src.m_prev_sibling != NONE)
    {
        m_relation[src.m_prev_sibling].m_next_sibling = dst_;
    }
    if(src.m_next_sibling != NONE)
    {
        m_relation[src.m_next_sibling].m_prev_sibling = dst_;
    }
    if(prt.m_first_child == src_)
    {
        prt.m_first_child = dst_;
    }
    if(prt.m_last_child  == src_)
    {
        prt.m_last_child  = dst_;
    }
    dst.m_parent       = src.m_parent;
    dst.m_first_child  = src.m_first_child;
    dst.m_last_child   = src.m_last_child;
    dst.m_prev_sibling = src.m_prev_sibling;
    dst.m_next_sibling = src.m_next_sibling;
}

//-----------------------------------------------------------------------------
void Tree::_swap_props(id_type n_, id_type m_)
{
    std::swap(m_type[n_], m_type[m_]);
    std::swap(m_val[n_], m_val[m_]);
    std::swap(m_val_tag[n_], m_val_tag[m_]);
    std::swap(m_val_anchor[n_], m_val_anchor[m_]);
    std::swap(m_key[n_], m_key[m_]);
    std::swap(m_key_tag[n_], m_key_tag[m_]);
    std::swap(m_key_anchor[n_], m_key_anchor[m_]);
}

//-----------------------------------------------------------------------------
void Tree::move(id_type node, id_type after)
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, node != after);
    _RYML_CB_ASSERT(m_callbacks,  ! is_root(node));
    _RYML_CB_ASSERT(m_callbacks, (after == NONE) || (has_sibling(node, after) && has_sibling(after, node)));
    _rem_hierarchy(node);
    _set_hierarchy(node, parent(node), after);
}

//-----------------------------------------------------------------------------

void Tree::move(id_type node, id_type new_parent, id_type after)
{
    _ryml_chkid(node);
    _ryml_chkid(new_parent);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, node != after);
    _RYML_CB_ASSERT(m_callbacks, new_parent != NONE);
    _RYML_CB_ASSERT(m_callbacks, new_parent != node);
    _RYML_CB_ASSERT(m_callbacks, new_parent != after);
    _RYML_CB_ASSERT(m_callbacks,  ! is_root(node));
    _rem_hierarchy(node);
    _set_hierarchy(node, new_parent, after);
}

id_type Tree::move(Tree *src, id_type node, id_type new_parent, id_type after)
{
    _ryml_chkid(node);
    _ryml_chkid(new_parent);
    _RYML_CB_ASSERT(m_callbacks, src != nullptr);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, new_parent != NONE);
    _RYML_CB_ASSERT(m_callbacks, new_parent != after);
    const id_type dup = duplicate(src, node, new_parent, after);
    src->remove(node);
    return dup;
}

void Tree::set_root_as_stream()
{
    id_type root = root_id();
    if(is_stream(root))
        return;
    // don't use _add_flags() because it's checked and will fail
    if(!has_children(root))
    {
        if(is_val(root))
        {
            m_type[root].add(SEQ);
            id_type next_doc = append_child(root);
            _copy_props_wo_key(next_doc, root);
            m_type[next_doc].add(DOC);
            m_type[next_doc].rem(SEQ);
        }
        m_type[root] = STREAM;
        return;
    }
    _RYML_CB_ASSERT(m_callbacks, !has_key(root));
    id_type next_doc = append_child(root);
    _copy_props_wo_key(next_doc, root);
    _add_flags(next_doc, DOC);
    for(id_type prev = NONE, ch = first_child(root), next = next_sibling(ch); ch != NONE; )
    {
        if(ch == next_doc)
            break;
        move(ch, next_doc, prev);
        prev = ch;
        ch = next;
        next = next_sibling(next);
    }
    m_type[root] = STREAM;
}


//-----------------------------------------------------------------------------
void Tree::remove_children(id_type node)
{
    _ryml_chkid(node);
    id_type ich = m_relation[node].m_first_child;
    while(ich != NONE)
    {
        remove_children(ich);
        _ryml_chkid(ich);
        const id_type next = m_relation[ich].m_next_sibling;
        _release(ich);
        if(ich == m_relation[node].m_last_child)
            break;
        ich = next;
    }
}

bool Tree::change_type(id_type node, NodeType type)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks, type.is_val() || type.is_map() || type.is_seq());
    _RYML_CB_ASSERT(m_callbacks, type.is_val() + type.is_map() + type.is_seq() == 1);
    _RYML_CB_ASSERT(m_callbacks, type.has_key() == has_key(node) || (has_key(node) && !type.has_key()));
    if(type.is_map() && is_map(node))
        return false;
    else if(type.is_seq() && is_seq(node))
        return false;
    else if(type.is_val() && is_val(node))
        return false;
    m_type[node] = (m_type[node] & (~(MAP|SEQ|VAL))) | type;
    remove_children(node);
    if(type.is_val() && !has_key(node))
        m_val[node].len = 0;
    return true;
}


//-----------------------------------------------------------------------------
id_type Tree::duplicate(id_type node, id_type parent, id_type after)
{
    return duplicate(this, node, parent, after);
}

id_type Tree::duplicate(Tree const* src, id_type node, id_type parent, id_type after)
{
    _RYML_CB_ASSERT(m_callbacks, src != nullptr);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, parent != NONE);
    _RYML_CB_ASSERT(m_callbacks,  ! src->is_root(node));
    id_type copy = _claim();
    _copy_props(copy, src, node);
    _set_hierarchy(copy, parent, after);
    duplicate_children(src, node, copy, NONE);
    return copy;
}


//-----------------------------------------------------------------------------
id_type Tree::duplicate_children(id_type node, id_type parent, id_type after)
{
    return duplicate_children(this, node, parent, after);
}

id_type Tree::duplicate_children(Tree const* src, id_type node, id_type parent, id_type after)
{
    _RYML_CB_ASSERT(m_callbacks, src != nullptr);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, parent != NONE);
    _RYML_CB_ASSERT(m_callbacks, after == NONE || has_child(parent, after));
    id_type prev = after;
    for(id_type i = src->first_child(node); i != NONE; i = src->next_sibling(i))
    {
        prev = duplicate(src, i, parent, prev);
    }
    return prev;
}

//-----------------------------------------------------------------------------
void Tree::duplicate_contents(id_type node, id_type where)
{
    duplicate_contents(this, node, where);
}

void Tree::duplicate_contents(Tree const *src, id_type node, id_type where)
{
    _RYML_CB_ASSERT(m_callbacks, src != nullptr);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, where != NONE);
    _copy_props_wo_key(where, src, node);
    duplicate_children(src, node, where, last_child(where));
}

//-----------------------------------------------------------------------------
id_type Tree::duplicate_children_no_rep(id_type node, id_type parent, id_type after)
{
    return duplicate_children_no_rep(this, node, parent, after);
}

id_type Tree::duplicate_children_no_rep(Tree const *src, id_type node, id_type parent, id_type after)
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, parent != NONE);
    _RYML_CB_ASSERT(m_callbacks, after == NONE || has_child(parent, after));

    // don't loop using pointers as there may be a relocation

    // find the position where "after" is
    id_type after_pos = NONE;
    if(after != NONE)
    {
        for(id_type i = first_child(parent), icount = 0; i != NONE; ++icount, i = next_sibling(i))
        {
            if(i == after)
            {
                after_pos = icount;
                break;
            }
        }
        _RYML_CB_ASSERT(m_callbacks, after_pos != NONE);
    }

    // for each child to be duplicated...
    id_type prev = after;
    for(id_type i = src->first_child(node); i != NONE; i = src->next_sibling(i))
    {
        if(is_seq(parent))
        {
            prev = duplicate(i, parent, prev);
        }
        else
        {
            _RYML_CB_ASSERT(m_callbacks, is_map(parent));
            // does the parent already have a node with key equal to that of the current duplicate?
            id_type rep = NONE, rep_pos = NONE;
            for(id_type j = first_child(parent), jcount = 0; j != NONE; ++jcount, j = next_sibling(j))
            {
                if(key(j) == key(i))
                {
                    rep = j;
                    rep_pos = jcount;
                    break;
                }
            }
            if(rep == NONE) // there is no repetition; just duplicate
            {
                prev = duplicate(src, i, parent, prev);
            }
            else  // yes, there is a repetition
            {
                if(after_pos != NONE && rep_pos < after_pos)
                {
                    // rep is located before the node which will be inserted,
                    // and will be overridden by the duplicate. So replace it.
                    remove(rep);
                    prev = duplicate(src, i, parent, prev);
                }
                else if(prev == NONE)
                {
                    // first iteration with prev = after = NONE and repetition
                    prev = rep;
                }
                else if(rep != prev)
                {
                    // rep is located after the node which will be inserted
                    // and overrides it. So move the rep into this node's place.
                    move(rep, prev);
                    prev = rep;
                }
            } // there's a repetition
        }
    }

    return prev;
}


//-----------------------------------------------------------------------------

void Tree::merge_with(Tree const *src, id_type src_node, id_type dst_node)
{
    _RYML_CB_ASSERT(m_callbacks, src != nullptr);
    if(src_node == NONE)
        src_node = src->root_id();
    if(dst_node == NONE)
        dst_node = root_id();
    _RYML_CB_ASSERT(m_callbacks, src->has_val(src_node) || src->is_seq(src_node) || src->is_map(src_node));
    if(src->has_val(src_node))
    {
        type_bits mask_src = ~STYLE; // keep the existing style if it is already a val
        if( ! has_val(dst_node))
        {
            if(has_children(dst_node))
                remove_children(dst_node);
            mask_src |= VAL_STYLE; // copy the src style
        }
        if(src->is_keyval(src_node))
        {
            _copy_props(dst_node, src, src_node, mask_src);
        }
        else
        {
            _RYML_CB_ASSERT(m_callbacks, src->is_val(src_node));
            _copy_props_wo_key(dst_node, src, src_node, mask_src);
        }
    }
    else if(src->is_seq(src_node))
    {
        if( ! is_seq(dst_node))
        {
            if(has_children(dst_node))
                remove_children(dst_node);
            _clear_type(dst_node);
            if(src->has_key(src_node))
                to_seq(dst_node, src->key(src_node));
            else
                to_seq(dst_node);
            m_type[dst_node] = src->m_type[src_node];
        }
        for(id_type sch = src->first_child(src_node); sch != NONE; sch = src->next_sibling(sch))
        {
            id_type dch = append_child(dst_node);
            _copy_props_wo_key(dch, src, sch);
            merge_with(src, sch, dch);
        }
    }
    else
    {
        _RYML_CB_ASSERT(m_callbacks, src->is_map(src_node));
        if( ! is_map(dst_node))
        {
            if(has_children(dst_node))
                remove_children(dst_node);
            _clear_type(dst_node);
            if(src->has_key(src_node))
                to_map(dst_node, src->key(src_node));
            else
                to_map(dst_node);
            m_type[dst_node] = src->m_type[src_node];
        }
        for(id_type sch = src->first_child(src_node); sch != NONE; sch = src->next_sibling(sch))
        {
            id_type dch = find_child(dst_node, src->key(sch));
            if(dch == NONE)
            {
                dch = append_child(dst_node);
                _copy_props(dch, src, sch);
            }
            merge_with(src, sch, dch);
        }
    }
}


//-----------------------------------------------------------------------------

void Tree::resolve()
{
    if(m_size == 0)
        return;
    ReferenceResolver rr;
    resolve(&rr);
}

void Tree::resolve(ReferenceResolver *C4_RESTRICT rr)
{
    if(m_size == 0)
        return;
    rr->resolve(this);
}


//-----------------------------------------------------------------------------

id_type Tree::num_children(id_type node) const
{
    id_type count = 0;
    for(id_type i = first_child(node); i != NONE; i = next_sibling(i))
        ++count;
    return count;
}

id_type Tree::child(id_type node, id_type pos) const
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    id_type count = 0;
    for(id_type i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(count++ == pos)
            return i;
    }
    return NONE;
}

id_type Tree::child_pos(id_type node, id_type ch) const
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    id_type count = 0;
    for(id_type i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(i == ch)
            return count;
        ++count;
    }
    return NONE;
}

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma GCC diagnostic ignored "-Wnull-dereference"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   if __GNUC__ >= 6
#       pragma GCC diagnostic ignored "-Wnull-dereference"
#   endif
#   if __GNUC__ > 9
#       pragma GCC diagnostic ignored "-Wanalyzer-null-dereference"
#   endif
#endif

id_type Tree::find_child(id_type node, csubstr const& name) const
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    _RYML_CB_ASSERT(m_callbacks, is_map(node));
    if(m_relation[node].m_first_child == NONE)
    {
        _RYML_CB_ASSERT(m_callbacks, m_relation[node].m_last_child == NONE);
        return NONE;
    }
    else
    {
        _RYML_CB_ASSERT(m_callbacks, m_relation[node].m_last_child != NONE);
    }
    for(id_type i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(m_key[i] == name)
        {
            return i;
        }
    }
    return NONE;
}

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

namespace {
id_type depth_desc_(Tree const& C4_RESTRICT t, id_type id, id_type currdepth=0, id_type maxdepth=0)
{
    maxdepth = currdepth > maxdepth ? currdepth : maxdepth;
    for(id_type child = t.first_child(id); child != NONE; child = t.next_sibling(child))
    {
        const id_type d = depth_desc_(t, child, currdepth+1, maxdepth);
        maxdepth = d > maxdepth ? d : maxdepth;
    }
    return maxdepth;
}
}

id_type Tree::depth_desc(id_type node) const
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    return depth_desc_(*this, node);
}

id_type Tree::depth_asc(id_type node) const
{
    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    id_type depth = 0;
    while(!is_root(node))
    {
        ++depth;
        node = parent(node);
    }
    return depth;
}


//-----------------------------------------------------------------------------

void Tree::to_val(id_type node, csubstr val, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || ! parent_is_map(node));
    _set_flags(node, VAL|more_flags);
    m_key[node].clear();
    m_key_tag[node].clear();
    m_key_anchor[node].clear();
    m_val[node] = val;
}

void Tree::to_keyval(id_type node, csubstr key, csubstr val, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEYVAL|more_flags);
    m_key[node] = key;
    m_val[node] = val;
}

void Tree::to_map(id_type node, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || ! parent_is_map(node)); // parent must not have children with keys
    _set_flags(node, MAP|more_flags);
    m_key[node].clear();
    m_key_tag[node].clear();
    m_key_anchor[node].clear();
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}

void Tree::to_map(id_type node, csubstr key, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEY|MAP|more_flags);
    m_key[node] = key;
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}

void Tree::to_seq(id_type node, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || parent_is_seq(node));
    _set_flags(node, SEQ|more_flags);
    m_key[node].clear();
    m_key_tag[node].clear();
    m_key_anchor[node].clear();
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}

void Tree::to_seq(id_type node, csubstr key, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _RYML_CB_ASSERT(m_callbacks, parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEY|SEQ|more_flags);
    m_key[node] = key;
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}

void Tree::to_doc(id_type node, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _set_flags(node, DOC|more_flags);
    m_key[node].clear();
    m_key_tag[node].clear();
    m_key_anchor[node].clear();
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}

void Tree::to_stream(id_type node, type_bits more_flags)
{
    _ryml_chkid(node);
    _RYML_CB_ASSERT(m_callbacks,  ! has_children(node));
    _set_flags(node, STREAM|more_flags);
    m_key[node].clear();
    m_key_tag[node].clear();
    m_key_anchor[node].clear();
    m_val[node].clear();
    m_val_tag[node].clear();
    m_val_anchor[node].clear();
}


//-----------------------------------------------------------------------------
id_type Tree::num_tag_directives() const
{
    // this assumes we have a very small number of tag directives
    for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
        if(m_tag_directives[i].handle.empty())
            return i;
    return RYML_MAX_TAG_DIRECTIVES;
}

void Tree::clear_tag_directives()
{
    for(TagDirective &td : m_tag_directives)
        td = {};
}

id_type Tree::add_tag_directive(TagDirective const& td)
{
    _RYML_CB_CHECK(m_callbacks, !td.handle.empty());
    _RYML_CB_CHECK(m_callbacks, !td.prefix.empty());
    _RYML_CB_CHECK(m_callbacks, td.handle.begins_with('!'));
    _RYML_CB_CHECK(m_callbacks, td.handle.ends_with('!'));
    // https://yaml.org/spec/1.2.2/#rule-ns-word-char
    _RYML_CB_CHECK(m_callbacks, td.handle == '!' || td.handle == "!!" || td.handle.trim('!').first_not_of("01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-") == npos);
    id_type pos = num_tag_directives();
    _RYML_CB_CHECK(m_callbacks, pos < RYML_MAX_TAG_DIRECTIVES);
    m_tag_directives[pos] = td;
    return pos;
}

bool Tree::add_tag_directive(csubstr directive_)
{
    TagDirective td;
    if(td.create_from_str(directive_, this))
    {
        add_tag_directive(td);
        return true;
    }
    return false;
}

size_t Tree::resolve_tag(substr output, csubstr tag, id_type node_id) const
{
    // lookup from the end. We want to find the first directive that
    // matches the tag and has a target node id leq than the given
    // node_id.
    for(id_type i = RYML_MAX_TAG_DIRECTIVES-1; i != (id_type)-1; --i)
    {
        auto const& td = m_tag_directives[i];
        if(td.handle.empty())
            continue;
        if(tag.begins_with(td.handle) && td.next_node_id <= node_id)
            return td.transform(tag, output, m_callbacks);
    }
    if(tag.begins_with('!'))
    {
        if(is_custom_tag(tag))
        {
            _RYML_CB_ERR(m_callbacks, "tag directive not found");
        }
    }
    return 0; // return 0 to signal that the tag is local and cannot be resolved
}

namespace {
csubstr _transform_tag(Tree *t, csubstr tag, id_type node)
{
    _c4dbgpf("[{}] resolving tag ~~~{}~~~", node, tag);
    size_t required_size = t->resolve_tag(substr{}, tag, node);
    if(!required_size)
    {
        if(tag.begins_with("!<"))
            tag = tag.sub(1);
        _c4dbgpf("[{}] resolved tag: ~~~{}~~~", node, tag);
        return tag;
    }
    const char *prev_arena = t->arena().str;(void)prev_arena;
    substr buf = t->alloc_arena(required_size);
    _RYML_CB_ASSERT(t->m_callbacks, t->arena().str == prev_arena);
    size_t actual_size = t->resolve_tag(buf, tag, node);
    _RYML_CB_ASSERT(t->m_callbacks, actual_size <= required_size);
    _c4dbgpf("[{}] resolved tag: ~~~{}~~~", node, buf.first(actual_size));
    return buf.first(actual_size);
}
void _resolve_tags(Tree *C4_RESTRICT t, id_type node)
{
    if(t->m_type[node] & KEYTAG)
        t->m_key_tag[node] = _transform_tag(t, t->m_key_tag[node], node);
    if(t->m_type[node] & VALTAG)
        t->m_val_tag[node] = _transform_tag(t, t->m_val_tag[node], node);
    for(id_type child = t->first_child(node); child != NONE; child = t->next_sibling(child))
        _resolve_tags(t, child);
}
size_t _count_resolved_tags_size(Tree const* C4_RESTRICT t, id_type node)
{
    size_t sz = 0;
    if(t->m_type[node] & KEYTAG)
        sz += t->resolve_tag(substr{}, t->m_key_tag[node], node);
    if(t->m_type[node] & VALTAG)
        sz += t->resolve_tag(substr{}, t->m_val_tag[node], node);
    for(id_type child = t->first_child(node); child != NONE; child = t->next_sibling(child))
        sz += _count_resolved_tags_size(t, child);
    return sz;
}
void _normalize_tags(Tree *C4_RESTRICT t, id_type node)
{
    if(t->m_type[node] & KEYTAG)
        t->m_key_tag[node] = normalize_tag(t->m_key_tag[node]);
    if(t->m_type[node] & VALTAG)
        t->m_val_tag[node] = normalize_tag(t->m_val_tag[node]);
    for(id_type child = t->first_child(node); child != NONE; child = t->next_sibling(child))
        _normalize_tags(t, child);
}
void _normalize_tags_long(Tree *C4_RESTRICT t, id_type node)
{
    if(t->m_type[node] & KEYTAG)
        t->m_key_tag[node] = normalize_tag_long(t->m_key_tag[node]);
    if(t->m_type[node] & VALTAG)
        t->m_val_tag[node] = normalize_tag_long(t->m_val_tag[node]);
    for(id_type child = t->first_child(node); child != NONE; child = t->next_sibling(child))
        _normalize_tags_long(t, child);
}
} // namespace

void Tree::resolve_tags()
{
    if(empty())
        return;
    size_t needed_size = _count_resolved_tags_size(this, root_id());
    if(needed_size)
        reserve_arena(arena_size() + needed_size);
    _resolve_tags(this, root_id());
}

void Tree::normalize_tags()
{
    if(empty())
        return;
    _normalize_tags(this, root_id());
}

void Tree::normalize_tags_long()
{
    if(empty())
        return;
    _normalize_tags_long(this, root_id());
}


//-----------------------------------------------------------------------------

csubstr Tree::lookup_result::resolved() const
{
    csubstr p = path.first(path_pos);
    if(p.ends_with('.'))
        p = p.first(p.len-1);
    return p;
}

csubstr Tree::lookup_result::unresolved() const
{
    return path.sub(path_pos);
}

void Tree::_advance(lookup_result *r, size_t more) const
{
    r->path_pos += more;
    if(r->path.sub(r->path_pos).begins_with('.'))
        ++r->path_pos;
}

Tree::lookup_result Tree::lookup_path(csubstr path, id_type start) const
{
    if(start == NONE)
        start = root_id();
    lookup_result r(path, start);
    if(path.empty())
        return r;
    _lookup_path(&r);
    if(r.target == NONE && r.closest == start)
        r.closest = NONE;
    return r;
}

id_type Tree::lookup_path_or_modify(csubstr default_value, csubstr path, id_type start)
{
    id_type target = _lookup_path_or_create(path, start);
    if(parent_is_map(target))
        to_keyval(target, key(target), default_value);
    else
        to_val(target, default_value);
    return target;
}

id_type Tree::lookup_path_or_modify(Tree const *src, id_type src_node, csubstr path, id_type start)
{
    id_type target = _lookup_path_or_create(path, start);
    merge_with(src, src_node, target);
    return target;
}

id_type Tree::_lookup_path_or_create(csubstr path, id_type start)
{
    if(start == NONE)
        start = root_id();
    lookup_result r(path, start);
    _lookup_path(&r);
    if(r.target != NONE)
    {
        C4_ASSERT(r.unresolved().empty());
        return r.target;
    }
    _lookup_path_modify(&r);
    return r.target;
}

void Tree::_lookup_path(lookup_result *r) const
{
    C4_ASSERT( ! r->unresolved().empty());
    _lookup_path_token parent{"", type(r->closest)};
    id_type node;
    do
    {
        node = _next_node(r, &parent);
        if(node != NONE)
            r->closest = node;
        if(r->unresolved().empty())
        {
            r->target = node;
            return;
        }
    } while(node != NONE);
}

void Tree::_lookup_path_modify(lookup_result *r)
{
    C4_ASSERT( ! r->unresolved().empty());
    _lookup_path_token parent{"", type(r->closest)};
    id_type node;
    do
    {
        node = _next_node_modify(r, &parent);
        if(node != NONE)
            r->closest = node;
        if(r->unresolved().empty())
        {
            r->target = node;
            return;
        }
    } while(node != NONE);
}

id_type Tree::_next_node(lookup_result * r, _lookup_path_token *parent) const
{
    _lookup_path_token token = _next_token(r, *parent);
    if( ! token)
        return NONE;

    id_type node = NONE;
    csubstr prev = token.value;
    if(token.type == MAP || token.type == SEQ)
    {
        _RYML_CB_ASSERT(m_callbacks, !token.value.begins_with('['));
        //_RYML_CB_ASSERT(m_callbacks, is_container(r->closest) || r->closest == NONE);
        _RYML_CB_ASSERT(m_callbacks, is_map(r->closest));
        node = find_child(r->closest, token.value);
    }
    else if(token.type == KEYVAL)
    {
        _RYML_CB_ASSERT(m_callbacks, r->unresolved().empty());
        if(is_map(r->closest))
            node = find_child(r->closest, token.value);
    }
    else if(token.type == KEY)
    {
        _RYML_CB_ASSERT(m_callbacks, token.value.begins_with('[') && token.value.ends_with(']'));
        token.value = token.value.offs(1, 1).trim(' ');
        id_type idx = 0;
        _RYML_CB_CHECK(m_callbacks, from_chars(token.value, &idx));
        node = child(r->closest, idx);
    }
    else
    {
        C4_NEVER_REACH();
    }

    if(node != NONE)
    {
        *parent = token;
    }
    else
    {
        csubstr p = r->path.sub(r->path_pos > 0 ? r->path_pos - 1 : r->path_pos);
        r->path_pos -= prev.len;
        if(p.begins_with('.'))
            r->path_pos -= 1u;
    }

    return node;
}

id_type Tree::_next_node_modify(lookup_result * r, _lookup_path_token *parent)
{
    _lookup_path_token token = _next_token(r, *parent);
    if( ! token)
        return NONE;

    id_type node = NONE;
    if(token.type == MAP || token.type == SEQ)
    {
        _RYML_CB_ASSERT(m_callbacks, !token.value.begins_with('['));
        //_RYML_CB_ASSERT(m_callbacks, is_container(r->closest) || r->closest == NONE);
        if( ! is_container(r->closest))
        {
            if(has_key(r->closest))
                to_map(r->closest, key(r->closest));
            else
                to_map(r->closest);
        }
        else
        {
            if(is_map(r->closest))
                node = find_child(r->closest, token.value);
            else
            {
                id_type pos = NONE;
                _RYML_CB_CHECK(m_callbacks, c4::atox(token.value, &pos));
                _RYML_CB_ASSERT(m_callbacks, pos != NONE);
                node = child(r->closest, pos);
            }
        }
        if(node == NONE)
        {
            _RYML_CB_ASSERT(m_callbacks, is_map(r->closest));
            node = append_child(r->closest);
            m_key[node] = token.value;
            m_type[node].add(KEY);
        }
    }
    else if(token.type == KEYVAL)
    {
        _RYML_CB_ASSERT(m_callbacks, r->unresolved().empty());
        if(is_map(r->closest))
        {
            node = find_child(r->closest, token.value);
            if(node == NONE)
                node = append_child(r->closest);
        }
        else
        {
            _RYML_CB_ASSERT(m_callbacks, !is_seq(r->closest));
            _add_flags(r->closest, MAP);
            node = append_child(r->closest);
        }
        m_key[node] = token.value;
        m_val[node] = "";
        m_type[node].add(KEYVAL);
    }
    else if(token.type == KEY)
    {
        _RYML_CB_ASSERT(m_callbacks, token.value.begins_with('[') && token.value.ends_with(']'));
        token.value = token.value.offs(1, 1).trim(' ');
        id_type idx;
        if( ! from_chars(token.value, &idx))
             return NONE;
        if( ! is_container(r->closest))
        {
            if(has_key(r->closest))
            {
                csubstr k = key(r->closest);
                _clear_type(r->closest);
                to_seq(r->closest, k);
            }
            else
            {
                _clear_type(r->closest);
                to_seq(r->closest);
            }
        }
        _RYML_CB_ASSERT(m_callbacks, is_container(r->closest));
        node = child(r->closest, idx);
        if(node == NONE)
        {
            _RYML_CB_ASSERT(m_callbacks, num_children(r->closest) <= idx);
            for(id_type i = num_children(r->closest); i <= idx; ++i)
            {
                node = append_child(r->closest);
                if(i < idx)
                {
                    if(is_map(r->closest))
                        to_keyval(node, /*"~"*/{}, /*"~"*/{});
                    else if(is_seq(r->closest))
                        to_val(node, /*"~"*/{});
                }
            }
        }
    }
    else
    {
        C4_NEVER_REACH();
    }

    _RYML_CB_ASSERT(m_callbacks, node != NONE);
    *parent = token;
    return node;
}

/* types of tokens:
 * - seeing "map."  ---> "map"/MAP
 * - finishing "scalar" ---> "scalar"/KEYVAL
 * - seeing "seq[n]" ---> "seq"/SEQ (--> "[n]"/KEY)
 * - seeing "[n]" ---> "[n]"/KEY
 */
Tree::_lookup_path_token Tree::_next_token(lookup_result *r, _lookup_path_token const& parent) const
{
    csubstr unres = r->unresolved();
    if(unres.empty())
        return {};

    // is it an indexation like [0], [1], etc?
    if(unres.begins_with('['))
    {
        size_t pos = unres.find(']');
        if(pos == csubstr::npos)
            return {};
        csubstr idx = unres.first(pos + 1);
        _advance(r, pos + 1);
        return {idx, KEY};
    }

    // no. so it must be a name
    size_t pos = unres.first_of(".[");
    if(pos == csubstr::npos)
    {
        _advance(r, unres.len);
        NodeType t;
        if(( ! parent) || parent.type.is_seq())
            return {unres, VAL};
        return {unres, KEYVAL};
    }

    // it's either a map or a seq
    _RYML_CB_ASSERT(m_callbacks, unres[pos] == '.' || unres[pos] == '[');
    if(unres[pos] == '.')
    {
        _RYML_CB_ASSERT(m_callbacks, pos != 0);
        _advance(r, pos + 1);
        return {unres.first(pos), MAP};
    }

    _RYML_CB_ASSERT(m_callbacks, unres[pos] == '[');
    _advance(r, pos);
    return {unres.first(pos), SEQ};
}


#undef _ryml_num_val_arrays

} // namespace ryml
} // namespace c4


C4_SUPPRESS_WARNING_GCC_CLANG_POP
C4_SUPPRESS_WARNING_MSVC_POP
