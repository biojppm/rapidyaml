#ifndef _C4_VERSION_HPP_
#define _C4_VERSION_HPP_

/** @file version.hpp */

#define C4CORE_VERSION "0.2.7"
#define C4CORE_VERSION_MAJOR 0
#define C4CORE_VERSION_MINOR 2
#define C4CORE_VERSION_PATCH 7

#include <c4/export.hpp>

namespace c4 {

C4CORE_EXPORT const char* version();
C4CORE_EXPORT int version_major();
C4CORE_EXPORT int version_minor();
C4CORE_EXPORT int version_patch();

} // namespace c4

#endif /* _C4_VERSION_HPP_ */
