#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "./emit.hpp"
#endif

namespace c4 {
namespace yml {

template<class Writer>
substr Emitter<Writer>::emit(NodeRef const& n, bool error_on_excess)
{
    this->_visit(n);
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
size_t Emitter<Writer>::_visit(NodeRef const& n, size_t ilevel)
{
    if(n.is_stream())
    {
        ;
    }
    _do_visit(n, ilevel);
    if(n.is_stream())
    {
        _write("...\n");
    }
    return this->Writer::m_pos;
}

/** @todo this function is too complex. break it down into manageable pieces */
template<class Writer>
void Emitter<Writer>::_do_visit(NodeRef const& n, size_t ilevel, bool indent)
{
    RepC ind{' ', 2 * size_t(indent) * ilevel};

    if(n.is_doc())
    {
        _write("---\n");
    }
    else if(n.is_keyval())
    {
        C4_ASSERT(n.has_parent());
        _write(ind); _writek(n); _write(": "); _writev(n); _write('\n');
    }
    else if(n.is_val())
    {
        C4_ASSERT(n.has_parent());
        _write(ind); _write("- "); _writev(n); _write('\n');
    }
    else if(n.is_container() && ! n.is_root())
    {
        C4_ASSERT(n.parent_is_map() || n.parent_is_seq());
        C4_ASSERT(n.is_map() || n.is_seq());

        if(n.parent_is_seq())
        {
            C4_ASSERT( ! n.has_key());
            _write(ind); _write("- ");
            if(n.has_val_tag())
            {
                _write(n.val_tag()); _write(' ');
            }
        }
        else if(n.parent_is_map())
        {
            C4_ASSERT(n.has_key());
            _write(ind); _writek(n); _write(':');
            if(n.has_val_tag())
            {
                _write(' '); _write(n.val_tag());
            }
        }
        else
        {
            C4_ERROR("tree error");
        }
        if(n.has_val_anchor())
        {
            _write(" &"); _write(n.val_anchor());
        }

        if(n.has_children())
        {
            if(n.is_seq())
            {
                if(n.parent_is_map())
                {
                    _write('\n');
                    indent = true;
                }
                else
                {
                    // do not indent the first child, as it will be written on the same line
                    indent = false;
                }
            }
            else if(n.is_map())
            {
                if(n.parent_is_seq())
                {
                    // do not indent the first child, as it will be written on the same line
                    indent = false;
                }
                else
                {
                    _write('\n');
                    indent = true;
                }
            }
            else
            {
                C4_ERROR("invalid node");
            }
        }
        else
        {
            if(n.parent_is_map())
            {
                _write(' ');
            }
            if(n.is_seq())
            {
                _write("[]\n");
            }
            else if(n.is_map())
            {
                _write("{}\n");
            }
        }
    }
    else if(n.is_container() && n.is_root())
    {
        if( ! n.has_children())
        {
            if(n.is_seq())
            {
                _write("[]\n");
            }
            else if(n.is_map())
            {
                _write("{}\n");
            }
        }
    }

    size_t next_level = ilevel + 1;
    if(n.is_stream() || n.is_doc() || n.is_root())
    {
        next_level = ilevel; // do not indent at top level
    }

    for(auto ch : n.children())
    {
        _do_visit(ch, next_level, indent);
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
    const bool no_newline = s.first_of('\n') == npos;

    if(no_dquotes && no_squotes && no_newline)
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
