#include "test_suite_events.hpp"

namespace c4 {
namespace yml {

struct EventsEmitter
{
    substr buf;
    size_t pos;
    EventsEmitter(substr buf_) : buf(buf_), pos() {}
    void emit_scalar(csubstr val, bool quoted);
    void emit_key_anchor_tag(Tree const& C4_RESTRICT tree, size_t node);
    void emit_val_anchor_tag(Tree const& C4_RESTRICT tree, size_t node);
    void emit_events(Tree const& C4_RESTRICT tree, size_t node);
    void emit_doc(Tree const& C4_RESTRICT tree, size_t node);
    void emit_events(Tree const& C4_RESTRICT tree);
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
};

void EventsEmitter::emit_scalar(csubstr val, bool quoted)
{
    static constexpr const char openscalar[] = {':', '\''};
    pr(openscalar[quoted]);
    size_t prev = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        if(val[i] == '\n')
        {
            pr(val.range(prev, i));
            pr('\\');
            pr('n');
            prev = i+1;
        }
        else if(val[i] == '\t')
        {
            pr(val.range(prev, i));
            pr('\\');
            pr('t');
            prev = i+1;
        }
        else if(val[i] == '\r')
            continue;  // not really sure about this
    }
    pr(val.sub(prev)); // print remaining portion
}

void EventsEmitter::emit_key_anchor_tag(Tree const& C4_RESTRICT tree, size_t node)
{
    if(tree.has_key_anchor(node))
    {
        pr(" &");
        pr(tree.key_anchor(node));
    }
    if(tree.has_key_tag(node))
    {
        pr(" <");
        pr(tree.key_tag(node));
        pr('>');
    }
}

void EventsEmitter::emit_val_anchor_tag(Tree const& C4_RESTRICT tree, size_t node)
{
    if(tree.has_val_anchor(node))
    {
        pr(" &");
        pr(tree.val_anchor(node));
    }
    if(tree.has_val_tag(node))
    {
        pr(" <");
        pr(tree.val_tag(node));
        pr('>');
    }
}

void EventsEmitter::emit_events(Tree const& C4_RESTRICT tree, size_t node)
{
    // TODO: aliases
    if(tree.has_key(node))
    {
        if(tree.is_key_ref(node))
        {
            pr("=ALI ");
            pr(tree.key(node));
        }
        else
        {
            pr("=VAL");
            emit_key_anchor_tag(tree, node);
            pr(' ');
            emit_scalar(tree.key(node), tree.is_key_quoted(node));
        }
        pr('\n');
    }
    if(tree.has_val(node))
    {
        if(tree.is_val_ref(node))
        {
            pr("=ALI ");
            pr(tree.val(node));
        }
        else
        {
            pr("=VAL");
            emit_val_anchor_tag(tree, node);
            pr(' ');
            emit_scalar(tree.val(node), tree.is_val_quoted(node));
        }
        pr('\n');
    }
    else if(tree.is_map(node))
    {
        pr("+MAP");
        emit_val_anchor_tag(tree, node);
        pr('\n');
        for(size_t child = tree.first_child(node); child != NONE; child = tree.next_sibling(child))
            emit_events(tree, child);
        pr("-MAP\n");
    }
    else if(tree.is_seq(node))
    {
        pr("+SEQ");
        emit_val_anchor_tag(tree, node);
        pr('\n');
        for(size_t child = tree.first_child(node); child != NONE; child = tree.next_sibling(child))
            emit_events(tree, child);
        pr("-SEQ\n");
    }
}

void EventsEmitter::emit_doc(Tree const& C4_RESTRICT tree, size_t node)
{
    pr("+DOC");
    if(tree.is_doc(node))
        pr(" ---");
    emit_val_anchor_tag(tree, node);
    if(tree.is_val(node))
    {
        emit_val_anchor_tag(tree, node);
        pr(' ');
        emit_scalar(tree.val(node), tree.is_val_quoted(node));
        pr('\n');
    }
    else
    {
        pr('\n');
        emit_events(tree, node);
    }
    pr("-DOC\n");
}

void EventsEmitter::emit_events(Tree const& C4_RESTRICT tree)
{
    size_t root = tree.root_id();
    pr("+STR\n");
    if(tree.is_stream(root))
        for(size_t node = tree.first_child(root); node != NONE; node = tree.next_sibling(node))
            emit_doc(tree, node);
    else
        emit_doc(tree, root);
    pr("-STR\n");
}

size_t emit_events(substr buf, Tree const& C4_RESTRICT tree)
{
    EventsEmitter e(buf);
    e.emit_events(tree);
    return e.pos;
}

} // namespace yml
} // namespace c4
