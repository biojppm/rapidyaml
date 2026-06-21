#include "c4/alloc.hpp"
#include "c4/memory_util.hpp"

#include <stdlib.h>
#include <string.h>
#if defined(C4_POSIX) || defined(C4_IOS) || defined(C4_MACOS) || defined(C4_ARM)
#   include <errno.h>
#endif
#if defined(C4_ARM)
#   include <malloc.h>
#endif

namespace c4 {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

namespace detail {


#ifdef C4_NO_ALLOC_DEFAULTS
aalloc_pfn s_aalloc = nullptr;
free_pfn s_afree = nullptr;
arealloc_pfn s_arealloc = nullptr;
#else


void afree_impl(void *ptr) // NOLINT(misc-use-internal-linkage)
{
#if defined(C4_WIN) || defined(C4_XBOX)
    ::_aligned_free(ptr);
#else
    ::free(ptr);
#endif
}


void* aalloc_impl(size_t size, size_t alignment) // NOLINT(misc-use-internal-linkage)
{
    // alignment must be nonzero and a power of 2
    C4_CHECK(alignment > 0 && (alignment & (alignment - 1u)) == 0);
    // NOTE: alignment needs to be sized in multiples of sizeof(void*)
    if(C4_UNLIKELY(alignment < sizeof(void*)))
        alignment = sizeof(void*);
    static_assert((sizeof(void*) & (sizeof(void*)-1u)) == 0, "sizeof(void*) must be a power of 2");
    C4_CHECK(((alignment & (sizeof(void*) - 1u))) == 0u);
    void *mem;
#if defined(C4_WIN) || defined(C4_XBOX)
    mem = ::_aligned_malloc(size, alignment);
    C4_CHECK(mem != nullptr || size == 0);
#elif defined(C4_POSIX) || defined(C4_IOS) || defined(C4_MACOS)
    int ret = ::posix_memalign(&mem, alignment, size);
    if(C4_UNLIKELY(ret))
    {
        C4_ASSERT(ret != EINVAL); // this was already handled above
        if(ret == ENOMEM)
        {
            C4_ERROR("There was insufficient memory to fulfill the "
                     "allocation request of %zu bytes (alignment=%lu)", size, size);
        }
        return nullptr; // LCOV_EXCL_LINE
    }
#elif defined(C4_ARM) || defined(C4_ANDROID)
    // https://stackoverflow.com/questions/53614538/undefined-reference-to-posix-memalign-in-arm-gcc
    // https://electronics.stackexchange.com/questions/467382/e2-studio-undefined-reference-to-posix-memalign/467753
    mem = memalign(alignment, size);
    C4_CHECK(mem != nullptr || size == 0);
#else
    (void)size;
    (void)alignment;
    mem = nullptr;
    C4_NOT_IMPLEMENTED_MSG("need to implement an aligned allocation for this platform");
#endif
    C4_ASSERT_MSG((uintptr_t(mem) & (alignment-1)) == 0, "address %p is not aligned to %zu boundary", mem, alignment);
    return mem;
}


C4CORE_EXPORT void* arealloc_impl(void* ptr, size_t oldsz, size_t newsz, size_t alignment) // NOLINT(misc-use-internal-linkage)
{
    /** @todo make this more efficient
     * @see https://stackoverflow.com/questions/9078259/does-realloc-keep-the-memory-alignment-of-posix-memalign
     * @see look for qReallocAligned() in http://code.qt.io/cgit/qt/qtbase.git/tree/src/corelib/global/qmalloc.cpp
     */
    void *tmp = aalloc(newsz, alignment);
    size_t min = newsz < oldsz ? newsz : oldsz;
    if(mem_overlaps(ptr, tmp, oldsz, newsz))
    {
        ::memmove(tmp, ptr, min); // LCOV_EXCL_LINE
    }
    else
    {
        ::memcpy(tmp, ptr, min);
    }
    afree(ptr);
    return tmp;
}

aalloc_pfn s_aalloc = aalloc_impl;       // NOLINT(misc-use-internal-linkage)
afree_pfn s_afree = afree_impl;          // NOLINT(misc-use-internal-linkage)
arealloc_pfn s_arealloc = arealloc_impl; // NOLINT(misc-use-internal-linkage)

#endif // C4_NO_ALLOC_DEFAULTS

} // namespace detail


aalloc_pfn get_aalloc()
{
    return detail::s_aalloc;
}
void set_aalloc(aalloc_pfn fn)
{
    detail::s_aalloc = fn;
}

afree_pfn get_afree()
{
    return detail::s_afree;
}
void set_afree(afree_pfn fn)
{
    detail::s_afree = fn;
}

arealloc_pfn get_arealloc()
{
    return detail::s_arealloc;
}
void set_arealloc(arealloc_pfn fn)
{
    detail::s_arealloc = fn;
}


void* aalloc(size_t sz, size_t alignment)
{
    C4_ASSERT_MSG(c4::get_aalloc() != nullptr, "did you forget to call set_aalloc()?");
    auto fn = c4::get_aalloc();
    void* ptr = fn(sz, alignment);
    return ptr;
}

void afree(void* ptr)
{
    C4_ASSERT_MSG(c4::get_afree() != nullptr, "did you forget to call set_afree()?");
    auto fn = c4::get_afree();
    fn(ptr);
}

void* arealloc(void *ptr, size_t oldsz, size_t newsz, size_t alignment)
{
    C4_ASSERT_MSG(c4::get_arealloc() != nullptr, "did you forget to call set_arealloc()?");
    auto fn = c4::get_arealloc();
    void* nptr = fn(ptr, oldsz, newsz, alignment);
    return nptr;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace c4
