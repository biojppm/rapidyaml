#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "c4/yml/emit.hpp"
#endif

/** @file emit.def.hpp Definitions for emit functions. */
#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#include "c4/yml/detail/dbgprint.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

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
    _RYML_WITH_COMMENTS(m_wsonly = true);
    _RYML_WITH_COMMENTS(m_comm_state.clear());
    if(type == EMIT_YAML)
        _emit_yaml(id);
    else if(type == EMIT_JSON)
        _emit_json(id);
    else
        _RYML_ERR_BASIC_(m_tree->callbacks(), "unknown emit type"); // LCOV_EXCL_LINE
    m_tree = nullptr;
    return this->Writer::_get(error_on_excess);
}

/** @cond dev */

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
        _blck_close_entry(id);
    }
    else if(emit_dash)
    {
        --m_ilevel;
        _blck_close_entry(id);
    }
    else
    {
        _top_close_entry(id);
    }

    if(ty.is_flow_ml())
    {
        _newl();
    }
    else if(m_tree->is_root(id)
       || emit_dash || emit_key
       || !ty.is_val()
       || !ty.is_val_plain())
    {
        _write_pws_and_pend(_PWS_NONE);
    }
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
            if(next_node != m_tree->first_child(parent))
            {
                _write_pws_and_pend(_PWS_NEWL);
                _write("...");
            }
            _write_pws_and_pend(_PWS_NONE);
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
        if(m_tree->is_val(child))
        {
            if((m_tree->type(child) & VALNIL) _RYML_WITH_COMMENTS( && !m_tree->comment(child)))
                _pend_newl();
        }
        else if(m_tree->is_container(child))
        {
            if(m_tree->is_flow(child))
                _pend_newl();
        }
        if(m_tree->next_sibling(child) != NONE)
        {
            write_tag_directives(m_tree->next_sibling(child));
        }
    }
    --m_depth;
    #ifdef RYML_WITH_COMMENTS
    CommentData const* comm = _comm_get(id, COMM_TRAILING);
    if(comm)
    {
        _write_pws_and_pend(_PWS_NONE);
        _write("...");
        _write_comm_trailing(comm);
    }
    _write_comm_leading(id, COMM_FOOTER);
    #endif
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
    const type_bits val_style = ty & VAL_STYLE;
    const bool is_ambiguous = ((ty & VAL_PLAIN) || !val_style)
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
        if(!val_style)
            ty = scalar_style_choose(val);
        _blck_write_scalar(val, val_style);
    }
    if(is_ambiguous)
    {
        --m_ilevel;
    }
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
    _RYML_WITH_COMMENTS(_comm_push());
    if(ty.is_doc() && !m_tree->is_root(node))
    {
        _write("---");
        _pend_space();
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_DOC_TRAILING));
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_LEADING));
    if(ty.has_val_anchor())
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_ANCHOR_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_ANCHOR_TRAILING));
    }
    if(ty.has_val_tag())
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_TAG_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_TAG_TRAILING));
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
            if(!ty.is_flow())
                _pend_newl();
        }
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_top_close_entry(id_type node)
{
    #ifdef RYML_WITH_COMMENTS
    if(!m_tree->is_stream(node)) // streams are handled in _write_stream()
    {
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_TRAILING));
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_FOOTER));
    }
    _comm_pop();
    #endif
    if(m_tree->is_val(node) && !(m_tree->type(node) & VALNIL))
    {
        _pend_newl();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    _RYML_WITH_COMMENTS(_comm_push());
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_LEADING));
    if(ty & VALANCH)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_ANCHOR_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_ANCHOR_TRAILING));
    }
    if(ty & VALTAG)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_TAG_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_TAG_TRAILING));
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING2));
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_map_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key(), m_tree, node);
    _RYML_WITH_COMMENTS(_comm_push());
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_LEADING));
    if(ty & KEYANCH)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_KEY_ANCHOR_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write("&");
        _write(m_tree->key_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_KEY_ANCHOR_TRAILING));
    }
    if(ty & KEYTAG)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_KEY_TAG_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->key_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_KEY_TAG_TRAILING));
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_KEY_LEADING));
    if(ty & KEYREF)
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_ref(m_tree->key(node));
    }
    else
    {
        _write_pws_and_pend(_PWS_NONE);
        csubstr key = m_tree->key(node);
        if(!(ty & (NodeType_e)_styles_flow_key))
            ty |= scalar_style_choose(key) & (NodeType_e)_styles_flow_key;
        _flow_write_scalar(key, ty & (NodeType_e)_styles_flow_key);
    }
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_KEY_TRAILING));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_COLON_LEADING));
    _write_pws_and_pend(_PWS_SPACE);
    _write(':');
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_COLON_TRAILING));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING));
    if(ty & VALANCH)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_ANCHOR_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_ANCHOR_TRAILING));
    }
    if(ty & VALTAG)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_TAG_LEADING));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_TAG_TRAILING));
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING2));
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_close_entry_sl(id_type node, id_type last_sibling)
{
    _RYML_WITH_COMMENTS(CommentData const *commvt = _comm_get(node, COMM_VAL_TRAILING));
    _RYML_WITH_COMMENTS(CommentData const *commcl = _comm_get(node, COMM_COMMA_LEADING));
    if(node != last_sibling _RYML_WITH_COMMENTS(|| commvt || commcl))
    {
        _RYML_WITH_COMMENTS(if(commvt) _write_comm_trailing(commvt));
        _RYML_WITH_COMMENTS(if(commcl) _write_comm_leading(commcl));
        _write_pws_and_pend(_PWS_NONE);
        _write(',');
    }
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_TRAILING));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_FOOTER));
    _RYML_WITH_COMMENTS(_comm_pop());
}

template<class Writer>
void Emitter<Writer>::_flow_close_entry_ml(id_type node, id_type last_sibling)
{
    _flow_close_entry_sl(node, last_sibling);
    _pend_newl();
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _write_pws_and_pend(_PWS_NONE);
    _RYML_WITH_COMMENTS(_comm_push());
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_LEADING));
    _write_pws_and_pend(_PWS_SPACE); // pend the space after the following dash
    _write('-');
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_DASH_TRAILING, /*indent_extra*/true));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING, /*indent_extra*/true));
    bool has_tag_or_anchor = false;
    if(ty & VALANCH)
    {
        has_tag_or_anchor = true;
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_ANCHOR_LEADING, /*indent_extra*/true));
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_ANCHOR_TRAILING, /*indent_extra*/true));
    }
    if(ty & VALTAG)
    {
        has_tag_or_anchor = true;
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_TAG_LEADING, /*indent_extra*/true));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_TAG_TRAILING, /*indent_extra*/true));
    }
    if(has_tag_or_anchor && ty.is_container())
    {
        if(!(ty & CONTAINER_STYLE))
            ty |= BLOCK;
        if((ty & BLOCK) && m_tree->has_children(node))
            _pend_newl();
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING2, /*indent_extra*/true));
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_write_qmrk(id_type node, csubstr key, type_bits ty, bool has_qmrk_comments)
{
#ifndef RYML_WITH_COMMENTS
    (void)node;
    (void)key;
    (void)ty;
    (void)has_qmrk_comments;
#else
#define _ryml_write_comm_leading_and_nest(node, commtype) \
    do {                                                                \
        CommentData const* comm = _comm_get(node, commtype);            \
        if(comm)                                                        \
        {                                                               \
            if(!nested)                                                 \
            {                                                           \
                nested = true;                                          \
                ++m_ilevel;                                             \
            }                                                           \
            _write_comm_leading(comm);                                  \
        }                                                               \
    } while(0)
#define _ryml_write_comm_trailing_and_nest(node, commtype)              \
    do {                                                                \
        if(_write_comm_trailing(node, commtype))                        \
        {                                                               \
            if(!nested)                                                 \
            {                                                           \
                nested = true;                                          \
                ++m_ilevel;                                             \
            }                                                           \
        }                                                               \
    } while(0)
    bool nested = false;
    _write_pws_and_pend(_PWS_SPACE);
    _write('?');
    _ryml_write_comm_trailing_and_nest(node, COMM_KEY_TRAILING_QMRK);
    if(ty & KEYANCH)
    {
        _ryml_write_comm_leading_and_nest(node, COMM_KEY_ANCHOR_LEADING);
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->key_anchor(node));
        _ryml_write_comm_trailing_and_nest(node, COMM_KEY_ANCHOR_TRAILING);
    }
    if(ty & KEYTAG)
    {
        _ryml_write_comm_leading_and_nest(node, COMM_KEY_TAG_LEADING);
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->key_tag(node));
        _ryml_write_comm_trailing_and_nest(node, COMM_KEY_TAG_TRAILING);
    }
    _ryml_write_comm_leading_and_nest(node, COMM_KEY_LEADING);
    if(!(ty & KEYREF))
    {
        _write_pws_and_pend(_PWS_NEWL);
        if(!(ty & KEY_STYLE))
            ty &= (scalar_style_choose(key) & KEY_STYLE);
        if(!(ty & _styles_block_key) || !has_qmrk_comments || nested)
        {
            _blck_write_scalar(key, ty & KEY_STYLE);
        }
        else
        {
            ++m_ilevel;
            _blck_write_scalar(key, ty & KEY_STYLE);
            --m_ilevel;
        }
    }
    else
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_ref(key);
    }
    // make sure trailing comments are not printed directly after block scalars
    if(!(ty & _styles_block_key))
        _ryml_write_comm_trailing_and_nest(node, COMM_KEY_TRAILING);
    else
        _ryml_write_comm_leading_and_nest(node, COMM_KEY_TRAILING);
    _ryml_write_comm_leading_and_nest(node, COMM_KEY_FOOTER);
    if(nested)
        --m_ilevel;
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_COLON_LEADING));
    #undef _ryml_write_comm_leading_and_nest
    #undef _ryml_write_comm_trailing_and_nest
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_map_open_entry(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->has_key(node), m_tree, node);
    NodeType ty = m_tree->type(node);
    csubstr key = m_tree->key(node);
    if(!(ty & (KEY_STYLE|KEYREF)))
        ty |= (scalar_style_choose(key) & KEY_STYLE);
    _write_pws_and_pend(_PWS_NONE);
    _RYML_WITH_COMMENTS(_comm_push());
#ifndef RYML_WITH_COMMENTS
    if(ty & KEYANCH)
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->key_anchor(node));
    }
    if(ty & KEYTAG)
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->key_tag(node));
    }
    if(ty & KEYREF)
    {
        _write_pws_and_pend(_PWS_SPACE);
        _write_ref(key);
    }
    else
    {
        _write_pws_and_pend(_PWS_NONE);
        type_bits use_qmrk = ty & (NodeType_e)_styles_block_key;
        if(!use_qmrk)
        {
            _blck_write_scalar(key, ty & KEY_STYLE);
        }
        else
        {
            _write("? ");
            _blck_write_scalar(key, ty & KEY_STYLE);
            _pend_newl();
        }
    }
#else
    enum : comment_data_type {
        _comms_requiring_qmrk =
        COMM_KEY_TRAILING_QMRK
        | COMM_KEY_FOOTER_QMRK
        | COMM_KEY_TAG_LEADING
        | COMM_KEY_TAG_TRAILING
        | COMM_KEY_ANCHOR_LEADING
        | COMM_KEY_ANCHOR_TRAILING
        | COMM_KEY_LEADING
        | COMM_KEY_TRAILING
        | COMM_KEY_FOOTER
        | COMM_COLON_LEADING
    };
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_LEADING));
    // should we use an explicit key (with leading '?')?
    bool has_qmrk_comments = m_opts.comments() && !!m_tree->comment(node, _comms_requiring_qmrk);
    if(!((ty & _styles_block_key) || has_qmrk_comments))
    {
        if(ty & KEYANCH)
        {
            _write_pws_and_pend(_PWS_SPACE);
            _write('&');
            _write(m_tree->key_anchor(node));
        }
        if(ty & KEYTAG)
        {
            _write_pws_and_pend(_PWS_SPACE);
            _write_tag(m_tree->key_tag(node));
        }
        if(!(ty & KEYREF))
        {
            if(!(ty & KEY_STYLE))
                ty &= (scalar_style_choose(key) & KEY_STYLE);
            _write_pws_and_pend(_PWS_NONE);
            _blck_write_scalar(key, ty & KEY_STYLE);
        }
        else
        {
            _write_pws_and_pend(_PWS_SPACE);
            _write_ref(key);
        }
    }
    else
    {
        _blck_write_qmrk(node, key, ty, has_qmrk_comments);
    }
#endif
    _write_pws_and_pend(_PWS_SPACE); // pend the space after the colon
    _write(':');
    _RYML_WITH_COMMENTS(bool indent_extra = (ty & VAL));
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_COLON_TRAILING, indent_extra));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING, indent_extra));
    if(ty & VALANCH)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_ANCHOR_LEADING, indent_extra));
        _write_pws_and_pend(_PWS_SPACE);
        _write('&');
        _write(m_tree->val_anchor(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_ANCHOR_TRAILING, indent_extra));
    }
    if(ty & VALTAG)
    {
        _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_TAG_LEADING, indent_extra));
        _write_pws_and_pend(_PWS_SPACE);
        _write_tag(m_tree->val_tag(node));
        _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_TAG_TRAILING, indent_extra));
    }
    if(ty.is_container() && m_tree->has_children(node))
    {
        if(!(ty & CONTAINER_STYLE))
            ty |= BLOCK;
        if(ty.is_block())
            _pend_newl();
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_LEADING2, indent_extra));
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_close_entry(id_type node)
{
    (void)node;
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_TRAILING));
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_FOOTER));
    _RYML_WITH_COMMENTS(_comm_pop());
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
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _blck_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & VAL_STYLE))
                    ty |= (scalar_style_choose(val) & VAL_STYLE);
                _blck_write_scalar(val, ty & VAL_STYLE);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            _visit_blck_container(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_close_entry(child);
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
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_keyval() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _blck_map_open_entry(child); // also writes the key
        if(ty.is_keyval())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & VAL_STYLE))
                    ty |= (scalar_style_choose(val) & VAL_STYLE);
                _blck_write_scalar(val, ty & VAL_STYLE);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            _visit_blck_container(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_close_entry(child);
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
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_BRACKET_TRAILING));
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _flow_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & (NodeType_e)_styles_flow_val))
                    ty |= (scalar_style_choose(val) & (NodeType_e)_styles_flow_val);
                _flow_write_scalar(val, ty & (NodeType_e)_styles_flow_val);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_close_entry_sl(child, last);
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_BRACKET_LEADING));
    _write(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml_seq(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    _write('[');
    _pend_newl();
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_BRACKET_TRAILING));
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        _flow_seq_open_entry(child);
        if(ty.is_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & (NodeType_e)_styles_flow_val))
                    ty |= (scalar_style_choose(val) & (NodeType_e)_styles_flow_val);
                _flow_write_scalar(val, ty & (NodeType_e)_styles_flow_val);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_close_entry_ml(child, last);
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_BRACKET_LEADING));
    if(m_opts.indent_flow_ml()) --m_ilevel;
    _write_pws_and_pend(_PWS_NONE);
    _write(']');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl_map(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    _write('{');
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_BRACKET_TRAILING));
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        _flow_map_open_entry(child);
        if(ty.has_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & (NodeType_e)_styles_flow_val))
                    ty |= (scalar_style_choose(val) & (NodeType_e)_styles_flow_val);
                _flow_write_scalar(val, ty & (NodeType_e)_styles_flow_val);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_close_entry_sl(child, last);
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_BRACKET_LEADING));
    _write_pws_and_pend(_PWS_NONE);
    _write('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml_map(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    _write('{');
    _pend_newl();
    _RYML_WITH_COMMENTS(_write_comm_trailing(node, COMM_VAL_BRACKET_TRAILING));
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    for(id_type child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        _flow_map_open_entry(child);
        if(ty.has_val())
        {
            _write_pws_and_pend(_PWS_NONE);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty & (NodeType_e)_styles_flow_val))
                    ty |= (scalar_style_choose(val) & (NodeType_e)_styles_flow_val);
                _flow_write_scalar(val, ty & (NodeType_e)_styles_flow_val);
            }
            else
            {
                _write_ref(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            _visit_flow_container(child);
            --m_depth;
        }
        _flow_close_entry_ml(child, last);
    }
    _RYML_WITH_COMMENTS(_write_comm_leading(node, COMM_VAL_BRACKET_LEADING));
    if(m_opts.indent_flow_ml()) --m_ilevel;
    _write_pws_and_pend(_PWS_NONE);
    _write('}');
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
void Emitter<Writer>::_flow_write_scalar(csubstr str, type_bits ty)
{
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), !(ty & _styles_block));
    if((ty & _styles_plain) || !(ty & SCALAR_STYLE))
    {
        _write_scalar_plain(str, m_ilevel);
    }
    else if(ty & _styles_squo)
    {
        _write_scalar_squo(str, m_ilevel);
    }
    else // if(ty & _styles_dquo)
    {
        _write_scalar_dquo(str, m_ilevel);
    }
}

template<class Writer>
void Emitter<Writer>::_blck_write_scalar(csubstr str, type_bits ty)
{
    if((ty & _styles_plain) || !(ty & SCALAR_STYLE))
    {
        _write_scalar_plain(str, m_ilevel);
    }
    else if(ty & _styles_squo)
    {
        _write_scalar_squo(str, m_ilevel);
    }
    else if(ty & _styles_dquo)
    {
        _write_scalar_dquo(str, m_ilevel);
    }
    else if(ty & _styles_literal)
    {
        _write_scalar_literal(str, m_ilevel);
    }
    else // if(ty & _styles_folded)
    {
        _write_scalar_folded(str, m_ilevel);
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
void Emitter<Writer>::_write_scalar_literal(csubstr s, id_type ilevel)
{
    _RYML_ASSERT_BASIC_(m_tree->callbacks(), s.find("\r") == csubstr::npos);
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
}

template<class Writer>
void Emitter<Writer>::_write_scalar_folded(csubstr s, id_type ilevel)
{
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

#ifdef RYML_WITH_COMMENTS
template<class Writer>
void Emitter<Writer>::_write_comm(csubstr s, id_type indentation)
{
    _write('#');
    size_t pos = 0; // last character that was already written
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s.str[i] == '\n')
        {
            csubstr sub = s.range(pos, i);
            _write(sub);  // write everything up to (including) this newline
            _newl();
            pos = i + 1;
            _write(' ', indentation); // _indent() is for level, but we have explicit indentation
            _write('#');
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
    {
        csubstr sub = s.sub(pos);
        _write(sub);
    }
}

template<class Writer>
void Emitter<Writer>::_write_comm_leadspace(csubstr s, id_type indentation)
{
    _write('#');
    size_t pos = 0; // last character that was already written
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s.str[i] == '\n')
        {
            csubstr sub = s.range(pos, i);
            if(sub.len && !sub.begins_with(' '))
                _write(' ');
            _write(sub);  // write everything up to (including) this newline
            _newl();
            pos = i + 1;
            _write(' ', indentation); // _indent() is for level, but we have explicit indentation
            _write('#');
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
    {
        csubstr sub = s.sub(pos);
        if(sub.len && !sub.begins_with(' '))
            _write(' ');
        _write(sub);
    }
}

template<class Writer>
void Emitter<Writer>::_write_comm_trailing(CommentData const* comm)
{
    _write(' ');
    if(!m_opts.comments_add_leading_space())
        _write_comm(comm->m_text, m_col);
    else
        _write_comm_leadspace(comm->m_text, m_col);
    _pend_newl();
}

template<class Writer>
void Emitter<Writer>::_write_comm_leading(CommentData const* comm)
{
    if(!m_wsonly)
    {
        _newl();
        _indent(m_ilevel);
    }
    if(!m_opts.comments_add_leading_space())
        _write_comm(comm->m_text, m_col);
    else
        _write_comm_leadspace(comm->m_text, m_col);
    _pend_newl();
}

template<class Writer>
CommentData const* Emitter<Writer>::_comm_get(id_type node, CommentType_e type, bool indent_extra)
{
    if(!m_opts.comments())
        return nullptr;
    CommState *result = &m_comm_state.top();
    #ifdef RYML_USE_ASSERT // ensure the queries are in order of CommentType
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), type >= result->latest_query, m_tree, node);
    result->latest_query = type;
    #endif
    result->comm = m_tree->comment(node, result->latest, type);
    if(result->comm)
    {
        result->latest = result->comm;
        if(indent_extra && !result->extra_indentation)
        {
            result->extra_indentation = 1;
            ++m_ilevel;
        }
    }
    return result->comm;
}

template<class Writer>
CommentData const* Emitter<Writer>::_write_comm_trailing(id_type node, CommentType_e type, bool indent_extra)
{
    CommentData const* comm = _comm_get(node, type, indent_extra);
    if(comm)
        _write_comm_trailing(comm);
    return comm;
}

template<class Writer>
CommentData const* Emitter<Writer>::_write_comm_leading(id_type node, CommentType_e type, bool indent_extra)
{
    CommentData const* comm = _comm_get(node, type, indent_extra);
    if(comm)
        _write_comm_leading(comm);
    return comm;
}
#endif // RYML_WITH_COMMENTS


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

/** @endcond */

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* _C4_YML_EMIT_DEF_HPP_ */
