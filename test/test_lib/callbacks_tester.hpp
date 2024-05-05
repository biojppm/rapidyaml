#ifndef C4_TEST_CALLBACKS_TESTER_HPP_
#define C4_TEST_CALLBACKS_TESTER_HPP_

#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include <c4/charconv.hpp>
#include "c4/yml/detail/parser_dbg.hpp"
#else
#endif
#include <vector>
#include <iostream>

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wcast-qual")

struct CallbacksTester
{
    std::vector<char> memory_pool;
    const char *id;
    size_t num_allocs, alloc_size;
    size_t num_deallocs, dealloc_size;

    CallbacksTester(const char *id_="notset", size_t sz=30u * 1024u) // 30KB
        : memory_pool(sz)
        , id(id_)
        , num_allocs()
        , alloc_size()
        , num_deallocs()
        , dealloc_size()
    {
    }

    // checking
    ~CallbacksTester()
    {
        check();
    }

    void check()
    {
        _c4dbgpf("size: alloc={}  dealloc={}", alloc_size, dealloc_size);
        _c4dbgpf("count: #allocs={}  #deallocs={}", num_allocs, num_deallocs);
        RYML_CHECK(num_allocs == num_deallocs);
        RYML_CHECK(alloc_size == dealloc_size);
    }

    Callbacks callbacks() const
    {
        Callbacks cb = get_callbacks();
        cb.m_user_data = (void*) this;
        cb.m_allocate = [](size_t len, void *, void *data){ return ((CallbacksTester*) data)->allocate(len); };
        cb.m_free = [](void *mem, size_t len, void *data){ return ((CallbacksTester*) data)->free(mem, len); };
        return cb;
    }

    void *allocate(size_t len)
    {
        _c4dbgpf("alloc[{}]={}B", num_allocs, len);
        void *ptr = &memory_pool[alloc_size];
        alloc_size += len;
        ++num_allocs;
        RYML_CHECK(alloc_size < memory_pool.size());
        return ptr;
    }

    void free(void *mem, size_t len)
    {
        RYML_CHECK((char*)mem     >= &memory_pool.front() && (char*)mem     <  &memory_pool.back());
        RYML_CHECK((char*)mem+len >= &memory_pool.front() && (char*)mem+len <= &memory_pool.back());
        _c4dbgpf("free[{}]={}B", num_deallocs, len);
        dealloc_size += len;
        ++num_deallocs;
        // no need to free here
    }
};

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4

#endif /* C4_TEST_CALLBACKS_TESTER_HPP_ */
