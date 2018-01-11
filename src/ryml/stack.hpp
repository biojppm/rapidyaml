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
    size_t m_pos;
    size_t m_size;

public:

    stack() : m_stack(nullptr), m_pos(0), m_size(0)
    {
    }
    ~stack()
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
    }

    stack(stack const& that) : stack()
    {
        reserve(that.m_pos);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_pos);
    }
    stack& operator= (stack const& that)
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
        reserve(that.m_pos);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_pos);
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

    size_t size() const { return m_pos; }
    size_t empty() const { return m_pos == 0; }

    void reserve(size_t sz)
    {
        if(sz == 0) sz = 8;
        C4_ASSERT(m_pos < sz);
        T *buf = (T*) RymlCallbacks::allocate(sz * sizeof(T), m_stack);
        if(m_stack)
        {
            memcpy(buf, m_stack, m_pos * sizeof(T));
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
        m_stack = buf;
        m_size = sz;
    }
    void push(T n)
    {
        if(m_pos >= m_size)
        {
            reserve(2 * m_size);
        }
        m_stack[m_pos] = n;
        m_pos++;
        //printf("stack_push[%zd]: %zd\n", m_pos, n);
    }

    T pop()
    {
        //printf("stack_pop[%zd]: %zd\n", m_pos, m_stack[m_pos - 1]);
        C4_ASSERT(m_pos > 0);
        m_pos--;
        T n = m_stack[m_pos];
        return n;
    }

    T peek() const
    {
        C4_ASSERT(m_pos > 0);
        T n = m_stack[m_pos - 1];
        return n;
    }

};

} // namespace yml
} // namespace c4

#endif /* _C4_RYML_STACK_HPP_ */
