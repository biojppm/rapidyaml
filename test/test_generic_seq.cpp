#include "./test_group.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(GENERIC_SEQ)
{

ADD_CASE_TO_GROUP("generic seq v0",
R"(
- item 1
- item 2
- - item 3.1
  - item 3.2
- key 1: value 1
  key 2: value 2
)",
  L{
      N("item 1"),
      N("item 2"),
      N(L{N("item 3.1"), N("item 3.2")}),
      N(L{N("key 1", "value 1"), N("key 2", "value 2")})
  }
);

ADD_CASE_TO_GROUP("generic seq v1",
R"(
- item 1
- item 2
-
  - item 3.1
  - item 3.2
-
  key 1: value 1
  key 2: value 2
)",
  L{
      N("item 1"),
      N("item 2"),
      N(L{N("item 3.1"), N("item 3.2")}),
      N(L{N("key 1", "value 1"), N("key 2", "value 2")})
  }
);
}

} // namespace yml
} // namespace c4
