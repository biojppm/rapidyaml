#ifndef C4_YML_EMITTER_DEF_HPP_
#define C4_YML_EMITTER_DEF_HPP_

/** @file emitter.def.hpp */

#ifndef C4_YML_EMITTER_HPP_
#include "c4/yml/emitter.hpp"
#endif
#ifndef C4_YML_TREE_HPP_
#include "c4/yml/tree.hpp"
#endif
#ifndef C4_YML_SCALAR_STYLE_HPP_
#include "c4/yml/scalar_style.hpp"
#endif
#ifndef C4_YML_DETAIL_DBGPRINT_HPP_
#include "c4/yml/detail/dbgprint.hpp"
#endif
#ifndef C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif
#ifndef C4_YML_DETAIL_EMIT_SCALAR_HPP_
#include "c4/yml/detail/emit_scalar.hpp"
#endif



C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(modernize-avoid-c-style-cast)

namespace c4 {
namespace yml {

namespace detail {
enum : type_bits { // NOLINT
    styles_block_key_ = KEY_LITERAL|KEY_FOLDED,
    styles_block_val_ = VAL_LITERAL|VAL_FOLDED,
    styles_block_     = (styles_block_key_ | styles_block_val_),
    styles_flow_key_  = KEY_STYLE & ~styles_block_key_,
    styles_flow_val_  = VAL_STYLE & ~styles_block_val_,
    styles_flow_      = styles_flow_key_ | styles_flow_val_,
    styles_squo_      = KEY_SQUO|VAL_SQUO,
    styles_dquo_      = KEY_DQUO|VAL_DQUO,
    styles_plain_     = KEY_PLAIN|VAL_PLAIN,
    styles_literal_   = KEY_LITERAL|VAL_LITERAL,
    styles_folded_    = KEY_FOLDED|VAL_FOLDED,
};
} // namespace detail


template<class Writer>
void Emitter<Writer>::emit_as(EmitType_e type, Tree const* tree, id_type id)
{
    RYML_ASSERT_BASIC_CB_(tree->callbacks(), !tree || !tree->empty() || id == NONE);
    if(!tree || tree->empty())
        return;
    if(id == NONE)
        id = tree->root_id();
    RYML_CHECK_VISIT_CB_(tree->callbacks(), id < tree->capacity(), tree, id);
    m_tree = tree;
    m_col = 0;
    m_depth = 0;
    m_ilevel = 0;
    m_pws = PWS_NONE_;
    m_flow_pws = {};
    if(type == EMIT_YAML)
        emit_yaml_(id);
    else if(type == EMIT_JSON)
        json_emit_(id);
    else
        RYML_ERR_BASIC_CB_(m_tree->callbacks(), "unknown emit type"); // LCOV_EXCL_LINE
    m_tree = nullptr;
}

/** @cond dev */


//-----------------------------------------------------------------------------

// The startup logic is made complicated from it having to accept
// initial non-root nodes, and having to deal with tricky tokens like
// doc separators, anchors, tags, optional keys or dashes, and
// comments.
//
// This function kickstarts the tree descent by handling all the
// initial and final logic at the top-level scope, thus avoiding
// top-level kickstart branches in the recursive descending code
// (which should be oblivious to such logic). This makes the recursive
// descending code a lot simpler.
template<class Writer>
void Emitter<Writer>::emit_yaml_(id_type id)
{
    const NodeType ty = m_tree->type(id);

    // emit leading tokens, such as keys or comments
    const bool has_parent = !m_tree->is_root(id);
    const bool emit_key = has_parent && ty.has_key() && m_opts.emit_nonroot_key();
    const bool emit_dash = has_parent && !ty.has_key() && !ty.is_doc() && m_opts.emit_nonroot_dash();
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !(emit_key && emit_dash), m_tree, id);

    // emit opening tokens (such as tags, anchors or comments)
    if(emit_key)
    {
        blck_map_open_entry_(id);
        ++m_ilevel;
    }
    else if(emit_dash)
    {
        blck_seq_open_entry_(id);
        ++m_ilevel;
    }
    else
    {
        top_open_entry_(id);
    }

    // emit the payload
    if(ty.is_stream())
    {
        RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_ilevel == 0, m_tree, id);
        visit_stream_(id);
    }
    else if(ty.is_doc())
    {
        RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_ilevel == 0, m_tree, id);
        visit_doc_(id);
    }
    else if(ty.is_container())
    {
        visit_blck_container_(id);
    }
    else if(ty.has_val())
    {
        visit_doc_val_(id);
    }

    // emit closing tokens (such as comments)
    if(emit_key)
    {
        --m_ilevel;
        blck_close_entry_(id);
    }
    else if(emit_dash)
    {
        --m_ilevel;
        blck_close_entry_(id);
    }
    else
    {
        top_close_entry_(id);
    }

    if(ty.is_flow_mlx())
    {
        newl_();
    }
    else if(m_tree->is_root(id)
       || emit_dash || emit_key
       || !ty.is_val())
    {
        write_pws_and_pend_(PWS_NONE_);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_stream_(id_type id)
{
    auto write_tag_directives = [this](const id_type next_node){
        const id_type doc = m_tree->ancestor_doc(next_node);
        const TagDirectiveRange tagds = m_tree->m_tag_directives.lookup_range(doc);
        if(tagds.e != tagds.b)
        {
            const id_type parent = m_tree->parent(next_node);
            if(next_node != m_tree->first_child(parent))
            {
                write_pws_and_pend_(PWS_NEWL_);
                write_("...");
            }
        }
        for(TagDirective const& td : tagds)
        {
            write_pws_and_pend_(PWS_NONE_);
            write_("%TAG ");
            write_(td.handle);
            write_(' ');
            write_(td.prefix);
            pend_newl_();
        }
    };
    const id_type first_child = m_tree->first_child(id);
    if(first_child != NONE)
        write_tag_directives(first_child);
    ++m_depth;
    for(id_type child = first_child; child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        m_ilevel = 0;
        write_pws_and_pend_(PWS_NONE_);
        top_open_entry_(child);
        visit_doc_(child);
        top_close_entry_(child);
        if(ty.is_val())
        {
            if(ty.m_bits & VALNIL)
                pend_newl_();
        }
        else if(ty.is_container())
        {
            if(ty.is_flow())
                pend_newl_();
        }
        if(m_tree->next_sibling(child) != NONE)
        {
            write_tag_directives(m_tree->next_sibling(child));
        }
    }
    --m_depth;
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_blck_container_(id_type id)
{
    NodeType ty = m_tree->type(id);
    if(!(ty.m_bits & CONTAINER_STYLE))
        ty.m_bits |= (m_tree->empty(id) ? FLOW_SL : BLOCK);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.is_flow_sl())
        visit_flow_sl_(id);
    else if(ty.is_flow_mlx())
        visit_flow_ml_(id);
    else
        visit_blck_(id);
}

template<class Writer>
void Emitter<Writer>::visit_flow_container_(id_type id)
{
    NodeType ty = m_tree->type(id);
    if(!(ty.m_bits & CONTAINER_STYLE))
        ty.m_bits |= FLOW_SL;
    write_pws_and_pend_(PWS_NONE_);
    if(ty.is_flow_mlx())
        visit_flow_ml_(id);
    else // if(ty.is_flow_sl())
        visit_flow_sl_(id);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_doc_val_(id_type id)
{
    // some plain scalars such as '...' and '---' must not
    // appear at 0-indentation
    NodeType ty = m_tree->type(id);
    const csubstr val = m_tree->val(id);
    type_bits val_style = ty.m_bits & VAL_STYLE;
    const bool is_ambiguous = ((ty.m_bits & VAL_PLAIN) || !val_style)
        && (val.begins_with("...") || val.begins_with("---"));
    if(is_ambiguous)
    {
        ++m_ilevel;
        if(m_pws != PWS_NONE_)
            pend_newl_();
        else
            indent_(m_ilevel);
    }
    write_pws_and_pend_(PWS_NONE_);
    if(m_tree->is_val_ref(id))
    {
        write_ref_(val);
    }
    else
    {
        if(!val_style)
            val_style = scalar_style_choose_block(val);
        blck_write_scalar_(val, val_style);
    }
    if(is_ambiguous)
    {
        --m_ilevel;
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_doc_(id_type id)
{
    const NodeType ty = m_tree->type(id);
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ty.is_doc(), m_tree, id);
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !ty.has_key(), m_tree, id);
    if(ty.is_container()) // this is more frequent
    {
        visit_blck_container_(id);
    }
    else if(ty.is_val())
    {
        visit_doc_val_(id);
    }
}


//-----------------------------------------------------------------------------

// to be called only at top level
template<class Writer>
void Emitter<Writer>::top_open_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    if(ty.is_doc() && !m_tree->is_root(node))
    {
        write_("---");
        pend_space_();
    }
    if(ty.has_val_anchor())
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.has_val_tag())
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(m_pws == PWS_SPACE_)
    {
        if(ty.has_val())
        {
            if(ty.is_val_plain() && !m_tree->val(node).len)
                pend_none_();
        }
        else
        {
            RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_container(), m_tree, node);
            if(!ty.is_flow())
                pend_newl_();
        }
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::top_close_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    if(ty.is_val() && !(ty.m_bits & VALNIL))
    {
        pend_newl_();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::flow_seq_open_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::flow_map_open_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_NONE_);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.has_key(), m_tree, node);
    if(ty.m_bits & KEYANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_("&");
        write_(m_tree->key_anchor(node));
    }
    if(ty.m_bits & KEYTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->key_tag(node));
    }
    if(ty.m_bits & KEYREF)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_ref_(m_tree->key(node));
    }
    else
    {
        write_pws_and_pend_(PWS_NONE_);
        csubstr key = m_tree->key(node);
        if(!(ty.m_bits & detail::styles_flow_key_))
            ty.m_bits |= scalar_style_choose_flow(key) & detail::styles_flow_key_;
        flow_write_scalar_(key, ty.m_bits & detail::styles_flow_key_);
    }
    write_pws_and_pend_(PWS_SPACE_);
    write_(':');
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
C4_NODISCARD bool Emitter<Writer>::maybe_start_flow_pws_ml_(id_type node) noexcept
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->type(node) & (FLOW_ML1|FLOW_MLN), m_tree, node);
    if(m_flow_pws.active)
        return false;
    NodeType ty = m_tree->type(node);
    if(m_opts.force_flow_spc())
        ty |= FLOW_SPC;
    m_flow_pws.start(ty, m_opts.max_cols());
    return true;
}

template<class Writer>
C4_NODISCARD typename Emitter<Writer>::flow_pws Emitter<Writer>::setup_flow_pws_sl_(id_type node) noexcept
{
    flow_pws ret = {};
    if(m_flow_pws.active)
    {
        ret = m_flow_pws;
    }
    else
    {
        NodeType ty = m_tree->type(node);
        if(m_opts.force_flow_spc())
            ty |= FLOW_SPC;
        ret.start(ty, 0);
    }
    return ret;
}

template<class Writer>
void Emitter<Writer>::flow_pws::start(NodeType ty, size_t max_cols_) noexcept
{
    max_cols = 0;
    pend_after_comma = (ty.m_bits & FLOW_SPC) ? PWS_SPACE_ : PWS_NONE_;
    if(ty.m_bits & FLOW_MLN)
    {
        max_cols_ = max_cols_ >= 2 ? max_cols_ : 2;
        // subtract 1 for the comma, and maybe the space from pend_after_comma
        max_cols = max_cols_ - 1 - pend_after_comma;
        // line above only works if:
        static_assert((size_t)PWS_NONE_ == 0 && (size_t)PWS_SPACE_ == 1, "invalid assumptions");
        active = true;
    }
    else if(ty.m_bits & FLOW_ML1)
    {
        pend_after_comma = PWS_NEWL_;
    }
}

template<class Writer>
void Emitter<Writer>::flow_close_entry_sl_(id_type node, id_type last_sibling, Pws_e pend_after)
{
    if(node != last_sibling)
    {
        write_pws_and_pend_(pend_after);
        write_(',');
    }
}

template<class Writer>
void Emitter<Writer>::flow_close_entry_ml_(id_type node, id_type last_sibling, Pws_e pend_after)
{
    if(node != last_sibling)
    {
        write_pws_and_pend_(pend_after);
        write_(',');
    }
    else
    {
        pend_newl_();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::blck_seq_open_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_NONE_);
    write_pws_and_pend_(PWS_SPACE_); // pend the space after the following dash
    write_('-');
    bool has_tag_or_anchor = false;
    if(ty.m_bits & VALANCH)
    {
        has_tag_or_anchor = true;
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        has_tag_or_anchor = true;
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(has_tag_or_anchor && ty.is_container())
    {
        if(!(ty.m_bits & CONTAINER_STYLE))
            ty |= BLOCK;
        if((ty.m_bits & BLOCK) && m_tree->has_children(node))
            pend_newl_();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::blck_map_open_entry_(id_type node)
{
    NodeType ty = m_tree->type(node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.has_key(), m_tree, node);
    csubstr key = m_tree->key(node);
    if(!(ty.m_bits & (KEY_STYLE|KEYREF)))
        ty.m_bits |= (scalar_style_choose_block(key).m_bits & KEY_STYLE);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.m_bits & KEYANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->key_anchor(node));
    }
    if(ty.m_bits & KEYTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->key_tag(node));
    }
    if(ty.m_bits & KEYREF)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_ref_(key);
    }
    else
    {
        write_pws_and_pend_(PWS_NONE_);
        const type_bits use_qmrk = ty.m_bits & detail::styles_block_key_;
        if(!use_qmrk)
        {
            blck_write_scalar_(key, ty.m_bits & KEY_STYLE);
        }
        else
        {
            write_("? ");
            blck_write_scalar_(key, ty.m_bits & KEY_STYLE);
            pend_newl_();
        }
    }
    write_pws_and_pend_(PWS_SPACE_); // pend the space after the colon
    write_(':');
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(ty.is_container() && m_tree->has_children(node))
    {
        if(!(ty.m_bits & CONTAINER_STYLE))
            ty |= BLOCK;
        if(ty.is_block())
            pend_newl_();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::blck_close_entry_(id_type node)
{
    (void)node;
    pend_newl_();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_blck_seq_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    bool empty = true;
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        blck_seq_open_entry_(child);
        if(ty.is_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & VAL_STYLE))
                    ty.m_bits |= (scalar_style_choose_block(val).m_bits & VAL_STYLE);
                blck_write_scalar_(val, ty.m_bits & VAL_STYLE);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            visit_blck_container_(child);
            --m_depth;
            --m_ilevel;
        }
        blck_close_entry_(child);
    }
    if(empty)
    {
        write_pws_and_pend_(PWS_NONE_);
        write_("[]");
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_blck_map_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    bool empty = true;
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_keyval() || ty.is_container() || ty == NOTYPE, m_tree, node);
        blck_map_open_entry_(child); // also writes the key
        if(ty.is_keyval())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & VAL_STYLE))
                    ty |= (scalar_style_choose_block(val).m_bits & VAL_STYLE);
                blck_write_scalar_(val, ty.m_bits & VAL_STYLE);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            visit_blck_container_(child);
            --m_depth;
            --m_ilevel;
        }
        blck_close_entry_(child);
    }
    if(empty)
    {
        write_pws_and_pend_(PWS_NONE_);
        write_("{}");
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_sl_seq_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    const flow_pws pws = setup_flow_pws_sl_(node);
    write_('[');
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), (ty & (VAL|SEQ|MAP)) || ty == NOTYPE, m_tree, node);
        flow_seq_open_entry_(child);
        if(ty.m_bits & VAL)
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & detail::styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & detail::styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & detail::styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.m_bits & (SEQ|MAP))
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_sl_(child, last, pws.next_pws(m_col));
    }
    write_(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_ml_seq_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    write_('[');
    pend_newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    const bool stop_at_end = maybe_start_flow_pws_ml_(node);
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        flow_seq_open_entry_(child);
        if(ty.is_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & detail::styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & detail::styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & detail::styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_ml_(child, last, m_flow_pws.next_pws(m_col));
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_sl_map_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    flow_pws pws = setup_flow_pws_sl_(node);
    write_('{');
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        flow_map_open_entry_(child);
        if(ty.has_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & detail::styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & detail::styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & detail::styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_sl_(child, last, pws.next_pws(m_col));
    }
    write_pws_and_pend_(PWS_NONE_);
    write_('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_ml_map_(id_type node)
{
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    write_('{');
    pend_newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    const bool stop_at_end = maybe_start_flow_pws_ml_(node);
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        flow_map_open_entry_(child);
        if(ty.has_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & detail::styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & detail::styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & detail::styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_ml_(child, last, m_flow_pws.next_pws(m_col));
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_blck_(id_type node)
{
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), !ty.is_stream(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    if(ty.is_seq())
    {
        visit_blck_seq_(node);
    }
    else
    {
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        visit_blck_map_(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_sl_(id_type node)
{
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), !ty.is_stream(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    if(ty.m_bits & SEQ)
    {
        visit_flow_sl_seq_(node);
    }
    else
    {
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        visit_flow_sl_map_(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::visit_flow_ml_(id_type node)
{
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), !ty.is_stream(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    if(ty.m_bits & SEQ)
    {
        visit_flow_ml_seq_(node);
    }
    else
    {
        RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        visit_flow_ml_map_(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::flow_write_scalar_(csubstr str, type_bits ty)
{
    RYML_ASSERT_BASIC_CB_(m_tree->callbacks(), !(ty & detail::styles_block_));
    if((ty & detail::styles_plain_) || !(ty & SCALAR_STYLE))
    {
        detail::emit_scalar_plain_(this, str, m_ilevel);
    }
    else if(ty & detail::styles_squo_)
    {
        detail::emit_scalar_squo_(this, str, m_ilevel);
    }
    else // if(ty & detail::styles_dquo_)
    {
        detail::emit_scalar_dquo_(this, str);
    }
}

template<class Writer>
void Emitter<Writer>::blck_write_scalar_(csubstr str, type_bits ty)
{
    if((ty & detail::styles_plain_) || !(ty & SCALAR_STYLE))
    {
        detail::emit_scalar_plain_(this, str, m_ilevel);
    }
    else if(ty & detail::styles_squo_)
    {
        detail::emit_scalar_squo_(this, str, m_ilevel);
    }
    else if(ty & detail::styles_dquo_)
    {
        detail::emit_scalar_dquo_(this, str);
    }
    else if(ty & detail::styles_literal_)
    {
        detail::emit_scalar_literal_(this, str, m_ilevel);
    }
    else // if(ty & detail::styles_folded_)
    {
        detail::emit_scalar_folded_(this, str, m_ilevel);
    }
}


//-----------------------------------------------------------------------------

namespace detail {
inline type_bits json_type_(type_bits ty)
{
    enum : type_bits { // NOLINT
        ml_bits = (BLOCK|(STREAM & ~SEQ)), // remove SEQ from STREAM to test
        sl_bits = (CONTAINER_STYLE & ~FLOW_SPC),
    };
    if(ty & ml_bits)
    {
        ty &= ~BLOCK;
        ty |= FLOW_ML1;
    }
    else if((ty & (SEQ|MAP)) && !(ty & sl_bits))
    {
        ty |= FLOW_SL;
    }
    return ty;
}
} // namespace detail


template<class Writer>
void Emitter<Writer>::json_emit_(id_type id)
{
    NodeType ty = m_tree->type(id);
    // JSON does not have streams
    if C4_UNLIKELY(ty.is_stream() && m_opts.json_err_on_stream())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "found stream node");
    static_assert(STREAM & SEQ, "STREAM must be a SEQ");
    if(!m_tree->is_root(id) && !m_opts.emit_nonroot_key())
        ty &= ~(KEY|KEY_STYLE);
    ty = detail::json_type_(ty);
    if(ty.is_flow_mlx())
    {
        json_visit_ml_(id, ty, 0);
        newl_();
    }
    else
    {
        json_visit_sl_(id, ty, 0);
        if(ty.has_key())
            newl_();
    }
}

template<class Writer>
void Emitter<Writer>::json_visit_sl_(id_type id, NodeType ty, id_type depth)
{
    if C4_UNLIKELY(depth > m_opts.max_depth())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "max depth exceeded");
    if(ty.is_val())
    {
        if C4_UNLIKELY(ty.is_val_ref() && m_opts.json_err_on_anchor())
            goto referror; // NOLINT
        json_writev_(id, ty);
    }
    else if(ty.is_keyval())
    {
        if C4_UNLIKELY((ty.is_key_ref() || ty.is_val_ref()) && m_opts.json_err_on_anchor())
            goto referror; // NOLINT
        json_writek_(id, ty);
        write_(": ");
        json_writev_(id, ty);
    }
    else if(ty.is_container())
    {
        ty = detail::json_type_(ty);
        if(ty.has_key())
        {
            if C4_UNLIKELY(ty.is_key_ref() && m_opts.json_err_on_anchor())
                goto referror; // NOLINT
            json_writek_(id, ty);
            write_(": ");
        }
        if(ty.is_seq())
            write_('[');
        else if(ty.is_map())
            write_('{');

        for(id_type child = m_tree->first_child(id); child != NONE; child = m_tree->next_sibling(child))
        {
            if(child != m_tree->first_child(id))
            {
                if((ty & FLOW_SPC) || m_opts.force_flow_spc())
                    write_(", ");
                else
                    write_(',');
            }
            json_visit_sl_(child, m_tree->type(child), depth+1);
        }

        if(ty.is_seq())
            write_(']');
        else if(ty.is_map())
            write_('}');
    }  // container
    return;
referror:
    RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
}

template<class Writer>
void Emitter<Writer>::json_visit_ml_(id_type id, NodeType ty, id_type depth)
{
    if C4_UNLIKELY(depth > m_opts.max_depth())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "max depth exceeded");
    if(ty.is_val())
    {
        if C4_UNLIKELY(ty.is_val_ref() && m_opts.json_err_on_anchor())
            goto referror; // NOLINT
        json_writev_(id, ty);
    }
    else if(ty.is_keyval())
    {
        if C4_UNLIKELY((ty.is_key_ref() || ty.is_val_ref()) && m_opts.json_err_on_anchor())
            goto referror; // NOLINT
        json_writek_(id, ty);
        write_(": ");
        json_writev_(id, ty);
    }
    else if(ty.is_container())
    {
        ty = detail::json_type_(ty);
        if(ty.has_key())
        {
            if C4_UNLIKELY(ty.is_key_ref() && m_opts.json_err_on_anchor())
                goto referror; // NOLINT
            json_writek_(id, ty);
            write_(": ");
        }
        if(ty.is_seq())
            write_('[');
        else if(ty.is_map())
            write_('{');

        if(m_tree->has_children(id))
        {
            ++depth;
            if(m_opts.indent_flow_ml()) ++m_ilevel;
            newl_();
            indent_(m_ilevel);
            for(id_type first = m_tree->first_child(id), child = first;
                child != NONE;
                child = m_tree->next_sibling(child))
            {
                if(child != first)
                {
                    write_(',');
                    const size_t maxcols = m_opts.max_cols();
                    if((ty.m_bits & FLOW_MLN) && (m_col+1 < maxcols))
                    {
                        if((ty.m_bits & FLOW_SPC) || m_opts.force_flow_spc())
                            write_(' ');
                    }
                    else if((ty.m_bits & FLOW_ML1) || (m_col+1 >= maxcols))
                    {
                        newl_();
                        indent_(m_ilevel);
                    }
                }
                NodeType chty = m_tree->type(child);
                if(chty.is_flow_sl())
                    json_visit_sl_(child, chty, depth);
                else
                    json_visit_ml_(child, chty, depth);
            }
            if(m_opts.indent_flow_ml()) --m_ilevel;
            --depth;
            newl_();
            indent_(m_ilevel);
        }
        else if(ty.m_bits & FLOW_ML1)
        {
            newl_();
            indent_(m_ilevel);
        }

        if(ty.is_seq())
            write_(']');
        else if(ty.is_map())
            write_('}');
    }
    return;
referror:
    RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
}


template<class Writer>
void Emitter<Writer>::json_writek_(id_type id, NodeType ty)
{
    if C4_UNLIKELY(ty.has_key_tag() && m_opts.json_err_on_tag())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have tags");
    if C4_UNLIKELY(ty.has_key_anchor() && m_opts.json_err_on_anchor())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
    csubstr key = m_tree->key(id);
    if(key.len)
    {
        if(detail::emit_json_maybe_write_naninf_(this, key))
            ;
        else
            detail::emit_json_write_scalar_dquo_(this, key);
    }
    else
    {
        write_("\"\"");
    }
}

template<class Writer>
void Emitter<Writer>::json_writev_(id_type id, NodeType ty)
{
    if C4_UNLIKELY(ty.has_val_tag() && m_opts.json_err_on_tag())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have tags");
    if C4_UNLIKELY(ty.has_val_anchor() && m_opts.json_err_on_anchor())
        RYML_ERR_VISIT_CB_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
    csubstr val = m_tree->val(id);
    if(val.len)
    {
        // use double quoted style if the style is marked quoted
        bool dquoted = ((ty.m_bits & VALQUO)
                        || (scalar_style_choose_json(val).m_bits & SCALAR_DQUO)); // choose the style
        if(dquoted)
            detail::emit_json_write_scalar_dquo_(this, val);
        else if(detail::emit_json_maybe_write_naninf_(this, val))
            ;
        else if(val.is_number())
            detail::emit_json_write_number_(this, val);
        else
            write_(val);
    }
    else
    {
        if(val.str || (ty.m_bits & (VALQUO|VALTAG)))
            write_("\"\"");
        else
            write_("null");
    }
}


/** @endcond */

} // namespace yml
} // namespace c4

// NOLINTEND(modernize-avoid-c-style-cast)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* C4_YML_EMITTTER_DEF_HPP_ */
