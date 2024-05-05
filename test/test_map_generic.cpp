#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(GENERIC_MAP)
{

ADD_CASE_TO_GROUP("generic map",
R"(
a simple key: a value   # The KEY token is produced here.
? a complex key
: another value
a mapping:
  key 1: value 1
  key 2: value 2
a sequence:
  - item 1
  - item 2
)",
N(MB, L{
      N(KP|VP, "a simple key", "a value"),
      N(KP|VP, "a complex key", "another value"),
      N(KP|MB, "a mapping", L{N(KP|VP, "key 1", "value 1"), N(KP|VP, "key 2", "value 2")}),
      N(KP|SB, "a sequence", L{N(VP, "item 1"), N(VP, "item 2")}),
  })
);


ADD_CASE_TO_GROUP("seq nested in map",
R"(
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
)",
N(MB, L{
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},}},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},}}}},
  })
);

ADD_CASE_TO_GROUP("seq nested in map, v2",
R"(
items:
    -
      part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    -
      part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
)",
N(MB, L{
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},}},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},}}}},
  })
);

}

} // namespace yml
} // namespace c4
