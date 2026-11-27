
#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(SIMPLE_MAP)
{

ADD_CASE_TO_GROUP("empty map, explicit",
"{}",
    MFS
);


ADD_CASE_TO_GROUP("empty map, explicit, whitespace",
" {}",
    MFS
);


ADD_CASE_TO_GROUP("empty map, multiline 1",
R"({

}
)",
    MFM
);

ADD_CASE_TO_GROUP("empty map, multiline 2",
R"({
}
)",
    MFM
);


ADD_CASE_TO_GROUP("empty map, multilines",
R"({
# ksjdfkjhsdfkjhsdfkjh


}
)",
    MFM
    );

ADD_CASE_TO_GROUP("empty map, multilines 1",
R"({
# foo bar baz akjasdkj
}
)",
    MFM
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
N(MFM, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
);

ADD_CASE_TO_GROUP("simple map, explicit, multiline, indented",
R"({
  foo: 0,
  bar: 1,
  baz: 2,
  bat: 3
})",
N(MFM, L{N{KP|VP, "foo", "0"}, N{KP|VP, "bar", "1"}, N{KP|VP, "baz", "2"}, N{KP|VP, "bat", "3"}})
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

ADD_CASE_TO_GROUP("simple map, null values", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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
N(MB, L{N(KP|VP, "key", "val"), N(KP|VN, "a", {}), N(KP|VN, "b", {}), N(KP|VN, "c", {}), N(KP|VN, "d", {}), N(KP|VN, "e", {}), N(KP|VN, "f", {}), N(KP|VN, "g", {}), N(KP|VP, "foo", "bar"),})
);

ADD_CASE_TO_GROUP("simple map expl, null values 1", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"({key: val, a, b, c, d, e: , f: , g: , foo: bar})",
N(MFS, L{N(KP|VP, "key", "val"), N(KP|VN, "a", {}), N(KP|VN, "b", {}), N(KP|VN, "c", {}), N(KP|VN, "d", {}), N(KP|VN, "e", {}), N(KP|VN, "f", {}), N(KP|VN, "g", {}), N(KP|VP, "foo", "bar"),})
);

ADD_CASE_TO_GROUP("simple map expl, null values 2", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- {a}
- {a, b, c}
- {a: 1, b: 2, c}
- {a: 1, b, c: 2}
- {a, b: 1, c: 2}
)",
N(SB, L{
   N(MFS, L{N(KP|VN, "a", {})}),
   N(MFS, L{N(KP|VN, "a", {}), N(KP|VN, "b", {}), N(KP|VN, "c", {})}),
   N(MFS, L{N(KP|VP, "a", "1"), N(KP|VP, "b", "2"), N(KP|VN, "c", {})}),
   N(MFS, L{N(KP|VP, "a", "1"), N(KP|VN, "b", {}), N(KP|VP, "c", "2")}),
   N(MFS, L{N(KP|VN, "a", {}), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
 })
);

ADD_CASE_TO_GROUP("simple map expl, null values 3, 4ABK", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- {foo: , bar: , baz: }
- {foo:, bar:, baz:}
- {foo:foo: , bar:bar: , baz:baz: }
- {foo:foo:, bar:bar:, baz:baz:}
- {:foo:foo: , :bar:bar: , :baz:baz: }
- {:foo:foo:, :bar:bar:, :baz:baz:}
)",
N(SB, L{
  N(MFS, L{N(KP|VN, "foo", {}), N(KP|VN, "bar", {}), N(KP|VN, "baz", {})}),
  N(MFS, L{N(KP|VN, "foo", {}), N(KP|VN, "bar", {}), N(KP|VN, "baz", {})}),
  N(MFS, L{N(KP|VN, "foo:foo", {}), N(KP|VN, "bar:bar", {}), N(KP|VN, "baz:baz", {})}),
  N(MFS, L{N(KP|VN, "foo:foo", {}), N(KP|VN, "bar:bar", {}), N(KP|VN, "baz:baz", {})}),
  N(MFS, L{N(KP|VN, ":foo:foo", {}), N(KP|VN, ":bar:bar", {}), N(KP|VN, ":baz:baz", {})}),
  N(MFS, L{N(KP|VN, ":foo:foo", {}), N(KP|VN, ":bar:bar", {}), N(KP|VN, ":baz:baz", {})}),
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

ADD_CASE_TO_GROUP("simple map expl, scalars with special chars, comma", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
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
N(MFM, L{ // this is crazy...
        N(KP|VN, "a0", {}),
        N(KP|VP, "b0", "val0"), N(KP|VN, "0000 c0", {}),
        N(KP|VP, "d0", "val0"), N(KP|VN, "0000 e0", {}),
        N(KP|VP, "f0", "val0"), N(KP|VN, "0000 h0", {}),
        N(KP|VP, "i0", "val0"), N(KP|VN, "0000 a1", {}),
        N(KP|VP, "b1", "val1"), N(KP|VN, "0001 c1", {}),
        N(KP|VP, "d1", "val1"), N(KP|VN, "0001 e1", {}),
        N(KP|VP, "f1", "val1"), N(KP|VN, "0001 h1", {}),
        N(KP|VP, "i1", "val1"), N(KP|VN, "0001 a2", {}),
        N(KP|VP, "b2", "val2"), N(KP|VN, "0002 c2", {}),
        N(KP|VP, "d2", "val2"), N(KP|VN, "0002 e2", {}),
        N(KP|VP, "f2", "val2"), N(KP|VN, "0002 h2", {}),
        N(KP|VP, "i2", "val2"), N(KP|VN, "0002 a3", {}),
        N(KP|VP, "b3", "val3"), N(KP|VN, "0003 c3", {}),
        N(KP|VP, "d3", "val3"), N(KP|VN, "0003 e3", {}),
        N(KP|VP, "f3", "val3"), N(KP|VN, "0003 h3", {}),
        N(KP|VP, "i3", "val3"), N(KP|VN, "0003", {}),
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
N(MFM, L{
    N(KP|MFS, "oka", L{N(KP|VP, "a", "b")}),
    N(KP|VP, "is it ok", "let's see"),
    N(KP|MFS, "okb", L{N(KP|VP, "a", "b")}),
    N(KP|MFS, "ok", L{N(KP|VP, "a", "b")}),
    N(KP|MFM, "must be ok", L{N(KP|VP, "c0", "d"), N(KP|VP, "c1", "d"), N(KP|VP, "c2", "d")}),
})
);

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v1",
R"(
: a
: b
)",
N(MB, L{
    N(KN|VP, "", "a"),
    N(KN|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 2JQS, v2",
R"(
:
  a
:
  b
)",
N(MB, L{
    N(KN|VP, "", "a"),
    N(KN|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v1",
R"({
: a,
: b,
})",
N(MFM, L{
    N(KN|VP, "", "a"),
    N(KN|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v2",
R"({
:
  a,
:
  b,
})",
N(MFM, L{
    N(KN|VP, "", "a"),
    N(KN|VP, "", "b"),
}));

ADD_CASE_TO_GROUP("simple map, empty keys 4ABK, v3",
R"({-k
:
  a,-z
:
  b,
})",
N(MFM, L{
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
N(MFM, L{
    N(KP|VP, ":k", "a"),
    N(KP|VP, ":z", "b"),
}));

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v1",
R"({foo
: bar,
baz
: bat
})",
N(MFM, L{
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
N(MFM, L{
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
  Location(2, 1)
);

ADD_CASE_TO_GROUP("simple map, values on next line 4MUZ, v4", EXPECT_PARSE_ERROR,
R"(foo


:
  bar
baz
:
  bat
)",
  Location(4, 1)
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
      N(DOC|MFM, L{
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

ADD_CASE_TO_GROUP("generic map",
R"(
a simple key: a value   # The KEY token is produced here.
? a complex key
: another value
a mapping:
  key 1: value 1
  key 2: value 2
a sequence:
  - item 1
  - item 2
)",
N(MB, L{
      N(KP|VP, "a simple key", "a value"),
      N(KP|VP, "a complex key", "another value"),
      N(KP|MB, "a mapping", L{N(KP|VP, "key 1", "value 1"), N(KP|VP, "key 2", "value 2")}),
      N(KP|SB, "a sequence", L{N(VP, "item 1"), N(VP, "item 2")}),
  })
);


ADD_CASE_TO_GROUP("seq nested in map",
R"(
items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
)",
N(MB, L{
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},}},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},}}}},
  })
);

ADD_CASE_TO_GROUP("seq nested in map, v2",
R"(
items:
    -
      part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4
    -
      part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     133.7
      quantity:  1
)",
N(MB, L{
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},}},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},}}}},
  })
);

ADD_CASE_TO_GROUP("nested map x2, explicit, same line",
R"({foo: {foo0: 00, bar0: 01, baz0: 02}, bar: {foo1: 10, bar1: 11, baz1: 12}, baz: {foo2: 20, bar2: 21, baz2: 22}})",
N(MFS,L{
      N{KP|MFS, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MFS, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MFS, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
);

ADD_CASE_TO_GROUP("nested map x2, explicit",
R"({
foo: {foo0: 00, bar0: 01, baz0: 02},
bar: {foo1: 10, bar1: 11, baz1: 12},
baz: {foo2: 20, bar2: 21, baz2: 22}
})",
N(MFM, L{
      N{KP|MFS, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MFS, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MFS, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
);

ADD_CASE_TO_GROUP("nested map x2",
R"(
foo:
  foo0: 00
  bar0: 01
  baz0: 02
bar:
  foo1: 10
  bar1: 11
  baz1: 12
baz:
  foo2: 20
  bar2: 21
  baz2: 22
)",
N(MB, L{
      N{KP|MB, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MB, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MB, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
);


ADD_CASE_TO_GROUP("nested map x2, commented",
	R"(
send_to:
  #host: 192.168.1.100
  #port: 7000
  host: 192.168.1.101
  port: 7001
  #host: 192.168.1.102
  #port: 7002
)",
N(MB, L{
	  N(KP|MB, "send_to", L{
	    N(KP|VP, "host", "192.168.1.101"),
        N(KP|VP, "port", "7001") })
	})
);


ADD_CASE_TO_GROUP("nested map x3, explicit",
R"({
  foo0: {
     foo1: {foo2: 000, bar2: 001, baz2: 002},
     bar1: {foo2: 010, bar2: 011, baz2: 012},
     baz1: {foo2: 020, bar2: 021, baz2: 022}
  },
  bar0: {
     foo1: {foo2: 100, bar2: 101, baz2: 102},
     bar1: {foo2: 110, bar2: 111, baz2: 112},
     baz1: {foo2: 120, bar2: 121, baz2: 122}
  },
  baz0: {
     foo1: {foo2: 200, bar2: 201, baz2: 202},
     bar1: {foo2: 210, bar2: 211, baz2: 212},
     baz1: {foo2: 220, bar2: 221, baz2: 222}
  }
})",
N(MFM, L{
      N{KP|MFM, "foo0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "000"}, N{KP|VP, "bar2", "001"}, N{KP|VP, "baz2", "002"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "010"}, N{KP|VP, "bar2", "011"}, N{KP|VP, "baz2", "012"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "020"}, N{KP|VP, "bar2", "021"}, N{KP|VP, "baz2", "022"}}} }},
      N{KP|MFM, "bar0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "100"}, N{KP|VP, "bar2", "101"}, N{KP|VP, "baz2", "102"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "110"}, N{KP|VP, "bar2", "111"}, N{KP|VP, "baz2", "112"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "120"}, N{KP|VP, "bar2", "121"}, N{KP|VP, "baz2", "122"}}} }},
      N{KP|MFM, "baz0", L{
         N{KP|MFS, "foo1", L{N{KP|VP, "foo2", "200"}, N{KP|VP, "bar2", "201"}, N{KP|VP, "baz2", "202"}}},
         N{KP|MFS, "bar1", L{N{KP|VP, "foo2", "210"}, N{KP|VP, "bar2", "211"}, N{KP|VP, "baz2", "212"}}},
         N{KP|MFS, "baz1", L{N{KP|VP, "foo2", "220"}, N{KP|VP, "bar2", "221"}, N{KP|VP, "baz2", "222"}}} }},
          })
);

ADD_CASE_TO_GROUP("nested map x3",
R"(
foo0:
  foo1:
    foo2: 000
    bar2: 001
    baz2: 002
  bar1:
    foo2: 010
    bar2: 011
    baz2: 012
  baz1:
    foo2: 020
    bar2: 021
    baz2: 022
bar0:
  foo1:
    foo2: 100
    bar2: 101
    baz2: 102
  bar1:
    foo2: 110
    bar2: 111
    baz2: 112
  baz1:
    foo2: 120
    bar2: 121
    baz2: 122
baz0:
  foo1:
    foo2: 200
    bar2: 201
    baz2: 202
  bar1:
    foo2: 210
    bar2: 211
    baz2: 212
  baz1:
    foo2: 220
    bar2: 221
    baz2: 222
)",
N(MB, L{
      N{KP|MB, "foo0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "000"}, N{KP|VP, "bar2", "001"}, N{KP|VP, "baz2", "002"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "010"}, N{KP|VP, "bar2", "011"}, N{KP|VP, "baz2", "012"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "020"}, N{KP|VP, "bar2", "021"}, N{KP|VP, "baz2", "022"}}} }},
      N{KP|MB, "bar0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "100"}, N{KP|VP, "bar2", "101"}, N{KP|VP, "baz2", "102"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "110"}, N{KP|VP, "bar2", "111"}, N{KP|VP, "baz2", "112"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "120"}, N{KP|VP, "bar2", "121"}, N{KP|VP, "baz2", "122"}}} }},
      N{KP|MB, "baz0", L{
         N{KP|MB, "foo1", L{N{KP|VP, "foo2", "200"}, N{KP|VP, "bar2", "201"}, N{KP|VP, "baz2", "202"}}},
         N{KP|MB, "bar1", L{N{KP|VP, "foo2", "210"}, N{KP|VP, "bar2", "211"}, N{KP|VP, "baz2", "212"}}},
         N{KP|MB, "baz1", L{N{KP|VP, "foo2", "220"}, N{KP|VP, "bar2", "221"}, N{KP|VP, "baz2", "222"}}} }},
      })
);

ADD_CASE_TO_GROUP("nested map x4, explicit",
R"({
  foo0: {
     foo1: { foo2: {foo3: 0000, bar3: 0001, baz3: 0002}, bar2: {foo3: 0010, bar3: 0011, baz3: 0012}, baz2: {foo3: 0020, bar3: 0021, baz3: 0022} },
     bar1: { foo2: {foo3: 0100, bar3: 0101, baz3: 0102}, bar2: {foo3: 0110, bar3: 0111, baz3: 0112}, baz2: {foo3: 0120, bar3: 0121, baz3: 0122} },
     baz1: { foo2: {foo3: 0200, bar3: 0201, baz3: 0202}, bar2: {foo3: 0210, bar3: 0211, baz3: 0212}, baz2: {foo3: 0220, bar3: 0221, baz3: 0222} },
  },
  bar0: {
     foo1: { foo2: {foo3: 1000, bar3: 1001, baz3: 1002}, bar2: {foo3: 1010, bar3: 1011, baz3: 1012}, baz2: {foo3: 1020, bar3: 1021, baz3: 1022} },
     bar1: { foo2: {foo3: 1100, bar3: 1101, baz3: 1102}, bar2: {foo3: 1110, bar3: 1111, baz3: 1112}, baz2: {foo3: 1120, bar3: 1121, baz3: 1122} },
     baz1: { foo2: {foo3: 1200, bar3: 1201, baz3: 1202}, bar2: {foo3: 1210, bar3: 1211, baz3: 1212}, baz2: {foo3: 1220, bar3: 1221, baz3: 1222} },
  },
  baz0: {
     foo1: { foo2: {foo3: 2000, bar3: 2001, baz3: 2002}, bar2: {foo3: 2010, bar3: 2011, baz3: 2012}, baz2: {foo3: 2020, bar3: 2021, baz3: 2022} },
     bar1: { foo2: {foo3: 2100, bar3: 2101, baz3: 2102}, bar2: {foo3: 2110, bar3: 2111, baz3: 2112}, baz2: {foo3: 2120, bar3: 2121, baz3: 2122} },
     baz1: { foo2: {foo3: 2200, bar3: 2201, baz3: 2202}, bar2: {foo3: 2210, bar3: 2211, baz3: 2212}, baz2: {foo3: 2220, bar3: 2221, baz3: 2222} },
  },
})",
N(MFM, L{
      N(KP|MFM, "foo0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0000"), N(KP|VP, "bar3", "0001"), N(KP|VP, "baz3", "0002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0010"), N(KP|VP, "bar3", "0011"), N(KP|VP, "baz3", "0012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0020"), N(KP|VP, "bar3", "0021"), N(KP|VP, "baz3", "0022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0100"), N(KP|VP, "bar3", "0101"), N(KP|VP, "baz3", "0102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0110"), N(KP|VP, "bar3", "0111"), N(KP|VP, "baz3", "0112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0120"), N(KP|VP, "bar3", "0121"), N(KP|VP, "baz3", "0122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "0200"), N(KP|VP, "bar3", "0201"), N(KP|VP, "baz3", "0202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "0210"), N(KP|VP, "bar3", "0211"), N(KP|VP, "baz3", "0212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "0220"), N(KP|VP, "bar3", "0221"), N(KP|VP, "baz3", "0222")})}),
      }),
      N(KP|MFM, "bar0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1000"), N(KP|VP, "bar3", "1001"), N(KP|VP, "baz3", "1002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1010"), N(KP|VP, "bar3", "1011"), N(KP|VP, "baz3", "1012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1020"), N(KP|VP, "bar3", "1021"), N(KP|VP, "baz3", "1022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1100"), N(KP|VP, "bar3", "1101"), N(KP|VP, "baz3", "1102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1110"), N(KP|VP, "bar3", "1111"), N(KP|VP, "baz3", "1112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1120"), N(KP|VP, "bar3", "1121"), N(KP|VP, "baz3", "1122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "1200"), N(KP|VP, "bar3", "1201"), N(KP|VP, "baz3", "1202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "1210"), N(KP|VP, "bar3", "1211"), N(KP|VP, "baz3", "1212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "1220"), N(KP|VP, "bar3", "1221"), N(KP|VP, "baz3", "1222")})}),
      }),
      N(KP|MFM, "baz0", L{
         N(KP|MFS, "foo1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2000"), N(KP|VP, "bar3", "2001"), N(KP|VP, "baz3", "2002")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2010"), N(KP|VP, "bar3", "2011"), N(KP|VP, "baz3", "2012")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2020"), N(KP|VP, "bar3", "2021"), N(KP|VP, "baz3", "2022")})}),
         N(KP|MFS, "bar1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2100"), N(KP|VP, "bar3", "2101"), N(KP|VP, "baz3", "2102")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2110"), N(KP|VP, "bar3", "2111"), N(KP|VP, "baz3", "2112")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2120"), N(KP|VP, "bar3", "2121"), N(KP|VP, "baz3", "2122")})}),
         N(KP|MFS, "baz1", L{N(KP|MFS, "foo2", L{N(KP|VP, "foo3", "2200"), N(KP|VP, "bar3", "2201"), N(KP|VP, "baz3", "2202")}),  N(KP|MFS, "bar2", L{N(KP|VP, "foo3", "2210"), N(KP|VP, "bar3", "2211"), N(KP|VP, "baz3", "2212")}),  N(KP|MFS, "baz2", L{N(KP|VP, "foo3", "2220"), N(KP|VP, "bar3", "2221"), N(KP|VP, "baz3", "2222")})}),
      })
    })
);

ADD_CASE_TO_GROUP("nested map x4",
R"(
foo0:
  foo1:
    foo2:
      foo3: 0000
      bar3: 0001
      baz3: 0002
    bar2:
      foo3: 0010
      bar3: 0011
      baz3: 0012
    baz2:
      foo3: 0020
      bar3: 0021
      baz3: 0022
  bar1:
    foo2:
      foo3: 0100
      bar3: 0101
      baz3: 0102
    bar2:
      foo3: 0110
      bar3: 0111
      baz3: 0112
    baz2:
      foo3: 0120
      bar3: 0121
      baz3: 0122
  baz1:
    foo2:
      foo3: 0200
      bar3: 0201
      baz3: 0202
    bar2:
      foo3: 0210
      bar3: 0211
      baz3: 0212
    baz2:
      foo3: 0220
      bar3: 0221
      baz3: 0222
bar0:
  foo1:
    foo2:
      foo3: 1000
      bar3: 1001
      baz3: 1002
    bar2:
      foo3: 1010
      bar3: 1011
      baz3: 1012
    baz2:
      foo3: 1020
      bar3: 1021
      baz3: 1022
  bar1:
    foo2:
      foo3: 1100
      bar3: 1101
      baz3: 1102
    bar2:
      foo3: 1110
      bar3: 1111
      baz3: 1112
    baz2:
      foo3: 1120
      bar3: 1121
      baz3: 1122
  baz1:
    foo2:
      foo3: 1200
      bar3: 1201
      baz3: 1202
    bar2:
      foo3: 1210
      bar3: 1211
      baz3: 1212
    baz2:
      foo3: 1220
      bar3: 1221
      baz3: 1222
baz0:
  foo1:
    foo2:
      foo3: 2000
      bar3: 2001
      baz3: 2002
    bar2:
      foo3: 2010
      bar3: 2011
      baz3: 2012
    baz2:
      foo3: 2020
      bar3: 2021
      baz3: 2022
  bar1:
    foo2:
      foo3: 2100
      bar3: 2101
      baz3: 2102
    bar2:
      foo3: 2110
      bar3: 2111
      baz3: 2112
    baz2:
      foo3: 2120
      bar3: 2121
      baz3: 2122
  baz1:
    foo2:
      foo3: 2200
      bar3: 2201
      baz3: 2202
    bar2:
      foo3: 2210
      bar3: 2211
      baz3: 2212
    baz2:
      foo3: 2220
      bar3: 2221
      baz3: 2222
)",
N(MB, L{
      N(KP|MB, "foo0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0000"), N(KP|VP, "bar3", "0001"), N(KP|VP, "baz3", "0002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0010"), N(KP|VP, "bar3", "0011"), N(KP|VP, "baz3", "0012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0020"), N(KP|VP, "bar3", "0021"), N(KP|VP, "baz3", "0022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0100"), N(KP|VP, "bar3", "0101"), N(KP|VP, "baz3", "0102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0110"), N(KP|VP, "bar3", "0111"), N(KP|VP, "baz3", "0112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0120"), N(KP|VP, "bar3", "0121"), N(KP|VP, "baz3", "0122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "0200"), N(KP|VP, "bar3", "0201"), N(KP|VP, "baz3", "0202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "0210"), N(KP|VP, "bar3", "0211"), N(KP|VP, "baz3", "0212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "0220"), N(KP|VP, "bar3", "0221"), N(KP|VP, "baz3", "0222")})}),
      }),
      N(KP|MB, "bar0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1000"), N(KP|VP, "bar3", "1001"), N(KP|VP, "baz3", "1002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1010"), N(KP|VP, "bar3", "1011"), N(KP|VP, "baz3", "1012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1020"), N(KP|VP, "bar3", "1021"), N(KP|VP, "baz3", "1022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1100"), N(KP|VP, "bar3", "1101"), N(KP|VP, "baz3", "1102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1110"), N(KP|VP, "bar3", "1111"), N(KP|VP, "baz3", "1112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1120"), N(KP|VP, "bar3", "1121"), N(KP|VP, "baz3", "1122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "1200"), N(KP|VP, "bar3", "1201"), N(KP|VP, "baz3", "1202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "1210"), N(KP|VP, "bar3", "1211"), N(KP|VP, "baz3", "1212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "1220"), N(KP|VP, "bar3", "1221"), N(KP|VP, "baz3", "1222")})}),
      }),
      N(KP|MB, "baz0", L{
         N(KP|MB, "foo1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2000"), N(KP|VP, "bar3", "2001"), N(KP|VP, "baz3", "2002")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2010"), N(KP|VP, "bar3", "2011"), N(KP|VP, "baz3", "2012")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2020"), N(KP|VP, "bar3", "2021"), N(KP|VP, "baz3", "2022")})}),
         N(KP|MB, "bar1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2100"), N(KP|VP, "bar3", "2101"), N(KP|VP, "baz3", "2102")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2110"), N(KP|VP, "bar3", "2111"), N(KP|VP, "baz3", "2112")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2120"), N(KP|VP, "bar3", "2121"), N(KP|VP, "baz3", "2122")})}),
         N(KP|MB, "baz1", L{N(KP|MB, "foo2", L{N(KP|VP, "foo3", "2200"), N(KP|VP, "bar3", "2201"), N(KP|VP, "baz3", "2202")}),  N(KP|MB, "bar2", L{N(KP|VP, "foo3", "2210"), N(KP|VP, "bar3", "2211"), N(KP|VP, "baz3", "2212")}),  N(KP|MB, "baz2", L{N(KP|VP, "foo3", "2220"), N(KP|VP, "bar3", "2221"), N(KP|VP, "baz3", "2222")})}),
      })
    })
);


ADD_CASE_TO_GROUP("map of empty seqs",
R"({foo: [], bar: [], baz: []})",
N(MFS, L{
         N(KP|SFS, "foo", L()),
         N(KP|SFS, "bar", L()),
         N(KP|SFS, "baz", L()),
     })
);

ADD_CASE_TO_GROUP("map of seqs, one line",
R"({men: [John Smith, Bill Jones], women: [Mary Smith, Susan Williams]})",
N(MFS, L{
         N(KP|SFS, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SFS, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs",
R"(
men:
  - John Smith
  - Bill Jones
women:
  - Mary Smith
  - Susan Williams
)",
N(MB, L{
         N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, not indented",
R"(
men:
- John Smith
- Bill Jones
women:
- Mary Smith
- Susan Williams
)",
N(MB, L{
         N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, not indented, more",
R"(
product:
- sku: BL4438H
  quantity: 1
  description: Super Hoop
  price: 2392.00  # jumping one level here would be wrong.
tax: 1234.5       # we must jump two levels
product2:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  tax2: 789.10      # we must jump two levels
tax3: 1234.5
product3:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  # a comment here, will it ruin parsing?
  tax2: 789.10      # we must jump two levels
tax4: 1234.5
product4:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
 # what about here?
  tax2: 789.10      # we must jump two levels
tax5: 1234.5
)",
N(MB, L{
    N(KP|SB, "product", L{
       N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
    }),
    N(KP|VP, "tax", "1234.5"),
    N(KP|MB, "product2", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax3", "1234.5"),
    N(KP|MB, "product3", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax4", "1234.5"),
    N(KP|MB, "product4", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax5", "1234.5"),
})
);

ADD_CASE_TO_GROUP("map of seqs, next line",
R"(
men:
  - 
    John Smith
  - 
    Bill Jones
women:
  - 
    Mary Smith
  - 
    Susan Williams
)",
N(MB, L{
        N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
        N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, next line without space",
R"(
men:
  -
    John Smith
  -
    Bill Jones
women:
  -
    Mary Smith
  -
    Susan Williams
)",
N(MB, L{
        N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
        N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, deal with unk",
R"(
skip_commits:
  files:
    - a
    - b
    - c
    - d
    - e
)",
N(MB, L{
  N(KP|MB, "skip_commits", L{
    N(KP|SB, "files", L{
      N(VP, "a"), N(VP, "b"), N(VP, "c"), N(VP, "d"), N(VP, "e")
    })
  })
})
);

ADD_CASE_TO_GROUP("doc as set, missing value", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(!!set
? a
? b
?
)",
N(MB, TL("!!set", L{N(KP|VN, "a", {}), N(KP|VN, "b", {}), N(KN|VN, {}, "")}))
);

ADD_CASE_TO_GROUP("doc as set, implicit", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(!!set
? a
? b
)",
N(MB, TL("!!set", L{N(KP|VN, "a", {}), N(KP|VN, "b", {})}))
);

ADD_CASE_TO_GROUP("doc as set", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(--- !!set
? aa
? bb
? cc
)",
N(STREAM, L{
  N(DOC|MB, TL("!!set", L{
    N(KP|VN, "aa", /*"~"*/{}),
    N(KP|VN, "bb", /*"~"*/{}),
    N(KP|VN, "cc", /*"~"*/{})
  }))
})
);

ADD_CASE_TO_GROUP("sets 2XXW", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(STREAM, L{
  N(DOC|MB, TL("!!set", L{
    N(KP|VN, "Mark McGwire", /*"~"*/{}),
    N(KP|VN, "Sammy Sosa", /*"~"*/{}),
    N(KP|VN, "Ken Griff", /*"~"*/{}),
  }))
})
);

ADD_CASE_TO_GROUP("sets 2XXW, no set", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
---
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(STREAM, L{
  N(DOC|MB, L{
    N(KP|VN, "Mark McGwire", /*"~"*/{}),
    N(KP|VN, "Sammy Sosa", /*"~"*/{}),
    N(KP|VN, "Ken Griff", /*"~"*/{}),
  })
})
);

ADD_CASE_TO_GROUP("sets 2XXW, no doc", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(!!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(MB, TL("!!set", L{
  N(KP|VN, "Mark McGwire", /*"~"*/{}),
  N(KP|VN, "Sammy Sosa", /*"~"*/{}),
  N(KP|VN, "Ken Griff", /*"~"*/{}),
}))
);

ADD_CASE_TO_GROUP("sets 2XXW, no doc, no set", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(MB, L{
  N(KP|VN, "Mark McGwire", /*"~"*/{}),
  N(KP|VN, "Sammy Sosa", /*"~"*/{}),
  N(KP|VN, "Ken Griff", /*"~"*/{}),
})
);

ADD_CASE_TO_GROUP("github3-problem1",
R"(
translation: [-2, -2, 5])",
N(MB, L{N(KP|SFS, "translation", L{N(VP, "-2"), N(VP, "-2"), N(VP, "5")})})
);

// these must work without quotes
ADD_CASE_TO_GROUP("github3-problem2-ex1", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
audio resource:
)",
N(MB, L{N(KP|VN, "audio resource", /*"~"*/{})})
);

ADD_CASE_TO_GROUP("github3-problem2-ex2", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
audio resource:
more:
  example: y
)",
N(MB, L{N(KP|VN, "audio resource", /*"~"*/{}), N(KP|MB, "more", L{N(KP|VP, "example", "y")})})
);

ADD_CASE_TO_GROUP("github3-problem3",
R"(component:
  type: perspective camera component
  some_data: {}  # this was working
  data:
    {}           # but this was not working
)",
N(MB, L{
  N(KP|MB, "component", L{
    N(KP|VP, "type", "perspective camera component"),
    N(KP|MFS, "some_data", L{}),
    N(KP|MFS, "data", L{})
    }
)})
);

/* THIS IS CAUSING VS TO CRASH OUT OF HEAP SPACE
ADD_CASE_TO_GROUP("github3",
R"(
universe:
  objects:
    object:
      uuid: A7AB039C0EF3A74480A1B398247039A7
      components:
        - component:
            type: name component
            data:
              object name: Root Node
        - component:
            type: transform component
            data:
              translation: [-2, -2, 5]
              rotation: [0, 0, 0, 1]
              scaling: [1, 1, 1]
        - component:
            type: perspective camera component
            data:
             {}
        - component:
            type: mesh component
            data:
              mesh resource: TODO
        - component:
            type: lua script component
            data:
             {}
        - component:
            type: audio component
            data:
              audio resource: ''
              type: 0
              current sample: 184102
              spatialized: true
      children:
        - object:
            uuid: E1C364A925D649408E83C8EEF5179A87
            components:
              - component:
                  type: name component
                  data:
                    object name: Prepend
            children:
              []
        - object:
            uuid: 377DBA885AF4CD42B8A56BB3471F60E5
            components:
              - component:
                  type: name component
                  data:
                    object name: pivot
            children:
              []
        - object:
            uuid: 6DD1835797DADB4F95232CE7E9DE41BA
            components:
              - component:
                  type: name component
                  data:
                    object name: Append
            children:
              []
)",
  L{N("universe", L{
        N("objects", L{
            N("object", L{
                N("uuid", "A7AB039C0EF3A74480A1B398247039A7"),
                N("components", L{
                    N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Root Node")}), }), }),
                    N(L{N("component", L{N("type", "transform component"), N("data", L{N("translation", L{N("-2"), N("-2"), N("5")}), N("rotation", L{N("0"), N("0"), N("0"), N("1")}), N("scaling", L{N("1"), N("1"), N("1")}),}), }), }),
                    N(L{N("component", L{N("type", "perspective camera component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "mesh component"), N("data", L{N("mesh resource", "TODO")}), }), }),
                    N(L{N("component", L{N("type", "lua script component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "audio component"), N("data", L{N("audio resource", ""), N("type", "0"), N("current sample", "184102"), N("spatialized", "true"), }), }), }), // component
                  }), // components
                N("children", L{
                    N(L{N("object", L{
                        N("uuid", "E1C364A925D649408E83C8EEF5179A87"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Prepend")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "377DBA885AF4CD42B8A56BB3471F60E5"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "pivot")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "6DD1835797DADB4F95232CE7E9DE41BA"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Append")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                  }), // children
                }), // object
              }) // objects
          }) // universe
      }
);
*/

ADD_CASE_TO_GROUP("github6-problem1",
R"(
- UQxRibHKEDI:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- DcYsg8VFdC0:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- Yt3ymqZXzLY:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
)",
N(SB, L{
N(MB, L{N(KP|SB, "UQxRibHKEDI", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
N(MB, L{N(KP|SB, "DcYsg8VFdC0", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
N(MB, L{N(KP|SB, "Yt3ymqZXzLY", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
})
);

ADD_CASE_TO_GROUP("github6",
R"(
videos:
- UQxRibHKEDI:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- DcYsg8VFdC0:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- Yt3ymqZXzLY:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
)",
N(MB, L{
  N(KP|SB, "videos", L{
    N(MB, L{N(KP|SB, "UQxRibHKEDI", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
    N(MB, L{N(KP|SB, "DcYsg8VFdC0", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
    N(MB, L{N(KP|SB, "Yt3ymqZXzLY", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
  })
})
);

ADD_CASE_TO_GROUP("github34/ex1",
R"(
# correct:
MessageID1: 'MapRegion_HyrulePrairie'
MessageID2: "MapRegion_HyrulePrairie"
MessageID3: 'MapRegion_HyrulePrairie'
MessageID4: "MapRegion_HyrulePrairie"
# incorrect: uninitialised memory?
MessageID5:  'MapRegion_HyrulePrairie'
MessageID6:  "MapRegion_HyrulePrairie"
MessageID7:   'MapRegion_HyrulePrairie'
MessageID8:   "MapRegion_HyrulePrairie"
MessageID9:          'MapRegion_HyrulePrairie'
MessageID0:          "MapRegion_HyrulePrairie"
)",
N(MB, L{
  N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID3", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID4", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID5", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID6", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID7", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID8", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID9", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID0", "MapRegion_HyrulePrairie"),
})
);

ADD_CASE_TO_GROUP("github34/ex2",
R"(
# correct:
- MessageID1: 'MapRegion_HyrulePrairie'
- MessageID2: "MapRegion_HyrulePrairie"
-  MessageID3: 'MapRegion_HyrulePrairie'
-  MessageID4: "MapRegion_HyrulePrairie"
# incorrect: uninitialised memory?
- MessageID5:  'MapRegion_HyrulePrairie'
- MessageID6:  "MapRegion_HyrulePrairie"
- MessageID7:   'MapRegion_HyrulePrairie'
- MessageID8:   "MapRegion_HyrulePrairie"
-  MessageID9:          'MapRegion_HyrulePrairie'
-  MessageID0:          "MapRegion_HyrulePrairie"
)",
N(SB, L{
  N(MB, L{N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID3", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID4", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID5", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID6", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID7", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID8", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID9", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID0", "MapRegion_HyrulePrairie")}),
})
);

ADD_CASE_TO_GROUP("github34",
R"(
# incorrect: uninitialised memory?
-  MessageID1:          'MapRegion_HyrulePrairie'
-  MessageID2:          "MapRegion_HyrulePrairie"

# incorrect: uninitialised memory?
-  MessageID3:          'MapRegion_HyrulePrairie '
-  MessageID4:          "MapRegion_HyrulePrairie "

# incorrect: for some reason the ' is included in the string
-  MessageID5: 'MapRegion_HyrulePrairie '
- MessageID6: 'MapRegion_HyrulePrairie '
-  MessageID7: "MapRegion_HyrulePrairie "
- MessageID8: "MapRegion_HyrulePrairie "

# incorrect: same issue
- MessageID9: 'MapRegion_HyrulePrairie '
- MessageID10: "MapRegion_HyrulePrairie "

# incorrect: still has the trailing quote
- MessageID11: 'MapRegion_HyrulePrairie'
- MessageID12: "MapRegion_HyrulePrairie"

# the string is parsed correctly in this case
- key1: true1
  MessageID1:          'MapRegion_HyrulePrairie1 '
- key2: true2
  MessageID2:          "MapRegion_HyrulePrairie2 "
)",
N(SB, L{
  N(MB, L{N(KP|VS, "MessageID1",  "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID2",  "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID3",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID4",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID5",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID6",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID7",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID8",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID9",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID10", "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID11", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID12", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VP, "key1", "true1"), N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie1 ")}),
  N(MB, L{N(KP|VP, "key2", "true2"), N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie2 ")}),
})
);

ADD_CASE_TO_GROUP("github129", RESOLVE_REFS, R"(
ref: &ref ref_val
a: *ref   # resolve the reference
b: '*ref' # don't resolve, it's just a string
c: "*ref" # don't resolve, it's just a string
d: >      # don't resolve, it's just a string
  *ref
e: >-     # don't resolve, it's just a string
  *ref
f: >+     # don't resolve, it's just a string
  *ref
g: |      # don't resolve, it's just a string
  *ref
h: |-     # don't resolve, it's just a string
  *ref
i: |+     # don't resolve, it's just a string
  *ref
)",
N(MB, L{
  N(KP|VP, "ref", "ref_val"),
  N(KP|VP, "a", "ref_val"),    // this should be resolved
  N(KP|VS, "b", "*ref"),   // this should not be resolved (just a string)
  N(KP|VD, "c", "*ref"),   // this should not be resolved (just a string)
  N(KP|VF, "d", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VF, "e", "*ref"),   // this should not be resolved (just a string)
  N(KP|VF, "f", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VL, "g", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VL, "h", "*ref"),   // this should not be resolved (just a string)
  N(KP|VL, "i", "*ref\n"),   // this should not be resolved (just a string)
})
);

} // CASE_GROUP()


} // namespace yml
} // namespace c4
