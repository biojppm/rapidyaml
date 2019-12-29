#include "./test_group.hpp"

namespace c4 {
namespace yml {

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
"simple map expl, null values",                                 \
"simple map expl, scalars with special chars, comma"            \

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
L{N("key", "val"), N("a", "~"), N("b", "~"), N("c", "~"), N("d", "~"), N("e", "~"), N("f", "~"), N("g", "~"), N("foo", "bar"),}
),

C("simple map expl, null values",
R"({key: val, a, b, c, d, e: , f: , g: , foo: bar})",
L{N("key", "val"), N("a", "~"), N("b", "~"), N("c", "~"), N("d", "~"), N("e", "~"), N("f", "~"), N("g", "~"), N("foo", "bar"),}
),

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
  N{"a:b", "val:000"}, N{"c:d", "val: 000"}, N{"e:f", "val : 000"}, N{"h:i", "val :000"},
  N{"a: b", "val:000"}, N{"c: d", "val: 000"}, N{"e: f", "val : 000"}, N{"h: i", "val :000"},
  N{"a : b", "val:000"}, N{"c : d", "val: 000"}, N{"e : f", "val : 000"}, N{"h : i", "val :000"},
  N{"a :b", "val:000"}, N{"c :d", "val: 000"}, N{"e :f", "val : 000"}, N{"h :i", "val :000"},
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
  N{"a#b", "val#000"}, N{"c#d", "val# 000"}, N{"e#f", "val # 000"}, N{"h#i", "val #000"},
  N{"a# b", "val#000"}, N{"c# d", "val# 000"}, N{"e# f", "val # 000"}, N{"h# i", "val #000"},
  N{"a # b", "val#000"}, N{"c # d", "val# 000"}, N{"e # f", "val # 000"}, N{"h # i", "val #000"},
  N{"a #b", "val#000"}, N{"c #d", "val# 000"}, N{"e #f", "val # 000"}, N{"h #i", "val #000"},
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
})",
    L{ // this is crazy...
        N{"a", "~"}, N{"b", "val"}, N{"000 c", "~"},
                     N{"d", "val"}, N{"000 e", "~"},
                     N{"f", "val"}, N{"000 h", "~"},
                     N{"i", "val"}, N{"000 a", "~"},
                     N{"b", "val"}, N{"000 c", "~"},
                     N{"d", "val"}, N{"000 e", "~"},
                     N{"f", "val"}, N{"000 h", "~"},
                     N{"i", "val"}, N{"000 a", "~"},
                     N{"b", "val"}, N{"000 c", "~"},
                     N{"d", "val"}, N{"000 e", "~"},
                     N{"f", "val"}, N{"000 h", "~"},
                     N{"i", "val"}, N{"000 a", "~"},
                     N{"b", "val"}, N{"000 c", "~"},
                     N{"d", "val"}, N{"000 e", "~"},
                     N{"f", "val"}, N{"000 h", "~"},
                     N{"i", "val"}, N{"000", "~"},
}
),

    )
}

INSTANTIATE_GROUP(SIMPLE_MAP)

} // namespace yml
} // namespace c4
