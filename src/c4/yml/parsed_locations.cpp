#ifndef _C4_YML_PARSED_LOCATIONS_HPP_
#include "c4/yml/parsed_locations.hpp"
#endif

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

ParsedLocations::~ParsedLocations() noexcept
{
    _free();
}

ParsedLocations::ParsedLocations()
    : m_newline_offsets()
    , m_newline_offsets_size()
    , m_newline_offsets_capacity()
{
}

ParsedLocations::ParsedLocations(ParsedLocations &&that) noexcept
    : m_newline_offsets(that.m_newline_offsets)
    , m_newline_offsets_size(that.m_newline_offsets_size)
    , m_newline_offsets_capacity(that.m_newline_offsets_capacity)
{
    that._clr();
}

ParsedLocations::ParsedLocations(ParsedLocations const& that)
    : m_newline_offsets()
    , m_newline_offsets_size()
    , m_newline_offsets_capacity()
{
    if(that.m_newline_offsets_capacity)
    {
        _resize(that.m_newline_offsets_capacity);
        _RYML_CHECK_BASIC_(get_callbacks(), m_newline_offsets_capacity == that.m_newline_offsets_capacity);
        memcpy(m_newline_offsets, that.m_newline_offsets, that.m_newline_offsets_size * sizeof(size_t));
        m_newline_offsets_size = that.m_newline_offsets_size;
    }
}

ParsedLocations& ParsedLocations::operator=(ParsedLocations &&that) noexcept
{
    _free();
    m_newline_offsets = (that.m_newline_offsets);
    m_newline_offsets_size = (that.m_newline_offsets_size);
    m_newline_offsets_capacity = (that.m_newline_offsets_capacity);
    that._clr();
    return *this;
}

ParsedLocations& ParsedLocations::operator=(ParsedLocations const& that)
{
    if(&that != this)
    {
        _free();
        if(that.m_newline_offsets_capacity > m_newline_offsets_capacity)
            _resize(that.m_newline_offsets_capacity);
        _RYML_CHECK_BASIC_(get_callbacks(), m_newline_offsets_capacity >= that.m_newline_offsets_capacity);
        _RYML_CHECK_BASIC_(get_callbacks(), m_newline_offsets_capacity >= that.m_newline_offsets_size);
        memcpy(m_newline_offsets, that.m_newline_offsets, that.m_newline_offsets_size * sizeof(size_t));
        m_newline_offsets_size = that.m_newline_offsets_size;
    }
    return *this;
}

void ParsedLocations::_clr()
{
    m_newline_offsets = {};
    m_newline_offsets_size = {};
    m_newline_offsets_capacity = {};
}

void ParsedLocations::_free()
{
    if(m_newline_offsets)
        _RYML_CB_FREE(get_callbacks(), m_newline_offsets, size_t, m_newline_offsets_capacity);
    _clr();
}


void ParsedLocations::reset(csubstr filename, csubstr src)
{
    m_filename = filename;
    m_src = src;
    size_t numnewlines = 1u + m_src.count('\n');
    _resize(numnewlines);
    m_newline_offsets_size = 0;
    for(size_t i = 0; i < m_src.len; i++)
        if(m_src.str[i] == '\n')
            m_newline_offsets[m_newline_offsets_size++] = i; // NOLINT
    m_newline_offsets[m_newline_offsets_size++] = m_src.len; // NOLINT
    _RYML_ASSERT_BASIC_(get_callbacks(), m_newline_offsets_size == numnewlines);
}


void ParsedLocations::_resize(size_t numnewlines)
{
    numnewlines = numnewlines >= 16 ? numnewlines : 16;
    if(numnewlines > m_newline_offsets_capacity)
    {
        if(m_newline_offsets)
            _RYML_CB_FREE(get_callbacks(), m_newline_offsets, size_t, m_newline_offsets_capacity);
        m_newline_offsets = _RYML_CB_ALLOC_HINT(get_callbacks(), size_t, numnewlines, m_newline_offsets);
        m_newline_offsets_capacity = numnewlines;
    }
}


//-----------------------------------------------------------------------------

csubstr ParsedLocations::location_contents(Location const& loc) const
{
    _RYML_ASSERT_BASIC_(get_callbacks(), loc.name == m_filename);
    _RYML_ASSERT_BASIC_(get_callbacks(), loc.offset < m_src.len);
    return m_src.sub(loc.offset);
}

Location ParsedLocations::val_location(const char *val, Callbacks const& cb, size_t linear_threshold) const
{
    if(C4_UNLIKELY(val == nullptr))
        return {m_filename, 0, 0, 0};
    _RYML_ASSERT_BASIC_(cb, m_newline_offsets != nullptr);
    _RYML_ASSERT_BASIC_(cb, m_newline_offsets_size > 0);
    // NOTE: the pointer needs to belong to the buffer that was used to parse.
    _RYML_CHECK_BASIC_(cb, val != nullptr || m_src.str == nullptr);
    _RYML_CHECK_BASIC_(cb, (val >= m_src.begin() && val <= m_src.end()) || (m_src.str == nullptr && val == nullptr));
    // ok. search the first stored newline after the given ptr
    using lineptr_type = size_t const* C4_RESTRICT;
    lineptr_type lineptr = nullptr;
    size_t offset = (size_t)(val - m_src.begin());
    // do a linear search if the size is small.
    if(m_newline_offsets_size < linear_threshold)
    {
        for(lineptr_type curr = m_newline_offsets, last = m_newline_offsets + m_newline_offsets_size; curr < last; ++curr)
        {
            if(*curr > offset)
            {
                lineptr = curr;
                break;
            }
        }
    }
    else
    {
        // do a bisection search if the size is not small.
        //
        // We could use std::lower_bound but this is simple enough and
        // spares the costly include of <algorithm>.
        size_t count = m_newline_offsets_size;
        lineptr = m_newline_offsets;
        while(count)
        {
            size_t step = count >> 1u;
            lineptr_type it = lineptr + step;
            if(*it < offset)
            {
                lineptr = ++it;
                count -= step + 1;
            }
            else
            {
                count = step;
            }
        }
    }
    _RYML_ASSERT_BASIC_(cb, lineptr >= m_newline_offsets);
    _RYML_ASSERT_BASIC_(cb, lineptr <= m_newline_offsets + m_newline_offsets_size);
    _RYML_ASSERT_BASIC_(cb, *lineptr > offset);
    Location loc;
    loc.name = m_filename;
    loc.offset = offset;
    loc.line = (size_t)(lineptr - m_newline_offsets);
    loc.col = offset;
    if(lineptr > m_newline_offsets)
    {
        _RYML_ASSERT_BASIC_(cb, *(lineptr-1) >= 1u);
        _RYML_ASSERT_BASIC_(cb, loc.col >= *(lineptr-1) - 1u);
        loc.col -= *(lineptr-1) - 1u;
    }
    return loc;
}

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH

} // namespace yml
} // namespace c4
