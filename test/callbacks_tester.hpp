#ifndef C4_TEST_CALLBACKS_TESTER_HPP_
#define C4_TEST_CALLBACKS_TESTER_HPP_

#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#endif
#include <vector>
#include <iostream>

namespace c4 {
namespace yml {

struct CallbacksTester
{
    std::vector<char> memory_pool;
    const char *id;
    size_t num_allocs, alloc_size;
    size_t num_deallocs, dealloc_size;

    CallbacksTester(const char *id_="notset", size_t sz=10u * 1024u) // 10KB
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
        std::cout << "size: alloc=" << alloc_size << " dealloc=" << dealloc_size << std::endl;
        std::cout << "count: #allocs=" << num_allocs << " #deallocs=" << num_deallocs << std::endl;
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
        std::cout << "alloc[" << num_allocs << "]=" << len << "B\n";
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
        std::cout << "free[" << num_deallocs << "]=" << len << "B\n";
        dealloc_size += len;
        ++num_deallocs;
        // no need to free here
    }
};

} // namespace yml
} // namespace c4

#endif /* C4_TEST_CALLBACKS_TESTER_HPP_ */
