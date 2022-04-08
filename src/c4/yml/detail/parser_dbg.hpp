#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#define _C4_YML_DETAIL_PARSER_DBG_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "../common.hpp"
#endif
#include <cstdio>

//-----------------------------------------------------------------------------
// some debugging scaffolds

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
//#pragma GCC diagnostic ignored "-Wpragma-system-header-outside-header"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Werror"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

// some debugging scaffolds
#ifdef RYML_DBG
#include <c4/dump.hpp>
namespace c4 {
inline void _dbg_dumper(csubstr s) { fwrite(s.str, 1, s.len, stdout); };
extern const substr _dbg_writebuf;
template<class ...Args>
void _dbg_printf(c4::csubstr fmt, Args const& C4_RESTRICT ...args)
{
    auto iter_results = c4::format_dump_resume<&_dbg_dumper>(_dbg_writebuf, fmt, args...);
    // resume writing if the results failed to fit the buffer
    if(C4_UNLIKELY(iter_results.bufsize > _dbg_writebuf.len)) // bufsize will be that of the largest element serialized. Eg int(1), will require 1 byte.
    {
        iter_results = format_dump_resume<&_dbg_dumper>(iter_results, _dbg_writebuf, fmt, args...);
        if(C4_UNLIKELY(iter_results.bufsize > _dbg_writebuf.len))
        {
            iter_results = format_dump_resume<&_dbg_dumper>(iter_results, _dbg_writebuf, fmt, args...);
        }
    }
    if(fmt.last_of('\n') != csubstr::npos)
        fflush(stdout);
}
} // namespace c4

#   define _c4dbgt(fmt, ...)   this->_dbg ("{}:{}: "   fmt     , __FILE__, __LINE__, ## __VA_ARGS__)
#   define _c4dbgpf(fmt, ...)  ::c4::_dbg_printf("{}:{}: "   fmt "\n", __FILE__, __LINE__, ## __VA_ARGS__)
#   define _c4dbgp(msg)        ::c4::_dbg_printf("{}:{}: "   msg "\n", __FILE__, __LINE__                )
#   define _c4dbgq(msg)        ::c4::_dbg_printf(msg "\n")
#   define _c4err(fmt, ...)   \
    do { if(c4::is_debugger_attached()) { C4_DEBUG_BREAK(); } \
         this->_err("ERROR:\n" "{}:{}: " fmt, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)
#else
#   define _c4dbgt(fmt, ...)
#   define _c4dbgpf(fmt, ...)
#   define _c4dbgp(msg)
#   define _c4dbgq(msg)
#   define _c4err(fmt, ...)   \
    do { if(c4::is_debugger_attached()) { C4_DEBUG_BREAK(); } \
         this->_err("ERROR: " fmt, ## __VA_ARGS__); } while(0)
#endif

#define _c4prsp(sp) sp
#define _c4presc(s) __c4presc(s.str, s.len)
inline c4::csubstr _c4prc(const char &C4_RESTRICT c)
{
    switch(c)
    {
    case '\n': return c4::csubstr("\\n");
    case '\t': return c4::csubstr("\\t");
    case '\0': return c4::csubstr("\\0");
    case '\r': return c4::csubstr("\\r");
    case '\f': return c4::csubstr("\\f");
    case '\b': return c4::csubstr("\\b");
    case '\v': return c4::csubstr("\\v");
    case '\a': return c4::csubstr("\\a");
    default: return c4::csubstr(&c, 1);
    }
}
inline void __c4presc(const char *s, size_t len)
{
    size_t prev = 0;
    for(size_t i = 0; i < len; ++i)
    {
        switch(s[i])
        {
        case '\n'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('n'); putchar('\n'); prev = i+1; break;
        case '\t'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('t'); prev = i+1; break;
        case '\0'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('0'); prev = i+1; break;
        case '\r'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('r'); prev = i+1; break;
        case '\f'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('f'); prev = i+1; break;
        case '\b'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('b'); prev = i+1; break;
        case '\v'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('v'); prev = i+1; break;
        case '\a'  : fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('a'); prev = i+1; break;
        case '\x1b': fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('e'); prev = i+1; break;
        case -0x3e/*0xc2u*/:
            if(i+1 < len)
            {
                if(s[i+1] == -0x60/*0xa0u*/)
                {
                    fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('_'); prev = i+2; ++i;
                }
                else if(s[i+1] == -0x7b/*0x85u*/)
                {
                    fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('N'); prev = i+2; ++i;
                }
                break;
            }
        case -0x1e/*0xe2u*/:
            if(i+2 < len && s[i+1] == -0x80/*0x80u*/)
            {
                if(s[i+2] == -0x58/*0xa8u*/)
                {
                    fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('L'); prev = i+3; i += 2;
                }
                else if(s[i+2] == -0x57/*0xa9u*/)
                {
                    fwrite(s+prev, 1, i-prev, stdout); putchar('\\'); putchar('P'); prev = i+3; i += 2;
                }
                break;
            }
        }
    }
    fwrite(s + prev, 1, len - prev, stdout);
}

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


#endif /* _C4_YML_DETAIL_PARSER_DBG_HPP_ */
