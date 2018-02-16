#include "./parse.hpp"
#include "./detail/parser_dbg.hpp"

#include <ctype.h>
#include <stdarg.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
Parser::Parser()
    :
    m_file(),
    m_buf(),
    m_root_id(NONE),
    m_tree(),
    m_stack(),
    m_state(),
    m_key_tag(),
    m_val_tag(),
    m_anchor(),
    m_num_anchors(0),
    m_num_references()
{
    m_stack.reserve(16);
    m_stack.push({});
    m_state = &m_stack.top();
}

//-----------------------------------------------------------------------------
void Parser::_reset()
{
    while(m_stack.size() > 1)
    {
        m_stack.pop();
    }
    C4_ASSERT(m_stack.size() == 1);
    m_stack.clear();
    m_stack.push({});
    m_state = &m_stack.top();
    m_state->reset(m_file.str, m_root_id);

    m_key_tag.clear(),
    m_val_tag.clear(),
    m_anchor.clear(),
    m_num_anchors = (0),
    m_num_references = (0);
}

//-----------------------------------------------------------------------------
bool Parser::_finished_file() const
{
    bool ret = m_state->pos.offset >= m_buf.len;
    if(ret)
    {
        _c4dbgp("finished file!!!");
    }
    return ret;
}

//-----------------------------------------------------------------------------
bool Parser::_finished_line() const
{
    bool ret = m_state->line_contents.rem.empty();
    return ret;
}

//-----------------------------------------------------------------------------
void Parser::parse(cspan const& file, span buf, NodeRef *root)
{
    m_file = file;
    m_buf = buf;
    m_root_id = root->id();
    m_tree = root->tree();

    _reset();

    while( ! _finished_file())
    {
        _scan_line();
        while( ! _finished_line())
        {
            _handle_line();
        }
        if(_finished_file()) break; // it may have finished because of multiline blocks
        _next_line();
    }

    _handle_finished_file();
}

//-----------------------------------------------------------------------------
void Parser::_handle_finished_file()
{
    C4_ASSERT( ! m_stack.empty());

    if(has_any(SSCL))
    {
        if(m_tree->type(m_state->node_id) == NOTYPE)
        {
            m_tree->to_seq(m_state->node_id);
        }
        else if(m_tree->is_doc(m_state->node_id))
        {
            m_tree->to_doc(m_state->node_id, SEQ);
        }
        else if(m_tree->is_seq(m_state->node_id))
        {
            ;
        }
        else
        {
            _c4err("internal error");
        }
        _append_val(_consume_scalar());
    }

    _c4dbgp("emptying stack...");
    while(m_stack.size() > 1)
    {
        C4_ASSERT( ! has_any(SSCL, &m_stack.top()));
        _pop_level();
    }

    m_state = &m_stack.top();
}

//-----------------------------------------------------------------------------
void Parser::_handle_line()
{
    _c4dbgq("\n-----------");
    _c4dbgt("handling line %zd", m_state->pos.line);

    C4_ASSERT( ! m_state->line_contents.rem.empty());

    if(has_any(RSEQ))
    {
        if(has_any(EXPL))
        {
            if(_handle_seq_expl())
            {
                return;
            }
        }
        else
        {
            if(_handle_seq_impl())
            {
                return;
            }
        }
    }
    else if(has_any(RMAP))
    {
        if(has_any(EXPL))
        {
            if(_handle_map_expl())
            {
                return;
            }
        }
        else
        {
            if(_handle_map_impl())
            {
                return;
            }
        }
    }
    else if(has_any(RUNK))
    {
        if(_handle_unk())
        {
            return;
        }
    }

    if(_handle_top())
    {
        return;
    }
}

//-----------------------------------------------------------------------------
bool Parser::_is_scalar_next() const
{
    cspan const& rem = m_state->line_contents.rem;
    // a scalar starts with either...
    return isalnum(rem[0]) // an alpha-numeric character
        || rem.begins_with('"') || rem.begins_with('\'') // double or single quotes
        || rem.begins_with('|') || rem.begins_with('>')  // or a block indicator
        || rem.begins_with("<<: ") || rem.begins_with('*'); // treat references as scalars
}

//-----------------------------------------------------------------------------
bool Parser::_handle_unk()
{
    _c4dbgp("handle_unk");

    cspan rem = m_state->line_contents.rem;
    const bool start_as_child = (node(m_state) == nullptr);

    C4_ASSERT(has_none(RNXT|RSEQ|RMAP));

    const bool scnext = _is_scalar_next();

    if(scnext && ! has_all(SSCL))
    {
        _c4dbgp("got a scalar");
        rem = _scan_scalar();
        // we still don't know whether it's a seq or a map
        // so just store the scalar
        _store_scalar(rem);
        return true;
    }
    else if(rem.begins_with(' '))
    {
        rem = rem.left_of(rem.first_not_of(' '));
        _c4dbgpf("skipping %zd spaces", rem.len);
        _line_progressed(rem.len);
        return true;
    }
    else if(rem.begins_with("- "))
    {
        _c4dbgpf("it's a seq (as_child=%d)", start_as_child);
        _push_level();
        _start_seq(start_as_child);
        _save_indentation();
        _line_progressed(2);
        return true;
    }
    else if(rem == '-')
    {
        _c4dbgpf("it's a seq (as_child=%d)", start_as_child);
        _push_level();
        _start_seq(start_as_child);
        _save_indentation();
        _line_progressed(1);
        return true;
    }
    else if(rem.begins_with('['))
    {
        _c4dbgpf("it's a seq, explicit (as_child=%d)", start_as_child);
        _push_level(/*explicit flow*/true);
        _start_seq(start_as_child);
        add_flags(EXPL);
        _line_progressed(1);
        return true;
    }

    else if(rem.begins_with('{'))
    {
        _c4dbgpf("it's a map, explicit (as_child=%d)", start_as_child);
        _push_level(/*explicit flow*/true);
        _start_map(start_as_child);
        addrem_flags(EXPL|RKEY, RVAL);
        _line_progressed(1);
        return true;
    }
    else if(rem.begins_with("? "))
    {
        _c4dbgpf("it's a map (as_child=%d) + this key is complex", start_as_child);
        _push_level();
        _start_map(start_as_child);
        addrem_flags(RKEY|CPLX, RVAL);
        _save_indentation();
        _line_progressed(2);
        return true;
    }
    else if(has_all(SSCL))
    {
        _c4dbgpf("there's a stored scalar: '%.*s'", _c4prsp(m_state->scalar));

        cspan saved_scalar;
        if(scnext)
        {
            saved_scalar = _scan_scalar();
            rem = m_state->line_contents.rem;
            _c4dbgpf("... and there's also a scalar next! '%.*s'", _c4prsp(saved_scalar));
        }

        if(rem.begins_with(", "))
        {
            _c4dbgpf("got a ',' -- it's a seq (as_child=%d)", start_as_child);
            _start_seq(start_as_child);
            add_flags(EXPL);
            _append_val(_consume_scalar());
            _line_progressed(2);
        }
        else if(rem.begins_with(','))
        {
            _c4dbgpf("got a ',' -- it's a seq (as_child=%d)", start_as_child);
            _start_seq(start_as_child);
            add_flags(EXPL);
            _append_val(_consume_scalar());
            _line_progressed(1);
        }
        else if(rem.begins_with(": "))
        {
            _c4dbgpf("got a ':' -- it's a map (as_child=%d)", start_as_child);
            _start_map(start_as_child); // wait for the val scalar to append the key-val pair
            _line_progressed(2);
            /*if(rem == ": ")
            {
                _c4dbgp("map key opened a new line -- starting val scope as unknown");
                _start_unk();
            }*/
        }
        else if(rem == ":")
        {
            _c4dbgpf("got a ':' -- it's a map (as_child=%d)", start_as_child);
            _start_map(start_as_child); // wait for the val scalar to append the key-val pair
            _line_progressed(1);
            //_c4dbgp("map key opened a new line -- starting val scope as unknown");
            //_start_unk();
        }
        else if(rem.begins_with("..."))
        {
            _c4dbgp("got stream end '...'");
            _line_progressed(3);
            _handle_finished_file();
        }
        else
        {
            _c4err("parse error");
        }

        if( ! saved_scalar.empty())
        {
            _store_scalar(saved_scalar);
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_seq_expl()
{
    _c4dbgpf("handle_seq_expl: node_id=%zd level=%zd", m_state->node_id, m_state->level);
    cspan rem = m_state->line_contents.rem;

    C4_ASSERT(has_none(RKEY));
    C4_ASSERT(has_all(EXPL));

    if(rem.begins_with(' '))
    {
        // with explicit flow, indentation does not matter
        _c4dbgp("starts with spaces");
        rem = rem.left_of(rem.first_not_of(' '));
        _c4dbgpf("skip %zd spaces", rem.len);
        _line_progressed(rem.len);
        return true;
    }
    else if(rem.begins_with('#'))
    {
        _c4dbgp("it's a comment");
        rem = _scan_comment(); // also progresses the line
        return true;
    }
    else if(rem.begins_with(']'))
    {
        _c4dbgp("end the sequence");
        _pop_level();
        _line_progressed(1);
        return true;
    }

    if(has_any(RVAL))
    {
        C4_ASSERT(has_none(RNXT));
        if(_is_scalar_next())
        {
            _c4dbgp("it's a scalar");
            rem = _scan_scalar();
            addrem_flags(RNXT, RVAL);
            _append_val(rem);
            return true;
        }
        else if(rem.begins_with('['))
        {
            _c4dbgp("val is a child seq");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _start_seq();
            add_flags(EXPL);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with('{'))
        {
            _c4dbgp("val is a child map");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _start_map();
            addrem_flags(EXPL|RKEY, RVAL);
            _line_progressed(1);
            return true;
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_anchors_and_refs())
        {
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_any(RNXT))
    {
        C4_ASSERT(has_none(RVAL));
        if(rem.begins_with(", "))
        {
            C4_ASSERT(has_all(EXPL));
            _c4dbgp("seq: expect next val");
            addrem_flags(RVAL, RNXT);
            _line_progressed(2);
            return true;
        }
        else if(rem.begins_with(','))
        {
            C4_ASSERT(has_all(EXPL));
            _c4dbgp("seq: expect next val");
            addrem_flags(RVAL, RNXT);
            _line_progressed(1);
            return true;
        }
    }
    else
    {
        _c4err("internal error");
    }

    return true;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_seq_impl()
{
    _c4dbgpf("handle_seq_impl: node_id=%zd level=%zd", m_state->node_id, m_state->level);
    cspan rem = m_state->line_contents.rem;

    C4_ASSERT(has_none(RKEY));
    C4_ASSERT(has_none(EXPL));

    if(rem.begins_with('#'))
    {
        _c4dbgp("it's a comment");
        rem = _scan_comment();
        return true;
    }

    if(has_any(RNXT))
    {
        C4_ASSERT(has_none(RVAL));

        if(_handle_indentation())
        {
            return true;
        }

        if(rem.begins_with("- "))
        {
            _c4dbgp("expect another val");
            addrem_flags(RVAL, RNXT);
            _line_progressed(2);
            return true;
        }
        else if(rem == '-')
        {
            _c4dbgp("expect another val");
            addrem_flags(RVAL, RNXT);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with(' '))
        {
            C4_ASSERT( ! _at_line_begin());
            rem = rem.left_of(rem.first_not_of(' '));
            _c4dbgpf("skipping %zd spaces", rem.len);
            _line_progressed(rem.len);
            return true;
        }
        else if(rem.begins_with("..."))
        {
            _c4dbgp("got stream end '...'");
            _line_progressed(3);
            _handle_finished_file();
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_any(RVAL))
    {
        if(_is_scalar_next())
        {
            _c4dbgp("it's a scalar");
            cspan s = _scan_scalar(); // this also progresses the line
            rem = m_state->line_contents.rem;
            if(rem.begins_with(':'))
            {
                _c4dbgp("actually, the scalar is the first key of a map, and it opens a new scope");
                addrem_flags(RNXT, RVAL); // before _push_level! This prepares the current level for popping by setting it to RNXT
                _push_level();
                _start_map();
                _store_scalar(s);
                _save_indentation(/*behind*/s.len);
                addrem_flags(RVAL, RKEY);
                _line_progressed(1);
            }
            else
            {
                _c4dbgp("appending val to current seq");
                _append_val(s);
                addrem_flags(RNXT, RVAL);
            }
            return true;
        }
        else if(rem.begins_with("- "))
        {
            _c4dbgp("val is a nested seq, indented");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level();
            _start_seq();
            _save_indentation();
            _line_progressed(2);
            return true;
        }
        else if(rem == '-')
        {
            _c4dbgp("val is a nested seq, indented");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level();
            _start_seq();
            _save_indentation();
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with('['))
        {
            _c4dbgp("val is a child seq, explicit");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _start_seq();
            add_flags(EXPL);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with('{'))
        {
            _c4dbgp("val is a child map, explicit");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _start_map();
            addrem_flags(EXPL|RKEY, RVAL);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with("? "))
        {
            _c4dbgp("val is a child map + this key is complex");
            addrem_flags(RNXT, RVAL); // before _push_level!
            _push_level();
            _start_map();
            addrem_flags(CPLX|RKEY, RVAL);
            _save_indentation();
            _line_progressed(2);
            return true;
        }
        else if(rem.begins_with(' '))
        {
            rem = rem.left_of(rem.first_not_of(' '));
            if(_at_line_begin())
            {
                if(rem.len == m_state->indref + 2)
                {
                    _c4dbgpf("skipping indentation: %zd spaces", rem.len);
                    _line_progressed(rem.len);
                    return true;
                }
                else
                {
                    _c4err("invalid indentation");
                }
            }
            else
            {
                _c4dbgpf("skipping %zd spaces", rem.len);
                _line_progressed(rem.len);
                return true;
            }
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_anchors_and_refs())
        {
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_map_expl()
{
    _c4dbgpf("handle_map_expl: node_id=%zd  level=%zd", m_state->node_id, m_state->level);
    cspan rem = m_state->line_contents.rem;

    C4_ASSERT(has_all(EXPL));

    if(has_any(EXPL)) // explicit flow, ie, inside {}, separated by commas
    {
        if(rem.begins_with(' '))
        {
            // with explicit flow, indentation does not matter
            _c4dbgp("starts with spaces");
            rem = rem.left_of(rem.first_not_of(' '));
            _c4dbgpf("skip %zd spaces", rem.len);
            _line_progressed(rem.len);
            return true;
        }
        else if(rem.begins_with('#'))
        {
            _c4dbgp("it's a comment");
            rem = _scan_comment(); // also progresses the line
            return true;
        }
        else if(rem.begins_with('}'))
        {
            _c4dbgp("end the map");
            _pop_level();
            _line_progressed(1);
            return true;
        }

        if(has_any(RNXT))
        {
            C4_ASSERT(has_none(RKEY|RVAL));
            if(rem.begins_with(", "))
            {
                _c4dbgp("seq: expect next keyval");
                addrem_flags(RKEY, RNXT);
                _line_progressed(2);
                return true;
            }
            else if(rem.begins_with(','))
            {
                _c4dbgp("seq: expect next keyval");
                addrem_flags(RKEY, RNXT);
                _line_progressed(1);
                return true;
            }
            else
            {
                _c4err("parse error");
            }
        }
        else if(has_any(RKEY))
        {
            C4_ASSERT(has_none(RNXT));
            C4_ASSERT(has_none(RVAL));
            C4_ASSERT(has_none(SSCL));

            if(_is_scalar_next())
            {
                _c4dbgp("it's a scalar");
                rem = _scan_scalar();
                _store_scalar(rem);
                rem = m_state->line_contents.rem;
            }

            if(rem.begins_with(": "))
            {
                _c4dbgp("wait for val");
                addrem_flags(RVAL, RKEY|CPLX);
                _line_progressed(2);
                return true;
            }
            else if(rem == ':')
            {
                _c4dbgp("start unknown");
                addrem_flags(RVAL, RKEY|CPLX);
                _start_unk();
                _line_progressed(1);
                return true;
            }
            else if(rem.begins_with('?'))
            {
                add_flags(CPLX);
                _line_progressed(1);
                return true;
            }
            else if(_handle_types())
            {
                return true;
            }
            else
            {
                _c4err("parse error");
            }
        }
        else if(has_any(RVAL))
        {
            C4_ASSERT(has_none(RNXT|RKEY));
            C4_ASSERT(has_all(SSCL));
            if(_is_scalar_next())
            {
                _c4dbgp("it's a scalar");
                rem = _scan_scalar();
                addrem_flags(RNXT, RVAL|RKEY);
                _append_key_val(rem);
                return true;
            }
            else if(rem.begins_with('['))
            {
                _c4dbgp("val is a child seq");
                addrem_flags(RNXT, RVAL|RKEY); // before _push_level!
                _push_level(/*explicit flow*/true);
                _move_scalar_from_top();
                _start_seq();
                add_flags(EXPL);
                _line_progressed(1);
                return true;
            }
            else if(rem.begins_with('{'))
            {
                _c4dbgp("val is a child map");
                addrem_flags(RNXT, RVAL|RKEY); // before _push_level!
                _push_level(/*explicit flow*/true);
                _move_scalar_from_top();
                _start_map();
                addrem_flags(EXPL|RKEY, RNXT|RVAL);
                _line_progressed(1);
                return true;
            }
            else if(_handle_types())
            {
                return true;
            }
            else if(_handle_anchors_and_refs())
            {
                return true;
            }
            else
            {
                _c4err("parse error");
            }
        }
        else
        {
            _c4err("internal error");
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_map_impl()
{
    _c4dbgpf("handle_map_impl: node_id=%zd  level=%zd", m_state->node_id, m_state->level);
    cspan rem = m_state->line_contents.rem;

    C4_ASSERT(has_none(EXPL));

    if(rem.begins_with('#'))
    {
        _c4dbgp("it's a comment");
        rem = _scan_comment();
        return true;
    }

    if(has_any(RNXT))
    {
        C4_ASSERT(has_none(RKEY));
        C4_ASSERT(has_none(RVAL));

        // actually, we don't need RNXT in indent-based maps.
        addrem_flags(RKEY, RNXT);
    }

    if(_handle_indentation())
    {
        //rem = m_state->line_contents.rem;
        return true;
    }

    if(has_any(RKEY))
    {
        C4_ASSERT(has_none(RNXT));
        C4_ASSERT(has_none(RVAL));

        if(_is_scalar_next())
        {
            _c4dbgp("it's a scalar");
            rem = _scan_scalar(); // this also progresses the line
            _store_scalar(rem);
            rem = m_state->line_contents.rem;

            if(rem.begins_with(':'))
            {
                _c4dbgp("wait for val");
                addrem_flags(RVAL, RKEY|CPLX);
                _line_progressed(1);
                rem = m_state->line_contents.rem;
                if(rem.begins_with(' '))
                {
                    C4_ASSERT( ! _at_line_begin());
                    rem = rem.left_of(rem.first_not_of(' '));
                    _c4dbgpf("skip %zd spaces", rem.len);
                    _line_progressed(rem.len);
                }
            }
            return true;
        }
        else if(rem.begins_with(' '))
        {
            C4_ASSERT( ! _at_line_begin());
            rem = rem.left_of(rem.first_not_of(' '));
            _c4dbgpf("skip %zd spaces", rem.len);
            _line_progressed(rem.len);
            return true;
        }
        else if(rem.begins_with('?'))
        {
            _c4dbgp("it's a complex key");
            add_flags(CPLX);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with(':') && has_all(CPLX))
        {
            _c4dbgp("complex key finished");
            addrem_flags(RVAL, RKEY|CPLX);
            _line_progressed(1);
            rem = m_state->line_contents.rem;
            if(rem.begins_with(' '))
            {
                C4_ASSERT( ! _at_line_begin());
                rem = rem.left_of(rem.first_not_of(' '));
                _c4dbgpf("skip %zd spaces", rem.len);
                _line_progressed(rem.len);
            }
            return true;
        }
        else if(_handle_types())
        {
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_any(RVAL))
    {
        C4_ASSERT(has_none(RNXT));
        C4_ASSERT(has_none(RKEY));

        if(_is_scalar_next())
        {
            _c4dbgp("it's a scalar");

            cspan s = _scan_scalar(); // this also progresses the line
            rem = m_state->line_contents.rem;

            if(rem.begins_with(": "))
            {
                _c4dbgp("actually, the scalar is the first key of a map");
                addrem_flags(RKEY, RVAL); // before _push_level! This prepares the current level for popping by setting it to RNXT
                _push_level();
                _start_map();
                _save_indentation(/*behind*/s.len);
                addrem_flags(RVAL, RKEY);
                _line_progressed(2);
            }
            else if(rem.begins_with(':'))
            {
                _c4dbgp("actually, the scalar is the first key of a map, and it opens a new scope");
                addrem_flags(RKEY, RVAL); // before _push_level! This prepares the current level for popping by setting it to RNXT
                _push_level();
                _start_map();
                _save_indentation(/*behind*/s.len);
                addrem_flags(RVAL, RKEY);
                _line_progressed(1);
            }
            else
            {
                _c4dbgp("appending keyval to current map");
                _append_key_val(s);
                addrem_flags(RKEY, RVAL);
            }
            return true;
        }
        else if(rem.begins_with("- "))
        {
            _c4dbgp("val is a nested seq, indented");
            addrem_flags(RKEY, RVAL); // before _push_level!
            _push_level();
            _start_seq();
            _save_indentation();
            _line_progressed(2);
            return true;
        }
        else if(rem == '-')
        {
            _c4dbgp("start unknown, indented");
            _start_unk();
            _save_indentation();
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with('['))
        {
            _c4dbgp("val is a child seq, explicit");
            addrem_flags(RKEY, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _move_scalar_from_top();
            _start_seq();
            add_flags(EXPL);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with('{'))
        {
            _c4dbgp("val is a child map, explicit");
            addrem_flags(RKEY, RVAL); // before _push_level!
            _push_level(/*explicit flow*/true);
            _move_scalar_from_top();
            _start_map();
            addrem_flags(EXPL|RKEY, RVAL);
            _line_progressed(1);
            return true;
        }
        else if(rem.begins_with(' '))
        {
            rem = rem.left_of(rem.first_not_of(' '));
            if(_at_line_begin())
            {
                if(rem.len == m_state->indref + 2)
                {
                    _c4dbgpf("skipping indentation: %zd spaces", rem.len);
                    _line_progressed(rem.len);
                    return true;
                }
                else
                {
                    _c4err("invalid indentation");
                }
            }
            else
            {
                _c4dbgpf("skipping %zd spaces", rem.len);
                _line_progressed(rem.len);
                return true;
            }
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_anchors_and_refs())
        {
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }
    else
    {
        _c4err("internal error");
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_top()
{
    _c4dbgp("handle_top");
    cspan rem = m_state->line_contents.rem;

    // use the full line, as the following tokens can appear only at top level
    C4_ASSERT(rem == m_state->line_contents.stripped);
    rem = m_state->line_contents.stripped;

    if(rem.begins_with('#'))
    {
        _c4dbgp("a comment line");
        _scan_comment();
        return true;
    }
    else if(rem.begins_with('%'))
    {
        _c4dbgp("%% directive!");
        if(rem.begins_with("%YAML"))
        {
            _c4err("not implemented");
        }
        else if(rem.begins_with("%TAG"))
        {
            _c4err("not implemented");
        }
        else
        {
            _c4err("unknown directive starting with %");
        }
        return true;
    }
    else if(rem.begins_with("---"))
    {
        C4_ASSERT(m_state->level == 0 || m_state->level == 1);
        if(m_state->level == 1)
        {
            // end/start a document
            C4_ASSERT(node(m_state)->is_doc());
            _c4dbgp("end current document");
            _pop_level();
        }

        // start a document
        _c4dbgp("start a document");
        _push_level();
        _start_doc();
        _line_progressed(3);

        // skip spaces after the tag
        rem = rem.subspan(3);
        if(rem.begins_with(' '))
        {
            //cspan s = rem.left_of(rem.first_not_of(' '));
            _line_progressed(rem.len);
        }
        return true;
    }
    else if(rem.begins_with("..."))
    {
        _c4dbgp("end current document");
        if( ! m_stack.empty())
        {
            _pop_level();
        }
        _line_progressed(3);
        return true;
    }
    else
    {
        _c4err("parse error");
    }

    return false;
}

//-----------------------------------------------------------------------------
cspan Parser::_scan_ref()
{
    cspan rem = m_state->line_contents.rem;
    C4_ASSERT(rem.begins_with("<<"));

    size_t pos = rem.find(": ");
    // for now we require the target anchor to be in the same line
    C4_ASSERT(pos != npos);
    _line_progressed(pos + 2);

    cspan ref = rem.right_of(pos);
    pos = ref.first_of('*');
    C4_ASSERT(pos != npos);
    ref = ref.right_of(pos);
    _line_progressed(pos);
    ref = ref.left_of(ref.first_of(' '));
    _line_progressed(ref.len);

    _c4dbgpf("scanned ref value: '%.*s'", _c4prsp(ref));
    return ref;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_anchors_and_refs()
{
    if(has_all(RVAL))
    {
        cspan rem = m_state->line_contents.rem;
        if(rem.begins_with('&'))
        {
            _c4dbgp("found an anchor!!!");
            C4_ASSERT(m_anchor.empty());
            cspan anchor = rem.left_of(rem.first_of(' '));
            _line_progressed(anchor.len);
            anchor = anchor.subspan(1); // skip the first character
            _c4dbgpf("anchor value: '%.*s'", _c4prsp(anchor));
            m_anchor = anchor;
            return true;
        }
        else if(rem.begins_with('*'))
        {
            _c4err("not implemented - this should have been catched elsewhere");
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_types()
{
    cspan rem = m_state->line_contents.rem;
    cspan t;

    if(rem.begins_with("!!"))
    {
        _c4dbgp("begins with '!!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 2);
        //t = t.subspan(2);
    }
    else if(rem.begins_with("!<"))
    {
        _c4dbgp("begins with '!<'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 2);
        //t = t.subspan(2, t.len-1);
    }
    else if(rem.begins_with("!h!"))
    {
        _c4dbgp("begins with '!h!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 3);
        //t = t.subspan(3);
    }
    else if(rem.begins_with('!'))
    {
        _c4dbgp("begins with '!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len > 1);
        //t = t.subspan(1);
    }

    if(t.empty())
    {
        _c4dbgp(".... tag was empty");
        return false;
    }

    _line_progressed(t.len);
    _c4dbgpf("tag was '%.*s'", _c4prsp(t));

    if(has_all(RMAP|RKEY))
    {
        _c4dbgpf("saving key tag '%.*s'", _c4prsp(t));
        m_key_tag = t;
    }
    else if(has_all(RMAP|RVAL))
    {
        _c4dbgpf("saving val tag '%.*s'", _c4prsp(t));
        m_val_tag = t;
    }
    else if(has_all(RSEQ|RVAL))
    {
        _c4dbgpf("saving val tag '%.*s'", _c4prsp(t));
        m_val_tag = t;
    }
    else
    {
        _c4err("internal error");
    }

    return true;
}

//-----------------------------------------------------------------------------
cspan Parser::_scan_scalar()
{
    cspan s = m_state->line_contents.rem;
    if(s.len == 0) return s;

    if(s.begins_with('\''))
    {
        s = _scan_quoted_scalar('\'');
        return s;
    }
    else if(s.begins_with('"'))
    {
        s = _scan_quoted_scalar('"');
        return s;
    }
    else if(s.begins_with('|') || s.begins_with('>'))
    {
        s = _scan_block();
        return s;
    }
    else if(has_any(RSEQ))
    {
        C4_ASSERT( ! has_all(RKEY));
        if(has_all(RVAL))
        {
            if(has_all(EXPL))
            {
                _c4dbgp("RSEQ|RVAL|EXPL");
                s = s.left_of(s.first_of(",]#:"));
                s = s.trimr(' ');
            }
            else
            {
                _c4dbgp("RSEQ|RVAL");
                s = s.left_of(s.first_of("#:"));
                s = s.trimr(' ');
            }
        }
        else
        {
            _c4err("internal error");
        }
    }
    else if(has_any(RMAP))
    {
        size_t colon_space = s.find(": ");
        if(colon_space == npos)
        {
            colon_space = s.find(":");
            C4_ASSERT(s.len > 0);
            if(colon_space != s.len-1)
            {
                colon_space = npos;
            }
        }

        if(has_all(RKEY))
        {
            _c4dbgp("RMAP|RKEY");
            if(has_any(CPLX))
            {
                C4_ASSERT(has_any(RMAP));
                s = s.left_of(colon_space);
            }
            else
            {
                s = s.triml(' ');
                s = s.left_of(colon_space);
                s = s.trimr(' ');
            }
        }
        else if(has_all(RVAL))
        {
            C4_ASSERT(has_none(CPLX));
            if(has_any(EXPL))
            {
                _c4dbgp("RMAP|RVAL|EXPL");
                s = s.left_of(s.first_of(",}#"));
                s = s.trim(' ');
            }
            else
            {
                s = s.left_of(s.first_of("#"));
                s = s.trim(' ');
            }
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_all(RUNK))
    {
        s = s.left_of(s.first_of(",:#"));
    }
    else
    {
        _c4err("not implemented");
    }

    _line_progressed(s.str - m_state->line_contents.rem.str + s.len);

    if(_at_line_end())
    {
        cspan n = _peek_next_line(m_state->pos.offset);
        if(n.begins_with(' ', m_state->line_contents.indentation + 1))
        {
            _c4dbgpf("does indentation increase? '%.*s'", _c4prsp(n));
            size_t ind = n.first_not_of(' ');
            if(ind == npos) // maybe n contains only spaces
            {
                ind = n.len;
            }
            const cspan contents = n.right_of(ind, /*include_pos*/true);
            if( ! contents.begins_with_any("-[{?") && (contents.first_of(':') == npos))
            {
                _c4dbgpf("reading scalar: it indents further: the scalar continues!!! indentation=%zd", ind);
                while(n.begins_with(' ', ind))
                {
                    _c4dbgpf("reading scalar: append another line: '%.*s'", _c4prsp(n));
                    _line_ended(); // advances to the peeked-at line, consuming all remaining (probably newline) characters on the current line
                    _scan_line();  // puts the peeked-at line in the buffer
                    C4_ASSERT(n == m_state->line_contents.rem);
                    _line_progressed(n.end() - (m_buf.str + m_state->pos.offset));
                    n = _peek_next_line(m_state->pos.offset);
                }
                span full(m_buf.str + (s.str - m_buf.str), m_buf.begin() + m_state->pos.offset);

                s = _filter_plain_scalar(full, ind);
            }
        }
    }

    _c4dbgpf("scalar was '%.*s'", _c4prsp(s));

    return s;
}

//-----------------------------------------------------------------------------
void Parser::_scan_line()
{
    if(m_state->pos.offset >= m_buf.len) return;

    char const* b = &m_buf[m_state->pos.offset];
    char const* e = b;

    // get the line stripped of newline chars
    while(e != m_buf.end() && (*e != '\n' && *e != '\r'))
    {
        ++e;
    }
    cspan stripped = m_buf.subspan(m_state->pos.offset, e - b);

    // advance pos to include the first line ending
    if(e != m_buf.end() && *e == '\r') ++e;
    if(e != m_buf.end() && *e == '\n') ++e;
    cspan full = m_buf.subspan(m_state->pos.offset, e - b);

    m_state->line_contents.reset(full, stripped);
}

//-----------------------------------------------------------------------------
cspan Parser::_peek_next_line(size_t pos) const
{
    pos = pos == npos ? m_state->pos.offset : pos;
    if(pos >= m_buf.len) return {};

    char const* b = &m_buf[pos];

    // skip one (or two) newlines
    if(b != m_buf.end() && *b == '\r') ++b;
    if(b != m_buf.end() && *b == '\n') ++b;
    if(b != m_buf.end() && *b == '\r') ++b;
    if(b != m_buf.end() && *b == '\n') ++b;

    // get the first non-empty line stripped of newline chars
    cspan curr;
    char const* e = b;
    while(1)
    {
        char const *tb = e;
        while(e != m_buf.end() && (*e != '\n' && *e != '\r'))
        {
            ++e;
        }
        curr.assign(tb, e);
        _c4dbgpf("peeking next line: '%.*s'", _c4prsp(curr));
        if(curr.empty() || curr.first_not_of(' ') != npos || e == m_buf.end())
        {
            break;
        }
    }

    cspan next(b, e);

    return next;
}

//-----------------------------------------------------------------------------
void Parser::_line_progressed(size_t ahead)
{
    _c4dbgpf("line[%zd] (%zd cols) progressed by %zd:  col %zd --> %zd   offset %zd --> %zd", m_state->pos.line, m_state->line_contents.full.len, ahead, m_state->pos.col, m_state->pos.col+ahead, m_state->pos.offset, m_state->pos.offset+ahead);
    m_state->pos.offset += ahead;
    m_state->pos.col += ahead;
    C4_ASSERT(m_state->pos.col <= m_state->line_contents.stripped.len+1);
    m_state->line_contents.rem = m_state->line_contents.rem.subspan(ahead);
}

void Parser::_line_ended()
{
    _c4dbgpf("line[%zd] (%zd cols) ended! offset %zd --> %zd", m_state->pos.line, m_state->line_contents.full.len, m_state->pos.offset, m_state->pos.offset+m_state->line_contents.full.len - m_state->line_contents.stripped.len);
    C4_ASSERT(m_state->pos.col == m_state->line_contents.stripped.len+1);
    m_state->pos.offset += m_state->line_contents.full.len - m_state->line_contents.stripped.len;
    ++m_state->pos.line;
    m_state->pos.col = 1;
}

//-----------------------------------------------------------------------------
void Parser::_save_indentation(size_t behind)
{
    C4_ASSERT(m_state->line_contents.rem.begin() >= m_state->line_contents.full.begin());
    m_state->indref = m_state->line_contents.rem.begin() - m_state->line_contents.full.begin();
    C4_ASSERT(behind <= m_state->indref);
    m_state->indref -= behind;
    _c4dbgpf("state[%zd]: saving indentation: %zd", m_state->indref, m_state-m_stack.begin());
}

//-----------------------------------------------------------------------------
void Parser::_push_level(bool explicit_flow_chars)
{
    _c4dbgpf("pushing level! currnode=%zd  currlevel=%zd", m_state->node_id, m_state->level);
    C4_ASSERT(m_state == &m_stack.top());
    if(node(m_state) == nullptr)
    {
        C4_ASSERT( ! explicit_flow_chars);
        return;
    }
    size_t st = RUNK;
    if(explicit_flow_chars)
    {
        st |= EXPL;
    }
    m_stack.push(*m_state);
    m_state = &m_stack.top();
    set_flags(st);
    m_state->node_id = (size_t)NONE;
    m_state->indref = (int)NONE;
    ++m_state->level;
    _c4dbgpf("pushing level: now, currlevel=%zd", m_state->level);
}

void Parser::_pop_level()
{
    _c4dbgpf("popping level! currnode=%zd currlevel=%zd", m_state->node_id, m_state->level);
    if(has_any(RMAP) || node(m_state)->is_map())
    {
        _stop_map();
    }
    if(has_any(RSEQ) || node(m_state)->is_seq())
    {
        _stop_seq();
    }
    if(node(m_state)->is_doc())
    {
        _stop_doc();
    }
    C4_ASSERT(m_stack.size() > 1);
    _prepare_pop();
    m_stack.pop();
    m_state = &m_stack.top();
    /*if(has_any(RMAP))
    {
        _toggle_key_val();
    }*/
    if(m_state->line_contents.indentation == 0)
    {
        //C4_ASSERT(has_none(RTOP));
        add_flags(RTOP);
    }
    _c4dbgpf("popping level: now, currnode=%zd currlevel=%zd", m_state->node_id, m_state->level);
}

//-----------------------------------------------------------------------------
void Parser::_start_unk(bool /*as_child*/)
{
    _c4dbgp("start_unk");
    _push_level();
    _move_scalar_from_top();
}

//-----------------------------------------------------------------------------
void Parser::_start_doc(bool as_child)
{
    _c4dbgpf("start_doc (as child=%d)", as_child);
    add_flags(RUNK);
    C4_ASSERT(node(m_stack.bottom()) == node(m_root_id));
    size_t parent_id = m_stack.size() < 2 ? m_root_id : m_stack.top(1).node_id;
    C4_ASSERT(parent_id != NONE);
    C4_ASSERT(m_tree->is_root(parent_id));
    C4_ASSERT(node(m_state) == nullptr || node(m_state) == node(m_root_id));
    if(as_child)
    {
        if( ! m_tree->is_stream(parent_id))
        {
            m_tree->to_stream(parent_id);
        }
        m_state->node_id = m_tree->append_child(parent_id);
        m_tree->to_doc(m_state->node_id);
    }
    else
    {
        C4_ASSERT(m_tree->is_seq(parent_id) || m_tree->empty(parent_id));
        m_state->node_id = parent_id;
        if( ! m_tree->is_doc(parent_id))
        {
            m_tree->to_doc(parent_id, DOC);
        }
    }
    _c4dbgpf("start_doc: id=%zd", m_state->node_id);
}

void Parser::_stop_doc()
{
    _c4dbgp("stop_doc");
    C4_ASSERT(node(m_state)->is_doc());
}

//-----------------------------------------------------------------------------
void Parser::_start_map(bool as_child)
{
    _c4dbgpf("start_map (as child=%d)", as_child);
    addrem_flags(RMAP|RVAL, RKEY|RUNK);
    C4_ASSERT(node(m_stack.bottom()) == node(m_root_id));
    size_t parent_id = m_stack.size() < 2 ? m_root_id : m_stack.top(1).node_id;
    C4_ASSERT(parent_id != NONE);
    C4_ASSERT(node(m_state) == nullptr || node(m_state) == node(m_root_id));
    if(as_child)
    {
        m_state->node_id = m_tree->append_child(parent_id);
        if(has_all(SSCL))
        {
            cspan key = _consume_scalar();
            m_tree->to_map(m_state->node_id, key);
            _c4dbgpf("start_map: id=%zd key='%.*s'", m_state->node_id, _c4prsp(node(m_state)->key()));
        }
        else
        {
            m_tree->to_map(m_state->node_id);
            _c4dbgpf("start_map: id=%zd", m_state->node_id);
        }
    }
    else
    {
        C4_ASSERT(m_tree->is_map(parent_id) || m_tree->empty(parent_id));
        m_state->node_id = parent_id;
        m_tree->to_map(parent_id);
        _move_scalar_from_top();
        _c4dbgpf("start_map: id=%zd", m_state->node_id);
    }
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("start_seq: set val tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(m_state->node_id, m_val_tag);
        m_val_tag.clear();
    }
    if( ! m_anchor.empty())
    {
        _c4dbgpf("start_map: set anchor to '%.*s'", _c4prsp(m_anchor));
        m_tree->set_anchor(m_state->node_id, m_anchor);
        m_anchor.clear();
        ++m_num_anchors;
    }
}

void Parser::_stop_map()
{
    _c4dbgp("stop_map");
    C4_ASSERT(node(m_state)->is_map());
}

//-----------------------------------------------------------------------------
void Parser::_start_seq(bool as_child)
{
    _c4dbgpf("start_seq (as child=%d)", as_child);
    addrem_flags(RSEQ|RVAL, RUNK);
    C4_ASSERT(node(m_stack.bottom()) == node(m_root_id));
    size_t parent_id = m_stack.size() < 2 ? m_root_id : m_stack.top(1).node_id;
    C4_ASSERT(parent_id != NONE);
    C4_ASSERT(node(m_state) == nullptr || node(m_state) == node(m_root_id));
    if(as_child)
    {
        m_state->node_id = m_tree->append_child(parent_id);
        if(has_all(SSCL))
        {
            C4_ASSERT(node(parent_id)->is_map());
            cspan name = _consume_scalar();
            m_tree->to_seq(m_state->node_id, name);
            _c4dbgpf("start_seq: id=%zd name='%.*s'", m_state->node_id, _c4prsp(node(m_state)->key()));
        }
        else
        {
            size_t as_doc = 0;
            if(node(m_state)->is_doc()) as_doc |= DOC;
            m_tree->to_seq(m_state->node_id, as_doc);
            _c4dbgpf("start_seq: id=%zd%s", m_state->node_id, as_doc ? " as doc" : "");
        }
    }
    else
    {
        C4_ASSERT(m_tree->is_seq(parent_id) || m_tree->empty(parent_id));
        m_state->node_id = parent_id;
        size_t as_doc = 0;
        if(node(m_state)->is_doc()) as_doc |= DOC;
        m_tree->to_seq(parent_id, as_doc);
        _move_scalar_from_top();
        _c4dbgpf("start_seq: id=%zd%s", m_state->node_id, as_doc?" as_doc":"");
    }
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("start_seq: set val tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(m_state->node_id, m_val_tag);
        m_val_tag.clear();
    }
    if( ! m_anchor.empty())
    {
        _c4dbgpf("start_seq: set anchor to '%.*s'", _c4prsp(m_anchor));
        m_tree->set_anchor(m_state->node_id, m_anchor);
        m_anchor.clear();
        ++m_num_anchors;
    }
}

void Parser::_stop_seq()
{
    _c4dbgp("stop_seq");
    C4_ASSERT(node(m_state)->is_seq());
}

//-----------------------------------------------------------------------------
NodeData* Parser::_append_val(cspan const& val)
{
    C4_ASSERT( ! has_all(SSCL));
    C4_ASSERT(node(m_state) != nullptr);
    C4_ASSERT(node(m_state)->is_seq());
    _c4dbgpf("append val: '%.*s' to parent id=%zd (level=%zd)", _c4prsp(val), m_state->node_id, m_state->level);
    size_t nid = m_tree->append_child(m_state->node_id);
    m_tree->to_val(nid, val);
    _c4dbgpf("append val: id=%zd name='%.*s' val='%.*s'", nid, _c4prsp(m_tree->get(nid)->m_key.scalar), _c4prsp(m_tree->get(nid)->m_val.scalar));
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("append val: set tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(nid, m_val_tag);
        m_val_tag.clear();
    }
    if( ! m_anchor.empty())
    {
        _c4dbgpf("append val: set anchor to '%.*s'", _c4prsp(m_anchor));
        m_tree->set_anchor(nid, m_anchor);
        m_anchor.clear();
        ++m_num_anchors;
    }
    if(m_tree->val(nid).begins_with('*'))
    {
        _c4dbgp("append val: it's a reference");
        m_tree->_add_flags(nid, REF);
        ++m_num_references;
    }
    return m_tree->get(nid);
}

NodeData* Parser::_append_key_val(cspan const& val)
{
    C4_ASSERT(node(m_state)->is_map());
    cspan key = _consume_scalar();
    _c4dbgpf("append keyval: '%.*s' '%.*s' to parent id=%zd (level=%zd)", _c4prsp(key), _c4prsp(val), m_state->node_id, m_state->level);
    size_t nid = m_tree->append_child(m_state->node_id);
    m_tree->to_keyval(nid, key, val);
    _c4dbgpf("append keyval: id=%zd name='%.*s' val='%.*s'", nid, _c4prsp(m_tree->get(nid)->key()), _c4prsp(m_tree->get(nid)->val()));
    if( ! m_key_tag.empty())
    {
        _c4dbgpf("append keyval: set key tag to '%.*s'", _c4prsp(m_key_tag));
        m_tree->set_key_tag(nid, m_key_tag);
        m_key_tag.clear();
    }
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("append keyval: set val tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(nid, m_val_tag);
        m_val_tag.clear();
    }
    if( ! m_anchor.empty())
    {
        _c4dbgpf("append keyval: set anchor to '%.*s'", _c4prsp(m_anchor));
        m_tree->set_anchor(nid, m_anchor);
        m_anchor.clear();
        ++m_num_anchors;
    }
    if(m_tree->key(nid) == "<<" || m_tree->val(nid).begins_with('*'))
    {
        _c4dbgp("append keyval: it's a reference");
        if( ! (m_tree->val(nid).begins_with('*')))
        {
             _c4err("malformed reference");
        }
        m_tree->_add_flags(nid, REF);
        ++m_num_references;
    }
    return m_tree->get(nid);
}

//-----------------------------------------------------------------------------
void Parser::_store_scalar(cspan const& s)
{
    _c4dbgpf("state[%zd]: storing scalar '%.*s' (flag: %zd) (old scalar='%.*s')", m_state-m_stack.begin(), _c4prsp(s), m_state->flags & SSCL, _c4prsp(m_state->scalar));
    C4_ASSERT(has_none(SSCL));
    add_flags(SSCL);
    m_state->scalar = s;
}

cspan Parser::_consume_scalar()
{
    _c4dbgpf("state[%zd]: consuming scalar '%.*s' (flag: %zd))", m_state-m_stack.begin(), _c4prsp(m_state->scalar), m_state->flags & SSCL);
    C4_ASSERT(m_state->flags & SSCL);
    cspan s = m_state->scalar;
    rem_flags(SSCL);
    m_state->scalar.clear();
    return s;
}

void Parser::_move_scalar_from_top()
{
    if(m_stack.size() < 2) return;
    State &prev = m_stack.top(1);
    C4_ASSERT(m_state == &m_stack.top());
    C4_ASSERT(m_state != &prev);
    if(prev.flags & SSCL)
    {
        _c4dbgpf("moving scalar '%.*s' from state[%zd] to state[%zd] (overwriting '%.*s')", _c4prsp(prev.scalar), &prev-m_stack.begin(), m_state-m_stack.begin(), _c4prsp(m_state->scalar));
        add_flags(prev.flags & SSCL);
        m_state->scalar = prev.scalar;
        rem_flags(SSCL, &prev);
        prev.scalar.clear();
    }
}

//-----------------------------------------------------------------------------
bool Parser::_handle_indentation()
{
    C4_ASSERT(has_none(EXPL));
    if( ! _at_line_begin()) return false;

    size_t ind = m_state->line_contents.indentation;

    if(ind == (size_t)m_state->indref)
    {
        _c4dbgpf("same indentation (%zd) -- nothing to see here", ind);
        _line_progressed(ind);
        return ind > 0;
    }
    else if(ind < (size_t)m_state->indref)
    {
        _c4dbgpf("smaller indentation (%zd < %zd)!!!", ind, m_state->indref);
        State const* popto = nullptr;
        C4_ASSERT(m_stack.is_contiguous()); // this search relies on the stack being contiguous
        for(State const* s = m_state-1; s >= m_stack.begin(); --s)
        {
            _c4dbgpf("searching for state with indentation %zd. curr=%zd (curr level=%zd)", ind, s->indref, s->level);
            if((size_t)s->indref == ind)
            {
                _c4dbgpf("gotit!!! at level %zd", s->level);
                popto = s;
                break;
            }
        }
        C4_ASSERT(popto != nullptr);
        C4_ASSERT(popto != m_state);
        C4_ASSERT(popto <  m_state);
        C4_ASSERT(popto->level <  m_state->level);
        _c4dbgpf("popping %zd level%s: from level %zd to level %zd", m_state->level-popto->level, m_state->level-popto->level > 1 ? "s":"", m_state->level, popto->level);
        while(m_state != popto)
        {
            _c4dbgpf("popping level %zd (indentation=%zd)", m_state->level, m_state->indref);
            _pop_level();
        }
        C4_ASSERT(m_state->indref == ind);
        _line_progressed(ind);
        return true;
    }
    else // ind > indref
    {
        _c4dbgpf("larger indentation (%zd > %zd)!!!", ind, m_state->indref);
        if(has_all(RMAP|RVAL))
        {
            addrem_flags(RKEY, RVAL);
            _start_unk();
            //_move_scalar_from_top();
            _line_progressed(ind);
            _save_indentation();
            return true;
        }
        else
        {
            _c4err("parse error - indentation should not increase at this point");
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
cspan Parser::_scan_comment()
{
    cspan s = m_state->line_contents.rem;
    C4_ASSERT(s.begins_with('#'));
    _line_progressed(s.len);
    // skip the # character
    s = s.subspan(1);
    // skip leading whitespace
    s = s.right_of(s.first_not_of(' '), /*include_pos*/true);
    _c4dbgpf("comment was '%.*s'", _c4prsp(s));
    return s;
}

//-----------------------------------------------------------------------------
cspan Parser::_scan_quoted_scalar(const char q)
{
    C4_ASSERT(q == '\'' || q == '\"');

    // quoted scalars can spread over multiple lines!
    // nice explanation here: http://yaml-multiline.info/

    bool needs_filter = false;

    // a span to the end of the file
    const size_t b = m_state->pos.offset;
    span s = m_buf.subspan(b);
    C4_ASSERT(s.begins_with(q));

    // skip the opening quote
    _line_progressed(1);
    s = s.subspan(1);

    // find the pos of the matching quote
    size_t pos = npos;
    while( ! _finished_file())
    {
        const cspan line = m_state->line_contents.rem;
        bool line_is_blank = true;

        if(q == '\'') // scalars with single quotes
        {
            _c4dbgpf("scanning single quoted scalar @ line[%zd]:  line=\"%.*s\"", m_state->pos.line, _c4prsp(line));
            for(size_t i = 0; i < line.len; ++i)
            {
                const char curr = line.str[i];
                const char next = i+1 < line.len ? line.str[i+1] : '\0';
                if(curr == '\'') // single quotes are escaped with two single quotes
                {
                    if(next != '\'') // so just look for the first quote
                    {                // without another after it
                        pos = i;
                        break;
                    }
                    else
                    {
                        needs_filter = true; // needs filter to remove escaped quotes
                        ++i; // skip the escaped quote
                    }
                }
                else if(curr != ' ')
                {
                    line_is_blank = false;
                }
            }
        }
        else // scalars with double quotes
        {
            _c4dbgpf("scanning double quoted scalar @ line[%zd]:  line='%.*s'", m_state->pos.line, _c4prsp(line));
            for(size_t i = 0; i < line.len; ++i)
            {
                const char curr = line.str[i];
                const char next = i+1 < line.len ? line.str[i+1] : '\0';
                if(curr != ' ')
                {
                    line_is_blank = false;
                }
                // every \ is an escape
                if(curr == '\\')
                {
                    needs_filter = true;
                    if(next == '"')
                    {
                        ++i;
                    }
                }
                else if(curr == '"')
                {
                    pos = i;
                    break;
                }
            }
        }

        // leading whitespace also needs filtering
        needs_filter = needs_filter || line_is_blank || (_at_line_begin() && line.begins_with(' '));

        if(pos == npos)
        {
            _line_progressed(line.len);
            _c4dbgpf("scanning scalar @ line[%zd]: sofar=\"%.*s\"", m_state->pos.line, _c4prsp(s.subspan(0, m_state->pos.offset-b)));
        }
        else
        {
            C4_ASSERT(pos >= 0 && pos < m_buf.len);
            C4_ASSERT(m_buf[m_state->pos.offset + pos] == q);
            _line_progressed(pos + 1); // progress beyond the quote
            pos = m_state->pos.offset - b - 1; // but we stop before it
            break;
        }

        _next_line();
        _scan_line();
    }

    if(pos == npos)
    {
        _c4err("reached end of file while looking for closing quote");
    }
    else if(pos == 0)
    {
        s.clear();
        C4_ASSERT( ! needs_filter);
    }
    else
    {
        C4_ASSERT(s.end() >= m_buf.begin() && s.end() <= m_buf.end());
        C4_ASSERT(s.end() == m_buf.end() || *s.end() == q);
        s = s.subspan(0, pos-1);
    }

    if(needs_filter)
    {
        cspan ret;
        if(q == '\'')
        {
            ret = _filter_squot_scalar(s);
        }
        else if(q == '"')
        {
            ret = _filter_dquot_scalar(s);
        }
        C4_ASSERT(ret.len < s.len || s.empty());
        _c4dbgpf("final scalar: \"%.*s\"", _c4prsp(ret));
        return ret;
    }

    _c4dbgpf("final scalar: \"%.*s\"", _c4prsp(s));

    return s;
}

//-----------------------------------------------------------------------------
cspan Parser::_scan_block()
{
    // nice explanation here: http://yaml-multiline.info/
    cspan s = m_state->line_contents.rem;
    C4_ASSERT(s.begins_with('|') || s.begins_with('>'));

    _c4dbgpf("scanning block: specs=\"%.*s\"", _c4prsp(s));

    // parse the spec
    BlockStyle_e newline = s.begins_with('>') ? BLOCK_FOLD : BLOCK_LITERAL;
    BlockChomp_e chomp = CHOMP_CLIP; // default to clip unless + or - are used
    size_t indentation = npos; // have to find out if no spec is given
    cspan digits;
    if(s.len > 1)
    {
        cspan t = s.subspan(1);
        C4_ASSERT(t.len >= 1);
        if(t[0] == '-')
        {
            chomp = CHOMP_STRIP;
            t = t.subspan(1);
        }
        else if(t[0] == '+')
        {
            chomp = CHOMP_KEEP;
            t = t.subspan(1);
        }

        // from here to the end, only digits are considered
        digits = t.left_of(t.first_not_of("0123456789"));
        if( ! digits.empty())
        {
            if( ! _read_decimal(digits, &indentation))
            {
                _c4err("parse error: could not read decimal");
            }
        }
    }

    // finish the current line
    _line_progressed(s.len);
    _next_line();
    _scan_line();

    if(indentation == npos)
    {
        indentation = m_state->line_contents.indentation;
    }

    _c4dbgpf("scanning block:  style=%s", newline==BLOCK_FOLD ? "fold" : "literal");
    _c4dbgpf("scanning block:  chomp=%s", chomp==CHOMP_CLIP ? "clip" : (chomp==CHOMP_STRIP ? "strip" : "keep"));
    _c4dbgpf("scanning block: indent=%zd (digits='%.*s')", indentation, _c4prsp(digits));

    // start with a zero-length block, already pointing at the right place
    span raw_block(m_buf.data() + m_state->pos.offset, size_t(0));// m_state->line_contents.full.subspan(0, 0);
    C4_ASSERT(raw_block.begin() == m_state->line_contents.full.begin());

    // read every full line into a raw block,
    // from which newlines are to be stripped as needed
    size_t num_lines = 0, first = m_state->pos.line;
    while(( ! _finished_file()))
    {
        _scan_line();
        if(m_state->line_contents.indentation < indentation) break;
        raw_block.len += m_state->line_contents.full.len;
        _c4dbgpf("scanning block: append '%.*s'", _c4prsp(m_state->line_contents.rem));
        _line_progressed(m_state->line_contents.rem.len);
        _next_line();
        ++num_lines;
    }
    C4_ASSERT(m_state->pos.line == (first + num_lines));
    (void)num_lines; // prevent warning
    (void)first;

    _c4dbgpf("scanning block: raw='%.*s'", _c4prsp(raw_block));

    // ok! now we strip the newlines and spaces according to the specs
    s = _filter_block_scalar(raw_block, newline, chomp, indentation);

    _c4dbgpf("scanning block: final='%.*s'", _c4prsp(s));

    return s;
}

//-----------------------------------------------------------------------------
inline span erase(span d, size_t pos, size_t num)
{
    C4_ASSERT(pos >= 0 && pos+num <= d.len);
    size_t num_to_move = d.len - pos - num;
    memmove(d.str + pos, d.str + pos + num, sizeof(span::char_type) * num_to_move);
    return span(d.str, d.len - num);
}

//-----------------------------------------------------------------------------
cspan Parser::_filter_plain_scalar(span s, size_t indentation)
{
    _c4dbgpf("filtering plain scalar: before='%.*s'", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    span r = _filter_whitespace(s, indentation);

    // now another sweep for newlines
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        //const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == '\n')
        {
            if(next != '\n')
            {
                r[i] = ' '; // a single unix newline: turn it into a space
            }
            else if(curr == '\n' && next == '\n')
            {
                r = erase(r, i+1, 1); // keep only one of consecutive newlines
            }
        }
    }

    C4_ASSERT(s.len >= r.len);
    _c4dbgpf("filtering plain scalar: num filtered chars=%zd", s.len - r.len);
    _c4dbgpf("filtering plain scalar: after='%.*s'", _c4prsp(r));

#ifdef RYML_DBG
    for(size_t i = r.len; i < s.len; ++i)
    {
        s[i] = '~';
    }
#endif

    return r;
}

//-----------------------------------------------------------------------------
cspan Parser::_filter_squot_scalar(span s)
{
    _c4dbgpf("filtering single-quoted scalar: before=\"%.*s\"", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    span r = _filter_whitespace(s);

    // now another sweep for quotes and newlines
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        //const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == '\'' && (curr == next))
        {
            r = erase(r, i+1, 1); // turn two consecutive single quotes into one
        }
        else if(curr == '\n')
        {
            if(next != '\n')
            {
                r[i] = ' '; // a single unix newline: turn it into a space
            }
            else if(curr == '\n' && next == '\n')
            {
                r = erase(r, i+1, 1); // keep only one of consecutive newlines
            }
        }
    }

    C4_ASSERT(s.len >= r.len);
    _c4dbgpf("filtering single-quoted scalar: num filtered chars=%zd", s.len - r.len);
    _c4dbgpf("filtering single-quoted scalar: after=\"%.*s\"", _c4prsp(r));

#ifdef RYML_DBG
    for(size_t i = r.len; i < s.len; ++i)
    {
        s[i] = '~';
    }
#endif

    return r;
}

//-----------------------------------------------------------------------------
cspan Parser::_filter_dquot_scalar(span s)
{
    _c4dbgpf("filtering double-quoted scalar: before='%.*s'", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    span r = _filter_whitespace(s);

    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        //const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == '\\')
        {
            if(next == curr)
            {
                r = erase(r, i+1, 1); // turn two consecutive backslashes into one
            }
            else if(next == '\n')
            {
                r = erase(r, i, 2);  // newlines are escaped with \ -- delete both
            }
            else if(next == '"')
            {
                r = erase(r, i, 1);  // fix escaped double quotes
            }
            else if(next == 'n')
            {
                r = erase(r, i+1, 1);
                r[i] = '\n';
            }
        }
        else if(curr == '\n')
        {
            if(next != '\n')
            {
                r[i] = ' '; // a single unix newline: turn it into a space
            }
            else if(curr == '\n' && next == '\n')
            {
                r = erase(r, i+1, 1); // keep only one of consecutive newlines
            }
        }
    }

    C4_ASSERT(s.len >= r.len);
    _c4dbgpf("filtering double-quoted scalar: num filtered chars=%zd", s.len - r.len);
    _c4dbgpf("filtering double-quoted scalar: after='%.*s'", _c4prsp(r));

#ifdef RYML_DBG
    for(size_t i = r.len; i < s.len; ++i)
    {
        s[i] = '~';
    }
#endif

    return r;
}

//-----------------------------------------------------------------------------
span Parser::_filter_whitespace(span r, size_t indentation, bool leading_whitespace)
{
    _c4dbgpf("filtering whitespace: before=\"%.*s\"", _c4prsp(r));

    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == ' ' && prev == '\n')
        {
            if(next == ' ')
            {
                cspan ss = r.subspan(i);
                ss = ss.left_of(ss.first_not_of(' '));
                C4_ASSERT(ss.len > 1);
                size_t num = ss.len;
                C4_ASSERT(num >= indentation);
                if(indentation)
                {
                    num = indentation;
                }
                if(leading_whitespace)
                {
                    num = ss.len;
                }
                r = erase(r, i, num);
            }
            else
            {
                r = erase(r, i, 1);
            }
        }
        // erase \r --- https://stackoverflow.com/questions/1885900
        else if(curr == '\r' && next == '\n') // this is the right order
        {
            r = erase(r, i, 1);
        }
        else if(curr == '\n' && next == '\r') // this is the wrong order
        {
            r = erase(r, i+1, 1);
        }
        else if(curr == '\r')
        {
            r[i] = '\n'; // use only \n
        }
    }

    _c4dbgpf("filtering whitespace: after=\"%.*s\"", _c4prsp(r));

    return r;
}

//-----------------------------------------------------------------------------
cspan Parser::_filter_block_scalar(span s, BlockStyle_e style, BlockChomp_e chomp, size_t indentation)
{
    _c4dbgpf("filtering block: '%.*s'", _c4prsp(s));

    C4_ASSERT(s.ends_with('\n') || s.ends_with('\r'));

    span r = s;

    if(indentation > 0)
    {
        r = _filter_whitespace(s, indentation, /*leading whitespace*/false);
        C4_ASSERT(r.begins_with(' ', indentation));
        r = erase(r, 0, indentation);
    }

    _c4dbgpf("filtering block: after whitespace='%.*s'", _c4prsp(r));

    switch(chomp)
    {
    case CHOMP_KEEP: // nothing to do
        break;
    case CHOMP_STRIP: // strip everything
    {
        auto pos = r.last_not_of("\r\n");
        C4_ASSERT(pos != npos);
        r = r.left_of(pos, /*include_pos*/true);
        break;
    }
    case CHOMP_CLIP: // clip to a single newline
    {
        auto pos = r.last_not_of("\r\n");
        C4_ASSERT(pos != npos && pos+1 < r.len);
        ++pos;
        if(r[pos] == '\r') // deal with \r\n sequences
        {
            C4_ASSERT(pos+1 < s.len);
            ++pos;
            C4_ASSERT(r[pos] == '\n');
        }
        r = r.left_of(pos, /*include_pos*/true);
        break;
    }
    default:
        _c4err("unknown chomp style");
    }

    _c4dbgpf("filtering block: after chomp='%.*s'", _c4prsp(r));

    switch(style)
    {
    case BLOCK_LITERAL:
        break;
    case BLOCK_FOLD:
        {
            auto pos = r.last_not_of("\r\n"); // do not fold trailing newlines
            C4_ASSERT(pos != npos);
            C4_ASSERT(pos < r.len);
            ++pos; // point pos at the first newline char
            span t = r.subspan(0, pos);
            for(size_t i = 0; i < t.len; ++i)
            {
                const char curr = t[i];
                //const char prev = i   > 0     ? t[i-1] : '\0';
                const char next = i+1 < t.len ? t[i+1] : '\0';
                if(curr == '\n')
                {
                    if(next != '\n')
                    {
                        t[i] = ' '; // a single unix newline: turn it into a space
                    }
                    else if(curr == '\n' && next == '\n')
                    {
                        t = erase(t, i+1, 1); // keep only one of consecutive newlines
                    }
                }
            }
            // copy over the trailing newlines
            span nl = r.subspan(pos);
            C4_ASSERT(t.len + nl.len <= r.len);
            for(size_t i = 0; i < nl.len; ++i)
            {
                r[t.len + i] = nl[i];
            }
            // now trim r
            r = r.subspan(0, t.len + nl.len);
        }
        break;
    default:
        _c4err("unknown block style");
    }

    _c4dbgpf("filtering block: final='%.*s'", _c4prsp(r));

#ifdef RYML_DBG
    for(size_t i = r.len; i < s.len; ++i)
    {
        s[i] = '~';
    }
#endif

    return r;
}

//-----------------------------------------------------------------------------
void Parser::_resolve_references()
{

}

//-----------------------------------------------------------------------------
bool Parser::_read_decimal(cspan const& str, size_t *decimal)
{
    C4_ASSERT(str.len >= 1);
    size_t n = 0;
    for(size_t i = 0; i < str.len; ++i)
    {
        char c = str.str[i];
        if(c < '0' || c > '9') return false;
        n = n*10 + (c-'0');
    }
    *decimal = n;
    return true;
}

//-----------------------------------------------------------------------------
size_t Parser::_count_nlines(cspan src)
{
    size_t n = (src.len > 0);
    while(src.len > 0)
    {
        n += (src.begins_with('\n') || src.begins_with('\r'));
        src = src.subspan(1);
    }
    return n;
}

//-----------------------------------------------------------------------------
void Parser::set_flags(size_t f, State * s)
{
#ifdef RYML_DBG
    char buf1[64], buf2[64];
    int len1 = _prfl(buf1, sizeof(buf1), f);
    int len2 = _prfl(buf2, sizeof(buf2), s->flags);
    _c4dbgpf("state[%zd]: setting flags to %.*s: before=%.*s", s-m_stack.begin(), len1, buf1, len2, buf2);
#endif
    s->flags = f;
}

void Parser::add_flags(size_t on, State * s)
{
#ifdef RYML_DBG
    char buf1[64], buf2[64], buf3[64];
    int len1 = _prfl(buf1, sizeof(buf1), on);
    int len2 = _prfl(buf2, sizeof(buf2), s->flags);
    int len3 = _prfl(buf3, sizeof(buf3), s->flags|on);
    _c4dbgpf("state[%zd]: adding flags %.*s: before=%.*s after=%.*s", s-m_stack.begin(), len1, buf1, len2, buf2, len3, buf3);
#endif
    s->flags |= on;
}

void Parser::addrem_flags(size_t on, size_t off, State * s)
{
#ifdef RYML_DBG
    char buf1[64], buf2[64], buf3[64], buf4[64];
    int len1 = _prfl(buf1, sizeof(buf1), on);
    int len2 = _prfl(buf2, sizeof(buf2), off);
    int len3 = _prfl(buf3, sizeof(buf3), s->flags);
    int len4 = _prfl(buf4, sizeof(buf4), ((s->flags|on)&(~off)));
    _c4dbgpf("state[%zd]: adding flags %.*s / removing flags %.*s: before=%.*s after=%.*s", s-m_stack.begin(), len1, buf1, len2, buf2, len3, buf3, len4, buf4);
#endif
    s->flags |= on;
    s->flags &= ~off;
}

void Parser::rem_flags(size_t off, State * s)
{
#ifdef RYML_DBG
    char buf1[64], buf2[64], buf3[64];
    int len1 = _prfl(buf1, sizeof(buf1), off);
    int len2 = _prfl(buf2, sizeof(buf2), s->flags);
    int len3 = _prfl(buf3, sizeof(buf3), s->flags&(~off));
    _c4dbgpf("state[%zd]: removing flags %.*s: before=%.*s after=%.*s", s-m_stack.begin(), len1, buf1, len2, buf2, len3, buf3);
#endif
    s->flags &= ~off;
}

//-----------------------------------------------------------------------------
void Parser::_err(const char *fmt, ...) const
{
#ifndef RYML_ERRMSG_SIZE
    #define RYML_ERRMSG_SIZE 1024
#endif
    char errmsg[RYML_ERRMSG_SIZE];
    int len = sizeof(errmsg);

    va_list args;
    va_start(args, fmt);
    len = _fmt_msg(errmsg, len, fmt, args);
    va_end(args);
    c4::yml::error(errmsg, len);
}

//-----------------------------------------------------------------------------
#ifdef RYML_DBG
void Parser::_dbg(const char *fmt, ...) const
{
    char errmsg[RYML_ERRMSG_SIZE];
    int len = sizeof(errmsg);

    va_list args;
    va_start(args, fmt);
    len = _fmt_msg(errmsg, len, fmt, args);
    va_end(args);
    printf("%.*s", len, errmsg);
}
#endif

//-----------------------------------------------------------------------------
#define _wrapbuf() pos += del; len -= del; if(len < 0) { pos = 0; len = buflen; }

int Parser::_fmt_msg(char *buf, int buflen, const char *fmt, va_list args) const
{
    int len = buflen;
    int pos = 0;
    auto const& lc = m_state->line_contents;


    // first line: print the message
    int del = vsnprintf(buf + pos, len, fmt, args);
    _wrapbuf();
    del = snprintf(buf + pos, len, "\n");
    _wrapbuf();

    // next line: print the yaml src line
    if( ! m_file.empty())
    {
        del = snprintf(buf + pos, len, "%.*s:%zd: '", (int)m_file.len, m_file.str, m_state->pos.line);
    }
    else
    {
        del = snprintf(buf + pos, len, "line %zd: '", m_state->pos.line);
    }
    int offs = del;
    _wrapbuf();
    del = snprintf(buf + pos, len, "%.*s' (sz=%zd)\n",
                   (int)lc.stripped.len, lc.stripped.str, lc.stripped.len);
    _wrapbuf();

    // next line: highlight the remaining portion of the previous line
    if(lc.rem.len)
    {
        size_t firstcol = lc.rem.begin() - lc.full.begin();
        size_t lastcol = firstcol + lc.rem.len;
        del = snprintf(buf + pos, len, "%*s", (int)(offs+firstcol), ""); // this works only for spaces....
        _wrapbuf();
        // the %*s technique works only for spaces, so put the characters directly
        del = (int)lc.rem.len;
        for(int i = 0; i < del && i < len; ++i) { buf[pos + i] = (i ? '~' : '^'); }
        _wrapbuf();
        del = snprintf(buf + pos, len, "  (cols %zd-%zd)\n", firstcol+1, lastcol+1);
        _wrapbuf();
    }
    else
    {
        del = snprintf(buf + pos, len, "\n");
        _wrapbuf();
    }

    // next line: print the state flags
    {
        del = snprintf(buf + pos, len, "top state: ");
        _wrapbuf();

        del = _prfl(buf + pos, len, m_state->flags);
        _wrapbuf();

        del = snprintf(buf + pos, len, "\n");
        _wrapbuf();
    }


    return pos;
}

int Parser::_prfl(char *buf, int buflen, size_t v)
{
    int len = buflen;
    int pos = 0, del = 0;

    bool gotone = false;
#define _prflag(fl)                                 \
    if((v & (fl)) == (fl))                          \
    {                                               \
        if(!gotone)                                 \
        {                                           \
            gotone = true;                          \
        }                                           \
        else                                        \
        {                                           \
            del = snprintf(buf + pos, len, "|");    \
            _wrapbuf();                             \
        }                                           \
        del = snprintf(buf + pos, len, #fl);        \
        _wrapbuf();                                 \
    }

    _prflag(RTOP);
    _prflag(RUNK);
    _prflag(RMAP);
    _prflag(RSEQ);
    _prflag(EXPL);
    _prflag(CPLX);
    _prflag(RKEY);
    _prflag(RVAL);
    _prflag(RNXT);
    _prflag(SSCL);
#undef _prflag

    return pos;
}

#undef _wrapbuf

} // namespace yml
} // namespace c4

#pragma GCC diagnostic pop
