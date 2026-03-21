
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#else
#include <ryml_all.hpp>
#endif


namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

namespace {
#ifdef RYML_NO_DEFAULT_CALLBACKS
[[noreturn]] void error_basic_impl(csubstr msg, ErrorDataBasic const& errdata, void * /*user_data*/)
{
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionBasic(msg, errdata);
    #else
    (void)msg;
    (void)errdata;
    abort();
    #endif
}
[[noreturn]] void error_parse_impl(csubstr msg, ErrorDataParse const& errdata, void * /*user_data*/)
{
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionParse(msg, errdata);
    #else
    (void)msg;
    (void)errdata;
    abort();
    #endif
}
[[noreturn]] void error_visit_impl(csubstr msg, ErrorDataVisit const& errdata, void * /*user_data*/)
{
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionVisit(msg, errdata);
    #else
    (void)msg;
    (void)errdata;
    abort();
    #endif
}
void* allocate_impl(size_t length, void * /*hint*/, void * /*user_data*/)
{
    void *mem = ::malloc(length);
    if(mem == nullptr)
        error_basic_impl("could not allocate memory", ErrorDataBasic{RYML_LOC_HERE()}, nullptr); // LCOV_EXCL_LINE
    return mem;
}
void free_impl(void *mem, size_t /*length*/, void * /*user_data*/)
{
    ::free(mem);
}
#endif // RYML_NO_DEFAULT_CALLBACKS
} // namespace

void ensure_callbacks()
{
    #ifdef RYML_NO_DEFAULT_CALLBACKS
    printf("setting default callbacks\n");
    Callbacks cb;
    cb.set_allocate(&allocate_impl);
    cb.set_free(&free_impl);
    cb.set_error_basic(&error_basic_impl);
    cb.set_error_parse(&error_parse_impl);
    cb.set_error_visit(&error_visit_impl);
    c4::yml::set_callbacks(cb);
    #endif
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4


