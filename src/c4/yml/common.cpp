#include "c4/yml/common.hpp"

#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS

namespace c4 {
namespace yml {

#ifndef RYML_NO_DEFAULT_CALLBACKS
namespace {

void error_impl(const char* msg, size_t length, void * /*user_data*/)
{
    fprintf(stderr, "%.*s\n", (int)length, msg);
    C4_DEBUG_BREAK();
    ::abort();
}

void* allocate_impl(size_t length, void * /*hint*/, void * /*user_data*/)
{
    void *mem = ::malloc(length);
    if(mem == nullptr)
    {
        const char msg[] = "could not allocate memory";
        error_impl(msg, sizeof(msg)-1, nullptr);
    }
    return mem;
}

void free_impl(void *mem, size_t /*length*/, void * /*user_data*/)
{
    ::free(mem);
}
} // empty namespace

Callbacks::Callbacks()
    :
    m_user_data(nullptr),
    m_allocate(&allocate_impl),
    m_free(&free_impl),
    m_error(&error_impl)
{
}

#else // RYML_NO_DEFAULT_CALLBACKS

Callbacks::Callbacks()
    :
    m_user_data(nullptr),
    m_allocate(nullptr),
    m_free(nullptr),
    m_error(nullptr)
{
}

#endif // RYML_NO_DEFAULT_CALLBACKS

Callbacks::Callbacks(void *user_data, pfn_allocate alloc, pfn_free free_, pfn_error error_)
    :
    m_user_data(user_data),
    m_allocate(alloc),
    m_free(free_),
    m_error(error_)
{
}

namespace {
Callbacks s_default_callbacks;
MemoryResourceCallbacks s_default_memory_resource;
MemoryResource* s_memory_resource = &s_default_memory_resource;
}

void set_callbacks(Callbacks const& c)
{
    s_default_callbacks = c;
}

Callbacks const& get_callbacks()
{
    return s_default_callbacks;
}

MemoryResource* get_memory_resource()
{
    return s_memory_resource;
}

void set_memory_resource(MemoryResource* r)
{
    s_memory_resource = r ? r : &s_default_memory_resource;
}

void error(const char *msg, size_t msg_len)
{
    s_default_callbacks.error(msg, msg_len);
}

} // namespace yml
} // namespace c4
