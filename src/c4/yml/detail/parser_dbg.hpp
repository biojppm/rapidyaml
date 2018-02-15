#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#define _C4_YML_DETAIL_PARSER_DBG_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "../common.hpp"
#endif

//-----------------------------------------------------------------------------
// some debugging scaffolds

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragma-system-header-outside-header"
#pragma GCC system_header

// some debugging scaffolds

#define _c4prsp(sp) ((int)(sp).len), (sp).str

#ifdef RYML_DBG
#   define _c4err(fmt, ...)   this->_err("\n" __FILE__ ":",C4_XQUOTE(__LINE__) ": ERROR parsing yml: " fmt     , ## __VA_ARGS__)
#   define _c4dbgt(fmt, ...)  this->_dbg(     __FILE__ ":",C4_XQUOTE(__LINE__)                    ": " fmt     , ## __VA_ARGS__)
#   define _c4dbgpf(fmt, ...)  printf   (     __FILE__ ":" C4_XQUOTE(__LINE__)                    ": " fmt "\n", ## __VA_ARGS__)
#   define _c4dbgp(msg)        printf   (     __FILE__ ":" C4_XQUOTE(__LINE__)                    ": " msg "\n"                )
#   define _c4dbgq(msg) printf(msg "\n")
#else
#   define _c4err(fmt, ...)   this->_err("ERROR parsing yml: " fmt, ## __VA_ARGS__)
#   define _c4dbgt(fmt, ...)
#   define _c4dbgpf(fmt, ...)
#   define _c4dbgp(msg)
#   define _c4dbgq(msg)
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Werror"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


#endif /* _C4_YML_DETAIL_PARSER_DBG_HPP_ */
