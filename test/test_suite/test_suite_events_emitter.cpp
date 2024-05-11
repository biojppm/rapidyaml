#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/string.hpp>
#endif
#include "test_suite_events.hpp"

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

struct EventsEmitter
{
    substr buf;
    size_t pos;
    std::string tagbuf;
    Tree const* C4_RESTRICT m_tree;
    EventsEmitter(Tree const& tree, substr buf_) : buf(buf_), pos(), m_tree(&tree) {}
    void emit_tag(csubstr tag, id_type node);
    void emit_scalar(csubstr val, char openchar);
    void emit_key_anchor_tag(id_type node);
    void emit_val_anchor_tag(id_type node);
    void emit_events(id_type node);
    void emit_doc(id_type node);
    void emit_events();
    template<size_t N>
    C4_ALWAYS_INLINE void pr(const char (&s)[N])
    {
        if(N > 1 && pos + N-1 <= buf.len)
            memcpy(buf.str + pos, s, N-1);
        pos += N-1;
    }
    C4_ALWAYS_INLINE void pr(csubstr s)
    {
        if(s.len && pos + s.len <= buf.len)
        {
            C4_ASSERT(s.str);
            memcpy(buf.str + pos, s.str, s.len);
        }
        pos += s.len;
    }
    C4_ALWAYS_INLINE void pr(char c)
    {
        if(pos + 1 <= buf.len)
            buf[pos] = c;
        ++pos;
    }
    C4_ALWAYS_INLINE size_t emit_to_esc(csubstr val, size_t prev, size_t i, char c)
    {
        pr(val.range(prev, i));
        pr('\\');
        pr(c);
        return i+1;
    }
    C4_ALWAYS_INLINE size_t emit_to_esc(csubstr val, size_t prev, size_t i, csubstr repl)
    {
        pr(val.range(prev, i));
        pr(repl);
        return i+1;
    }
};

inline char _ev_scalar_code(NodeType masked)
{
    if(masked & SCALAR_LITERAL)
        return '|';
    if(masked & SCALAR_FOLDED)
        return '>';
    if(masked & SCALAR_SQUO)
        return '\'';
    if(masked & SCALAR_DQUO)
        return '"';
    if(masked & SCALAR_PLAIN)
        return ':';
    return ':';
}
inline char _ev_scalar_code_key(NodeType t)
{
    return _ev_scalar_code(t & KEY_STYLE);
}
inline char _ev_scalar_code_val(NodeType t)
{
    return _ev_scalar_code(t & VAL_STYLE);
}
inline char _ev_scalar_code_key(Tree const* p, id_type node)
{
    return _ev_scalar_code(p->_p(node)->m_type & KEY_STYLE);
}
inline char _ev_scalar_code_val(Tree const* p, id_type node)
{
    return _ev_scalar_code(p->_p(node)->m_type & VAL_STYLE);
}

void EventsEmitter::emit_scalar(csubstr val, char openchar)
{
    pr(openchar);
    size_t prev = 0;
    uint8_t const* C4_RESTRICT s = (uint8_t const* C4_RESTRICT) val.str;
    for(size_t i = 0; i < val.len; ++i)
    {
        switch(s[i])
        {
        case UINT8_C(0x0a): // \n
            prev = emit_to_esc(val, prev, i, 'n'); break;
        case UINT8_C(0x5c): // '\\'
            prev = emit_to_esc(val, prev, i, '\\'); break;
        case UINT8_C(0x09): // \t
            prev = emit_to_esc(val, prev, i, 't'); break;
        case UINT8_C(0x0d): // \r
            prev = emit_to_esc(val, prev, i, 'r'); break;
        case UINT8_C(0x00): // \0
            prev = emit_to_esc(val, prev, i, '0'); break;
        case UINT8_C(0x0c): // \f (form feed)
            prev = emit_to_esc(val, prev, i, 'f'); break;
        case UINT8_C(0x08): // \b (backspace)
            prev = emit_to_esc(val, prev, i, 'b'); break;
        case UINT8_C(0x07): // \a (bell)
            prev = emit_to_esc(val, prev, i, 'a'); break;
        case UINT8_C(0x0b): // \v (vertical tab)
            prev = emit_to_esc(val, prev, i, 'v'); break;
        case UINT8_C(0x1b): // \e (escape)
            prev = emit_to_esc(val, prev, i, "\\e"); break;
        case UINT8_C(0xc2):
            if(i+1 < val.len)
            {
                uint8_t np1 = s[i+1];
                if(np1 == UINT8_C(0xa0))
                    prev = 1u + emit_to_esc(val, prev, i++, "\\_");
                else if(np1 == UINT8_C(0x85))
                    prev = 1u + emit_to_esc(val, prev, i++, "\\N");
            }
            break;
        case UINT8_C(0xe2):
            if(i+2 < val.len)
            {
                if(s[i+1] == UINT8_C(0x80))
                {
                    if(s[i+2] == UINT8_C(0xa8))
                    {
                        prev = 2u + emit_to_esc(val, prev, i, "\\L");
                        i += 2u;
                    }
                    else if(s[i+2] == UINT8_C(0xa9))
                    {
                        prev = 2u + emit_to_esc(val, prev, i, "\\P");
                        i += 2u;
                    }
                }
            }
            break;
        }
    }
    pr(val.sub(prev)); // print remaining portion
}

void EventsEmitter::emit_tag(csubstr tag, id_type node)
{
    size_t tagsize = m_tree->resolve_tag(to_substr(tagbuf), tag, node);
    if(tagsize)
    {
        if(tagsize > tagbuf.size())
        {
            tagbuf.resize(tagsize);
            tagsize = m_tree->resolve_tag(to_substr(tagbuf), tag, node);
        }
        pr(to_substr(tagbuf).first(tagsize));
    }
    else
    {
        csubstr ntag = normalize_tag_long(tag, to_substr(tagbuf));
        if(!ntag.str)
        {
            tagbuf.resize(2 * ntag.len);
            ntag = normalize_tag_long(tag, to_substr(tagbuf));
        }
        if(ntag.begins_with('<'))
        {
            pr(ntag);
        }
        else
        {
            pr('<');
            pr(ntag);
            pr('>');
        }
    }
}

void EventsEmitter::emit_key_anchor_tag(id_type node)
{
    if(m_tree->has_key_anchor(node))
    {
        pr(" &");
        pr(m_tree->key_anchor(node));
    }
    if(m_tree->has_key_tag(node))
    {
        pr(' ');
        emit_tag(m_tree->key_tag(node), node);
    }
}

void EventsEmitter::emit_val_anchor_tag(id_type node)
{
    if(m_tree->has_val_anchor(node))
    {
        pr(" &");
        pr(m_tree->val_anchor(node));
    }
    if(m_tree->has_val_tag(node))
    {
        pr(' ');
        emit_tag(m_tree->val_tag(node), node);
    }
}

void EventsEmitter::emit_events(id_type node)
{
    if(m_tree->has_key(node))
    {
        if(m_tree->is_key_ref(node))
        {
            csubstr k = m_tree->key(node);
            if(k != "<<")
            {
                pr("=ALI ");
                pr(k);
                pr('\n');
            }
            else
            {
                pr("=VAL :");
                pr(k);
                pr('\n');
            }
        }
        else
        {
            pr("=VAL");
            emit_key_anchor_tag(node);
            pr(' ');
            emit_scalar(m_tree->key(node), _ev_scalar_code_key(m_tree, node));
            pr('\n');
        }
    }
    if(m_tree->has_val(node))
    {
        if(m_tree->is_val_ref(node))
        {
            pr("=ALI ");
            pr(m_tree->val(node));
            pr('\n');
        }
        else
        {
            pr("=VAL");
            emit_val_anchor_tag(node);
            pr(' ');
            emit_scalar(m_tree->val(node), _ev_scalar_code_val(m_tree, node));
            pr('\n');
        }
    }
    else if(m_tree->is_map(node))
    {
        pr((m_tree->type(node) & CONTAINER_STYLE_FLOW) ? csubstr("+MAP {}") : csubstr("+MAP"));
        emit_val_anchor_tag(node);
        pr('\n');
        for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
            emit_events(child);
        pr("-MAP\n");
    }
    else if(m_tree->is_seq(node))
    {
        pr((m_tree->type(node) & CONTAINER_STYLE_FLOW) ? csubstr("+SEQ []") : csubstr("+SEQ"));
        emit_val_anchor_tag(node);
        pr('\n');
        for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
            emit_events(child);
        pr("-SEQ\n");
    }
}

void EventsEmitter::emit_doc(id_type node)
{
    if(m_tree->type(node) == NOTYPE)
        return;
    if(m_tree->has_parent(node))
        pr("+DOC ---"); // parent must be a stream
    else
        pr("+DOC");
    if(m_tree->is_val(node))
    {
        pr("\n=VAL");
        emit_val_anchor_tag(node);
        pr(' ');
        emit_scalar(m_tree->val(node), _ev_scalar_code_val(m_tree, node));
        pr('\n');
    }
    else
    {
        pr('\n');
        emit_events(node);
    }
    pr("-DOC\n");
}

void EventsEmitter::emit_events()
{
    pr("+STR\n");
    if(!m_tree->empty())
    {
        id_type root = m_tree->root_id();
        if(m_tree->is_stream(root))
            for(id_type node = m_tree->first_child(root); node != NONE; node = m_tree->next_sibling(node))
                emit_doc(node);
        else
            emit_doc(root);
    }
    pr("-STR\n");
}

size_t emit_events_from_tree(substr buf, Tree const& C4_RESTRICT tree)
{
    EventsEmitter e(tree, buf);
    e.emit_events();
    return e.pos;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4
