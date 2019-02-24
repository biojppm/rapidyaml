#include "./test_group.hpp"

namespace c4 {
namespace yml {


#define TAG_PROPERTY_CASES \
    "tag property in implicit map",\
    "tag property in explicit map",\
    "tag property in implicit seq",\
    "tag property in explicit seq",\
    "tagged explicit sequence in map"


CASE_GROUP(TAG_PROPERTY)
{
    APPEND_CASES(

C("tag property in implicit map",
R"(ivar: !!int 0
svar: !!str 0
!!str key: !!int val
)",
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N(TS("!!str", "key"), TS("!!int", "val"))
    }
),

C("tag property in explicit map",
R"({
ivar: !!int 0,
svar: !!str 0,
!!str key: !!int val
}
)",
    L{
      N("ivar", TS("!!int", "0")),
      N("svar", TS("!!str", "0")),
      N(TS("!!str", "key"), TS("!!int", "val"))
    }
),

C("tag property in implicit seq",
R"(- !!int 0
- !!str 0
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tag property in explicit seq",
R"([
!!int 0,
!!str 0
]
)",
    L{
      N(TS("!!int", "0")),
      N(TS("!!str", "0")),
    }
),

C("tagged explicit sequence in map",
R"(some_seq: !its_type [
!!int 0,
!!str 0
]
)",
    L{N("some_seq", TL("!its_type", L{
              N(TS("!!int", "0")),
              N(TS("!!str", "0")),
                  }))
          }
),
    )
}

INSTANTIATE_GROUP(TAG_PROPERTY);

} // namespace yml
} // namespace c4
