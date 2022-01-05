#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_case.hpp"
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

#define _checkloc(node, line_, col_, str)                               \
    {                                                                   \
        const Location loc = parser.location(node);                     \
        EXPECT_EQ(loc.name, "myfile.yml");                              \
        EXPECT_EQ(loc.line, line_);                                     \
        EXPECT_EQ(loc.col, col_);                                       \
        EXPECT_EQ(t.arena().sub(loc.offset, csubstr(str).len), csubstr(str)); \
    }

TEST(locations, seq_block)
{
    Tree t;
    Parser parser;
    csubstr yaml = R"(
- this
- is
- a
- seq
- - and
  - this
  - - as
    - well
  - - # this one is tricky because we have a comment
      # in between the sequence start and the first scalar
      the scalar value is here
    -   and here's another value
    -
      - another val
      - yet another val
)";
    parser.parse_in_arena("myfile.yml", yaml, &t);
    const NodeRef seq = t.rootref();
    ASSERT_TRUE(seq.is_seq());
    _checkloc(seq         ,  1u, 0u, "- ");
    _checkloc(seq[0]      ,  1u, 2u, "this");
    _checkloc(seq[1]      ,  2u, 2u, "is");
    _checkloc(seq[2]      ,  3u, 2u, "a");
    _checkloc(seq[3]      ,  4u, 2u, "seq");
    _checkloc(seq[4]      ,  5u, 2u, "- ");
    _checkloc(seq[4][0]   ,  5u, 4u, "and");
    _checkloc(seq[4][1]   ,  6u, 4u, "this");
    _checkloc(seq[4][2]   ,  7u, 4u, "- ");
    _checkloc(seq[4][2][0],  7u, 6u, "as");
    _checkloc(seq[4][2][1],  8u, 6u, "well");
    _checkloc(seq[4][3]   , 11u, 6u, "the scalar value is here"); // different from above! the comment throws this off
    _checkloc(seq[4][3][0], 11u, 6u, "the scalar value is here");
    _checkloc(seq[4][3][1], 12u, 8u, "and here's another value");
    _checkloc(seq[4][3][2], 14u, 6u, "- ");
    _checkloc(seq[4][3][2][0], 14u, 8u, "another val");
    _checkloc(seq[4][3][2][1], 15u, 8u, "yet another val");
}

TEST(locations, seq_flow)
{
    Tree t;
    Parser parser;
    csubstr yaml = R"([one,two,three,four,items])";
    parser.parse_in_arena("myfile.yml", yaml, &t);
    const NodeRef seq = t.rootref();
    ASSERT_TRUE(seq.is_seq());
    _checkloc(seq   ,  0u,  0u, "[");
    _checkloc(seq[0],  0u,  1u, "one");
    _checkloc(seq[1],  0u,  5u, "two");
    _checkloc(seq[2],  0u,  9u, "three");
    _checkloc(seq[3],  0u, 15u, "four");
    _checkloc(seq[4],  0u, 20u, "items");
}

TEST(locations, seq_flow_nested)
{
    Tree t;
    Parser parser;
    csubstr yaml = R"([
  one,
  two,
  [woops,     there,    [  goes,     master]],
  five,
  {wait: is,   this:   {   really: a map? },   nope: [  a seq!, { had: you there,  eehh: no. } ]},
  yep,
  it,
  was
])";
    parser.parse_in_arena("myfile.yml", yaml, &t);
    const NodeRef seq = t.rootref();
    ASSERT_TRUE(seq.is_seq());
    _checkloc(seq                      ,  0u,  0u, "[");
    _checkloc(seq[0]                   ,  1u,  2u, "one");
    _checkloc(seq[1]                   ,  2u,  2u, "two");
    _checkloc(seq[2]                   ,  3u,  2u, "[");
    _checkloc(seq[2][0]                ,  3u,  3u, "woops");
    _checkloc(seq[2][1]                ,  3u, 14u, "there");
    _checkloc(seq[2][2]                ,  3u, 24u, "[");
    _checkloc(seq[2][2][0]             ,  3u, 27u, "goes");
    _checkloc(seq[2][2][1]             ,  3u, 37u, "master");
    _checkloc(seq[3]                   ,  4u,  2u, "five");
    _checkloc(seq[4]                   ,  5u,  2u, "{");
    _checkloc(seq[4]["wait"]           ,  5u,  3u, "wait");
    _checkloc(seq[4]["this"]           ,  5u, 15u, "this");
    _checkloc(seq[4]["this"]["really"] ,  5u, 27u, "really");
    _checkloc(seq[4]["nope"]           ,  5u, 47u, "nope");
    _checkloc(seq[4]["nope"][0]        ,  5u, 56u, "a seq!");
    _checkloc(seq[4]["nope"][1]        ,  5u, 64u, "{");
    _checkloc(seq[4]["nope"][1]["had"] ,  5u, 66u, "had");
    _checkloc(seq[4]["nope"][1]["eehh"],  5u, 83u, "eehh");
    _checkloc(seq[5]                   ,  6u,  2u, "yep");
    _checkloc(seq[6]                   ,  7u,  2u, "it");
    _checkloc(seq[7]                   ,  8u,  2u, "was");
}

TEST(locations, small)
{
    Tree t;
    Parser parser;
    csubstr yaml = R"(---
foo: definitely
bar:
  - 1
  - 2
baz:
    - 1_
    - 2_
    -     3_
)";
    parser.parse_in_arena("myfile.yml", yaml, &t);
    const NodeRef stream = t.rootref();
    const NodeRef map = t.docref(0);
    ASSERT_TRUE(map.is_map());
    ASSERT_TRUE(map.is_doc());
    _checkloc(stream       , 0u, 2u, "-");
    _checkloc(map          , 1u, 0u, "foo");
    _checkloc(map["foo"]   , 1u, 0u, "foo");
    _checkloc(map["bar"]   , 2u, 0u, "bar");
    _checkloc(map["bar"][0], 3u, 4u, "1");
    _checkloc(map["bar"][1], 4u, 4u, "2");
    _checkloc(map["baz"]   , 5u, 0u, "baz");
    _checkloc(map["baz"][0], 6u, 6u, "1_");
    _checkloc(map["baz"][1], 7u, 6u, "2_");
    _checkloc(map["baz"][2], 8u, 10u, "3_");
}

// do a test with a buffer of at least 30 lines to ensure hitting
// the binary search path
TEST(locations, large)
{
    Tree t;
    Parser parser;
    csubstr yaml = R"(---
foo1: definitely  # 1
bar1:
  - 1
  - 2
baz1:
    - 1_
    - 2_
    -     3_



foo2: definitely  # 12
bar2:
  - 1
  - 2
baz2:
    - 1_
    - 2_
    -     3_



foo3: definitely  # 23
bar3:
  - 1
  - 2
baz3:
    - 1_
    - 2_
    -     3_



foo4: definitely  # 34
bar4:
  - 1
  - 2
baz4:
    - 1_
    - 2_
    -     3_



foo5: definitely  # 45
bar5:
  - 1
  - 2
baz5:
    - 1_
    - 2_
    -     3_



foo6: definitely  # 56
bar6:
  - 1
  - 2
baz6:
    - 1_
    - 2_
    -     3_
)";
    parser.parse_in_arena("myfile.yml", yaml, &t);
    const NodeRef map = t.docref(0);
    ASSERT_TRUE(map.is_map());
    ASSERT_TRUE(map.is_doc());
    _checkloc(t.rootref()   ,     0u, 2u, "-");
    _checkloc(map           ,     1u, 0u, "foo1");
    _checkloc(map["foo1"]   ,  0u+1u, 0u, "foo1");
    _checkloc(map["bar1"]   ,  0u+2u, 0u, "bar1");
    _checkloc(map["bar1"][0],  0u+3u, 4u, "1");
    _checkloc(map["bar1"][1],  0u+4u, 4u, "2");
    _checkloc(map["baz1"]   ,  0u+5u, 0u, "baz");
    _checkloc(map["baz1"][0],  0u+6u, 6u, "1_");
    _checkloc(map["baz1"][1],  0u+7u, 6u, "2_");
    _checkloc(map["baz1"][2],  0u+8u, 10u, "3_");
    //
    _checkloc(map["foo2"]   ,  11u+1u, 0u, "foo2");
    _checkloc(map["bar2"]   ,  11u+2u, 0u, "bar2");
    _checkloc(map["bar2"][0],  11u+3u, 4u, "1");
    _checkloc(map["bar2"][1],  11u+4u, 4u, "2");
    _checkloc(map["baz2"]   ,  11u+5u, 0u, "baz2");
    _checkloc(map["baz2"][0],  11u+6u, 6u, "1_");
    _checkloc(map["baz2"][1],  11u+7u, 6u, "2_");
    _checkloc(map["baz2"][2],  11u+8u, 10u, "3_");
    //
    _checkloc(map["foo3"]   ,  22u+1u, 0u, "foo3");
    _checkloc(map["bar3"]   ,  22u+2u, 0u, "bar3");
    _checkloc(map["bar3"][0],  22u+3u, 4u, "1");
    _checkloc(map["bar3"][1],  22u+4u, 4u, "2");
    _checkloc(map["baz3"]   ,  22u+5u, 0u, "baz3");
    _checkloc(map["baz3"][0],  22u+6u, 6u, "1_");
    _checkloc(map["baz3"][1],  22u+7u, 6u, "2_");
    _checkloc(map["baz3"][2],  22u+8u, 10u, "3_");
    //
    _checkloc(map["foo4"]   ,  33u+1u, 0u, "foo4");
    _checkloc(map["bar4"]   ,  33u+2u, 0u, "bar4");
    _checkloc(map["bar4"][0],  33u+3u, 4u, "1");
    _checkloc(map["bar4"][1],  33u+4u, 4u, "2");
    _checkloc(map["baz4"]   ,  33u+5u, 0u, "baz4");
    _checkloc(map["baz4"][0],  33u+6u, 6u, "1_");
    _checkloc(map["baz4"][1],  33u+7u, 6u, "2_");
    _checkloc(map["baz4"][2],  33u+8u, 10u, "3_");
    //
    _checkloc(map["foo5"]   ,  44u+1u, 0u, "foo5");
    _checkloc(map["bar5"]   ,  44u+2u, 0u, "bar5");
    _checkloc(map["bar5"][0],  44u+3u, 4u, "1");
    _checkloc(map["bar5"][1],  44u+4u, 4u, "2");
    _checkloc(map["baz5"]   ,  44u+5u, 0u, "baz5");
    _checkloc(map["baz5"][0],  44u+6u, 6u, "1_");
    _checkloc(map["baz5"][1],  44u+7u, 6u, "2_");
    _checkloc(map["baz5"][2],  44u+8u, 10u, "3_");
    //
    _checkloc(map["foo6"]   ,  55u+1u, 0u, "foo6");
    _checkloc(map["bar6"]   ,  55u+2u, 0u, "bar6");
    _checkloc(map["bar6"][0],  55u+3u, 4u, "1");
    _checkloc(map["bar6"][1],  55u+4u, 4u, "2");
    _checkloc(map["baz6"]   ,  55u+5u, 0u, "baz6");
    _checkloc(map["baz6"][0],  55u+6u, 6u, "1_");
    _checkloc(map["baz6"][1],  55u+7u, 6u, "2_");
    _checkloc(map["baz6"][2],  55u+8u, 10u, "3_");
}


// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
