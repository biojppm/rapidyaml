#ifndef _C4_TPL_COMMON_HPP_
#define _C4_TPL_COMMON_HPP_

#include <c4/yml/common.hpp>
#include <c4/yml/subs.hpp>

namespace c4 {
namespace tpl {

using csubs = c4::yml::csubs;
using subs  = c4::yml::subs;
using Allocator = c4::yml::Allocator;

enum : size_t { NONE = size_t(-1) };
enum : size_t { npos = size_t(-1) };


} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_COMMON_HPP_ */
