#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_MAPX3)
{

ADD_CASE_TO_GROUP("nested map x3, explicit",
R"({
  foo0: {
     foo1: {foo2: 000, bar2: 001, baz2: 002},
     bar1: {foo2: 010, bar2: 011, baz2: 012},
     baz1: {foo2: 020, bar2: 021, baz2: 022}
  },
  bar0: {
     foo1: {foo2: 100, bar2: 101, baz2: 102},
     bar1: {foo2: 110, bar2: 111, baz2: 112},
     baz1: {foo2: 120, bar2: 121, baz2: 122}
  },
  baz0: {
     foo1: {foo2: 200, bar2: 201, baz2: 202},
     bar1: {foo2: 210, bar2: 211, baz2: 212},
     baz1: {foo2: 220, bar2: 221, baz2: 222}
  }
})",
N(MFS, L{
      N{KP|MFS, "foo0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "000"}, N{KP|VP, "bar2", "001"}, N{KP|VP, "baz2", "002"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "010"}, N{KP|VP, "bar2", "011"}, N{KP|VP, "baz2", "012"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "020"}, N{KP|VP, "bar2", "021"}, N{KP|VP, "baz2", "022"}}} }},
      N{KP|MFS, "bar0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "100"}, N{KP|VP, "bar2", "101"}, N{KP|VP, "baz2", "102"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "110"}, N{KP|VP, "bar2", "111"}, N{KP|VP, "baz2", "112"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "120"}, N{KP|VP, "bar2", "121"}, N{KP|VP, "baz2", "122"}}} }},
      N{KP|MFS, "baz0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "200"}, N{KP|VP, "bar2", "201"}, N{KP|VP, "baz2", "202"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "210"}, N{KP|VP, "bar2", "211"}, N{KP|VP, "baz2", "212"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "220"}, N{KP|VP, "bar2", "221"}, N{KP|VP, "baz2", "222"}}} }},
          })
);

ADD_CASE_TO_GROUP("nested map x3",
R"(
foo0:
  foo1:
    foo2: 000
    bar2: 001
    baz2: 002
  bar1:
    foo2: 010
    bar2: 011
    baz2: 012
  baz1:
    foo2: 020
    bar2: 021
    baz2: 022
bar0:
  foo1:
    foo2: 100
    bar2: 101
    baz2: 102
  bar1:
    foo2: 110
    bar2: 111
    baz2: 112
  baz1:
    foo2: 120
    bar2: 121
    baz2: 122
baz0:
  foo1:
    foo2: 200
    bar2: 201
    baz2: 202
  bar1:
    foo2: 210
    bar2: 211
    baz2: 212
  baz1:
    foo2: 220
    bar2: 221
    baz2: 222
)",
N(MB, L{
      N{KP|MB, "foo0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "000"}, N{KP|VP, "bar2", "001"}, N{KP|VP, "baz2", "002"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "010"}, N{KP|VP, "bar2", "011"}, N{KP|VP, "baz2", "012"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "020"}, N{KP|VP, "bar2", "021"}, N{KP|VP, "baz2", "022"}}} }},
      N{KP|MB, "bar0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "100"}, N{KP|VP, "bar2", "101"}, N{KP|VP, "baz2", "102"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "110"}, N{KP|VP, "bar2", "111"}, N{KP|VP, "baz2", "112"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "120"}, N{KP|VP, "bar2", "121"}, N{KP|VP, "baz2", "122"}}} }},
      N{KP|MB, "baz0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "200"}, N{KP|VP, "bar2", "201"}, N{KP|VP, "baz2", "202"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "210"}, N{KP|VP, "bar2", "211"}, N{KP|VP, "baz2", "212"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "220"}, N{KP|VP, "bar2", "221"}, N{KP|VP, "baz2", "222"}}} }},
      })
);
}

} // namespace yml
} // namespace c4
