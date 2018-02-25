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

//-----------------------------------------------------------------------------

/** the type of the function used to report errors. This function must abort
 * interrupt execution, either through abort() or by raising an exception. */
using pfn_error = void (*)(const char* msg, size_t msg_len, void *user_data);

void error(const char *msg, size_t msg_len);

template< size_t N >
inline void error(const char (&msg)[N])
{
    error(msg, N-1);
}

//-----------------------------------------------------------------------------

/** the type of the function used to allocate memory */
using pfn_allocate = void* (*)(size_t len, void* hint, void *user_data);
/** the type of the function used to free memory */
using pfn_free = void (*)(void* mem, size_t size, void *user_data);

/// a c-style callbacks class
struct Callbacks
{
    void *       m_user_data;
    pfn_allocate m_allocate;
    pfn_free     m_free;
    pfn_error    m_error;

    Callbacks();
    Callbacks(void *user_data, pfn_allocate alloc, pfn_free free, pfn_error);

    inline void* allocate(size_t len, void* hint) const
    {
        void* mem = m_allocate(len, hint, m_user_data);
        if(mem == nullptr)
        {
            this->error("out of memory");
        }
        return mem;
    }

    inline void free(void *mem, size_t len) const
    {
        m_free(mem, len, m_user_data);
    }

    void error(const char *msg, size_t msg_len) const
    {
        m_error(msg, msg_len, m_user_data);
    }

    template< size_t N >
    inline void error(const char (&msg)[N]) const
    {
        error(msg, N-1);
    }

};

/// set the global callbacks
void set_callbacks(Callbacks const& c);
/// get the global callbacks
Callbacks const& get_callbacks();

//-----------------------------------------------------------------------------

class MemoryResource
{
public:

    virtual ~MemoryResource() = default;

    virtual void * allocate(size_t num_bytes, void *hint) = 0;
    virtual void   free(void *mem, size_t num_bytes) = 0;
};

/// set the global memory resource
void set_memory_resource(MemoryResource *r);
/// get the global memory resource 
MemoryResource *get_memory_resource();

//-----------------------------------------------------------------------------

// a memory resource adapter to the c-style allocator
class MemoryResourceCallbacks : public MemoryResource
{
public:

    Callbacks cb;

    MemoryResourceCallbacks() : cb(get_callbacks()) {}
    MemoryResourceCallbacks(Callbacks const& c) : cb(c) {}

    void* allocate(size_t len, void* hint) override final
    {
        return cb.allocate(len, hint);
    }

    void free(void *mem, size_t len) override final
    {
        return cb.free(mem, len);
    }
};

//-----------------------------------------------------------------------------

/** an allocator is a lightweight non-owning handle to a memory resource */
struct Allocator
{
    MemoryResource *r;
    Allocator() : r(get_memory_resource()) {}
    Allocator(MemoryResource *m) : r(m) {}

    inline void *allocate(size_t num_bytes, void *hint)
    {
        void *mem = r->allocate(num_bytes, hint);
        if(mem == nullptr)
        {
            error("out of memory");
        }
        return mem;
    }

    inline void free(void *mem, size_t num_bytes)
    {
        r->free(mem, num_bytes);
    }
};


} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
