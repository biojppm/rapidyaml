#ifndef RYML_SINGLE_HEADER
#include "c4/yml/version.hpp"
#include "c4/yml/detail/print.hpp"
#endif
#include "./test_lib/test_case.hpp"
#include <gtest/gtest.h>

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {
namespace {

TEST(print_tree, basic)
{
    Tree tree = parse_in_arena(R"(
%TAG !foo! bar-
---
plain: plain
'squo': 'squo'
"dquo": "dquo"
? &anchor !tag |-
  literal
: &anchor !tag |-
  literal
? &anchor !<tag> >-
  folded
: &anchor !<tag> >-
  folded
flow seq: [val, val]
flow map: {key: val}
block seq:
  - val
  - val
block map:
  key: val
empty val:
 : empty key
)");
    print_tree("message", tree);
    ConstNodeRef doc = tree.cdocref(0);
    print_tree(tree.crootref());
    print_tree(doc["plain"]);
    print_tree(doc["block seq"]);
    print_tree(doc["block map"]);
}

} // namespace
} // namespace yml
} // namespace c4
