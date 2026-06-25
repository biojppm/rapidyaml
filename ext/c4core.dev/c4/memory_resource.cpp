#include "c4/memory_resource.hpp"
#include "c4/memory_util.hpp"

#include <stdlib.h>
#include <string.h>
#if defined(C4_POSIX) || defined(C4_IOS) || defined(C4_MACOS) || defined(C4_ARM)
#   include <errno.h>
#endif
#if defined(C4_ARM)
#   include <malloc.h>
#endif

#include <memory>

namespace c4 {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

MemoryResourceMalloc s_mrmalloc; // NOLINT
thread_local static MemoryResource* s_mrcurr = nullptr; // NOLINT

void set_memory_resource(MemoryResource* mr)
{
    C4_ASSERT(mr != nullptr);
    s_mrcurr = mr;
}

MemoryResource* get_memory_resource()
{
    if(!s_mrcurr)
        s_mrcurr = &s_mrmalloc;
    return s_mrcurr;
}

MemoryResourceMalloc* get_memory_resource_malloc()
{
    return &s_mrmalloc;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void detail::_MemoryResourceSingleChunk::release()
{
    if(m_mem && m_owner)
    {
        impl_type::deallocate(m_mem, m_size);
    }
    m_mem = nullptr;
    m_size = 0;
    m_owner = false;
    m_pos = 0;
}

void detail::_MemoryResourceSingleChunk::acquire(size_t sz)
{
    clear();
    m_owner = true;
    m_mem = (char*) impl_type::allocate(sz, alignof(max_align_t));
    m_size = sz;
    m_pos = 0;
}

void detail::_MemoryResourceSingleChunk::acquire(void *mem, size_t sz)
{
    clear();
    m_owner = false;
    m_mem = (char*) mem;
    m_size = sz;
    m_pos = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void* MemoryResourceLinear::do_allocate(size_t sz, size_t alignment, void *hint)
{
    C4_UNUSED(hint);
    if(sz == 0) return nullptr;
    // make sure there's enough room to allocate
    if(m_pos + sz > m_size)
    {
        C4_ERROR("out of memory");
    }
    void *mem = m_mem + m_pos;
    size_t space = m_size - m_pos;
    if(std::align(alignment, sz, mem, space))
    {
        C4_ASSERT(m_pos <= m_size);
        C4_ASSERT(m_size - m_pos >= space);
        m_pos += (m_size - m_pos) - space;
        m_pos += sz;
        C4_ASSERT(m_pos <= m_size);
    }
    else
    {
        C4_ERROR("could not align memory");
    }
    return mem;
}

void MemoryResourceLinear::do_deallocate(void* ptr, size_t sz, size_t alignment)
{
    C4_UNUSED(ptr);
    C4_UNUSED(sz);
    C4_UNUSED(alignment);
    // nothing to do!!
}

void* MemoryResourceLinear::do_reallocate(void* ptr, size_t oldsz, size_t newsz, size_t alignment)
{
    if(newsz == oldsz) return ptr;
    // is ptr the most recently allocated (MRA) block?
    char *cptr = (char*)ptr;
    bool same_pos = (m_mem + m_pos == cptr + oldsz);
    // no need to get more memory when shrinking
    if(newsz < oldsz)
    {
        // if this is the MRA, we can safely shrink the position
        if(same_pos)
        {
            m_pos -= oldsz - newsz;
        }
        return ptr;
    }
    // we're growing the block, and it fits in size
    else if(same_pos && cptr + newsz <= m_mem + m_size)
    {
        // if this is the MRA, we can safely shrink the position
        m_pos += newsz - oldsz;
        return ptr;
    }
    // we're growing the block or it doesn't fit -
    // delegate any of these situations to do_deallocate()
    return do_allocate(newsz, alignment, ptr);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @todo add a free list allocator. A good candidate because of its
 * small size is TLSF.
 *
 * @see https://github.com/mattconte/tlsf
 *
 * Comparisons:
 *
 * @see https://www.researchgate.net/publication/262375150_A_Comparative_Study_on_Memory_Allocators_in_Multicore_and_Multithreaded_Applications_-_SBESC_2011_-_Presentation_Slides
 * @see http://webkit.sed.hu/blog/20100324/war-allocators-tlsf-action
 * @see https://github.com/emeryberger/Malloc-Implementations/tree/master/allocators
 *
 * */

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef C4_REDEFINE_CPPNEW
#include <new>
void* operator new(size_t size)
{
    auto *mr = ::c4::get_memory_resource();
    return mr->allocate(size);
}
void operator delete(void *p) noexcept
{
    C4_NEVER_REACH();
}
void operator delete(void *p, size_t size)
{
    auto *mr = ::c4::get_memory_resource();
    mr->deallocate(p, size);
}
void* operator new[](size_t size)
{
    return operator new(size);
}
void operator delete[](void *p) noexcept
{
    operator delete(p);
}
void operator delete[](void *p, size_t size)
{
    operator delete(p, size);
}
void* operator new(size_t size, std::nothrow_t)
{
    return operator new(size);
}
void operator delete(void *p, std::nothrow_t)
{
    operator delete(p);
}
void operator delete(void *p, size_t size, std::nothrow_t)
{
    operator delete(p, size);
}
void* operator new[](size_t size, std::nothrow_t)
{
    return operator new(size);
}
void operator delete[](void *p, std::nothrow_t)
{
    operator delete(p);
}
void operator delete[](void *p, size_t, std::nothrow_t)
{
    operator delete(p, size);
}
#endif // C4_REDEFINE_CPPNEW
