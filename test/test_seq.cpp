#include "./test_lib/test_utils.hpp"
#include "./test_lib/test_helpers_tree.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/emit.hpp"
#include "c4/yml/parse.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(simple_seq, bad_seq1)
{
    Tree tree;
    ExpectError::check_error_parse(&tree, [&]{
        parse_in_arena(R"(
---
[ a, b, c ] ]
)", &tree);
    });
}

TEST(simple_seq, bad_seq2)
{
    Tree tree;
    ExpectError::check_error_parse(&tree, [&]{
        parse_in_arena(R"(
---
[ [ a, b, c ]
)", &tree);
    });
}

TEST(simple_seq, two_nested_flow_seqs)
{
    Tree tree = parse_in_arena("[[]]");
    EXPECT_TRUE(tree.rootref().is_seq());
    ASSERT_TRUE(tree.rootref().has_children());
    EXPECT_TRUE(tree.rootref().first_child().is_seq());
    ASSERT_FALSE(tree.rootref().first_child().has_children());
}

TEST(simple_seq, many_unmatched_brackets)
{
    std::string src;
    src.reserve(10000000u);
    for(size_t num_brackets : {4u, 8u, 32u, 64u})
    {
        SCOPED_TRACE(num_brackets);
        for(size_t i = src.size(); i < num_brackets; ++i)
            src += '[';
        Tree tree;
        Location loc = {};
        loc.line = 1;
        loc.col = num_brackets + 1u;
        ExpectError::check_error_parse(&tree, [&]{
            parse_in_place(to_substr(src), &tree);
        }, loc);
    }
}

TEST(simple_seq, missing_quoted_key)
{
    csubstr yaml = R"(
"top1" :
  ["0", "1", ]
'top2' :
  ["0", "1", ]
---
"top1" :
  - "0"
  - "1"
'top2' :
  - "0"
  - "1"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef n = t.docref(0);
        EXPECT_TRUE(n["top1"].is_key_quoted());
        EXPECT_TRUE(n["top2"].is_key_quoted());
        EXPECT_TRUE(n["top1"][0].is_val_quoted());
        EXPECT_TRUE(n["top1"][1].is_val_quoted());
        EXPECT_TRUE(n["top2"][0].is_val_quoted());
        EXPECT_TRUE(n["top2"][1].is_val_quoted());
        n = t.docref(1);
        EXPECT_TRUE(n["top1"].is_key_quoted());
        EXPECT_TRUE(n["top2"].is_key_quoted());
        EXPECT_TRUE(n["top1"][0].is_val_quoted());
        EXPECT_TRUE(n["top1"][1].is_val_quoted());
        EXPECT_TRUE(n["top2"][0].is_val_quoted());
        EXPECT_TRUE(n["top2"][1].is_val_quoted());
    });
}

TEST(simple_seq, block_nested_with_tags)
{
    std::string yaml = R"(- !!seq
  - !!seq
    - foo
- bar
)";
    Tree t = parse_in_arena(to_csubstr(yaml));
    EXPECT_EQ(yaml, emitrs_yaml<std::string>(t));
}

TEST(simple_seq, deeply_nested_to_cover_parse_stack_resizes)
{
    csubstr yaml = R"(
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[0, 1, 2, 3, 4, 5, 6, 7]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
)";
    Tree t = parse_in_arena(yaml);
    id_type id = t.root_id();
    while(t.has_children(id))
        id = t.first_child(id);
    ASSERT_TRUE(t.cref(id).has_parent());
    ConstNodeRef seq = t.cref(id).parent();
    ASSERT_TRUE(seq.is_seq());
    EXPECT_EQ(seq[0].val(), csubstr("0"));
    EXPECT_EQ(seq[1].val(), csubstr("1"));
    EXPECT_EQ(seq[2].val(), csubstr("2"));
    EXPECT_EQ(seq[3].val(), csubstr("3"));
    EXPECT_EQ(seq[4].val(), csubstr("4"));
    EXPECT_EQ(seq[5].val(), csubstr("5"));
    EXPECT_EQ(seq[6].val(), csubstr("6"));
    EXPECT_EQ(seq[7].val(), csubstr("7"));
}


#ifdef RYML_WITH_TAB_TOKENS
TEST(simple_seq, block_tab_tokens__0_spaces_only)
{
    Tree tree = parse_in_arena(R"(
- 0
- 1
- 2
)");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}
TEST(simple_seq, block_tab_tokens__1_tabs_after)
{
    Tree tree = parse_in_arena(R"(
-	0
-	1
-	2
)");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}
TEST(simple_seq, block_tab_tokens__2_tabs_before_after)
{
    Tree tree = parse_in_arena(R"(
-	0	
-	1	
-	2	
)");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}
TEST(simple_seq, block_tab_tokens__3_tabs_everywhere)
{
    Tree tree = parse_in_arena(R"(
-	0	0	
-	1	1	
-	2	2	
)");
    EXPECT_EQ(tree[0].val(), csubstr("0\t0"));
    EXPECT_EQ(tree[1].val(), csubstr("1\t1"));
    EXPECT_EQ(tree[2].val(), csubstr("2\t2"));
}
TEST(simple_seq, block_tab_tokens__4_tabs_indentation_error)
{
    ExpectError::check_error_parse([]{
        Tree tree = parse_in_arena(R"(
	-	0	0	
	-	1	1	
	-	2	2	
)");
    });
}

TEST(simple_seq, flow_tab_tokens__0_flow_no_tabs)
{
    Tree tree = parse_in_arena(R"([0, 1, 2])");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}

TEST(simple_seq, flow_tab_tokens__1_flow_tabs_after)
{
    Tree tree = parse_in_arena(R"([0,	1,	2]	)");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}

TEST(simple_seq, flow_tab_tokens__2_flow_tabs_before_and_after)
{
    Tree tree = parse_in_arena(R"([0	,	1	,	2])");
    EXPECT_EQ(tree[0].val(), csubstr("0"));
    EXPECT_EQ(tree[1].val(), csubstr("1"));
    EXPECT_EQ(tree[2].val(), csubstr("2"));
}

TEST(simple_seq, flow_tab_tokens__3_flow_tabs_everywhere)
{
    Tree tree = parse_in_arena(R"(	[	
	0	0	,	
	1	1	,	
	2	2	,	
	]	
)");
    EXPECT_EQ(tree[0].val(), csubstr("0\t0"));
    EXPECT_EQ(tree[1].val(), csubstr("1\t1"));
    EXPECT_EQ(tree[2].val(), csubstr("2\t2"));
}
#endif // RYML_WITH_TAB_TOKENS


TEST(simple_seq, unterminated_seqimap)
{
    {
        SCOPED_TRACE("space after");
        Tree t;
        ExpectError::check_error_parse(&t, [&]{
            parse_in_arena("[a: ");
        });
    }
    {
        SCOPED_TRACE("no space after");
        Tree t;
        ExpectError::check_error_parse(&t, [&]{
            parse_in_arena("[a:");
        });
    }
}

TEST(seq_of_map, with_anchors)
{
    {
        // this case is vanilla:
        csubstr yaml = R"(- a0: v0
  &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- a0: w0
  *a1 : w1
  *a2 : w2
  *a3 : w3
- &seq
  a4: v4
)";
        Tree t = parse_in_arena(yaml);
        std::string emitted = emitrs_yaml<std::string>(t);
        EXPECT_EQ(yaml, emitted);
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
- *a1 : w1
  *a2 : w2
  *a3 : w3
)";
        Tree t = parse_in_arena(yaml);
        EXPECT_EQ(emitrs_yaml<std::string>(t), yaml);
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
        Tree t = parse_in_arena(yaml);
        EXPECT_EQ(emitrs_yaml<std::string>(t), yaml);
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
        Tree t = parse_in_arena(yaml);
        EXPECT_EQ(emitrs_yaml<std::string>(t), yaml);
    }
}

TEST(seq_of_map, missing_scalars_v1)
{
    Tree t = parse_in_arena(R"(a:
  - ~: ~
  - ~: ~
)");
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    ASSERT_EQ(t["a"].num_children(), 2u);
    ASSERT_EQ(t["a"][0].num_children(), 1u);
    EXPECT_EQ(t["a"][0].first_child().key(), "~");
    EXPECT_EQ(t["a"][0].first_child().val(), "~");
    ASSERT_EQ(t["a"][1].num_children(), 1u);
    EXPECT_EQ(t["a"][1].first_child().key(), "~");
    EXPECT_EQ(t["a"][1].first_child().val(), "~");
}

static void test_missing_scalars(csubstr src)
{
    Tree t = parse_in_arena(src);
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    ASSERT_EQ(t["a"].num_children(), 2u);
    ASSERT_EQ(t["a"][0].num_children(), 1u);
    EXPECT_EQ(t["a"][0].first_child().key().len, 0);
    EXPECT_EQ(t["a"][0].first_child().val().len, 0);
    EXPECT_EQ(t["a"][0].first_child().key().str, nullptr);
    EXPECT_EQ(t["a"][0].first_child().val().str, nullptr);
    ASSERT_EQ(t["a"][1].num_children(), 1u);
    EXPECT_EQ(t["a"][1].first_child().key().len, 0);
    EXPECT_EQ(t["a"][1].first_child().val().len, 0);
    EXPECT_EQ(t["a"][1].first_child().key().str, nullptr);
    EXPECT_EQ(t["a"][1].first_child().val().str, nullptr);
}

TEST(seq_of_map, missing_scalars_v2)
{
    test_missing_scalars(R"(a:
  - : 
  - : 
)");
}
TEST(seq_of_map, missing_scalars_v3)
{
    test_missing_scalars(R"(a:
  - :
  - :
)");
}

} // namespace yml
} // namespace c4
