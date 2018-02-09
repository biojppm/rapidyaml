#ifndef _C4_YML_COMMON_HPP_
#define _C4_YML_COMMON_HPP_


#define RYML_DBG
#define RYML_ERRMSG_SIZE 1024

#define RYML_INLINE inline



//#define RYML_NO_DEFAULT_CALLBACKS
#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS

#ifndef C4_QUOTE
#   define C4_QUOTE(x) #x
#   define C4_XQUOTE(x) C4_QUOTE(x)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#ifndef C4_ERROR
#   define C4_ERROR(msg) \
    c4::yml::RymlCallbacks::error(__FILE__ ":" C4_XQUOTE(__LINE__) ": fatal error: " msg "\n")
#endif

#ifndef C4_ASSERT
#   ifdef NDEBUG
#       define C4_ASSERT(expr) (void)(0)
#   else
#       ifndef C4_DEBUG_BREAK  /* generates SIGTRAP. This assumes x86. Disable at will. */
#           define C4_DEBUG_BREAK() asm("int $3")
#       endif
#       include <assert.h>
#       define C4_ASSERT(expr)                                          \
    {                                                                   \
        if( ! (expr))                                                   \
        {                                                               \
            C4_DEBUG_BREAK();                                           \
            c4::yml::RymlCallbacks::error(__FILE__ ":" C4_XQUOTE(__LINE__) ": Assert failed: " #expr "\n"); \
        }                                                               \
    }
#   endif
#endif

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {

/** a null position */
enum : size_t { npos = size_t(-1) };

/** an index to none */
enum : size_t { NONE = size_t(-1) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef RYML_NO_DEFAULT_CALLBACKS
struct RymlCallbacks
{
    static void* allocate(size_t length, void * /*hint*/)
    {
        void *mem = ::malloc(length);
        if(mem == nullptr)
        {
            const char msg[] = "could not allocate memory";
            error(msg, sizeof(msg));
        }
        return mem;
    }

    static void free(void *mem, size_t /*length*/)
    {
        ::free(mem);
    }

    static void error(const char* msg, size_t length)
    {
        fprintf(stderr, "%.*s\n", (int)length, msg);
        abort();
    }
    template< size_t N >
    static void error(char const (&msg)[N])
    {
        error(&msg[0], N-1);
    }
};
#endif // RYML_NO_DEFAULT_CALLBACKS

} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
