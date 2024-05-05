#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(NESTED_SEQX3)
{

ADD_CASE_TO_GROUP("nested seq x3, explicit",
R"([
[[000, 001, 002], [010, 011, 012], [020, 021, 022]],
[[100, 101, 102], [110, 111, 112], [120, 121, 122]],
[[200, 201, 202], [210, 211, 212], [220, 221, 222]],
])",
N(SFS, L{
      N{SFS, L{N{SFS, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SFS, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SFS, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SFS, L{N{SFS, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SFS, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SFS, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SFS, L{N{SFS, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SFS, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SFS, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
    })
);

ADD_CASE_TO_GROUP("nested seq x3",
R"(
- - - 000
    - 001
    - 002
  - - 010
    - 011
    - 012
  - - 020
    - 021
    - 022
- - - 100
    - 101
    - 102
  - - 110
    - 111
    - 112
  - - 120
    - 121
    - 122
- - - 200
    - 201
    - 202
  - - 210
    - 211
    - 212
  - - 220
    - 221
    - 222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x3, continued on next line",
R"(
-
  -
    - 000
    - 001
    - 002
  -
    - 010
    - 011
    - 012
  -
    - 020
    - 021
    - 022
-
  -
    - 100
    - 101
    - 102
  -
    - 110
    - 111
    - 112
  -
    - 120
    - 121
    - 122
-
  -
    - 200
    - 201
    - 202
  -
    - 210
    - 211
    - 212
  -
    - 220
    - 221
    - 222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x3, all continued on next line",
R"(
-
  -
    -
      000
    -
      001
    -
      002
  -
    -
      010
    -
      011
    -
      012
  -
    -
      020
    -
      021
    -
      022
-
  -
    -
      100
    -
      101
    -
      102
  -
    -
      110
    -
      111
    -
      112
  -
    -
      120
    -
      121
    -
      122
-
  -
    -
      200
    -
      201
    -
      202
  -
    -
      210
    -
      211
    -
      212
  -
    -
      220
    -
      221
    -
      222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);
}

} // namespace yml
} // namespace c4
