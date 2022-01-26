#include "test_suite_events.hpp"

namespace c4 {
namespace yml {

struct EventsEmitter
{
    substr buf;
    size_t pos;
    Tree const* C4_RESTRICT m_tree;
    EventsEmitter(Tree const& tree, substr buf_) : buf(buf_), pos(), m_tree(&tree) {}
    void emit_tag(csubstr tag);
    void emit_scalar(csubstr val, bool quoted);
    void emit_key_anchor_tag(size_t node);
    void emit_val_anchor_tag(size_t node);
    void emit_events(size_t node);
    void emit_doc(size_t node);
    void emit_events();
    template<size_t N>
    C4_ALWAYS_INLINE void pr(const char (&s)[N])
    {
        if(pos + N-1 <= buf.len)
            memcpy(buf.str + pos, s, N-1);
        pos += N-1;
    }
    C4_ALWAYS_INLINE void pr(csubstr s)
    {
        if(pos + s.len <= buf.len)
            memcpy(buf.str + pos, s.str, s.len);
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
};

void EventsEmitter::emit_scalar(csubstr val, bool quoted)
{
    static constexpr const char openscalar[] = {':', '\''};
    pr(openscalar[quoted]);
    size_t prev = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        switch(val[i])
        {
        case '\n':
            prev = emit_to_esc(val, prev, i, 'n'); break;
        case '\t':
            prev = emit_to_esc(val, prev, i, 't'); break;
        case '\\':
            prev = emit_to_esc(val, prev, i, '\\'); break;
        case '\r':
            prev = emit_to_esc(val, prev, i, 'r'); break;
        case '\b':
            prev = emit_to_esc(val, prev, i, 'b'); break;
        case '\f':
            prev = emit_to_esc(val, prev, i, 'f'); break;
        case '\0':
            prev = emit_to_esc(val, prev, i, '0'); break;
        }
    }
    pr(val.sub(prev)); // print remaining portion
}

void EventsEmitter::emit_tag(csubstr tag)
{
    if(tag.begins_with('<'))
    {
        pr(tag);
    }
    else
    {
        pr('<');
        pr(tag);
        pr('>');
    }
}

void EventsEmitter::emit_key_anchor_tag(size_t node)
{
    if(m_tree->has_key_anchor(node))
    {
        pr(" &");
        pr(m_tree->key_anchor(node));
    }
    if(m_tree->has_key_tag(node))
    {
        pr(' ');
        emit_tag(m_tree->key_tag(node));
    }
}

void EventsEmitter::emit_val_anchor_tag(size_t node)
{
    if(m_tree->has_val_anchor(node))
    {
        pr(" &");
        pr(m_tree->val_anchor(node));
    }
    if(m_tree->has_val_tag(node))
    {
        pr(' ');
        emit_tag(m_tree->val_tag(node));
    }
}

void EventsEmitter::emit_events(size_t node)
{
    if(m_tree->has_key(node))
    {
        if(m_tree->is_key_ref(node))
        {
            pr("=ALI ");
            pr(m_tree->key(node));
            pr('\n');
        }
        else
        {
            pr("=VAL");
            emit_key_anchor_tag(node);
            pr(' ');
            emit_scalar(m_tree->key(node), m_tree->is_key_quoted(node));
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
            emit_scalar(m_tree->val(node), m_tree->is_val_quoted(node));
            pr('\n');
        }
    }
    else if(m_tree->is_map(node))
    {
        pr("+MAP");
        emit_val_anchor_tag(node);
        pr('\n');
        for(size_t child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
            emit_events(child);
        pr("-MAP\n");
    }
    else if(m_tree->is_seq(node))
    {
        pr("+SEQ");
        emit_val_anchor_tag(node);
        pr('\n');
        for(size_t child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
            emit_events(child);
        pr("-SEQ\n");
    }
}

void EventsEmitter::emit_doc(size_t node)
{
    if(m_tree->type(node) == NOTYPE)
        return;
    pr("+DOC");
    if(m_tree->is_val(node))
    {
        pr("\n=VAL");
        emit_val_anchor_tag(node);
        pr(' ');
        emit_scalar(m_tree->val(node), m_tree->is_val_quoted(node));
        pr('\n');
    }
    else
    {
        emit_val_anchor_tag(node);
        pr('\n');
        emit_events(node);
    }
    pr("-DOC\n");
}

void EventsEmitter::emit_events()
{
    size_t root = m_tree->root_id();
    pr("+STR\n");
    if(!m_tree->empty())
    {
        if(m_tree->is_stream(root))
            for(size_t node = m_tree->first_child(root); node != NONE; node = m_tree->next_sibling(node))
                emit_doc(node);
        else
            emit_doc(root);
    }
    pr("-STR\n");
}

size_t emit_events(substr buf, Tree const& C4_RESTRICT tree)
{
    EventsEmitter e(tree, buf);
    e.emit_events();
    return e.pos;
}

} // namespace yml
} // namespace c4
