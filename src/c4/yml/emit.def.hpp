#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "c4/yml/emit.hpp"
#endif

/** @file emit.def.hpp Definitions for emit functions. */
#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#include "c4/yml/detail/dbgprint.hpp"
#endif

#if 1 // debug scaffold
#define _c4dbgemt(tk) _write("[" #tk "]")
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
    m_count = 0;
    if(type == EMIT_YAML)
        _emit_yaml(id);
    else if(type == EMIT_JSON)
        _visit_json(id, 0);
    else
        _RYML_ERR_BASIC_(m_tree->callbacks(), "unknown emit type");
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
// kickstart branches in the recursive descending code. This makes the
// recusive descending code a lot simpler.
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
        _visit_doc_container(id);
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

    // terminate non-flow with a newline
    if(ty != NOTYPE && !ty.is_flow())
    {
        _newl();
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
                _write("...");
                _newl();
            }
            _write("%TAG ");
            _write(tagds.b->handle);
            _write(' ');
            _write(tagds.b->prefix);
            _newl();
        }
    };
    const id_type first_child = m_tree->first_child(id);
    if(first_child != NONE)
        write_tag_directives(first_child);
    ++m_depth;
    for(id_type child = first_child; child != NONE; child = m_tree->next_sibling(child))
    {
        if(child != first_child)
            _newl();
        _top_open_entry(child);
        _visit_doc(child);
        _top_close_entry(child);
        if(m_tree->next_sibling(child) != NONE)
            write_tag_directives(m_tree->next_sibling(child));
    }
    --m_depth;
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_doc_container(id_type id)
{
    const NodeType ty = m_tree->type(id);
    const bool write_sep = ty.is_doc() && !m_tree->is_root(id);
    if(write_sep)
        _newl();
    if(ty.is_flow_sl())
        _visit_flow_sl(id);
    else if(ty.is_flow_ml())
        _visit_flow_ml(id);
    else
        _visit_blck(id);
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
    const bool write_sep = ty.is_doc() && !m_tree->is_root(id);
    if(write_sep)
    {
        if(!val.empty() || !ty.is_val_plain())
        {
            _newl();
        }
    }
    if(is_ambiguous)
    {
        ++m_ilevel;
        _indent(m_ilevel);
    }
    _blck_write_scalar_val(id);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_doc(id_type id)
{
    const NodeType ty = m_tree->type(id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, ty.is_doc(), m_tree, id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, !ty.has_key(), m_tree, id);

    m_count = 0; // ensure no extra line at the beginning of block docs

    if(ty.is_container()) // this is more frequent
    {
        _visit_doc_container(id);
    }
    else if(ty.is_val())
    {
#ifdef RYML_WITH_COMMENTS
        CommentData const* comm = m_tree->comment(id, COMM_LV);
        if(comm)
        {
            _write_comment(comm->m_text, m_col);
            _newl();
        }
#endif
        _visit_doc_val(id);
#ifdef RYML_WITH_COMMENTS
        comm = m_tree->comment(id, comm, COMM_TV);
        if(comm)
        {
            _write(' ');
            _write_comment(comm->m_text, m_col);
            _newl();
        }
        const CommentData *commf = m_tree->comment(id, comm, COMM_FV);
        if(commf)
        {
            if(!comm)
                _newl();
            _write_comment(commf->m_text, m_col);
            _newl();
        }
#endif
    }
}


//-----------------------------------------------------------------------------

// to be called only at top level
template<class Writer>
void Emitter<Writer>::_top_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _indent(m_ilevel);
    bool spc = false;
    bool write_sep = ty.is_doc() && !m_tree->is_root(node);
#ifndef RYML_WITH_COMMENTS
    if(write_sep)
    {
        _write("---");
        spc = true;
    }
    if(ty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
#else // comments
    bool newl = false;
    if(write_sep)
    {
        _write("---");
        spc = true;
        CommentData const *comm = m_tree->comment(node, COMM_TT);
        if(comm)
        {
            _write(" #");
            _write_comment(comm->m_text, m_col);
            spc = false;
            newl = true;
        }
    }
    CommentData const* comm = m_tree->comment(node, COMM_LV);
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        newl = true;
        spc = false;
    }
    if(ty.has_val_tag())
    {
        if(newl)
            _newl();
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(newl)
            _newl();
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_top_close_entry(id_type node)
{
    (void)node;
#ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(node, COMM_TV);
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
    }
    comm = m_tree->comment(node, comm, COMM_FV);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
    }
    comm = m_tree->comment(node, comm, COMM_FV2);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_sl_write_comma(id_type node, id_type first_sibling)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), first_sibling == m_tree->first_sibling(node), m_tree, node);
    if(node != first_sibling)
    {
        _write(',');
#ifdef RYML_WITH_COMMENTS
        CommentData const *comm = m_tree->comment(node, COMM_TT);
        if(comm)
        {
            _write(" #");
            _write_comment(comm->m_text, m_col);
            _newl();
            _indent(m_ilevel);
        }
#endif
    }
}

template<class Writer>
void Emitter<Writer>::_flow_ml_write_comma(id_type node, id_type first_sibling)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), first_sibling == m_tree->first_sibling(node), m_tree, node);
    if(node != first_sibling)
    {
        _write(',');
#ifdef RYML_WITH_COMMENTS
        CommentData const *comm = m_tree->comment(node, COMM_TT);
        if(comm)
        {
            _write(" #");
            _write_comment(comm->m_text, m_col);
        }
#endif
        _newl();
        _indent(m_ilevel);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    if(m_count++)
    {
        // ensure that nested block seqs are emitted as '- - -`,
        // ie as dashes nested in the same line
        if(_open_entry_with_newl(node))
        {
            _newl();
            _indent(m_ilevel);
        }
        else
        {
            _write(' ');
        }
    }
#ifndef RYML_WITH_COMMENTS
    _write('-');
    if(ty.has_val_tag())
    {
        _write(' ');
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write(" &");
        _write(m_tree->val_anchor(node));
    }
#else // comments
    CommentData const* comm = m_tree->comment(node, COMM_LV);
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel);
    }
    _write('-');
    comm = m_tree->comment(node, comm, COMM_LV2);
    bool spc = true;
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    if(ty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
    }
#endif
    if(ty.is_val() || ty.is_flow())
    {
        _RYML_WITH_COMMENTS(if(spc))
            _write(' ');
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_seq_close_entry(id_type node)
{
    (void)node;
#ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(node, COMM_TV);
    bool spc = true;
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_FV);
    if(comm)
    {
        if(spc)
        {
            _write(' ');
        }
        else
        {
            _newl();
            _indent(m_ilevel);
        }
        _write_comment(comm->m_text, m_col);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_FV2);
    if(comm)
    {
        if(spc)
        {
            _write(' ');
        }
        else
        {
            _newl();
            _indent(m_ilevel);
        }
        _write_comment(comm->m_text, m_col);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_map_open_entry(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->has_key(node), m_tree, node);
    NodeType ty = m_tree->type(node);
    if(m_count++)
    {
        if(_open_entry_with_newl(node))
        {
            _newl();
            _indent(m_ilevel);
        }
        else
        {
            _write(' ');
        }
    }
#ifndef RYML_WITH_COMMENTS
    bool spc = false;
    if(ty.has_key_tag())
    {
        _write_tag(m_tree->key_tag(node));
        spc = true;
    }
    if(ty.has_key_anchor())
    {
        if(spc)
            _write(' ');
        _write("&");
        _write(m_tree->key_anchor(node));
    }
    /*no else!*/
    if(spc)
        _write(' ');
    if(ty.is_key_ref())
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->key(node).begins_with('*') || m_tree->key(node) == "<<", m_tree, node);
        _write(m_tree->key(node)); // cf asserted assumptions
        _write(' ');
    }
    else
    {
        _blck_write_scalar_key(node);
    }
    if(ty & (KEY_LITERAL|KEY_FOLDED))
        _indent(m_ilevel);
    _write(':');
    if(ty.has_val_tag())
    {
        _write(' ');
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write(" &");
        _write(m_tree->val_anchor(node));
    }
    else if(ty.is_val_ref())
    {
        _write(" *");
        return;
    }
#else // with comments
    CommentData const* comm = m_tree->comment(node, COMM_LK);
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel);
    }
    bool spc = false;
    if(ty.has_key_tag())
    {
        _write_tag(m_tree->key_tag(node));
        spc = true;
    }
    if(ty.has_key_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->key_anchor(node));
    }
    /*no else!*/
    if(spc)
        _write(' ');
    if(ty.is_key_ref())
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->key(node).begins_with('*') || m_tree->key(node) == "<<", m_tree, node);
        _write(m_tree->key(node)); // cf asserted assumptions
        _write(' ');
    }
    else
    {
        _blck_write_scalar_key(node);
    }
    if(ty & (KEY_LITERAL|KEY_FOLDED))
        _indent(m_ilevel);
    _write(':');
    spc = true;
    comm = m_tree->comment(node, comm, COMM_TK);
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_LV);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_LV2);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    if(ty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
    else if(ty.is_val_ref())
    {
        if(spc)
            _write(' ');
        _write('*');
        return;
    }
#endif // with comments
    if(ty.has_val() || ty.is_flow())
    {
        if(spc)
            _write(' ');
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_map_close_entry(id_type node)
{
    (void)node;
#ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(node, COMM_TV);
    bool has_comments = false;
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        has_comments = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV);
    if(comm)
    {
        if(has_comments)
            _newl();
        _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
        has_comments = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV2);
    if(comm)
    {
        if(has_comments)
            _newl();
        _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_seq_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    bool spc = false;
#ifndef RYML_WITH_COMMENTS
    if(ty.has_val_tag())
    {
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
    else if(ty.is_val_ref())
    {
        if(spc)
            _write(' ');
        _write('*');
        return;
    }
#else // comments
    CommentData const* comm = m_tree->comment(node, COMM_LV);
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel);
    }
    comm = m_tree->comment(node, comm, COMM_LV2);
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
    }
    if(ty.has_val_tag())
    {
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
    else if(ty.is_val_ref())
    {
        if(spc)
            _write(' ');
        _write('*');
        return;
    }
#endif
    if(ty.is_val() || ty.is_flow())
    {
        if(spc)
            _write(' ');
    }
    else
    {
        _newl();
        _indent(m_ilevel + 1);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_seq_close_entry(id_type node)
{
    (void)node;
#ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(node, COMM_TV);
    bool prev_comm = false;
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        prev_comm = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV);
    if(comm)
    {
        if(prev_comm)
            _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
        _newl();
        prev_comm = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV2);
    if(comm)
    {
        if(prev_comm)
            _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
        _newl();
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_map_open_entry(id_type node)
{
    NodeType ty = m_tree->type(node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key(), m_tree, node);
#ifndef RYML_WITH_COMMENTS
    bool spc = false;
    if(ty.has_key_tag())
    {
        _write_tag(m_tree->key_tag(node));
        spc = true;
    }
    if(ty.has_key_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->key_anchor(node));
    }
    /*no else!*/
    if(ty.is_key_ref())
    {
        if(spc)
            _write(' ');
        csubstr ref = m_tree->key_ref(node);
        if(ref != "<<")
            _write('*');
        _write(ref);
    }
    else
    {
        if(spc)
            _write(' ');
        _blck_write_scalar_key(node);
    }
    _write(':');
    if(ty.has_val_tag())
    {
        _write(' ');
        _write_tag(m_tree->val_tag(node));
    }
    if(ty.has_val_anchor())
    {
        _write(" &");
        _write(m_tree->val_anchor(node));
    }
    else if(ty.is_val_ref())
    {
        _write(" *");
        return;
    }
#else // with comments
    CommentData const* comm = m_tree->comment(node, COMM_LK);
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel);
    }
    {
        bool spc = false;
        if(ty.has_key_tag())
        {
            _write_tag(m_tree->key_tag(node));
            spc = true;
        }
        if(ty.has_key_anchor())
        {
            if(spc)
                _write(' ');
            _write("&");
            _write(m_tree->key_anchor(node));
        }
        /*no else!*/
        if(ty.is_key_ref())
        {
            if(spc)
                _write(' ');
            csubstr ref = m_tree->key_ref(node);
            if(ref != "<<")
                _write('*');
            _write(ref);
        }
        else
        {
            if(spc)
                _write(' ');
            _blck_write_scalar_key(node);
        }
    }
    _write(':');
    bool spc = true;
    comm = m_tree->comment(node, comm, COMM_TK);
    if(comm)
    {
        _write(' ');
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_LV);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    comm = m_tree->comment(node, comm, COMM_LV2);
    if(comm)
    {
        _newl();
        _indent(m_ilevel + 1);
        _write_comment(comm->m_text, m_col);
        _newl();
        _indent(m_ilevel + 1);
        spc = false;
    }
    if(ty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }
    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }
    else if(ty.is_val_ref())
    {
        if(spc)
            _write(' ');
        _write('*');
        return;
    }
#endif // with comments
    _write(' ');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_map_close_entry(id_type node)
{
    (void)node;
#ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(node, COMM_TV);
    bool prev_comm = false;
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
        prev_comm = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV);
    if(comm)
    {
        if(prev_comm)
            _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
        _newl();
        prev_comm = true;
    }
    comm = m_tree->comment(node, comm, COMM_FV2);
    if(comm)
    {
        if(prev_comm)
            _indent(m_ilevel);
        _write_comment(comm->m_text, m_col);
        _newl();
    }
#endif
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
            if(!ty.is_val_ref())
                _blck_write_scalar_val(child);
            else
                _write(m_tree->val(child));
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            if(ty.is_flow_sl())
                _visit_flow_sl(child);
            else if(ty.is_flow_ml())
                _visit_flow_ml(child);
            else
                _visit_blck(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_seq_close_entry(child);
    }
    if(empty)
        _write(" []");
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
            _blck_write_scalar_val(child);
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            if(ty.is_flow_sl())
                _visit_flow_sl(child);
            else if(ty.is_flow_ml())
                _visit_flow_ml(child);
            else
                _visit_blck(child);
            --m_depth;
            --m_ilevel;
        }
        _blck_map_close_entry(child);
    }
    if(empty)
        _write(" {}");
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
            _flow_write_scalar_val(child);
        }
        else if(ty.is_container())
        {
            ++m_depth;
            if(ty.is_flow_ml())
                _visit_flow_ml(child);
            else
                _visit_flow_sl(child);
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
            _flow_write_scalar_val(child);
        }
        else if(ty.is_container())
        {
            ++m_depth;
            if(ty.is_flow_ml())
                _visit_flow_ml(child);
            else
                _visit_flow_sl(child);
            --m_depth;
        }
        _flow_map_close_entry(child);
    }
    _write('}');
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node) _RYML_WITH_COMMENTS( || m_tree->comment(node, COMM_ANY)), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(m_depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    const NodeType ty = m_tree->type(node);
    if(ty.is_seq())
    {
        _visit_blck_seq(node);
    }
    else if(ty.is_map())
    {
        _visit_blck_map(node);
    }
    else if(ty.has_val())
    {
        _blck_write_scalar_val(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl(id_type node)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node) _RYML_WITH_COMMENTS( || m_tree->comment(node, COMM_ANY)), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(m_depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");
    const NodeType ty = m_tree->type(node);
    if(ty.is_seq())
    {
        _visit_flow_sl_seq(node);
    }
    else if(ty.is_map())
    {
        _visit_flow_sl_map(node);
    }
    else if(ty.has_val())
    {
        _flow_write_scalar_val(node);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_ml(id_type id)
{
    C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702) // unreachable error, triggered by flow_ml not implemented
    C4_UNUSED(id);
    c4::yml::err_basic(RYML_LOC_HERE(), "not implemented");
    #ifdef THIS_IS_A_WORK_IN_PROGRESS
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_CB_ERR(m_tree->callbacks(), "max depth exceeded");
    const bool prev_flow = m_flow;
    m_flow = true;
    // do it...
    m_flow = prev_flow;
    #endif
    C4_SUPPRESS_WARNING_MSVC_POP
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_write_scalar_key(id_type id)
{
    enum {
        _block_styles = KEY_LITERAL|KEY_FOLDED,
        _flow_styles = KEY_STYLE & ~_block_styles,
    };
    csubstr str = m_tree->key(id);
    NodeType ty = m_tree->type(id) & _flow_styles;
    if(!(ty & KEY_STYLE))
        ty |= scalar_style_choose(str);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !(ty & _block_styles), m_tree, id);
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
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "not implemented");
    }
}

template<class Writer>
void Emitter<Writer>::_flow_write_scalar_val(id_type id)
{
    enum {
        _block_styles = VAL_LITERAL|VAL_FOLDED,
        _flow_styles = VAL_STYLE & ~_block_styles,
    };
    csubstr str = m_tree->val(id);
    NodeType ty = m_tree->type(id) & _flow_styles;
    if(!(ty & VAL_STYLE))
        ty |= scalar_style_choose(str);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !(ty & _block_styles), m_tree, id);
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
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "not implemented");
    }
}

template<class Writer>
void Emitter<Writer>::_blck_write_scalar_key(id_type id)
{
    csubstr key = m_tree->key(id);
    NodeType ty = m_tree->type(id) & _keysc;
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
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "not implemented");
    }
}

template<class Writer>
void Emitter<Writer>::_blck_write_scalar_val(id_type id)
{
    csubstr str = m_tree->val(id);
    NodeType ty = m_tree->type(id) & _valsc;
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
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "not implemented");
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

#ifdef RYML_WITH_COMMENTS
template<class Writer>
void Emitter<Writer>::_write_comment(csubstr s, id_type indentation)
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
        _write(s.sub(pos));
}
#endif // RYML_WITH_COMMENTS


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_json(id_type id, id_type depth)
{
    _RYML_CHECK_VISIT_(m_tree->callbacks(), !m_tree->is_stream(id), m_tree, id); // JSON does not have streams
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "max depth exceeded");
    if(m_tree->is_keyval(id))
    {
        _writek_json(id);
        _write(": ");
        _writev_json(id);
    }
    else if(m_tree->is_val(id))
    {
        _writev_json(id);
    }
    else if(m_tree->is_container(id))
    {
        if(m_tree->has_key(id))
        {
            _writek_json(id);
            _write(": ");
        }
        if(m_tree->is_seq(id))
            _write('[');
        else if(m_tree->is_map(id))
            _write('{');
    }  // container

    for(id_type ich = m_tree->first_child(id); ich != NONE; ich = m_tree->next_sibling(ich))
    {
        if(ich != m_tree->first_child(id))
            _write(',');
        _visit_json(ich, depth+1);
    }

    if(m_tree->is_seq(id))
        _write(']');
    else if(m_tree->is_map(id))
        _write('}');
}

template<class Writer>
void Emitter<Writer>::_write_json(NodeScalar const& C4_RESTRICT sc, NodeType flags)
{
    if(flags & (KEYTAG|VALTAG))
        if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_TAG)
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "JSON does not have tags");
    if(C4_UNLIKELY(flags.has_anchor()))
        if(m_opts.json_error_flags() & EmitOptions::JSON_ERR_ON_ANCHOR)
            _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "JSON does not have anchors");
    if(sc.scalar.len)
    {
        // use double quoted style...
        // if it is a key (mandatory in JSON)
        // if the style is marked quoted
        bool dquoted = ((flags & (KEY|VALQUO))
                        || (scalar_style_json_choose(sc.scalar) & SCALAR_DQUO)); // choose the style
        if(dquoted)
            _write_scalar_json_dquo(sc.scalar);
        else
            _write(sc.scalar);
    }
    else
    {
        if(sc.scalar.str || (flags & (KEY|VALQUO|KEYTAG|VALTAG)))
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
