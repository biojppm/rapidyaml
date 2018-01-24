#ifndef _C4_RYML_STACK_HPP_
#define _C4_RYML_STACK_HPP_

#include "./common.hpp"

namespace c4 {
namespace yml {

namespace detail {
template< class T > class stack;
} // namespace detail

/** A lightweight stack. This avoids a dependency on std. */
template< class T >
class detail::stack
{
private:

    T *    m_stack;
    size_t m_size;
    size_t m_capacity;

public:

    constexpr static bool is_contiguous() { return true; }

    stack() : m_stack(nullptr), m_size(0), m_capacity(0)
    {
    }
    ~stack()
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_capacity * sizeof(T));
        }
    }

    stack(stack const& that) : stack()
    {
        reserve(that.m_size);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_size);
    }
    stack& operator= (stack const& that)
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_capacity * sizeof(T));
        }
        reserve(that.m_size);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_size);
        return *this;
    }

    stack(stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        that.m_stack = nullptr;
    }
    stack& operator= (stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        that.m_stack = nullptr;
        return *this;
    }

    size_t size() const { return m_size; }
    size_t empty() const { return m_size == 0; }
    size_t capacity() const { return m_capacity; }

    void clear()
    {
        m_size = 0;
    }

    void reserve(size_t sz)
    {
        if(sz == 0) sz = 8;
        if(sz <= m_size) return;
        T *buf = (T*) RymlCallbacks::allocate(sz * sizeof(T), m_stack);
        if(m_stack)
        {
            memcpy(buf, m_stack, m_size * sizeof(T));
            RymlCallbacks::free(m_stack, m_capacity * sizeof(T));
        }
        m_stack = buf;
        m_capacity = sz;
    }

    void push(T const& n)
    {
        if(m_size >= m_capacity)
        {
            reserve(2 * m_capacity);
        }
        m_stack[m_size] = n;
        m_size++;
        //printf("stack_push[%zd]: %zd\n", m_size, n);
    }

    T const& pop()
    {
        //printf("stack_pop[%zd]: %zd\n", m_size, m_stack[m_size - 1]);
        C4_ASSERT(m_size > 0);
        m_size--;
        return m_stack[m_size];
    }

    T const& top() const { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }
    T      & top()       { C4_ASSERT(m_size > 0); return m_stack[m_size - 1]; }

    T const& top(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }
    T      & top(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[m_size - 1 - i]; }

    T const& bottom() const { C4_ASSERT(m_size > 0); return m_stack[0]; }
    T      & bottom()       { C4_ASSERT(m_size > 0); return m_stack[0]; }

    T const& bottom(size_t i) const { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }
    T      & bottom(size_t i)       { C4_ASSERT(i >= 0 && i < m_size); return m_stack[i]; }

    using iterator = T *;
    using const_iterator = T const *;

    iterator begin() { return m_stack; }
    iterator end  () { return m_stack + m_size; }

    const_iterator begin() const { return m_stack; }
    const_iterator end  () const { return m_stack + m_size; }
};

} // namespace yml
} // namespace c4

#endif /* _C4_RYML_STACK_HPP_ */
