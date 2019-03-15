#ifndef _C4_YML_DETAIL_STACK_HPP_
#define _C4_YML_DETAIL_STACK_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "../common.hpp"
#endif

#ifdef RYML_DBG
#   include <type_traits>
#endif

namespace c4 {
namespace yml {

namespace detail {
template<class T, size_t N> class stack;
} // namespace detail

/** A lightweight contiguous stack with SSO. This avoids a dependency on std. */
template<class T, size_t N=16>
class detail::stack
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
    static_assert(std::is_trivially_destructible<T>::value, "T must be trivially destructible");

    enum : size_t { sso_size = N };

private:

    T         m_buf[N];
    T *       m_stack;
    size_t    m_size;
    size_t    m_capacity;
    Allocator m_alloc;

public:

    constexpr static bool is_contiguous() { return true; }

    stack() : m_stack(m_buf), m_size(0), m_capacity(N), m_alloc() {}
    stack(Allocator const& c) : m_stack(m_buf), m_size(0), m_capacity(N), m_alloc(c)
    {
    }
    ~stack()
    {
        _free();
    }

    stack(stack const& that) : stack()
    {
        resize(that.m_size);
        memcpy(m_stack, that.m_stack, that.m_size * sizeof(T));
    }
    stack& operator= (stack const& that)
    {
        resize(that.m_size);
        memcpy(m_stack, that.m_stack, that.m_size * sizeof(T));
        return *this;
    }

    stack(stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        that._yield();
    }
    stack& operator= (stack &&that)
    {
        _free();
        memcpy(this, &that, sizeof(*this));
        that._yield();
        return *this;
    }

public:

    void _free()
    {
        C4_ASSERT(m_stack != nullptr); // this structure cannot be memset() to zero
        if(m_stack != m_buf)
        {
            C4_ASSERT(m_capacity > N);
            m_alloc.free(m_stack, m_capacity * sizeof(T));
        }
        else
        {
            C4_ASSERT(m_capacity == N);
        }
    }

    void _yield()
    {
        // make sure no deallocation happens on destruction
        if(m_size <= N) return;
        C4_ASSERT(m_stack != m_buf);
        m_stack = m_buf;
        m_capacity = N;
    }

public:

    size_t size() const { return m_size; }
    size_t empty() const { return m_size == 0; }
    size_t capacity() const { return m_capacity; }

    void clear()
    {
        m_size = 0;
    }

    void resize(size_t sz)
    {
        reserve(sz);
        m_size = sz;
    }

    void reserve(size_t sz)
    {
        if(sz <= m_size) return;
        if(sz <= N)
        {
            m_stack = m_buf;
            m_capacity = N;
            return;
        }
        C4_ASSERT(false);
        T *buf = (T*) m_alloc.allocate(sz * sizeof(T), m_stack);
        memcpy(buf, m_stack, m_size * sizeof(T));
        if(m_stack != m_buf)
        {
            m_alloc.free(m_stack, m_capacity * sizeof(T));
        }
        m_stack = buf;
        m_capacity = sz;
    }

    void push(T const& C4_RESTRICT n)
    {
        if(m_size == m_capacity)
        {
            size_t cap = m_capacity == 0 ? N : 2 * m_capacity;
            reserve(cap);
        }
        m_stack[m_size] = n;
        ++m_size;
    }

    T const& C4_RESTRICT pop()
    {
        C4_ASSERT(m_size > 0);
        --m_size;
        return m_stack[m_size];
    }

    C4_ALWAYS_INLINE T const& C4_RESTRICT top() const { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }
    C4_ALWAYS_INLINE T      & C4_RESTRICT top()       { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }

    C4_ALWAYS_INLINE T const& C4_RESTRICT bottom() const { C4_ASSERT(m_size > 0); return m_stack[0]; }
    C4_ALWAYS_INLINE T      & C4_RESTRICT bottom()       { C4_ASSERT(m_size > 0); return m_stack[0]; }

    C4_ALWAYS_INLINE T const& C4_RESTRICT top(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }
    C4_ALWAYS_INLINE T      & C4_RESTRICT top(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }

    C4_ALWAYS_INLINE T const& C4_RESTRICT bottom(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }
    C4_ALWAYS_INLINE T      & C4_RESTRICT bottom(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }

    C4_ALWAYS_INLINE T const& C4_RESTRICT operator[](size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }
    C4_ALWAYS_INLINE T      & C4_RESTRICT operator[](size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }

    using       iterator = T       * C4_RESTRICT;
    using const_iterator = T const * C4_RESTRICT;

    iterator begin() { return m_stack; }
    iterator end  () { return m_stack + m_size; }

    const_iterator begin() const { return m_stack; }
    const_iterator end  () const { return m_stack + m_size; }
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_DETAIL_STACK_HPP_ */
