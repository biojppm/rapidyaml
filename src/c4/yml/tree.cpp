#include "./tree.hpp"
#include "./node.hpp"
#include "./detail/stack.hpp"

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

const char* NodeType::type_str(NodeType_e ty)
{
    switch(ty & _TYMASK)
    {
    case VAL     : return "VAL";
    case MAP     : return "MAP";
    case SEQ     : return "SEQ";
    case KEYVAL  : return "KEYVAL";
    case KEYMAP  : return "KEYMAP";
    case KEYSEQ  : return "KEYSEQ";
    case DOC     : return "DOC";
    case DOCSEQ  : return "DOCSEQ";
    case DOCMAP  : return "DOCMAP";
    case STREAM  : return "STREAM";
    case NOTYPE  : return "NOTYPE";
    default:
        if(ty & REF)
        {
            return "REF";
        }
        return "(unknown?)";
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

NodeRef Tree::rootref()
{
    return NodeRef(this, root_id());
}

NodeRef const Tree::rootref() const
{
    return NodeRef(const_cast< Tree* >(this), root_id());
}

//-----------------------------------------------------------------------------
Tree::Tree()
:
    m_buf(nullptr),
    m_cap(0),
    m_size(0),
    m_head(NONE),
    m_tail(NONE),
    m_free_head(NONE),
    m_free_tail(NONE),
    m_arena(),
    m_arena_pos(0)
{
}

Tree::Tree(size_t node_capacity, size_t arena_capacity) : Tree()
{
    reserve(node_capacity, arena_capacity);
}

Tree::~Tree()
{
    _free();
}


Tree::Tree(Tree const& that)
{
    _copy(that);
}

Tree& Tree::operator= (Tree const& that)
{
    _free();
    _copy(that);
    return *this;
}

Tree::Tree(Tree && that)
{
    _move(that);
}

Tree& Tree::operator= (Tree && that)
{
    _free();
    _move(that);
    return *this;
}

void Tree::_free()
{
    if(m_buf)
    {
        c4::yml::free(m_buf, m_cap * sizeof(NodeData));
    }
    if(m_arena.str)
    {
        c4::yml::free(m_arena.str, m_arena.len);
    }
}

void Tree::_copy(Tree const& that)
{
    memcpy(this, &that, sizeof(Tree));
    m_buf = (NodeData*)c4::yml::allocate(m_cap * sizeof(NodeData), that.m_buf);
    memcpy(m_buf, that.m_buf, m_cap * sizeof(NodeData));
    span arena((char*)c4::yml::allocate(m_arena.len, m_arena.str), m_arena.len);
    _relocate(arena); // does a memcpy and updates nodes with spans using the old arena
    m_arena = arena;
}

void Tree::_move(Tree & that)
{
    memcpy(this, &that, sizeof(Tree));
    that.m_buf = nullptr;
    that.m_arena = {};
}

void Tree::_relocate(span const& next_arena)
{
    memcpy(next_arena.str, m_arena.str, m_arena_pos);
    for(NodeData *n = m_buf, *e = m_buf + m_cap; n != e; ++n)
    {
        if(in_arena(n->m_key.scalar)) n->m_key.scalar = _relocated(n->m_key.scalar, next_arena);
        if(in_arena(n->m_key.tag   )) n->m_key.tag    = _relocated(n->m_key.tag   , next_arena);
        if(in_arena(n->m_val.scalar)) n->m_val.scalar = _relocated(n->m_val.scalar, next_arena);
        if(in_arena(n->m_val.tag   )) n->m_val.tag    = _relocated(n->m_val.tag   , next_arena);
    }
}

//-----------------------------------------------------------------------------
void Tree::reserve(size_t cap, size_t arena_cap)
{
    if(cap > m_cap)
    {
        if(m_free_head == NONE)
        {
            C4_ASSERT(m_free_tail == m_free_head);
            m_free_head = m_cap;
            m_free_tail = cap;
        }
        else
        {
            C4_ASSERT(m_buf != nullptr);
            C4_ASSERT(m_free_tail != NONE);
            m_buf[m_free_tail].m_next_sibling = m_cap;
        }
        NodeData *buf = (NodeData*)c4::yml::allocate(cap * sizeof(NodeData), m_buf);
        if(m_buf)
        {
            memcpy(buf, m_buf, m_cap * sizeof(NodeData));
            c4::yml::free(m_buf, m_cap * sizeof(NodeData));
        }
        size_t first = m_cap, del = cap - m_cap;
        m_cap = cap;
        m_buf = buf;
        _clear_range(first, del);
        if( ! m_size)
        {
            size_t r = _claim();
            _set_hierarchy(r, NONE, NONE);
            C4_ASSERT(r == 0);
            C4_ASSERT(root_id() == 0);
        }
    }

    if(arena_cap > m_arena.len)
    {
        span buf;
        buf.str = (char*)c4::yml::allocate(arena_cap, m_arena.str);
        buf.len = arena_cap;
        if(m_arena.str)
        {
            C4_ASSERT(m_arena.len >= 0);
            _relocate(buf); // does a memcpy and changes nodes using the arena
            c4::yml::free(m_arena.str, m_arena.len);
        }
        m_arena = buf;
    }
}

//-----------------------------------------------------------------------------
void Tree::clear()
{
    _clear_range(0, m_cap);
    m_size = 0;
    m_head = NONE;
    m_tail = NONE;
    m_free_head = 0;
    m_free_tail = m_cap;
    m_arena_pos = 0;
}

//-----------------------------------------------------------------------------
void Tree::_clear_range(size_t first, size_t num)
{
    if(num == 0) return; // prevent overflow when subtracting
    C4_ASSERT(first >= 0 && first + num <= m_cap);
    memset(m_buf + first, 0, num * sizeof(NodeData));
    for(size_t i = first, e = first + num; i < e; ++i)
    {
        _clear(i);
        NodeData *n = m_buf + i;
        n->m_prev_sibling = i - 1;
        n->m_next_sibling = i + 1;
    }
    m_buf[first + num - 1].m_next_sibling = NONE;
}

//-----------------------------------------------------------------------------
void Tree::_release(size_t i)
{
    C4_ASSERT(i >= 0 && i < m_cap);
    NodeData & w = m_buf[i];

    _rem_hierarchy(i);

    // add to the front of the free list
    w.m_next_sibling = m_free_head;
    w.m_prev_sibling = NONE;
    if(m_free_head != NONE)
    {
        m_buf[m_free_head].m_prev_sibling = i;
    }
    m_free_head = i;
    if(m_free_tail == NONE)
    {
        m_free_tail = m_free_head;
    }

    _clear(i);

    --m_size;
}

//-----------------------------------------------------------------------------
size_t Tree::_claim()
{
    if(m_free_head == NONE || m_buf == nullptr)
    {
        size_t sz = 2 * m_cap;
        sz = sz ? sz : 16;
        reserve(sz);
        C4_ASSERT(m_free_head != NONE);
    }

    C4_ASSERT(m_size < m_cap);
    C4_ASSERT(m_free_head >= 0 && m_free_head < m_cap);

    size_t ichild = m_free_head;
    NodeData *child = m_buf + ichild;

    ++m_size;
    m_free_head = child->m_next_sibling;
    if(m_free_head == NONE)
    {
        m_free_tail = NONE;
        C4_ASSERT(m_size == m_cap);
    }

    _clear(ichild);

    return ichild;
}

//-----------------------------------------------------------------------------
void Tree::_set_hierarchy(size_t ichild, size_t iparent, size_t iprev_sibling)
{
    C4_ASSERT(iparent == NONE || (iparent >= 0 && iparent < m_cap));
    C4_ASSERT(iprev_sibling == NONE || (iprev_sibling >= 0 && iprev_sibling < m_cap));

    NodeData * child = get(ichild);

    child->m_parent = iparent;
    child->m_prev_sibling = NONE;
    child->m_next_sibling = NONE;

    if(iparent == NONE)
    {
        C4_ASSERT(ichild == 0);
        C4_ASSERT(iprev_sibling == NONE);
        m_head = ichild;
    }
    m_tail = ichild;

    if(iparent == NONE) return;

    size_t inext_sibling = iprev_sibling != NONE ? next_sibling(iprev_sibling) : first_child(iparent);
    NodeData *parent = get(iparent);
    NodeData *psib   = get(iprev_sibling);
    NodeData *nsib   = get(inext_sibling);

    if(psib)
    {
        C4_ASSERT(next_sibling(iprev_sibling) == id(nsib));
        child->m_prev_sibling = id(psib);
        psib->m_next_sibling = id(child);
        C4_ASSERT(psib->m_prev_sibling != psib->m_next_sibling || psib->m_prev_sibling == NONE);
    }

    if(nsib)
    {
        C4_ASSERT(prev_sibling(inext_sibling) == id(psib));
        child->m_next_sibling = id(nsib);
        nsib->m_prev_sibling = id(child);
        C4_ASSERT(nsib->m_prev_sibling != nsib->m_next_sibling || nsib->m_prev_sibling == NONE);
    }

    if(parent->m_first_child == NONE)
    {
        C4_ASSERT(parent->m_last_child == NONE);
        parent->m_first_child = id(child);
        parent->m_last_child = id(child);
    }
    else
    {
        if(child->m_next_sibling == parent->m_first_child)
        {
            parent->m_first_child = id(child);
        }
        if(child->m_prev_sibling == parent->m_last_child)
        {
            parent->m_last_child = id(child);
        }
    }
}

//-----------------------------------------------------------------------------
void Tree::_rem_hierarchy(size_t i)
{
    C4_ASSERT(i >= 0 && i < m_cap);

    NodeData & w = m_buf[i];

    // remove from the parent
    if(w.m_parent != NONE)
    {
        NodeData & p = m_buf[w.m_parent];
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
        NodeData * prev = get(w.m_prev_sibling);
        prev->m_next_sibling = w.m_next_sibling;
    }
    if(w.m_next_sibling != NONE)
    {
        NodeData * next = get(w.m_next_sibling);
        next->m_prev_sibling = w.m_prev_sibling;
    }
}

//-----------------------------------------------------------------------------
void Tree::move(size_t node, size_t after)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT( ! get(node)->is_root());
    C4_ASSERT(has_sibling(node, after) && has_sibling(after, node));

    _rem_hierarchy(node);
    _set_hierarchy(node, get(node)->m_parent, after);
}

//-----------------------------------------------------------------------------
void Tree::move(size_t node, size_t new_parent, size_t after)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT(new_parent != NONE);
    C4_ASSERT( ! get(node)->is_root());

    _rem_hierarchy(node);
    _set_hierarchy(node, new_parent, after);
}

//-----------------------------------------------------------------------------
size_t Tree::duplicate(size_t node, size_t parent, size_t after)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT(parent != NONE);
    C4_ASSERT( ! get(node)->is_root());

    size_t copy = _claim();

    _copy_props(copy, node);
    _set_hierarchy(copy, parent, after);

    size_t last = NONE;
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        last = duplicate(i, copy, last);
    }

    return copy;
}

void Tree::duplicate_children(size_t node, size_t parent, size_t after)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT(parent != NONE);
    C4_ASSERT(after == NONE || has_child(parent, after));

    // don't loop using pointers as there may be a relocation
    size_t prev = after;
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        prev = duplicate(i, parent, prev);
    }
}

void Tree::duplicate_contents(size_t node, size_t where)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT(where != NONE);
    _copy_props_wo_key(where, node);
    duplicate_children(node, where, NONE);
}

void Tree::duplicate_children_no_rep(size_t node, size_t parent, size_t after)
{
    C4_ASSERT(node != NONE);
    C4_ASSERT(parent != NONE);
    C4_ASSERT(after == NONE || has_child(parent, after));

    // don't loop using pointers as there may be a relocation

    // find the position where "after" is
    size_t after_pos = NONE;
    if(after != NONE)
    {
        for(size_t i = first_child(parent), icount = 0; i != NONE; ++icount, i = next_sibling(i))
        {
            if(i == after)
            {
                after_pos = icount;
                break;
            }
        }
        C4_ASSERT(after_pos != NONE);
    }

    // for each child to be duplicated...
    size_t prev = after;
    for(size_t i = first_child(node), icount = 0; i != NONE; ++icount, i = next_sibling(i))
    {
        if(get(parent)->is_seq())
        {
            prev = duplicate(i, parent, prev);
        }
        else
        {
            C4_ASSERT(get(parent)->is_map());
            // does the parent already have a node with the same key of the current duplicate?
            size_t rep = NONE, rep_pos = NONE;
            for(size_t j = first_child(parent), jcount = 0; j != NONE; ++jcount, j = next_sibling(j))
            {
                if(get(j)->key() == get(i)->key())
                {
                    rep = j;
                    rep_pos = jcount;
                    break;
                }
            }
            if(rep == NONE) // there's no repetition; just duplicate
            {
                prev = duplicate(i, parent, prev);
            }
            else  // yes, there's a repetition
            {
                if(after_pos == NONE || rep_pos >= after_pos)
                {
                    // rep is located after the node which will be inserted
                    // and overrides the duplicate. So do nothing here: don't
                    // duplicate the child.
                    ;
                }
                else if(after_pos != NONE && rep_pos < after_pos)
                {
                    // rep is located before the node which will be inserted,
                    // and will be overridden by the duplicate. So replace it.
                    remove(rep);
                    prev = duplicate(i, parent, prev);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
namespace detail {
struct ReferenceResolver;
} // namespace detail

struct detail::ReferenceResolver
{
    struct refdata {
        bool   is_ref;
        size_t node;
        size_t prev_anchor;
        size_t target;
    };

    Tree *t;
    stack<refdata> refs;

    using const_iterator = refdata const*;

    const_iterator begin() const { return refs.begin(); }
    const_iterator end() const { return refs.end(); }

    ReferenceResolver(Tree *t_) : t(t_)
    {
        resolve();
    }

    size_t count(size_t n)
    {
        size_t c = 0;
        if(t->is_ref(n) || t->has_anchor(n))
        {
            ++c;
        }
        for(size_t ch = t->first_child(n); ch != NONE; ch = t->next_sibling(ch))
        {
            c += count(ch);
        }
        return c;
    }

    void store()
    {
        size_t nrefs = 0;
        nrefs = count(t->root_id());
        if(nrefs == 0) return;
        refs.reserve(nrefs);
        _store(t->root_id());

        size_t prev_anchor = npos;
        size_t count = 0;
        for(auto &rd : refs)
        {
            rd.prev_anchor = prev_anchor;
            if( ! rd.is_ref)
            {
                prev_anchor = count;
            }
            ++count;
        }
    }

    void _store(size_t n)
    {
        if(t->is_ref(n))
        {
            refs.push({true, n, npos, npos});
        }
        else if(t->has_anchor(n))
        {
            refs.push({false, n, npos, npos});
        }

        for(size_t ch = t->first_child(n); ch != NONE; ch = t->next_sibling(ch))
        {
            _store(ch);
        }
    }

    void resolve()
    {
        store();
        if(refs.empty()) return;

        /** from the specs: "an alias node refers to the most recent
         * node in the serialization having the specified anchor". So
         * we start looking upward from ref nodes.
         *
         * @see http://yaml.org/spec/1.2/spec.html#id2765878 */
        for(size_t i = 0, e = refs.size(); i < e; ++i)
        {
            auto & rd = refs.top(i);
            if( ! rd.is_ref) continue;
            cspan refname = t->val(rd.node).subspan(1);
            auto const* ra = &rd;
            while(ra->prev_anchor != npos)
            {
                ra = &refs[ra->prev_anchor];
                cspan anchor_name = t->anchor(ra->node);
                if(anchor_name == refname)
                {
                    rd.target = ra->node;
                    break;
                }
            }
            C4_ASSERT(rd.target != npos);
        }
    }

};

void Tree::resolve()
{
    if(m_size == 0) return;

    detail::ReferenceResolver rr(this);

    // resolve the references
    for(auto const& rd : rr)
    {
        if( ! rd.is_ref) continue;
        NodeData *n = get(rd.node);
        size_t prev = n->m_prev_sibling;
        size_t parent = n->m_parent;
        if(n->is_keyval() && n->key() == "<<")
        {
            remove(rd.node);
            duplicate_children_no_rep(rd.target, parent, prev);
        }
        else
        {
            duplicate_contents(rd.target, rd.node);
        }
    }

    // clear the anchors
    for(auto const& rd : rr)
    {
        rem_anchor(rd.node);
    }
}

//-----------------------------------------------------------------------------

size_t Tree::num_children(size_t node) const
{
    if(_p(node)->is_val()) return 0;
    size_t count = 0;
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        ++count;
    }
    return count;
}

size_t Tree::child(size_t node, size_t pos) const
{
    C4_ASSERT(node != NONE);
    if(_p(node)->is_val()) return NONE;
    size_t count = 0;
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(count++ == pos) return i;
    }
    return NONE;
}

size_t Tree::child_pos(size_t node, size_t ch) const
{
    size_t count = 0;
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(i == ch) return count;
        ++count;
    }
    return npos;
}

size_t Tree::find_child(size_t node, cspan const& name) const
{
    C4_ASSERT(node != NONE);
    if(_p(node)->is_val()) return NONE;
    C4_ASSERT(_p(node)->is_map());
    C4_ASSERT( ! name.empty());
    if(get(node)->m_first_child == NONE)
    {
        C4_ASSERT(_p(node)->m_last_child == NONE);
        return NONE;
    }
    else
    {
        C4_ASSERT(_p(node)->m_last_child != NONE);
    }
    for(size_t i = first_child(node); i != NONE; i = next_sibling(i))
    {
        if(_p(i)->m_key.scalar == name)
        {
            return i;
        }
    }
    return NONE;
}

//-----------------------------------------------------------------------------

void Tree::to_val(size_t node, cspan const& val, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    C4_ASSERT(parent(node) == NONE || ! parent_is_map(node));
    _set_flags(node, VAL|more_flags);
    _p(node)->m_key.clear();
    _p(node)->m_val = val;
}

void Tree::to_keyval(size_t node, cspan const& key, cspan const& val, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    //C4_ASSERT( ! key.empty());
    C4_ASSERT(parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEYVAL|more_flags);
    _p(node)->m_key = key;
    _p(node)->m_val = val;
}

void Tree::set_key_tag(size_t node, cspan const& tag)
{
    C4_ASSERT(has_key(node));
    _p(node)->m_key.tag = tag;
    _add_flags(node, KEYTAG);
}

void Tree::set_val_tag(size_t node, cspan const& tag)
{
    //C4_ASSERT(has_val());
    _p(node)->m_val.tag = tag;
    _add_flags(node, VALTAG);
}

void Tree::set_anchor(size_t node, cspan const& anchor)
{
    _p(node)->m_anchor = anchor;
    _add_flags(node, ANCHOR);
}

void Tree::rem_anchor(size_t node)
{
    _p(node)->m_anchor.clear();
    _rem_flags(node, ANCHOR);
}

void Tree::to_map(size_t node, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    C4_ASSERT(parent(node) == NONE || ! parent_is_map(node));
    _set_flags(node, MAP|more_flags);
    _p(node)->m_key.clear();
    _p(node)->m_val.clear();
}

void Tree::to_map(size_t node, cspan const& key, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    C4_ASSERT( ! key.empty());
    C4_ASSERT(parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEY|MAP|more_flags);
    _p(node)->m_key = key;
    _p(node)->m_val.clear();
}

void Tree::to_seq(size_t node, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    _set_flags(node, SEQ|more_flags);
    _p(node)->m_key.clear();
    _p(node)->m_val.clear();
}

void Tree::to_seq(size_t node, cspan const& key, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    C4_ASSERT(parent(node) == NONE || parent_is_map(node));
    _set_flags(node, KEY|SEQ|more_flags);
    _p(node)->m_key = key;
    _p(node)->m_val.clear();
}

void Tree::to_doc(size_t node, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    _set_flags(node, DOC|more_flags);
    _p(node)->m_key.clear();
    _p(node)->m_val.clear();
}

void Tree::to_stream(size_t node, int more_flags)
{
    C4_ASSERT( ! has_children(node));
    _set_flags(node, STREAM|more_flags);
    _p(node)->m_key.clear();
    _p(node)->m_val.clear();
}

} // namespace ryml
} // namespace c4

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif