#include "c4/yml/filter.hpp"

namespace c4 {
namespace yml {

namespace {

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


substr ScalarFilterProcessor::filter_squoted(substr scalar, size_t indentation, Location const* loc)
{
    (void)scalar;
    (void)loc;

    // a debugging scaffold:
    #if 0
    #define _c4dbgfsq(...) _c4dbgpf("filt_squo_scalar: " __VA_ARGS__)
    #else
    #define _c4dbgfsq(...)
    #endif

    // from the YAML spec for double-quoted scalars:
    // https://yaml.org/spec/1.2-old/spec.html#style/flow/single-quoted

    _c4dbgfsq("before=~~~{}~~~", s);

    // we will filter by writing chunks [wpos, rpos[
    size_t wpos = 0; // write position
    size_t rpos = 0; // read position

    for(size_t i = 0; i < scalar.len; ++i)
    {
        const char curr = scalar.str[i];
        _c4dbgfsq("[{}]: '{}'", i, _c4prc(curr));
        if(curr == ' ' || curr == '\t')
        {
            //_filter_ws</*keep_trailing_ws*/true>(r, &i, &pos);
            _c4dbgfsq("found whitespace '{}'", _c4prc(curr));

            size_t first = i > 0 ? scalar.first_not_of(" \t", i) : scalar.first_not_of(' ', i);
            if(first != npos)
            {
                if(scalar.str[first] == '\n' || scalar.str[first] == '\r') // skip trailing whitespace
                {
                    _c4dbgfsq("whitespace is trailing on line. firstnonws='{}'@{}", _c4prc(scalar[first]), first);
                    i = first - 1; // correct for the loop increment
                }
                else // a legit whitespace
                {
                    scalar.str[wpos++] = curr;
                    _c4dbgfsq("legit whitespace. sofar=[{}]~~~{}~~~", wpos, scalar.first(wpos));
                }
            }
            else
            {
                _c4dbgfsq("... everything else is trailing whitespace");
                for(size_t j = i; j < scalar.len; ++j)
                    scalar.str[wpos++] = scalar.str[j];
                i = scalar.len;
            }
        }
        else if(curr == '\n')
        {
            //filtered_chars =
            //_filter_nl</*backslash_is_escape*/false,
            ///*keep_trailing_ws*/true>(r, &i, &pos,
            ///*indentation*/0);

            _RYML_CB_ASSERT(*m_callbacks, indentation != npos);
            _RYML_CB_ASSERT(*m_callbacks, curr == '\n');

            _c4dbgfsq("found newline. sofar=[{}]~~~{}~~~", wpos, scalar.first(wpos));
            size_t ii = i;
            size_t numnl_following = _count_following_newlines(scalar, &ii, indentation);
            if(numnl_following)
            {
                _c4dbgfsq("{} consecutive (empty) lines {} in the middle. totalws={}", 1+numnl_following, ii < scalar.len ? "in the middle" : "at the end", ii - i);
                for(size_t j = 0; j < numnl_following; ++j)
                    scalar.str[wpos++] = '\n';
            }
            else
            {
                scalar.str[wpos++] = ' ';
                _c4dbgfsq("single newline. convert to space. ii={}/{}. sofar=[{}]~~~{}~~~", ii, r.len, *pos, m_filter_arena.first(*pos));
            }
            i = ii - 1; // correct for the loop increment
        }
        else if(curr == '\r')  // skip \r --- https://stackoverflow.com/questions/1885900
        {
            ;
        }
        else if(curr == '\'')
        {
            char next = i+1 < scalar.len ? scalar.str[i+1] : '\0';
            if(next == '\'')
            {
                _c4dbgfsq("[{}]: two consecutive quotes", i);
                scalar.str[wpos++] = '\'';
                ++i;
            }
        }
        else
        {
            ++wpos;
        }
    }

    _RYML_CB_ASSERT(*m_callbacks, wpos <= rpos);
    if(wpos < rpos)
    {
    }

    substr ret = scalar.first(wpos);
    _c4dbgfsq(": #filteredchars={} after=~~~{}~~~", scalar.len - wpos, ret);

    #undef _c4dbgfsq
    return ret;
}

substr ScalarFilterProcessor::filter_dquoted(substr scalar, size_t indentation, Location const* loc)
{
    (void)scalar;
    (void)indentation;
    (void)loc;
    return scalar;
}

substr ScalarFilterProcessor::filter_block_literal(substr scalar, size_t indentation, Location const* loc)
{
    (void)indentation;
    (void)loc;
    return scalar;
}

substr ScalarFilterProcessor::filter_block_folded(substr scalar, size_t indentation, Location const* loc)
{
    (void)indentation;
    (void)loc;
    return scalar;
}

substr ScalarFilterProcessor::filter_plain(substr scalar, size_t indentation, Location const* loc)
{
    (void)indentation;
    (void)loc;
    return scalar;
}


} // namespace yml
} // namespace c4

