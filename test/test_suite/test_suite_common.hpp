#ifndef C4_YML_TEST_SUITE_COMMON_HPP_
#define C4_YML_TEST_SUITE_COMMON_HPP_

#include <gtest/gtest.h>

#include <c4/yml/std/std.hpp>

#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>

#include "test_case.hpp"

#define RYML_NFO (1 || RYML_DBG)

#if RYML_NFO
#define _nfo_print_tree(title, tree) do { c4::log("{}:{}: " title ":", __FILE__, __LINE__); print_tree(tree); c4::yml::emit(tree, stdout); fflush(stdout); } while(0)
#define _nfo_logf(fmt, ...)          do { c4::log("{}:{}: " fmt      , __FILE__, __LINE__, __VA_ARGS__);                                   fflush(stdout); } while(0)
#define _nfo_log(fmt)                do { c4::log("{}:{}: " fmt      , __FILE__, __LINE__);                                                fflush(stdout); } while(0)
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


} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_COMMON_HPP_ */
