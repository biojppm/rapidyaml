#ifndef _C4_YML_EMIT_DEF_HPP_
#define _C4_YML_EMIT_DEF_HPP_

#ifndef _C4_YML_EMIT_HPP_
#include "./emit.hpp"
#endif

namespace c4 {
namespace yml {

#define _c4this  (static_cast< Writer      * >(this))
#define _c4cthis (static_cast< Writer const* >(this))

template< class Writer >
subs Emitter< Writer >::emit(NodeRef const& n, bool error_on_excess)
{
    this->_visit(n);
    subs result = _c4this->_get(error_on_excess);
    return result;
}

template< class Writer >
size_t Emitter< Writer >::tell() const
{
    return _c4cthis->m_pos;
}

template< class Writer >
void Emitter< Writer >::seek(size_t p)
{
    _c4this->m_pos = p;
}

template< class Writer >
size_t Emitter< Writer >::_visit(NodeRef const& n, size_t ilevel)
{
    if(n.is_stream())
    {
        ;
    }
    _do_visit(n, ilevel);
    if(n.is_stream())
    {
        _write(csubs("...\n"));
    }
    return _c4this->m_pos;
}

/** @todo this function is too complex. break it down into manageable pieces */
template< class Writer >
void Emitter< Writer >::_do_visit(NodeRef const& n, size_t ilevel, bool indent)
{
    RepC ind{' ', 2 * size_t(indent) * ilevel};

    if(n.is_doc())
    {
        _write(csubs("---\n"));
    }
    else if(n.is_keyval())
    {
        C4_ASSERT(n.has_parent());
        if( ! n.has_anchor())
        {
            _write(ind, n.keysc(), csubs(": "), n.valsc(), '\n');
        }
        else
        {
            _write(ind, n.keysc(), csubs(": "), AnchorScalar(n), '\n');
        }
    }
    else if(n.is_val())
    {
        C4_ASSERT(n.has_parent());
        if( ! n.has_anchor())
        {
            _write(ind, csubs("- "), n.valsc(), '\n');
        }
        else
        {
            _write(ind, csubs("- "), AnchorScalar(n), '\n');
        }
    }
    else if(n.is_container() && ! n.is_root())
    {
        C4_ASSERT(n.parent_is_map() || n.parent_is_seq());
        C4_ASSERT(n.is_map() || n.is_seq());

        if(n.parent_is_seq())
        {
            C4_ASSERT( ! n.has_key());
            _write(ind, csubs("- "));
            if(n.has_val_tag())
            {
                _write(n.val_tag(), ' ');
            }
        }
        else if(n.parent_is_map())
        {
            C4_ASSERT(n.has_key());
            _write(ind, n.keysc(), ':');
            if(n.has_val_tag())
            {
                _write(' ', n.val_tag());
            }
        }
        else
        {
            C4_ERROR("tree error");
        }
        if(n.has_anchor())
        {
            _write(' ', '&', n.anchor());
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
                _write(csubs("[]\n"));
            }
            else if(n.is_map())
            {
                _write(csubs("{}\n"));
            }
        }
    }
    else if(n.is_container() && n.is_root())
    {
        if( ! n.has_children())
        {
            if(n.is_seq())
            {
                _write(csubs("[]\n"));
            }
            else if(n.is_map())
            {
                _write(csubs("{}\n"));
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

template< class Writer >
void Emitter< Writer >::_write_one(AnchorScalar const& sc)
{
    if( ! sc.tag.empty())
    {
        _c4this->_do_write(sc.tag);
        _c4this->_do_write(' ');
    }
    if( ! sc.anchor.empty())
    {
        _c4this->_do_write('&');
        _c4this->_do_write(sc.anchor);
        _c4this->_do_write(' ');
    }

    _write_scalar(sc.scalar);
}

template< class Writer >
void Emitter< Writer >::_write_one(NodeScalar const& sc)
{
    if( ! sc.tag.empty())
    {
        _c4this->_do_write(sc.tag);
        _c4this->_do_write(' ');
    }

    _write_scalar(sc.scalar);
}

template< class Writer >
void Emitter< Writer >::_write_scalar(csubs const& s)
{
    const bool no_dquotes = s.first_of( '"') == npos;
    const bool no_squotes = s.first_of('\'') == npos;
    const bool no_newline = s.first_of('\n') == npos;

    if(no_dquotes && no_squotes && no_newline)
    {
        if( ! s.empty())
        {
            _c4this->_do_write(s);
        }
        else
        {
            _c4this->_do_write("''");
        }
    }
    else
    {
        if(no_squotes && !no_dquotes)
        {
            _c4this->_do_write('\'');
            _c4this->_do_write(s);
            _c4this->_do_write('\'');
        }
        else if(no_dquotes && !no_squotes)
        {
            _c4this->_do_write('"');
            _c4this->_do_write(s);
            _c4this->_do_write('"');
        }
        else
        {
            size_t pos = 0;
            _c4this->_do_write('\'');
            for(size_t i = 0; i < s.len; ++i)
            {
                if(s[i] == '\'' || s[i] == '\n')
                {
                    csubs sub = s.sub(pos, i-pos);
                    pos = i;
                    _c4this->_do_write(sub);
                    _c4this->_do_write(s[i]); // write the character twice
                }
            }
            if(pos < s.len)
            {
                csubs sub = s.sub(pos);
                _c4this->_do_write(sub);
            }
            _c4this->_do_write('\'');
        }
    }
}

#undef _c4this
#undef _c4cthis

} // namespace yml
} // namespace c4

#endif /* _C4_YML_EMIT_DEF_HPP_ */
