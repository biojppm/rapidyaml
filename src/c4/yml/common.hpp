#ifndef _C4_YML_COMMON_HPP_
#define _C4_YML_COMMON_HPP_

#include <cstddef>

#define RYML_INLINE inline

#ifndef C4_QUOTE
#   define C4_QUOTE(x) #x
#   define C4_XQUOTE(x) C4_QUOTE(x)
#endif

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#ifndef C4_DEBUG_BREAK  /* generates SIGTRAP. This assumes x86. Disable at will. */
#    ifdef _MSC_VER
#        define C4_DEBUG_BREAK() __debugbreak()
#    else
#        define C4_DEBUG_BREAK() asm("int $3")
#    endif
#endif

#ifndef C4_ASSERT
#   ifdef NDEBUG
#       define C4_ASSERT(expr) (void)(0)
#   else
#       include <assert.h>
#       define C4_ASSERT(expr)                                          \
    {                                                                   \
        if( ! (expr))                                                   \
        {                                                               \
            C4_DEBUG_BREAK();                                           \
            c4::yml::error(__FILE__ ":" C4_XQUOTE(__LINE__) ": assert failed: " #expr "\n"); \
        }                                                               \
    }
#   endif
#endif


#ifndef C4_ERROR
#   define C4_ERROR(msg) \
    c4::yml::error(__FILE__ ":" C4_XQUOTE(__LINE__) ": fatal error: " msg "\n")
#endif

#define C4_ERROR_IF(cond, msg)         \
    if(cond)                           \
    {                                  \
        C4_ERROR(msg);                 \
    }

#define C4_ERROR_IF_NOT(cond, msg)     \
    if(!(cond))                        \
    {                                  \
        C4_ERROR(msg);                 \
    }

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {

/** a null position */
enum : size_t { npos = size_t(-1) };

/** an index to none */
enum : size_t { NONE = size_t(-1) };

/** the type of the function used to allocate memory */
using allocate_callback = void* (*)(size_t len, void* hint);
/** the type of the function used to free memory */
using free_callback = void (*)(void* mem, size_t size);
/** the type of the function used to report errors */
using error_callback = void (*)(const char* msg, size_t msg_len);

void set_allocate_callback(allocate_callback fn);
allocate_callback get_allocate_callback();
void* allocate(size_t len, void *hint);

void set_free_callback(free_callback fn);
free_callback get_free_callback();
void free(void *mem, size_t mem_len);

void set_error_callback(error_callback fn);
error_callback get_error_callback();
void error(const char *msg, size_t msg_len);
template< size_t N >
inline void error(const char (&msg)[N])
{
    error(msg, N-1);
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
