#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "./emit.hpp"
#endif

namespace c4 {
namespace yml {

template<class Writer>
substr Emitter<Writer>::emit(Tree const& t, size_t id, bool error_on_excess)
{
    this->_visit(t, id);
    substr result = this->Writer::_get(error_on_excess);
    return result;
}

template<class Writer>
size_t Emitter<Writer>::tell() const
{
    return this->Writer::m_pos;
}

template<class Writer>
void Emitter<Writer>::seek(size_t p)
{
    this->Writer::m_pos = p;
}

template<class Writer>
size_t Emitter<Writer>::_visit(Tree const& t, size_t id, size_t ilevel)
{
    if(t.is_stream(id))
    {
        ;
    }
    _do_visit(t, id, ilevel);
    if(t.is_stream(id))
    {
        _write("...\n");
    }
    return this->Writer::m_pos;
}

/** @todo this function is too complex. break it down into manageable pieces */
template<class Writer>
void Emitter<Writer>::_do_visit(Tree const& t, size_t id, size_t ilevel, bool indent)
{
    RepC ind{' ', 2 * size_t(indent) * ilevel};

    if(t.is_doc(id))
    {
        _write("---");
        bool nl = false;
        if(t.has_val_tag(id))
        {
            if( ! nl) _write(' ');
            _write(t.val_tag(id));
            nl = true;
        }
        if(t.has_val_anchor(id))
        {
            if( ! nl) _write(' ');
            _write('&');
            _write(t.val_anchor(id));
            nl = true;
        }
        _write('\n');
    }
    else if(t.is_keyval(id))
    {
        C4_ASSERT(t.has_parent(id));
        _write(ind);
        _writek(t, id);
        _write(": ");
        _writev(t, id);
        _write('\n');
    }
    else if(t.is_val(id))
    {
        C4_ASSERT(t.has_parent(id));
        _write(ind);
        _write("- ");
        _writev(t, id);
        _write('\n');
    }
    else if(t.is_container(id))
    {
        if(C4_LIKELY( ! t.is_root(id)))
        {
            C4_ASSERT(t.parent_is_map(id) || t.parent_is_seq(id));
            C4_ASSERT(t.is_map(id) || t.is_seq(id));

            if(t.has_key(id))
            {
                C4_ASSERT(t.parent_is_map(id));
                _write(ind);
                _writek(t, id);
                _write(':');
            }
            else
            {
                C4_ASSERT(t.parent_is_seq(id));
                _write(ind);
                _write('-');
            }

            if(t.has_val_tag(id))
            {
                _write(' ');
                _write(t.val_tag(id));
            }
            if(t.has_val_anchor(id))
            {
                _write(' ');
                _write('&');
                _write(t.val_anchor(id));
            }
            
            indent = true;
            if(t.has_children(id))
            {
                _write('\n');
            }
            else
            {
                if(t.is_seq(id))
                {
                    _write(" []\n");
                }
                else if(t.is_map(id))
                {
                    _write(" {}\n");
                }
            }
        } // !root
        else // root // @todo this branch should not be needed
        {
            C4_ASSERT(t.is_root(id));
            bool nl = false;
            if(t.has_val_tag(id))
            {
                _write(t.val_tag(id));
                nl = true;
            }
            if(t.has_val_anchor(id))
            {
                _write('&');
                _write(t.val_anchor(id));
                nl = true;
            }

            if( ! t.has_children(id))
            {
                C4_ASSERT(t.is_seq(id) || t.is_map(id));
                if(t.is_seq(id))
                {
                    if(nl) _write(' ');
                    _write("[]");
                    nl = true;
                }
                else if(t.is_map(id))
                {
                    if(nl) _write(' ');
                    _write("{}");
                    nl = true;
                }
            }

            if(nl)
            {
                _write('\n');
            }
        } // root
    } // container

    size_t next_level = ilevel + 1;
    if(t.is_stream(id) || t.is_doc(id) || t.is_root(id))
    {
        next_level = ilevel; // do not indent at top level
    }

    for(size_t ich = t.first_child(id); ich != NONE; ich = t.next_sibling(ich))
    {
        _do_visit(t, ich, next_level, indent);
        indent = true;
    }
}

template<class Writer>
void Emitter<Writer>::_write(NodeScalar const& sc, NodeType flags)
{
    if( ! sc.tag.empty())
    {
        this->Writer::_do_write(sc.tag);
        this->Writer::_do_write(' ');
    }
    if(flags.has_anchor())
    {
        C4_ASSERT(flags.is_ref() != flags.has_anchor());
        C4_ASSERT( ! sc.anchor.empty());
        this->Writer::_do_write('&');
        this->Writer::_do_write(sc.anchor);
        this->Writer::_do_write(' ');
    }

    _write_scalar(sc.scalar);
}

template<class Writer>
void Emitter<Writer>::_write_scalar(csubstr s)
{
    const bool no_dquotes = s.first_of( '"') == npos;
    const bool no_squotes = s.first_of('\'') == npos;
    // force use quotes when any of these characters is present
    const bool no_special = s.first_of("#:-,\n{}[]") == npos;

    if(no_dquotes && no_squotes && no_special)
    {
        if( ! s.empty())
        {
            this->Writer::_do_write(s);
        }
        else
        {
            this->Writer::_do_write("''");
        }
    }
    else
    {
        if(no_squotes && !no_dquotes)
        {
            this->Writer::_do_write('\'');
            this->Writer::_do_write(s);
            this->Writer::_do_write('\'');
        }
        else if(no_dquotes && !no_squotes)
        {
            this->Writer::_do_write('"');
            this->Writer::_do_write(s);
            this->Writer::_do_write('"');
        }
        else
        {
            size_t pos = 0;
            this->Writer::_do_write('\'');
            for(size_t i = 0; i < s.len; ++i)
            {
                if(s[i] == '\'' || s[i] == '\n')
                {
                    csubstr sub = s.sub(pos, i-pos);
                    pos = i;
                    this->Writer::_do_write(sub);
                    this->Writer::_do_write(s[i]); // write the character twice
                }
            }
            if(pos < s.len)
            {
                csubstr sub = s.sub(pos);
                this->Writer::_do_write(sub);
            }
            this->Writer::_do_write('\'');
        }
    }
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_EMIT_DEF_HPP_ */
