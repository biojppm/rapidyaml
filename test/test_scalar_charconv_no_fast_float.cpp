#ifdef RYML_SINGLE_HEADER
// The warning macros are not available until the amalgamated header is included.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // sscanf: this function may be unsafe
#endif
#include "ryml_all.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else
#include "c4/error.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // sscanf: this function may be unsafe
#include "c4/yml/scalar_charconv.hpp"
C4_SUPPRESS_WARNING_MSVC_POP
#endif
