#ifndef _C4_YML_EXTRA_STRING_HPP_
#define _C4_YML_EXTRA_STRING_HPP_

#ifndef RYML_SINGLE_HEADER
#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif
#endif

#include <new>

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

#ifndef RYML_STRING_SSO_SIZE
#define RYML_STRING_SSO_SIZE 128
#endif

#ifndef RYML_STRING_LIST_SSO_SIZE
#define RYML_STRING_LIST_SSO_SIZE 66
#endif

namespace c4 {
namespace yml {
namespace extra {

/** an owning string class used by the yaml std event handler (and the
 * YamlScript handler). we use this instead of std::string because:
 *  1) this spares the dependency on the standard library
 *  2) enables possibility of adding borrowing semantics (in the future) */
struct string
{
    enum : id_type { sso_size = RYML_STRING_SSO_SIZE };
    char m_buf[sso_size];
    char *C4_RESTRICT m_str;
    id_type m_size;
    id_type m_capacity;

public:

    string()
        : m_buf()
        , m_str(m_buf)
        , m_size(0)
        , m_capacity(sso_size)
    {}
    ~string() noexcept
    {
        _free();
    }

    string(string const& that) RYML_NOEXCEPT : string()
    {
        resize(that.m_size);
        _cp(&that);
    }

    string(string &&that) noexcept : string()
    {
        _mv(&that);
    }

    string& operator= (string const& that) RYML_NOEXCEPT
    {
        resize(that.m_size);
        _cp(&that);
        return *this;
    }

    string& operator= (string &&that) noexcept
    {
        _mv(&that);
        return *this;
    }

public:

    C4_ALWAYS_INLINE C4_HOT operator csubstr() const noexcept { return {m_str, m_size}; }
    C4_ALWAYS_INLINE C4_HOT operator substr() noexcept { return {m_str, m_size}; }

public:

    const char* data() const noexcept { return m_str; }
    id_type size() const noexcept { return m_size; }
    id_type capacity() const noexcept { return m_capacity; }

    void clear()
    {
        m_size = 0;
    }

    void resize(id_type sz)
    {
        reserve(sz);
        m_size = sz;
    }

    void reserve(id_type sz)
    {
        if(sz <= m_capacity)
            return;
        id_type cap = m_capacity < string::sso_size ? string::sso_size : 2 * m_capacity;
        cap = cap > sz ? cap : sz;
        if(cap <= sso_size)
            return;
        Callbacks cb = get_callbacks();
        char *buf = (char*) _RYML_CB_ALLOC(cb, char, cap);
        if(m_size)
            memcpy(buf, m_str, (size_t)m_size);
        if(m_str != m_buf)
        {
            _RYML_CB_FREE(cb, m_str, char, m_size);
        }
        m_str = buf;
        m_capacity = cap;
    }

public:

    C4_ALWAYS_INLINE C4_HOT void append(char c)
    {
        if(C4_UNLIKELY(m_size == m_capacity))
            reserve(m_size + 1);
        m_str[m_size++] = c;
    }
    C4_ALWAYS_INLINE C4_HOT void append(csubstr cs)
    {
        if(cs.len)
        {
            const id_type ilen = (id_type)cs.len;
            if(C4_UNLIKELY(m_size + ilen > m_capacity))
                reserve(m_size + ilen);
            memcpy(m_str + m_size, cs.str, cs.len);
            m_size += ilen;
        }
    }
    C4_ALWAYS_INLINE void insert(char c, id_type pos)
    {
        RYML_ASSERT(pos <= m_size);
        if(pos < m_size)
        {
            if(C4_UNLIKELY(m_size == m_capacity))
                reserve(m_size + 1);
            char *C4_RESTRICT src = m_str + pos;
            memmove(src + 1, src, m_size - pos);
            *src = c;
            ++m_size;
        }
        else
        {
            append(c);
        }
    }
    C4_NO_INLINE void insert(csubstr cs, id_type pos)
    {
        RYML_ASSERT(pos <= m_size);
        if(cs.len)
        {
            if(pos < m_size)
            {
                const id_type ilen = (id_type)cs.len;
                if(C4_UNLIKELY(m_size + ilen > m_capacity))
                    reserve(m_size + ilen);
                char *C4_RESTRICT src = m_str + pos;
                memmove(src + cs.len, src, m_size - pos);
                memcpy(src, cs.str, cs.len);
                m_size += ilen;
            }
            else
            {
                append(cs);
            }
        }
    }
    C4_NO_INLINE size_t find_last(csubstr pattern) RYML_NOEXCEPT
    {
        RYML_ASSERT(pattern.len);
        if(m_size >= pattern.len)
        {
            for(size_t i = m_size - pattern.len; i != (size_t)-1; --i)
            {
                if(m_str[i] == pattern[0])
                {
                    bool gotit = true;
                    for(size_t j = 1; j < pattern.len; ++j)
                    {
                        if(m_str[i + j] != pattern[j])
                        {
                            gotit = false;
                            break;
                        }
                    }
                    if(gotit)
                        return i;
                }
            }
        }
        return npos;
    }

public:

    void _free()
    {
        RYML_ASSERT(m_str != nullptr); // this structure cannot be memset() to zero
        if(m_str != m_buf)
        {
            _RYML_CB_FREE(get_callbacks(), m_str, char, (size_t)m_capacity);
            m_str = m_buf;
            m_capacity = sso_size;
        }
        RYML_ASSERT(m_capacity == sso_size);
        m_size = 0;
    }

    void _cp(string const* C4_RESTRICT that)
    {
        #if RYML_USE_ASSERT
        if(that->m_str != that->m_buf)
        {
            RYML_ASSERT(that->m_capacity > sso_size);
            RYML_ASSERT(that->m_size <= that->m_capacity);
        }
        else
        {
            RYML_ASSERT(that->m_capacity <= sso_size);
            RYML_ASSERT(that->m_size <= that->m_capacity);
        }
        #endif
        memcpy(m_str, that->m_str, that->m_size);
        m_size = that->m_size;
        m_capacity = that->m_size < sso_size ? sso_size : that->m_size;
    }

    void _mv(string *C4_RESTRICT that)
    {
        if(that->m_str != that->m_buf)
        {
            RYML_ASSERT(that->m_capacity > sso_size);
            RYML_ASSERT(that->m_size <= that->m_capacity);
            m_str = that->m_str;
        }
        else
        {
            RYML_ASSERT(that->m_capacity <= sso_size);
            RYML_ASSERT(that->m_size <= that->m_capacity);
            memcpy(m_buf, that->m_buf, that->m_size);
            m_str = m_buf;
        }
        m_size = that->m_size;
        m_capacity = that->m_capacity;
        // make sure no deallocation happens on destruction
        RYML_ASSERT(that->m_str != this->m_buf);
        that->m_str = that->m_buf;
        that->m_capacity = sso_size;
        that->m_size = 0;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** a string collection used by the event handler. using this instead of
 * std::vector spares the dependency on the standard library. */
struct string_vector
{
    enum : id_type { sso_size = RYML_STRING_LIST_SSO_SIZE };
    union {
        alignas(string) string m_buf[sso_size];
        alignas(string) char   m_buf_bytes[sso_size * sizeof(string)];
    };
    string *C4_RESTRICT m_arr;
    id_type m_size;
    id_type m_capacity;

public:

    string_vector()
        : m_arr(m_buf)
        , m_size(0)
        , m_capacity(sso_size)
    {}
    ~string_vector() noexcept
    {
        _free();
    }

    string_vector(string_vector const& that) RYML_NOEXCEPT : string_vector()
    {
        reserve(that.m_size);
        m_size = that.m_size;
        for(id_type i = 0; i < that.m_size; ++i)
            new ((void*)(m_arr+i)) string(that.m_arr[i]);
    }

    string_vector(string_vector &&that) noexcept : string_vector()
    {
        reserve(that.m_size);
        m_size = that.m_size;
        for(id_type i = 0; i < that.m_size; ++i)
            new ((void*)(m_arr+i)) string(std::move(that.m_arr[i]));
        that.~string_vector();
    }

    string_vector& operator= (string_vector const& that) RYML_NOEXCEPT
    {
        _free();
        reserve(that.m_size);
        for(id_type i = 0; i < that.m_size; ++i)
            m_arr[i].operator=(that.m_arr[i]);
        m_size = that.m_size;
        return *this;
    }

    string_vector& operator= (string_vector &&that) noexcept
    {
        _free();
        reserve(that.m_size);
        for(id_type i = 0; i < that.m_size; ++i)
            m_arr[i].operator=(std::move(that.m_arr[i]));
        m_size = that.m_size;
        that.~string_vector();
        return *this;
    }

    void _free()
    {
        RYML_ASSERT(m_arr != nullptr); // this structure cannot be memset() to zero
        for(id_type i = 0; i < m_size; ++i)
            m_arr[i].~string();
        if(m_arr != m_buf)
        {
            _RYML_CB_FREE(get_callbacks(), m_arr, string, (size_t)m_capacity);
            m_arr = m_buf;
            m_capacity = sso_size;
        }
        RYML_ASSERT(m_capacity == sso_size);
        m_size = 0;
    }

public:

    id_type size() const noexcept { return m_size; }
    id_type capacity() const noexcept { return m_capacity; }

    void clear()
    {
        resize(0);
    }

    void resize(id_type sz)
    {
        reserve(sz);
        if(sz >= m_size)
        {
            for(id_type i = m_size; i < sz; ++i)
                new ((void*)(m_arr + i)) string();
        }
        else
        {
            for(id_type i = sz; i < m_size; ++i)
                m_arr[i].~string();
        }
        m_size = sz;
    }

    void reserve(id_type sz)
    {
        if(sz <= m_capacity)
            return;
        id_type cap = m_capacity < string::sso_size ? string::sso_size : 2 * m_capacity;
        cap = cap > sz ? cap : sz;
        if(cap <= sso_size)
            return;
        Callbacks cb = get_callbacks();
        string *buf = (string*) _RYML_CB_ALLOC(cb, string, cap);
        for(id_type i = 0; i < m_size; ++i)
            new ((void*)(buf + i)) string(std::move(m_arr[i]));
        if(m_arr != m_buf)
        {
            _RYML_CB_FREE(cb, m_arr, string, m_size);
        }
        m_arr = buf;
        m_capacity = cap;
    }

public:

    string& emplace_back()
    {
        RYML_ASSERT(m_size < m_capacity);
        if(m_size == m_capacity)
            reserve(m_size + 1);
        string& ret = m_arr[m_size++];
        new ((void*)&ret) string();
        return ret;
    }
    string& operator[] (id_type i)
    {
        RYML_ASSERT(m_size <= m_capacity);
        RYML_ASSERT(i < m_size);
        return m_arr[i];
    }
    string const& operator[] (id_type i) const
    {
        RYML_ASSERT(m_size <= m_capacity);
        RYML_ASSERT(i < m_size);
        return m_arr[i];
    }
};

} // namespace extra
} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP

#endif /* _C4_YML_EXTRA_STRING_HPP_ */
