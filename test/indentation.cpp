#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define INDENTATION_CASES \
    "4 chars",\
    "2 chars + 4 chars, ex0",\
    "2 chars + 4 chars, ex1",\
    "2 chars + 4 chars, ex2"

CASE_GROUP(INDENTATION)
{
    APPEND_CASES(

C("4 chars",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex0",
R"(
key:
     value
another_key:
    sub_key0:
        - val0
        - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex1",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
        - val2
        - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex2",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
        - val4
        - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),
    )
}

INSTANTIATE_GROUP(INDENTATION);

} // namespace yml
} // namespace c4
