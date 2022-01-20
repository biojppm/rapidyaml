#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/fs/fs.hpp>
#include <cstdio>

using namespace c4;
using namespace c4::yml;

// print utils
template<size_t N>
C4_ALWAYS_INLINE void pr(const char (&s)[N]) { std::fwrite(s, 1, N-1, stdout); }
C4_ALWAYS_INLINE void pr(csubstr s) { std::fwrite(s.str, 1, s.len, stdout); }
C4_ALWAYS_INLINE void pr(char c) { std::putchar(c); }


//-----------------------------------------------------------------------------

C4_ALWAYS_INLINE void emit_scalar(csubstr val, bool quoted)
{
    static constexpr const char openscalar[] = {':', '\''};
    pr(openscalar[quoted]);
    size_t pos = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        if(val[i] == '\n')
        {
            pr(val.range(pos, i));
            pr('\\');
            pr('n');
            pos = i+1;
        }
        else if(val[i] == '\t')
        {
            pr(val.range(pos, i));
            pr('\\');
            pr('t');
            pos = i+1;
        }
        else if(val[i] == '\r')
            continue;  // not really sure about this
    }
    pr(val.sub(pos)); // print remaining portion
}

C4_ALWAYS_INLINE void emit_key_anchor_tag(Tree const& C4_RESTRICT tree, size_t node)
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

C4_ALWAYS_INLINE void emit_val_anchor_tag(Tree const& C4_RESTRICT tree, size_t node)
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

void emit_events(Tree const& C4_RESTRICT tree, size_t node)
{
    // TODO: aliases
    if(tree.has_key(node))
    {
        pr("=VAL");
        emit_key_anchor_tag(tree, node);
        pr(' ');
        // ryml does not handle containers as keys
        emit_scalar(tree.key(node), tree.is_key_quoted(node));
        pr('\n');
    }
    if(tree.has_val(node))
    {
        pr("=VAL");
        emit_val_anchor_tag(tree, node);
        pr(' ');
        emit_scalar(tree.val(node), tree.is_val_quoted(node));
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

void emit_doc(Tree const& C4_RESTRICT tree, size_t node)
{
    // TODO: not sure if this is how it should be.
    // - must a +DOC always be emitted?
    // - in a +DOC, when should we add '---' ?
    pr("+DOC");
    if(tree.is_doc(node))
        pr(" ---");   // is this really optional?
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

void emit_events(Tree const& C4_RESTRICT tree)
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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, const char *argv[])
{
    if(argc != 2)
    {
        std::printf("usage: %s <path/to/file>\n", argv[0]);
        return 1;
    }

    csubstr filename = to_csubstr(argv[1]);
    C4_CHECK_MSG(fs::path_exists(filename.str), "cannot find file: %s (cwd=%s)", filename.str, fs::cwd<std::string>().c_str());

    std::string buf;
    fs::file_get_contents(filename.str, &buf);
    Tree tree;
    tree.reserve(to_substr(buf).count('\n'));
    parse_in_place(filename, to_substr(buf), &tree);

    emit_events(tree);

    return 0;
}
