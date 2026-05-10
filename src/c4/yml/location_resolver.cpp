#ifndef C4_YML_LOCATION_RESOLVER_HPP_
#include "c4/yml/location_resolver.hpp"
#endif
#ifndef _C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

LocationResolver::~LocationResolver() noexcept
{
    _free();
}

LocationResolver::LocationResolver(Callbacks const& cb) noexcept
    : m_filename()
    , m_src()
    , m_offsets()
    , m_size()
    , m_capacity()
    , m_callbacks(cb)
{
}

LocationResolver::LocationResolver() noexcept
    : LocationResolver(get_callbacks())
{
}

LocationResolver::LocationResolver(LocationResolver &&that) noexcept
    : m_filename(that.m_filename)
    , m_src(that.m_src)
    , m_offsets(that.m_offsets)
    , m_size(that.m_size)
    , m_capacity(that.m_capacity)
    , m_callbacks(that.m_callbacks)
{
    that._clr();
}

LocationResolver::LocationResolver(LocationResolver const& that)
    : m_filename(that.m_filename)
    , m_src(that.m_src)
    , m_offsets()
    , m_size()
    , m_capacity()
    , m_callbacks(that.m_callbacks)
{
    if(that.m_capacity)
    {
        _resize(that.m_capacity, false);
        RYML_ASSERT_BASIC_CB_(m_callbacks, m_capacity == that.m_capacity);
        memcpy(m_offsets, that.m_offsets, that.m_size * sizeof(size_t));
        m_size = that.m_size;
    }
}

LocationResolver& LocationResolver::operator=(LocationResolver &&that) noexcept
{
    _free();
    m_offsets = that.m_offsets;
    m_size = that.m_size;
    m_capacity = that.m_capacity;
    that._clr();
    return *this;
}

LocationResolver& LocationResolver::operator=(LocationResolver const& that)
{
    if(&that != this)
    {
        _free();
        m_callbacks = that.m_callbacks; // only after freeing!
        if(that.m_capacity)
            _resize(that.m_capacity, false);
        RYML_ASSERT_BASIC_CB_(m_callbacks, m_capacity >= that.m_capacity);
        RYML_ASSERT_BASIC_CB_(m_callbacks, m_capacity >= that.m_size);
        memcpy(m_offsets, that.m_offsets, that.m_size * sizeof(size_t));
        m_size = that.m_size;
    }
    return *this;
}

void LocationResolver::_clr()
{
    m_offsets = {};
    m_size = {};
    m_capacity = {};
}

void LocationResolver::_free()
{
    if(m_offsets)
        RYML_CB_FREE_(get_callbacks(), m_offsets, size_t, m_capacity);
    _clr();
}

void LocationResolver::set_callbacks(Callbacks const& cb)
{
    if(cb == m_callbacks)
        return;
    if(m_capacity)
    {
        size_t *buf = RYML_CB_ALLOC_HINT_(m_callbacks, size_t, m_capacity, m_offsets);
        if(m_size)
            memcpy(buf, m_offsets, m_size * sizeof(size_t));
        RYML_CB_FREE_(m_callbacks, m_offsets, size_t, m_capacity);
        m_offsets = buf;
    }
}

void LocationResolver::reserve(size_t num_newlines)
{
    num_newlines = num_newlines > 2 * m_capacity ? num_newlines : 2 * m_capacity;
    _resize(num_newlines, /*copy*/true);
}

void LocationResolver::reset(csubstr filename, csubstr src)
{
    m_filename = filename;
    m_src = src;
    size_t count = 0;
    for(size_t i = 0; i < m_src.len; i++)
    {
        if(m_src.str[i] == '\n')
        {
            if(C4_UNLIKELY(count == m_capacity))
                reserve(2u * (count + 1));
            m_offsets[count++] = i; // NOLINT
        }
    }
    if(C4_UNLIKELY(count == m_capacity))
        reserve(2u * (count + 1));
    m_offsets[count++] = m_src.len; // NOLINT
    m_size = count;
}


void LocationResolver::_resize(size_t numnewlines, bool copy)
{
    numnewlines = numnewlines >= 16 ? numnewlines : 16;
    if(numnewlines > m_capacity)
    {
        size_t *buf = RYML_CB_ALLOC_HINT_(m_callbacks, size_t, numnewlines, m_offsets);
        if(m_capacity)
        {
            if(copy)
                memcpy(buf, m_offsets, m_size * sizeof(size_t));
            RYML_CB_FREE_(m_callbacks, m_offsets, size_t, m_capacity);
        }
        m_offsets = buf;
        m_capacity = numnewlines;
    }
}


//-----------------------------------------------------------------------------

csubstr LocationResolver::location_contents(Location const& loc) const
{
    RYML_ASSERT_BASIC_CB_(m_callbacks, loc.name == m_filename);
    RYML_ASSERT_BASIC_CB_(m_callbacks, loc.offset < m_src.len);
    return m_src.sub(loc.offset);
}

Location LocationResolver::val_location(const char *val, size_t linear_threshold) const
{
    if(C4_UNLIKELY(val == nullptr))
        return {m_filename, 0, 0, 0};
    RYML_ASSERT_BASIC_CB_(m_callbacks, m_offsets != nullptr);
    RYML_ASSERT_BASIC_CB_(m_callbacks, m_size > 0);
    // NOTE: the pointer needs to belong to the buffer that was used to parse.
    RYML_CHECK_BASIC_CB_(m_callbacks, val != nullptr || m_src.str == nullptr);
    RYML_CHECK_BASIC_CB_(m_callbacks, (val >= m_src.begin() && val <= m_src.end()) || (m_src.str == nullptr && val == nullptr));
    // ok. search the first stored newline after the given ptr
    using lineptr_type = size_t const* C4_RESTRICT;
    lineptr_type lineptr = nullptr;
    size_t offset = (size_t)(val - m_src.begin());
    // do a linear search if the size is small.
    if(m_size < linear_threshold)
    {
        for(lineptr_type curr = m_offsets, last = m_offsets + m_size; curr < last; ++curr)
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
        size_t count = m_size;
        lineptr = m_offsets;
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
    RYML_ASSERT_BASIC_CB_(m_callbacks, lineptr >= m_offsets);
    RYML_ASSERT_BASIC_CB_(m_callbacks, lineptr <= m_offsets + m_size);
    RYML_ASSERT_BASIC_CB_(m_callbacks, *lineptr > offset);
    Location loc;
    loc.name = m_filename;
    loc.offset = offset;
    loc.line = (size_t)(lineptr - m_offsets);
    loc.col = offset;
    if(lineptr > m_offsets)
    {
        RYML_ASSERT_BASIC_CB_(m_callbacks, *(lineptr-1) >= 1u);
        RYML_ASSERT_BASIC_CB_(m_callbacks, loc.col >= *(lineptr-1) - 1u);
        loc.col -= *(lineptr-1) - 1u;
    }
    return loc;
}

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH

} // namespace yml
} // namespace c4
