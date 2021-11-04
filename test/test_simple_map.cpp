#include "./test_group.hpp"

namespace c4 {
namespace yml {

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define SIMPLE_MAP_CASES                                        \
"empty map",                                                    \
"empty map, multiline",                                         \
"empty map, multilines",                                        \
"simple map, explicit, single line",                            \
"simple map, explicit, multiline, unindented",                  \
"simple map, explicit, multiline, indented",                    \
"simple map",                                                   \
"simple map, values on next line",                              \
"simple map, with comments",                                    \
"simple map, with comments interspersed",                       \
"simple map, with comments interspersed implicit key X8DW",     \
"simple map, with indented comments interspersed, before",      \
"simple map, with indented comments interspersed, after",       \
"simple map, scalars with special chars, comma",                \
"simple map, scalars with special chars, semicolon",            \
"simple map, scalars with special chars, cardinal",             \
"simple map, scalars with special chars, dash",                 \
"simple map, scalars with special chars, left-bracket",         \
"simple map, scalars with special chars, right-bracket",        \
"simple map, scalars with special chars, left-curly",           \
"simple map, scalars with special chars, right-curly",          \
"simple map, null values",                                      \
"simple map expl, null values 1",                               \
"simple map expl, null values 2",                               \
"simple map expl, null values 3, 4ABK",                         \
"simple map expl, scalars with special chars, comma",           \
"simple map, spaces before semicolon, issue54",                 \
"simple map, spaces before semicolon, issue65, v0",             \
"simple map, spaces before semicolon, issue65, v1",             \
"simple map, spaces before semicolon, issue65, v2",             \
"simple map, spaces before semicolon, issue65, v3",             \
"simple map, empty keys 2JQS, v1",                              \
"simple map, empty keys 2JQS, v2",                              \
"simple map, empty keys 4ABK, v1",                              \
"simple map, empty keys 4ABK, v2",                              \
"simple map, values on next line 4MUZ, v1",                     \
"simple map, values on next line 4MUZ, v2"/*                    \
"simple map, values on next line 4MUZ, v3",                     \
"simple map, values on next line 4MUZ, v4"*/


CASE_GROUP(SIMPLE_MAP)
{
    APPEND_CASES(

C("empty map",
"{}",
    MAP
),

C("empty map, multiline",
R"({
}
)",
    MAP
),

C("empty map, multilines",
R"({
# foo bar baz akjasdkj
}
)",
    MAP
),

C("simple map, explicit, single line",
"{foo: 0, bar: 1, baz: 2, bat: 3}",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, unindented",
R"({
foo: 0,
bar: 1,
baz: 2,
bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map",
R"(
foo: 0
bar: 1
baz: 2
bat: 3
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, values on next line",
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
),

C("simple map, with comments",
R"(
foo: 0   # this is a foo
bar: 1   # this is a bar
baz: 2   # this is a baz
bat: 3   # this is a bat
)",
    L{N{"foo", "0"}, N{"bar", "1"}, N{"baz", "2"}, N{"bat", "3"}}
),

C("simple map, with comments interspersed",
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
),

C("simple map, with comments interspersed implicit key X8DW",
R"(
? key
# comment
: value
)",
    L{N("key", "value")}
),

C("simple map, with indented comments interspersed, before",
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
),

C("simple map, with indented comments interspersed, after",
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
),

C("simple map, null values",
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
),

C("simple map expl, null values 1",
R"({key: val, a, b, c, d, e: , f: , g: , foo: bar})",
L{N("key", "val"), N(KEYVAL, "a", /*"~"*/{}), N(KEYVAL, "b", /*"~"*/{}), N(KEYVAL, "c", /*"~"*/{}), N(KEYVAL, "d", /*"~"*/{}), N(KEYVAL, "e", /*"~"*/{}), N(KEYVAL, "f", /*"~"*/{}), N(KEYVAL, "g", /*"~"*/{}), N("foo", "bar"),}
),

C("simple map expl, null values 2",
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
),

C("simple map expl, null values 3, 4ABK",
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
}),

C("simple map, scalars with special chars, comma",
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
),

C("simple map, scalars with special chars, semicolon",
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
),

C("simple map, scalars with special chars, cardinal",
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
),

C("simple map, scalars with special chars, dash",
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
),

C("simple map, scalars with special chars, left-bracket",
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
),

C("simple map, scalars with special chars, right-bracket",
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
),

C("simple map, scalars with special chars, left-curly",
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
),

C("simple map, scalars with special chars, right-curly",
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
),

C("simple map expl, scalars with special chars, comma", IGNORE_YAMLCPP_PARSE_FAIL,
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
),


C("simple map, spaces before semicolon, issue54",
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
),

C("simple map, spaces before semicolon, issue65, v0",
R"({a : b})",
L{
    N("a", "b"),
}
),

C("simple map, spaces before semicolon, issue65, v1",
R"(a : b)",
L{
    N("a", "b"),
}
),

C("simple map, spaces before semicolon, issue65, v2",
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
),

C("simple map, spaces before semicolon, issue65, v3",
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
}),

C("simple map, empty keys 2JQS, v1",
R"(
: a
: b
)",
N(MAP, L{
    N("", "a"),
    N("", "b"),
})),

C("simple map, empty keys 2JQS, v2",
R"(
:
  a
:
  b
)",
N(MAP, L{
    N("", "a"),
    N("", "b"),
})),

C("simple map, empty keys 4ABK, v1",
R"({
: a,
: b,
})",
N(MAP, L{
    N("", "a"),
    N("", "b"),
})),

C("simple map, empty keys 4ABK, v2",
R"({
:
  a,
:
  b,
})",
N(MAP, L{
    N("", "a"),
    N("", "b"),
})),

C("simple map, values on next line 4MUZ, v1",
R"({foo
: bar,
baz
: bat
})",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
})),

C("simple map, values on next line 4MUZ, v2",
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
})),

/* this is not valid YAML: plain scalars can't have ':' as a token
C("simple map, values on next line 4MUZ, v3",
R"(foo
: bar
baz
: bat
)",
N(MAP, L{
    N("foo", "bar"),
    N("baz", "bat"),
})),

C("simple map, values on next line 4MUZ, v4",
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
})),
*/

    )
}

INSTANTIATE_GROUP(SIMPLE_MAP)

} // namespace yml
} // namespace c4
