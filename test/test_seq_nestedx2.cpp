#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_SEQX2)
{

ADD_CASE_TO_GROUP("nested seq x2, empty, oneline",
R"([[], [], []])",
N(SFS, L{SFS, SFS, SFS})
);

ADD_CASE_TO_GROUP("nested seq x2, explicit, same line",
R"([[00, 01, 02], [10, 11, 12], [20, 21, 22]])",
N(SFS, L{
      N(SFS, L{N(VP, "00"), N(VP, "01"), N(VP, "02")}),
      N(SFS, L{N(VP, "10"), N(VP, "11"), N(VP, "12")}),
      N(SFS, L{N(VP, "20"), N(VP, "21"), N(VP, "22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, explicit first+last level, same line, no spaces",
R"([[00,01,02],[10,11,12],[20,21,22]])",
N(SFS, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
          })
);

ADD_CASE_TO_GROUP("nested seq x2, explicit",
R"([
[00, 01, 02],
[10, 11, 12],
[20, 21, 22],
])",
N(SFS, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
          })
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
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
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
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
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
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, all next line, with whitespace",
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
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, implicit first, explicit last level",
R"(
- [00, 01, 02]
- [10, 11, 12]
- [20, 21, 22]
)",
N(SB, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);
}


} // namespace yml
} // namespace c4
