#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "c4/yml/emit.hpp"
#endif

/** @file emit.def.hpp Definitions for emit functions. */
#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#include "c4/yml/detail/dbgprint.hpp"
#endif

namespace c4 {
namespace yml {

template<class Writer>
substr Emitter<Writer>::emit_as(EmitType_e type, Tree const& tree, id_type id, bool error_on_excess)
{
    if(tree.empty())
    {
        _RYML_ASSERT_BASIC_(tree.callbacks(), id == NONE);
        return {};
    }
    if(id == NONE)
        id = tree.root_id();
    _RYML_CHECK_VISIT_(tree.callbacks(), id < tree.capacity(), &tree, id);
    m_tree = &tree;
    m_col = 0;
    m_depth = 0;
    m_ilevel = 0;
    m_pws = _PWS_NONE;
    if(type == EMIT_YAML)
    {
        _emit_yaml(id);
    }
    else if(type == EMIT_JSON)
    {
        _emit_json(id);
    }
    else
        _RYML_ERR_BASIC_(m_tree->callbacks(), "unknown emit type"); // LCOV_EXCL_LINE
    m_tree = nullptr;
    return this->Writer::_get(error_on_excess);
}


//-----------------------------------------------------------------------------


// The startup logic is made complicated from it having to accept
// initial non-root nodes, and having to deal with tricky tokens like
// doc separators, anchors, tags, optional keys or dashes, and
// comments.
//
// It kickstarts the tree descent by handling all the initial and
// final logic at the top-level scope, thus avoiding top-level
// kickstart branches in the recursive descending code (which should
// be oblivious of such logic). This makes the recursive descending
// code a lot simpler.
template<class Writer>
void Emitter<Writer>::_emit_yaml(id_type id)
{
    const NodeType ty = m_tree->type(id);

    // emit leading tokens, such as keys or comments
    const bool has_parent = !m_tree->is_root(id);
    const bool emit_key = has_parent && ty.has_key() && m_opts.emit_nonroot_key();
    const bool emit_dash = has_parent && !ty.has_key() && !ty.is_doc() && m_opts.emit_nonroot_dash();
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, !(emit_key && emit_dash), m_tree, id);

    // emit opening tokens (such as tags, anchors or comments)
    if(emit_key)
    {
        _blck_map_open_entry(id);
        ++m_ilevel;
    }
    else if(emit_dash)
    {
        _blck_seq_open_entry(id);
        ++m_ilevel;
    }
    else
    {
        _top_open_entry(id);
    }

    // emit the payload
    if(ty.is_stream())
    {
        _RYML_ASSERT_VISIT_(m_tree->m_callbacks, m_ilevel == 0, m_tree, id);
        _visit_stream(id);
    }
    else if(ty.is_doc())
    {
        _RYML_ASSERT_VISIT_(m_tree->m_callbacks, m_ilevel == 0, m_tree, id);
        _visit_doc(id);
    }
    else if(ty.is_container())
    {
        _visit_blck_container(id);
    }
    else if(ty.has_val())
    {
        _visit_doc_val(id);
    }

    // emit closing tokens (such as comments)
    if(emit_key)
    {
        --m_ilevel;
        _blck_map_close_entry(id);
    }
    else if(emit_dash)
    {
        --m_ilevel;
        _blck_seq_close_entry(id);
    }
    else
    {
        _top_close_entry(id);
    }

    if(ty.is_flow_ml())
        _newl();
    else if(m_tree->is_root(id)
       || emit_dash || emit_key
       || !ty.is_val()
       || ty.is_val_literal() || ty.is_val_folded())
        _write_pws_and_pend(_PWS_NONE);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_stream(id_type id)
{
    TagDirectiveRange tagds = m_tree->tag_directives();
    auto write_tag_directives = [&tagds, this](const id_type next_node){
        TagDirective const* C4_RESTRICT end = tagds.b;
        while(end < tagds.e)
        {
            if(end->next_node_id > next_node)
                break;
            ++end;
        }
        const id_type parent = m_tree->parent(next_node);
        for( ; tagds.b != end; ++tagds.b)
        {
            _write_pws_and_pend(_PWS_NONE);
            if(next_node != m_tree->first_child(parent))
            {
                _write("...");
                _newl();
            }
            _write("%TAG ");
            _write(tagds.b->handle);
            _write(' ');
            _write(tagds.b->prefix);
            _pend_newl();
        }
    };
    const id_type first_child = m_tree->first_child(id);
    if(first_child != NONE)
        write_tag_directives(first_child);
    ++m_depth;
    for(id_type child = first_child; child != NONE; child = m_tree->next_sibling(child))
    {
        m_ilevel = 0;
        _write_pws_and_pend(_PWS_NONE);
        _top_open_entry(child);
        _visit_doc(child);
        _top_close_entry(child);
        if(m_pws == _PWS_NONE)
            _pend_newl();
        if(m_tree->next_sibling(child) != NONE)
            write_tag_directives(m_tree->next_sibling(child));
    }
    --m_depth;
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_container(id_type id)
{
    NodeType ty = m_tree->type(id);
    if(!(ty & CONTAINER_STYLE))
        ty |= (m_tree->empty(id) ? FLOW_SL : BLOCK);
    _write_pws_and_pend(_PWS_NONE);
    if(ty.is_flow_sl())
        _visit_flow_sl(id);
    else if(ty.is_flow_ml())
        _visit_flow_ml(id);
    else
        _visit_blck(id);
}

template<class Writer>
void Emitter<Writer>::_visit_flow_container(id_type id)
{
    NodeType ty = m_tree->type(id);
    if(!(ty & CONTAINER_STYLE))
        ty |= FLOW_SL;
    _write_pws_and_pend(_PWS_NONE);
    if(ty.is_flow_ml())
        _visit_flow_ml(id);
    else // if(ty.is_flow_sl())
        _visit_flow_sl(id);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_doc_val(id_type id)
{
    // some plain scalars such as '...' and '---' must not
    // appear at 0-indentation
    NodeType ty = m_tree->type(id);
    const csubstr val = m_tree->val(id);
    const type_bits has_style_marks = ty & VAL_STYLE;
    const bool is_ambiguous = (ty.is_val_plain() || !has_style_marks)
        && (val.begins_with("...") || val.begins_with("---"));
    if(is_ambiguous)
    {
        ++m_ilevel;
        if(m_pws != _PWS_NONE)
            _pend_newl();
        else
            _indent(m_ilevel);
    }
    _write_pws_and_pend(_PWS_NONE);
    if(m_tree->is_val_ref(id))
    {
        _write_ref(val);
    }
    else
    {
        _blck_write_scalar_val(id);
    }
    if(is_ambiguous)
    {
        --m_ilevel;
    }
    _pend_newl();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_doc(id_type id)
{
    const NodeType ty = m_tree->type(id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, ty.is_doc(), m_tree, id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, !ty.has_key(), m_tree, id);

    if(ty.is_container()) // this is more frequent
    {
        _visit_blck_container(id);
    }
    else if(ty.is_val())
    {
        _visit_doc_val(id);
    }
}


//-----------------------------------------------------------------------------

// to be called only at top level
template<class Writer>
void Emitter<Writer>::_top_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    if(ty.is_doc() && !m_tree->is_root(node))
    {
        _write("---");
        _pend_space();
    }
    if(ty.has_val_tag())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
    }
    if(m_pws == _PWS_SPACE)
    {
        if(ty.has_val())
        {
            if(ty.is_val_plain() && !m_tree->val(node).len)
                _pend_none();
        }
        else
        {
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_container(), m_tree, node);
            if(ty.is_block())
                _pend_newl();
        }
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_top_close_entry(id_type node)
{
    (void)node;
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_sl_write_comma(id_type node, id_type first_sibling)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), first_sibling == m_tree->first_sibling(node), m_tree, node);
    if(node != first_sibling)
        _write(',');
}

template<class Writer>
void Emitter<Writer>::_flow_ml_write_comma(id_type node, id_type last_sibling)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), last_sibling == m_tree->last_sibling(node), m_tree, node);
    if(node != last_sibling)
        _write(',');
    _pend_newl();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    if(ty.has_val_tag())
    {
        _write_tag(m_tree->val_tag(node));
        _pend_space();
    }
    if(ty.has_val_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_seq_close_entry(id_type node)
{
    (void)node;
    _write_pws_and_pend(_PWS_NONE);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_map_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key(), m_tree, node);
    if(ty.has_key_tag())
    {
        _write_tag(m_tree->key_tag(node));
        _pend_space();
    }
    if(ty.has_key_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write("&");
        _write(m_tree->key_anchor(node));
    }
    if(ty.is_key_ref())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_ref(m_tree->key(node));
    }
    else
    {
        _write_pws_and_pend(_PWS_NONE);
        _blck_write_scalar_key(node);
    }
    _write_pws_and_pend(_PWS_SPACE);
    _write(':');
    if(ty.has_val_tag())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_map_close_entry(id_type node)
{
    (void)node;
    _write_pws_and_pend(_PWS_NONE);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    _write_pws_and_pend(_PWS_SPACE); // pend the space after the following dash
    _write('-');
    bool has_tag_or_anchor = false;
    if(ty.has_val_tag())
    {
        has_tag_or_anchor = true;
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        has_tag_or_anchor = true;
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
    }
    if(has_tag_or_anchor && ty.is_container())
    {
        if(!(ty & CONTAINER_STYLE))
            ty |= BLOCK;
        if(ty.is_block() && m_tree->has_children(node))
            _pend_newl();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_seq_close_entry(id_type node)
{
    (void)node;
    _pend_newl();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_map_open_entry(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->has_key(node), m_tree, node);
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    if(ty.has_key_tag())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->key_tag(node));
    }
    if(ty.has_key_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->key_anchor(node));
    }
    if(ty.is_key_ref())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_ref(m_tree->key(node));
    }
    else
    {
        _write_pws_and_pend(_PWS_NONE);
        _blck_write_scalar_key(node);
        if(ty & (KEY_LITERAL|KEY_FOLDED))
            _indent(m_ilevel);
    }
    _write_pws_and_pend(_PWS_SPACE); // pend the space after the colon
    _write(':');
    if(ty.has_val_tag())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
    }
    if(ty.is_container() && m_tree->has_children(node))
    {
        if(!(ty & CONTAINER_STYLE))
            ty |= BLOCK;
        if(ty.is_block())
            _pend_newl();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_map_close_entry(id_type node)
{
    (void)node;
    _pend_newl();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_seq(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    bool empty = true;
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _blck_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _blck_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            _visit_blck_container(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_seq_close_entry(child);
    }
    if(empty)
    {
        _write_pws_and_pend(_PWS_NONE);
        _write("[]");
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_map(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    bool empty = true;
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_keyval() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _blck_map_open_entry(child); // also writes the key
        if(ty.is_keyval())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _blck_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            _visit_blck_container(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_map_close_entry(child);
    }
    if(empty)
    {
        _write_pws_and_pend(_PWS_NONE);
        _write("{}");
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl_seq(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    _write('[');
    for(id_type first = m_tree->first_child(node), child = first; child != NONE; child = m_tree->next_sibling(child))
    {
        _flow_sl_write_comma(child, first);
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _flow_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _flow_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_seq_close_entry(child);
    }
    _write(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl_map(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    _write('{');
    for(id_type first = m_tree->first_child(node), child = first; child != NONE; child = m_tree->next_sibling(child))
    {
        _flow_sl_write_comma(child, first);
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        _flow_map_open_entry(child);
        if(ty.has_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _flow_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_map_close_entry(child);
    }
    _write('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml_map(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    _write('{');
    _pend_newl();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        _flow_map_open_entry(child);
        if(ty.has_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _flow_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_map_close_entry(child);
        _flow_ml_write_comma(child, last);
    }
    if(m_opts.indent_flow_ml()) --m_ilevel;
    _write_pws_and_pend(_PWS_NONE);
    _write('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml_seq(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    _write('[');
    _pend_newl();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _flow_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            if(!ty.is_val_ref())
                _flow_write_scalar_val(child);
            else
                _write_ref(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_seq_close_entry(child);
        _flow_ml_write_comma(child, last);
    }
    if(m_opts.indent_flow_ml()) --m_ilevel;
    _write_pws_and_pend(_PWS_NONE);
    _write(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(m_depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    const NodeType ty = m_tree->type(node);
    if(ty.is_seq())
    {
        _visit_blck_seq(node);
    }
    else
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        _visit_blck_map(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(m_depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    const NodeType ty = m_tree->type(node);
    if(ty.is_seq())
    {
        _visit_flow_sl_seq(node);
    }
    else
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        _visit_flow_sl_map(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(m_depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    const NodeType ty = m_tree->type(node);
    if(ty.is_seq())
    {
        _visit_flow_ml_seq(node);
    }
    else if(ty.is_map())
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_map(), m_tree, node);
        _visit_flow_ml_map(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_write_scalar_key(id_type id)
{
    enum : type_bits {
        _block_styles = (KEY_LITERAL|KEY_FOLDED),
        _flow_styles = KEY_STYLE & ~(KEY_LITERAL|KEY_FOLDED),
    };
    csubstr str = m_tree->key(id);
    NodeType ty = m_tree->type(id) & static_cast<NodeType_e>(_flow_styles);
    if(!(ty & KEY_STYLE))
        ty |= scalar_style_choose(str);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !(ty & (KEY_LITERAL|KEY_FOLDED)), m_tree, id);
    if(ty & KEY_PLAIN)
    {
        if(C4_LIKELY(!(str.begins_with(": ") || str.begins_with(":\t"))))
            _write_scalar_plain(str, m_ilevel);
        else
            _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & KEY_SQUO)
    {
        _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & KEY_DQUO)
    {
        _write_scalar_dquo(str, m_ilevel);
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "not implemented"); // LCOV_EXCL_LINE
    }
}

template<class Writer>
void Emitter<Writer>::_flow_write_scalar_val(id_type id)
{
    enum : type_bits {
        _block_styles = VAL_LITERAL|VAL_FOLDED,
        _flow_styles = (VAL_STYLE) & (~(VAL_LITERAL|VAL_FOLDED)),
    };
    csubstr str = m_tree->val(id);
    NodeType ty = m_tree->type(id) & static_cast<NodeType_e>(_flow_styles);
    if(!(ty & VAL_STYLE))
        ty |= scalar_style_choose(str);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !(ty & (VAL_LITERAL|VAL_FOLDED)), m_tree, id);
    if(ty & VAL_PLAIN)
    {
        if(C4_LIKELY(!(str.begins_with(": ") || str.begins_with(":\t"))))
            _write_scalar_plain(str, m_ilevel);
        else
            _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & VAL_SQUO)
    {
        _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & VAL_DQUO)
    {
        _write_scalar_dquo(str, m_ilevel);
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "not implemented"); // LCOV_EXCL_LINE
    }
}

template<class Writer>
void Emitter<Writer>::_blck_write_scalar_key(id_type id)
{
    csubstr key = m_tree->key(id);
    NodeType ty = m_tree->type(id);
    if(!(ty & KEY_STYLE))
        ty |= scalar_style_choose(key);
    if(ty & KEY_PLAIN)
    {
        if(C4_LIKELY(!(key.begins_with(": ") || key.begins_with(":\t"))))
            _write_scalar_plain(key, m_ilevel);
        else
            _write_scalar_squo(key, m_ilevel);
    }
    else if(ty & KEY_SQUO)
    {
        _write_scalar_squo(key, m_ilevel);
    }
    else if(ty & KEY_DQUO)
    {
        _write_scalar_dquo(key, m_ilevel);
    }
    else if(ty & KEY_LITERAL)
    {
        _write_scalar_literal(key, m_ilevel, /*explicit*/true);
    }
    else if(ty & KEY_FOLDED)
    {
        _write_scalar_folded(key, m_ilevel, /*explicit*/true);
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "not implemented"); // LCOV_EXCL_LINE
    }
}

template<class Writer>
void Emitter<Writer>::_blck_write_scalar_val(id_type id)
{
    csubstr str = m_tree->val(id);
    NodeType ty = m_tree->type(id);
    if(!(ty & VAL_STYLE))
        ty |= scalar_style_choose(str);
    if(ty & VAL_PLAIN)
    {
        if(C4_LIKELY(!(str.begins_with(": ") || str.begins_with(":\t"))))
            _write_scalar_plain(str, m_ilevel);
        else
            _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & VAL_SQUO)
    {
        _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & VAL_DQUO)
    {
        _write_scalar_dquo(str, m_ilevel);
    }
    else if(ty & VAL_LITERAL)
    {
        _write_scalar_literal(str, m_ilevel, /*explicit*/false);
    }
    else if(ty & VAL_FOLDED)
    {
        _write_scalar_folded(str, m_ilevel, /*explicit*/false);
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "not implemented"); // LCOV_EXCL_LINE
    }
}

template<class Writer>
size_t Emitter<Writer>::_write_escaped_newlines(csubstr s, size_t i)
{
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.len > i);
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.str[i] == '\n');
    //_c4dbgpf("nl@i={} rem=[{}]~~~{}~~~", i, s.sub(i).len, s.sub(i));
    // add an extra newline for each sequence of consecutive
    // newline/whitespace
    _newl();
    do
    {
        _newl(); // write the newline again
        ++i; // increase the outer loop counter!
    } while(i < s.len && s.str[i] == '\n');
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), i > 0);
    --i;
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.str[i] == '\n');
    return i;
}


inline bool _is_indented_block(csubstr s, size_t prev, size_t i) noexcept
{
    if(prev == 0 && s.begins_with_any(" \t"))
        return true;
    const size_t pos = s.first_not_of('\n', i);
    return (pos != npos) && (s.str[pos] == ' ' || s.str[pos] == '\t');
}


template<class Writer>
size_t Emitter<Writer>::_write_indented_block(csubstr s, size_t i, id_type ilevel)
{
    //_c4dbgpf("indblock@i={} rem=[{}]~~~\n{}~~~", i, s.sub(i).len, s.sub(i));
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), i > 0);
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.str[i-1] == '\n');
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), i < s.len);
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.str[i] == ' ' || s.str[i] == '\t' || s.str[i] == '\n');
again:
    size_t pos = s.find("\n ", i);
    if(pos == npos)
        pos = s.find("\n\t", i);
    if(pos != npos)
    {
        ++pos;
        //_c4dbgpf("indblock line@i={} rem=[{}]~~~\n{}~~~", i, s.range(i, pos).len, s.range(i, pos));
        _indent(ilevel + 1);
        _write(s.range(i, pos));
        i = pos;
        goto again; // NOLINT
    }
    // consume the newlines after the indented block
    // to prevent them from being escaped
    pos = s.find('\n', i);
    if(pos != npos)
    {
        const size_t pos2 = s.first_not_of('\n', pos);
        pos = (pos2 != npos) ? pos2 : pos;
        //_c4dbgpf("indblock line@i={} rem=[{}]~~~\n{}~~~", i, s.range(i, pos).len, s.range(i, pos));
        _indent(ilevel + 1);
        _write(s.range(i, pos));
        i = pos;
    }
    return i;
}

template<class Writer>
void Emitter<Writer>::_write_scalar_literal(csubstr s, id_type ilevel, bool explicit_key)
{
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.find("\r") == csubstr::npos);
    if(explicit_key)
        _write("? ");
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    _write('|');
    if(explicit_indentation)
        _write('2');
    //
    if(numnewlines_at_end > 1 || is_newline_only)
        _write('+');
    else if(numnewlines_at_end == 0)
        _write('-');
    //
    if(trimmed.len)
    {
        _newl();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // write everything up to this point
            csubstr since_pos = trimmed.range(pos, i+1); // include the newline
            _indent(ilevel + 1);
            _write(since_pos);
            pos = i+1; // already written
        }
        if(pos < trimmed.len)
        {
            _indent(ilevel + 1);
            _write(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        _newl();
    if(explicit_key)
        _newl();
}

template<class Writer>
void Emitter<Writer>::_write_scalar_folded(csubstr s, id_type ilevel, bool explicit_key)
{
    if(explicit_key)
        _write("? ");
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.find("\r") == csubstr::npos);
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    _write('>');
    if(explicit_indentation)
        _write('2');
    //
    if(numnewlines_at_end == 0)
        _write('-');
    else if(numnewlines_at_end > 1 || is_newline_only)
        _write('+');
    //
    if(trimmed.len)
    {
        _newl();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // escape newline sequences
            if( ! _is_indented_block(s, pos, i))
            {
                if(pos < i)
                {
                    _indent(ilevel + 1);
                    _write(s.range(pos, i));
                    i = _write_escaped_newlines(s, i);
                    pos = i + 1;
                }
                else
                {
                    if(i+1 < s.len)
                    {
                        if(s.str[i+1] == '\n')
                        {
                            ++i;
                            i = _write_escaped_newlines(s, i);
                            pos = i+1;
                        }
                        else
                        {
                            _newl();
                            pos = i+1;
                        }
                    }
                }
            }
            else // do not escape newlines in indented blocks
            {
                ++i;
                _indent(ilevel + 1);
                _write(s.range(pos, i));
                if(pos > 0 || !s.begins_with_any(" \t"))
                    i = _write_indented_block(s, i, ilevel);
                pos = i;
            }
        }
        if(pos < trimmed.len)
        {
            _indent(ilevel + 1);
            _write(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        _newl();
    if(explicit_key)
        _newl();
}

template<class Writer>
void Emitter<Writer>::_write_scalar_squo(csubstr s, id_type ilevel)
{
    size_t pos = 0; // tracks the last character that was already written
    _write('\'');
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s[i] == '\n')
        {
            _write(s.range(pos, i));  // write everything up to (excluding) this char
            //_c4dbgpf("newline at {}. writing ~~~{}~~~", i, s.range(pos, i));
            i = _write_escaped_newlines(s, i);
            //_c4dbgpf("newline --> {}", i);
            if(i < s.len)
                _indent(ilevel + 1);
            pos = i+1;
        }
        else if(s[i] == '\'')
        {
            csubstr sub = s.range(pos, i+1);
            //_c4dbgpf("squote at {}. writing ~~~{}~~~", i, sub);
            _write(sub); // write everything up to (including) this squote
            _write('\''); // write the squote again
            pos = i+1;
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        _write(s.sub(pos));
    _write('\'');
}

template<class Writer>
void Emitter<Writer>::_write_scalar_dquo(csubstr s, id_type ilevel)
{
    size_t pos = 0; // tracks the last character that was already written
    _write('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        switch(curr) // NOLINT
        {
        case '"':
        case '\\':
        {
            csubstr sub = s.range(pos, i);
            _write(sub);  // write everything up to (excluding) this char
            _write('\\'); // write the escape
            _write(curr); // write the char
            pos = i+1;
            break;
        }
#ifndef prefer_writing_newlines_as_double_newlines
        case '\n':
        {
            csubstr sub = s.range(pos, i);
            _write(sub);   // write everything up to (excluding) this char
            _write("\\n"); // write the escape
            pos = i+1;
            (void)ilevel;
            break;
        }
#else
        case '\n':
        {
            // write everything up to (excluding) this newline
            //_c4dbgpf("nl@i={} rem=[{}]~~~{}~~~", i, s.sub(i).len, s.sub(i));
            _write(s.range(pos, i));
            i = _write_escaped_newlines(s, i);
            ++i;
            pos = i;
            // as for the next line...
            if(i < s.len)
            {
                _indent(ilevel + 1); // indent the next line
                // escape leading whitespace, and flush it
                size_t first = s.first_not_of(" \t", i);
                //_c4dbgpf("@i={} first={} rem=[{}]~~~{}~~~", i, first, s.sub(i).len, s.sub(i));
                if(first > i)
                {
                    if(first == npos)
                        first = s.len;
                    _write('\\');
                    _write(s.range(i, first));
                    _write('\\');
                    i = first-1;
                    pos = first;
                }
            }
            break;
        }
        // escape trailing whitespace before a newline
        case ' ':
        case '\t':
        {
            const size_t next = s.first_not_of(" \t\r", i);
            if(next != npos && s.str[next] == '\n')
            {
                csubstr sub = s.range(pos, i);
                _write(sub);  // write everything up to (excluding) this char
                _write('\\'); // escape the whitespace
                pos = i;
            }
            break;
        }
#endif
        case '\r':
        {
            csubstr sub = s.range(pos, i);
            _write(sub);  // write everything up to (excluding) this char
            _write("\\r"); // write the escaped char
            pos = i+1;
            break;
        }
        case '\b':
        {
            csubstr sub = s.range(pos, i);
            _write(sub);  // write everything up to (excluding) this char
            _write("\\b"); // write the escaped char
            pos = i+1;
            break;
        }
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        _write(s.sub(pos));
    _write('"');
}

template<class Writer>
void Emitter<Writer>::_write_scalar_plain(csubstr s, id_type ilevel)
{
    if(C4_UNLIKELY(ilevel == 0 && (s.begins_with("...") || s.begins_with("---"))))
    {
        _indent(ilevel + 1); // indent the next line
        ++ilevel;
    }
    size_t pos = 0; // tracks the last character that was already written
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        if(curr == '\n')
        {
            csubstr sub = s.range(pos, i);
            _write(sub);  // write everything up to (including) this newline
            i = _write_escaped_newlines(s, i);
            pos = i+1;
            if(pos < s.len)
                _indent(ilevel + 1); // indent the next line
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        _write(s.sub(pos));
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_emit_json(id_type id)
{
    NodeType ty = m_tree->type(id);
    if(ty.is_flow_sl() || !(ty & CONTAINER_STYLE))
    {
        _visit_json_sl(id, 0);
    }
    else
    {
        _visit_json_ml(id, 0);
        _newl();
    }
}

template<class Writer>
void Emitter<Writer>::_visit_json_sl(id_type id, id_type depth)
{
    _RYML_CHECK_VISIT_(m_tree->callbacks(), !m_tree->is_stream(id), m_tree, id); // JSON does not have streams
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "max depth exceeded");
    NodeType ty = m_tree->type(id);
    if(ty.is_keyval())
    {
        _writek_json(id, ty);
        _write(": ");
        _writev_json(id, ty);
    }
    else if(ty.is_val())
    {
        _writev_json(id, ty);
    }
    else if(ty.is_container())
    {
        if(ty.has_key())
        {
            _writek_json(id, ty);
            _write(": ");
        }
        if(ty.is_seq())
            _write('[');
        else if(ty.is_map())
            _write('{');
    }  // container

    for(id_type child = m_tree->first_child(id); child != NONE; child = m_tree->next_sibling(child))
    {
        if(child != m_tree->first_child(id))
            _write(',');
        _visit_json_sl(child, depth+1);
    }

    if(ty.is_seq())
        _write(']');
    else if(ty.is_map())
        _write('}');
}

template<class Writer>
void Emitter<Writer>::_visit_json_ml(id_type id, id_type depth)
{
    _RYML_CHECK_VISIT_(m_tree->callbacks(), !m_tree->is_stream(id), m_tree, id); // JSON does not have streams
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "max depth exceeded");
    NodeType ty = m_tree->type(id);
    if(ty.is_keyval())
    {
        _writek_json(id, ty);
        _write(": ");
        _writev_json(id, ty);
    }
    else if(ty.is_val())
    {
        _writev_json(id, ty);
    }
    else if(ty.is_container())
    {
        if(ty.has_key())
        {
            _writek_json(id, ty);
            _write(": ");
        }
        if(ty.is_seq())
            _write('[');
        else if(ty.is_map())
            _write('{');
    }  // container

    if(m_tree->has_children(id))
    {
        ++depth;
        if(m_opts.indent_flow_ml()) ++m_ilevel;
        const id_type first = m_tree->first_child(id);
        const id_type last = m_tree->last_child(id);
        for(id_type child = first; child != NONE; child = m_tree->next_sibling(child))
        {
            _newl();
            _indent(m_ilevel);
            NodeType chty = m_tree->type(child);
            if(chty.is_flow_sl() || !(chty & CONTAINER_STYLE))
                _visit_json_sl(child, depth);
            else
                _visit_json_ml(child, depth);
            if(child != last)
                _write(',');
        }
        if(m_opts.indent_flow_ml()) --m_ilevel;
        --depth;
        _newl();
        _indent(m_ilevel);
    }

    if(ty.is_seq())
        _write(']');
    else if(ty.is_map())
        _write('}');
}

template<class Writer>
void Emitter<Writer>::_writek_json(id_type id, NodeType ty)
{
    if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_TAG)
        if(C4_UNLIKELY(ty.has_key_tag()))
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "JSON does not have tags");
    if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_ANCHOR)
        if(C4_UNLIKELY(ty.has_key_anchor()))
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
    csubstr key = m_tree->key(id);
    if(key.len)
        _write_scalar_json_dquo(key);
    else
        _write("\"\"");
}

template<class Writer>
void Emitter<Writer>::_writev_json(id_type id, NodeType ty)
{
    if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_TAG)
        if(C4_UNLIKELY(ty.has_val_tag()))
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "JSON does not have tags");
    if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_ANCHOR)
        if(C4_UNLIKELY(ty.has_val_anchor()))
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "JSON does not have anchors");
    csubstr val = m_tree->val(id);
    if(val.len)
    {
        // use double quoted style if the style is marked quoted
        bool dquoted = ((ty & VALQUO)
                        || (scalar_style_json_choose(val) & SCALAR_DQUO)); // choose the style
        if(dquoted)
            _write_scalar_json_dquo(val);
        else
            _write(val);
    }
    else
    {
        if(val.str || (ty & (VALQUO|VALTAG)))
            _write("\"\"");
        else
            _write("null");
    }
}


template<class Writer>
void Emitter<Writer>::_write_scalar_json_dquo(csubstr s)
{
    size_t pos = 0;
    _write('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        switch(s.str[i])
        {
        case '"':
            _write(s.range(pos, i));
            _write("\\\"");
            pos = i + 1;
            break;
        case '\n':
            _write(s.range(pos, i));
            _write("\\n");
            pos = i + 1;
            break;
        case '\t':
            _write(s.range(pos, i));
            _write("\\t");
            pos = i + 1;
            break;
        case '\\':
            _write(s.range(pos, i));
            _write("\\\\");
            pos = i + 1;
            break;
        case '\r':
            _write(s.range(pos, i));
            _write("\\r");
            pos = i + 1;
            break;
        case '\b':
            _write(s.range(pos, i));
            _write("\\b");
            pos = i + 1;
            break;
        case '\f':
            _write(s.range(pos, i));
            _write("\\f");
            pos = i + 1;
            break;
        }
    }
    if(pos < s.len)
    {
        csubstr sub = s.sub(pos);
        _write(sub);
    }
    _write('"');
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_EMIT_DEF_HPP_ */
