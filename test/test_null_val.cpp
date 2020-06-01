#include "./test_group.hpp"

namespace c4 {
namespace yml {
#define NULL_VAL_CASES \
    "null map vals, expl",\
    "null map vals, impl",\
    "null seq vals, impl",\
    "null seq vals in map, impl, mixed 1",\
    "null seq vals in map, impl, mixed 2",\
    "null seq vals in map, impl, mixed 3",\
    "null map vals in seq, impl, mixed 1",\
    "null map vals in seq, impl, mixed 2",\
    "null map vals in seq, impl, mixed 3"

CASE_GROUP(NULL_VAL)
{
    APPEND_CASES(

C("null map vals, expl",
R"({foo: , bar: , baz: }
)",
L{N("foo", "~"), N("bar", "~"), N("baz", "~")}
),

C("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
L{N("foo", "~"), N("bar", "~"), N("baz", "~")}
),

C("null seq vals, impl",
R"(- 
- 
- 
)",
L{N("~"), N("~"), N("~")}
),

C("null seq vals in map, impl, mixed 1",
R"(
foo:
  - 
  - 
  - 
bar: 
baz: 
)",
L{N("foo", L{N("~"), N("~"), N("~")}), N("bar", "~"), N("baz", "~")}
),

C("null seq vals in map, impl, mixed 2",
R"(
foo:
bar: 
  - 
  - 
  - 
baz: 
)",
L{N("foo", "~"), N("bar", L{N("~"), N("~"), N("~")}), N("baz", "~")}
),

C("null seq vals in map, impl, mixed 3",
R"(
foo:
bar: 
baz: 
  - 
  - 
  - 
)",
L{N("foo", "~"), N("bar", "~"), N("baz", L{N("~"), N("~"), N("~")})}
),

C("null map vals in seq, impl, mixed 1",
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
L{N(L{N("foo", "~"), N("bar", "~"), N("baz", "~")}), N("~"), N("~")}
),

C("null map vals in seq, impl, mixed 2",
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
L{N("~"), N(L{N("foo", "~"), N("bar", "~"), N("baz", "~")}), N("~")}
),

C("null map vals in seq, impl, mixed 3",
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
L{N("~"), N("~"), N(L{N("foo", "~"), N("bar", "~"), N("baz", "~")})}
),
    )
}

INSTANTIATE_GROUP(NULL_VAL)

} // namespace yml
} // namespace c4
