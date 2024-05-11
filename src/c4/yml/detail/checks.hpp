#ifndef C4_YML_DETAIL_CHECKS_HPP_
#define C4_YML_DETAIL_CHECKS_HPP_

#include "c4/yml/tree.hpp"

#ifdef __clang__
#   pragma clang diagnostic push
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits" // error: comparison of unsigned expression >= 0 is always true
#elif defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4296/*expression is always 'boolean_value'*/)
#endif

namespace c4 {
namespace yml {


void check_invariants(Tree const& t, id_type node=NONE);
void check_free_list(Tree const& t);
void check_arena(Tree const& t);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline void check_invariants(Tree const& t, id_type node)
{
    if(node == NONE)
    {
        if(t.size() == 0) return;
        node = t.root_id();
    }

    NodeRelation const& n = t.m_relation[node];
#if defined(RYML_DBG) && 0
    if(n.m_first_child != NONE || n.m_last_child != NONE)
    {
        printf("check(%zu): fc=%zu lc=%zu\n", node, n.m_first_child, n.m_last_child);
    }
    else
    {
        printf("check(%zu)\n", node);
    }
#endif

    C4_CHECK(n.m_parent != node);
    if(n.m_parent == NONE)
    {
        C4_CHECK(t.is_root(node));
    }
    else //if(n.m_parent != NONE)
    {
        C4_CHECK(t.has_child(n.m_parent, node));

        NodeRelation const& p = t.m_relation[n.m_parent];
        if(n.m_prev_sibling == NONE)
        {
            C4_CHECK(p.m_first_child == node);
            C4_CHECK(t.first_sibling(node) == node);
        }
        else
        {
            C4_CHECK(p.m_first_child != node);
            C4_CHECK(t.first_sibling(node) != node);
        }

        if(n.m_next_sibling == NONE)
        {
            C4_CHECK(p.m_last_child == node);
            C4_CHECK(t.last_sibling(node) == node);
        }
        else
        {
            C4_CHECK(p.m_last_child != node);
            C4_CHECK(t.last_sibling(node) != node);
        }
    }

    C4_CHECK(n.m_first_child != node);
    C4_CHECK(n.m_last_child != node);
    if(n.m_first_child != NONE || n.m_last_child != NONE)
    {
        C4_CHECK(n.m_first_child != NONE);
        C4_CHECK(n.m_last_child != NONE);
    }

    C4_CHECK(n.m_prev_sibling != node);
    C4_CHECK(n.m_next_sibling != node);
    if(n.m_prev_sibling != NONE)
    {
        C4_CHECK(t.m_relation[n.m_prev_sibling].m_next_sibling == node);
        C4_CHECK(t.m_relation[n.m_prev_sibling].m_prev_sibling != node);
    }
    if(n.m_next_sibling != NONE)
    {
        C4_CHECK(t.m_relation[n.m_next_sibling].m_prev_sibling == node);
        C4_CHECK(t.m_relation[n.m_next_sibling].m_next_sibling != node);
    }

    id_type count = 0;
    for(id_type i = n.m_first_child; i != NONE; i = t.next_sibling(i))
    {
#if defined(RYML_DBG) && 0
        printf("check(%zu):               descend to child[%zu]=%zu\n", node, count, i);
#endif
        NodeRelation const& ch = t.m_relation[i];
        C4_CHECK(ch.m_parent == node);
        C4_CHECK(ch.m_next_sibling != i);
        ++count;
    }
    C4_CHECK(count == t.num_children(node));

    if(n.m_prev_sibling == NONE && n.m_next_sibling == NONE)
    {
        if(n.m_parent != NONE)
        {
            C4_CHECK(t.num_children(n.m_parent) == 1);
            C4_CHECK(t.num_siblings(node) == 1);
        }
    }

    if(node == t.root_id())
    {
        C4_CHECK(t.size() == t.m_size);
        C4_CHECK(t.capacity() == t.m_cap);
        C4_CHECK(t.m_cap == t.m_size + t.slack());
        check_free_list(t);
        check_arena(t);
    }

    for(id_type i = t.first_child(node); i != NONE; i = t.next_sibling(i))
    {
        check_invariants(t, i);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline void check_free_list(Tree const& t)
{
    if(t.m_free_head == NONE)
    {
        C4_CHECK(t.m_free_tail == t.m_free_head);
        return;
    }

    C4_CHECK(t.m_free_head >= 0 && t.m_free_head < t.m_cap);
    C4_CHECK(t.m_free_tail >= 0 && t.m_free_tail < t.m_cap);

    NodeRelation const& head = t.m_relation[t.m_free_head];
    //NodeRelation const& tail = t.m_relation[t.m_free_tail];

    //C4_CHECK(head.m_prev_sibling == NONE);
    //C4_CHECK(tail.m_next_sibling == NONE);

    id_type count = 0;
    for(id_type i = t.m_free_head, prev = NONE; i != NONE; i = t.m_relation[i].m_next_sibling)
    {
        NodeRelation const& elm = t.m_relation[i];
        if(&elm != &head)
        {
            C4_CHECK(elm.m_prev_sibling == prev);
        }
        prev = i;
        ++count;
    }
    C4_CHECK(count == t.slack());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline void check_arena(Tree const& t)
{
    C4_CHECK(t.m_arena.len == 0 || (t.m_arena_pos >= 0 && t.m_arena_pos <= t.m_arena.len));
    C4_CHECK(t.arena_size() == t.m_arena_pos);
    C4_CHECK(t.arena_slack() + t.m_arena_pos == t.m_arena.len);
}


} /* namespace yml */
} /* namespace c4 */

#ifdef __clang__
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* C4_YML_DETAIL_CHECKS_HPP_ */
