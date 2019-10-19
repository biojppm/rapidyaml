#include "c4/yml/parse.hpp"
#include "c4/yml/detail/parser_dbg.hpp"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits" // to remove a warning on an assertion that a size_t >= 0. Later on, this size_t will turn into a template argument, and then it can become < 0.
#endif

namespace c4 {
namespace yml {

static bool _is_scalar_next__runk(csubstr s)
{
    if(s.begins_with(": ") || s.begins_with_any("#,:?"))
    {
        return false;
    }
    return true;
}

static bool _is_scalar_next__rseq_rval(csubstr s)
{
    if(s.begins_with_any("[{!?&"))
    {
        return false;
    }
    else if(s.begins_with("- ") || s == "-")
    {
        return false;
    }
    return true;
}

static bool _is_scalar_next__rseq_rnxt(csubstr s)
{
    if(s.begins_with("- "))
    {
        return false;
    }
    else if(s == "-")
    {
        return false;
    }

    return true;
}

static bool _is_scalar_next__rmap(csubstr s)
{
    if(s.begins_with(": ") || s.begins_with_any("#,!?&"))
    {
        return false;
    }
    return true;
}

static bool _is_scalar_next__rmap_val(csubstr s)
{
    if(s.begins_with("- ") || s.begins_with_any("{[") || s == "-")
    {
        return false;
    }
    return true;
}

static bool _is_doc_sep(csubstr s)
{
    constexpr const csubstr dashes = "---";
    constexpr const csubstr ellipsis = "...";
    constexpr const csubstr whitesp = " \t";
    if(s.begins_with(dashes))
    {
        return s == dashes || s.sub(3).begins_with_any(whitesp);
    }
    else if(s.begins_with(ellipsis))
    {
        return s == ellipsis || s.sub(3).begins_with_any(whitesp); 
    }
    return false;
}


//-----------------------------------------------------------------------------
Parser::Parser(Allocator const& a)
    :
    m_file(),
    m_buf(),
    m_root_id(NONE),
    m_tree(),
    m_stack(a),
    m_state(),
    m_key_tag(),
    m_val_tag(),
    m_key_anchor(),
    m_val_anchor()
{
    State st{};
    m_stack.push(st);
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

    m_key_tag.clear();
    m_val_tag.clear();
    m_key_anchor.clear();
    m_val_anchor.clear();
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
void Parser::parse(csubstr file, substr buf, Tree *t, size_t node_id)
{
    m_file = file;
    m_buf = buf;
    m_root_id = node_id;
    m_tree = t;

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
    _end_stream();
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
bool Parser::_handle_unk()
{
    _c4dbgp("handle_unk");

    csubstr rem = m_state->line_contents.rem;
    const bool start_as_child = (node(m_state) == nullptr);

    C4_ASSERT(has_none(RNXT|RSEQ|RMAP));
    if(m_state->indref > 0)
    {
        csubstr ws = rem.left_of(rem.first_not_of(' '));
        if(m_state->indref <= ws.len)
        {
            _c4dbgpf("skipping base indentation of %zd", m_state->indref);
            _line_progressed(m_state->indref);
            rem = rem.sub(m_state->indref);
        }
    }

    if(rem.begins_with("- "))
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
    else if(_handle_types())
    {
        return true;
    }
    else if(has_all(SSCL))
    {
        _c4dbgpf("there's a stored scalar: '%.*s'", _c4prsp(m_state->scalar));

        csubstr saved_scalar;
        if(_scan_scalar(&saved_scalar))
        {
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
            _end_stream();
            _line_progressed(3);
        }
        else if(rem.begins_with('#'))
        {
            _c4dbgpf("it's a comment: '%.*s'", _c4prsp(rem));
            _scan_comment();
            return true;
        }
        else if(_handle_key_anchors_and_refs())
        {
            return true;
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
    else
    {
        C4_ASSERT( ! has_any(SSCL));
        csubstr scalar;
        size_t indentation = m_state->line_contents.indentation; // save
        if(_scan_scalar(&scalar))
        {
            _c4dbgp("got a scalar");
            rem = m_state->line_contents.rem;
            _store_scalar(scalar);
            if(rem.begins_with(": "))
            {
                _c4dbgpf("got a ': ' next -- it's a map (as_child=%d)", start_as_child);
                _push_level();
                _start_map(start_as_child); // wait for the val scalar to append the key-val pair
                _set_indentation(indentation);
                _line_progressed(2); // call this AFTER saving the indentation
            }
            else if(rem == ":")
            {
                _c4dbgpf("got a ':' next -- it's a map (as_child=%d)", start_as_child);
                _push_level();
                _start_map(start_as_child); // wait for the val scalar to append the key-val pair
                _set_indentation(indentation);
                _line_progressed(1); // call this AFTER saving the indentation
            }
            else
            {
                // we still don't know whether it's a seq or a map
                // so just store the scalar
            }
            return true;
        }
        else if(rem.begins_with(' '))
        {
            csubstr ws = rem.left_of(rem.first_not_of(' '));
            rem = rem.right_of(ws);
            if(has_all(RTOP) && rem.begins_with("---"))
            {
                _c4dbgp("there's a doc starting, and it's indented");
                _set_indentation(ws.len);
            }
            else
            {
                _c4dbgpf("skipping %zd spaces", ws.len);
            }
            _line_progressed(ws.len);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_seq_expl()
{
    _c4dbgpf("handle_seq_expl: node_id=%zd level=%zd", m_state->node_id, m_state->level);
    csubstr rem = m_state->line_contents.rem;

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
        if(_scan_scalar(&rem))
        {
            _c4dbgp("it's a scalar");
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
        else if(_handle_val_anchors_and_refs())
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
        else
        {
            _c4err("was expecting a comma");
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
    csubstr rem = m_state->line_contents.rem;

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
            _end_stream();
            _line_progressed(3);
            return true;
        }
        else if(rem.begins_with("---"))
        {
            _c4dbgp("got document start '---'");
            _start_new_doc(rem);
            return true;
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_any(RVAL))
    {
        // there can be empty values
        if(_handle_indentation())
        {
            return true;
        }

        csubstr s;
        if(_scan_scalar(&s)) // this also progresses the line
        {
            _c4dbgp("it's a scalar");
            rem = m_state->line_contents.rem;
            size_t skip = 0;
            if(rem.begins_with(' '))
            {
                _c4dbgp("skipping whitespace...");
                skip = rem.first_not_of(' ');
                _line_progressed(skip);
                rem = rem.sub(skip);
            }

            if(rem.begins_with(": ") || rem.ends_with(':'))
            {
                _c4dbgp("actually, the scalar is the first key of a map, and it opens a new scope");
                addrem_flags(RNXT, RVAL); // before _push_level! This prepares the current level for popping by setting it to RNXT
                _push_level();
                _start_map();
                _store_scalar(s);
                _save_indentation(/*behind*/s.len + skip);
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
            if(_rval_dash_start_or_continue_seq())
            {
                _line_progressed(2);
            }
            return true;
        }
        else if(rem == '-')
        {
            if(_rval_dash_start_or_continue_seq())
            {
                _line_progressed(1);
            }
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
            csubstr spc = rem.left_of(rem.first_not_of(' '));
            if(_at_line_begin())
            {
                _c4dbgpf("skipping value indentation: %zd spaces", spc.len);
                _line_progressed(spc.len);
                return true;
            }
            else
            {
                _c4dbgpf("skipping %zd spaces", spc.len);
                _line_progressed(spc.len);
                return true;
            }
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_val_anchors_and_refs())
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

bool Parser::_rval_dash_start_or_continue_seq()
{
    const csubstr rem = m_state->line_contents.rem;
    size_t ind = rem.begin() - m_state->line_contents.full.begin();
    C4_ASSERT(ind >= m_state->indref);
    size_t delta_ind = ind - m_state->indref;
    if( ! delta_ind)
    {
        _c4dbgp("prev val was empty");
        addrem_flags(RNXT, RVAL);
        _append_val("~");
        return false;
    }
    _c4dbgp("val is a nested seq, indented");
    addrem_flags(RNXT, RVAL); // before _push_level!
    _push_level();
    _start_seq();
    _save_indentation();
    return true;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_map_expl()
{
    _c4dbgpf("handle_map_expl: node_id=%zd  level=%zd", m_state->node_id, m_state->level);
    csubstr rem = m_state->line_contents.rem;

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
            if(has_all(SSCL))
            {
                _c4dbgp("the last val was null");
                _append_key_val("~");
                rem_flags(RVAL);
            }
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

            if(_scan_scalar(&rem))
            {
                _c4dbgp("it's a scalar");
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
                _c4dbgp("complex key");
                add_flags(CPLX);
                _line_progressed(1);
                return true;
            }
            else if(rem.begins_with(','))
            {
                _c4dbgp("prev scalar was a key with null value");
                _append_key_val("~");
                _line_progressed(1);
                return true;
            }
            else if(_handle_types())
            {
                return true;
            }
            else if(_handle_key_anchors_and_refs())
            {
                return true;
            }
            else if(rem == "")
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
            if(_scan_scalar(&rem))
            {
                _c4dbgp("it's a scalar");
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
            else if(_handle_val_anchors_and_refs())
            {
                return true;
            }
            else if(rem.begins_with(','))
            {
                _c4dbgp("appending empty val");
                _append_key_val("~");
                addrem_flags(RKEY, RVAL);
                _line_progressed(1);
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
    csubstr rem = m_state->line_contents.rem;

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

        if(_scan_scalar(&rem)) // this also progresses the line
        {
            _c4dbgp("it's a scalar");
            _store_scalar(rem);
            if(has_all(CPLX|RSET))
            {
                _c4dbgp("it's a complex key, so use null value '~'");
                _append_key_val("~");
            }
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
            //C4_ASSERT( ! _at_line_begin());
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
        else if(has_all(CPLX) && rem.begins_with(':'))
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
        else if(rem.begins_with("..."))
        {
            _c4dbgp("end current document");
            _end_stream();
            _line_progressed(3);
            return true;
        }
        else if(rem.begins_with("---"))
        {
            _c4dbgp("start new document '---'");
            _start_new_doc(rem);
            return true;
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_key_anchors_and_refs())
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

        csubstr s;
        if(_scan_scalar(&s)) // this also progresses the line
        {
            _c4dbgp("it's a scalar");

            rem = m_state->line_contents.rem;

            if(rem.begins_with(": "))
            {
                _c4dbgp("actually, the scalar is the first key of a map");
                addrem_flags(RKEY, RVAL); // before _push_level! This prepares the current level for popping by setting it to RNXT
                _push_level();
                _move_scalar_from_top();
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
                _move_scalar_from_top();
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
            _move_scalar_from_top();
            _start_seq();
            _save_indentation();
            _line_progressed(2);
            return true;
        }
        else if(rem == '-')
        {
            _c4dbgp("maybe a seq. start unknown, indented");
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
            csubstr spc = rem.left_of(rem.first_not_of(' '));
            if(_at_line_begin())
            {
                _c4dbgpf("skipping value indentation: %zd spaces", spc.len);
                _line_progressed(spc.len);
                return true;
            }
            else
            {
                _c4dbgpf("skipping %zd spaces", spc.len);
                _line_progressed(spc.len);
                return true;
            }
        }
        else if(_handle_types())
        {
            return true;
        }
        else if(_handle_val_anchors_and_refs())
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
    csubstr rem = m_state->line_contents.rem;

    if(rem.begins_with('#'))
    {
        _c4dbgp("a comment line");
        _scan_comment();
        return true;
    }

    // use the full line, as the following tokens can appear only at top level
    C4_ASSERT(rem == m_state->line_contents.stripped
              ||
              m_state->indref > 0 && rem.begin() > m_state->line_contents.stripped.begin() && 
              m_state->indref == (rem.begin() - m_state->line_contents.stripped.begin()));
    if(m_state->indref == 0)
    {
        rem = m_state->line_contents.stripped;
    }

    if(rem.begins_with('%'))
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
            _c4err("unknown directive starting with %%");
        }
        return true;
    }
    else if(rem.begins_with("---"))
    {
        _start_new_doc(rem);
        return true;
    }
    else if(rem.begins_with("..."))
    {
        _c4dbgp("end current document");
        _end_stream();
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
csubstr Parser::_scan_ref()
{
    csubstr rem = m_state->line_contents.rem;
    C4_ASSERT(rem.begins_with("<<"));

    size_t pos = rem.find(": ");
    // for now we require the target anchor to be in the same line
    C4_ASSERT(pos != npos);
    _line_progressed(pos + 2);

    csubstr ref = rem.right_of(pos);
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
bool Parser::_handle_key_anchors_and_refs()
{
    C4_ASSERT(!has_any(RVAL));
    csubstr rem = m_state->line_contents.rem;
    if(rem.begins_with('&'))
    {
        _c4dbgp("found a key anchor!!!");
        C4_ASSERT(m_key_anchor.empty());
        csubstr anchor = rem.left_of(rem.first_of(' '));
        _line_progressed(anchor.len);
        anchor = anchor.sub(1); // skip the first character
        _c4dbgpf("key anchor value: '%.*s'", _c4prsp(anchor));
        m_key_anchor = anchor;
        return true;
    }
    else if(rem.begins_with('*'))
    {
        _c4err("not implemented - this should have been catched elsewhere");
        C4_NEVER_REACH();
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_val_anchors_and_refs()
{
    C4_ASSERT(!has_any(RKEY));
    csubstr rem = m_state->line_contents.rem;
    if(rem.begins_with('&'))
    {
        _c4dbgp("found a val anchor!!!");
        C4_ASSERT(m_val_anchor.empty());
        csubstr anchor = rem.left_of(rem.first_of(' '));
        _line_progressed(anchor.len);
        anchor = anchor.sub(1); // skip the first character
        _c4dbgpf("val anchor value: '%.*s'", _c4prsp(anchor));
        m_val_anchor = anchor;
        return true;
    }
    else if(rem.begins_with('*'))
    {
        _c4err("not implemented - this should have been catched elsewhere");
        C4_NEVER_REACH();
        return false;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool Parser::_handle_types()
{
    csubstr rem = m_state->line_contents.rem.triml(' ');
    csubstr t;

    if(rem.begins_with("!!"))
    {
        _c4dbgp("begins with '!!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 2);
        //t = t.sub(2);
        if(t == "!!set")
        {
            add_flags(RSET);
        }
    }
    else if(rem.begins_with("!<"))
    {
        _c4dbgp("begins with '!<'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 2);
        //t = t.sub(2, t.len-1);
    }
    else if(rem.begins_with("!h!"))
    {
        _c4dbgp("begins with '!h!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len >= 3);
        //t = t.sub(3);
    }
    else if(rem.begins_with('!'))
    {
        _c4dbgp("begins with '!'");
        t = rem.left_of(rem.first_of(' '));
        C4_ASSERT(t.len > 1);
        //t = t.sub(1);
    }

    if(t.empty())
    {
        _c4dbgp(".... tag was empty");
        return false;
    }

    _c4dbgpf("there was a tag: '%.*s'", _c4prsp(t));
    _line_progressed(t.len);

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
    else if(has_all(RTOP|RUNK))
    {
        _c4dbgpf("saving doc tag '%.*s'", _c4prsp(t));
        m_val_tag = t;
    }
    else
    {
        _c4err("internal error");
    }

    return true;
}


//-----------------------------------------------------------------------------
bool Parser::_scan_scalar(csubstr *scalar)
{
    csubstr s = m_state->line_contents.rem;
    if(s.len == 0) return false;
    s = s.trim(' ');
    if(s.len == 0) return false;

    if(s.begins_with('\''))
    {
        *scalar = _scan_quoted_scalar('\'');
        return true;
    }
    else if(s.begins_with('"'))
    {
        *scalar = _scan_quoted_scalar('"');
        return true;
    }
    else if(s.begins_with('|') || s.begins_with('>'))
    {
        *scalar = _scan_block();
        return true;
    }
    else if(has_any(RTOP) && _is_doc_sep(s))
    {
        return false;
    }
    else if(has_any(RSEQ))
    {
        C4_ASSERT( ! has_all(RKEY));
        if(has_all(RVAL))
        {
            _c4dbgp("RSEQ|RVAL");
            if( ! _is_scalar_next__rseq_rval(s))
            {
                return false;
            }
            s = s.left_of(s.find(" #")); // is there a comment?
            s = s.left_of(s.find(": ")); // is there a key-value?
            if(s.ends_with(':')) s = s.left_of(s.len-1);
            if(has_all(EXPL))
            {
                _c4dbgp("RSEQ|RVAL|EXPL");
                s = s.left_of(s.first_of(",]"));
            }
            s = s.trimr(' ');
        }
        else if(has_all(RNXT))
        {
            if( ! _is_scalar_next__rseq_rnxt(s))
            {
                return false;
            }
            _c4err("internal error");
        }
        else
        {
            _c4err("internal error");
        }
    }
    else if(has_any(RMAP))
    {
        if( ! _is_scalar_next__rmap(s))
        {
            return false;
        }
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
            if(has_any(CPLX))
            {
                _c4dbgp("RMAP|RKEY|CPLX");
                C4_ASSERT(has_any(RMAP));
                s = s.left_of(colon_space);
            }
            else
            {
                _c4dbgp("RMAP|RKEY");
                s = s.triml(' ');
                s = s.left_of(colon_space);
                s = s.trimr(' ');
                if(has_any(EXPL))
                {
                    _c4dbgp("RMAP|RVAL|EXPL");
                    s = s.left_of(s.first_of(",}"));
                }
                else if(s.begins_with("..."))
                {
                    return false;
                }
                else if(s.begins_with("---"))
                {
                    return false;
                }
            }
        }
        else if(has_all(RVAL))
        {
            _c4dbgp("RMAP|RVAL");
            C4_ASSERT(has_none(CPLX));
            if( ! _is_scalar_next__rmap_val(s))
            {
                return false;
            }
            s = s.left_of(s.find(" #")); // is there a comment?
            if(has_any(EXPL))
            {
                _c4dbgp("RMAP|RVAL|EXPL");
                s = s.left_of(s.first_of(",}"));
            }
            s = s.trim(' ');
        }
        else
        {
            _c4err("parse error");
        }
    }
    else if(has_all(RUNK))
    {
        _c4dbgp("RUNK");
        if( ! _is_scalar_next__runk(s))
        {
            return false;
        }
        s = s.left_of(s.find(" #"));
        size_t pos = s.find(": ");
        if(pos != npos)
        {
            s = s.left_of(pos);
        }
        else if(s.ends_with(':'))
        {
            s = s.left_of(s.len-1);
        }
        else
        {
            s = s.left_of(s.first_of(','));
        }
        s = s.trim(' ');
    }
    else
    {
        _c4err("not implemented");
    }

    _line_progressed(s.str - m_state->line_contents.rem.str + s.len);

    // deal with scalars that continue to the next line

    // PHEWW*. this sucks. it's crazy and very fragile. revisit and make this better.
    if(_at_line_end() && !s.begins_with_any("*"))
    {
        csubstr n = _peek_next_line(m_state->pos.offset);
        if(has_none(EXPL) && n.begins_with(' ', m_state->line_contents.indentation + 1))
        {
            _c4dbgpf("does indentation increase? '%.*s'", _c4prsp(n));
            size_t ind = n.first_not_of(' ');  // maybe n contains only spaces
            if(ind == npos)
            {
                ind = n.len;
            }
            const csubstr contents = n.right_of(ind, /*include_pos*/true);
            if( ! contents.begins_with_any("-[{?#") && (contents.first_of(':') == npos))
            {
                _c4dbgpf("reading scalar: it indents further: the scalar continues!!! indentation=%zd", ind);
                while(n.begins_with(' ', ind))
                {
                    _c4dbgpf("reading scalar: append another line: '%.*s'", _c4prsp(n));
                    _line_ended(); // advances to the peeked-at line, consuming all remaining (probably newline) characters on the current line
                    _scan_line();  // puts the peeked-at line in the buffer
                    C4_ASSERT(n == m_state->line_contents.rem);
                    if(_finished_file()) break;
                    _line_progressed(n.end() - (m_buf.str + m_state->pos.offset));
                    n = _peek_next_line(m_state->pos.offset);
                }
                substr full(m_buf.str + (s.str - m_buf.str), m_buf.begin() + m_state->pos.offset);
                s = _filter_plain_scalar(full, ind);
            }
        }
        else if(has_all(EXPL))
        {
            constexpr const csubstr chars = "[]{}?#,";
            size_t pos = n.first_of(chars);
            while(pos != 0)
            {
                if(pos == npos)
                {
                    _c4dbgpf("reading scalar: append another line: '%.*s'", _c4prsp(n));
                    pos = n.len;
                    _line_ended(); // advances to the peeked-at line, consuming all remaining (probably newline) characters on the current line
                    _scan_line();  // puts the peeked-at line in the buffer
                    C4_ASSERT(n == m_state->line_contents.rem);
                    if(_finished_file()) break;
                    _line_progressed(n.end() - (m_buf.str + m_state->pos.offset));
                    n = _peek_next_line(m_state->pos.offset);
                    pos = n.first_of(chars);
                }
                else
                {
                    n = n.left_of(pos);
                    _c4dbgpf("reading scalar: append another line: '%.*s'", _c4prsp(n));
                    _line_ended(); // advances to the peeked-at line, consuming all remaining (probably newline) characters on the current line
                    _scan_line();  // puts the peeked-at line in the buffer
                    if(_finished_file()) break;
                    _line_progressed(n.end() - (m_buf.str + m_state->pos.offset));
                    break;
                }
            }
            substr full(m_buf.str + (s.str - m_buf.str), m_buf.begin() + m_state->pos.offset);
            full = full.trimr("\r\n ");
            s = _filter_plain_scalar(full, /*indentation*/0);
        }
    }

    _c4dbgpf("scalar was '%.*s'", _c4prsp(s));

    *scalar = s;
    return true;
}

//-----------------------------------------------------------------------------
void Parser::_scan_line()
{
    if(m_state->pos.offset >= m_buf.len) return;

    char const* b = &m_buf[m_state->pos.offset];
    char const* e = b;

    // get the line stripped of newline chars
    while(e < m_buf.end() && (*e != '\n' && *e != '\r'))
    {
        ++e;
    }
    csubstr stripped = m_buf.sub(m_state->pos.offset, e - b);

    // advance pos to include the first line ending
    if(e != m_buf.end() && *e == '\r') ++e;
    if(e != m_buf.end() && *e == '\n') ++e;
    csubstr full = m_buf.sub(m_state->pos.offset, e - b);

    m_state->line_contents.reset(full, stripped);
}

//-----------------------------------------------------------------------------
csubstr Parser::_peek_next_line(size_t pos) const
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
    csubstr curr;
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

    csubstr next(b, e);

    return next;
}

//-----------------------------------------------------------------------------
void Parser::_line_progressed(size_t ahead)
{
    _c4dbgpf("line[%zd] (%zd cols) progressed by %zd:  col %zd --> %zd   offset %zd --> %zd", m_state->pos.line, m_state->line_contents.full.len, ahead, m_state->pos.col, m_state->pos.col+ahead, m_state->pos.offset, m_state->pos.offset+ahead);
    m_state->pos.offset += ahead;
    m_state->pos.col += ahead;
    C4_ASSERT(m_state->pos.col <= m_state->line_contents.stripped.len+1);
    m_state->line_contents.rem = m_state->line_contents.rem.sub(ahead);
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
void Parser::_set_indentation(size_t indentation)
{
    m_state->indref = indentation;
    _c4dbgpf("state[%zd]: saving indentation: %zd", m_state-m_stack.begin(), m_state->indref);
}
void Parser::_save_indentation(size_t behind)
{
    C4_ASSERT(m_state->line_contents.rem.begin() >= m_state->line_contents.full.begin());
    m_state->indref = m_state->line_contents.rem.begin() - m_state->line_contents.full.begin();
    C4_ASSERT(behind <= m_state->indref);
    m_state->indref -= behind;
    _c4dbgpf("state[%zd]: saving indentation: %zd", m_state-m_stack.begin(), m_state->indref);
}

//-----------------------------------------------------------------------------
void Parser::_write_key_anchor(size_t node_id)
{
    C4_ASSERT(m_tree->has_key(node_id));
    if( ! m_key_anchor.empty())
    {
        _c4dbgpf("node=%zd: set key anchor to '%.*s'", node_id, _c4prsp(m_key_anchor));
        m_tree->set_key_anchor(node_id, m_key_anchor);
        m_key_anchor.clear();
    }
    else
    {
        csubstr r = m_tree->key(node_id);
        if(r.begins_with('*'))
        {
            _c4dbgpf("node=%zd: set key reference: '%.*s'", node_id, _c4prsp(r));
            m_tree->set_key_ref(node_id, r.sub(1));
        }
        else if(r == "<<")
        {
            _c4dbgpf("node=%zd: it's an inheriting reference", node_id);
            if(m_tree->is_seq(node_id))
            {
                _c4dbgpf("node=%zd: inheriting from seq of %zd", node_id, m_tree->num_children(node_id));
                for(size_t i = m_tree->first_child(node_id); i != NONE; i = m_tree->next_sibling(i))
                {
                    if( ! (m_tree->val(i).begins_with('*')))
                    {
                        _c4err("malformed reference: '%.*s'", _c4prsp(m_tree->val(i)));
                    }
                }
            }
            else if( ! (m_tree->val(node_id).begins_with('*') ))
            {
                 _c4err("malformed reference: '%.*s'", _c4prsp(m_tree->val(node_id)));
            }
            //m_tree->set_key_ref(node_id, r);
        }
    }
}

//-----------------------------------------------------------------------------
void Parser::_write_val_anchor(size_t node_id)
{
    if( ! m_val_anchor.empty())
    {
        _c4dbgpf("node=%zd: set val anchor to '%.*s'", node_id, _c4prsp(m_val_anchor));
        m_tree->set_val_anchor(node_id, m_val_anchor);
        m_val_anchor.clear();
    }
    csubstr r = m_tree->has_val(node_id) ? m_tree->val(node_id) : "";
    if(r.begins_with('*'))
    {
        _c4dbgpf("node=%zd: set val reference: '%.*s'", node_id, _c4prsp(r));
        m_tree->set_val_ref(node_id, r.sub(1));
    }
}

//-----------------------------------------------------------------------------
void Parser::_push_level(bool explicit_flow_chars)
{
    _c4dbgpf("pushing level! currnode=%zd  currlevel=%zd", m_state->node_id, m_state->level);
    C4_ASSERT(m_state == &m_stack.top());
    if(node(m_state) == nullptr)
    {
        _c4dbgp("pushing level! actually no, current node is null");
        //C4_ASSERT( ! explicit_flow_chars);
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
    m_state->indref = (size_t)NONE;
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
    add_flags(RUNK|RTOP);
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
    _handle_types();
}

void Parser::_stop_doc()
{
    _c4dbgp("stop_doc");
    C4_ASSERT(node(m_state)->is_doc());
}

void Parser::_end_stream()
{
    C4_ASSERT( ! m_stack.empty());
    if(has_any(SSCL))
    {
        if(m_tree->type(m_state->node_id) == NOTYPE)
        {
            m_tree->to_seq(m_state->node_id);
            _append_val(_consume_scalar());
        }
        else if(m_tree->is_doc(m_state->node_id))
        {
            m_tree->to_doc(m_state->node_id, SEQ);
            _append_val(_consume_scalar());
        }
        else if(m_tree->is_seq(m_state->node_id))
        {
            _append_val(_consume_scalar());
        }
        else if(m_tree->is_map(m_state->node_id))
        {
            _append_key_val("~");
        }
        else
        {
            _c4err("internal error");
        }
    }
    else if(has_all(RSEQ|RVAL) && has_none(EXPL))
    {
        _append_val("~");
    }

    while(m_stack.size() > 1)
    {
        _c4dbgpf("popping level: %zu (stack sz=%zu...", m_state->level, m_stack.size());
        C4_ASSERT( ! has_any(SSCL, &m_stack.top()));
        _pop_level();
    }

}

void Parser::_start_new_doc(csubstr rem)
{
    _c4dbgp("_start_new_doc");
    C4_ASSERT(rem.begins_with("---"));

    _end_stream();

    // start a document
    _c4dbgp("start a document");
    size_t indref = m_state->indref;
    _line_progressed(3);
    _push_level();
    _start_doc();
    _set_indentation(indref);

    // skip spaces after the tag
    rem = rem.sub(3);
    if(rem.begins_with(' '))
    {
        rem = rem.left_of(rem.first_not_of(' '));
        _line_progressed(rem.len);
    }
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
            csubstr key = _consume_scalar();
            m_tree->to_map(m_state->node_id, key);
            _c4dbgpf("start_map: id=%zd key='%.*s'", m_state->node_id, _c4prsp(node(m_state)->key()));
            _write_key_anchor(m_state->node_id);
        }
        else
        {
            m_tree->to_map(m_state->node_id);
            _c4dbgpf("start_map: id=%zd", m_state->node_id);
        }
        _write_val_anchor(m_state->node_id);
    }
    else
    {
        C4_ASSERT(m_tree->is_map(parent_id) || m_tree->empty(parent_id));
        m_state->node_id = parent_id;
        _c4dbgpf("start_map: id=%zd", m_state->node_id);
        int as_doc = 0;
        if(node(m_state)->is_doc()) as_doc |= DOC;
        m_tree->to_map(parent_id, as_doc);
        _move_scalar_from_top();
        _write_val_anchor(parent_id);
        if(parent_id != NONE)
        {
            State const& parent_state = m_stack.top(1);
            if(parent_state.flags & RSET)
            {
                add_flags(RSET);
            }
        }
    }
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("start_map: set val tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(m_state->node_id, m_val_tag);
        m_val_tag.clear();
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
            csubstr name = _consume_scalar();
            m_tree->to_seq(m_state->node_id, name);
            _c4dbgpf("start_seq: id=%zd name='%.*s'", m_state->node_id, _c4prsp(node(m_state)->key()));
            _write_key_anchor(m_state->node_id);
        }
        else
        {
            int as_doc = 0;
            if(node(m_state)->is_doc()) as_doc |= DOC;
            m_tree->to_seq(m_state->node_id, as_doc);
            _c4dbgpf("start_seq: id=%zd%s", m_state->node_id, as_doc ? " as doc" : "");
        }
        _write_val_anchor(m_state->node_id);
    }
    else
    {
        C4_ASSERT(m_tree->is_seq(parent_id) || m_tree->empty(parent_id));
        m_state->node_id = parent_id;
        int as_doc = 0;
        if(node(m_state)->is_doc()) as_doc |= DOC;
        m_tree->to_seq(parent_id, as_doc);
        _move_scalar_from_top();
        _c4dbgpf("start_seq: id=%zd%s", m_state->node_id, as_doc ? " as_doc" : "");
        _write_val_anchor(parent_id);
    }
    if( ! m_val_tag.empty())
    {
        _c4dbgpf("start_seq: set val tag to '%.*s'", _c4prsp(m_val_tag));
        m_tree->set_val_tag(m_state->node_id, m_val_tag);
        m_val_tag.clear();
    }
}

void Parser::_stop_seq()
{
    _c4dbgp("stop_seq");
    C4_ASSERT(node(m_state)->is_seq());
}

//-----------------------------------------------------------------------------
NodeData* Parser::_append_val(csubstr const& val)
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
    _write_val_anchor(nid);
    return m_tree->get(nid);
}

NodeData* Parser::_append_key_val(csubstr const& val)
{
    C4_ASSERT(node(m_state)->is_map());
    csubstr key = _consume_scalar();
    _c4dbgpf("append keyval: '%.*s' '%.*s' to parent id=%zd (level=%zd)", _c4prsp(key), _c4prsp(val), m_state->node_id, m_state->level);
    size_t nid = m_tree->append_child(m_state->node_id);
    m_tree->to_keyval(nid, key, val);
    _c4dbgpf("append keyval: id=%zd key='%.*s' val='%.*s'", nid, _c4prsp(m_tree->get(nid)->key()), _c4prsp(m_tree->get(nid)->val()));
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
    _write_key_anchor(nid);
    _write_val_anchor(nid);
    return m_tree->get(nid);
}

//-----------------------------------------------------------------------------
void Parser::_store_scalar(csubstr const& s)
{
    _c4dbgpf("state[%zd]: storing scalar '%.*s' (flag: %zd) (old scalar='%.*s')", m_state-m_stack.begin(), _c4prsp(s), m_state->flags & SSCL, _c4prsp(m_state->scalar));
    C4_ASSERT(has_none(SSCL));
    add_flags(SSCL);
    m_state->scalar = s;
}

csubstr Parser::_consume_scalar()
{
    _c4dbgpf("state[%zd]: consuming scalar '%.*s' (flag: %zd))", m_state-m_stack.begin(), _c4prsp(m_state->scalar), m_state->flags & SSCL);
    C4_ASSERT(m_state->flags & SSCL);
    csubstr s = m_state->scalar;
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
    csubstr rem = m_state->line_contents.rem;
    /** @todo instead of trimming, we should suse the indentation index from above */
    csubstr remt = rem.triml(' ');

    if(remt.empty() || remt.begins_with('#')) // this is a blank or comment line
    {
        _line_progressed(rem.size());
        return true;
    }

    if(ind == (size_t)m_state->indref)
    {
        if(has_all(SSCL|RVAL) && ! rem.sub(ind).begins_with('-'))
        {
            if(has_all(RMAP))
            {
                _append_key_val("~");
                addrem_flags(RKEY, RVAL);
            }
            else if(has_all(RSEQ))
            {
                _append_val(_consume_scalar());
                addrem_flags(RNXT, RVAL);
            }
            else
            {
                _c4err("internal error");
            }
        }
        else
        {
            _c4dbgpf("same indentation (%zd) -- nothing to see here", ind);
        }
        _line_progressed(ind);
        return ind > 0;
    }
    else if(ind < (size_t)m_state->indref)
    {
        _c4dbgpf("smaller indentation (%zd < %zd)!!!", ind, m_state->indref);
        if(has_all(RVAL))
        {
            _c4dbgp("there was an empty val -- appending");
            if(has_all(RMAP))
            {
                C4_ASSERT(has_all(SSCL));
                _append_key_val("~");
            }
            else if(has_all(RSEQ))
            {
                C4_ASSERT(has_none(SSCL));
                _append_val("~");
            }
        }
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
            if(/*ind == m_state->indref + 2 && */_is_scalar_next__rmap_val(remt)
               && remt.find(":") == npos
               && remt.find("?") == npos)
            {
                _c4dbgpf("actually it seems a value: '%.*s'", _c4prsp(remt));
            }
            else
            {
                addrem_flags(RKEY, RVAL);
                _start_unk();
                //_move_scalar_from_top();
                _line_progressed(ind);
                _save_indentation();
                return true;
            }
        }
        else if(has_all(RSEQ|RVAL))
        {
            // nothing to do here
        }
        else if(rem.triml(' ').begins_with("#"))
        {
            C4_NEVER_REACH(); // this should have been handled earlier
        }
        else
        {
            _c4err("parse error - indentation should not increase at this point");
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
csubstr Parser::_scan_comment()
{
    csubstr s = m_state->line_contents.rem;
    C4_ASSERT(s.begins_with('#'));
    _line_progressed(s.len);
    // skip the # character
    s = s.sub(1);
    // skip leading whitespace
    s = s.right_of(s.first_not_of(' '), /*include_pos*/true);
    _c4dbgpf("comment was '%.*s'", _c4prsp(s));
    return s;
}

//-----------------------------------------------------------------------------
csubstr Parser::_scan_quoted_scalar(const char q)
{
    C4_ASSERT(q == '\'' || q == '"');

    // quoted scalars can spread over multiple lines!
    // nice explanation here: http://yaml-multiline.info/

    bool needs_filter = false;

    // a span to the end of the file
    const size_t b = m_state->pos.offset;
    substr s = m_buf.sub(b);
    if(s.begins_with(' '))
    {
        s = s.triml(' ');
        _line_progressed((size_t)(s.begin() - m_buf.sub(b).begin()));
    }
    C4_ASSERT(s.begins_with(q));

    // skip the opening quote
    _line_progressed(1);
    s = s.sub(1);

    // find the pos of the matching quote
    size_t pos = npos;
    while( ! _finished_file())
    {
        const csubstr line = m_state->line_contents.rem;
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
                    if(next == '"' || next == '\\')
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
            _c4dbgpf("scanning scalar @ line[%zd]: sofar=\"%.*s\"", m_state->pos.line, _c4prsp(s.sub(0, m_state->pos.offset-b)));
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
        s = s.sub(0, pos-1);
    }

    if(needs_filter)
    {
        csubstr ret;
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
csubstr Parser::_scan_block()
{
    // nice explanation here: http://yaml-multiline.info/
    csubstr s = m_state->line_contents.rem;
    csubstr trimmed = s.triml(" ");
    if(trimmed.str > s.str)
    {
        _c4dbgp("skipping whitespace");
        _line_progressed(trimmed.str - s.str);
        s = trimmed;
    }
    C4_ASSERT(s.begins_with('|') || s.begins_with('>'));

    _c4dbgpf("scanning block: specs=\"%.*s\"", _c4prsp(s));

    // parse the spec
    BlockStyle_e newline = s.begins_with('>') ? BLOCK_FOLD : BLOCK_LITERAL;
    BlockChomp_e chomp = CHOMP_CLIP; // default to clip unless + or - are used
    size_t indentation = npos; // have to find out if no spec is given
    csubstr digits;
    if(s.len > 1)
    {
        csubstr t = s.sub(1);
        C4_ASSERT(t.len >= 1);
        if(t[0] == '-')
        {
            chomp = CHOMP_STRIP;
            t = t.sub(1);
        }
        else if(t[0] == '+')
        {
            chomp = CHOMP_KEEP;
            t = t.sub(1);
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
    substr raw_block(m_buf.data() + m_state->pos.offset, size_t(0));// m_state->line_contents.full.sub(0, 0);
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
csubstr Parser::_filter_plain_scalar(substr s, size_t indentation)
{
    _c4dbgpf("filtering plain scalar: before='%.*s'", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    substr r = _filter_whitespace(s, indentation);

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
                r = r.erase(i+1, 1); // keep only one of consecutive newlines
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
csubstr Parser::_filter_squot_scalar(substr s)
{
    _c4dbgpf("filtering single-quoted scalar: before=\"%.*s\"", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    substr r = _filter_whitespace(s);

    // now another sweep for quotes and newlines
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        //const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == '\'' && (curr == next))
        {
            r = r.erase(i+1, 1); // turn two consecutive single quotes into one
        }
        else if(curr == '\n')
        {
            if(next != '\n')
            {
                r[i] = ' '; // a single unix newline: turn it into a space
            }
            else if(curr == '\n' && next == '\n')
            {
                r = r.erase(i+1, 1); // keep only one of consecutive newlines
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
csubstr Parser::_filter_dquot_scalar(substr s)
{
    _c4dbgpf("filtering double-quoted scalar: before='%.*s'", _c4prsp(s));

    // do a first sweep to clean leading whitespace
    substr r = _filter_whitespace(s);

    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        //const char prev = i   > 0     ? r[i-1] : '\0';
        const char next = i+1 < r.len ? r[i+1] : '\0';
        if(curr == '\\')
        {
            if(next == curr)
            {
                r = r.erase(i+1, 1); // turn two consecutive backslashes into one
            }
            else if(next == '\n')
            {
                r = r.erase(i, 2);  // newlines are escaped with \ -- delete both
            }
            else if(next == '"')
            {
                r = r.erase(i, 1);  // fix escaped double quotes
            }
            else if(next == 'n')
            {
                r = r.erase(i+1, 1);
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
                r = r.erase(i+1, 1); // keep only one of consecutive newlines
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
substr Parser::_filter_whitespace(substr r, size_t indentation, bool leading_whitespace)
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
                csubstr ss = r.sub(i);
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
                r = r.erase(i, num);
            }
            else
            {
                r = r.erase(i, 1);
            }
        }
        // erase \r --- https://stackoverflow.com/questions/1885900
        else if(curr == '\r' && next == '\n') // this is the right order
        {
            r = r.erase(i, 1);
        }
        else if(curr == '\n' && next == '\r') // this is the wrong order
        {
            r = r.erase(i+1, 1);
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
csubstr Parser::_filter_block_scalar(substr s, BlockStyle_e style, BlockChomp_e chomp, size_t indentation)
{
    _c4dbgpf("filtering block: '%.*s'", _c4prsp(s));

    C4_ASSERT(s.ends_with('\n') || s.ends_with('\r'));

    substr r = s;

    if(indentation > 0)
    {
        r = _filter_whitespace(s, indentation, /*leading whitespace*/false);
        C4_ASSERT(r.begins_with(' ', indentation));
        r = r.erase(0, indentation);
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
    //default:
    //    _c4err("unknown chomp style");
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
            substr t = r.sub(0, pos);
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
                        t = t.erase(i+1, 1); // keep only one of consecutive newlines
                    }
                }
            }
            // copy over the trailing newlines
            substr nl = r.sub(pos);
            C4_ASSERT(t.len + nl.len <= r.len);
            for(size_t i = 0; i < nl.len; ++i)
            {
                r[t.len + i] = nl[i];
            }
            // now trim r
            r = r.sub(0, t.len + nl.len);
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
bool Parser::_read_decimal(csubstr const& str, size_t *decimal)
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
size_t Parser::_count_nlines(csubstr src)
{
    size_t n = (src.len > 0);
    while(src.len > 0)
    {
        n += (src.begins_with('\n') || src.begins_with('\r'));
        src = src.sub(1);
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

#ifdef RYML_DBG
    // next line: print the state flags
    {
        del = snprintf(buf + pos, len, "top state: ");
        _wrapbuf();

        del = _prfl(buf + pos, len, m_state->flags);
        _wrapbuf();

        del = snprintf(buf + pos, len, "\n");
        _wrapbuf();
    }
#endif

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
    _prflag(RSET);
#undef _prflag

    return pos;
}

#undef _wrapbuf

} // namespace yml
} // namespace c4

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif
