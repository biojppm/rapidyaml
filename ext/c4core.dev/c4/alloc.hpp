#ifndef C4_ALLOC_HPP_
#define C4_ALLOC_HPP_

/** @file alloc.hpp Provides c-style facilities to allocate raw
 *  memory. This API provides aligned allocation functions, forwarding
 *  the call to a user-modifiable function. */

/** @defgroup memory memory utilities */

/** @defgroup raw_memory_alloc Raw memory allocation
 * @ingroup memory
 */

#include "c4/export.hpp"
#include <stddef.h>

namespace c4 {


// aligned allocation.

/** Aligned allocation. Merely calls the current get_aalloc() function.
 * @see get_aalloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void* aalloc(size_t sz, size_t alignment);

/** Aligned free. Merely calls the current get_afree() function.
 * @see get_afree()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void afree(void* ptr);

/** Aligned reallocation. Merely calls the current get_arealloc() function.
 * @see get_arealloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void* arealloc(void* ptr, size_t oldsz, size_t newsz, size_t alignment);



// allocation setup facilities: function pointer setters/getters

/** Function pointer type for aligned allocation
 * @see set_aalloc()
 * @ingroup raw_memory_alloc */
using aalloc_pfn = void* (*)(size_t size, size_t alignment);

/** Function pointer type for aligned deallocation
 * @see set_afree()
 * @ingroup raw_memory_alloc */
using afree_pfn = void  (*)(void *ptr);

/** Function pointer type for aligned reallocation
 * @see set_arealloc()
 * @ingroup raw_memory_alloc */
using arealloc_pfn = void* (*)(void *ptr, size_t oldsz, size_t newsz, size_t alignment);



/** Set the global aligned allocation function.
 * @see aalloc()
 * @see get_aalloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void set_aalloc(aalloc_pfn fn);

/** Set the global aligned deallocation function.
 * @see afree()
 * @see get_afree()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void set_afree(afree_pfn fn);

/** Set the global aligned reallocation function.
 * @see arealloc()
 * @see get_arealloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT void set_arealloc(arealloc_pfn fn);



/** Get the global aligned reallocation function.
 * @see arealloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT aalloc_pfn get_aalloc();

/** Get the global aligned deallocation function.
 * @see afree()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT afree_pfn get_afree();

/** Get the global aligned reallocation function.
 * @see arealloc()
 * @ingroup raw_memory_alloc */
C4CORE_EXPORT arealloc_pfn get_arealloc();

} // namespace c4

#endif
