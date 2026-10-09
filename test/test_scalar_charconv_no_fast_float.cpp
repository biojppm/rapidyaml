#ifdef RYML_SINGLE_HEADER
// The warning macros are not available until the amalgamated header is included.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996) // sscanf: this function may be unsafe
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "ryml_all.hpp"
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#else
#include "c4/error.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // sscanf: this function may be unsafe
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated-declarations") // sscanf is deprecated
#include "c4/yml/scalar_charconv.hpp"
C4_SUPPRESS_WARNING_CLANG_POP
C4_SUPPRESS_WARNING_MSVC_POP
#endif
