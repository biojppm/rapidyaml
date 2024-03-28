#ifndef C4_YML_DETAIL_PRINT_HPP_
#define C4_YML_DETAIL_PRINT_HPP_

#include "c4/yml/tree.hpp"
#include "c4/yml/node.hpp"

#ifdef RYML_DBG
#define _c4dbg_tree(...) print_tree(__VA_ARGS__)
#define _c4dbg_node(...) print_tree(__VA_ARGS__)
#else
#define _c4dbg_tree(...)
#define _c4dbg_node(...)
#endif

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

inline const char* _container_style_code(Tree const& p, size_t node)
{
    if(p.is_container(node))
    {
        if(p._p(node)->m_type & (_WIP_STYLE_FLOW_SL|_WIP_STYLE_FLOW_ML))
        {
            return "[FLOW]";
        }
        if(p._p(node)->m_type & (_WIP_STYLE_BLOCK))
        {
            return "[BLCK]";
        }
    }
    return "";
}
inline char _scalar_code(NodeType masked)
{
    if(masked & (_WIP_KEY_LITERAL|_WIP_VAL_LITERAL))
        return '|';
    if(masked & (_WIP_KEY_FOLDED|_WIP_VAL_FOLDED))
        return '>';
    if(masked & (_WIP_KEY_SQUO|_WIP_VAL_SQUO))
        return '\'';
    if(masked & (_WIP_KEY_DQUO|_WIP_VAL_DQUO))
        return '"';
    if(masked & (_WIP_KEY_PLAIN|_WIP_VAL_PLAIN))
        return '~';
    return '@';
}
inline char _scalar_code_key(NodeType t)
{
    return _scalar_code(t & _WIP_KEY_STYLE);
}
inline char _scalar_code_val(NodeType t)
{
    return _scalar_code(t & _WIP_VAL_STYLE);
}
inline char _scalar_code_key(Tree const& p, size_t node)
{
    return _scalar_code_key(p._p(node)->m_type);
}
inline char _scalar_code_val(Tree const& p, size_t node)
{
    return _scalar_code_key(p._p(node)->m_type);
}
inline size_t print_node(Tree const& p, size_t node, int level, size_t count, bool print_children)
{
    printf("[%zd]%*s[%zd] %p", count, (2*level), "", node, (void const*)p.get(node));
    if(p.is_root(node))
    {
        printf(" [ROOT]");
    }
    printf(" %s%s:", p.type_str(node), _container_style_code(p, node));
    if(p.has_key(node))
    {
        if(p.has_key_anchor(node))
        {
            csubstr ka = p.key_anchor(node);
            printf(" &%.*s", (int)ka.len, ka.str);
        }
        if(p.has_key_tag(node))
        {
            csubstr kt = p.key_tag(node);
            csubstr k  = p.key(node);
            printf(" %.*s %c%.*s%c", (int)kt.len, kt.str, _scalar_code_key(p, node), (int)k.len, k.str, _scalar_code_key(p, node));
        }
        else
        {
            csubstr k  = p.key(node);
            printf(" %c%.*s%c", _scalar_code_key(p, node), (int)k.len, k.str, _scalar_code_key(p, node));
        }
    }
    else
    {
        RYML_ASSERT( ! p.has_key_tag(node));
    }
    if(p.has_val(node))
    {
        if(p.has_val_tag(node))
        {
            csubstr vt = p.val_tag(node);
            csubstr v  = p.val(node);
            printf(" %.*s %c%.*s%c", (int)vt.len, vt.str, _scalar_code_val(p, node), (int)v.len, v.str, _scalar_code_val(p, node));
        }
        else
        {
            csubstr v  = p.val(node);
            printf(" %c%.*s%c", _scalar_code_val(p, node), (int)v.len, v.str, _scalar_code_val(p, node));
        }
    }
    else
    {
        if(p.has_val_tag(node))
        {
            csubstr vt = p.val_tag(node);
            printf(" %c%.*s%c", _scalar_code_val(p, node), (int)vt.len, vt.str, _scalar_code_val(p, node));
        }
    }
    if(p.has_val_anchor(node))
    {
        auto &a = p.val_anchor(node);
        printf(" valanchor='&%.*s'", (int)a.len, a.str);
    }
    printf(" (%zd sibs)", p.num_siblings(node));

    ++count;

    if(p.is_container(node))
    {
        printf(" %zu children:\n", p.num_children(node));
        if(print_children)
        {
            for(size_t i = p.first_child(node); i != NONE; i = p.next_sibling(i))
            {
                count = print_node(p, i, level+1, count, print_children);
            }
        }
    }
    else
    {
        printf("\n");
    }

    return count;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline void print_node(ConstNodeRef const& p, int level=0)
{
    print_node(*p.tree(), p.id(), level, 0, true);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline size_t print_tree(const char *message, Tree const& p, size_t node=NONE)
{
    printf("--------------------------------------\n");
    if(message != nullptr)
        printf("%s:\n", message);
    size_t ret = 0;
    if(!p.empty())
    {
        if(node == NONE)
            node = p.root_id();
        ret = print_node(p, node, 0, 0, true);
    }
    printf("#nodes=%zd vs #printed=%zd\n", p.size(), ret);
    printf("--------------------------------------\n");
    return ret;
}

inline size_t print_tree(Tree const& p, size_t node=NONE)
{
    return print_tree(nullptr, p, node);
}

inline void print_tree(ConstNodeRef const& p, int level)
{
    print_node(p, level);
    for(ConstNodeRef ch : p.children())
    {
        print_tree(ch, level+1);
    }
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} /* namespace yml */
} /* namespace c4 */


#endif /* C4_YML_DETAIL_PRINT_HPP_ */
