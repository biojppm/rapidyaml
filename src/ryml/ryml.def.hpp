
#ifndef _C4_RYML_HPP_
#include <ryml/ryml.hpp>
#endif

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

size_t Node::id() const
{
    return m_s->id(this);
}

Node * Node::prev_node() const
{
    return m_s->get(m_list.prev);
}

Node * Node::next_node() const
{
    return m_s->get(m_list.next);
}

Node * Node::parent() const
{
    return m_s->get(m_parent);
}

size_t Node::num_children() const
{
    size_t count = 0;
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling())
    {
        ++count;
    }
    return count;
}

Node * Node::child(size_t i) const
{
    size_t count = 0;
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling(), ++count)
    {
        if(count == i) return n;
    }
    return nullptr;
}
Node * Node::first_child() const
{
    return m_s->get(m_children.first);
}
Node * Node::last_child() const
{
    return m_s->get(m_children.last);
}

Node * Node::find_child(cspan const& name) const
{
    if(m_children.first == NONE)
    {
        C4_ASSERT(m_children.last == NONE);
        return nullptr;
    }
    else
    {
        C4_ASSERT(m_children.last != NONE);
    }
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling())
    {
        if(n->m_name == name)
        {
            return n;
        }
    }
    return nullptr;
}

size_t Node::num_siblings() const
{
    return (m_parent != NONE) ? m_s->get(m_parent)->num_children() : 0;
}

Node * Node::sibling(size_t i) const
{
    C4_ASSERT(parent() != nullptr);
    return m_s->get(m_parent)->child(i);
}

Node * Node::first_sibling() const
{
    return m_s->get(m_parent)->first_child();
}

Node * Node::last_sibling() const
{
    return m_s->get(m_parent)->last_child();
}

Node * Node::find_sibling(cspan const& name) const
{
    return m_s->get(m_parent)->find_child(name);
}

Node * Node::prev_sibling() const
{
    Node *n = m_s->get(m_siblings.prev);
    C4_ASSERT(!n || n->m_parent == m_parent);
    return n;
}

Node * Node::next_sibling() const
{
    Node *n = m_s->get(m_siblings.next);
    C4_ASSERT(!n || n->m_parent == m_parent);
    return n;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Tree::Tree()
    :
    m_buf(nullptr),
    m_num(0),
    m_head(NONE),
    m_tail(NONE),
    m_free_head(NONE),
    m_free_tail(NONE)
{
}

Tree::Tree(size_t sz) : Tree()
{
    reserve(sz);
}

Tree::~Tree()
{
    if(m_buf)
    {
        RymlCallbacks::free(m_buf, m_num * sizeof(Node));
    }
}

//-----------------------------------------------------------------------------
void Tree::reserve(size_t sz)
{
    if(sz <= m_num) return;
    Node *buf = (Node*)RymlCallbacks::allocate(sz * sizeof(Node), nullptr);
    if(m_buf)
    {
        memcpy(buf, m_buf, m_num * sizeof(Node));
        RymlCallbacks::free(m_buf, m_num * sizeof(Node));
    }
    if(m_free_head == NONE)
    {
        C4_ASSERT(m_free_tail == m_free_head);
        m_free_head = m_num;
        m_free_tail = sz;
    }
    else
    {
        C4_ASSERT(m_free_tail != NONE);
        m_buf[m_free_tail].m_list.next = m_num;
    }
    size_t first = m_num, del = sz - m_num;
    m_num = sz;
    m_buf = buf;
    clear_range(first, del);
}

//-----------------------------------------------------------------------------
void Tree::clear()
{
    clear_range(0, m_num);
    m_head = NONE;
    m_tail = NONE;
    m_free_head = 0;
    m_free_tail = m_num;
}

//-----------------------------------------------------------------------------
void Tree::clear_range(size_t first, size_t num)
{
    if(num == 0) return; // prevent overflow when subtracting
    C4_ASSERT(first >= 0 && first + num <= m_num);
    memset(m_buf + first, 0, num * sizeof(Node));
    for(size_t i = first, e = first + num; i < e; ++i)
    {
        Node *n = m_buf + i;
        n->m_list.prev = i - 1;
        n->m_list.next = i + 1;
        n->m_parent = NONE;
        n->m_children.first = NONE;
        n->m_children.last = NONE;
        n->m_siblings.prev = NONE;
        n->m_siblings.next = NONE;
    }
    m_buf[first + num - 1].m_list.next = NONE;
}

//-----------------------------------------------------------------------------
Node *Tree::claim(Node *after)
{
    C4_ASSERT(after == nullptr || (m_buf <= after && (after <= m_buf + m_num)));
    size_t last = after ? after - m_buf : m_tail;
    Node *n = claim(last, NONE);
    return n;
}

//-----------------------------------------------------------------------------
Node *Tree::claim(size_t prev, size_t next)
{
    C4_ASSERT(prev == NONE || prev >= 0 && prev < m_num);
    C4_ASSERT(next == NONE || next >= 0 && next < m_num);
    if(m_free_head == NONE)
    {
        reserve(2 * m_num);
    }
    size_t f = m_free_head;
    Node *n = m_buf + f;
    m_free_head = n->m_list.next;
    if(m_free_head == NONE)
    {
        m_free_tail = NONE;
    }
    n->m_s = this;
    n->m_list.prev = prev;
    n->m_list.next = next;
    n->m_siblings.prev = NONE;
    n->m_siblings.next = NONE;
    n->m_children.first = NONE;
    n->m_children.last = NONE;
    if(prev == NONE) m_head = f;
    else
    {
        Node *p = m_buf + prev;
        p->m_list.next = f;
    }
    if(next == NONE) m_tail = f;
    else
    {
        Node *v = m_buf + next;
        v->m_list.prev = f;
    }
    return n;
}

//-----------------------------------------------------------------------------
void Tree::set_parent(Node *parent, Node *child, Node *prev_sibling, Node *next_sibling)
{
    C4_ASSERT(child != nullptr && child >= m_buf && child < m_buf + m_num);
    C4_ASSERT(parent == nullptr || parent >= m_buf && parent < m_buf + m_num);
    child->m_parent = parent ? parent - m_buf : NONE;
    child->m_siblings.prev = NONE;
    child->m_siblings.next = NONE;
    if(prev_sibling)
    {
        child->m_siblings.prev = prev_sibling->id();
        prev_sibling->m_siblings.next = child->id();
    }
    if(next_sibling)
    {
        child->m_siblings.next = next_sibling->id();
        next_sibling->m_siblings.prev = child->id();
    }
    if( ! parent) return;
    if(parent->m_children.first == NONE)
    {
        C4_ASSERT(parent->m_children.last == NONE);
        parent->m_children.first = child->id();
        parent->m_children.last = child->id();
    }
    else
    {
        if(child->m_siblings.next == parent->m_children.first)
        {
            parent->m_children.first = child->id();
        }
        if(child->m_siblings.prev == parent->m_children.last)
        {
            parent->m_children.last = child->id();
        }
    }
}

//-----------------------------------------------------------------------------
void Tree::free(size_t i)
{
    C4_ASSERT(i >= 0 && i < m_num);
    m_buf[i].m_list.next = m_free_head;
    m_buf[i].m_list.prev = NONE;
    m_buf[m_free_head].m_list.prev = i;
    m_free_head = i;
    if(m_free_tail == NONE)
    {
        m_free_tail = m_free_head;
    }
}

} // namespace ryml
} // namespace c4
