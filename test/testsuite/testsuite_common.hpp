#ifndef C4_YML_TEST_SUITE_COMMON_HPP_
#define C4_YML_TEST_SUITE_COMMON_HPP_

#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/dump.hpp>
#endif

#include <c4/fs/fs.hpp>

#include "test_lib/test_case.hpp"
#include <gtest/gtest.h>
#include <cstdio>

#if defined(C4_MSVC) || defined(C4_MINGW)
#include <malloc.h>
#elif (defined(__clang__) && defined(_MSC_VER)) || \
      defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#if (0 || defined(RYML_DBG))
#define RYML_NFO (1)
#else
#define RYML_NFO (0)
#endif

#if RYML_NFO
#define _nfo_print_tree(title, tree) do { c4::yml::tslog_("{}:{}: " title ":", __FILE__, __LINE__); print_tree(tree); c4::yml::emit_yaml(tree, stdout); fflush(stdout); } while(0)
#define _nfo_logf(fmt, ...)          do { c4::yml::tslog_("{}:{}: " fmt      , __FILE__, __LINE__, __VA_ARGS__);                                        fflush(stdout); } while(0)
#define _nfo_log(fmt)                do { c4::yml::tslog_("{}:{}: " fmt      , __FILE__, __LINE__);                                                     fflush(stdout); } while(0)
#define _nfo_printf(...) printf(__VA_ARGS__)
#else
#define _nfo_print_tree(title, tree)
#define _nfo_logf(fmt, ...)
#define _nfo_log(fmt)
#define _nfo_printf(...)
#endif
#define _nfo_llogf(fmt, ...) _nfo_logf("line[{}]: '{}': " fmt, linenum, line, __VA_ARGS__)
#define _nfo_llog(fmt)       _nfo_logf("line[{}]: '{}': " fmt, linenum, line)


namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")

inline C4_NO_INLINE void tsdumper_(csubstr s)
{
    if(s.len)
        (void)fwrite(s.str, 1, s.len, stdout);
}
template<class DumpFn, class ...Args>
C4_NO_INLINE void tsfmt_(DumpFn &&dumpfn, csubstr fmt, Args&& ...args)
{
    DumpResults results;
    // try writing everything:
    {
        // buffer for converting individual arguments. it is defined
        // in a child scope to free it in case the buffer is too small
        // for any of the arguments.
        char writebuf[RYML_LOGBUF_SIZE];
        results = c4::format_dump_resume(std::forward<DumpFn>(dumpfn), writebuf, fmt, std::forward<Args>(args)...);
    }
    // if any of the arguments failed to fit the buffer, allocate a
    // larger buffer (with alloca(), up to a limit) and resume writing.
    //
    // results.bufsize is set to the size of the largest element
    // serialized. Eg int(1) will require 1 byte.
    if C4_UNLIKELY (results.bufsize > RYML_LOGBUF_SIZE)
    {
        const size_t bufsize = results.bufsize <= RYML_LOGBUF_SIZE_MAX ? results.bufsize : RYML_LOGBUF_SIZE_MAX;
        #ifdef C4_MSVC
        substr largerbuf = {static_cast<char*>(_alloca(bufsize)), bufsize};
        #else
        substr largerbuf = {static_cast<char*>(alloca(bufsize)), bufsize};
        #endif
        results = c4::format_dump_resume(std::forward<DumpFn>(dumpfn), results, largerbuf, fmt, std::forward<Args>(args)...);
    }
}
template<class DumpFn, class ...Args>
C4_NO_INLINE void tsdmp_(DumpFn &&dumpfn, Args&& ...args)
{
    DumpResults results;
    // try writing everything:
    {
        // buffer for converting individual arguments. it is defined
        // in a child scope to free it in case the buffer is too small
        // for any of the arguments.
        char writebuf[RYML_LOGBUF_SIZE];
        results = c4::cat_dump_resume(std::forward<DumpFn>(dumpfn), writebuf, std::forward<Args>(args)...);
    }
    // if any of the arguments failed to fit the buffer, allocate a
    // larger buffer (with alloca(), up to a limit) and resume writing.
    //
    // results.bufsize is set to the size of the largest element
    // serialized. Eg int(1) will require 1 byte.
    if C4_UNLIKELY (results.bufsize > RYML_LOGBUF_SIZE)
    {
        const size_t bufsize = results.bufsize <= RYML_LOGBUF_SIZE_MAX ? results.bufsize : RYML_LOGBUF_SIZE_MAX;
        #ifdef C4_MSVC
        substr largerbuf = {static_cast<char*>(_alloca(bufsize)), bufsize};
        #else
        substr largerbuf = {static_cast<char*>(alloca(bufsize)), bufsize};
        #endif
        results = c4::cat_dump_resume(std::forward<DumpFn>(dumpfn), results, largerbuf, std::forward<Args>(args)...);
    }
}
template<class ...Args>
C4_NO_INLINE void tslog_(csubstr fmt, Args const& ...args)
{
    tsfmt_(&tsdumper_, fmt, args...);
}
template<class ...Args>
C4_NO_INLINE void tsdump_(Args const& ...args)
{
    tsdmp_(&tsdumper_, args...);
}
C4_SUPPRESS_WARNING_POP


} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_COMMON_HPP_ */
