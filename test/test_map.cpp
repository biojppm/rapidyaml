#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

TEST(simple_map, issue361)
{
    {
        const Tree t = parse_in_arena(R"(desc: foo bar)");
        ASSERT_EQ(t["desc"].val(), "foo bar");
    }
    //
    {
        const Tree t = parse_in_arena(R"(desc:
  foo bar)");
        ASSERT_EQ(t["desc"].val(), "foo bar");
    }
    //
    {
        const Tree t = parse_in_arena(R"(desc: foo:bar)");
        ASSERT_EQ(t["desc"].val(), "foo:bar");
    }
    //
    {
        const Tree t = parse_in_arena(R"(desc:
  foo:bar)");
        ASSERT_EQ(t["desc"].val(), "foo:bar");
    }
    //
    {
        const Tree t = parse_in_arena(R"(desc: Timecode in "HH:MM:SS:FF" format where the last ':' can be replaced by ',' or ';'.)");
        ASSERT_EQ(t["desc"].val(), "Timecode in \"HH:MM:SS:FF\" format where the last ':' can be replaced by ',' or ';'.");
    }
    //
    {
        const Tree t = parse_in_arena(R"(desc:
  Timecode in "HH:MM:SS:FF" format where the last ':' can be replaced by ',' or ';'.)");
        ASSERT_EQ(t["desc"].val(), "Timecode in \"HH:MM:SS:FF\" format where the last ':' can be replaced by ',' or ';'.");
    }
    //
    {
        const Tree t = parse_in_arena(R"(value:
  desc:
    Timecode in "HH:MM:SS:FF" format where the last ':' can be replaced by ',' or ';'.)");
        ASSERT_EQ(t["value"]["desc"].val(), "Timecode in \"HH:MM:SS:FF\" format where the last ':' can be replaced by ',' or ';'.");
    }
}

TEST(simple_map, issue377)
{
    csubstr yaml = R"(
const game::perception::PerceiveData:
      PerceiveData: {default: nullptr}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().has_child("const game::perception::PerceiveData"));
        const ConstNodeRef cpd = t["const game::perception::PerceiveData"];
        ASSERT_TRUE(cpd.is_map());
        ASSERT_TRUE(cpd.has_child("PerceiveData"));
        ASSERT_EQ(cpd["PerceiveData"].num_children(), 1u);
        ASSERT_TRUE(cpd["PerceiveData"].has_child("default"));
        ASSERT_EQ(cpd["PerceiveData"]["default"].val(), "nullptr");
    });
}

TEST(simple_map, issue373_0)
{
    test_check_emit_check(R"({"": ""})", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 1u);
        EXPECT_EQ(tree[0].key(), "");
        EXPECT_EQ(tree[0].val(), "");
        EXPECT_EQ(tree[""].key(), "");
        EXPECT_EQ(tree[""].val(), "");
    });
}

TEST(simple_map, issue373_1)
{
    test_check_emit_check(R"("": "")", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 1u);
        EXPECT_EQ(tree[0].key(), "");
        EXPECT_EQ(tree[0].val(), "");
        EXPECT_EQ(tree[""].key(), "");
        EXPECT_EQ(tree[""].val(), "");
    });
}

TEST(simple_map, issue373_2)
{
    test_check_emit_check(R"({m: {"":""}})", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 1u);
        EXPECT_EQ(tree["m"][0].key(), "");
        EXPECT_EQ(tree["m"][0].val(), "");
        EXPECT_EQ(tree["m"][""].key(), "");
        EXPECT_EQ(tree["m"][""].val(), "");
    });
}

TEST(simple_map, issue373_3)
{
    test_check_emit_check(R"(m:
    "a": ""
)", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 1u);
        ASSERT_EQ(tree["m"].num_children(), 1u);
        EXPECT_EQ(tree["m"][0].key(), "a");
        EXPECT_EQ(tree["m"][0].val(), "");
        EXPECT_EQ(tree["m"]["a"].key(), "a");
        EXPECT_EQ(tree["m"]["a"].val(), "");
    });
}

TEST(simple_map, issue373_4)
{
    test_check_emit_check(R"(m:
    "": ""
)", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 1u);
        ASSERT_EQ(tree["m"].num_children(), 1u);
        EXPECT_EQ(tree["m"][0].key(), "");
        EXPECT_EQ(tree["m"][0].val(), "");
        EXPECT_EQ(tree["m"][""].key(), "");
        EXPECT_EQ(tree["m"][""].val(), "");
    });
}

TEST(simple_map, issue373_5)
{
    test_check_emit_check(R"(m:
    "": ""
n:
    "": "a"
)", [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 2u);
        ASSERT_EQ(tree["m"].num_children(), 1u);
        EXPECT_EQ(tree["m"][0].key(), "");
        EXPECT_EQ(tree["m"][0].val(), "");
        ASSERT_EQ(tree["n"].num_children(), 1u);
        EXPECT_EQ(tree["n"][0].key(), "");
        EXPECT_EQ(tree["n"][0].val(), "a");
    });
}

TEST(simple_map, issue274)
{
    csubstr yaml = R"(
foo:
- bar
-
baz: qux
foo2:
- bar
- 
baz2: qux
)";
    test_check_emit_check(yaml, [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 4u);
        ASSERT_EQ(tree["foo"].num_children(), 2u);
        EXPECT_EQ(tree["foo"][0].val(), "bar");
        EXPECT_EQ(tree["foo"][1].val(), "");
        EXPECT_EQ(tree["baz"].val(), "qux");
        ASSERT_EQ(tree["foo2"].num_children(), 2u);
        EXPECT_EQ(tree["foo2"][0].val(), "bar");
        EXPECT_EQ(tree["foo2"][1].val(), "");
        EXPECT_EQ(tree["baz2"].val(), "qux");
    });
}

TEST(simple_map, keys_with_leading_colon)
{
    csubstr yaml = R"(
:foo:
  :bar: a
  :barbar: b
  :barbarbar: c
)";
    test_check_emit_check(yaml, [](Tree const &tree){
        EXPECT_EQ(tree[":foo"][":bar"].val(), "a");
        EXPECT_EQ(tree[":foo"][":barbar"].val(), "b");
        EXPECT_EQ(tree[":foo"][":barbarbar"].val(), "c");
    });
}

TEST(simple_map, val_with_leading_colon)
{
    csubstr yaml = R"(
foo:
  bar: :a
  barbar: :b
  barbarbar: :c
)";
    test_check_emit_check(yaml, [](Tree const &tree){
        EXPECT_EQ(tree["foo"]["bar"].val(), ":a");
        EXPECT_EQ(tree["foo"]["barbar"].val(), ":b");
        EXPECT_EQ(tree["foo"]["barbarbar"].val(), ":c");
    });
}

TEST(simple_map, open_on_new_doc_without_space)
{
    csubstr yaml = R"(
foo: bar
---
foo: bar
---
foo: bar
---
foo:
...
foo:
---
)";
    test_check_emit_check(yaml, [](Tree const &tree){
        EXPECT_EQ(tree.docref(0)["foo"].val(), "bar");
        EXPECT_EQ(tree.docref(1)["foo"].val(), "bar");
        EXPECT_EQ(tree.docref(2)["foo"].val(), "bar");
        EXPECT_EQ(tree.docref(3)["foo"].val(), "");
        EXPECT_EQ(tree.docref(4)["foo"].val(), "");
    });
}

TEST(simple_map, open_on_new_doc_with_space_before_colon)
{
    csubstr yaml = R"(
foo0 : bar
---
foo1 : bar  # the " :" was causing an assert
---
foo2 : bar
---
foo3	: bar  # this is a tab
---
foo4   	  : bar  # tabs and spaces
---
foo5	   	  : bar  # spaces and tabs
)";
    test_check_emit_check(yaml, [](Tree const &tree){
        ASSERT_EQ(tree.rootref().num_children(), 6u);
        EXPECT_EQ(tree.docref(0)["foo0"].val(), "bar");
        EXPECT_EQ(tree.docref(1)["foo1"].val(), "bar");
        EXPECT_EQ(tree.docref(2)["foo2"].val(), "bar");
        EXPECT_EQ(tree.docref(3)["foo3"].val(), "bar");
        EXPECT_EQ(tree.docref(4)["foo4"].val(), "bar");
        EXPECT_EQ(tree.docref(5)["foo5"].val(), "bar");
    });
}


TEST(simple_map, test_suite_UT92_0)
{
    csubstr yaml = R"(
- { matches
 % : 20 }
- { matches
 %: 20 }
- { matches
 %:
 20 }
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t[0].has_child("matches %"));
        EXPECT_EQ(t[0]["matches %"].val(), "20");
        ASSERT_TRUE(t[0].has_child("matches %"));
        ASSERT_TRUE(t[1].has_child("matches %"));
        EXPECT_EQ(t[1]["matches %"].val(), "20");
        ASSERT_TRUE(t[1].has_child("matches %"));
        ASSERT_TRUE(t[2].has_child("matches %"));
        EXPECT_EQ(t[2]["matches %"].val(), "20");
        ASSERT_TRUE(t[2].has_child("matches %"));
    });
}

#ifdef TO_BE_FIXED
TEST(simple_map, test_suite_UT92_1)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
        csubstr yaml = R"(
- { matches
% : 20 }
- { matches
%: 20 }
- { matches
%:
 20 }
)";
        parse_in_arena(yaml, &tree);
        print_tree(tree);
    });
}
#endif

TEST(simple_map, two_nested_flow_maps_not_accepted_because_of_container_key)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
        parse_in_arena("{{}}", &tree);
    });
}

TEST(simple_map, many_unmatched_brackets)
{
    std::string src;
    src.reserve(10000000u);
    for(size_t num_brackets : {4u, 8u, 32u, 256u, 4096u, 1024u})
    {
        SCOPED_TRACE(num_brackets);
        for(size_t i = src.size(); i < num_brackets; ++i)
            src += '{';
        Tree tree;
        ExpectError::check_error(&tree, [&]{
            parse_in_place(to_substr(src), &tree);
        });
    }
}

TEST(simple_map, missing_quoted_key_0)
{
    csubstr yaml = R"(
'x5':
'y5': z
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t["x5"].is_key_quoted());
        EXPECT_TRUE(t["y5"].is_key_quoted());
        EXPECT_EQ(t["y5"].val(), "z");
    });
}

TEST(simple_map, missing_quoted_key_1)
{
    csubstr yaml = R"(
"top1" :
  "key1" : scalar1
'top2' :
  'key2' : scalar2
---
"top1" :
  "key1" : scalar1
'top2' :
  'key2' : scalar2
---
'x2': {'y': z}
---
'x3':
  'y': z
---
x4:
  'y': z
---
'x5':
'y5': z
---
x6:
'y6': z
---
'x7' : [
  'y' : z,
 ]
---
"x8" :
  "y" : value,
  "x" : value
"y8" :
  "y" : value,
  "x" : value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        id_type doc = 0;
        EXPECT_TRUE(t.docref(doc)["top1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"]["key1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"]["key2"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["top1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"]["key1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"]["key2"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x2"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x2"]["y"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x3"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x3"]["y"].is_key_quoted());
        ++doc;
        EXPECT_FALSE(t.docref(doc)["x4"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x4"]["y"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x5"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y5"].is_key_quoted());
        ++doc;
        EXPECT_FALSE(t.docref(doc)["x6"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y6"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x7"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x7"][0]["y"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x8"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x8"]["y"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x8"]["x"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y8"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y8"]["y"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y8"]["x"].is_key_quoted());
    });
}

void verify_error_is_reported(csubstr case_name, csubstr yaml, LineCol lc={})
{
    SCOPED_TRACE(case_name);
    SCOPED_TRACE(yaml);
    Tree tree;
    Location loc = {};
    loc.line = lc.line;
    loc.col = lc.col;
    ExpectError::check_error(&tree, [&](){
        parse_in_arena(yaml, &tree);
    }, loc);
}

TEST(simple_map, no_map_key_flow)
{
    verify_error_is_reported("map key", R"({ first: Sammy, last: Sosa }: foo)", LineCol{1,29});
}

TEST(simple_map, no_map_key_block)
{
    verify_error_is_reported("map key", R"(?
  first: Sammy
  last: Sosa
:
  foo
)", LineCol{2,9});
}

TEST(simple_map, no_seq_key_flow)
{
    verify_error_is_reported("seq key", R"([Sammy, Sosa]: foo)", LineCol{1, 14});
}

TEST(simple_map, no_seq_key_block)
{
    verify_error_is_reported("map key", R"(?
  - Sammy
  - Sosa
:
  foo
)", LineCol{2, 3});
}

#ifdef RYML_WITH_TAB_TOKENS

TEST(simple_map, block_tab_tokens__0_spaces_only)
{
    Tree tree = parse_in_arena(R"(
a: 0
b: 1
c: 2
)");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, block_tab_tokens__1_tabs_after_token)
{
    Tree tree = parse_in_arena(R"(
a:	0
b:	1
c:	2
)");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, block_tab_tokens__2_tabs_before_and_after_token)
{
    Tree tree = parse_in_arena(R"(
a	:	0
b	:	1
c	:	2
)");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, block_tab_tokens__3_tabs_before_token)
{
    Tree tree = parse_in_arena(R"(
a	: 0
b	: 1
c	: 2
)");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, block_tab_tokens__4_tabs_before_newline)
{
    Tree tree = parse_in_arena(R"(
a	:	0	
b	:	1	
c	:	2	
)");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, block_tab_tokens__5_tabs_everywhere)
{
    Tree tree = parse_in_arena(R"(
a	a	:	0	0	
b	b	:	1	1	
c	c	:	2	2	
)");
    EXPECT_EQ(tree["a\ta"].val(), csubstr("0\t0"));
    EXPECT_EQ(tree["b\tb"].val(), csubstr("1\t1"));
    EXPECT_EQ(tree["c\tc"].val(), csubstr("2\t2"));
}


TEST(simple_map, flow_tab_tokens__0_no_tabs)
{
    Tree tree = parse_in_arena(R"({a: 0, b: 1, c: 2})");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, flow_tab_tokens__1_tabs_after_token)
{
    Tree tree = parse_in_arena(R"({a:	0,	b:	1,	c:	2})");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, flow_tab_tokens__2_tabs_before_after_token)
{
    Tree tree = parse_in_arena(R"({a	:	0,	b	:	1,	c	:	2})");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, flow_tab_tokens__3_tabs_after_val)
{
    Tree tree = parse_in_arena(R"({a	: 0	,	b	:	1	,	c	:	2	})");
    EXPECT_EQ(tree["a"].val(), csubstr("0"));
    EXPECT_EQ(tree["b"].val(), csubstr("1"));
    EXPECT_EQ(tree["c"].val(), csubstr("2"));
}

TEST(simple_map, flow_tab_tokens__4_everywhere)
{
    Tree tree = parse_in_arena(R"(	{a	a	: 0	0	,	b	b	:	1	1	,	c	c	:	2	2	}	)");
    EXPECT_EQ(tree["a\ta"].val(), csubstr("0\t0"));
    EXPECT_EQ(tree["b\tb"].val(), csubstr("1\t1"));
    EXPECT_EQ(tree["c\tc"].val(), csubstr("2\t2"));
}

TEST(simple_map, flow_tab_tokens__5_everywhere)
{
    Tree tree = parse_in_arena(R"(	
	{	
	a	a	:	0	0	,	
	b	b	:	1	1	,	
	c	c	:	2	2	
	}	
	)");
    EXPECT_EQ(tree["a\ta"].val(), csubstr("0\t0"));
    EXPECT_EQ(tree["b\tb"].val(), csubstr("1\t1"));
    EXPECT_EQ(tree["c\tc"].val(), csubstr("2\t2"));
}

#endif // RYML_WITH_TAB_TOKENS

TEST(simple_map, tokens_after_flow_0_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }foo", LineCol{1,32});
}

TEST(simple_map, tokens_after_flow_0_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\nfoo", LineCol{2,4});
}

TEST(simple_map, tokens_after_flow_2_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }- foo", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_2_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n- foo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_3_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }[foo,bar]", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_3_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n[foo,bar]", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_4_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }{foo: bar}", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_4_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n{foo: bar}", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_5_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }'foo'", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_5_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n'foo'", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_6_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }|\nfoo", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_6_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n|\nfoo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_7_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }>\nfoo", LineCol{1,29});
}

TEST(simple_map, tokens_after_flow_7_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n>\nfoo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_8_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }?foo", LineCol{1,33});
}

TEST(simple_map, tokens_after_flow_8_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n?\nfoo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_9_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }:foo", LineCol{1,33});
}

TEST(simple_map, tokens_after_flow_9_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n:\nfoo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_10_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }: foo", LineCol{1,29});// fixme
}

TEST(simple_map, tokens_after_flow_10_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n: \nfoo", LineCol{2,1});
}

TEST(simple_map, tokens_after_flow_11_0)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa } &foo", LineCol{1,34});// fixme
}

TEST(simple_map, tokens_after_flow_11_1)
{
    verify_error_is_reported("", "{ first: Sammy, last: Sosa }\n&foo", LineCol{2,5});
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(SIMPLE_MAP)
{
//
ADD_CASE_TO_GROUP("empty map",
"{}",
    MFS
);

ADD_CASE_TO_GROUP("empty map, multiline",
R"({
}
)",
    MFS
);

ADD_CASE_TO_GROUP("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MFS
);

ADD_CASE_TO_GROUP("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
N(MFS, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
N(MFS, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
N(MFS, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, values on next line",
R"(
foo:
  0
bar:
  1
baz:
  2
bat:
  3
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a baz
bat: 3   # this is a bat
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, with comments interspersed",
R"(
# this is a foo
foo: 0
# this is a bar
bar: 1
# this is a baz
baz: 2
# this is a bat
bat: 3
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, with indented comments interspersed, before",
R"(
  # this is a foo
foo: 0
  # this is a bar
bar: 1
  # this is a baz
baz: 2
  # this is a bat
bat: 3
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, with indented comments interspersed, after",
R"(
foo: 0
  # this is a foo
bar: 1
  # this is a bar
baz: 2
  # this is a baz
bat: 3
  # this is a bat
)",
N(MB, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, with comments interspersed implicit key X8DW",
R"(
? key
# comment
: value
)",
N(MB, L{N(KP|VP, "key", "value")})
);

ADD_CASE_TO_GROUP("simple map, null values",
R"(
key: val
a:
b:
c:
d:
e:
f:
g:
foo: bar
)",
N(MB, L{N(KP|VP, "key", "val"), N(KP|VP, "a", {}), N(KP|VP, "b", {}), N(KP|VP, "c", {}), N(KP|VP, "d", {}), N(KP|VP, "e", {}), N(KP|VP, "f", {}), N(KP|VP, "g", {}), N(KP|VP, "foo", "bar"),})
);

ADD_CASE_TO_GROUP("simple map expl, null values 1",
R"({key: val, a, b, c, d, e: , f: , g: , foo: bar})",
N(MFS, L{N(KP|VP, "key", "val"), N(KP|VP, "a", {}), N(KP|VP, "b", {}), N(KP|VP, "c", {}), N(KP|VP, "d", {}), N(KP|VP, "e", {}), N(KP|VP, "f", {}), N(KP|VP, "g", {}), N(KP|VP, "foo", "bar"),})
);

ADD_CASE_TO_GROUP("simple map expl, null values 2",
R"(
- {a}
- {a, b, c}
- {a: 1, b: 2, c}
- {a: 1, b, c: 2}
- {a, b: 1, c: 2}
)",
N(SB, L{
   N(MFS, L{N(KP|VP, "a", {})}),
   N(MFS, L{N(KP|VP, "a", {}), N(KP|VP, "b", {}), N(KP|VP, "c", {})}),
   N(MFS, L{N(KP|VP, "a", "1"), N(KP|VP, "b", "2"), N(KP|VP, "c", {})}),
   N(MFS, L{N(KP|VP, "a", "1"), N(KP|VP, "b", {}), N(KP|VP, "c", "2")}),
   N(MFS, L{N(KP|VP, "a", {}), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
 })
);

ADD_CASE_TO_GROUP("simple map expl, null values 3, 4ABK",
R"(
- {foo: , bar: , baz: }
- {foo:, bar:, baz:}
- {foo:foo: , bar:bar: , baz:baz: }
- {foo:foo:, bar:bar:, baz:baz:}
- {:foo:foo: , :bar:bar: , :baz:baz: }
- {:foo:foo:, :bar:bar:, :baz:baz:}
)",
N(SB, L{
  N(MFS, L{N(KP|VP, "foo", {}), N(KP|VP, "bar", {}), N(KP|VP, "baz", {})}),
  N(MFS, L{N(KP|VP, "foo", {}), N(KP|VP, "bar", {}), N(KP|VP, "baz", {})}),
  N(MFS, L{N(KP|VP, "foo:foo", {}), N(KP|VP, "bar:bar", {}), N(KP|VP, "baz:baz", {})}),
  N(MFS, L{N(KP|VP, "foo:foo", {}), N(KP|VP, "bar:bar", {}), N(KP|VP, "baz:baz", {})}),
  N(MFS, L{N(KP|VP, ":foo:foo", {}), N(KP|VP, ":bar:bar", {}), N(KP|VP, ":baz:baz", {})}),
  N(MFS, L{N(KP|VP, ":foo:foo", {}), N(KP|VP, ":bar:bar", {}), N(KP|VP, ":baz:baz", {})}),
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, comma",
R"(
a,b: val,000
c,d: val, 000
e,f: val , 000
h,i: val ,000
a, b: val,000
c, d: val, 000
e, f: val , 000
h, i: val ,000
a , b: val,000
c , d: val, 000
e , f: val , 000
h , i: val ,000
a ,b: val,000
c ,d: val, 000
e ,f: val , 000
h ,i: val ,000
)",
N(MB, L{
  N(KP|VP, "a,b", "val,000"), N(KP|VP, "c,d", "val, 000"), N(KP|VP, "e,f", "val , 000"), N(KP|VP, "h,i", "val ,000"),
  N(KP|VP, "a, b", "val,000"), N(KP|VP, "c, d", "val, 000"), N(KP|VP, "e, f", "val , 000"), N(KP|VP, "h, i", "val ,000"),
  N(KP|VP, "a , b", "val,000"), N(KP|VP, "c , d", "val, 000"), N(KP|VP, "e , f", "val , 000"), N(KP|VP, "h , i", "val ,000"),
  N(KP|VP, "a ,b", "val,000"), N(KP|VP, "c ,d", "val, 000"), N(KP|VP, "e ,f", "val , 000"), N(KP|VP, "h ,i", "val ,000"),
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, semicolon",
R"(
a:b: val:000
c:d: "val: 000"
e:f: "val : 000"
h:i: val :000
"a: b": val:000
"c: d": "val: 000"
"e: f": "val : 000"
"h: i": val :000
"a : b": val:000
"c : d": "val: 000"
"e : f": "val : 000"
"h : i": val :000
a :b: val:000
c :d: "val: 000"
e :f: "val : 000"
h :i: val :000
)",
N(MB, L{
  N{KP|VP, "a:b", "val:000"},  N{KP|VD, "c:d", "val: 000"},  N{KP|VD, "e:f", "val : 000"},  N{KP|VP, "h:i", "val :000"},
  N{KD|VP, "a: b", "val:000"}, N{KD|VD, "c: d", "val: 000"}, N{KD|VD, "e: f", "val : 000"}, N{KD|VP, "h: i", "val :000"},
  N{KD|VP, "a : b", "val:000"},N{KD|VD, "c : d", "val: 000"},N{KD|VD, "e : f", "val : 000"},N{KD|VP, "h : i", "val :000"},
  N{KP|VP, "a :b", "val:000"}, N{KP|VD, "c :d", "val: 000"}, N{KP|VD, "e :f", "val : 000"}, N{KP|VP, "h :i", "val :000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, cardinal",
R"(
a#b: val#000
c#d: val# 000
e#f: "val # 000"
h#i: "val #000"

a# b: val#000
c# d: val# 000
e# f: "val # 000"
h# i: "val #000"

"a # b": val#000
"c # d": val# 000
"e # f": "val # 000"
"h # i": "val #000"

"a #b": val#000
"c #d": val# 000
"e #f": "val # 000"
"h #i": "val #000"
)",
N(MB, L{
  N{KP|VP, "a#b", "val#000"},  N{KP|VP, "c#d", "val# 000"},  N{KP|VD, "e#f", "val # 000"},  N{KP|VD, "h#i", "val #000"},
  N{KP|VP, "a# b", "val#000"}, N{KP|VP, "c# d", "val# 000"}, N{KP|VD, "e# f", "val # 000"}, N{KP|VD, "h# i", "val #000"},
  N{KD|VP, "a # b", "val#000"},N{KD|VP, "c # d", "val# 000"},N{KD|VD, "e # f", "val # 000"},N{KD|VD, "h # i", "val #000"},
  N{KD|VP, "a #b", "val#000"}, N{KD|VP, "c #d", "val# 000"}, N{KD|VD, "e #f", "val # 000"}, N{KD|VD, "h #i", "val #000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, dash",
R"(
a-b: val-000
c-d: val- 000
e-f: val - 000
h-i: val -000
a- b: val-000
c- d: val- 000
e- f: val - 000
h- i: val -000
a - b: val-000
c - d: val- 000
e - f: val - 000
h - i: val -000
a -b: val-000
c -d: val- 000
e -f: val - 000
h -i: val -000
)",
N(MB, L{
  N{KP|VP, "a-b", "val-000"}, N{KP|VP, "c-d", "val- 000"}, N{KP|VP, "e-f", "val - 000"}, N{KP|VP, "h-i", "val -000"},
  N{KP|VP, "a- b", "val-000"}, N{KP|VP, "c- d", "val- 000"}, N{KP|VP, "e- f", "val - 000"}, N{KP|VP, "h- i", "val -000"},
  N{KP|VP, "a - b", "val-000"}, N{KP|VP, "c - d", "val- 000"}, N{KP|VP, "e - f", "val - 000"}, N{KP|VP, "h - i", "val -000"},
  N{KP|VP, "a -b", "val-000"}, N{KP|VP, "c -d", "val- 000"}, N{KP|VP, "e -f", "val - 000"}, N{KP|VP, "h -i", "val -000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, left-bracket",
R"(
a[b: val[000
c[d: val[ 000
e[f: val [ 000
h[i: val [000
a[ b: val[000
c[ d: val[ 000
e[ f: val [ 000
h[ i: val [000
a [ b: val[000
c [ d: val[ 000
e [ f: val [ 000
h [ i: val [000
a [b: val[000
c [d: val[ 000
e [f: val [ 000
h [i: val [000
)",
N(MB, L{
  N{KP|VP, "a[b", "val[000"}, N{KP|VP, "c[d", "val[ 000"}, N{KP|VP, "e[f", "val [ 000"}, N{KP|VP, "h[i", "val [000"},
  N{KP|VP, "a[ b", "val[000"}, N{KP|VP, "c[ d", "val[ 000"}, N{KP|VP, "e[ f", "val [ 000"}, N{KP|VP, "h[ i", "val [000"},
  N{KP|VP, "a [ b", "val[000"}, N{KP|VP, "c [ d", "val[ 000"}, N{KP|VP, "e [ f", "val [ 000"}, N{KP|VP, "h [ i", "val [000"},
  N{KP|VP, "a [b", "val[000"}, N{KP|VP, "c [d", "val[ 000"}, N{KP|VP, "e [f", "val [ 000"}, N{KP|VP, "h [i", "val [000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, right-bracket",
R"(
a]b: val]000
c]d: val] 000
e]f: val ] 000
h]i: val ]000
a] b: val]000
c] d: val] 000
e] f: val ] 000
h] i: val ]000
a ] b: val]000
c ] d: val] 000
e ] f: val ] 000
h ] i: val ]000
a ]b: val]000
c ]d: val] 000
e ]f: val ] 000
h ]i: val ]000
)",
N(MB, L{
  N{KP|VP, "a]b", "val]000"}, N{KP|VP, "c]d", "val] 000"}, N{KP|VP, "e]f", "val ] 000"}, N{KP|VP, "h]i", "val ]000"},
  N{KP|VP, "a] b", "val]000"}, N{KP|VP, "c] d", "val] 000"}, N{KP|VP, "e] f", "val ] 000"}, N{KP|VP, "h] i", "val ]000"},
  N{KP|VP, "a ] b", "val]000"}, N{KP|VP, "c ] d", "val] 000"}, N{KP|VP, "e ] f", "val ] 000"}, N{KP|VP, "h ] i", "val ]000"},
  N{KP|VP, "a ]b", "val]000"}, N{KP|VP, "c ]d", "val] 000"}, N{KP|VP, "e ]f", "val ] 000"}, N{KP|VP, "h ]i", "val ]000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, left-curly",
R"(
a{b: val{000
c{d: val{ 000
e{f: val { 000
h{i: val {000
a{ b: val{000
c{ d: val{ 000
e{ f: val { 000
h{ i: val {000
a { b: val{000
c { d: val{ 000
e { f: val { 000
h { i: val {000
a {b: val{000
c {d: val{ 000
e {f: val { 000
h {i: val {000
)",
N(MB, L{
  N{KP|VP, "a{b", "val{000"}, N{KP|VP, "c{d", "val{ 000"}, N{KP|VP, "e{f", "val { 000"}, N{KP|VP, "h{i", "val {000"},
  N{KP|VP, "a{ b", "val{000"}, N{KP|VP, "c{ d", "val{ 000"}, N{KP|VP, "e{ f", "val { 000"}, N{KP|VP, "h{ i", "val {000"},
  N{KP|VP, "a { b", "val{000"}, N{KP|VP, "c { d", "val{ 000"}, N{KP|VP, "e { f", "val { 000"}, N{KP|VP, "h { i", "val {000"},
  N{KP|VP, "a {b", "val{000"}, N{KP|VP, "c {d", "val{ 000"}, N{KP|VP, "e {f", "val { 000"}, N{KP|VP, "h {i", "val {000"},
})
);

ADD_CASE_TO_GROUP("simple map, scalars with special chars, right-curly",
R"(
a}b: val}000
c}d: val} 000
e}f: val } 000
h}i: val }000
a} b: val}000
c} d: val} 000
e} f: val } 000
h} i: val }000
a } b: val}000
c } d: val} 000
e } f: val } 000
h } i: val }000
a }b: val}000
c }d: val} 000
e }f: val } 000
h }i: val }000
)",
N(MB, L{
  N{KP|VP, "a}b", "val}000"}, N{KP|VP, "c}d", "val} 000"}, N{KP|VP, "e}f", "val } 000"}, N{KP|VP, "h}i", "val }000"},
  N{KP|VP, "a} b", "val}000"}, N{KP|VP, "c} d", "val} 000"}, N{KP|VP, "e} f", "val } 000"}, N{KP|VP, "h} i", "val }000"},
  N{KP|VP, "a } b", "val}000"}, N{KP|VP, "c } d", "val} 000"}, N{KP|VP, "e } f", "val } 000"}, N{KP|VP, "h } i", "val }000"},
  N{KP|VP, "a }b", "val}000"}, N{KP|VP, "c }d", "val} 000"}, N{KP|VP, "e }f", "val } 000"}, N{KP|VP, "h }i", "val }000"},
})
);

ADD_CASE_TO_GROUP("simple map expl, scalars with special chars, comma",
R"({
a0,b0: val0,0000
c0,d0: val0, 0000
e0,f0: val0 , 0000
h0,i0: val0 ,0000
a1, b1: val1,0001
c1, d1: val1, 0001
e1, f1: val1 , 0001
h1, i1: val1 ,0001
a2 , b2: val2,0002
c2 , d2: val2, 0002
e2 , f2: val2 , 0002
h2 , i2: val2 ,0002
a3 ,b3: val3,0003
c3 ,d3: val3, 0003
e3 ,f3: val3 , 0003
h3 ,i3: val3 ,0003
})",
N(MFS, L{ // this is crazy...
        N(KP|VP, "a0", {}),
        N(KP|VP, "b0", "val0"), N(KP|VP, "0000 c0", {}),
        N(KP|VP, "d0", "val0"), N(KP|VP, "0000 e0", {}),
        N(KP|VP, "f0", "val0"), N(KP|VP, "0000 h0", {}),
        N(KP|VP, "i0", "val0"), N(KP|VP, "0000 a1", {}),
        N(KP|VP, "b1", "val1"), N(KP|VP, "0001 c1", {}),
        N(KP|VP, "d1", "val1"), N(KP|VP, "0001 e1", {}),
        N(KP|VP, "f1", "val1"), N(KP|VP, "0001 h1", {}),
        N(KP|VP, "i1", "val1"), N(KP|VP, "0001 a2", {}),
        N(KP|VP, "b2", "val2"), N(KP|VP, "0002 c2", {}),
        N(KP|VP, "d2", "val2"), N(KP|VP, "0002 e2", {}),
        N(KP|VP, "f2", "val2"), N(KP|VP, "0002 h2", {}),
        N(KP|VP, "i2", "val2"), N(KP|VP, "0002 a3", {}),
        N(KP|VP, "b3", "val3"), N(KP|VP, "0003 c3", {}),
        N(KP|VP, "d3", "val3"), N(KP|VP, "0003 e3", {}),
        N(KP|VP, "f3", "val3"), N(KP|VP, "0003 h3", {}),
        N(KP|VP, "i3", "val3"), N(KP|VP, "0003", {}),
})
);


ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue54",
R"(
foo   : crl
keyA :
    keyA.B : test value
"key C"     : val C
'key D'     : val D
elm2     :
    "key C"     : val C
    'key D'     : val D
    key E       : val E
elm3     :
    'key D'     : val D
    "key C"     : val C
    key E       : val E
elm4     :
    key E       : val E
    'key D'     : val D
    "key C"     : val C
)",
N(MB, L{
    N(KP|VP, "foo", "crl"),
    N(KP|MB, "keyA", L{N(KP|VP, "keyA.B", "test value")}),
    N(KD|VP, "key C", "val C"),
    N(KS|VP, "key D", "val D"),
    N(KP|MB, "elm2", L{N(KD|VP, "key C", "val C"), N(KS|VP, "key D", "val D"), N(KP|VP, "key E", "val E"),}),
    N(KP|MB, "elm3", L{N(KS|VP, "key D", "val D"), N(KD|VP, "key C", "val C"), N(KP|VP, "key E", "val E"),}),
    N(KP|MB, "elm4", L{N(KP|VP, "key E", "val E"), N(KS|VP, "key D", "val D"), N(KD|VP, "key C", "val C"),}),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v0",
R"({a : b})",
N(MFS, L{
    N(KP|VP, "a", "b"),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v1",
"a    \t: b",
N(MB, L{
    N(KP|VP, "a", "b"),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v2",
"a    \t: b",
N(MB, L{
    N(KP|VP, "a", "b"),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v2_0",
"a\t    \t: b",
N(MB, L{
    N(KP|VP, "a", "b"),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v2_1",
"a \t    \t : b",
N(MB, L{
    N(KP|VP, "a", "b"),
})
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v3",
R"({
oka: {a : b},
is it ok      :     let's see,
okb: {a : b},
ok   : {a : b},
must be ok   : {
    c0  : d,
    c1    : d,
    c2       : d,
}
})",
N(MFS, L{
    N(KP|MFS, "oka", L{N(KP|VP, "a", "b")}),
    N(KP|VP, "is it ok", "let's see"),
    N(KP|MFS, "okb", L{N(KP|VP, "a", "b")}),
    N(KP|MFS, "ok", L{N(KP|VP, "a", "b")}),
    N(KP|MFS, "must be ok", L{N(KP|VP, "c0", "d"), N(KP|VP, "c1", "d"), N(KP|VP, "c2", "d")}),
})
);

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v1",
R"(
: a
: b
)",
N(MB, L{
    N(KP|VP, "", "a"),
    N(KP|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v2",
R"(
:
  a
:
  b
)",
N(MB, L{
    N(KP|VP, "", "a"),
    N(KP|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v1",
R"({
: a,
: b,
})",
N(MFS, L{
    N(KP|VP, "", "a"),
    N(KP|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v2",
R"({
:
  a,
:
  b,
})",
N(MFS, L{
    N(KP|VP, "", "a"),
    N(KP|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v3",
R"({-k
:
  a,-z
:
  b,
})",
N(MFS, L{
    N(KP|VP, "-k", "a"),
    N(KP|VP, "-z", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v4",
R"({:k
:
  a,:z
:
  b,
})",
N(MFS, L{
    N(KP|VP, ":k", "a"),
    N(KP|VP, ":z", "b"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v1",
R"({foo
: bar,
baz
: bat
})",
N(MFS, L{
    N(KP|VP, "foo", "bar"),
    N(KP|VP, "baz", "bat"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v2",
R"({foo
:
  bar,
baz
:
  bat
})",
N(MFS, L{
    N(KP|VP, "foo", "bar"),
    N(KP|VP, "baz", "bat"),
}));

// this is not valid YAML: plain scalars can't have ':' as a token
ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v3", EXPECT_PARSE_ERROR,
R"(foo
: bar
baz
: bat
)",
  LineCol(2, 1)
);

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v4", EXPECT_PARSE_ERROR,
R"(foo


:
  bar
baz
:
  bat
)",
  LineCol(4, 1)
);

ADD_CASE_TO_GROUP("json compact",
R"(---
{
"foo0":"bar",
"foo1":bar,
"foo3":{"a":map},
"foo5":[a,seq],
}
--- {"foo0":"bar","foo1":bar,"foo3":{"a":map},"foo5":[a,seq],}
)",
N(STREAM,
  L{
      N(DOC|MFS, L{
              N(KD|VD,"foo0","bar"),
              N(KD|VP,"foo1","bar"),
              N(KD|MFS,"foo3", L{N(KD|VP,"a","map")}),
              N(KD|SFS,"foo5", L{N(VP, "a"),N(VP, "seq")}),
          }),
      N(DOC|MFS, L{
              N(KD|VD,"foo0","bar"),
              N(KD|VP,"foo1","bar"),
              N(KD|MFS,"foo3", L{N(KD|VP,"a","map")}),
              N(KD|SFS,"foo5", L{N(VP, "a"),N(VP, "seq")}),
          }),
    })
);


ADD_CASE_TO_GROUP("issue223 0 fails",
R"(
            A:
                - 1
                - 4
            B:
                - 2
                - 3
 )",
N(MB, L{
  N(KP|SB, "A", L{N(VP, "1"), N(VP, "4")}),
  N(KP|SB, "B", L{N(VP, "2"), N(VP, "3")}),
  })
);

ADD_CASE_TO_GROUP("issue223 1 passes",
R"(A:
  - 1
  - 4
B:
  - 2
  - 3
)",
N(MB, L{
  N(KP|SB, "A", L{N(VP, "1"), N(VP, "4")}),
  N(KP|SB, "B", L{N(VP, "2"), N(VP, "3")}),
  })
);

ADD_CASE_TO_GROUP("issue223 2 passes",
R"(A:
  - 1
  - 4
B:
  - 2
  - 3)",
N(MB, L{
  N(KP|SB, "A", L{N(VP, "1"), N(VP, "4")}),
  N(KP|SB, "B", L{N(VP, "2"), N(VP, "3")}),
  })
);
ADD_CASE_TO_GROUP("issue223 3 fails",
R"(A:
  - 1
  - 4
B:
  - 2
  - 3
 )",
N(MB, L{
  N(KP|SB, "A", L{N(VP, "1"), N(VP, "4")}),
  N(KP|SB, "B", L{N(VP, "2"), N(VP, "3")}),
  })
);


ADD_CASE_TO_GROUP("indentation problem 0",
R"(version: '{build}'
image: Visual Studio 2017
environment:
  matrix:
    - compiler: msvc-15-seh
      generator: "Visual Studio 15 2017"
      configuration: Debug
    - compiler: msvc-15-seh
      generator: "Visual Studio 15 2017 Win64"
      configuration: Debug
)",
N(MB, L{
  N(KP|VS, "version", "{build}"),
  N(KP|VP, "image", "Visual Studio 2017"),
  N(KP|MB, "environment", L{
    N(KP|SB, "matrix", L{
      N(MB, L{
        N(KP|VP, "compiler", "msvc-15-seh"),
        N(KP|VD, "generator", "Visual Studio 15 2017"),
        N(KP|VP, "configuration", "Debug"),
      }),
      N(MB, L{
        N(KP|VP, "compiler", "msvc-15-seh"),
        N(KP|VD, "generator", "Visual Studio 15 2017 Win64"),
        N(KP|VP, "configuration", "Debug"),
      })
    })
  })
})
);

ADD_CASE_TO_GROUP("indentation problem 1",
R"(version: '{build}'

image: Visual Studio 2017

environment:
  matrix:
    - compiler: msvc-15-seh

    - compiler: msvc-15-seh
    - compiler: msvc-15-seh
)",
N(MB, L{
  N(KP|VS, "version", "{build}"),
  N(KP|VP, "image", "Visual Studio 2017"),
  N(KP|MB, "environment", L{
    N(KP|SB, "matrix", L{
      N(MB, L{N(KP|VP, "compiler", "msvc-15-seh"),}),
      N(MB, L{N(KP|VP, "compiler", "msvc-15-seh"),}),
      N(MB, L{N(KP|VP, "compiler", "msvc-15-seh"),}),
    })
  })
})
);

ADD_CASE_TO_GROUP("indentation problem 2",
R"(matrix:

  include:
    #
    #
    - env: CXX_=g++-7 A=64 BT=Coverage
    - env: CXX_=g++-7 A=32 BT=Coverage
    #
    #

    - env: CXX_=g++-7       A=32 BT=Debug   SAN=ALL VG=ON
)",
N(MB, L{
  N(KP|MB, "matrix", L{
    N(KP|SB, "include", L{
      N(MB, L{N(KP|VP, "env", "CXX_=g++-7 A=64 BT=Coverage"),}),
      N(MB, L{N(KP|VP, "env", "CXX_=g++-7 A=32 BT=Coverage"),}),
      N(MB, L{N(KP|VP, "env", "CXX_=g++-7       A=32 BT=Debug   SAN=ALL VG=ON"),}),
    })
  })
})
);

ADD_CASE_TO_GROUP("keys with colon #379, 0",
R"(c: "proxy"
a:p_p:
  c: "clean"
a:d_d:
  c: "proxy"
)",
N(MB, L{
  N(KP|VD, "c", "proxy"),
  N(KP|MB, "a:p_p", L{N(KP|VD, "c", "clean")}),
  N(KP|MB, "a:d_d", L{N(KP|VD, "c", "proxy")}),
})
);

ADD_CASE_TO_GROUP("keys with colon #379, 1",
R"(
a:p_p:
  c: "clean"
c: "proxy"
a:d_d:
  c: "proxy"
)",
N(MB, L{
  N(KP|MB, "a:p_p", L{N(KP|VD, "c", "clean")}),
  N(KP|VD, "c", "proxy"),
  N(KP|MB, "a:d_d", L{N(KP|VD, "c", "proxy")}),
})
);

}

} // namespace yml
} // namespace c4
