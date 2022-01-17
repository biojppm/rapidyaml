#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif

#include "./test_case.hpp"

#include <gtest/gtest.h>

namespace c4 {
namespace yml {

std::string emit2str(Tree const& t)
{
    return emitrs<std::string>(t);
}

TEST(style, flags)
{
    Tree tree = parse_in_arena("foo: bar");
    EXPECT_TRUE(tree.rootref().type().default_block());
    EXPECT_FALSE(tree.rootref().type().marked_flow());
    EXPECT_FALSE(tree.rootref().type().marked_flow_sl());
    EXPECT_FALSE(tree.rootref().type().marked_flow_ml());
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_TRUE(tree.rootref().type().marked_flow());
    EXPECT_TRUE(tree.rootref().type().marked_flow_sl());
    EXPECT_FALSE(tree.rootref().type().marked_flow_ml());
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_ML);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_TRUE(tree.rootref().type().marked_flow());
    EXPECT_FALSE(tree.rootref().type().marked_flow_sl());
    EXPECT_TRUE(tree.rootref().type().marked_flow_ml());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
csubstr scalar_yaml = R"(
this is the key: >-
  this is the multiline
  "val" with

  'empty' lines
)";

TEST(scalar, base)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(tree[0].key(), csubstr("this is the key"));
    EXPECT_EQ(tree[0].val(), csubstr("this is the multiline \"val\" with\n'empty' lines"));
}

TEST(scalar, block_literal)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    tree._add_flags(tree.root_id(), _WIP_VAL_LITERAL);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
}

TEST(scalar, block_folded)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    EXPECT_FALSE(tree.type(tree[0].id()).key_marked_folded());
    EXPECT_FALSE(tree.type(tree[0].id()).val_marked_folded());
    tree._add_flags(tree[0].id(), _WIP_VAL_FOLDED);
    EXPECT_FALSE(tree.type(tree[0].id()).key_marked_folded());
    EXPECT_TRUE(tree.type(tree[0].id()).val_marked_folded());
    EXPECT_EQ(emit2str(tree), R"(this is the key: >-
  this is the multiline "val" with
  'empty' lines
)");
    //
    tree._add_flags(tree[0].id(), _WIP_KEY_FOLDED);
    EXPECT_TRUE(tree.type(tree[0].id()).key_marked_folded());
    EXPECT_TRUE(tree.type(tree[0].id()).val_marked_folded());
    EXPECT_EQ(emit2str(tree), "? >-\n  this is the key\n: >-\n  this is the multiline \"val\" with\n  'empty' lines\n");
    //
    tree._rem_flags(tree[0].id(), _WIP_VAL_FOLDED);
    EXPECT_TRUE(tree.type(tree[0].id()).key_marked_folded());
    EXPECT_FALSE(tree.type(tree[0].id()).val_marked_folded());
    EXPECT_EQ(emit2str(tree), R"(? >-
  this is the key
: |-
  this is the multiline "val" with
  'empty' lines
)");
}

TEST(scalar, squot)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    EXPECT_FALSE(tree.type(tree[0].id()).val_marked_squo());
    tree._add_flags(tree[0].id(), _WIP_VAL_SQUO);
    EXPECT_TRUE(tree.type(tree[0].id()).val_marked_squo());
    EXPECT_EQ(emit2str(tree), R"(this is the key: 'this is the multiline "val" with

''empty'' lines'
)");
}

TEST(scalar, dquot)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    EXPECT_FALSE(tree.type(tree[0].id()).val_marked_dquo());
    tree._add_flags(tree[0].id(), _WIP_VAL_DQUO);
    EXPECT_TRUE(tree.type(tree[0].id()).val_marked_dquo());
    EXPECT_EQ(emit2str(tree), R"(this is the key: "this is the multiline \"val\" with

'empty' lines"
)");
}

TEST(scalar, plain)
{
    Tree tree = parse_in_arena(scalar_yaml);
    EXPECT_EQ(emit2str(tree), R"(this is the key: |-
  this is the multiline "val" with
  'empty' lines
)");
    EXPECT_FALSE(tree.type(tree[0].id()).val_marked_plain());
    tree._add_flags(tree[0].id(), _WIP_VAL_PLAIN);
    EXPECT_TRUE(tree.type(tree[0].id()).val_marked_plain());
    EXPECT_EQ(emit2str(tree), R"(this is the key: this is the multiline "val" with

'empty' lines
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(stream, block)
{
    Tree tree = parse_in_arena(R"(
---
scalar
%YAML 1.2
---
foo
---
bar
)");
    EXPECT_TRUE(tree.rootref().is_stream());
    EXPECT_TRUE(tree.docref(0).is_doc());
    EXPECT_TRUE(tree.docref(0).is_val());
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), "--- scalar %YAML 1.2\n--- foo\n--- bar\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(seq, block)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    EXPECT_EQ(emit2str(tree), R"(- 1
- 2
- 3
- 4
- 5
- 6
)");
}

TEST(seq, flow_sl)
{
    Tree tree = parse_in_arena("[1, 2, 3, 4, 5, 6]");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"([1,2,3,4,5,6])");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keyseq, block)
{
    Tree tree = parse_in_arena("{foo: [1, 2, 3, 4, 5, 6]}");
    EXPECT_TRUE(tree.rootref().type().default_block());
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - 2
  - 3
  - 4
  - 5
  - 6
)");
    tree = parse_in_arena("{foo: [1, [2, 3], 4, [5, 6]]}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - - 2
    - 3
  - 4
  - - 5
    - 6
)");
}

TEST(keyseq, flow_sl)
{
    Tree tree = parse_in_arena("{foo: [1, 2, 3, 4, 5, 6]}");
    EXPECT_TRUE(tree.rootref().type().default_block());
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_FALSE(tree.rootref().type().default_block());
    EXPECT_EQ(emit2str(tree), R"({foo: [1,2,3,4,5,6]})");
    //
    tree = parse_in_arena("{foo: [1, [2, 3], 4, [5, 6]]}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: [1,[2,3],4,[5,6]]})");
    //
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][1].id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  - 1
  - [2,3]
  - 4
  - [5,6]
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(map, block)
{
    Tree tree = parse_in_arena("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    EXPECT_EQ(emit2str(tree), R"(1: 10
2: 10
3: 10
4: 10
5: 10
6: 10
)");
}

TEST(map, flow_sl)
{
    Tree tree = parse_in_arena("{1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({1: 10,2: 10,3: 10,4: 10,5: 10,6: 10})");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(keymap, block)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: 10
  3: 10
  4: 10
  5: 10
  6: 10
)");
}


TEST(keymap, flow_sl)
{
    Tree tree = parse_in_arena("{foo: {1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10}}");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: 10,3: 10,4: 10,5: 10,6: 10}})");
    //
    tree = parse_in_arena("{foo: {1: 10, 2: {2: 10, 3: 10}, 4: 10, 5: {5: 10, 6: 10}}}");
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2:
    2: 10
    3: 10
  4: 10
  5:
    5: 10
    6: 10
)");
    tree._add_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"({foo: {1: 10,2: {2: 10,3: 10},4: 10,5: {5: 10,6: 10}}})");
    tree._rem_flags(tree.root_id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][1].id(), _WIP_STYLE_FLOW_SL);
    tree._add_flags(tree["foo"][3].id(), _WIP_STYLE_FLOW_SL);
    EXPECT_EQ(emit2str(tree), R"(foo:
  1: 10
  2: {2: 10,3: 10}
  4: 10
  5: {5: 10,6: 10}
)");
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
