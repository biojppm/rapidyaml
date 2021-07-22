#include "./test_group.hpp"
#include "test_case.hpp"

namespace c4 {
namespace yml {

TEST(seq_of_map, with_anchors)
{
    {
        // this case is vanilla:
        csubstr yaml = R"(- a0: v0
  &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- a0: w0
  *a1: w1
  *a2: w2
  *a3: w3
- &seq
  a4: v4
)";
        Tree t = parse(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
        ASSERT_EQ(t.rootref().num_children(), 3u);
        ASSERT_EQ(t[2].has_val_anchor(), true);
        ASSERT_EQ(t[2].val_anchor(), "seq");
    }
    {
        // but this case may fail because the indentation
        // may be set from the scalar instead of the tag:
        csubstr yaml = R"(- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1: w1
  *a2: w2
  *a3: w3
)";
        Tree t = parse(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
    }
}

TEST(seq_of_map, with_tags)
{
    {
        // this case is vanilla:
        csubstr yaml = R"(- a0: v0
  !!str a1: v1
  !!str a2: v2
  !!str a3: v3
- a0: w0
  !!int a1: !!str w1
  !!int a2: !!str w2
  !!int a3: !!str w3
- a0: v1
  !foo a1: v1
  !foo a2: v2
  !foo a3: v3
)";
        Tree t = parse(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
    }
    {
        // but this case may fail because the indentation
        // may be set from the scalar instead of the tag:
        csubstr yaml = R"(- !!str a1: v1
  !!str a2: v2
  !!str a3: v3
- !!int a1: !!str w1
  !!int a2: !!str w2
  !!int a3: !!str w3
- !foo a1: v1
  !foo a2: v2
  !foo a3: v3
)";
        Tree t = parse(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define SEQ_OF_MAP_CASES                                \
    "seq of empty maps, one line",                      \
    "seq of maps, one line",                            \
    "seq of maps, implicit seq, explicit maps",         \
    "seq of maps",                                      \
    "seq of maps, next line",                           \
    "seq of maps, bug #32 ex1",                         \
    "seq of maps, bug #32 ex2",                         \
    "seq of maps, bug #32 ex3",                         \
    "seq of maps, implicit map in seq",                 \
    "seq of maps, implicit map in seq, missing scalar", \
    "seq of maps, implicit with anchors, unresolved",   \
    "seq of maps, implicit with anchors, resolved",     \
    "seq of maps, implicit with tags"


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

C("seq of maps, bug #32 ex1",
R"(
- 'a': 1
  b: 2
)",
  L{
      N{L{N(QK, "a", "1"), N("b", "2")}}
  }
),

C("seq of maps, bug #32 ex2",
R"(
- a: 1
  b: 2
- b: 2
  'a': 1
- b: 2
  'a': 1
  c: 3
- {'a': 1, b: 2}
)",
  L{
      N{L{N("a", "1"), N("b", "2")}},
      N{L{N("b", "2"), N(QK, "a", "1")}},
      N{L{N("b", "2"), N(QK, "a", "1"), N("c", "3")}},
      N{L{N(QK, "a", "1"), N("b", "2")}},
  }
),

C("seq of maps, bug #32 ex3",
R"(
'a': 1
b: 2
b: 2
'a': 1
)",
L{
    N(QK, "a", "1"), N("b", "2"), N("b", "2"), N(QK, "a", "1"),
}),


C("seq of maps, implicit map in seq",
R"('implicit block key' : [
  'implicit flow key 1' : value1,
  'implicit flow key 2' : value2,
  'implicit flow key 3' : value3,
  'implicit flow key m' : {key1: val1, key2: val2},
  'implicit flow key s' : [val1, val2],
])",
L{N("implicit block key", L{
  N(L{N("implicit flow key 1", "value1")}),
  N(L{N("implicit flow key 2", "value2")}),
  N(L{N("implicit flow key 3", "value3")}),
  N(L{N("implicit flow key m", L{N("key1", "val1"), N("key2", "val2")})}),
  N(L{N("implicit flow key s", L{N("val1"), N("val2")})}),
})}),

C("seq of maps, implicit map in seq, missing scalar",
R"({a : [
  : foo
],
b : [
  :
foo
],
c : [
  :
,
  :
]})",
L{
  N("a", L{N(MAP, L{N("", "foo")}),}),
  N("b", L{N(MAP, L{N("", "foo")}),}),
  N("c", L{N(MAP, L{N(KEYVAL, "", /*"~"*/{})}), N(MAP, L{N(KEYVAL, "", /*"~"*/{})}),}),
}),


C("seq of maps, implicit with anchors, unresolved",
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1: w1
  *a2: w2
  *a3: w3
)",
L{
  N(L{N( "a1", AR(KEYANCH, "a1"), "v1"), N( "a2", AR(KEYANCH, "a2"), "v2"), N( "a3", AR(KEYANCH, "a3"), "v3")}),
  N(L{N("*a1", AR(KEYREF, "*a1"), "w1"), N("*a2", AR(KEYREF, "*a2"), "w2"), N("*a3", AR(KEYREF, "*a3"), "w3")}),
}),

C("seq of maps, implicit with anchors, resolved", RESOLVE_REFS,
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1: w1
  *a2: w2
  *a3: w3
)",
L{
  N(L{N("a1", "v1"), N("a2", "v2"), N("a3", "v3")}),
  N(L{N("a1", "w1"), N("a2", "w2"), N("a3", "w3")}),
}),


C("seq of maps, implicit with tags",
R"(
- !!str a1: v1
  !!str a2: v2
  !!str a3: v3
- a1: !!str w1
  a2: !!str w2
  a3: !!str w3
- !foo a1: v1
  !foo a2: v2
  !foo a3: v3
)",
L{
  N(L{N(TS("!!str", "a1"), "v1"), N(TS("!!str", "a2"), "v2"), N(TS("!!str", "a3"), "v3")}),
  N(L{N("a1", TS("!!str", "w1")), N("a2", TS("!!str", "w2")), N("a3", TS("!!str", "w3"))}),
  N(L{N(TS("!foo", "a1"), "v1"), N(TS("!foo", "a2"), "v2"), N(TS("!foo", "a3"), "v3")}),
}),


    )
}

INSTANTIATE_GROUP(SEQ_OF_MAP)

} // namespace yml
} // namespace c4
