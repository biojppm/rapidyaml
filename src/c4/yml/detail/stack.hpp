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

private:

    T      m_buf[N];
    T *    m_stack;
    size_t m_size;
    size_t m_capacity;

    Allocator m_alloc;

public:

    constexpr static bool is_contiguous() { return true; }

    stack() : m_stack(nullptr), m_size(0), m_capacity(0), m_alloc() {}
    stack(Allocator const& c) : m_stack(nullptr), m_size(0), m_capacity(0), m_alloc(c)
    {
    }
    ~stack()
    {
        if(m_stack != m_buf)
        {
            m_alloc.free(m_stack, m_capacity * sizeof(T));
        }
    }

    stack(stack const& that) : stack()
    {
        reserve(that.m_size);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_size);
    }
    stack& operator= (stack const& that)
    {
        if(m_stack != m_buf)
        {
            m_alloc.free(m_stack, m_capacity * sizeof(T));
        }
        reserve(that.m_size);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_size);
        return *this;
    }

    stack(stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        if(m_size > N) that.m_ptr = nullptr;
    }
    stack& operator= (stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        if(m_size > N) that.m_ptr = nullptr;
        return *this;
    }

public:

    size_t size() const { return m_size; }
    size_t empty() const { return m_size == 0; }
    size_t capacity() const { return m_capacity; }

    void clear()
    {
        m_size = 0;
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
        T *buf = (T*) m_alloc.allocate(sz * sizeof(T), m_stack);
        memcpy(buf, m_stack, m_size * sizeof(T));
        if(m_stack != m_buf)
        {
            m_alloc.free(m_stack, m_capacity * sizeof(T));
        }
        m_stack = buf;
        m_capacity = sz;
    }

    void push(T const& n)
    {
        if(m_size == m_capacity)
        {
            size_t cap = m_capacity == 0 ? N : 2 * m_capacity;
            reserve(cap);
        }
        m_stack[m_size] = n;
        m_size++;
    }

    T const& pop()
    {
        C4_ASSERT(m_size > 0);
        m_size--;
        return m_stack[m_size];
    }

    T const& top() const { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }
    T      & top()       { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }

    T const& bottom() const { C4_ASSERT(m_size > 0); return m_stack[0]; }
    T      & bottom()       { C4_ASSERT(m_size > 0); return m_stack[0]; }

    T const& top(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }
    T      & top(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }

    T const& bottom(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }
    T      & bottom(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }

    T const& operator[](size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }
    T      & operator[](size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }

    using iterator = T *;
    using const_iterator = T const *;

    iterator begin() { return m_stack; }
    iterator end  () { return m_stack + m_size; }

    const_iterator begin() const { return m_stack; }
    const_iterator end  () const { return m_stack + m_size; }
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_DETAIL_STACK_HPP_ */
