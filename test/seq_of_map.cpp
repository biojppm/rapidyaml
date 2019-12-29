#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SEQ_OF_MAP_CASES                        \
    "seq of empty maps, one line",              \
    "seq of maps, one line",                    \
    "seq of maps, implicit seq, explicit maps", \
    "seq of maps",                              \
    "seq of maps, next line"
CASE_GROUP(SEQ_OF_MAP)
{
    APPEND_CASES(

C("seq of empty maps, one line",
R"([{}, {}, {}])",
  L{MAP, MAP, MAP}
),

C("seq of maps, one line",
R"([{name: John Smith, age: 33}, {name: Mary Smith, age: 27}])",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps, implicit seq, explicit maps",
R"(
- {name: John Smith, age: 33}
- {name: Mary Smith, age: 27}
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps",
R"(
- name: John Smith
  age: 33
- name: Mary Smith
  age: 27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

C("seq of maps, next line",
R"(
- 
  name:
    John Smith
  age:
    33
- 
  name: 
    Mary Smith
  age:
    27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
),

    )
}

INSTANTIATE_GROUP(SEQ_OF_MAP)

} // namespace yml
} // namespace c4
