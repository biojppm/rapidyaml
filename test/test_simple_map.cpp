#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(simple_map, open_on_new_doc_without_space)
{
    Tree tree = parse_in_arena(R"(
foo: bar
---
foo: bar
---
foo: bar
)");
    EXPECT_EQ(tree.docref(0)["foo"].val(), "bar");
    EXPECT_EQ(tree.docref(1)["foo"].val(), "bar");
    EXPECT_EQ(tree.docref(2)["foo"].val(), "bar");
}

TEST(simple_map, open_on_new_doc_with_space_before_colon)
{
    Tree tree = parse_in_arena(R"(
foo0 : bar
---
foo1 : bar  # the " :" was causing an assert
---
foo2 : bar
---
foo3	: bar
---
foo4   	  : bar
)");
    EXPECT_EQ(tree.docref(0)["foo0"].val(), "bar");
    EXPECT_EQ(tree.docref(1)["foo1"].val(), "bar");
    EXPECT_EQ(tree.docref(2)["foo2"].val(), "bar");
    EXPECT_EQ(tree.docref(3)["foo3"].val(), "bar");
    EXPECT_EQ(tree.docref(4)["foo4"].val(), "bar");
}


TEST(simple_map, test_suite_UT92)
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

TEST(simple_map, two_nested_flow_maps_not_accepted_because_of_container_key)
{
    Tree tree;
    ExpectError::do_check(&tree, [&]{
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
        ExpectError::do_check(&tree, [&]{
            parse_in_place(to_substr(src), &tree);
        });
    }
}

TEST(simple_map, missing_quoted_key)
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
'y': z
---
x6:
'y': z
---
'x7' : [
  'y' : z,
 ]
---
"x8" :
  "y" : value,
  "x" : value
"y" :
  "y" : value,
  "x" : value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        size_t doc = 0;
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
        EXPECT_TRUE(t.docref(doc)["y"].is_key_quoted());
        ++doc;
        EXPECT_FALSE(t.docref(doc)["x6"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x7"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x7"][0]["y"].is_key_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["x8"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x8"]["y"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["x8"]["x"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y"]["y"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["y"]["x"].is_key_quoted());
    });
}

#ifdef JAVAI
void verify_error_is_reported(csubstr case_name, csubstr yaml, size_t col={})
{
    SCOPED_TRACE(case_name);
    SCOPED_TRACE(yaml);
    Tree tree;
    Location loc = {};
    loc.col = col;
    ExpectError::do_check(&tree, [&](){
        parse_in_arena(yaml, &tree);
    }, loc);
}

TEST(simple_map, no_map_key_flow)
{
    verify_error_is_reported("map key", R"({ first: Sammy, last: Sosa }: foo)", 28u);
}

TEST(simple_map, no_map_key_block)
{
    verify_error_is_reported("map key", R"(?
  first: Sammy
  last: Sosa
:
  foo
)");
}

TEST(simple_map, no_seq_key_flow)
{
    verify_error_is_reported("seq key", R"([Sammy, Sosa]: foo)", 28u);
}

TEST(simple_map, no_seq_key_block)
{
    verify_error_is_reported("map key", R"(?
  - Sammy
  - Sosa
:
  foo
)");
}
#endif

#ifdef RYML_WITH_TAB_TOKENS
TEST(simple_map, block_tab_tokens)
{
    Tree tree = parse_in_arena(R"(
--- # block, spaces only
a: 0
b: 1
c: 2
--- # block, tabs after token
a:	0
b:	1
c:	2
--- # block, tabs before and after token
a	:	0
b	:	1
c	:	2
--- # block, tabs before token
a	: 0
b	: 1
c	: 2
--- # block, tabs before newline
a	: 0	
b	: 1	
c	: 2	
)");
    EXPECT_EQ(tree.docref(0)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(0)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(0)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(1)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(1)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(1)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(2)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(2)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(2)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(3)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(3)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(3)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(4)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(4)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(4)["c"].val(), csubstr("2"));
}

TEST(simple_map, flow_tab_tokens)
{
    Tree tree = parse_in_arena(R"(
--- # flow, no tabs
{a: 0, b: 1, c: 2}
--- # flow, tabs after token
{a:	0, b:	1, c:	2}
--- # flow, tabs before and after token
{a	:	0, b	:	1, c	:	2}
--- # flow, tabs before token
{a	: 0, b	: 1, c	: 2}
--- # flow, tabs after val
{a	: 0	, b	: 1	, c	: 2	}
--- # flow, tabs after val and comma
{a	:	0	, b	:	1	,	c	:	2	}
--- # flow, tabs everywhere
	{	
	a	:	0	,	
	b	:	1	,	
	c	:	2	
	}	
	)");
    EXPECT_EQ(tree.docref(0)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(0)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(0)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(1)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(1)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(1)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(2)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(2)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(2)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(3)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(3)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(3)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(4)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(4)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(4)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(5)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(5)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(5)["c"].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(6)["a"].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(6)["b"].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(6)["c"].val(), csubstr("2"));
}
#endif // RYML_WITH_TAB_TOKENS


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(SIMPLE_MAP)
{
//
ADD_CASE_TO_GROUP("empty map",
"{}",
    MAP
);

ADD_CASE_TO_GROUP("empty map, multiline",
R"({
}
)",
    MAP
);

ADD_CASE_TO_GROUP("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MAP
);

ADD_CASE_TO_GROUP("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
);

ADD_CASE_TO_GROUP("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
);

ADD_CASE_TO_GROUP("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
);

ADD_CASE_TO_GROUP("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
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
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
);

ADD_CASE_TO_GROUP("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a baz
bat: 3   # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
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
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
);

ADD_CASE_TO_GROUP("simple map, with comments interspersed implicit key X8DW",
R"(
? key
# comment
: value
)",
    L{N("key", "value")}
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
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
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
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
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
L{N("key", "val"), N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{}), N(KEYVAL, "c", /*"~"*/{}), N(KEYVAL, "d", /*"~"*/{}), N(KEYVAL, "e", /*"~"*/{}), N(KEYVAL, "f", /*"~"*/{}), N(KEYVAL, "g", /*"~"*/{}), N("foo", "bar"),}
);

ADD_CASE_TO_GROUP("simple map expl, null values 1",
R"({key: val, a, b, c, d, e: , f: , g: , foo: bar})",
L{N("key", "val"), N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{}), N(KEYVAL, "c", /*"~"*/{}), N(KEYVAL, "d", /*"~"*/{}), N(KEYVAL, "e", /*"~"*/{}), N(KEYVAL, "f", /*"~"*/{}), N(KEYVAL, "g", /*"~"*/{}), N("foo", "bar"),}
);

ADD_CASE_TO_GROUP("simple map expl, null values 2",
R"(
- {a}
- {a, b, c}
- {a: 1, b: 2, c}
- {a: 1, b, c: 2}
- {a, b: 1, c: 2}
)",
L{
   N(L{N(KEYVAL, "a", /*"~"*/{})}),
   N(L{N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{}), N(KEYVAL, "c", /*"~"*/{})}),
   N(L{N("a", "1"), N("b", "2"), N(KEYVAL, "c", /*"~"*/{})}),
   N(L{N("a", "1"), N(KEYVAL, "b", /*"~"*/{}), N("c", "2")}),
   N(L{N(KEYVAL, "a", /*"~"*/{}), N("b", "1"), N("c", "2")}),
 }
);

ADD_CASE_TO_GROUP("simple map expl, null values 3, 4ABK",
R"(
- {foo: , bar: , baz: }
- {foo:, bar:, baz:}
- {foo:foo: , bar:bar: , baz:baz: }
- {foo:foo:, bar:bar:, baz:baz:}
)",
L{
  N(L{N(KEYVAL, "foo", {}), N(KEYVAL, "bar", {}), N(KEYVAL, "baz", {})}),
  N(L{N(KEYVAL, "foo", {}), N(KEYVAL, "bar", {}), N(KEYVAL, "baz", {})}),
  N(L{N(KEYVAL, "foo:foo", {}), N(KEYVAL, "bar:bar", {}), N(KEYVAL, "baz:baz", {})}),
  N(L{N(KEYVAL, "foo:foo", {}), N(KEYVAL, "bar:bar", {}), N(KEYVAL, "baz:baz", {})}),
});

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
    L{
  N{"a,b", "val,000"}, N{"c,d", "val, 000"}, N{"e,f", "val , 000"}, N{"h,i", "val ,000"},
  N{"a, b", "val,000"}, N{"c, d", "val, 000"}, N{"e, f", "val , 000"}, N{"h, i", "val ,000"},
  N{"a , b", "val,000"}, N{"c , d", "val, 000"}, N{"e , f", "val , 000"}, N{"h , i", "val ,000"},
  N{"a ,b", "val,000"}, N{"c ,d", "val, 000"}, N{"e ,f", "val , 000"}, N{"h ,i", "val ,000"},
}
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
    L{
  N{    "a:b", "val:000"},  N{QV,  "c:d", "val: 000"},  N{QV,  "e:f", "val : 000"},  N{    "h:i", "val :000"},
  N{QK, "a: b", "val:000"}, N{QKV, "c: d", "val: 000"}, N{QKV, "e: f", "val : 000"},N{QK, "h: i", "val :000"},
  N{QK, "a : b", "val:000"},N{QKV, "c : d", "val: 000"},N{QKV, "e : f", "val : 000"},N{QK, "h : i", "val :000"},
  N{    "a :b", "val:000"}, N{QV,  "c :d", "val: 000"}, N{QV,  "e :f", "val : 000"}, N{    "h :i", "val :000"},
}
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
    L{
   N{    "a#b",   "val#000"}, N{    "c#d",   "val# 000"}, N{QV,  "e#f",   "val # 000"}, N{QV,  "h#i",   "val #000"},
   N{    "a# b",  "val#000"}, N{    "c# d",  "val# 000"}, N{QV,  "e# f",  "val # 000"}, N{QV,  "h# i",  "val #000"},
   N{QK, "a # b", "val#000"}, N{QK, "c # d", "val# 000"}, N{QKV, "e # f", "val # 000"}, N{QKV, "h # i", "val #000"},
   N{QK, "a #b",  "val#000"}, N{QK, "c #d",  "val# 000"}, N{QKV, "e #f",  "val # 000"}, N{QKV, "h #i",  "val #000"},
}
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
    L{
  N{"a-b", "val-000"}, N{"c-d", "val- 000"}, N{"e-f", "val - 000"}, N{"h-i", "val -000"},
  N{"a- b", "val-000"}, N{"c- d", "val- 000"}, N{"e- f", "val - 000"}, N{"h- i", "val -000"},
  N{"a - b", "val-000"}, N{"c - d", "val- 000"}, N{"e - f", "val - 000"}, N{"h - i", "val -000"},
  N{"a -b", "val-000"}, N{"c -d", "val- 000"}, N{"e -f", "val - 000"}, N{"h -i", "val -000"},
}
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
    L{
  N{"a[b", "val[000"}, N{"c[d", "val[ 000"}, N{"e[f", "val [ 000"}, N{"h[i", "val [000"},
  N{"a[ b", "val[000"}, N{"c[ d", "val[ 000"}, N{"e[ f", "val [ 000"}, N{"h[ i", "val [000"},
  N{"a [ b", "val[000"}, N{"c [ d", "val[ 000"}, N{"e [ f", "val [ 000"}, N{"h [ i", "val [000"},
  N{"a [b", "val[000"}, N{"c [d", "val[ 000"}, N{"e [f", "val [ 000"}, N{"h [i", "val [000"},
}
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
    L{
  N{"a]b", "val]000"}, N{"c]d", "val] 000"}, N{"e]f", "val ] 000"}, N{"h]i", "val ]000"},
  N{"a] b", "val]000"}, N{"c] d", "val] 000"}, N{"e] f", "val ] 000"}, N{"h] i", "val ]000"},
  N{"a ] b", "val]000"}, N{"c ] d", "val] 000"}, N{"e ] f", "val ] 000"}, N{"h ] i", "val ]000"},
  N{"a ]b", "val]000"}, N{"c ]d", "val] 000"}, N{"e ]f", "val ] 000"}, N{"h ]i", "val ]000"},
}
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
    L{
  N{"a{b", "val{000"}, N{"c{d", "val{ 000"}, N{"e{f", "val { 000"}, N{"h{i", "val {000"},
  N{"a{ b", "val{000"}, N{"c{ d", "val{ 000"}, N{"e{ f", "val { 000"}, N{"h{ i", "val {000"},
  N{"a { b", "val{000"}, N{"c { d", "val{ 000"}, N{"e { f", "val { 000"}, N{"h { i", "val {000"},
  N{"a {b", "val{000"}, N{"c {d", "val{ 000"}, N{"e {f", "val { 000"}, N{"h {i", "val {000"},
}
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
    L{
  N{"a}b", "val}000"}, N{"c}d", "val} 000"}, N{"e}f", "val } 000"}, N{"h}i", "val }000"},
  N{"a} b", "val}000"}, N{"c} d", "val} 000"}, N{"e} f", "val } 000"}, N{"h} i", "val }000"},
  N{"a } b", "val}000"}, N{"c } d", "val} 000"}, N{"e } f", "val } 000"}, N{"h } i", "val }000"},
  N{"a }b", "val}000"}, N{"c }d", "val} 000"}, N{"e }f", "val } 000"}, N{"h }i", "val }000"},
}
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
    L{ // this is crazy...
        N(KEYVAL, "a0", /*"~"*/{}),
        N("b0", "val0"),
        N(KEYVAL, "0000 c0", /*"~"*/{}),
        N("d0", "val0"), N(KEYVAL, "0000 e0", /*"~"*/{}),
        N("f0", "val0"), N(KEYVAL, "0000 h0", /*"~"*/{}),
        N("i0", "val0"), N(KEYVAL, "0000 a1", /*"~"*/{}),
        N("b1", "val1"), N(KEYVAL, "0001 c1", /*"~"*/{}),
        N("d1", "val1"), N(KEYVAL, "0001 e1", /*"~"*/{}),
        N("f1", "val1"), N(KEYVAL, "0001 h1", /*"~"*/{}),
        N("i1", "val1"), N(KEYVAL, "0001 a2", /*"~"*/{}),
        N("b2", "val2"), N(KEYVAL, "0002 c2", /*"~"*/{}),
        N("d2", "val2"), N(KEYVAL, "0002 e2", /*"~"*/{}),
        N("f2", "val2"), N(KEYVAL, "0002 h2", /*"~"*/{}),
        N("i2", "val2"), N(KEYVAL, "0002 a3", /*"~"*/{}),
        N("b3", "val3"), N(KEYVAL, "0003 c3", /*"~"*/{}),
        N("d3", "val3"), N(KEYVAL, "0003 e3", /*"~"*/{}),
        N("f3", "val3"), N(KEYVAL, "0003 h3", /*"~"*/{}),
        N("i3", "val3"), N(KEYVAL, "0003", /*"~"*/{}),
}
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
L{
    N("foo", "crl"),
    N("keyA", L{N("keyA.B", "test value")}),
    N(QK, "key C", "val C"),
    N(QK, "key D", "val D"),
    N("elm2", L{N(QK, "key C", "val C"), N(QK, "key D", "val D"), N("key E", "val E"),}),
    N("elm3", L{N(QK, "key D", "val D"), N(QK, "key C", "val C"), N("key E", "val E"),}),
    N("elm4", L{N("key E", "val E"), N(QK, "key D", "val D"), N(QK, "key C", "val C"),}),
}
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v0",
R"({a : b})",
L{
    N("a", "b"),
}
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v1",
R"(a : b)",
L{
    N("a", "b"),
}
);

ADD_CASE_TO_GROUP("simple map, spaces before semicolon, issue65, v2",
R"(
is it ok      :     let's see
ok     : {a : b, c     : d,      e      : f,}
must be ok   :
    c0  : d
    c1    : d
    c2       : d
)",
L{
    N("is it ok", "let's see"),
    N("ok", L{N("a", "b"), N("c", "d"), N("e", "f")}),
    N("must be ok", L{N("c0", "d"), N("c1", "d"), N("c2", "d")}),
}
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
L{
    N("oka", L{N("a", "b")}),
    N("is it ok", "let's see"),
    N("okb", L{N("a", "b")}),
    N("ok", L{N("a", "b")}),
    N("must be ok", L{N("c0", "d"), N("c1", "d"), N("c2", "d")}),
});

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v1",
R"(
: a
: b
)",
N(MAP, L{
    N("", "a"),
    N("", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v2",
R"(
:
  a
:
  b
)",
N(MAP, L{
    N("", "a"),
    N("", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v1",
R"({
: a,
: b,
})",
N(MAP, L{
    N("", "a"),
    N("", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v2",
R"({
:
  a,
:
  b,
})",
N(MAP, L{
    N("", "a"),
    N("", "b"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v1",
R"({foo
: bar,
baz
: bat
})",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v2",
R"({foo
:
  bar,
baz
:
  bat
})",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
}));

/* this is not valid YAML: plain scalars can't have ':' as a token
ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v3",
R"(foo
: bar
baz
: bat
)",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v4",
R"(foo
:
  bar
baz
:
  bat
)",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
}));
*/

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
      N(DOCMAP, L{
              N(KEYVAL|KEYQUO|VALQUO,"foo0","bar"),
              N(KEYVAL|KEYQUO,"foo1","bar"),
              N(KEYMAP|KEYQUO,"foo3", L{N(KEYVAL|KEYQUO,"a","map")}),
              N(KEYSEQ|KEYQUO,"foo5", L{N("a"),N("seq")}),
          }),
      N(DOCMAP, L{
              N(KEYVAL|KEYQUO|VALQUO,"foo0","bar"),
              N(KEYVAL|KEYQUO,"foo1","bar"),
              N(KEYMAP|KEYQUO,"foo3", L{N(KEYVAL|KEYQUO,"a","map")}),
              N(KEYSEQ|KEYQUO,"foo5", L{N("a"),N("seq")}),
          }),
    })
);
}

} // namespace yml
} // namespace c4
