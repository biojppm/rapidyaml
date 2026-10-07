#ifndef C4_YML_DETAIL_EMIT_SCALAR_HPP_
#define C4_YML_DETAIL_EMIT_SCALAR_HPP_

#ifndef C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif
#ifndef C4_YML_SCALAR_CHARCONV_HPP_
#include "c4/yml/scalar_charconv.hpp"
#endif

namespace c4 {
namespace yml {
namespace detail {


template<class Emitter>
C4_NODISCARD size_t emit_escaped_newlines_(Emitter *em, csubstr s, size_t i)
{
    RYML_ASSERT_BASIC_(s.len > i);
    RYML_ASSERT_BASIC_(s.str[i] == '\n');
    //_c4dbgpf("nl@i={} rem=[{}]~~~{}~~~", i, s.sub(i).len, s.sub(i));
    // add an extra newline for each sequence of consecutive
    // newline/whitespace
    em->newl_();
    do
    {
        em->newl_(); // write the newline again
        ++i; // increase the outer loop counter!
    } while(i < s.len && s.str[i] == '\n');
    RYML_ASSERT_BASIC_(i > 0);
    --i;
    RYML_ASSERT_BASIC_(s.str[i] == '\n');
    return i;
}


template<class Emitter>
void emit_scalar_plain_(Emitter *em, csubstr s, typename Emitter::indent_type ilevel)
{
    if C4_UNLIKELY(ilevel == 0 && (s.begins_with("...") || s.begins_with("---")))
    {
        em->indent_(ilevel + 1); // indent the next line
        ++ilevel;
    }
    size_t pos = 0; // tracks the last character that was already written
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        if(curr == '\n')
        {
            csubstr sub = s.range(pos, i);
            em->write_(sub);  // write everything up to (including) this newline
            i = detail::emit_escaped_newlines_(em, s, i);
            pos = i+1;
            if(pos < s.len)
                em->indent_(ilevel + 1); // indent the next line
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        em->write_(s.sub(pos));
}


template<class Emitter>
void emit_scalar_dquo_(Emitter *em, csubstr s)
{
    size_t pos = 0; // tracks the last character that was already written
    em->write_('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        switch(curr) // NOLINT
        {
        case '"':
        case '\\':
        {
            csubstr sub = s.range(pos, i);
            em->write_(sub);  // write everything up to (excluding) this char
            em->write_('\\'); // write the escape
            em->write_(curr); // write the char
            pos = i+1;
            break;
        }
        case '\n':
        {
            csubstr sub = s.range(pos, i);
            em->write_(sub);   // write everything up to (excluding) this char
            em->write_("\\n"); // write the escape
            pos = i+1;
            break;
        }
        case '\r':
        {
            csubstr sub = s.range(pos, i);
            em->write_(sub);  // write everything up to (excluding) this char
            em->write_("\\r"); // write the escaped char
            pos = i+1;
            break;
        }
        case '\b':
        {
            csubstr sub = s.range(pos, i);
            em->write_(sub);  // write everything up to (excluding) this char
            em->write_("\\b"); // write the escaped char
            pos = i+1;
            break;
        }
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        em->write_(s.sub(pos));
    em->write_('"');
}


template<class Emitter>
void emit_scalar_squo_(Emitter *em, csubstr s, typename Emitter::indent_type ilevel)
{
    size_t pos = 0; // tracks the last character that was already written
    em->write_('\'');
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s[i] == '\n')
        {
            em->write_(s.range(pos, i));  // write everything up to (excluding) this char
            //_c4dbgpf("newline at {}. writing ~~~{}~~~", i, s.range(pos, i));
            i = detail::emit_escaped_newlines_(em, s, i);
            //_c4dbgpf("newline --> {}", i);
            if(i < s.len)
                em->indent_(ilevel + 1);
            pos = i+1;
        }
        else if(s[i] == '\'')
        {
            csubstr sub = s.range(pos, i+1);
            //_c4dbgpf("squote at {}. writing ~~~{}~~~", i, sub);
            em->write_(sub); // write everything up to (including) this squote
            em->write_('\''); // write the squote again
            pos = i+1;
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        em->write_(s.sub(pos));
    em->write_('\'');
}



inline C4_NODISCARD bool _is_indented_block(csubstr s, size_t prev, size_t i) noexcept
{
    if(prev == 0 && s.begins_with_any(" \t"))
        return true;
    const size_t pos = s.first_not_of('\n', i);
    return (pos != npos) && (s.str[pos] == ' ' || s.str[pos] == '\t');
}


template<class Emitter>
C4_NODISCARD size_t emit_indented_block_(Emitter *em, csubstr s, size_t i, typename Emitter::indent_type ilevel)
{
    //_c4dbgpf("indblock@i={} rem=[{}]~~~\n{}~~~", i, s.sub(i).len, s.sub(i));
    RYML_ASSERT_BASIC_(i > 0);
    RYML_ASSERT_BASIC_(s.str[i-1] == '\n');
    RYML_ASSERT_BASIC_(i < s.len);
    RYML_ASSERT_BASIC_(s.str[i] == ' ' || s.str[i] == '\t' || s.str[i] == '\n');
again:
    size_t pos = s.find("\n ", i);
    if(pos == npos)
        pos = s.find("\n\t", i);
    if(pos != npos)
    {
        ++pos;
        //_c4dbgpf("indblock line@i={} rem=[{}]~~~\n{}~~~", i, s.range(i, pos).len, s.range(i, pos));
        em->indent_(ilevel + 1);
        em->write_(s.range(i, pos));
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
        em->indent_(ilevel + 1);
        em->write_(s.range(i, pos));
        i = pos;
    }
    return i;
}


template<class Emitter>
void emit_scalar_literal_(Emitter *em, csubstr s, typename Emitter::indent_type ilevel)
{
    RYML_ASSERT_BASIC_(s.find("\r") == csubstr::npos);
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    em->write_('|');
    if(explicit_indentation)
        em->write_('2');
    //
    if(numnewlines_at_end > 1 || is_newline_only)
        em->write_('+');
    else if(numnewlines_at_end == 0)
        em->write_('-');
    //
    if(trimmed.len)
    {
        em->newl_();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // write everything up to this point
            csubstr since_pos = trimmed.range(pos, i+1); // include the newline
            em->indent_(ilevel + 1);
            em->write_(since_pos);
            pos = i+1; // already written
        }
        if(pos < trimmed.len)
        {
            em->indent_(ilevel + 1);
            em->write_(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        em->newl_();
}


template<class Emitter>
void emit_scalar_folded_(Emitter *em, csubstr s, typename Emitter::indent_type ilevel)
{
    RYML_ASSERT_BASIC_(s.find("\r") == csubstr::npos);
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    em->write_('>');
    if(explicit_indentation)
        em->write_('2');
    //
    if(numnewlines_at_end == 0)
        em->write_('-');
    else if(numnewlines_at_end > 1 || is_newline_only)
        em->write_('+');
    //
    if(trimmed.len)
    {
        em->newl_();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // escape newline sequences
            if( ! detail::_is_indented_block(s, pos, i))
            {
                if(pos < i)
                {
                    em->indent_(ilevel + 1);
                    em->write_(s.range(pos, i));
                    i = detail::emit_escaped_newlines_(em, s, i);
                    pos = i + 1;
                }
                else
                {
                    if(i+1 < s.len)
                    {
                        if(s.str[i+1] == '\n')
                        {
                            ++i;
                            i = detail::emit_escaped_newlines_(em, s, i);
                            pos = i+1;
                        }
                        else
                        {
                            em->newl_();
                            pos = i+1;
                        }
                    }
                }
            }
            else // do not escape newlines in indented blocks
            {
                ++i;
                em->indent_(ilevel + 1);
                em->write_(s.range(pos, i));
                if(pos > 0 || !s.begins_with_any(" \t"))
                    i = detail::emit_indented_block_(em, s, i, ilevel);
                pos = i;
            }
        }
        if(pos < trimmed.len)
        {
            em->indent_(ilevel + 1);
            em->write_(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        em->newl_();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class Emitter>
C4_NODISCARD bool emit_json_maybe_write_naninf_(Emitter *em, csubstr s)
{
    switch(s.len)
    {
    case 3: case 4: case 5: // inf, nan, .nan, -.inf
    case 8: case 9: // infinity, -infinity
        break;
    default:
        return false;
    }
    const char first = s.str[0];
    csubstr rest = s.sub(1);
    if(s.len == 4 && first == '.')
    {
        if(scalar_is_inf3(rest.str))
            goto write_inf_positive; // NOLINT
        else if(scalar_is_nan3(rest.str))
            goto write_nan; // NOLINT
    }
    else if(first == '-' || first == '+') // begins with sign: must be inf
    {
        // match [-+].inf
        if((rest.len == 4 && rest.str[0] == '.' && scalar_is_inf3(rest.str + 1))
           // match [-+]inf
           || (rest.len == 3 && scalar_is_inf3(rest.str))
           // match [-+]infinity
           || (rest.len == 8 && (0 == memcmp(rest.str, "infinity", 8))))
        {
            if(first == '-')
                goto write_inf_negative; // NOLINT
            else
                goto write_inf_positive; // NOLINT
        }
    }
    else if(s.len == 8 && (0 == memcmp(s.str, "infinity", 8)))
    {
        goto write_inf_positive; // NOLINT
    }
    else if(s.len == 3)
    {
        if(scalar_is_inf3(s.str))
            goto write_inf_positive; // NOLINT
        else if(scalar_is_nan3(s.str))
            goto write_nan; // NOLINT
    }
    return false;
write_inf_positive:
    em->write_("\".inf\"");
    return true;
write_inf_negative:
    em->write_("\"-.inf\"");
    return true;
write_nan:
    em->write_("\".nan\"");
    return true;
}


template<class Emitter>
void emit_json_write_scalar_dquo_(Emitter *em, csubstr s)
{
    size_t pos = 0;
    em->write_('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        switch(s.str[i])
        {
        case '"':
            em->write_(s.range(pos, i));
            em->write_("\\\"");
            pos = i + 1;
            break;
        case '\n':
            em->write_(s.range(pos, i));
            em->write_("\\n");
            pos = i + 1;
            break;
        case '\t':
            em->write_(s.range(pos, i));
            em->write_("\\t");
            pos = i + 1;
            break;
        case '\\':
            em->write_(s.range(pos, i));
            em->write_("\\\\");
            pos = i + 1;
            break;
        case '\r':
            em->write_(s.range(pos, i));
            em->write_("\\r");
            pos = i + 1;
            break;
        case '\b':
            em->write_(s.range(pos, i));
            em->write_("\\b");
            pos = i + 1;
            break;
        case '\f':
            em->write_(s.range(pos, i));
            em->write_("\\f");
            pos = i + 1;
            break;
        }
    }
    if(pos < s.len)
    {
        csubstr sub = s.sub(pos);
        em->write_(sub);
    }
    em->write_('"');
}


template<class Emitter>
void emit_json_write_number_(Emitter *em, csubstr s)
{
    if(s.is_integer())
    {
        em->write_(s);
    }
    else
    {
        if(s.begins_with('-') && s.len > 1)
        {
            csubstr rest = s.sub(1);
            if(rest.begins_with('.'))
            {
                em->write_("-0");
                em->write_(rest);
            }
            else if(rest.ends_with('.'))
            {
                em->write_(s);
                em->write_('0');
            }
            else
            {
                em->write_(s);
            }
        }
        else if(s.begins_with('.'))
        {
            em->write_('0');
            em->write_(s);
        }
        else if(s.ends_with('.'))
        {
            em->write_(s);
            em->write_('0');
        }
        else
        {
            em->write_(s);
        }
    }
}


} // namespace detail
} // namespace yml
} // namespace c4

#endif /* C4_YML_DETAIL_EMIT_SCALAR_HPP_ */
