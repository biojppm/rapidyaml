#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_MAPX4)
{

ADD_CASE_TO_GROUP("nested map x4, explicit",
R"({
  foo0: {
     foo1: { foo2: {foo3: 0000, bar3: 0001, baz3: 0002}, bar2: {foo3: 0010, bar3: 0011, baz3: 0012}, baz2: {foo3: 0020, bar3: 0021, baz3: 0022} },
     bar1: { foo2: {foo3: 0100, bar3: 0101, baz3: 0102}, bar2: {foo3: 0110, bar3: 0111, baz3: 0112}, baz2: {foo3: 0120, bar3: 0121, baz3: 0122} },
     baz1: { foo2: {foo3: 0200, bar3: 0201, baz3: 0202}, bar2: {foo3: 0210, bar3: 0211, baz3: 0212}, baz2: {foo3: 0220, bar3: 0221, baz3: 0222} },
  },
  bar0: {
     foo1: { foo2: {foo3: 1000, bar3: 1001, baz3: 1002}, bar2: {foo3: 1010, bar3: 1011, baz3: 1012}, baz2: {foo3: 1020, bar3: 1021, baz3: 1022} },
     bar1: { foo2: {foo3: 1100, bar3: 1101, baz3: 1102}, bar2: {foo3: 1110, bar3: 1111, baz3: 1112}, baz2: {foo3: 1120, bar3: 1121, baz3: 1122} },
     baz1: { foo2: {foo3: 1200, bar3: 1201, baz3: 1202}, bar2: {foo3: 1210, bar3: 1211, baz3: 1212}, baz2: {foo3: 1220, bar3: 1221, baz3: 1222} },
  },
  baz0: {
     foo1: { foo2: {foo3: 2000, bar3: 2001, baz3: 2002}, bar2: {foo3: 2010, bar3: 2011, baz3: 2012}, baz2: {foo3: 2020, bar3: 2021, baz3: 2022} },
     bar1: { foo2: {foo3: 2100, bar3: 2101, baz3: 2102}, bar2: {foo3: 2110, bar3: 2111, baz3: 2112}, baz2: {foo3: 2120, bar3: 2121, baz3: 2122} },
     baz1: { foo2: {foo3: 2200, bar3: 2201, baz3: 2202}, bar2: {foo3: 2210, bar3: 2211, baz3: 2212}, baz2: {foo3: 2220, bar3: 2221, baz3: 2222} },
  },
})",
N(MFS, L{
      N(KP|MFS, "foo0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0000"), N(KP|VP, "bar3", "0001"), N(KP|VP, "baz3", "0002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0010"), N(KP|VP, "bar3", "0011"), N(KP|VP, "baz3", "0012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0020"), N(KP|VP, "bar3", "0021"), N(KP|VP, "baz3", "0022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0100"), N(KP|VP, "bar3", "0101"), N(KP|VP, "baz3", "0102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0110"), N(KP|VP, "bar3", "0111"), N(KP|VP, "baz3", "0112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0120"), N(KP|VP, "bar3", "0121"), N(KP|VP, "baz3", "0122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0200"), N(KP|VP, "bar3", "0201"), N(KP|VP, "baz3", "0202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0210"), N(KP|VP, "bar3", "0211"), N(KP|VP, "baz3", "0212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0220"), N(KP|VP, "bar3", "0221"), N(KP|VP, "baz3", "0222")})}),
      }),
      N(KP|MFS, "bar0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1000"), N(KP|VP, "bar3", "1001"), N(KP|VP, "baz3", "1002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1010"), N(KP|VP, "bar3", "1011"), N(KP|VP, "baz3", "1012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1020"), N(KP|VP, "bar3", "1021"), N(KP|VP, "baz3", "1022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1100"), N(KP|VP, "bar3", "1101"), N(KP|VP, "baz3", "1102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1110"), N(KP|VP, "bar3", "1111"), N(KP|VP, "baz3", "1112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1120"), N(KP|VP, "bar3", "1121"), N(KP|VP, "baz3", "1122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1200"), N(KP|VP, "bar3", "1201"), N(KP|VP, "baz3", "1202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1210"), N(KP|VP, "bar3", "1211"), N(KP|VP, "baz3", "1212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1220"), N(KP|VP, "bar3", "1221"), N(KP|VP, "baz3", "1222")})}),
      }),
      N(KP|MFS, "baz0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2000"), N(KP|VP, "bar3", "2001"), N(KP|VP, "baz3", "2002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2010"), N(KP|VP, "bar3", "2011"), N(KP|VP, "baz3", "2012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2020"), N(KP|VP, "bar3", "2021"), N(KP|VP, "baz3", "2022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2100"), N(KP|VP, "bar3", "2101"), N(KP|VP, "baz3", "2102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2110"), N(KP|VP, "bar3", "2111"), N(KP|VP, "baz3", "2112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2120"), N(KP|VP, "bar3", "2121"), N(KP|VP, "baz3", "2122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2200"), N(KP|VP, "bar3", "2201"), N(KP|VP, "baz3", "2202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2210"), N(KP|VP, "bar3", "2211"), N(KP|VP, "baz3", "2212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2220"), N(KP|VP, "bar3", "2221"), N(KP|VP, "baz3", "2222")})}),
      })
    })
);

ADD_CASE_TO_GROUP("nested map x4",
R"(
foo0:
  foo1:
    foo2:
      foo3: 0000
      bar3: 0001
      baz3: 0002
    bar2:
      foo3: 0010
      bar3: 0011
      baz3: 0012
    baz2:
      foo3: 0020
      bar3: 0021
      baz3: 0022
  bar1:
    foo2:
      foo3: 0100
      bar3: 0101
      baz3: 0102
    bar2:
      foo3: 0110
      bar3: 0111
      baz3: 0112
    baz2:
      foo3: 0120
      bar3: 0121
      baz3: 0122
  baz1:
    foo2:
      foo3: 0200
      bar3: 0201
      baz3: 0202
    bar2:
      foo3: 0210
      bar3: 0211
      baz3: 0212
    baz2:
      foo3: 0220
      bar3: 0221
      baz3: 0222
bar0:
  foo1:
    foo2:
      foo3: 1000
      bar3: 1001
      baz3: 1002
    bar2:
      foo3: 1010
      bar3: 1011
      baz3: 1012
    baz2:
      foo3: 1020
      bar3: 1021
      baz3: 1022
  bar1:
    foo2:
      foo3: 1100
      bar3: 1101
      baz3: 1102
    bar2:
      foo3: 1110
      bar3: 1111
      baz3: 1112
    baz2:
      foo3: 1120
      bar3: 1121
      baz3: 1122
  baz1:
    foo2:
      foo3: 1200
      bar3: 1201
      baz3: 1202
    bar2:
      foo3: 1210
      bar3: 1211
      baz3: 1212
    baz2:
      foo3: 1220
      bar3: 1221
      baz3: 1222
baz0:
  foo1:
    foo2:
      foo3: 2000
      bar3: 2001
      baz3: 2002
    bar2:
      foo3: 2010
      bar3: 2011
      baz3: 2012
    baz2:
      foo3: 2020
      bar3: 2021
      baz3: 2022
  bar1:
    foo2:
      foo3: 2100
      bar3: 2101
      baz3: 2102
    bar2:
      foo3: 2110
      bar3: 2111
      baz3: 2112
    baz2:
      foo3: 2120
      bar3: 2121
      baz3: 2122
  baz1:
    foo2:
      foo3: 2200
      bar3: 2201
      baz3: 2202
    bar2:
      foo3: 2210
      bar3: 2211
      baz3: 2212
    baz2:
      foo3: 2220
      bar3: 2221
      baz3: 2222
)",
N(MB, L{
      N(KP|MB, "foo0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0000"), N(KP|VP, "bar3", "0001"), N(KP|VP, "baz3", "0002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0010"), N(KP|VP, "bar3", "0011"), N(KP|VP, "baz3", "0012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0020"), N(KP|VP, "bar3", "0021"), N(KP|VP, "baz3", "0022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0100"), N(KP|VP, "bar3", "0101"), N(KP|VP, "baz3", "0102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0110"), N(KP|VP, "bar3", "0111"), N(KP|VP, "baz3", "0112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0120"), N(KP|VP, "bar3", "0121"), N(KP|VP, "baz3", "0122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0200"), N(KP|VP, "bar3", "0201"), N(KP|VP, "baz3", "0202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0210"), N(KP|VP, "bar3", "0211"), N(KP|VP, "baz3", "0212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0220"), N(KP|VP, "bar3", "0221"), N(KP|VP, "baz3", "0222")})}),
      }),
      N(KP|MB, "bar0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1000"), N(KP|VP, "bar3", "1001"), N(KP|VP, "baz3", "1002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1010"), N(KP|VP, "bar3", "1011"), N(KP|VP, "baz3", "1012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1020"), N(KP|VP, "bar3", "1021"), N(KP|VP, "baz3", "1022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1100"), N(KP|VP, "bar3", "1101"), N(KP|VP, "baz3", "1102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1110"), N(KP|VP, "bar3", "1111"), N(KP|VP, "baz3", "1112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1120"), N(KP|VP, "bar3", "1121"), N(KP|VP, "baz3", "1122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1200"), N(KP|VP, "bar3", "1201"), N(KP|VP, "baz3", "1202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1210"), N(KP|VP, "bar3", "1211"), N(KP|VP, "baz3", "1212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1220"), N(KP|VP, "bar3", "1221"), N(KP|VP, "baz3", "1222")})}),
      }),
      N(KP|MB, "baz0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2000"), N(KP|VP, "bar3", "2001"), N(KP|VP, "baz3", "2002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2010"), N(KP|VP, "bar3", "2011"), N(KP|VP, "baz3", "2012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2020"), N(KP|VP, "bar3", "2021"), N(KP|VP, "baz3", "2022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2100"), N(KP|VP, "bar3", "2101"), N(KP|VP, "baz3", "2102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2110"), N(KP|VP, "bar3", "2111"), N(KP|VP, "baz3", "2112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2120"), N(KP|VP, "bar3", "2121"), N(KP|VP, "baz3", "2122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2200"), N(KP|VP, "bar3", "2201"), N(KP|VP, "baz3", "2202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2210"), N(KP|VP, "bar3", "2211"), N(KP|VP, "baz3", "2212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2220"), N(KP|VP, "bar3", "2221"), N(KP|VP, "baz3", "2222")})}),
      })
    })
);
}

} // namespace yml
} // namespace c4
