#ifndef _C4_TPL_COMMON_HPP_
#define _C4_TPL_COMMON_HPP_

#include <c4/yml/common.hpp>
#include <c4/yml/span.hpp>

namespace c4 {
namespace tpl {

using cspan = c4::yml::cspan;
using span  = c4::yml::span;
using Allocator = c4::yml::Allocator;

enum : size_t { NONE = size_t(-1) };
enum : size_t { npos = size_t(-1) };


} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_COMMON_HPP_ */
