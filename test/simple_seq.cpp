#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SIMPLE_SEQ_CASES                                \
"simple seq",                                           \
"simple seq, explicit, single line",                    \
"simple seq, explicit, multiline, unindented",          \
"simple seq, explicit, multiline, comments inline",     \
"simple seq, explicit, multiline, comments prev line",  \
"simple seq, explicit, multiline, indented",            \
"simple seq, comments inline",                          \
"simple seq, comments prev line"


CASE_GROUP(SIMPLE_SEQ)
{
    APPEND_CASES(

C("simple seq",
R"(- 0
- 1
- 2
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),


C("simple seq, explicit, single line",
"[0, 1, 2, 3]",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, unindented",
R"([
0,
1,
2,
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, comments prev line",
R"([
# bla0
0,
# bla1
1,
# bla2
2,
# bla3
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, explicit, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments inline",
R"(
- 0   # this is a foo
- 1   # this is a bar
- 2   # this is a bar
- 3   # this is a bar
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),

C("simple seq, comments prev line",
R"(
# this is a foo
- 0
# this is a bar
- 1
# this is a baz
- 2
# this is a bat
- 3
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
),
    )
}

INSTANTIATE_GROUP(SIMPLE_SEQ);

} // namespace yml
} // namespace c4
