#include "./test_group.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_SEQX2)
{

ADD_CASE_TO_GROUP("nested seq x2, empty, oneline",
R"([[], [], []])",
    L{SEQ, SEQ, SEQ}
);

ADD_CASE_TO_GROUP("nested seq x2, explicit, same line",
R"([[00, 01, 02], [10, 11, 12], [20, 21, 22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2, explicit first+last level, same line, no spaces",
R"([[00,01,02],[10,11,12],[20,21,22]])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2, explicit",
R"([
[00, 01, 02],
[10, 11, 12],
[20, 21, 22],
])",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2",
R"(
- - 00
  - 01
  - 02
- - 10
  - 11
  - 12
- - 20
  - 21
  - 22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2, next line",
R"(
-
  - 00
  - 01
  - 02
-
  - 10
  - 11
  - 12
-
  - 20
  - 21
  - 22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2, all next line",
R"(
-
  -
    00
  -
    01
  -
    02
-
  -
    10
  -
    11
  -
    12
-
  -
    20
  -
    21
  -
    22
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);

ADD_CASE_TO_GROUP("nested seq x2, implicit first, explicit last level",
R"(
- [00, 01, 02]
- [10, 11, 12]
- [20, 21, 22]
)",
    L{
      N{L{N{"00"}, N{"01"}, N{"02"}}},
      N{L{N{"10"}, N{"11"}, N{"12"}}},
      N{L{N{"20"}, N{"21"}, N{"22"}}},
          }
);
}


} // namespace yml
} // namespace c4
