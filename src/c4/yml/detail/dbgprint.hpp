#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#define _C4_YML_DETAIL_DBGPRINT_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "../common.hpp"
#endif

#ifdef RYML_DBG
#include <cstdio>
#endif


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
#   define _c4dbgt(fmt, ...)   do { if(_dbg_enabled()) {                \
                               this->_dbg ("{}:{}: "   fmt     , __FILE__, __LINE__, __VA_ARGS__); } } while(0)
#   define _c4dbgpf(fmt, ...)  _dbg_printf("{}:{}: "   fmt "\n", __FILE__, __LINE__, __VA_ARGS__)
#   define _c4dbgpf_(fmt, ...) _dbg_printf("{}:{}: "   fmt     , __FILE__, __LINE__, __VA_ARGS__)
#   define _c4dbgp(msg)        _dbg_printf("{}:{}: "   msg "\n", __FILE__, __LINE__             )
#   define _c4dbgp_(msg)       _dbg_printf("{}:{}: "   msg     , __FILE__, __LINE__             )
#   define _c4dbgq(msg)        _dbg_printf(msg "\n")
#   define _c4presc(...)       do { if(_dbg_enabled()) __c4presc(__VA_ARGS__); } while(0)
#   define _c4prscalar(msg, scalar, keep_newlines)                  \
    do {                                                            \
        _c4dbgpf_("{}: [{}]~~~", msg, scalar.len);                  \
        if(_dbg_enabled()) {                                        \
            __c4presc((scalar), (keep_newlines)); \
        }                                                           \
        _c4dbgq("~~~");                                             \
    } while(0)
#endif // RYML_DBG


//-----------------------------------------------------------------------------

#ifdef RYML_DBG


#if defined(C4_MSVC) || defined(C4_MINGW)
#include <malloc.h>
#elif (defined(__clang__) && defined(_MSC_VER)) || \
      defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <stdlib.h>
#else
#include <alloca.h>
#endif


#define RYML_LOGBUF_SIZE_MAX RYML_ERRMSG_SIZE

#include <c4/dump.hpp>

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")

namespace c4 {
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
    // larger buffer (up to a limit) and resume writing.
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
    if(!_dbg_enabled())
        return; // LCOV_EXCL_LINE
    _RYML_ASSERT_BASIC(s.str || !s.len);
    size_t prev = 0;
    for(size_t i = 0; i < s.len; ++i)
    {
        switch(s.str[i])
        {
        case '\n'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\n"); if(keep_newlines) { _dbg_dumper("\n"); } prev = i+1; break;
        case '\t'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\t"); prev = i+1; break;
        case '\0'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\0"); prev = i+1; break;
        case '\r'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\r"); prev = i+1; break;
        case '\f'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\f"); prev = i+1; break;
        case '\b'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\b"); prev = i+1; break;
        case '\v'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\v"); prev = i+1; break;
        case '\a'  : _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\a"); prev = i+1; break;
        case '\x1b': _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\x1b"); prev = i+1; break;
        case _RYML_CHCONST(-0x3e, 0xc2):
            if(i+1 < s.len)
            {
                if(s.str[i+1] == _RYML_CHCONST(-0x60, 0xa0))
                {
                    _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\_"); prev = i+1;
                }
                else if(s.str[i+1] == _RYML_CHCONST(-0x7b,0x85))
                {
                    _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\N"); prev = i+1;
                }
            }
            break;
        case _RYML_CHCONST(-0x1e, 0xe2):
            if(i+2 < s.len && s.str[i+1] == _RYML_CHCONST(-0x80,0x80))
            {
                if(s.str[i+2] == _RYML_CHCONST(-0x58,0xa8))
                {
                    _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\L"); prev = i+1;
                }
                else if(s.str[i+2] == _RYML_CHCONST(-0x57,0xa9))
                {
                    _dbg_dumper(s.range(prev, i)); _dbg_dumper("\\P"); prev = i+1;
                }
            }
            break;
        }
    }
    if(s.len > prev)
        _dbg_dumper(s.sub(prev));
}
inline C4_NO_INLINE void __c4presc(const char *s, size_t len, bool keep_newlines=false)
{
    __c4presc(csubstr(s, len), keep_newlines);
}
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP

#endif // RYML_DBG

#endif /* _C4_YML_DETAIL_DBGPRINT_HPP_ */
