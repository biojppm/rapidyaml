#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#define _C4_YML_DETAIL_PARSER_DBG_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "../common.hpp"
#endif

//-----------------------------------------------------------------------------
// some debugging scaffolds

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wpragma-system-header-outside-header"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define _c4prsp(sp) ((int)(sp).len), (sp).str

#ifdef RYML_DBG
#   define _c4prt_(fn, file, line, what, msg, ...)        \
    this->_##fn(file ":" C4_QUOTE(line) ": " what msg, ## __VA_ARGS__)
#   define _c4err(fmt, ...)  _c4prt_(err, "\n" __FILE__, __LINE__, "ERROR parsing yml: ", fmt, ## __VA_ARGS__)
#   define _c4dbgf(fmt, ...) _c4prt_(dbg,      __FILE__, __LINE__,                    "", fmt, ## __VA_ARGS__)
#   define _c4dbgp(fmt, ...)  printf(__FILE__ ":" C4_XQUOTE(__LINE__) ": " fmt "\n", ## __VA_ARGS__)
#   define _c4dbgs(fmt, ...)  printf(fmt "\n", ## __VA_ARGS__)
#else
#   define _c4err(fmt, ...) this->_err(fmt, ## __VA_ARGS__)
#   define _c4dbg(fmt, ...)
#   define _c4dbgs(fmt, ...)
#endif

#pragma GCC diagnostic pop
#pragma clang diagnostic pop


#endif /* _C4_YML_DETAIL_PARSER_DBG_HPP_ */
