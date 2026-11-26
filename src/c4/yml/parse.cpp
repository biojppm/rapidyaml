#include "c4/yml/parse.hpp"

#ifndef C4_YML_NODE_HPP_
#include "c4/yml/node.hpp"
#endif
#ifndef C4_YML_PARSE_ENGINE_HPP_
#include "c4/yml/parse_engine.hpp"
#endif
#ifndef C4_YML_PARSE_ENGINE_DEF_HPP_
#include "c4/yml/parse_engine.def.hpp"
#endif
#ifndef C4_YML_EVENT_HANDLER_TREE_HPP_
#include "c4/yml/event_handler_tree.hpp"
#endif


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {


// instantiate the parser class
template class RYML_EXPORT ParseEngine<EventHandlerTree>;


namespace {
// so many things can go wrong...
void check_(Tree *tree)
{
    if C4_UNLIKELY(!tree)
        RYML_ERR_BASIC_("null tree");
    if C4_UNLIKELY(tree->empty())
        tree->reserve();
}
void check_(Tree *tree, id_type id)
{
    check_(tree);
    if C4_UNLIKELY(id == NONE || id >= tree->m_first_unused )
        RYML_ERR_VISIT_CB_(tree->m_callbacks, tree, id, "invalid node");
}
void check_(NodeRef &node)
{
    check_(node.tree(), node.id());
    node.create();
}
void check_(Parser *parser)
{
    if C4_UNLIKELY(!parser)
        RYML_ERR_BASIC_("null parser");
    if C4_UNLIKELY(!parser->m_evt_handler)
        // the parser callbacks are from the handler. do not use parser->callbacks()
        RYML_ERR_BASIC_("null handler");
}
void check_(Parser *parser, Tree *tree)
{
    if C4_UNLIKELY(!parser && !tree)
    {
        RYML_ERR_BASIC_("null parser and tree");
    }
    else if C4_UNLIKELY(!parser)
    {
        RYML_ERR_BASIC_CB_(tree->callbacks(), "null parser");
    }
    else if C4_UNLIKELY(!tree)
    {
        if C4_UNLIKELY(!parser->m_evt_handler)
            RYML_ERR_BASIC_("null tree and handler");
        else
            RYML_ERR_BASIC_CB_(parser->callbacks(), "null tree");
    }
    if C4_UNLIKELY(!parser->m_evt_handler)
    {
        RYML_ERR_BASIC_("null handler");
    }
    if C4_UNLIKELY(tree->empty())
    {
        tree->reserve();
    }
}
void check_(Parser *parser, NodeRef &node)
{
    check_(parser, node.tree());
    check_(node);
}
void checksrc_(Tree *tree, csubstr src)
{
    if C4_UNLIKELY(src.len && !src.str)
        RYML_ERR_BASIC_CB_(tree->callbacks(), "null source buffer");
}
substr cpsrc_(Tree *tree, csubstr src)
{
    checksrc_(tree, src);
    return tree->copy_to_arena(src);
}
// helpers: check and copy to arena
substr checkcp_(Tree *tree, csubstr src)
{
    check_(tree);
    return cpsrc_(tree, src);
}
substr checkcp_(NodeRef &node, csubstr src)
{
    check_(node);
    return cpsrc_(node.tree(), src);
}
substr checkcp_(Parser *parser, Tree *tree, csubstr src)
{
    check_(parser, tree);
    return cpsrc_(tree, src);
}
substr checkcp_(Parser *parser, NodeRef &node, csubstr src)
{
    check_(parser, node);
    return cpsrc_(node.tree(), src);
}
using Handler = Parser::handler_type;
struct TmpParser
{
    Handler handler;
    Parser  parser;
    // assumes checks above were done prior to instantiation
    TmpParser(ParserOptions const& opts={}) // NOLINT
        : handler(get_callbacks())
        , parser(&handler, opts)
    {
    }
    TmpParser(Tree* tree, ParserOptions const& opts={}) // NOLINT
        : handler(tree->callbacks())
        , parser(&handler, opts)
    {
    }
    TmpParser(NodeRef &node, ParserOptions const& opts={}) // NOLINT
        : handler(node.tree()->callbacks())
        , parser(&handler, opts)
    {
    }
};
// assumes checks above were done prior to calling
C4_ALWAYS_INLINE void reset_handler_(Parser *parser, Tree *tree, id_type node_id)
{
    RYML_ASSERT_BASIC_(parser); // LCOV_EXCL_LINE lcov weirdly fails here
    RYML_ASSERT_BASIC_(parser->m_evt_handler);
    RYML_ASSERT_BASIC_(tree);
    if C4_UNLIKELY(node_id == NONE || node_id >= tree->capacity())
        RYML_ERR_VISIT_CB_(tree->m_callbacks, tree, node_id, "invalid node");
    parser->m_evt_handler->reset(tree, node_id);
    RYML_ASSERT_BASIC_(parser->m_evt_handler->m_tree == tree);
}
enum : size_t { // NOLINT
    kib_ = 1024,
    /** the sampled estimate reads this many windows of the source:
     * at the start, at one third, at two thirds and at the end */
    est_num_windows_ = 4,
    /** the bytes in each window. With 4 windows the sample is 64 KiB,
     * which takes about 0.1 ms to count */
    est_window_size_ = 16 * kib_,
};
/** estimate the nodes of src from a sample of it: count the nodes in
 * each window, then scale the count to the size of src. */
uint64_t estimate_tree_capacity_sampled_(csubstr src)
{
    const size_t sample_size = est_num_windows_ * est_window_size_;
    if(src.len <= sample_size)
        return estimate_tree_capacity(src);
    id_type count = 0;
    for(size_t k = 0; k < est_num_windows_; ++k)
    {
        const size_t pos = k * ((src.len - est_window_size_) / (est_num_windows_ - 1));
        count += estimate_tree_capacity(src.sub(pos, est_window_size_)) - 1; // -1: each window counts a root
    }
    // count * src.len does not fit in 32 bits for a source of a few
    // MB, so scale in 64 bits even where id_type is 32 bits
    return 1u + ((static_cast<uint64_t>(count) * static_cast<uint64_t>(src.len)) / static_cast<uint64_t>(sample_size));
}
/** when the options ask for it, reserve the nodes that the source
 * will add to the tree. This is called before the event handler is
 * reset, because the handler keeps a pointer into the node array. */
C4_ALWAYS_INLINE void reserve_from_source_(Parser const* parser, Tree *tree, csubstr src)
{
    if(parser->options().estimate_capacity()
       && src.len >= parser->options().estimate_capacity_min_size())
    {
        // -1: the root of the source is the destination node, which
        // is already in the tree
        const uint64_t num_new = estimate_tree_capacity_sampled_(src) - 1u;
        const uint64_t size = static_cast<uint64_t>(tree->size());
        const uint64_t cap = static_cast<uint64_t>(tree->capacity());
        // grow only if the tree has no room for the estimate. So a
        // tree that the caller reserved for the source keeps its size.
        if(size + num_new > cap)
        {
            // Add 25%: too many nodes costs only address space (the
            // tree does not touch nodes it does not use), while too
            // few makes the tree grow while parsing.
            uint64_t next = size + num_new + (num_new / 4u);
            // At least double, as the tree does when it grows by
            // itself. Else many parses into one tree would each
            // reallocate and copy the whole node array.
            next = next > 2u * cap ? next : 2u * cap;
            const uint64_t max_ids = static_cast<uint64_t>(std::numeric_limits<id_type>::max()) - 2u; // NONE, VALID
            const uint64_t max_nodes = static_cast<uint64_t>(SIZE_MAX / sizeof(NodeData));
            next = next < max_ids ? next : max_ids;
            next = next < max_nodes ? next : max_nodes;
            tree->reserve(static_cast<id_type>(next));
        }
    }
}
// assumes checks above were done prior to calling
void parse_yaml_(Parser *parser, csubstr filename, substr yaml, Tree *tree, id_type node_id)
{
    checksrc_(tree, yaml);
    reserve_from_source_(parser, tree, yaml);
    reset_handler_(parser, tree, node_id);
    parser->parse_in_place_ev(filename, yaml);
}
// assumes checks above were done prior to calling
void parse_json_(Parser *parser, csubstr filename, substr json, Tree *tree, id_type node_id)
{
    checksrc_(tree, json);
    reserve_from_source_(parser, tree, json);
    reset_handler_(parser, tree, node_id);
    parser->parse_json_in_place_ev(filename, json);
}
} // namespace



// this is vertically aligned to highlight the parameter differences.
void parse_in_place(Parser *parser, csubstr filename, substr yaml, Tree *tree, id_type node_id) { check_(parser, tree); parse_yaml_(parser, filename, yaml, tree, node_id); }
void parse_in_place(Parser *parser,                   substr yaml, Tree *tree, id_type node_id) { check_(parser, tree); parse_yaml_(parser, {}      , yaml, tree, node_id); }
void parse_in_place(Parser *parser, csubstr filename, substr yaml, Tree *tree                 ) { check_(parser, tree); parse_yaml_(parser, filename, yaml, tree, tree->root_id()); }
void parse_in_place(Parser *parser,                   substr yaml, Tree *tree                 ) { check_(parser, tree); parse_yaml_(parser, {}      , yaml, tree, tree->root_id()); }
void parse_in_place(Parser *parser, csubstr filename, substr yaml, NodeRef node               ) { check_(parser, node); parse_yaml_(parser, filename, yaml, node.tree(), node.id()); }
void parse_in_place(Parser *parser,                   substr yaml, NodeRef node               ) { check_(parser, node); parse_yaml_(parser, {}      , yaml, node.tree(), node.id()); }
Tree parse_in_place(Parser *parser, csubstr filename, substr yaml                             ) { check_(parser); Tree tree(parser->callbacks()); parse_yaml_(parser, filename, yaml, &tree, tree.root_id()); return tree; }
Tree parse_in_place(Parser *parser,                   substr yaml                             ) { check_(parser); Tree tree(parser->callbacks()); parse_yaml_(parser, {}      , yaml, &tree, tree.root_id()); return tree; }

// this is vertically aligned to highlight the parameter differences.
void parse_in_place(csubstr filename, substr yaml, Tree *tree, id_type node_id, ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, filename, yaml, tree, node_id); }
void parse_in_place(                  substr yaml, Tree *tree, id_type node_id, ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, {}      , yaml, tree, node_id); }
void parse_in_place(csubstr filename, substr yaml, Tree *tree                 , ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, filename, yaml, tree, tree->root_id()); }
void parse_in_place(                  substr yaml, Tree *tree                 , ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, {}      , yaml, tree, tree->root_id()); }
void parse_in_place(csubstr filename, substr yaml, NodeRef node               , ParserOptions const& opts) { check_(node); TmpParser tmp(node, opts); parse_yaml_(&tmp.parser, filename, yaml, node.tree(), node.id()); }
void parse_in_place(                  substr yaml, NodeRef node               , ParserOptions const& opts) { check_(node); TmpParser tmp(node, opts); parse_yaml_(&tmp.parser, {}      , yaml, node.tree(), node.id()); }
Tree parse_in_place(csubstr filename, substr yaml                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree;          parse_yaml_(&tmp.parser, filename, yaml, &tree, tree.root_id()); return tree; }
Tree parse_in_place(                  substr yaml                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree;          parse_yaml_(&tmp.parser, {}      , yaml, &tree, tree.root_id()); return tree; }



// this is vertically aligned to highlight the parameter differences.
void parse_json_in_place(Parser *parser, csubstr filename, substr json, Tree *tree, id_type node_id) { check_(parser, tree); parse_json_(parser, filename, json, tree, node_id); }
void parse_json_in_place(Parser *parser,                   substr json, Tree *tree, id_type node_id) { check_(parser, tree); parse_json_(parser, {}      , json, tree, node_id); }
void parse_json_in_place(Parser *parser, csubstr filename, substr json, Tree *tree                 ) { check_(parser, tree); parse_json_(parser, filename, json, tree, tree->root_id()); }
void parse_json_in_place(Parser *parser,                   substr json, Tree *tree                 ) { check_(parser, tree); parse_json_(parser, {}      , json, tree, tree->root_id()); }
void parse_json_in_place(Parser *parser, csubstr filename, substr json, NodeRef node               ) { check_(parser, node); parse_json_(parser, filename, json, node.tree(), node.id()); }
void parse_json_in_place(Parser *parser,                   substr json, NodeRef node               ) { check_(parser, node); parse_json_(parser, {}      , json, node.tree(), node.id()); }
Tree parse_json_in_place(Parser *parser, csubstr filename, substr json                             ) { check_(parser); Tree tree(parser->callbacks()); parse_json_(parser, filename, json, &tree, tree.root_id()); return tree; }
Tree parse_json_in_place(Parser *parser,                   substr json                             ) { check_(parser); Tree tree(parser->callbacks()); parse_json_(parser, {}      , json, &tree, tree.root_id()); return tree; }

// this is vertically aligned to highlight the parameter differences.
void parse_json_in_place(csubstr filename, substr json, Tree *tree, id_type node_id, ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, filename, json, tree, node_id); }
void parse_json_in_place(                  substr json, Tree *tree, id_type node_id, ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, {}      , json, tree, node_id); }
void parse_json_in_place(csubstr filename, substr json, Tree *tree                 , ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, filename, json, tree, tree->root_id()); }
void parse_json_in_place(                  substr json, Tree *tree                 , ParserOptions const& opts) { check_(tree); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, {}      , json, tree, tree->root_id()); }
void parse_json_in_place(csubstr filename, substr json, NodeRef node               , ParserOptions const& opts) { check_(node); TmpParser tmp(node, opts); parse_json_(&tmp.parser, filename, json, node.tree(), node.id()); }
void parse_json_in_place(                  substr json, NodeRef node               , ParserOptions const& opts) { check_(node); TmpParser tmp(node, opts); parse_json_(&tmp.parser, {}      , json, node.tree(), node.id()); }
Tree parse_json_in_place(csubstr filename, substr json                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree;          parse_json_(&tmp.parser, filename, json, &tree, tree.root_id()); return tree; }
Tree parse_json_in_place(                  substr json                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree;          parse_json_(&tmp.parser, {}      , json, &tree, tree.root_id()); return tree; }



// this is vertically aligned to highlight the parameter differences.
void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, Tree *tree, id_type node_id) { substr src = checkcp_(parser, tree, yaml); parse_yaml_(parser, filename, src, tree, node_id); }
void parse_in_arena(Parser *parser,                   csubstr yaml, Tree *tree, id_type node_id) { substr src = checkcp_(parser, tree, yaml); parse_yaml_(parser, {}      , src, tree, node_id); }
void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, Tree *tree                 ) { substr src = checkcp_(parser, tree, yaml); parse_yaml_(parser, filename, src, tree, tree->root_id()); }
void parse_in_arena(Parser *parser,                   csubstr yaml, Tree *tree                 ) { substr src = checkcp_(parser, tree, yaml); parse_yaml_(parser, {}      , src, tree, tree->root_id()); }
void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, NodeRef node               ) { substr src = checkcp_(parser, node, yaml); parse_yaml_(parser, filename, src, node.tree(), node.id()); }
void parse_in_arena(Parser *parser,                   csubstr yaml, NodeRef node               ) { substr src = checkcp_(parser, node, yaml); parse_yaml_(parser, {}      , src, node.tree(), node.id()); }
Tree parse_in_arena(Parser *parser, csubstr filename, csubstr yaml                             ) { check_(parser); Tree tree(parser->callbacks()); substr src = cpsrc_(&tree, yaml); parse_yaml_(parser, filename, src, &tree, tree.root_id()); return tree; }
Tree parse_in_arena(Parser *parser,                   csubstr yaml                             ) { check_(parser); Tree tree(parser->callbacks()); substr src = cpsrc_(&tree, yaml); parse_yaml_(parser, {}      , src, &tree, tree.root_id()); return tree; }

// this is vertically aligned to highlight the parameter differences.
void parse_in_arena(csubstr filename, csubstr yaml, Tree *tree, id_type node_id, ParserOptions const& opts) { substr src = checkcp_(tree, yaml); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, filename, src, tree, node_id); }
void parse_in_arena(                  csubstr yaml, Tree *tree, id_type node_id, ParserOptions const& opts) { substr src = checkcp_(tree, yaml); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, {}      , src, tree, node_id); }
void parse_in_arena(csubstr filename, csubstr yaml, Tree *tree                 , ParserOptions const& opts) { substr src = checkcp_(tree, yaml); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, filename, src, tree, tree->root_id()); }
void parse_in_arena(                  csubstr yaml, Tree *tree                 , ParserOptions const& opts) { substr src = checkcp_(tree, yaml); TmpParser tmp(tree, opts); parse_yaml_(&tmp.parser, {}      , src, tree, tree->root_id()); }
void parse_in_arena(csubstr filename, csubstr yaml, NodeRef node               , ParserOptions const& opts) { substr src = checkcp_(node, yaml); TmpParser tmp(node, opts); parse_yaml_(&tmp.parser, filename, src, node.tree(), node.id()); }
void parse_in_arena(                  csubstr yaml, NodeRef node               , ParserOptions const& opts) { substr src = checkcp_(node, yaml); TmpParser tmp(node, opts); parse_yaml_(&tmp.parser, {}      , src, node.tree(), node.id()); }
Tree parse_in_arena(csubstr filename, csubstr yaml                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree; substr src = cpsrc_(&tree, yaml); parse_yaml_(&tmp.parser, filename, src, &tree, tree.root_id()); return tree; }
Tree parse_in_arena(                  csubstr yaml                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree; substr src = cpsrc_(&tree, yaml); parse_yaml_(&tmp.parser, {}      , src, &tree, tree.root_id()); return tree; }



// this is vertically aligned to highlight the parameter differences.
void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, Tree *tree, id_type node_id) { substr src = checkcp_(parser, tree, json); parse_json_(parser, filename, src, tree, node_id); }
void parse_json_in_arena(Parser *parser,                   csubstr json, Tree *tree, id_type node_id) { substr src = checkcp_(parser, tree, json); parse_json_(parser, {}      , src, tree, node_id); }
void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, Tree *tree                 ) { substr src = checkcp_(parser, tree, json); parse_json_(parser, filename, src, tree, tree->root_id()); }
void parse_json_in_arena(Parser *parser,                   csubstr json, Tree *tree                 ) { substr src = checkcp_(parser, tree, json); parse_json_(parser, {}      , src, tree, tree->root_id()); }
void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, NodeRef node               ) { substr src = checkcp_(parser, node, json); parse_json_(parser, filename, src, node.tree(), node.id()); }
void parse_json_in_arena(Parser *parser,                   csubstr json, NodeRef node               ) { substr src = checkcp_(parser, node, json); parse_json_(parser, {}      , src, node.tree(), node.id()); }
Tree parse_json_in_arena(Parser *parser, csubstr filename, csubstr json                             ) { check_(parser); Tree tree(parser->callbacks()); substr src = cpsrc_(&tree, json); parse_json_(parser, filename, src, &tree, tree.root_id()); return tree; }
Tree parse_json_in_arena(Parser *parser,                   csubstr json                             ) { check_(parser); Tree tree(parser->callbacks()); substr src = cpsrc_(&tree, json); parse_json_(parser, {}      , src, &tree, tree.root_id()); return tree; }

// this is vertically aligned to highlight the parameter differences.
void parse_json_in_arena(csubstr filename, csubstr json, Tree *tree, id_type node_id, ParserOptions const& opts) { substr src = checkcp_(tree, json); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, filename, src, tree, node_id); }
void parse_json_in_arena(                  csubstr json, Tree *tree, id_type node_id, ParserOptions const& opts) { substr src = checkcp_(tree, json); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, {}      , src, tree, node_id); }
void parse_json_in_arena(csubstr filename, csubstr json, Tree *tree                 , ParserOptions const& opts) { substr src = checkcp_(tree, json); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, filename, src, tree, tree->root_id()); }
void parse_json_in_arena(                  csubstr json, Tree *tree                 , ParserOptions const& opts) { substr src = checkcp_(tree, json); TmpParser tmp(tree, opts); parse_json_(&tmp.parser, {}      , src, tree, tree->root_id()); }
void parse_json_in_arena(csubstr filename, csubstr json, NodeRef node               , ParserOptions const& opts) { substr src = checkcp_(node, json); TmpParser tmp(node, opts); parse_json_(&tmp.parser, filename, src, node.tree(), node.id()); }
void parse_json_in_arena(                  csubstr json, NodeRef node               , ParserOptions const& opts) { substr src = checkcp_(node, json); TmpParser tmp(node, opts); parse_json_(&tmp.parser, {}      , src, node.tree(), node.id()); }
Tree parse_json_in_arena(csubstr filename, csubstr json                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree; substr src = cpsrc_(&tree, json); parse_json_(&tmp.parser, filename, src, &tree, tree.root_id()); return tree; }
Tree parse_json_in_arena(                  csubstr json                             , ParserOptions const& opts) { TmpParser tmp(opts); Tree tree; substr src = cpsrc_(&tree, json); parse_json_(&tmp.parser, {}      , src, &tree, tree.root_id()); return tree; }



//-----------------------------------------------------------------------------

namespace {
// estimate_tree_capacity() reads the source one machine word at a
// time, and tests all the bytes of a word at once. These helpers do
// that. A "mark" is a word that has 0x80 in each byte that passed a
// test, and 0 in each other byte. Nothing here depends on the byte
// order, because each byte of a word is tested apart from the others,
// and the marks are added up without regard to their position.

#if C4_WORDSIZE == 8
using est_word_type = uint64_t;
#else
using est_word_type = uint32_t;
#endif
static_assert(sizeof(est_word_type) == C4_WORDSIZE, "the loop steps by C4_WORDSIZE");

/** a word with the byte b in each of its bytes */
C4_ALWAYS_INLINE est_word_type est_bcast_(uint8_t b) noexcept
{
    return (~static_cast<est_word_type>(0) / static_cast<est_word_type>(0xff)) * b; // 0x0101...01 * b
}

/** mark the bytes of w that are equal to c. Any w and any c.
 *
 * How: x = w ^ bcast(c) has a zero byte where w has c. In each byte,
 * (x & 0x7f) + 0x7f sets the high bit if one of the low 7 bits is
 * set, and it cannot carry into the next byte. OR-ing x also sets the
 * high bit when the high bit of x is set. So the high bit is clear
 * only for a zero byte of x. OR-ing 0x7f and inverting keeps only
 * that high bit. */
C4_ALWAYS_INLINE est_word_type mark_bytes_eq_(est_word_type w, char c) noexcept
{
    const est_word_type lo7 = est_bcast_(0x7f);
    const est_word_type x = w ^ est_bcast_(static_cast<uint8_t>(c));
    return ~(((x & lo7) + lo7) | x | lo7);
}

/** add up the bytes of w, as numbers from 0 to 255. The sum is at
 * most 8*255, so it fits in the 16-bit lane that the multiply fills */
C4_ALWAYS_INLINE id_type sum_bytes_(est_word_type w) noexcept
{
    const est_word_type lo8 = (~static_cast<est_word_type>(0) / static_cast<est_word_type>(0xffff)) * 0xffu; // 0x00ff00ff...
    const est_word_type pairs = (w & lo8) + ((w >> 8u) & lo8); // 16-bit lanes, each <= 510
    // the multiply adds all the 16-bit lanes into the top lane
    const est_word_type lanes = ~static_cast<est_word_type>(0) / static_cast<est_word_type>(0xffff); // 0x00010001...
    return static_cast<id_type>((pairs * lanes) >> ((8u * sizeof(est_word_type)) - 16u));
}
} // namespace

RYML_EXPORT id_type estimate_tree_capacity(csubstr src)
{
    id_type num_nodes = 1; // root
    size_t i = 0;
    // Each step loads two words: the word at i, and the word at i+1,
    // to see the byte that follows each dash. Each byte adds at most 1
    // to its byte of acc, so acc is added up every 255 words, before a
    // byte can overflow.
    while(i + C4_WORDSIZE < src.len)
    {
        const size_t words = (src.len - i - 1) / C4_WORDSIZE;
        const size_t e = i + (C4_WORDSIZE * (words < 255u ? words : 255u));
        est_word_type acc = 0;
        for( ; i < e; i += C4_WORDSIZE)
        {
            est_word_type w;
            est_word_type next;
            memcpy(&w, src.str + i, C4_WORDSIZE);
            memcpy(&next, src.str + i + 1, C4_WORDSIZE);
            // '{' or '[': '[' | 0x20 == '{'
            const est_word_type node = mark_bytes_eq_(w, '\n')
                | mark_bytes_eq_(w, ',')
                | mark_bytes_eq_(w | est_bcast_(0x20), '{');
            // "- " or a dash at the end of a line
            const est_word_type entry = mark_bytes_eq_(w, '-')
                & (mark_bytes_eq_(next, ' ') | mark_bytes_eq_(next, '\n'));
            acc += (node | entry) >> 7u;
        }
        num_nodes += sum_bytes_(acc);
    }
    for( ; i < src.len; ++i)
    {
        const char c = src.str[i];
        num_nodes += (c == '\n') || (c == ',') || (c == '[') || (c == '{');
        num_nodes += (c == '-') && (i + 1 < src.len) && (src.str[i + 1] == ' ' || src.str[i + 1] == '\n');
    }
    return num_nodes;
}

} // namespace yml
} // namespace c4
