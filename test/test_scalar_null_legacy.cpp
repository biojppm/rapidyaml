#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(NULL_VAL)
{

ADD_CASE_TO_GROUP("empty scalar, single quoted", NO_COMPARE_EMITTED_INTS,//FIXME
                  "''",
                  N(VS, "")
);

ADD_CASE_TO_GROUP("all null", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
-
- # with space
- null
- Null
- NULL
- ~
- null: null
- Null: Null
- NULL: NULL
- ~: ~
- ~: null
- null: ~
)",
N(SB, L{
N(VN, nullptr),
N(VN, nullptr),
N(VP, "null"),
N(VP, "Null"),
N(VP, "NULL"),
N(VP, "~"),
N(MB, L{N(KP|VP, "null", "null")}),
N(MB, L{N(KP|VP, "Null", "Null")}),
N(MB, L{N(KP|VP, "NULL", "NULL")}),
N(MB, L{N(KP|VP, "~", "~")}),
N(MB, L{N(KP|VP, "~", "null")}),
N(MB, L{N(KP|VP, "null", "~")}),
})
);

ADD_CASE_TO_GROUP("null map vals, expl", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"({foo: , bar: , baz: }
)",
N(MFS, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null map vals, impl", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
foo: 
bar: 
baz: 
)",
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals, impl", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(- 
- 
- 
)",
N(SB, L{N(VN, nullptr), N(VN, nullptr), N(VN, nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 1", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
foo:
  - 
  - 
  - 
bar: 
baz: 
)",
N(MB, L{N(KP|SB, "foo", L{VN, VN, VN}), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 2", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
foo:
bar: 
  - 
  - 
  - 
baz: 
)",
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|SB, "bar", L{VN, VN, VN}), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 3", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
foo:
bar: 
baz: 
  - 
  - 
  - 
)",
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|SB, "baz", L{VN, VN, VN})})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 1", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
N(SB, L{
  N(MB, L{
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
  }),
  VN,
  VN
})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 2", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
N(SB, L{
  VN,
  N(MB, L{
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
  }),
  VN
})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 3", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
N(SB, L{
  VN,
  VN,
  N(MB, L{
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
  }),
})
);

ADD_CASE_TO_GROUP("issue84.1", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
fixed case:
  foo: a
  bar: 
your case:
  foo: a
  bar: ''
whatever: baz
)",
N(MB, L{
  N(KP|MB, "fixed case", L{N(KP|VP, "foo", "a"), N(KP|VN, "bar", nullptr)}),
  N(KP|MB, "your case", L{N(KP|VP, "foo", "a"), N(KP|VS, "bar", "")}),
  N(KP|VP, "whatever", "baz"),
})
);

ADD_CASE_TO_GROUP("issue84.2", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
version: 0
type: xml
param_root:
  objects:
    System: {SameGroupActorName: '', IsGetItemSelf: false}
    General:
      Speed: 1.0
      Life: 100
      IsLifeInfinite: false
      ElectricalDischarge: 1.0
      IsBurnOutBorn: false
      BurnOutBornName: 
      IsBurnOutBornIdent: false
      ChangeDropTableName: ''
)",
N(MB, L{
  N(KP|VP, "version", "0"),
  N(KP|VP, "type", "xml"),
  N(KP|MB, "param_root", L{
      N(KP|MB, "objects", L{
          N(KP|MFS, "System", L{
              N(KP|VS, "SameGroupActorName", ""),
              N(KP|VP, "IsGetItemSelf", "false")
          }),
          N(KP|MB, "General", L{
              N(KP|VP, "Speed", "1.0"),
              N(KP|VP, "Life", "100"),
              N(KP|VP, "IsLifeInfinite", "false"),
              N(KP|VP, "ElectricalDischarge", "1.0"),
              N(KP|VP, "IsBurnOutBorn", "false"),
              N(KP|VN, "BurnOutBornName", nullptr),
              N(KP|VP, "IsBurnOutBornIdent", "false"),
              N(KP|VS, "ChangeDropTableName", ""),
          }),
      })
  }),
})
);

ADD_CASE_TO_GROUP("issue84.3", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
version: 10
type: test
param_root:
  objects:
    TestContent:
      Str64_empty: ''
      Str64_empty2:
      Str64_empty3: ''
  lists: {}
)",
N(MB, L{
  N(KP|VP, "version", "10"),
  N(KP|VP, "type", "test"),
  N(KP|MB, "param_root", L{
      N(KP|MB, "objects", L{
          N(KP|MB, "TestContent", L{
              N(KP|VS, "Str64_empty", ""),
              N(KP|VN, "Str64_empty2", nullptr),
              N(KP|VS, "Str64_empty3", ""),
          }),
      }),
      N(KP|MFS, "lists", L{})
  }),
})
);

} // ADD_GROUP()

} // namespace yml
} // namespace c4
