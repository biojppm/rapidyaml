#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse_engine.def.hpp"
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_helpers_tree.hpp"
#include "./test_lib/callbacks_tester.hpp"

#include <unordered_map>
#include <vector>

C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated-declarations")
C4_SUPPRESS_WARNING_MSVC(4996) // deprecated
C4_SUPPRESS_WARNING_MSVC(4389) // signed/unsigned mismatch
C4_SUPPRESS_WARNING_CLANG("-Wdollar-in-identifier-extension")

namespace c4 {
namespace yml {

template<class Function>
C4_NO_INLINE void verify_success_(Tree &tree, Function &&fn)
{
    ExpectError::check_success(&tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
C4_NO_INLINE void verify_success_(csubstr src, Function &&fn)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_success(&tree, [&]{
        (void)fn(tree);
    });
}

template<class Function>
C4_NO_INLINE void verify_assertion_(Tree &tree, Function &&fn, ExpectedErrorType errtype=ExpectedErrorType::err_basic)
{
    ExpectError::check_assert(errtype, &tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
C4_NO_INLINE void verify_assertion_(csubstr src, Function &&fn, ExpectedErrorType errtype=ExpectedErrorType::err_basic)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_assert(errtype, &tree, [&]{
        (void)fn(tree);
    });
}

template<class Function>
C4_NO_INLINE void verify_error_(Tree &tree, Function &&fn, ExpectedErrorType errtype=ExpectedErrorType::err_basic)
{
    ExpectError::check_error(errtype, &tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
C4_NO_INLINE void verify_error_(csubstr src, Function &&fn, ExpectedErrorType errtype=ExpectedErrorType::err_basic)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_error(errtype, &tree, [&]{
        (void)fn(tree);
    });
}

#define verify_success(...)                     \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_success_(__VA_ARGS__);           \
    }

#define verify_assertion(...)                   \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_assertion_(__VA_ARGS__);         \
    }

#define verify_error(...)                       \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_error_(__VA_ARGS__);             \
    }

} // namespace yml
} // namespace c4
