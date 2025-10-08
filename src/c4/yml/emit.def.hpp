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
    m_flow = false;
    m_col = 0;
    if(type == EMIT_YAML)
        _emit_yaml(id);
    else if(type == EMIT_JSON)
        _do_visit_json(id, 0);
    else
        _RYML_ERR_BASIC_(m_tree->callbacks(), "unknown emit type");
    m_tree = nullptr;
    return this->Writer::_get(error_on_excess);
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_emit_yaml(id_type id)
{
    // save branches in the visitor by doing the initial stream/doc
    // logic here, sparing the need to check stream/val/keyval inside
    // the visitor functions
    auto dispatch = [this](id_type node, NodeType ty){
        if(ty.is_flow_sl())
            _visit_flow_sl(node, 0, 0);
        else if(ty.is_flow_ml())
            _visit_flow_ml(node, 0, 0);
        else
        {
            _visit_blck(node, 0, 0, 1);
        }
    };
    const NodeType ty = m_tree->type(id);
    if(!m_tree->is_root(id))
    {
        if(ty.is_container() && !ty.is_flow())
        {
            id_type ilevel = 0;
            if(ty.has_key())
            {
                _write(m_tree->key(id));
                _write(":\n");
                ++ilevel;
            }
            _visit_blck_container(id, ty, 0, ilevel, ilevel);
            return;
        }
    }

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
                _write("...\n");
            _write("%TAG ");
            _write(tagds.b->handle);
            _write(' ');
            _write(tagds.b->prefix);
            _newl();
        }
    };
    if(ty.is_stream())
    {
        const id_type first_child = m_tree->first_child(id);
        if(first_child != NONE)
            write_tag_directives(first_child);
        for(id_type child = first_child; child != NONE; child = m_tree->next_sibling(child))
        {
            const NodeType cty = m_tree->type(child);
            dispatch(child, cty);
            if(cty.is_doc() && cty.is_flow_sl())
                _newl();
            if(m_tree->next_sibling(child) != NONE)
                write_tag_directives(m_tree->next_sibling(child));
        }
    }
    else if(ty.is_container())
    {
        dispatch(id, ty);
    }
    else if(ty.is_doc())
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), !ty.is_container(), m_tree, id); // checked above
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_val(), m_tree, id); // so it must be a val
        _write_doc(id);
    }
    else if(ty.is_keyval())
    {
        _writek(id, 0);
        _write(": ");
        _writev(id, 0);
        if(!ty.is_flow())
            _newl();
    }
    else if(ty.is_val())
    {
        //_write("- ");
        _writev(id, 0);
        if(!ty.is_flow())
            _newl();
    }
    #ifdef RYML_WITH_COMMENTS
    else if(m_tree->comment(id, COMM_LV))
    {
        _write_comment(m_tree->comment(id, COMM_LV)->m_text, 0);
        _newl();
    }
    #endif
    else if(ty == NOTYPE)
    {
        ;
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, id, "unknown type");
    }
}


//-----------------------------------------------------------------------------

#define _rymlindent_nextline() this->_indent(ilevel + 1);

template<class Writer>
void Emitter<Writer>::_write_doc(id_type id)
{
    const NodeType ty = m_tree->type(id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, ty.is_doc(), m_tree, id);
    _RYML_ASSERT_VISIT_(m_tree->m_callbacks, !ty.has_key(), m_tree, id);

    #ifdef RYML_WITH_COMMENTS
    CommentData const* comm = m_tree->comment(id, COMM_TV);
    #endif

    if(!m_tree->is_root(id))
    {
        _RYML_ASSERT_VISIT_(m_tree->m_callbacks, m_tree->is_stream(m_tree->parent(id)), m_tree, id);
        _write("---");
        #ifdef RYML_WITH_COMMENTS
        if(comm)
            _write(' ');
        #endif
    }

    #ifdef RYML_WITH_COMMENTS
    if(comm)
    {
        _write_comment(comm->m_text, m_col);
        _newl();
    }
    #endif

    if(ty.is_container()) // this is more frequent
    {
        const bool tag = ty.has_val_tag();
        const bool anchor = ty.has_val_anchor();
        if(!tag && !anchor)
        {
            ;
        }
        else if(!tag && anchor)
        {
            if(!m_tree->is_root(id))
                _write(' ');
            _write('&');
            _write(m_tree->val_anchor(id));
        }
        else if(tag && !anchor)
        {
            if(!m_tree->is_root(id))
                _write(' ');
            _write_tag(m_tree->val_tag(id));
        }
        else // tag && anchor
        {
            if(!m_tree->is_root(id))
                _write(' ');
            _write_tag(m_tree->val_tag(id));
            _write(" &");
            _write(m_tree->val_anchor(id));
        }
        #ifdef RYML_NO_COVERAGE__TO_BE_DELETED
        if((tag || anchor) && (m_tree->has_children(id) && m_tree->is_root(id)))
            _newl();
        #endif
    }
    else if(ty.has_val())
    {
        // some plain scalars such as '...' and '---' must not
        // appear at 0-indentation
        const csubstr val = m_tree->val(id);
        const bool preceded_by_3_dashes = !m_tree->is_root(id);
        const type_bits style_marks = ty & VAL_STYLE;
        const bool is_plain = ty.is_val_plain();
        const bool is_ambiguous = (is_plain || !style_marks)
            && ((val.begins_with("...") || val.begins_with("---"))
                ||
                (val.find('\n') != npos));
        if(preceded_by_3_dashes)
        {
            if(is_plain && val.len == 0 && !ty.has_val_anchor() && !ty.has_val_tag())
            {
                _newl();
                return;
            }
            else if(val.len && is_ambiguous)
            {
                _newl();
            }
            else
            {
                _write(' ');
            }
        }
        id_type ilevel = 0u;
        if(is_ambiguous)
        {
            _rymlindent_nextline();
            ++ilevel;
        }
        _writev(id, ilevel);
        if(val.len && m_tree->is_root(id))
            _newl();
    }

    #ifndef RYML_WITH_COMMENTS
    if(!m_tree->is_root(id))
        _newl();
    #else
    {
        const CommentData *commf = m_tree->comment(id, comm, COMM_FV);
        if(commf)
        {
            if(!comm)
                _newl();
            _write_comment(commf->m_text, m_col);
            _newl();
        }
        else if(!m_tree->is_root(id))
            _newl();
    }
    #endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_blck_open_container(id_type node, NodeType ty, id_type ilevel, id_type &do_indent)
{
    C4_UNUSED(do_indent);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_container(), m_tree, node);
    bool spc = false; // write a space
    bool nl = false;  // write a newline
    if(ty.has_key())
    {
/*FIXME*/_indent(ilevel, do_indent);
        #ifdef RYML_WITH_COMMENTS
        {
            CommentData const* comm = m_tree->comment(node, COMM_LK);
            if(comm)
            {
                if(!m_tree->is_root(node))
                    if(m_tree->first_sibling(node) != node)
                        _newl();
                _indent(ilevel, do_indent);
                _write_comment(comm->m_text, ilevel);
                _newl();
                _indent(ilevel, do_indent);
            }
        }
        #endif
        _writek(node, ilevel);
        _write(':');
        spc = true;
    }
    else if(!m_tree->is_root(node))
    {
/*FIXME*/_indent(ilevel, do_indent);
        #ifndef RYML_WITH_COMMENTS
        _write('-');
        spc = true;
        #else
        {
            CommentData const* comm = m_tree->comment(node, COMM_LV);
            if(comm)
            {
                _write_comment(comm->m_text, ilevel);
                _newl();
                _indent(ilevel, do_indent);
            }
        }
        _write('-');
        {
            CommentData const* comm = m_tree->comment(node, COMM_LV2);
            if(comm)
            {
                _write(' ');
                _indent(ilevel, do_indent);
                _write_comment(comm->m_text, ilevel);
                _newl();
            }
        }
        spc = true;
        #endif
    }

    if(ty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
        nl = true;
    }

    if(ty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
        nl = true;
    }

    if(m_tree->has_children(node))
    {
        if(ty.has_key())
        {
            nl = true;
            spc = false;
            #ifdef RYML_WITH_COMMENTS
            {
                CommentData const* comm = m_tree->comment(node, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                }
            }
            #endif
        }
        else // ! has key(node)
        {
            #ifdef RYML_WITH_COMMENTS
            {
                CommentData const* comm = m_tree->comment(node, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                    nl = true;
                    spc = false;
                }
                else if(!m_tree->is_root(node) && !nl)
                {
                    spc = true;
                }
            }
            #else
            if(!m_tree->is_root(node) && !nl)
            {
                spc = true;
            }
            #endif
        }
    }
    else // ! has_children (node)
    {
        #ifdef RYML_WITH_COMMENTS
        {
            CommentData const* comm = m_tree->comment(node, COMM_TV);
            if(comm)
            {
                _write(' ');
                _write_comment(comm->m_text, m_col);
                _newl();
                this->_indent(ilevel+1);
            }
        }
        #endif
        if(ty.is_seq())
            _write(" []\n");
        else if(ty.is_map())
            _write(" {}\n");
        return;
    }

    if(spc)
        _write(' ');

    do_indent = 0;
    if(nl)
    {
        _newl();
        do_indent = 1;
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_flow_open_container(id_type node, NodeType nty, id_type ilevel)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), nty.is_map() || nty.is_seq(), m_tree, node);

    bool spc = false; // write a space

    if(nty.has_key())
    {
        #ifdef RYML_WITH_COMMENTS
        {
            CommentData const* comm = m_tree->comment(node, COMM_LK);
            if(comm)
            {
//_write("aqui 2");
                if(!m_tree->is_root(node))
                    if(m_tree->first_sibling(node) != node)
                        _newl();
                _indent(ilevel);
                _write_comment(comm->m_text, ilevel);
                _newl();
            }
        }
        #endif
        _writek(node, ilevel);
        _write(':');
        spc = true;
    }
    #ifdef RYML_WITH_COMMENTS
    else
    {
//_write("wtf 0");
        CommentData const* comm = m_tree->comment(node, COMM_LV);
        if(comm)
        {
            if(!m_tree->is_root(node))
                if(m_tree->first_sibling(node) != node)
                    _newl();
            _indent(ilevel);
            _write_comment(comm->m_text, ilevel);
            _newl();
            _indent(ilevel);
        }
//_write("wtf 1");
    }
    #endif

    if(nty.has_val_tag())
    {
        if(spc)
            _write(' ');
        _write_tag(m_tree->val_tag(node));
        spc = true;
    }

    if(nty.has_val_anchor())
    {
        if(spc)
            _write(' ');
        _write('&');
        _write(m_tree->val_anchor(node));
        spc = true;
    }

    if(spc)
        _write(' ');

    #ifdef RYML_WITH_COMMENTS
    {
        CommentData const* commtk = m_tree->comment(node, COMM_TK);
        if(commtk)
        {
            _write_comment(commtk->m_text, m_col);
            _newl();
            _indent(ilevel + 1);
        }
        if(nty.has_key())
        {
            CommentData const* comm = m_tree->comment(node, COMM_LV);
            if(!commtk)
                _newl();
            if(comm)
            {
                _write_comment(comm->m_text, m_col);
                _newl();
                _indent(ilevel);
            }
        }
    }
    #endif

    if(nty.is_map())
    {
        _write('{');
    }
    else
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), nty.is_seq(), m_tree, node);
        _write('[');
    }

    #ifdef RYML_WITH_COMMENTS
    {
        CommentData const* comm = m_tree->comment(node, COMM_TV);
        if(comm)
        {
            _write(' ');
            _write_comment(comm->m_text, m_col);
            _newl();
            _indent(ilevel);
        }
    }
    #endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_flow_sl(id_type node, id_type depth, id_type ilevel)
{
    const bool prev_flow = m_flow;
    m_flow = true;
    const NodeType ty = m_tree->type(node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !ty.is_stream(), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.is_container() || ty.is_doc() _RYML_WITH_COMMENTS( || m_tree->comment(node)), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");

    if(ty.is_doc())
    {
        _write_doc(node);
    }
    else if(ty.is_container())
    {
        _flow_open_container(node, ty, ilevel);
    }

//_write("asdasdasd 1");
    ++ilevel;
    for(id_type first = m_tree->first_child(node), child = first; child != NONE; )
    {
        const NodeType chty = m_tree->type(child);
        if(chty.is_keyval())
        {
            #ifndef RYML_WITH_COMMENTS
            _writek(child, ilevel);
            _write(": ");
            _writev(child, ilevel);
            #else
            {
                CommentData const *comm = m_tree->comment(child, COMM_LK);
                if(comm)
                {
                    if(child != first)
                        _newl();
                    _indent(ilevel);
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            _writek(child, ilevel);
            _write(": ");
            {
                CommentData const *commtk = m_tree->comment(child, COMM_TK);
                if(commtk)
                {
                    _write_comment(commtk->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
                CommentData const *commlv = m_tree->comment(child, COMM_LV);
                if(commlv)
                {
                    if(!commtk)
                        _newl();
                    _indent(ilevel);
                    _write_comment(commlv->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            _writev(child, ilevel);
            {
                CommentData const *comm = m_tree->comment(child, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            {
                CommentData const *comm = m_tree->comment(child, COMM_FV);
                if(comm)
                {
                    _newl();
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            #endif
        }
        else if(chty.is_val())
        {
            #ifndef RYML_WITH_COMMENTS
            _writev(child, ilevel);
            #else
            {
                CommentData const *comm = m_tree->comment(child, COMM_LV);
                if(comm)
                {
                    if(child != first)
                    {
                        _newl();
                        _indent(ilevel);
                    }
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            _writev(child, ilevel);
            {
                CommentData const *comm = m_tree->comment(child, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            {
                CommentData const *comm = m_tree->comment(child, COMM_FV);
                if(comm)
                {
                    _newl();
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    _indent(ilevel);
                }
            }
            #endif
        }
        else if(chty.is_container())
        {
            // with single-line flow, we can never go back to block
            _visit_flow_sl(child, depth + 1, ilevel + 1);
        }
        #ifdef RYML_WITH_COMMENTS
        else if(chty == NOTYPE)
        {
            CommentData const *commlk = m_tree->comment(child, COMM_LK);
            if(commlk)
            {
                if(child != first)
                {
                    _newl();
                }
                _indent(ilevel);
                _write_comment(commlk->m_text, m_col);
                _newl();
                _indent(ilevel);
            }
            CommentData const *commlv = m_tree->comment(child, COMM_LV);
            if(commlv)
            {
                if(child != first)
                {
                    if(!commlk)
                    {
                        _newl();
                    }
                }
                _indent(ilevel);
                _write_comment(commlv->m_text, m_col);
                _newl();
                _indent(ilevel);
            }
        }
        else if(!chty.is_container())
        {
            CommentData const* comm = m_tree->comment(child, COMM_TV);
            if(comm)
            {
                _write(' ');
                _write_comment(comm->m_text, m_col);
                _newl();
                _indent(ilevel);
            }
        }
        CommentData const *commc = m_tree->comment(child, COMM_TC);
        #endif
        child = m_tree->next_sibling(child);
        if(child == NONE)
            break;
        if(m_tree->type(child) != NOTYPE  _RYML_WITH_COMMENTS(|| commc))
            _write(',');
        #ifdef RYML_WITH_COMMENTS
        if(commc)
        {
            _write(' ');
            _write_comment(commc->m_text, m_col);
            _newl();
            _indent(ilevel);
        }
        #endif
    }
    --ilevel;

    if(ty.is_map())
    {
        _write('}');
    }
    else if(ty.is_seq())
    {
        _write(']');
    }

    #ifdef RYML_WITH_COMMENTS
    {
        CommentData const* comm = m_tree->comment(node, COMM_FV);
        if(comm)
        {
            _write(' ');
            _write_comment(comm->m_text, m_col);
            _newl();
        }
        CommentData const* comm2 = m_tree->comment(node, COMM_FV2);
        if(comm2)
        {
            if(!comm)
                _newl();
            _write_comment(comm2->m_text, m_col);
            _newl();
        }
    }
    #endif

    m_flow = prev_flow;
}


//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702) // unreachable error, triggered by flow_ml not implemented

template<class Writer>
void Emitter<Writer>::_visit_flow_ml(id_type id, id_type depth, id_type ilevel)
{
    C4_UNUSED(id);
    C4_UNUSED(depth);
    C4_UNUSED(ilevel);
    c4::yml::err_basic(RYML_LOC_HERE(), "not implemented");
    #ifdef THIS_IS_A_WORK_IN_PROGRESS
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_CB_ERR(m_tree->callbacks(), "max depth exceeded");
    const bool prev_flow = m_flow;
    m_flow = true;
    // do it...
    m_flow = prev_flow;
    #endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_seq(id_type node, id_type depth, id_type level, bool do_indent)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_seq(node), m_tree, node);
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        const NodeType cty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), !cty.has_key(), m_tree, node);
        if(cty.is_val())
        {
            #ifndef RYML_WITH_COMMENTS
            _indent(level, do_indent);
            _write("- ");
            _writev(child, level);
            _newl();
            #else
            {
                CommentData const* comm = m_tree->comment(child, COMM_LV);
                if(comm)
                {
                    _indent(level, do_indent);
                    _write_comment(comm->m_text, m_col);
                    _newl();
                }
            }
            _indent(level, do_indent);
            _write("- ");
            id_type thislevel = level;
            {
                CommentData const* comm = m_tree->comment(child, COMM_LV2);
                if(comm)
                {
                    _write_comment(comm->m_text, m_col);
                    _newl();
                    ++thislevel;
                    _indent(thislevel, do_indent);
                }
            }
            _writev(child, thislevel);
            {
                CommentData const* comm = m_tree->comment(child, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                }
            }
            _newl();
            {
                CommentData const* comm = m_tree->comment(child, COMM_FV);
                if(comm)
                {
                    _indent(thislevel, do_indent);
                    _write_comment(comm->m_text, m_col);
                    _newl();
                }
            }
            #endif
        }
        else if(cty.is_container())
        {
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), cty.is_container(), m_tree, node);
            if(cty.is_flow_sl())
            {
                _indent(level, do_indent);
                _write("- ");
                _visit_flow_sl(child, depth+1, 0u);
                _newl();
            }
            else if(cty.is_flow_ml())
            {
                _indent(level, do_indent);
                _write("- ");
                _visit_flow_ml(child, depth+1, 0u);
                _newl();
            }
            else
            {
                _visit_blck(child, depth+1, level, do_indent); // same indentation level
            }
        }
        #ifdef RYML_WITH_COMMENTS
        else
        {
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), cty == NOTYPE, m_tree, child);
            CommentData const* comm = m_tree->comment(child, COMM_LV);
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), comm != nullptr, m_tree, child);
            if(comm)
            {
                _indent(level);
                _write_comment(comm->m_text, level);
                _newl();
            }
        }
        #endif
        do_indent = true;
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_map(id_type node, id_type depth, id_type level, bool do_indent)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_map(node), m_tree, node);
    for(id_type child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        const NodeType ty = m_tree->type(child);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty.has_key() || ty == NOTYPE, m_tree, node);
        if(ty.is_keyval())
        {
//_write("~wtf 2~");
            _indent(level, do_indent);
//_write("~wtf 3~");
            #ifndef RYML_WITH_COMMENTS
            _writek(child, level);
            _write(": ");
            _writev(child, level);
            _newl();
            #else
            {
                CommentData const* comm = m_tree->comment(child, COMM_LK);
                if(comm)
                {
                    _indent(level, do_indent);
                    _write_comment(comm->m_text, m_col);
                    _newl();
                }
            }
            _writek(child, level);
            _write(": ");
            id_type val_level = level;
            {
                CommentData const* commtk = m_tree->comment(child, COMM_TK);
                if(commtk)
                {
                    _write_comment(commtk->m_text, m_col);
                    _newl();
                    ++val_level;
                    _indent(val_level);
                }
                CommentData const* commlv = m_tree->comment(child, COMM_LV);
                if(commlv)
                {
                    if(!commtk)
                    {
                        _newl();
                        ++val_level;
                        _indent(val_level);
                    }
                    _write_comment(commlv->m_text, m_col);
                    _newl();
                    _indent(val_level);
                }
            }
            _writev(child, val_level);
            {
                CommentData const* comm = m_tree->comment(child, COMM_TV);
                if(comm)
                {
                    _write(' ');
                    _write_comment(comm->m_text, m_col);
                }
            }
            _newl();
            {
                CommentData const* comm = m_tree->comment(child, COMM_FV);
                if(comm)
                {
                    _indent(level + 1);
                    _write_comment(comm->m_text, level);
                    _newl();
                }
            }
            #endif
        }
        else if(ty.is_container())
        {
            if(ty.is_flow_sl())
            {
                _indent(level, do_indent);
                _visit_flow_sl(child, depth+1, 0u);
//_write("aqui 1");
                _newl();
            }
            else if(ty.is_flow_ml())
            {
                _indent(level, do_indent);
                _visit_flow_ml(child, depth+1, 0u);
                _newl();
            }
            else
            {
                _visit_blck(child, depth+1, level, do_indent); // same level!
            }
        }
        #ifdef RYML_WITH_COMMENTS
        else
        {
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), ty == NOTYPE, m_tree, child);
            CommentData const* comm = m_tree->comment(child, COMM_LK);
            _RYML_ASSERT_VISIT_(m_tree->callbacks(), comm != nullptr, m_tree, child);
            if(comm)
            {
                _indent(level);
                _write_comment(comm->m_text, level);
                _newl();
            }
        }
        #endif
        // keyval vs container
        do_indent = true;
    } // for children
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck_container(id_type node, NodeType ty, id_type depth, id_type next_level, id_type do_indent)
{
    if(ty.is_seq())
    {
        _visit_blck_seq(node, depth, next_level, do_indent);
    }
    else
    {
        _visit_blck_map(node, depth, next_level, do_indent);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_visit_blck(id_type node, id_type depth, id_type ilevel, id_type do_indent)
{
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), !m_tree->is_stream(node), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_container(node) || m_tree->is_doc(node) _RYML_WITH_COMMENTS( || m_tree->comment(node, COMM_ANY)), m_tree, node);
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if(C4_UNLIKELY(depth > m_opts.max_depth()))
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, node, "max depth exceeded");

    const NodeType ty = m_tree->type(node);
    if(ty.is_doc())
    {
        _write_doc(node);
        if(!m_tree->has_children(node))
            return;
    }
    else if(ty.is_container())
    {
        _blck_open_container(node, ty, ilevel, do_indent);
    }
    else if(ty == NOTYPE)
        return;

    id_type next_level = ilevel + 1;
    if(m_tree->is_root(node) || ty.is_doc())
        next_level = ilevel; // do not indent at top level

    _visit_blck_container(node, ty, depth, next_level, do_indent);
}

C4_SUPPRESS_WARNING_MSVC_POP



//-----------------------------------------------------------------------------

template<class Writer>
void Emitter<Writer>::_do_visit_json(id_type id, id_type depth)
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
        _do_visit_json(ich, depth+1);
    }

    if(m_tree->is_seq(id))
        _write(']');
    else if(m_tree->is_map(id))
        _write('}');
}

template<class Writer>
void Emitter<Writer>::_write(NodeScalar const& C4_RESTRICT sc, NodeType flags, id_type ilevel)
{
    if( ! sc.tag.empty())
    {
        _write_tag(sc.tag);
        _write(' ');
    }
    if(flags.has_anchor())
    {
        _RYML_ASSERT_VISIT_(m_tree->callbacks(), flags.is_ref() != flags.has_anchor(), m_tree, NONE);
        _RYML_ASSERT_VISIT_(m_tree->callbacks(),  ! sc.anchor.empty(), m_tree, NONE);
        _write('&');
        _write(sc.anchor);
        _write(' ');
    }
    else if(flags.is_ref())
    {
        if(sc.anchor != "<<")
            _write('*');
        _write(sc.anchor);
        if(flags.is_key_ref())
            _write(' ');
        return;
    }

    // ensure the style flags only have one of KEY or VAL
    _RYML_ASSERT_VISIT_(m_tree->callbacks(), ((flags & SCALAR_STYLE) == 0) || (((flags & KEY_STYLE) == 0) != ((flags & VAL_STYLE) == 0)), m_tree, NONE);
    type_bits style_marks = flags & SCALAR_STYLE;
    if(!style_marks)
        style_marks = scalar_style_choose(sc.scalar);
    if(style_marks & (KEY_LITERAL|VAL_LITERAL))
    {
        _write_scalar_literal(sc.scalar, ilevel, flags.has_key());
    }
    else if(style_marks & (KEY_FOLDED|VAL_FOLDED))
    {
        _write_scalar_folded(sc.scalar, ilevel, flags.has_key());
    }
    else if(style_marks & (KEY_SQUO|VAL_SQUO))
    {
        _write_scalar_squo(sc.scalar, ilevel);
    }
    else if(style_marks & (KEY_DQUO|VAL_DQUO))
    {
        _write_scalar_dquo(sc.scalar, ilevel);
    }
    else if(style_marks & (KEY_PLAIN|VAL_PLAIN))
    {
        if(C4_LIKELY(!(sc.scalar.begins_with(": ") || sc.scalar.begins_with(":\t"))))
            _write_scalar_plain(sc.scalar, ilevel);
        else
            _write_scalar_squo(sc.scalar, ilevel);
    }
    else
    {
        _RYML_ERR_VISIT_(m_tree->callbacks(), m_tree, NONE, "not implemented");
    }
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
        _rymlindent_nextline();
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
        _rymlindent_nextline();
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
            _rymlindent_nextline()
            _write(since_pos);
            pos = i+1; // already written
        }
        if(pos < trimmed.len)
        {
            _rymlindent_nextline()
            _write(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        _newl();
    if(explicit_key)
    {
        _newl();
        this->_indent(ilevel);
    }
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
                    _rymlindent_nextline()
                    _write(s.range(pos, i));
                    i = _write_escaped_newlines(s, i);
                    pos = i+1;
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
                _rymlindent_nextline()
                _write(s.range(pos, i));
                if(pos > 0 || !s.begins_with_any(" \t"))
                    i = _write_indented_block(s, i, ilevel);
                pos = i;
            }
        }
        if(pos < trimmed.len)
        {
            _rymlindent_nextline()
            _write(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        _newl();
    if(explicit_key)
    {
        _newl();
        this->_indent(ilevel);
    }
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
                _rymlindent_nextline()
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
    // write missing characters at the end of the string
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
                _rymlindent_nextline() // indent the next line
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
    // write missing characters at the end of the string
    if(pos < s.len)
        _write(s.sub(pos));
    _write('"');
}

template<class Writer>
void Emitter<Writer>::_write_scalar_plain(csubstr s, id_type ilevel)
{
    if(C4_UNLIKELY(ilevel == 0 && (s.begins_with("...") || s.begins_with("---"))))
    {
        _rymlindent_nextline()     // indent the next line
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
                _rymlindent_nextline()     // indent the next line
        }
    }
    // write missing characters at the end of the string
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
    // write missing characters at the end of the string
    if(pos < s.len)
        _write(s.sub(pos));
}
#endif // RYML_WITH_COMMENTS


#undef _rymlindent_nextline

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
