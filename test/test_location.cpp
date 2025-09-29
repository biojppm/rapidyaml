#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

TEST(locations, default_is_no_location)
{
    {
        ParserOptions opts;
        EXPECT_EQ(opts.locations(), false);
    }
    {
        Parser::handler_type evt_handler = {};
        Parser parser(&evt_handler);
        EXPECT_EQ(parser.options().locations(), false);
    }
    {
        Parser::handler_type evt_handler = {};
        Parser parser(&evt_handler, ParserOptions{});
        EXPECT_EQ(parser.options().locations(), false);
    }
    {
        Parser::handler_type evt_handler = {};
        Parser parser(&evt_handler, ParserOptions().locations(false));
        EXPECT_EQ(parser.options().locations(), false);
    }
    {
        Parser::handler_type evt_handler = {};
        Parser parser(&evt_handler, ParserOptions().locations(true));
        EXPECT_EQ(parser.options().locations(), true);
    }
}


TEST(locations, error_is_triggered_querying_with_locations_disabled)
{
    bool parsed_ok = false;
    ExpectError::check_error([&]{
        Parser::handler_type evt_handler = {};
        Parser parser(&evt_handler, ParserOptions().locations(false));
        Tree t = parse_in_arena(&parser, "test", "foo: bar");
        parsed_ok = true;
        t["foo"].location(parser);
    });
    EXPECT_TRUE(parsed_ok);
}



#define _checkloc(node, line_, col_, str)                               \
    {                                                                   \
        const Location loc = node.location(parser);                     \
        EXPECT_EQ(loc.name, "myfile.yml");                              \
        EXPECT_EQ(loc.line, line_);                                     \
        EXPECT_EQ(loc.col, col_);                                       \
        EXPECT_EQ(t.arena().sub(loc.offset, csubstr(str).len), csubstr(str)); \
    }

TEST(locations, no_error_is_triggered_querying_with_locations)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, ParserOptions().locations(true));
    EXPECT_EQ(parser.options().locations(), true);
    Tree t = parse_in_arena(&parser, "myfile.yml", "foo: bar");
    _checkloc(t["foo"], 0, 0, "foo");
}


TEST(locations, docval)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", "docval");
    _checkloc(t.rootref(), 0u, 0u, "docval");
    t = parse_in_arena(&parser, "myfile.yml", "\n docval");
    _checkloc(t.rootref(), 1u, 1u, "docval");
    t = parse_in_arena(&parser, "myfile.yml", "\n\n docval");
    _checkloc(t.rootref(), 2u, 1u, "docval");
}

TEST(locations, docval_null)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", "~");
    _checkloc(t.rootref(), 0u, 0u, "~");
    t = parse_in_arena(&parser, "myfile.yml", "");
    _checkloc(t.rootref(), 0u, 0u, "");
    t = parse_in_arena(&parser, "myfile.yml", R"(#
#
#
#
#
)");
    _checkloc(t.rootref(), 0u, 0u, "");
}

TEST(locations, seq_block)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    csubstr yaml = R"(
- this
- is
- a
- seq
- - and
  - this
  - - as
    - well
  - - # this one works as well
      # even with a comment in between
      the scalar value is here
    -   and here's another value
    -
      - another val
      - yet another val
)";
    Tree t = parse_in_arena(&parser, "myfile.yml", yaml);
    ConstNodeRef seq = t.rootref();
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
    _checkloc(seq[4][3]   ,  9u, 4u, "- # this one works as well");
    _checkloc(seq[4][3][0], 11u, 6u, "the scalar value is here");
    _checkloc(seq[4][3][1], 12u, 8u, "and here's another value");
    _checkloc(seq[4][3][2], 14u, 6u, "- ");
    _checkloc(seq[4][3][2][0], 14u, 8u, "another val");
    _checkloc(seq[4][3][2][1], 15u, 8u, "yet another val");
}

TEST(locations, map_block)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    csubstr yaml = R"(
this: ~
is: ~
a: ~
map: ~
and:
  this:
    as: ~
    well: ~
  aswell: # this one works as well
      # even with a comment in between
    val: here
    hah: here
)";
    Tree t = parse_in_arena(&parser, "myfile.yml", yaml);
    ConstNodeRef map = t.rootref();
    ASSERT_TRUE(map.is_map());
    _checkloc(map                         ,  1u, 0u, "this:");
    _checkloc(map["this"]                 ,  1u, 0u, "this:");
    _checkloc(map["is"]                   ,  2u, 0u, "is:");
    _checkloc(map["a"]                    ,  3u, 0u, "a:");
    _checkloc(map["map"]                  ,  4u, 0u, "map:");
    _checkloc(map["and"]                  ,  5u, 0u, "and:");
    _checkloc(map["and"]["this"]          ,  6u, 2u, "this:");
    _checkloc(map["and"]["this"]["as"]    ,  7u, 4u, "as:");
    _checkloc(map["and"]["this"]["well"]  ,  8u, 4u, "well:");
    _checkloc(map["and"]["aswell"]        ,  9u, 2u, "aswell:");
    _checkloc(map["and"]["aswell"]["val"] , 11u, 4u, "val:");
    _checkloc(map["and"]["aswell"]["hah"] , 12u, 4u, "hah:");
}

TEST(locations, seq_block_null)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    const Tree t = parse_in_arena(&parser, "myfile.yml", R"(---
- ~
- ~
- notnull
- ~
- ~
---
- ~
- - ~
- - - ~
- - - - ~
- - - - - ~
- - - - ~
- - - ~
- - ~
- ~
---
-
-
-
-
  -
  -
  -
    -
      -
        -
        -
      -
    -
  -
  -
  -
-
)");
    _checkloc(t.rootref()   ,               0u, 0u, "---");
    _checkloc(t.docref(0)   ,               1u, 0u, "- ");
    _checkloc(t.docref(0)[0],               1u, 2u, "~");
    _checkloc(t.docref(0)[1],               2u, 2u, "~");
    _checkloc(t.docref(0)[2],               3u, 2u, "notnull");
    _checkloc(t.docref(0)[3],               4u, 2u, "~");
    _checkloc(t.docref(0)[4],               5u, 2u, "~");
    _checkloc(t.docref(1)   ,               7u, 0u, "- ");
    _checkloc(t.docref(1)[0],               7u, 2u, "~");
    _checkloc(t.docref(1)[1],               8u, 2u, "- ");
    _checkloc(t.docref(1)[1][0],            8u, 4u, "~");
    _checkloc(t.docref(1)[2],               9u, 2u, "- ");
    _checkloc(t.docref(1)[2][0],            9u, 4u, "- ");
    _checkloc(t.docref(1)[2][0][0],         9u, 6u, "~");
    _checkloc(t.docref(1)[3],              10u, 2u, "- ");
    _checkloc(t.docref(1)[3][0],           10u, 4u, "- ");
    _checkloc(t.docref(1)[3][0][0],        10u, 6u, "- ");
    _checkloc(t.docref(1)[3][0][0][0],     10u, 8u, "~");
    _checkloc(t.docref(1)[4],              11u, 2u, "- ");
    _checkloc(t.docref(1)[4][0],           11u, 4u, "- ");
    _checkloc(t.docref(1)[4][0][0],        11u, 6u, "- ");
    _checkloc(t.docref(1)[4][0][0][0],     11u, 8u, "- ");
    _checkloc(t.docref(1)[4][0][0][0][0],  11u, 10u, "~");
    _checkloc(t.docref(1)[5],              12u, 2u, "- ");
    _checkloc(t.docref(1)[5][0],           12u, 4u, "- ");
    _checkloc(t.docref(1)[5][0][0],        12u, 6u, "- ");
    _checkloc(t.docref(1)[5][0][0][0],     12u, 8u, "~");
    _checkloc(t.docref(1)[6],              13u, 2u, "- ");
    _checkloc(t.docref(1)[6][0],           13u, 4u, "- ");
    _checkloc(t.docref(1)[6][0][0],        13u, 6u, "~");
    _checkloc(t.docref(1)[7],              14u, 2u, "- ");
    _checkloc(t.docref(1)[7][0],           14u, 4u, "~");
    _checkloc(t.docref(1)[8],              15u, 2u, "~");
    _checkloc(t.docref(2)   ,              17u, 0u, "-");
    _checkloc(t.docref(2)[0],              17u, 0u, "-");
    _checkloc(t.docref(2)[1],              17u, 0u, "-");
    _checkloc(t.docref(2)[2],              21u, 2u, "-");
    _checkloc(t.docref(2)[3],              21u, 2u, "-");
    _checkloc(t.docref(2)[3][0],           21u, 2u, "-");
    _checkloc(t.docref(2)[3][1],           24u, 4u, "-");
    _checkloc(t.docref(2)[3][2],           24u, 4u, "-");
    _checkloc(t.docref(2)[3][2][0],        25u, 6u, "-");
    _checkloc(t.docref(2)[3][2][0][0],     26u, 8u, "-");
    _checkloc(t.docref(2)[3][2][0][0][0],  26u, 8u, "-");
    _checkloc(t.docref(2)[3][2][0][0][1],  26u, 8u, "-");
    _checkloc(t.docref(2)[3][2][1],        25u, 6u, "-");
    _checkloc(t.docref(2)[3][3],           24u, 4u, "-"); // fix: this should be after the previous child
    _checkloc(t.docref(2)[3][4],           21u, 2u, "-"); // fix: this should be after the previous child
    _checkloc(t.docref(2)[3][5],           21u, 2u, "-"); // fix: this should be after the previous child
}

TEST(locations, map_block_null)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", R"(---
~: v
---
null: v
---
 : v
)");
    _checkloc(t.rootref()   , 0u, 0u, "---");
    _checkloc(t.docref(0)   , 1u, 0u, "");
    _checkloc(t.docref(0)[0], 1u, 0u, "~");
    _checkloc(t.docref(1)   , 3u, 0u, "null");
    _checkloc(t.docref(1)[0], 3u, 0u, "null");
    _checkloc(t.docref(2)   , 5u, 1u, "");
    _checkloc(t.docref(2)[0], 5u, 3u, "");
}

TEST(locations, empty_seq)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", R"(---
- []
- []
- notnull
- []
- []
---
- []
---
[]
---
key0: []
key1: []
key2: notnull
key3: []
key4: []
---
key: []
)");
    _checkloc(t.rootref()   ,  0u, 0u, "---");
    _checkloc(t.docref(0)   ,  1u, 0u, "- ");
    _checkloc(t.docref(0)[0],  1u, 2u, "[]");
    _checkloc(t.docref(0)[1],  2u, 2u, "[]");
    _checkloc(t.docref(0)[2],  3u, 2u, "notnull");
    _checkloc(t.docref(0)[3],  4u, 2u, "[]");
    _checkloc(t.docref(0)[4],  5u, 2u, "[]");
    _checkloc(t.docref(1)   ,  7u, 0u, "- ");
    _checkloc(t.docref(1)[0],  7u, 2u, "[]");
    _checkloc(t.docref(2)   ,  9u, 0u, "[]");
    _checkloc(t.docref(3)   ,  11u, 0u, "key0"); // WTF
    _checkloc(t.docref(3)["key0"],  11u, 0u, "key0");
    _checkloc(t.docref(3)["key1"],  12u, 0u, "key1");
    _checkloc(t.docref(3)["key2"],  13u, 0u, "key2");
    _checkloc(t.docref(3)["key3"],  14u, 0u, "key3");
    _checkloc(t.docref(3)["key4"],  15u, 0u, "key4");
}

TEST(locations, empty_map)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", R"(---
- {}
- {}
- notnull
- {}
- {}
---
- {}
---
{}
---
key0: {}
key1: {}
key2: notnull
key3: {}
key4: {}
---
key: {}
)");
    _checkloc(t.rootref()   ,  0u, 0u, "---");
    _checkloc(t.docref(0)   ,  1u, 0u, "- ");
    _checkloc(t.docref(0)[0],  1u, 2u, "{}");
    _checkloc(t.docref(0)[1],  2u, 2u, "{}");
    _checkloc(t.docref(0)[2],  3u, 2u, "notnull");
    _checkloc(t.docref(0)[3],  4u, 2u, "{}");
    _checkloc(t.docref(0)[4],  5u, 2u, "{}");
    _checkloc(t.docref(1)   ,  7u, 0u, "- ");
    _checkloc(t.docref(1)[0],  7u, 2u, "{}");
    _checkloc(t.docref(2)   ,  9u, 0u, "{}");
    _checkloc(t.docref(3)   ,  11u, 0u, "key0"); // WTF
    _checkloc(t.docref(3)["key0"],  11u, 0u, "key0");
    _checkloc(t.docref(3)["key1"],  12u, 0u, "key1");
    _checkloc(t.docref(3)["key2"],  13u, 0u, "key2");
    _checkloc(t.docref(3)["key3"],  14u, 0u, "key3");
    _checkloc(t.docref(3)["key4"],  15u, 0u, "key4");
}


TEST(locations, seq_flow)
{
    Tree t;
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    csubstr yaml = R"([one,two,three,four,items])";
    parse_in_arena(&parser, "myfile.yml", yaml, &t);
    ConstNodeRef seq = t.rootref();
    ASSERT_TRUE(seq.is_seq());
    _checkloc(seq   ,  0u,  0u, "[");
    _checkloc(seq[0],  0u,  1u, "one");
    _checkloc(seq[1],  0u,  5u, "two");
    _checkloc(seq[2],  0u,  9u, "three");
    _checkloc(seq[3],  0u, 15u, "four");
    _checkloc(seq[4],  0u, 20u, "items");
}

TEST(locations, map_flow)
{
    Tree t;
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    csubstr yaml = R"({one: item,two: items,three: items,four: items})";
    parse_in_arena(&parser, "myfile.yml", yaml, &t);
    ConstNodeRef map = t.rootref();
    ASSERT_TRUE(map.is_map());
    _checkloc(map   ,  0u,  0u, "{");
    _checkloc(map[0],  0u,  1u, "one:");
    _checkloc(map[1],  0u, 11u, "two:");
    _checkloc(map[2],  0u, 22u, "three:");
    _checkloc(map[3],  0u, 35u, "four:");
}

TEST(locations, seq_flow_nested)
{
    Tree t;
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
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
    parse_in_arena(&parser, "myfile.yml", yaml, &t);
    ConstNodeRef seq = t.rootref();
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

TEST(locations, grow_array)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, "myfile.yml", "docval");
    _checkloc(t.rootref(), 0u, 0u, "docval");
    t = parse_in_arena(&parser, "myfile.yml", "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\ndocval");
    _checkloc(t.rootref(), 47u, 0u, "docval");
}

// do a test with a buffer size up to 30 lines to ensure hitting
// the binary search path
TEST(locations, small_array)
{
    Tree t;
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    csubstr yaml = R"(---
foo: yes
bar:
  - 1
  - 2
baz:
    - 1_
    - 2_
    -     3_
)";
    parse_in_arena(&parser, "myfile.yml", yaml, &t);
    ConstNodeRef stream = t.rootref();
    ConstNodeRef map = t.docref(0);
    ASSERT_TRUE(map.is_map());
    ASSERT_TRUE(map.is_doc());
    _checkloc(stream       , 0u, 0u, "---");
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
TEST(locations, large_array)
{
    Tree t;
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
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
    parse_in_arena(&parser, "myfile.yml", yaml, &t);
    ConstNodeRef map = t.docref(0);
    ASSERT_TRUE(map.is_map());
    ASSERT_TRUE(map.is_doc());
    _checkloc(t.rootref()   ,     0u, 0u, "---");
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


TEST(locations, issue260_0)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree tree = parse_in_arena(&parser, "source.yml", R"(Body:
  - Id: 1
    Name: Apple
    Script: |
      Line One
      Line Two
  - Id: 2
    Name: Cat
    Script: |
      Line One
      Line Two
  - Id: 3
    Name: Dog
    Script: |
      Line One
      Line Two)");
    EXPECT_EQ(tree["Body"][2]["Name"].val(), "Dog");
    EXPECT_EQ(tree["Body"][2]["Name"].location(parser).line, 12u);
}

TEST(locations, issue260_1)
{
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree tree = parse_in_arena(&parser, "source.yml", R"(Body:  # 0
  - Id: 1           # line 1
    Name: Apple
  - Id: 2           # line 3
    Name: Cat
    Script: |
      Line One
      Line Two
  - Id: 3           # line 8
    Name: Cat2
    Script: >
      Line One
      Line Two
  - Id: 4           # line 13
    Name: Cat3
    Script: "
      Line One
      Line Two"
  - Id: 5           # line 18
    Name: Cat4
    Script: '
      Line One
      Line Two'
  - Id: 5           # line 23
    Name: Cat5
    Script:
      Line One
      Line Two
  - Id: 6           # line 28
    Name: Dog
    Script: |
      Line One
      Line Two)");
    EXPECT_EQ(tree["Body"][0].location(parser).line, 1u);
    EXPECT_EQ(tree["Body"][1].location(parser).line, 3u);
    EXPECT_EQ(tree["Body"][2].location(parser).line, 8u);
    EXPECT_EQ(tree["Body"][3].location(parser).line, 13u);
    EXPECT_EQ(tree["Body"][4].location(parser).line, 18u);
    EXPECT_EQ(tree["Body"][5].location(parser).line, 23u);
    EXPECT_EQ(tree["Body"][6].location(parser).line, 28u);
    EXPECT_EQ(tree["Body"][0]["Id"].location(parser).line, 1u);
    EXPECT_EQ(tree["Body"][1]["Id"].location(parser).line, 3u);
    EXPECT_EQ(tree["Body"][2]["Id"].location(parser).line, 8u);
    EXPECT_EQ(tree["Body"][3]["Id"].location(parser).line, 13u);
    EXPECT_EQ(tree["Body"][4]["Id"].location(parser).line, 18u);
    EXPECT_EQ(tree["Body"][5]["Id"].location(parser).line, 23u);
    EXPECT_EQ(tree["Body"][6]["Id"].location(parser).line, 28u);
    EXPECT_EQ(tree["Body"][0]["Name"].location(parser).line, 1u+1u);
    EXPECT_EQ(tree["Body"][1]["Name"].location(parser).line, 3u+1u);
    EXPECT_EQ(tree["Body"][2]["Name"].location(parser).line, 8u+1u);
    EXPECT_EQ(tree["Body"][3]["Name"].location(parser).line, 13u+1u);
    EXPECT_EQ(tree["Body"][4]["Name"].location(parser).line, 18u+1u);
    EXPECT_EQ(tree["Body"][5]["Name"].location(parser).line, 23u+1u);
    EXPECT_EQ(tree["Body"][6]["Name"].location(parser).line, 28u+1u);
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
