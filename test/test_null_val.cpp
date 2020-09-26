#include "./test_group.hpp"

namespace c4 {
namespace yml {
#define NULL_VAL_CASES \
    "null map vals, expl",\
    "null map vals, impl",\
    "null seq vals, impl",\
    "null seq vals in map, impl, mixed 1",\
    "null seq vals in map, impl, mixed 2",\
    "null seq vals in map, impl, mixed 3",\
    "null map vals in seq, impl, mixed 1",\
    "null map vals in seq, impl, mixed 2",\
    "null map vals in seq, impl, mixed 3",\
    "issue84.1",\
    "issue84.2",\
    "issue84.3"


TEST(null_val, simple)
{
    auto tree = parse("{foo: , bar: '', baz: [,,,], bat: [ , , , ], two: [,,], one: [,], empty: []}");

    EXPECT_EQ(tree["foo"].val(), nullptr);
    EXPECT_EQ(tree["bar"].val(), "");
    ASSERT_EQ(tree["baz"].num_children(), 3u);
    EXPECT_EQ(tree["baz"][0].val(), nullptr);
    EXPECT_EQ(tree["baz"][1].val(), nullptr);
    EXPECT_EQ(tree["baz"][2].val(), nullptr);
    ASSERT_EQ(tree["bat"].num_children(), 3u);
    EXPECT_EQ(tree["bat"][0].val(), nullptr);
    EXPECT_EQ(tree["bat"][1].val(), nullptr);
    EXPECT_EQ(tree["bat"][2].val(), nullptr);
    ASSERT_EQ(tree["two"].num_children(), 2u);
    EXPECT_EQ(tree["two"][0].val(), nullptr);
    EXPECT_EQ(tree["two"][1].val(), nullptr);
    ASSERT_EQ(tree["one"].num_children(), 1u);
    EXPECT_EQ(tree["one"][0].val(), nullptr);
    EXPECT_EQ(tree["empty"].num_children(), 0u);

    tree = parse(R"(
# these have no space after the dash
-
-
-
# these have ONE space after the dash
- 
- 
- 
)");
    ASSERT_EQ(tree.rootref().num_children(), 6u);
    EXPECT_EQ(tree[0].val(), nullptr);
    EXPECT_EQ(tree[1].val(), nullptr);
    EXPECT_EQ(tree[2].val(), nullptr);
    EXPECT_EQ(tree[3].val(), nullptr);
    EXPECT_EQ(tree[4].val(), nullptr);
    EXPECT_EQ(tree[5].val(), nullptr);
}


CASE_GROUP(NULL_VAL)
{
    APPEND_CASES(

C("null map vals, expl",
R"({foo: , bar: , baz: }
)",
L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})}
),

C("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})}
),

C("null seq vals, impl",
R"(- 
- 
- 
)",
L{N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{})}
),

C("null seq vals in map, impl, mixed 1",
R"(
foo:
  - 
  - 
  - 
bar: 
baz: 
)",
L{N("foo", L{N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{})}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})}
),

C("null seq vals in map, impl, mixed 2",
R"(
foo:
bar: 
  - 
  - 
  - 
baz: 
)",
L{N(KEYVAL, "foo", /*"~"*/{}), N("bar", L{N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{})}), N(KEYVAL, "baz", /*"~"*/{})}
),

C("null seq vals in map, impl, mixed 3",
R"(
foo:
bar: 
baz: 
  - 
  - 
  - 
)",
L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N("baz", L{N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{})})}
),

C("null map vals in seq, impl, mixed 1",
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
L{N(L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})}), N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{})}
),

C("null map vals in seq, impl, mixed 2",
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
L{N(VAL, /*"~"*/{}), N(L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})}), N(VAL, /*"~"*/{})}
),

C("null map vals in seq, impl, mixed 3",
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
L{N(VAL, /*"~"*/{}), N(VAL, /*"~"*/{}), N(L{N(KEYVAL, "foo", /*"~"*/{}), N(KEYVAL, "bar", /*"~"*/{}), N(KEYVAL, "baz", /*"~"*/{})})}
),

C("issue84.1",
R"(
fixed case:
  foo: a
  bar: 
your case:
  foo: a
  bar: ''
whatever: baz
)",
L{
N("fixed case", L{N("foo", "a"), N(KEYVAL, "bar", /*"~"*/{})}),
N("your case", L{N("foo", "a"), N("bar", "")}),
N("whatever", "baz"),
}),

C("issue84.2",
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
L{
N("version", "0"),
N("type", "xml"),
N("param_root", L{
    N("objects", L{
        N("System", L{
            N("SameGroupActorName", ""),
            N("IsGetItemSelf", "false")
        }),
        N("General", L{
            N("Speed", "1.0"),
            N("Life", "100"),
            N("IsLifeInfinite", "false"),
            N("ElectricalDischarge", "1.0"),
            N("IsBurnOutBorn", "false"),
            N(KEYVAL, "BurnOutBornName", /*"~"*/{}),
            N("IsBurnOutBornIdent", "false"),
            N("ChangeDropTableName", ""),
        }),
    })
}),
}),

C("issue84.3",
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
L{
N("version", "10"),
N("type", "test"),
N("param_root", L{
    N("objects", L{
        N("TestContent", L{
            N("Str64_empty", ""),
            N(KEYVAL, "Str64_empty2", /*"~"*/{}),
            N("Str64_empty3", ""),
        }),
    }),
    N(KEYMAP, "lists", L{})
}),
})

    )
}

INSTANTIATE_GROUP(NULL_VAL)

} // namespace yml
} // namespace c4
