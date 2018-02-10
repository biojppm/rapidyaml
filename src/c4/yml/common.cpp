#include "./common.hpp"

#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS

namespace c4 {
namespace yml {

#ifndef RYML_NO_DEFAULT_CALLBACKS
namespace {
void* allocate_impl(size_t length, void * /*hint*/)
{
    void *mem = ::malloc(length);
    if(mem == nullptr)
    {
        C4_ERROR("could not allocate memory");
    }
    return mem;
}

void free_impl(void *mem, size_t /*length*/)
{
    ::free(mem);
}

void error_impl(const char* msg, size_t length)
{
    fprintf(stderr, "%.*s\n", (int)length, msg);
    abort();
}

allocate_callback s_allocate_fn = &allocate_impl;
free_callback s_free_fn = &free_impl;
error_callback s_error_fn = &error_impl;
} // empty namespace
#else
allocate_callback s_allocate_fn = nullptr;
free_callback s_free_fn = nullptr;
error_callback s_error_fn = nullptr;
#endif // RYML_NO_DEFAULT_CALLBACKS

//-----------------------------------------------------------------------------
void set_allocate_callback(allocate_callback fn)
{
#ifdef RYML_NO_DEFAULT_CALLBACKS
    s_allocate_fn = fn;
#else
    s_allocate_fn = fn ? fn : &allocate_impl;
#endif
}

allocate_callback get_allocate_callback()
{
    return s_allocate_fn;
}

void* allocate(size_t len, void *hint)
{
    return s_allocate_fn(len, hint);
}

//-----------------------------------------------------------------------------
void set_free_callback(free_callback fn)
{
#ifdef RYML_NO_DEFAULT_CALLBACKS
    s_free_fn = fn;
#else
    s_free_fn = fn ? fn : &free_impl;
#endif
}

free_callback get_free_callback()
{
    return s_free_fn;
}

void free(void *mem, size_t mem_len)
{
    s_free_fn(mem, mem_len);
}

//-----------------------------------------------------------------------------
void set_error_callback(error_callback fn)
{
#ifdef RYML_NO_DEFAULT_CALLBACKS
    s_error_fn = fn;
#else
    s_error_fn = fn ? fn : &error_impl;
#endif
}

error_callback get_error_callback()
{
    return s_error_fn;
}

void error(const char *msg, size_t msg_len)
{
    s_error_fn(msg, msg_len);
}

} // namespace yml
} // namespace c4
