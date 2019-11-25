#ifndef _C4_YML_COMMON_HPP_
#define _C4_YML_COMMON_HPP_

#include <cstddef>
#include <c4/common.hpp>

#define RYML_INLINE inline

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

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

/** the type of the function used to report errors. This function must
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
        C4_ASSERT(r != nullptr);
        r->free(mem, num_bytes);
    }
};


} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
