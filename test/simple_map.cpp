#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SIMPLE_MAP_CASES                                \
    "empty map",                                        \
        "empty map, multiline",                         \
        "empty map, multilines",                        \
        "simple map, explicit, single line",            \
        "simple map, explicit, multiline, unindented",  \
        "simple map, explicit, multiline, indented",    \
        "simple map",                                   \
        "simple map, values on next line",              \
        "simple map, with comments",                    \
        "simple map, with comments interspersed",       \
        "simple map, with indented comments interspersed, before",\
        "simple map, with indented comments interspersed, after"

CASE_GROUP(SIMPLE_MAP)
{
    APPEND_CASES(

C("empty map",
"{}",
    MAP
),

C("empty map, multiline",
R"({
}
)",
    MAP
),

C("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MAP
),

C("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, values on next line",
R"(
foo:
  0
bar:
  1
baz:
  2
bat:
  3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a baz
bat: 3   # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments interspersed",
R"(
# this is a foo
foo: 0
# this is a bar
bar: 1
# this is a baz
baz: 2
# this is a bat
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with indented comments interspersed, before",
R"(
  # this is a foo
foo: 0
  # this is a bar
bar: 1
  # this is a baz
baz: 2
  # this is a bat
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with indented comments interspersed, after",
R"(
foo: 0
  # this is a foo
bar: 1
  # this is a bar
baz: 2
  # this is a baz
bat: 3
  # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),
    )
}


INSTANTIATE_GROUP(SIMPLE_MAP);

} // namespace yml
} // namespace c4
