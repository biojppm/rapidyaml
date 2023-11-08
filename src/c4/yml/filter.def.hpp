#ifndef C4_YML_FILTER_DEF_HPP_
#define C4_YML_FILTER_DEF_HPP_

#include "c4/yml/filter.hpp"
#include "c4/charconv.hpp"
#include "c4/utf.hpp"
#include "c4/yml/detail/parser_dbg.hpp"
#include "c4/yml/filter_processor.hpp"

C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

// see https://blogs.perl.org/users/tinita/2018/03/strings-in-yaml---to-quote-or-not-to-quote.html

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline size_t _count_following_newlines(csubstr r, size_t *C4_RESTRICT i)
{
    RYML_ASSERT(r[*i] == '\n');
    size_t numnl_following = 0;
    ++(*i);
    for( ; *i < r.len; ++(*i))
    {
        if(r.str[*i] == '\n')
            ++numnl_following;
        // skip leading whitespace
        else if(r.str[*i] == ' ' || r.str[*i] == '\t' || r.str[*i] == '\r')
            ;
        else
            break;
    }
    return numnl_following;
}

/** @p i is set to the first non whitespace character after the line
 * @return the number of empty lines after the initial position */
inline size_t _count_following_newlines(csubstr r, size_t *C4_RESTRICT i, size_t indentation)
{
    RYML_ASSERT(r[*i] == '\n');
    size_t numnl_following = 0;
    ++(*i);
    if(indentation == 0)
    {
        for( ; *i < r.len; ++(*i))
        {
            if(r.str[*i] == '\n')
                ++numnl_following;
            // skip leading whitespace
            else if(r.str[*i] == ' ' || r.str[*i] == '\t' || r.str[*i] == '\r')
                ;
            else
                break;
        }
    }
    else
    {
        for( ; *i < r.len; ++(*i))
        {
            if(r.str[*i] == '\n')
            {
                ++numnl_following;
                // skip the indentation after the newline
                size_t stop = *i + indentation;
                for( ; *i < r.len; ++(*i))
                {
                    if(r.str[*i] != ' ' && r.str[*i] != '\r')
                        break;
                    RYML_ASSERT(*i < stop);
                }
                C4_UNUSED(stop);
            }
            // skip leading whitespace
            else if(r.str[*i] == ' ' || r.str[*i] == '\t' || r.str[*i] == '\r')
                ;
            else
                break;
        }
    }
    return numnl_following;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// a debugging scaffold:
#if 0
#define _c4dbgfws(fmt, ...) _c4dbgpf("filt_ws[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfws(...)
#endif

template<class Parser>
template<class FilterProcessor>
bool ScalarFilterCRTP<Parser>::_filter_ws_handle_to_first_non_space(FilterProcessor &proc) noexcept
{
    _c4dbgfws("found whitespace '{}'", _c4prc(proc.curr()));
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.curr() == ' ' || proc.curr() == '\t');

    const size_t first_pos = proc.rpos > 0 ? proc.src.first_not_of(" \t", proc.rpos) : proc.src.first_not_of(' ', proc.rpos);
    if(first_pos != npos)
    {
        const char first_char = proc.src[first_pos];
        _c4dbgfws("firstnonws='{}'@{}", _c4prc(first_char), first_pos);
        if(first_char == '\n' || first_char == '\r') // skip trailing whitespace
        {
            _c4dbgfws("whitespace is trailing on line", "");
            proc.skip(first_pos - proc.rpos);
        }
        else // a legit whitespace
        {
            proc.copy();
            _c4dbgfws("legit whitespace. sofar=[{}]~~~{}~~~", proc.wpos, proc.sofar());
        }
        return true;
    }
    else
    {
        return false;
    }
}

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_ws_copy_trailing(FilterProcessor &proc) noexcept
{
    if(!_filter_ws_handle_to_first_non_space(proc))
    {
        _c4dbgfws("... everything else is trailing whitespace - copy {} chars", proc.src.len - proc.rpos);
        proc.copy(proc.src.len - proc.rpos);
    }
}

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_ws_skip_trailing(FilterProcessor &proc) noexcept
{
    if(!_filter_ws_handle_to_first_non_space(proc))
    {
        _c4dbgfws("... everything else is trailing whitespace - skip {} chars", proc.src.len - proc.rpos);
        proc.skip(proc.src.len - proc.rpos);
    }
}

#undef _c4dbgfws


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* plain scalars */

// a debugging scaffold:
#if 0
#define _c4dbgfps(fmt, ...) _c4dbgpf("filt_plain[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfps(fmt, ...)
#endif

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_nl_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.curr() == '\n');

    _c4dbgfps("found newline. sofar=[{}]~~~{}~~~", proc.wpos, proc.sofar());
    size_t ii = proc.rpos;
    const size_t numnl_following = _count_following_newlines(proc.src, &ii, indentation);
    if(numnl_following)
    {
        proc.set('\n', numnl_following);
        _c4dbgfps("{} consecutive (empty) lines {}. totalws={}", 1+numnl_following, ii < proc.src.len ? "in the middle" : "at the end", proc.rpos-ii);
    }
    else
    {
        const size_t ret = proc.src.first_not_of(" \t", proc.rpos+1);
        if(ret != npos)
        {
            proc.set(' ');
             _c4dbgfps("single newline. convert to space. ret={}/{}. sofar=[{}]~~~{}~~~", ii, proc.src.len, proc.wpos, proc.sofar());
        }
        else
        {
            _c4dbgfps("last newline, everything else is whitespace. ii={}/{}", ii, proc.src.len);
            ii = proc.src.len;
        }
    }
    proc.rpos = ii;
}

template<class Parser>
template<class FilterProcessor>
FilterResult ScalarFilterCRTP<Parser>::_filter_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), indentation != npos);
    _c4dbgfps("before=[{}]~~~{}~~~", proc.src.len, proc.src);

    while(proc.has_more_chars())
    {
        const char curr = proc.curr();
        _c4dbgfps("'{}', sofar=[{}]~~~{}~~~", _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case ' ':
        case '\t':
            _c4dbgfps("whitespace", curr);
            _filter_ws_skip_trailing(proc);
            break;
        case '\n':
            _c4dbgfps("newline", curr);
            _filter_nl_plain(proc, /*indentation*/indentation);
            break;
        case '\r':  // skip \r --- https://stackoverflow.com/questions/1885900
            _c4dbgfps("carriage return, ignore", curr);
            proc.skip();
            break;
        default:
            proc.copy();
            break;
        }
    }

    _c4dbgfps("after[{}]=~~~{}~~~", proc.wpos, proc.sofar());

    return proc.result();
}

#undef _c4dbgfps


template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_plain(csubstr scalar, substr dst, size_t indentation) noexcept
{
    FilterProcessorSrcDst proc(scalar, dst);
    return _filter_plain(proc, indentation);
}

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_plain_inplace(substr dst, size_t cap, size_t indentation) noexcept
{
    FilterProcessorInplace proc(dst, cap);
    return _filter_plain(proc, indentation);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* single quoted */

// a debugging scaffold:
#if 0
#define _c4dbgfsq(fmt, ...) _c4dbgpf("filt_squo[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfsq(fmt, ...)
#endif

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_nl_squoted(FilterProcessor &C4_RESTRICT proc) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.curr() == '\n');

    _c4dbgfsq("found newline. sofar=[{}]~~~{}~~~", proc.wpos, proc.sofar());
    size_t ii = proc.rpos;
    const size_t numnl_following = _count_following_newlines(proc.src, &ii);
    if(numnl_following)
    {
        proc.set('\n', numnl_following);
        _c4dbgfsq("{} consecutive (empty) lines {}. totalws={}", 1+numnl_following, ii < proc.src.len ? "in the middle" : "at the end", proc.rpos-ii);
    }
    else
    {
        const size_t ret = proc.src.first_not_of(" \t", proc.rpos+1);
        if(ret != npos)
        {
            proc.set(' ');
            _c4dbgfsq("single newline. convert to space. ret={}/{}. sofar=[{}]~~~{}~~~", ii, proc.src.len, proc.wpos, proc.sofar());
        }
        else
        {
            proc.set(' ');
            _c4dbgfsq("single newline. convert to space. ii={}/{}. sofar=[{}]~~~{}~~~", ii, proc.src.len, proc.wpos, proc.sofar());
        }
    }
    proc.rpos = ii;
}

template<class Parser>
template<class FilterProcessor>
FilterResult ScalarFilterCRTP<Parser>::_filter_squoted(FilterProcessor &C4_RESTRICT proc) noexcept
{
    // from the YAML spec for double-quoted scalars:
    // https://yaml.org/spec/1.2-old/spec.html#style/flow/single-quoted

    _c4dbgfsq("before=[{}]~~~{}~~~", proc.src.len, proc.src);

    while(proc.has_more_chars())
    {
        const char curr = proc.curr();
        _c4dbgfsq("'{}', sofar=[{}]~~~{}~~~", _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case ' ':
        case '\t':
            _c4dbgfsq("whitespace", curr);
            _filter_ws_copy_trailing(proc);
            break;
        case '\n':
            _c4dbgfsq("newline", curr);
            _filter_nl_squoted(proc);
            break;
        case '\r':  // skip \r --- https://stackoverflow.com/questions/1885900
            _c4dbgfsq("skip cr", curr);
            proc.skip();
            break;
        case '\'':
            _c4dbgfsq("squote", curr);
            if(proc.next() == '\'')
            {
                _c4dbgfsq("two consecutive squotes", curr);
                proc.skip();
                proc.copy();
            }
            break;
        default:
            proc.copy();
            break;
        }
    }

    _c4dbgfsq(": #filteredchars={} after=~~~[{}]{}~~~", proc.src.len-proc.sofar().len, proc.sofar().len, proc.sofar());

    return proc.result();
}

#undef _c4dbgfsq

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_squoted(csubstr scalar, substr dst) noexcept
{
    FilterProcessorSrcDst proc(scalar, dst);
    return _filter_squoted(proc);
}

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_squoted_inplace(substr dst, size_t cap) noexcept
{
    FilterProcessorInplace proc(dst, cap);
    return _filter_squoted(proc);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/* double quoted */

// a debugging scaffold:
#if 0
#define _c4dbgfdq(fmt, ...) _c4dbgpf("filt_dquo[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfdq(...)
#endif

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_nl_dquoted(FilterProcessor &C4_RESTRICT proc) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.curr() == '\n');

    _c4dbgfdq("found newline. sofar=[{}]~~~{}~~~", proc.wpos, proc.sofar());
    size_t ii = proc.rpos;
    const size_t numnl_following = _count_following_newlines(proc.src, &ii);
    if(numnl_following)
    {
        proc.set('\n', numnl_following);
        _c4dbgfdq("{} consecutive (empty) lines {}. totalws={}", 1+numnl_following, ii < proc.src.len ? "in the middle" : "at the end", proc.rpos-ii);
    }
    else
    {
        const size_t ret = proc.src.first_not_of(" \t", proc.rpos+1);
        if(ret != npos)
        {
            proc.set(' ');
            _c4dbgfdq("single newline. convert to space. ret={}/{}. sofar=[{}]~~~{}~~~", ii, proc.src.len, proc.wpos, proc.sofar());
        }
        else
        {
            proc.set(' ');
            _c4dbgfdq("single newline. convert to space. ii={}/{}. sofar=[{}]~~~{}~~~", ii, proc.src.len, proc.wpos, proc.sofar());
        }
        if(ii < proc.src.len && proc.src.str[ii] == '\\')
        {
            _c4dbgfdq("backslash at [{}]", ii);
            const char next = ii+1 < proc.src.len ? proc.src.str[ii+1] : '\0';
            if(next == ' ' || next == '\t')
            {
                _c4dbgfdq("extend skip to backslash", "");
                ++ii;
            }
        }
    }
    proc.rpos = ii;
}

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_dquoted_backslash(FilterProcessor &C4_RESTRICT proc, LocCRef loc) noexcept
{
    char next = proc.next();
    _c4dbgfdq("backslash, next='{}'", _c4prc(next));
    if(next == '\r')
    {
        if(proc.rpos+2 < proc.src.len && proc.src.str[proc.rpos+2] == '\n')
        {
            proc.skip(); // newline escaped with \ -- skip both (add only one as i is loop-incremented)
            next = '\n';
            _c4dbgfdq("[{}]: was \\r\\n, now next='\\n'", proc.rpos);
        }
    }

    if(next == '\n')
    {
        size_t ii = proc.rpos + 2;
        for( ; ii < proc.src.len; ++ii)
        {
            // skip leading whitespace
            if(proc.src.str[ii] == ' ' || proc.src.str[ii] == '\t')
                ;
            else
                break;
        }
        proc.skip(ii - proc.rpos);
    }
    else if(next == '"' || next == '/' || next == ' ' || next == '\t')
    {
        // escapes for json compatibility
        proc.translate_esc(next);
        _c4dbgfdq("here, used '{}'", _c4prc(next));
    }
    else if(next == '\r')
    {
        //proc.skip();
    }
    else if(next == 'n')
    {
        proc.translate_esc('\n');
    }
    else if(next == 'r')
    {
        proc.translate_esc('\r');
    }
    else if(next == 't')
    {
        proc.translate_esc('\t');
    }
    else if(next == '\\')
    {
        proc.translate_esc('\\');
    }
    else if(next == 'x') // UTF8
    {
        if(C4_UNLIKELY(proc.rpos + 1u + 2u >= proc.src.len))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "\\x requires 2 hex digits. scalar pos={}", proc.rpos);
        csubstr codepoint = proc.src.sub(proc.rpos + 2u, 2u);
        _c4dbgfdq("utf8 ~~~{}~~~ rpos={} rem=~~~{}~~~", codepoint, proc.rpos, proc.src.sub(proc.rpos));
        uint8_t byteval = {};
        if(C4_UNLIKELY(!read_hex(codepoint, &byteval)))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "failed to read \\x codepoint. scalar pos={}", proc.rpos);
        proc.translate_esc((const char*)&byteval, 1u, /*nread*/3u);
        _c4dbgfdq("utf8 after rpos={} rem=~~~{}~~~", proc.rpos, proc.src.sub(proc.rpos));
    }
    else if(next == 'u') // UTF16
    {
        if(C4_UNLIKELY(proc.rpos + 1u + 4u >= proc.src.len))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "\\u requires 4 hex digits. scalar pos={}", proc.rpos);
        char readbuf[8];
        csubstr codepoint = proc.src.sub(proc.rpos + 2u, 4u);
        uint32_t codepoint_val = {};
        if(C4_UNLIKELY(!read_hex(codepoint, &codepoint_val)))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "failed to parse \\u codepoint. scalar pos={}", proc.rpos);
        size_t numbytes = decode_code_point((uint8_t*)readbuf, sizeof(readbuf), codepoint_val);
        C4_ASSERT(numbytes <= 4);
        proc.translate_esc(readbuf, numbytes, /*nread*/5u);
    }
    else if(next == 'U') // UTF32
    {
        if(C4_UNLIKELY(proc.rpos + 1u + 8u >= proc.src.len))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "\\U requires 8 hex digits. scalar pos={}", proc.rpos);
        char readbuf[8];
        csubstr codepoint = proc.src.sub(proc.rpos + 2u, 8u);
        uint32_t codepoint_val = {};
        if(C4_UNLIKELY(!read_hex(codepoint, &codepoint_val)))
            _c4errflt(((Parser const*)this)->callbacks(), loc, "failed to parse \\U codepoint. scalar pos={}", proc.rpos);
        size_t numbytes = decode_code_point((uint8_t*)readbuf, sizeof(readbuf), codepoint_val);
        C4_ASSERT(numbytes <= 4);
        proc.translate_esc(readbuf, numbytes, /*nread*/9u);
    }
    // https://yaml.org/spec/1.2.2/#rule-c-ns-esc-char
    else if(next == '0')
    {
        proc.translate_esc('\0');
    }
    else if(next == 'b') // backspace
    {
        proc.translate_esc('\b');
    }
    else if(next == 'f') // form feed
    {
        proc.translate_esc('\f');
    }
    else if(next == 'a') // bell character
    {
        proc.translate_esc('\a');
    }
    else if(next == 'v') // vertical tab
    {
        proc.translate_esc('\v');
    }
    else if(next == 'e') // escape character
    {
        proc.translate_esc('\x1b');
    }
    else if(next == '_') // unicode non breaking space \u00a0
    {
        // https://www.compart.com/en/unicode/U+00a0
        const char payload[] = {
            _RYML_CHCONST(-0x3e, 0xc2),
            _RYML_CHCONST(-0x60, 0xa0),
        };
        proc.translate_esc(payload, /*nwrite*/2, /*nread*/1);
    }
    else if(next == 'N') // unicode next line \u0085
    {
        // https://www.compart.com/en/unicode/U+0085
        const char payload[] = {
            _RYML_CHCONST(-0x3e, 0xc2),
            _RYML_CHCONST(-0x7b, 0x85),
        };
        proc.translate_esc(payload, /*nwrite*/2, /*nread*/1);
    }
    else if(next == 'L') // unicode line separator \u2028
    {
        // https://www.utf8-chartable.de/unicode-utf8-table.pl?start=8192&number=1024&names=-&utf8=0x&unicodeinhtml=hex
        const char payload[] = {
            _RYML_CHCONST(-0x1e, 0xe2),
            _RYML_CHCONST(-0x80, 0x80),
            _RYML_CHCONST(-0x58, 0xa8),
        };
        proc.translate_esc(payload, /*nwrite*/3, /*nread*/1);
    }
    else if(next == 'P') // unicode paragraph separator \u2029
    {
        // https://www.utf8-chartable.de/unicode-utf8-table.pl?start=8192&number=1024&names=-&utf8=0x&unicodeinhtml=hex
        const char payload[] = {
            _RYML_CHCONST(-0x1e, 0xe2),
            _RYML_CHCONST(-0x80, 0x80),
            _RYML_CHCONST(-0x57, 0xa9),
        };
        proc.translate_esc(payload, /*nwrite*/3, /*nread*/1);
    }
    else if(next == '\0')
    {
        proc.skip();
    }
    _c4dbgfdq("backslash...sofar=[{}]~~~{}~~~", proc.wpos, proc.sofar());
}


template<class Parser>
template<class FilterProcessor>
FilterResult ScalarFilterCRTP<Parser>::_filter_dquoted(FilterProcessor &C4_RESTRICT proc, LocCRef loc) noexcept
{
    _c4dbgfdq("before=[{}]~~~{}~~~", proc.src.len, proc.src);
    // from the YAML spec for double-quoted scalars:
    // https://yaml.org/spec/1.2-old/spec.html#style/flow/double-quoted
    while(proc.has_more_chars())
    {
        const char curr = proc.curr();
        _c4dbgfdq("'{}' sofar=[{}]~~~{}~~~", _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case ' ':
        case '\t':
        {
            _c4dbgfdq("whitespace", curr);
            _filter_ws_copy_trailing(proc);
            break;
        }
        case '\n':
        {
            _c4dbgfdq("newline", curr);
            _filter_nl_dquoted(proc);
            break;
        }
        case '\r':  // skip \r --- https://stackoverflow.com/questions/1885900
        {
            _c4dbgfdq("carriage return, ignore", curr);
            break;
        }
        case '\\':
        {
            _filter_dquoted_backslash(proc, loc);
            break;
        }
        default:
        {
            proc.copy();
            break;
        }
        }
    }
    _c4dbgfdq("after[{}]=~~~{}~~~", proc.wpos, proc.sofar());
    return proc.result();
}

#undef _c4dbgfdq


template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_dquoted(csubstr scalar, substr dst, LocCRef loc) noexcept
{
    FilterProcessorSrcDst proc(scalar, dst);
    return _filter_dquoted(proc, loc);
}

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_dquoted_inplace(substr dst, size_t cap, LocCRef loc) noexcept
{
    FilterProcessorInplace proc(dst, cap);
    return _filter_dquoted(proc, loc);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// block filtering helpers

inline size_t _find_last_newline_and_larger_indentation(csubstr s, size_t indentation) noexcept
{
    if(indentation + 1 > s.len)
        return npos;
    for(size_t i = s.len-indentation-1; i != size_t(-1); --i)
    {
        if(s.str[i] == '\n')
        {
            csubstr rem = s.sub(i + 1);
            size_t first = rem.first_not_of(' ');
            first = (first != npos) ? first : rem.len;
            if(first > indentation)
                return i;
        }
    }
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n\n\n", 0) == npos);
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n \n\n", 0) == 2);
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n\n \n", 0) == 3);
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n \n \n", 0) == 4);
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n \n  \n", 1) == 4);
    //_RYML_CB_ASSERT(((Parser const*)this)->callbacks(), _find_last_newline_and_larger_indentation("ab\n  \n \n", 1) == 2);
    return npos;
}

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_chomp(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp, size_t indentation) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), chomp == CHOMP_CLIP || chomp == CHOMP_KEEP || chomp == CHOMP_STRIP);
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.rem().first_not_of(" \n\r") == npos);

    // a debugging scaffold:
    #if 0
    #define _c4dbgchomp(fmt, ...) _c4dbgpf("chomp[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
    #else
    #define _c4dbgchomp(...)
    #endif

    // advance to the last line having spaces beyond the indentation
    {
        size_t last = _find_last_newline_and_larger_indentation(proc.rem(), indentation);
        if(last != npos)
        {
            _c4dbgchomp("found newline and larger indentation. last={}", last);
            last = proc.rpos + last + size_t(1) + indentation;  // last started at to-be-read.
            _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), last <= proc.src.len);
            // remove indentation spaces, copy the rest
            while((proc.rpos < last) && proc.has_more_chars())
            {
                const char curr = proc.curr();
                _c4dbgchomp("curr='{}'", _c4prc(curr));
                switch(curr)
                {
                case '\n':
                    {
                        _c4dbgchomp("newline! remlen={}", proc.rem().len);
                        proc.copy();
                        // are there spaces after the newline?
                        csubstr at_next_line = proc.rem();
                        if(at_next_line.begins_with(' '))
                        {
                            // there are spaces.
                            const size_t first_non_space = at_next_line.first_not_of(' ');
                            _c4dbgchomp("first_non_space={}", first_non_space);
                            if(first_non_space != npos) // there are spaces
                            {
                                _c4dbgchomp("next line begins with spaces. indentation={}", indentation);
                                if(first_non_space <= indentation)
                                {
                                    _c4dbgchomp("skip spaces={}<=indentation={}", first_non_space, indentation);
                                    proc.skip(first_non_space);
                                }
                                else
                                {
                                    _c4dbgchomp("skip indentation={}<spaces={}", indentation, first_non_space);
                                    proc.skip(indentation);
                                    // copy the spaces after the indentation
                                    _c4dbgchomp("copy {}={}-{} spaces", first_non_space - indentation, first_non_space, indentation);
                                    proc.copy(first_non_space - indentation);
                                }
                            }
                        }
                        break;
                    }
                case '\r':
                    proc.skip();
                    break;
                default:
                    _RYML_CB_ERR(((Parser const*)this)->callbacks(), "never reach this");
                    break;
                }
            }
        }
    }
    // from now on, we only have line ends (or indentation spaces)
    switch(chomp)
    {
    case CHOMP_CLIP:
    {
        bool had_one = false;
        while(proc.has_more_chars())
        {
            const char curr = proc.curr();
            _c4dbgchomp("CLIP: '{}'", _c4prc(curr));
            switch(curr)
            {
            case '\n':
            {
                _c4dbgchomp("copy newline!", curr);
                proc.copy();
                proc.set_at_end();
                had_one = true;
                break;
            }
            case ' ':
            case '\r':
                _c4dbgchomp("skip!", curr);
                proc.skip();
                break;
            }
        }
        if(!had_one) // there were no newline characters. add one.
        {
            _c4dbgchomp("chomp=CLIP: add missing newline @{}", proc.wpos);
            proc.set('\n');
        }
        break;
    }
    case CHOMP_KEEP:
    {
        _c4dbgchomp("chomp=KEEP: copy all remaining new lines of {} characters", proc.rem().len);
        while(proc.has_more_chars())
        {
            const char curr = proc.curr();
            _c4dbgchomp("KEEP: '{}'", _c4prc(curr));
            switch(curr)
            {
            case '\n':
                _c4dbgchomp("copy newline!", curr);
                proc.copy();
                break;
            case ' ':
            case '\r':
                _c4dbgchomp("skip!", curr);
                proc.skip();
                break;
            }
        }
        break;
    }
    case CHOMP_STRIP:
    {
        _c4dbgchomp("chomp=STRIP: strip {} characters", proc.rem().len);
        // nothing to do!
        break;
    }
    }

    #undef _c4dbgchomp
}


// a debugging scaffold:
#if 0
#define _c4dbgfb(fmt, ...) _c4dbgpf("filt_block[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfb(...)
#endif

template<class Parser>
template<class FilterProcessor>
void ScalarFilterCRTP<Parser>::_filter_block_indentation(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept
{
    csubstr rem = proc.rem(); // remaining
    if(rem.len)
    {
        size_t first = rem.first_not_of(' ');
        if(first != npos)
        {
            _c4dbgfb("{} spaces follow before next nonws character", first);
            if(first < indentation)
            {
                _c4dbgfb("skip {}<{} spaces from indentation", first, indentation);
                proc.skip(first);
            }
            else
            {
                _c4dbgfb("skip {} spaces from indentation", indentation);
                proc.skip(indentation);
            }
        }
        else
        {
            _c4dbgfb("all spaces to the end: {} spaces", first);
            first = rem.len;
            if(first)
            {
                if(first < indentation)
                {
                    _c4dbgfb("skip everything", first);
                    proc.skip(proc.src.len - proc.rpos);
                }
                else
                {
                    _c4dbgfb("skip {} spaces from indentation", indentation);
                    proc.skip(indentation);
                }
            }
        }
    }
}

template<class Parser>
template<class FilterProcessor>
size_t ScalarFilterCRTP<Parser>::_handle_all_whitespace(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp) noexcept
{
    _c4dbgfb("indentation={} before=[{}]~~~{}~~~", indentation, proc.src.len, proc.src);

    csubstr contents = proc.src.trimr(" \n\r");
    if(!contents.len)
    {
        _c4dbgfb("all whitespace: len={}", proc.src.len);
        if(chomp == CHOMP_KEEP && proc.src.len)
        {
            _c4dbgfb("chomp=KEEP all {} newlines", proc.src.count('\n'));
            while(proc.has_more_chars())
            {
                const char curr = proc.curr();
                if(curr == '\n')
                    proc.copy();
                else
                    proc.skip();
            }
        }
    }
    return contents.len;
}

template<class Parser>
template<class FilterProcessor>
size_t ScalarFilterCRTP<Parser>::_extend_to_chomp(FilterProcessor &C4_RESTRICT proc, size_t contents_len) noexcept
{
    _c4dbgfb("contents_len={}", contents_len);

    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), contents_len > 0u);

    // extend contents to just before the first newline at the end,
    // in case it is preceded by spaces
    size_t firstnewl = proc.src.first_of('\n', contents_len);
    if(firstnewl != npos)
    {
        contents_len = firstnewl;
        _c4dbgfb("contents_len={}  <--- firstnewl={}", contents_len, firstnewl);
    }
    else
    {
        contents_len = proc.src.len;
        _c4dbgfb("contents_len={}  <--- src.len={}", contents_len, proc.src.len);
    }

    return contents_len;
}

#undef _c4dbgfb


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// a debugging scaffold:
#if 0
#define _c4dbgfbl(fmt, ...) _c4dbgpf("filt_block_lit[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfbl(...)
#endif

template<class Parser>
template<class FilterProcessor>
FilterResult ScalarFilterCRTP<Parser>::_filter_block_literal(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept
{
    _c4dbgfbl("indentation={} before=[{}]~~~{}~~~", indentation, proc.src.len, proc.src);

    size_t contents_len = _handle_all_whitespace(proc, chomp);
    if(!contents_len)
        return proc.result();

    contents_len = _extend_to_chomp(proc, contents_len);

    _c4dbgfbl("to filter=[{}]~~~{}~~~", contents_len, proc.src.first(contents_len));

    _filter_block_indentation(proc, indentation);

    // now filter the bulk
    while(proc.has_more_chars(/*maxpos*/contents_len))
    {
        const char curr = proc.curr();
        _c4dbgfbl("'{}' sofar=[{}]~~~{}~~~",  _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case '\n':
        {
            _c4dbgfbl("found newline. skip indentation on the next line", curr);
            proc.copy();  // copy the newline
            _filter_block_indentation(proc, indentation);
            break;
        }
        case '\r':
            proc.skip();
            break;
        default:
            proc.copy();
            break;
        }
    }

    _c4dbgfbl("before chomp: #tochomp={}   sofar=[{}]~~~{}~~~", proc.rem().len, proc.sofar().len, proc.sofar());

    _filter_chomp(proc, chomp, indentation);

    _c4dbgfbl("final=[{}]~~~{}~~~", proc.sofar().len, proc.sofar());

    return proc.result();
}

#undef _c4dbgfbl

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_block_literal_inplace(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept
{
    FilterProcessorInplace proc(scalar, cap);
    return _filter_block_literal(proc, indentation, chomp);
}

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_block_literal(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept
{
    FilterProcessorSrcDst proc(scalar, dst);
    return _filter_block_literal(proc, indentation, chomp);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// a debugging scaffold:
#if 0
#define _c4dbgfbf(fmt, ...) _c4dbgpf("filt_block_folded[{}->{}]: " fmt, proc.rpos, proc.wpos, __VA_ARGS__)
#else
#define _c4dbgfbf(...)
#endif


template<class Parser>
template<class FilterProcessor>
size_t ScalarFilterCRTP<Parser>::_filter_block_folded_indented(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len, size_t curr_indentation) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.rem().first_not_of(' ') == curr_indentation);
    proc.copy(curr_indentation);
    while(proc.has_more_chars(len))
    {
        const char curr = proc.curr();
        _c4dbgfbf("'{}' sofar=[{}]~~~{}~~~",  _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case '\n':
            {
                proc.copy();
                _filter_block_indentation(proc, indentation);
                const size_t first = proc.rem().first_not_of(' ');
                _c4dbgfbf("newline. firstns={}",  first);
                if(first == 0)
                {
                    _c4dbgfbf("done with indented block",  first);
                    goto endloop;
                }
                else if(first != npos)
                {
                    proc.copy(first);
                    _c4dbgfbf("copy all {} spaces",  first);
                }
                break;
            }
            break;
        case '\r':
            proc.skip();
            break;
        default:
            proc.copy();
            break;
        }
    }
 endloop:
    return indentation;
}


template<class Parser>
template<class FilterProcessor>
size_t ScalarFilterCRTP<Parser>::_filter_block_folded_newlines(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len) noexcept
{
    _RYML_CB_ASSERT(((Parser const*)this)->callbacks(), proc.curr() == '\n');
    size_t num_newl_extra = 0;
    // skip the first newline
    if(proc.wpos)
    {
        _c4dbgfbf("skip first newline in block",  0);
        proc.skip();
    }
    else
    {
        _c4dbgfbf("first newline is leading, copy",  0);
        proc.copy();
        ++num_newl_extra;
    }
    _filter_block_indentation(proc, indentation);
    // now copy all the following newlines until the indentation increases
    while(proc.has_more_chars(len))
    {
        const char curr = proc.curr();
        _c4dbgfbf("'{}' sofar=[{}]~~~{}~~~",  _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case '\n':
            ++num_newl_extra;
            proc.copy();
            _filter_block_indentation(proc, indentation);
            break;
        case ' ':
            {
                const size_t first = proc.rem().first_not_of(' ');
                if(first != npos)
                {
                    _c4dbgfbf("indentation increased to {}",  first);
                    if(!num_newl_extra)
                    {
                        _c4dbgfbf("was the indentation. add newline", first);
                        proc.set('\n');
                        ++num_newl_extra;
                    }
                    _filter_block_folded_indented(proc, indentation, len, first);
                }
                break;
            }
        case '\r':
            proc.skip();
            break;
        default:
            goto endloop;
            break;
        }
    }
 endloop:
    if(!num_newl_extra)
    {
        _c4dbgfbf("add missing newline from the first", 0);
        proc.set(' ');
    }
    return indentation;
}


template<class Parser>
template<class FilterProcessor>
FilterResult ScalarFilterCRTP<Parser>::_filter_block_folded(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept
{
    _c4dbgfbf("indentation={} before=[{}]~~~{}~~~", indentation, proc.src.len, proc.src);

    size_t contents_len = _handle_all_whitespace(proc, chomp);
    if(!contents_len)
        return proc.result();

    contents_len = _extend_to_chomp(proc, contents_len);

    _c4dbgfbf("to filter=[{}]~~~{}~~~", contents_len, proc.src.first(contents_len));

    _filter_block_indentation(proc, indentation);

    // now filter the bulk
    while(proc.has_more_chars(/*maxpos*/contents_len))
    {
        const char curr = proc.curr();
        _c4dbgfbf("'{}' sofar=[{}]~~~{}~~~",  _c4prc(curr), proc.wpos, proc.sofar());
        switch(curr)
        {
        case '\n':
        {
            _c4dbgfbf("found newline", curr);
            (void)_filter_block_folded_newlines(proc, indentation, contents_len);
            break;
        }
        case '\r':
            proc.skip();
            break;
        default:
            proc.copy();
            break;
        }
    }

    _c4dbgfbf("before chomp: #tochomp={}   sofar=[{}]~~~{}~~~", proc.rem().len, proc.sofar().len, proc.sofar());

    _filter_chomp(proc, chomp, indentation);

    _c4dbgfbf("final=[{}]~~~{}~~~", proc.sofar().len, proc.sofar());

    return proc.result();
}

#undef _c4dbgfbf

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_block_folded_inplace(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept
{
    FilterProcessorInplace proc(scalar, cap);
    return _filter_block_folded(proc, indentation, chomp);
}

template<class Parser>
FilterResult ScalarFilterCRTP<Parser>::filter_block_folded(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept
{
    FilterProcessorSrcDst proc(scalar, dst);
    return _filter_block_folded(proc, indentation, chomp);
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_GCC_CLANG_POP


#endif /* C4_YML_FILTER_DEF_HPP_ */
