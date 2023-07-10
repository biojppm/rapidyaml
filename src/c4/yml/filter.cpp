#include "c4/yml/filter.hpp"

namespace c4 {
namespace yml {

namespace {

struct FilterProcessorInplace
{
    substr subject;
    size_t rpos;
    size_t wpos;
    C4_ALWAYS_INLINE void reset(substr s) noexcept
    {
        subject = s;
        rpos = 0;
        wpos = 0;
    }
    C4_ALWAYS_INLINE void skip(size_t num) noexcept
    {
        rpos += num;
    }
    C4_ALWAYS_INLINE void advance() noexcept
    {
        ++rpos;
        ++wpos;
    }
    C4_ALWAYS_INLINE bool skipped_chars() const noexcept
    {
        return wpos != rpos;
    }
};

struct FilterProcessorSrcDst
{
    csubstr src;
    substr dst;
    size_t rpos;
    size_t wpos;
    C4_ALWAYS_INLINE void reset(csubstr src_, substr dst_) noexcept
    {
        src = src_;
        dst = dst_;
        rpos = 0;
        wpos = 0;
    }
    C4_ALWAYS_INLINE void skip(size_t num) noexcept
    {
        rpos += num;
    }
    C4_ALWAYS_INLINE void advance() noexcept
    {
        dst.str[wpos++] = src.str[rpos++];
    }
    C4_ALWAYS_INLINE bool skipped_chars() const noexcept
    {
        return wpos != rpos;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** @p i is set to the first non whitespace character after the line
 * @return the number of empty lines after the initial position */
size_t _count_following_newlines(csubstr r, size_t *C4_RESTRICT i, size_t indentation)
{
    RYML_ASSERT(r[*i] == '\n');
    size_t numnl_following = 0;
    ++(*i);
    for( ; *i < r.len; ++(*i))
    {
        if(r.str[*i] == '\n')
        {
            ++numnl_following;
            if(indentation) // skip the indentation after the newline
            {
                size_t stop = *i + indentation;
                for( ; *i < r.len; ++(*i))
                {
                    if(r.str[*i] != ' ' && r.str[*i] != '\r')
                        break;
                    RYML_ASSERT(*i < stop);
                }
                C4_UNUSED(stop);
            }
        }
        else if(r.str[*i] == ' ' || r.str[*i] == '\t' || r.str[*i] == '\r')  // skip leading whitespace
            ;
        else
            break;
    }
    return numnl_following;
}

} // namespace


template<bool backslash_is_escape, bool keep_trailing_whitespace>
bool ScalarFilterProcessor::_filter_nl(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos, size_t indentation)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfnl(fmt, ...) _c4dbgpf("filter_nl[{}]: " fmt, *i, __VA_ARGS__)
    #else
    #define _c4dbgfnl(...)
    #endif

    const char curr = r[*i];
    bool replaced = false;

    _RYML_CB_ASSERT(*m_callbacks, indentation != npos);
    _RYML_CB_ASSERT(*m_callbacks, curr == '\n');

    _c4dbgfnl("found newline. sofar=[{}]~~~{}~~~", *pos, m_filter_arena.first(*pos));
    size_t ii = *i;
    size_t numnl_following = _count_following_newlines(r, &ii, indentation);
    if(numnl_following)
    {
        _c4dbgfnl("{} consecutive (empty) lines {} in the middle. totalws={}", 1+numnl_following, ii < r.len ? "in the middle" : "at the end", ii - *i);
        for(size_t j = 0; j < numnl_following; ++j)
            dst.str[(*pos)++] = '\n';
    }
    else
    {
        if(r.first_not_of(" \t", *i+1) != npos)
        {
            dst.str[(*pos)++] = ' ';
            _c4dbgfnl("single newline. convert to space. ii={}/{}. sofar=[{}]~~~{}~~~", ii, r.len, *pos, m_filter_arena.first(*pos));
            replaced = true;
        }
        else
        {
            if C4_IF_CONSTEXPR (keep_trailing_whitespace)
            {
                dst.str[(*pos)++] = ' ';
                _c4dbgfnl("single newline. convert to space. ii={}/{}. sofar=[{}]~~~{}~~~", ii, r.len, *pos, m_filter_arena.first(*pos));
                replaced = true;
            }
            else
            {
                _c4dbgfnl("last newline, everything else is whitespace. ii={}/{}", ii, r.len);
                *i = r.len;
            }
        }
        if C4_IF_CONSTEXPR (backslash_is_escape)
        {
            if(ii < r.len && r.str[ii] == '\\')
            {
                const char next = ii+1 < r.len ? r.str[ii+1] : '\0';
                if(next == ' ' || next == '\t')
                {
                    _c4dbgfnl("extend skip to backslash{}", "");
                    ++ii;
                }
            }
        }
    }
    *i = ii - 1; // correct for the loop increment

    #undef _c4dbgfnl

    return replaced;
}

template<bool keep_trailing_whitespace>
void ScalarFilterProcessor::_filter_ws(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfws(fmt, ...) _c4dbgpf("filt_nl[{}]: " fmt, *i, __VA_ARGS__)
    #else
    #define _c4dbgfws(...)
    #endif

    const char curr = r[*i];
    _c4dbgfws("found whitespace '{}'", _c4prc(curr));
    _RYML_CB_ASSERT(*m_callbacks, curr == ' ' || curr == '\t');

    size_t first = *i > 0 ? r.first_not_of(" \t", *i) : r.first_not_of(' ', *i);
    if(first != npos)
    {
        if(r[first] == '\n' || r[first] == '\r') // skip trailing whitespace
        {
            _c4dbgfws("whitespace is trailing on line. firstnonws='{}'@{}", _c4prc(r[first]), first);
            *i = first - 1; // correct for the loop increment
        }
        else // a legit whitespace
        {
            dst.str[(*pos)++] = curr;
            _c4dbgfws("legit whitespace. sofar=[{}]~~~{}~~~", *pos, m_filter_arena.first(*pos));
        }
    }
    else
    {
        _c4dbgfws("... everything else is trailing whitespace{}", "");
        if C4_IF_CONSTEXPR (keep_trailing_whitespace)
            for(size_t j = *i; j < r.len; ++j)
                dst.str[(*pos)++] = r[j];
        *i = r.len;
    }

    #undef _c4dbgfws
}

substr ScalarFilterProcessor::filter_plain(csubstr scalar, substr dst, size_t indentation, Location const* loc)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfps(...) _c4dbgpf("filt_plain" __VA_ARGS__)
    #else
    #define _c4dbgfps(...)
    #endif

    _RYML_CB_ASSERT(*m_callbacks, dst.len >= scalar.len);

    _c4dbgfps(": before=~~~{}~~~", scalar);

    csubstr r = scalar.triml(" \t");
    size_t pos = 0; // the filtered size
    bool filtered_chars = false;
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r.str[i];
        _c4dbgfps("[{}]: '{}'", i, _c4prc(curr));
        if(curr == ' ' || curr == '\t')
        {
            _filter_ws</*keep_trailing_ws*/false>(r, dst, &i, &pos);
        }
        else if(curr == '\n')
        {
            filtered_chars = _filter_nl</*backslash_is_escape*/false, /*keep_trailing_ws*/false>(r, dst, &i, &pos, indentation);
        }
        else if(curr == '\r')  // skip \r --- https://stackoverflow.com/questions/1885900
        {
            ;
        }
        else
        {
            dst.str[pos++] = r[i];
        }
    }

    _RYML_CB_ASSERT(*m_callbacks, pos <= dst.len);
    if(pos < r.len || filtered_chars)
    {
        r = dst.first(pos);
    }
    _RYML_CB_ASSERT(*m_callbacks, dst.len <= scalar.len);
    _c4dbgfps("#filteredchars={} after=~~~{}~~~", scalar.len - dst.len, r);

    #undef _c4dbgfps
    return dst.first(pos);
}

substr ScalarFilterProcessor::filter_squoted(csubstr scalar, substr dst, Location const* loc)
{
    (void)loc;
    // a debugging scaffold:
    #if 0
    #define _c4dbgfsq(...) _c4dbgpf("filt_squo" __VA_ARGS__)
    #else
    #define _c4dbgfsq(...)
    #endif

    // from the YAML spec for double-quoted scalars:
    // https://yaml.org/spec/1.2-old/spec.html#style/flow/single-quoted

    _RYML_CB_ASSERT(*m_callbacks, dst.len >= scalar.len);

    _c4dbgfsq(": before=~~~{}~~~", s);

    csubstr r = scalar;
    size_t pos = 0; // the filtered size
    bool filtered_chars = false;
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        _c4dbgfsq("[{}]: '{}'", i, _c4prc(curr));
        if(curr == ' ' || curr == '\t')
        {
            _filter_ws</*keep_trailing_ws*/true>(r, dst, &i, &pos);
        }
        else if(curr == '\n')
        {
            filtered_chars = _filter_nl</*backslash_is_escape*/false, /*keep_trailing_ws*/true>(r, dst, &i, &pos, /*indentation*/0);
        }
        else if(curr == '\r')  // skip \r --- https://stackoverflow.com/questions/1885900
        {
            ;
        }
        else if(curr == '\'')
        {
            char next = i+1 < r.len ? r[i+1] : '\0';
            if(next == '\'')
            {
                _c4dbgfsq("[{}]: two consecutive quotes", i);
                filtered_chars = true;
                dst.str[pos++] = '\'';
                ++i;
            }
        }
        else
        {
            dst.str[pos++] = curr;
        }
    }

    _RYML_CB_ASSERT(*m_callbacks, pos <= dst.len);
    if(pos < r.len || filtered_chars)
    {
        r = dst.first(pos);
    }

    _RYML_CB_ASSERT(*m_callbacks, scalar.len >= r.len);
    _c4dbgpf(": #filteredchars={} after=~~~{}~~~", scalar.len - r.len, r);

    #undef _c4dbgfsq
    return dst.first(pos);
}

substr ScalarFilterProcessor::filter_dquoted(csubstr scalar, substr dst, size_t indentation, Location const* loc)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfdq(...) _c4dbgpf("filt_dquo" __VA_ARGS__)
    #else
    #define _c4dbgfdq(...)
    #endif

    _c4dbgfdq(": before=~~~{}~~~", s);

    // from the YAML spec for double-quoted scalars:
    // https://yaml.org/spec/1.2-old/spec.html#style/flow/double-quoted
    //
    // All leading and trailing white space characters are excluded
    // from the content. Each continuation line must therefore contain
    // at least one non-space character. Empty lines, if any, are
    // consumed as part of the line folding.

    _grow_filter_arena(s.len + 2u * s.count('\\'));
    substr r = s;
    size_t pos = 0; // the filtered size
    bool filtered_chars = false;
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r[i];
        _c4dbgfdq("[{}]: '{}'", i, _c4prc(curr));
        if(curr == ' ' || curr == '\t')
        {
            _filter_ws</*keep_trailing_ws*/true>(r, dst, &i, &pos);
        }
        else if(curr == '\n')
        {
            filtered_chars = _filter_nl</*backslash_is_escape*/true, /*keep_trailing_ws*/true>(r, dst, &i, &pos, /*indentation*/0);
        }
        else if(curr == '\r')  // skip \r --- https://stackoverflow.com/questions/1885900
        {
            ;
        }
        else if(curr == '\\')
        {
            char next = i+1 < r.len ? r[i+1] : '\0';
            _c4dbgfdq("[{}]: backslash, next='{}'", i, _c4prc(next));
            filtered_chars = true;
            if(next == '\r')
            {
                if(i+2 < r.len && r[i+2] == '\n')
                {
                    ++i; // newline escaped with \ -- skip both (add only one as i is loop-incremented)
                    next = '\n';
                    _c4dbgfdq("[{}]: was \\r\\n, now next='\\n'", i);
                }
            }
            // remember the loop will also increment i
            if(next == '\n')
            {
                size_t ii = i + 2;
                for( ; ii < r.len; ++ii)
                {
                    if(r.str[ii] == ' ' || r.str[ii] == '\t')  // skip leading whitespace
                        ;
                    else
                        break;
                }
                i += ii - i - 1;
            }
            else if(next == '"' || next == '/'  || next == ' ' || next == '\t') // escapes for json compatibility
            {
                dst.str[pos++] = next;
                ++i;
            }
            else if(next == '\r')
            {
                //++i;
            }
            else if(next == 'n')
            {
                dst.str[pos++] = '\n';
                ++i;
            }
            else if(next == 'r')
            {
                dst.str[pos++] = '\r';
                ++i; // skip
            }
            else if(next == 't')
            {
                dst.str[pos++] = '\t';
                ++i;
            }
            else if(next == '\\')
            {
                dst.str[pos++] = '\\';
                ++i;
            }
            else if(next == 'x') // UTF8
            {
                if(i + 1u + 2u >= r.len)
                    _c4err("\\x requires 2 hex digits");
                uint8_t byteval = {};
                if(!read_hex(r.sub(i + 2u, 2u), &byteval))
                    _c4err("failed to read \\x codepoint");
                dst.str[pos++] = *(char*)&byteval;
                i += 1u + 2u;
            }
            else if(next == 'u') // UTF16
            {
                if(i + 1u + 4u >= r.len)
                    _c4err("\\u requires 4 hex digits");
                char readbuf[8];
                csubstr codepoint = r.sub(i + 2u, 4u);
                uint32_t codepoint_val = {};
                if(!read_hex(codepoint, &codepoint_val))
                    _c4err("failed to parse \\u codepoint");
                size_t numbytes = decode_code_point((uint8_t*)readbuf, sizeof(readbuf), codepoint_val);
                C4_ASSERT(numbytes <= 4);
                memcpy(m_filter_arena.str + pos, readbuf, numbytes);
                pos += numbytes;
                i += 1u + 4u;
            }
            else if(next == 'U') // UTF32
            {
                if(i + 1u + 8u >= r.len)
                    _c4err("\\U requires 8 hex digits");
                char readbuf[8];
                csubstr codepoint = r.sub(i + 2u, 8u);
                uint32_t codepoint_val = {};
                if(!read_hex(codepoint, &codepoint_val))
                    _c4err("failed to parse \\U codepoint");
                size_t numbytes = decode_code_point((uint8_t*)readbuf, sizeof(readbuf), codepoint_val);
                C4_ASSERT(numbytes <= 4);
                memcpy(m_filter_arena.str + pos, readbuf, numbytes);
                pos += numbytes;
                i += 1u + 8u;
            }
            // https://yaml.org/spec/1.2.2/#rule-c-ns-esc-char
            else if(next == '0')
            {
                dst.str[pos++] = '\0';
                ++i;
            }
            else if(next == 'b') // backspace
            {
                dst.str[pos++] = '\b';
                ++i;
            }
            else if(next == 'f') // form feed
            {
                dst.str[pos++] = '\f';
                ++i;
            }
            else if(next == 'a') // bell character
            {
                dst.str[pos++] = '\a';
                ++i;
            }
            else if(next == 'v') // vertical tab
            {
                dst.str[pos++] = '\v';
                ++i;
            }
            else if(next == 'e') // escape character
            {
                dst.str[pos++] = '\x1b';
                ++i;
            }
            else if(next == '_') // unicode non breaking space \u00a0
            {
                // https://www.compart.com/en/unicode/U+00a0
                dst.str[pos++] = _RYML_CHCONST(-0x3e, 0xc2);
                dst.str[pos++] = _RYML_CHCONST(-0x60, 0xa0);
                ++i;
            }
            else if(next == 'N') // unicode next line \u0085
            {
                // https://www.compart.com/en/unicode/U+0085
                dst.str[pos++] = _RYML_CHCONST(-0x3e, 0xc2);
                dst.str[pos++] = _RYML_CHCONST(-0x7b, 0x85);
                ++i;
            }
            else if(next == 'L') // unicode line separator \u2028
            {
                // https://www.utf8-chartable.de/unicode-utf8-table.pl?start=8192&number=1024&names=-&utf8=0x&unicodeinhtml=hex
                dst.str[pos++] = _RYML_CHCONST(-0x1e, 0xe2);
                dst.str[pos++] = _RYML_CHCONST(-0x80, 0x80);
                dst.str[pos++] = _RYML_CHCONST(-0x58, 0xa8);
                ++i;
            }
            else if(next == 'P') // unicode paragraph separator \u2029
            {
                // https://www.utf8-chartable.de/unicode-utf8-table.pl?start=8192&number=1024&names=-&utf8=0x&unicodeinhtml=hex
                dst.str[pos++] = _RYML_CHCONST(-0x1e, 0xe2);
                dst.str[pos++] = _RYML_CHCONST(-0x80, 0x80);
                dst.str[pos++] = _RYML_CHCONST(-0x57, 0xa9);
                ++i;
            }
            _c4dbgfdq("[{}]: backslash...sofar=[{}]~~~{}~~~", i, pos, dst.first(pos));
        }
        else
        {
            dst.str[pos++] = curr;
        }
    }

    _RYML_CB_ASSERT(*m_callbacks, pos <= dst.len);
    if(pos < r.len || filtered_chars)
    {
        r = dst.first(pos);
    }

    _RYML_CB_ASSERT(*m_callbacks, s.len >= r.len);
    _c4dbgpf(": #filteredchars={} after=~~~{}~~~", s.len - r.len, r);

    #undef _c4dbgfdq

    return r;
}

substr ScalarFilterProcessor::filter_block_literal(csubstr scalar, substr dst, size_t indentation, Location const* loc)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfbl(fmt, ...) _c4dbgpf("filt_block" fmt, __VA_ARGS__)
    #else
    #define _c4dbgfbl(...)
    #endif

    _c4dbgfbl(": indentation={} before=[{}]~~~{}~~~", indentation, s.len, s);

    if(chomp != CHOMP_KEEP && s.trim(" \n\r").len == 0u)
    {
        _c4dbgp("filt_block: empty scalar");
        return s.first(0);
    }

    substr r = s;

    _c4dbgp("filt_block: style=literal");
    // trim leading whitespace up to indentation
    {
        size_t numws = r.first_not_of(' ');
        if(numws != npos)
        {
            if(numws > indentation)
                r = r.sub(indentation);
            else
                r = r.sub(numws);
            _c4dbgfbl(": after triml=[{}]~~~{}~~~", r.len, r);
        }
        else
        {
            if(chomp != CHOMP_KEEP || r.len == 0)
            {
                _c4dbgfbl(": all spaces {}, return empty", r.len);
                return r.first(0);
            }
            else
            {
                r[0] = '\n';
                return r.first(1);
            }
        }
    }
    _grow_filter_arena(s.len + 2u);  // use s.len! because we may need to add a newline at the end, so the leading indentation will allow space for that newline
    size_t pos = 0; // the filtered size
    for(size_t i = 0; i < r.len; ++i)
    {
        const char curr = r.str[i];
        _c4dbgfbl("[{}]='{}'  pos={}", i, _c4prc(curr), pos);
        if(curr == '\r')
            continue;
        dst.str[pos++] = curr;
        if(curr == '\n')
        {
            _c4dbgfbl("[{}]: found newline", i);
            // skip indentation on the next line
            csubstr rem = r.sub(i+1);
            size_t first = rem.first_not_of(' ');
            if(first != npos)
            {
                _RYML_CB_ASSERT(*m_callbacks, first < rem.len);
                _RYML_CB_ASSERT(*m_callbacks, i+1+first < r.len);
                _c4dbgfbl("[{}]: {} spaces follow before next nonws character @ [{}]='{}'", i, first, i+1+first, rem.str[first]);
                if(first < indentation)
                {
                    _c4dbgfbl("[{}]: skip {}<{} spaces from indentation", i, first, indentation);
                    i += first;
                }
                else
                {
                    _c4dbgfbl("[{}]: skip {} spaces from indentation", i, indentation);
                    i += indentation;
                }
            }
            else
            {
                _RYML_CB_ASSERT(*m_callbacks, i+1 <= r.len);
                first = rem.len;
                _c4dbgfbl("[{}]: {} spaces to the end", i, first);
                if(first)
                {
                    if(first < indentation)
                    {
                        _c4dbgfbl("[{}]: skip everything", i);
                        --pos;
                        break;
                    }
                    else
                    {
                        _c4dbgfbl("[{}]: skip {} spaces from indentation", i, indentation);
                        i += indentation;
                    }
                }
                else if(i+1 == r.len)
                {
                    if(chomp == CHOMP_STRIP)
                        --pos;
                    break;
                }
            }
        }
    }
    _RYML_CB_ASSERT(*m_callbacks, s.len >= pos);
    _c4dbgfbl(": #filteredchars={} after=~~~{}~~~", s.len - r.len, r);
    bool changed = _apply_chomp(m_filter_arena, &pos, chomp);
    _RYML_CB_ASSERT(*m_callbacks, pos <= dst.len);
    _RYML_CB_ASSERT(*m_callbacks, pos <= s.len);
    if(pos < r.len || changed)
    {
        r = _finish_filter_arena(s, pos); // write into s
    }

    _c4dbgfbl(": final=[{}]~~~{}~~~", r.len, r);

    #undef _c4dbgfbl

    return r;
}

substr ScalarFilterProcessor::filter_block_folded(csubstr scalar, substr dst, size_t indentation, Location const* loc)
{
    // a debugging scaffold:
    #if 0
    #define _c4dbgfbl(fmt, ...) _c4dbgpf("filt_block" fmt, __VA_ARGS__)
    #else
    #define _c4dbgfbl(...)
    #endif

    _c4dbgfbl(": indentation={} before=[{}]~~~{}~~~", indentation, s.len, s);

    if(chomp != CHOMP_KEEP && s.trim(" \n\r").len == 0u)
    {
        _c4dbgp("filt_block: empty scalar");
        return s.first(0);
    }

    substr r = s;
    _c4dbgp("filt_block: style=fold");
    _grow_filter_arena(r.len + 2);
    size_t pos = 0; // the filtered size
    bool filtered_chars = false;
    bool started = false;
    bool is_indented = false;
    size_t i = r.first_not_of(' ');
    _c4dbgfbl(": first non space at {}", i);
    if(i > indentation)
    {
        is_indented = true;
        i = indentation;
    }
    _c4dbgfbl(": start folding at {}, is_indented={}", i, (int)is_indented);
    auto on_change_indentation = [&](size_t numnl_following, size_t last_newl, size_t first_non_whitespace){
        _c4dbgfbl("[{}]: add 1+{} newlines", i, numnl_following);
        for(size_t j = 0; j < 1 + numnl_following; ++j)
            dst.str[pos++] = '\n';
        for(i = last_newl + 1 + indentation; i < first_non_whitespace; ++i)
        {
            if(r.str[i] == '\r')
                continue;
            _c4dbgfbl("[{}]: add '{}'", i, _c4prc(r.str[i]));
            dst.str[pos++] = r.str[i];
        }
        --i;
    };
    for( ; i < r.len; ++i)
    {
        const char curr = r.str[i];
        _c4dbgfbl("[{}]='{}'", i, _c4prc(curr));
        if(curr == '\n')
        {
            filtered_chars = true;
            // skip indentation on the next line, and advance over the next non-indented blank lines as well
            size_t first_non_whitespace;
            size_t numnl_following = (size_t)-1;
            while(r[i] == '\n')
            {
                ++numnl_following;
                csubstr rem = r.sub(i+1);
                size_t first = rem.first_not_of(' ');
                _c4dbgfbl("[{}]: found newline. first={} rem.len={}", i, first, rem.len);
                if(first != npos)
                {
                    first_non_whitespace = first + i+1;
                    while(first_non_whitespace < r.len && r[first_non_whitespace] == '\r')
                        ++first_non_whitespace;
                    _RYML_CB_ASSERT(*m_callbacks, first < rem.len);
                    _RYML_CB_ASSERT(*m_callbacks, i+1+first < r.len);
                    _c4dbgfbl("[{}]: {} spaces follow before next nonws character @ [{}]='{}'", i, first, i+1+first, _c4prc(rem.str[first]));
                    if(first < indentation)
                    {
                        _c4dbgfbl("[{}]: skip {}<{} spaces from indentation", i, first, indentation);
                        i += first;
                    }
                    else
                    {
                        _c4dbgfbl("[{}]: skip {} spaces from indentation", i, indentation);
                        i += indentation;
                        if(first > indentation)
                        {
                            _c4dbgfbl("[{}]: {} further indented than {}, stop newlining", i, first, indentation);
                            goto finished_counting_newlines;
                        }
                    }
                    // prepare the next while loop iteration
                    // by setting i at the next newline after
                    // an empty line
                    if(r[first_non_whitespace] == '\n')
                        i = first_non_whitespace;
                    else
                        goto finished_counting_newlines;
                }
                else
                {
                    _RYML_CB_ASSERT(*m_callbacks, i+1 <= r.len);
                    first = rem.len;
                    first_non_whitespace = first + i+1;
                    if(first)
                    {
                        _c4dbgfbl("[{}]: {} spaces to the end", i, first);
                        if(first < indentation)
                        {
                            _c4dbgfbl("[{}]: skip everything", i);
                            i += first;
                        }
                        else
                        {
                            _c4dbgfbl("[{}]: skip {} spaces from indentation", i, indentation);
                            i += indentation;
                            if(first > indentation)
                            {
                                _c4dbgfbl("[{}]: {} spaces missing. not done yet", i, indentation - first);
                                goto finished_counting_newlines;
                            }
                        }
                    }
                    else // if(i+1 == r.len)
                    {
                        _c4dbgfbl("[{}]: it's the final newline", i);
                        _RYML_CB_ASSERT(*m_callbacks, i+1 == r.len);
                        _RYML_CB_ASSERT(*m_callbacks, rem.len == 0);
                    }
                    goto end_of_scalar;
                }
            }
        end_of_scalar:
            // Write all the trailing newlines. Since we're
            // at the end no folding is needed, so write every
            // newline (add 1).
            _c4dbgfbl("[{}]: add {} trailing newlines", i, 1+numnl_following);
            for(size_t j = 0; j < 1 + numnl_following; ++j)
                dst.str[pos++] = '\n';
            break;
        finished_counting_newlines:
            _c4dbgfbl("[{}]: #newlines={} firstnonws={}", i, numnl_following, first_non_whitespace);
            while(first_non_whitespace < r.len && r[first_non_whitespace] == '\t')
                ++first_non_whitespace;
            _c4dbgfbl("[{}]: #newlines={} firstnonws={}", i, numnl_following, first_non_whitespace);
            _RYML_CB_ASSERT(*m_callbacks, first_non_whitespace <= r.len);
            size_t last_newl = r.last_of('\n', first_non_whitespace);
            size_t this_indentation = first_non_whitespace - last_newl - 1;
            _c4dbgfbl("[{}]: #newlines={} firstnonws={} lastnewl={} this_indentation={} vs indentation={}", i, numnl_following, first_non_whitespace, last_newl, this_indentation, indentation);
            _RYML_CB_ASSERT(*m_callbacks, first_non_whitespace >= last_newl + 1);
            _RYML_CB_ASSERT(*m_callbacks, this_indentation >= indentation);
            if(!started)
            {
                _c4dbgfbl("[{}]: #newlines={}. write all leading newlines", i, numnl_following);
                for(size_t j = 0; j < 1 + numnl_following; ++j)
                    dst.str[pos++] = '\n';
                if(this_indentation > indentation)
                {
                    is_indented = true;
                    _c4dbgfbl("[{}]: advance ->{}", i, last_newl + indentation);
                    i = last_newl + indentation;
                }
                else
                {
                    i = first_non_whitespace - 1;
                    _c4dbgfbl("[{}]: advance ->{}", i, first_non_whitespace);
                }
            }
            else if(this_indentation == indentation)
            {
                _c4dbgfbl("[{}]: same indentation", i);
                if(!is_indented)
                {
                    if(numnl_following == 0)
                    {
                        _c4dbgfbl("[{}]: fold!", i);
                        dst.str[pos++] = ' ';
                    }
                    else
                    {
                        _c4dbgfbl("[{}]: add {} newlines", i, 1 + numnl_following);
                        for(size_t j = 0; j < numnl_following; ++j)
                            dst.str[pos++] = '\n';
                    }
                    i = first_non_whitespace - 1;
                    _c4dbgfbl("[{}]: advance {}->{}", i, i, first_non_whitespace);
                }
                else
                {
                    _c4dbgfbl("[{}]: back to ref indentation", i);
                    is_indented = false;
                    on_change_indentation(numnl_following, last_newl, first_non_whitespace);
                    _c4dbgfbl("[{}]: advance {}->{}", i, i, first_non_whitespace);
                }
            }
            else
            {
                _c4dbgfbl("[{}]: increased indentation.", i);
                is_indented = true;
                _RYML_CB_ASSERT(*m_callbacks, this_indentation > indentation);
                on_change_indentation(numnl_following, last_newl, first_non_whitespace);
                _c4dbgfbl("[{}]: advance {}->{}", i, i, first_non_whitespace);
            }
        }
        else if(curr != '\r')
        {
            if(curr != '\t')
                started = true;
            dst.str[pos++] = curr;
        }
    }
    _RYML_CB_ASSERT(*m_callbacks, pos <= dst.len);
    _c4dbgfbl(": #filteredchars={} after=[{}]~~~{}~~~", (int)s.len - (int)pos, pos, dst.first(pos));
    bool changed = _apply_chomp(m_filter_arena, &pos, chomp);
    if(pos < r.len || filtered_chars || changed)
    {
        r = dst.first(pos); // write into s
    }

    _c4dbgfbl(": final=[{}]~~~{}~~~", r.len, r);

    #undef _c4dbgfbl

    return r.first(pos);
}


} // namespace yml
} // namespace c4
