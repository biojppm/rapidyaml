#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define GENERIC_MAP_CASES                       \
        "generic map"

CASE_GROUP(GENERIC_MAP)
{
    APPEND_CASES(

C("generic map",
R"(
a simple key: a value   # The KEY token is produced here.
? a complex key
: another value
a mapping:
  key 1: value 1
  key 2: value 2
a sequence:
  - item 1
  - item 2
)",
  L{
      N("a simple key", "a value"),
      N("a complex key", "another value"),
      N("a mapping", L{N("key 1", "value 1"), N("key 2", "value 2")}),
      N("a sequence", L{N("item 1"), N("item 2")}),
  }
),
    )
}

INSTANTIATE_GROUP(GENERIC_MAP);

} // namespace yml
} // namespace c4
