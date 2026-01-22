#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#define _C4_YML_DETAIL_DBGPRINT_HPP_


//-----------------------------------------------------------------------------
// debug prints

#ifndef RYML_DBG
#   define _c4dbgt(fmt, ...)
#   define _c4dbgpf(fmt, ...)
#   define _c4dbgpf_(fmt, ...)
#   define _c4dbgp(msg)
#   define _c4dbgp_(msg)
#   define _c4dbgq(msg)
#   define _c4presc(...)
#   define _c4prscalar(msg, scalar, keep_newlines)
#else
#   define _c4dbgt(fmt, ...)   do {                                     \
                                   if(_dbg_enabled()) {                 \
                                       this->_dbg ("{}:{}: "   fmt     , __FILE__, __LINE__, __VA_ARGS__); \
                                   }                                    \
                               } while(0)
#   define _c4dbgpf(fmt, ...)  _dbg_printf("{}:{}: "   fmt "\n", __FILE__, __LINE__, __VA_ARGS__)
#   define _c4dbgpf_(fmt, ...) _dbg_printf("{}:{}: "   fmt     , __FILE__, __LINE__, __VA_ARGS__)
#   define _c4dbgp(msg)        _dbg_printf("{}:{}: "   msg "\n", __FILE__, __LINE__             )
#   define _c4dbgp_(msg)       _dbg_printf("{}:{}: "   msg     , __FILE__, __LINE__             )
#   define _c4dbgq(msg)        _dbg_printf(msg "\n")
#   define _c4presc(...)       __c4presc(__VA_ARGS__)
#   define _c4prscalar(msg, scalar, keep_newlines)                  \
    do {                                                            \
        if(_dbg_enabled()) {                                        \
            _c4dbgpf_("{}: [{}]~~~", msg, scalar.len);              \
            __c4presc((scalar), (keep_newlines));                   \
            _c4dbgq("~~~");                                         \
        }                                                           \
    } while(0)


//-----------------------------------------------------------------------------
// implementation

#include <cstdio>

#if defined(C4_MSVC) || defined(C4_MINGW)
#include <malloc.h>
#elif (defined(__clang__) && defined(_MSC_VER)) || \
      defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <stdlib.h>
#else
#include <alloca.h>
#endif

#ifndef _C4_YML_ESCAPE_SCALAR_HPP_
#include "c4/yml/escape_scalar.hpp"
#endif

#ifndef _C4_DUMP_HPP_
#include "c4/dump.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")

namespace c4 {
namespace yml {
inline bool& _dbg_enabled() { static bool enabled = true; return enabled; }
inline C4_NO_INLINE void _dbg_set_enabled(bool yes) { _dbg_enabled() = yes; }
inline C4_NO_INLINE void _dbg_dumper(csubstr s)
{
    _RYML_ASSERT_BASIC(s.str || !s.len);
    if(s.len)
        fwrite(s.str, 1, s.len, stdout);
}
template<class DumpFn, class ...Args>
C4_NO_INLINE void _dbg_dump(DumpFn &&dumpfn, csubstr fmt, Args&& ...args)
{
    DumpResults results;
    // try writing everything:
    {
        // buffer for converting individual arguments. it is defined
        // in a child scope to free it in case the buffer is too small
        // for any of the arguments.
        char writebuf[RYML_LOGBUF_SIZE];
        results = format_dump_resume(std::forward<DumpFn>(dumpfn), writebuf, fmt, std::forward<Args>(args)...);
    }
    // if any of the arguments failed to fit the buffer, allocate a
    // larger buffer (with alloca(), up to a limit) and resume writing.
    //
    // results.bufsize is set to the size of the largest element
    // serialized. Eg int(1) will require 1 byte.
    if(C4_UNLIKELY(results.bufsize > RYML_LOGBUF_SIZE))
    {
        const size_t bufsize = results.bufsize <= RYML_LOGBUF_SIZE_MAX ? results.bufsize : RYML_LOGBUF_SIZE_MAX;
        #ifdef C4_MSVC
        substr largerbuf = {static_cast<char*>(_alloca(bufsize)), bufsize};
        #else
        substr largerbuf = {static_cast<char*>(alloca(bufsize)), bufsize};
        #endif
        results = format_dump_resume(std::forward<DumpFn>(dumpfn), results, largerbuf, fmt, std::forward<Args>(args)...);
    }
}
template<class ...Args>
C4_NO_INLINE void _dbg_printf(csubstr fmt, Args const& ...args)
{
    if(_dbg_enabled())
        _dbg_dump(&_dbg_dumper, fmt, args...);
}
inline C4_NO_INLINE void __c4presc(csubstr s, bool keep_newlines=false)
{
    if(_dbg_enabled())
        escape_scalar_fn(_dbg_dumper, s, keep_newlines);
}
inline C4_NO_INLINE void __c4presc(const char *s, size_t len, bool keep_newlines=false)
{
    if(_dbg_enabled())
        escape_scalar_fn(_dbg_dumper, csubstr(s, len), keep_newlines);
}
} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP

#endif // RYML_DBG

#endif /* _C4_YML_DETAIL_DBGPRINT_HPP_ */
