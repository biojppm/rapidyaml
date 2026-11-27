#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(SIMPLE_SEQ)
{

ADD_CASE_TO_GROUP("empty seq, explicit",
"[]",
    SFS
);


ADD_CASE_TO_GROUP("empty seq, explicit, whitespace",
" []",
    SFS
);


ADD_CASE_TO_GROUP("empty seq, multiline",
R"([
]
)",
    SFM
);

ADD_CASE_TO_GROUP("empty seq, multilines",
R"([
# ksjdfkjhsdfkjhsdfkjh


]
)",
    SFM
);

ADD_CASE_TO_GROUP("simple seq",
R"(- 0
- 1
- 2
- 3
)",
N(SB, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);


ADD_CASE_TO_GROUP("simple seq, flow, single line",
"[0, 1, 2, 3]",
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, single line, trailcomma",
"[0, 1, 2, 3,]",
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"},})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, unindented",
R"([
0,
1,
2,
3
])",
N(SFM, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"},})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, unindented, trailcomma",
R"([
0,
1,
2,
3,
])",
N(SFM, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
N(SFM, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, comments prev line",
R"([
# bla0
0,
# bla1
1,
# bla2
2,
# bla3
3
])",
N(SFM, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
N(SFM, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, comments inline",
R"(
- 0   # this is a foo
- 1   # this is a bar
- 2   # this is a bar
- 3   # this is a bar
)",
N(SB, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, comments prev line",
R"(
# this is a foo
- 0
# this is a bar
- 1
# this is a baz
- 2
# this is a bat
- 3
)",
N(SB, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, empty elements", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(-
-
- 
-  
- # with comments
- # more comments
)",
N(SB, L{VN, VN, VN, VN, VN, VN})
);

ADD_CASE_TO_GROUP("simple seq, empty elements with non-empty first", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
- non-empty
-
- 
-  
- # with comments
- # more comments
)",
N(SB, L{N{VP, "non-empty"}, VN, VN, VN, VN, VN})
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, comma",
R"(
- a,b
- c,d
- e,f
- a, b
- c, d
- e, f
- a , b
- c , d
- e , f
- a ,b
- c ,d
- e ,f
)",
N(SB, L{N{VP, "a,b"}, N{VP, "c,d"}, N{VP, "e,f"},
  N{VP, "a, b"}, N{VP, "c, d"}, N{VP, "e, f"},
  N{VP, "a , b"}, N{VP, "c , d"}, N{VP, "e , f"},
  N{VP, "a ,b"}, N{VP, "c ,d"}, N{VP, "e ,f"},
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, colon",
R"(
- a:b
- "c:d"
- 'e:f'
- a :b
- "c :d"
- 'e :f'
- a : b    # THIS IS A KEY-VAL!!!
- "c : d"
- 'e : f'
- a: b     # THIS IS A KEY-VAL!!!
- "c: d"
- 'e: f'
)",
N(SB, L{
  N(VP, "a:b"), N(VD, "c:d"), N(VS, "e:f"),
  N(VP, "a :b"), N(VD, "c :d"), N(VS, "e :f"),
  N(MB, L{N(KP|VP, "a", "b")}), N(VD, "c : d"), N(VS, "e : f"),
  N(MB, L{N(KP|VP, "a", "b")}), N(VD, "c: d"), N(VS, "e: f"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, cardinal",
R"(
- a#b
- "a#b"
- 'a#b'
- a# b
- "a# b"
- 'a# b'
- a # b
- "a # b"
- 'a # b'
- a #b
- "a #b"
- 'a #b'
)",
N(SB, L{
  N(VP, "a#b"), N(VD, "a#b"), N(VS, "a#b"),
  N(VP, "a# b"), N(VD, "a# b"), N(VS, "a# b"),
  N(VP, "a"), N(VD, "a # b"), N(VS, "a # b"),
  N(VP, "a"), N(VD, "a #b"), N(VS, "a #b"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, dash",
R"(
- a-b
- "a-b"
- 'a-b'
- a- b
- "a- b"
- 'a- b'
- a - b
- "a - b"
- 'a - b'
- a -b
- "a -b"
- 'a -b'
)",
N(SB, L{
 N(VP, "a-b"),   N(VD, "a-b"),   N(VS, "a-b"),
 N(VP, "a- b"),  N(VD, "a- b"),  N(VS, "a- b"),
 N(VP, "a - b"), N(VD, "a - b"), N(VS, "a - b"),
 N(VP, "a -b"),  N(VD, "a -b"),  N(VS, "a -b"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, left-curly",
R"(
- a{b
- "a{b"
- 'a{b'
- a{ b
- "a{ b"
- 'a{ b'
- a { b
- "a { b"
- 'a { b'
- a {b
- "a {b"
- 'a {b'
)",
N(SB, L{
  N(VP, "a{b"),   N(VD, "a{b"),   N(VS, "a{b"),
  N(VP, "a{ b"),  N(VD, "a{ b"),  N(VS, "a{ b"),
  N(VP, "a { b"), N(VD, "a { b"), N(VS, "a { b"),
  N(VP, "a {b"),  N(VD, "a {b"),  N(VS, "a {b"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, right-curly",
R"(
- a}b
- "a}b"
- 'a}b'
- a} b
- "a} b"
- 'a} b'
- a } b
- "a } b"
- 'a } b'
- a }b
- "a }b"
- 'a }b'
)",
N(SB, L{
  N(VP, "a}b"),   N(VD, "a}b"),   N(VS, "a}b"),
  N(VP, "a} b"),  N(VD, "a} b"),  N(VS, "a} b"),
  N(VP, "a } b"), N(VD, "a } b"), N(VS, "a } b"),
  N(VP, "a }b"),  N(VD, "a }b"),  N(VS, "a }b"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, left-bracket",
R"(
- a[b
- "a[b"
- 'a[b'
- a[ b
- "a[ b"
- 'a[ b'
- a [ b
- "a [ b"
- 'a [ b'
- a [b
- "a [b"
- 'a [b'
)",
N(SB, L{
  N(VP, "a[b"),   N(VD, "a[b"),   N(VS, "a[b"),
  N(VP, "a[ b"),  N(VD, "a[ b"),  N(VS, "a[ b"),
  N(VP, "a [ b"), N(VD, "a [ b"), N(VS, "a [ b"),
  N(VP, "a [b"),  N(VD, "a [b"),  N(VS, "a [b"),
    })
);

ADD_CASE_TO_GROUP("simple seq, scalars with special chars, right-bracket",
R"(
- a]b
- "a]b"
- 'a]b'
- a] b
- "a] b"
- 'a] b'
- a ] b
- "a ] b"
- 'a ] b'
- a ]b
- "a ]b"
- 'a ]b'
)",
N(SB, L{
  N(VP, "a]b"),   N(VD, "a]b"),   N(VS, "a]b"),
  N(VP, "a] b"),  N(VD, "a] b"),  N(VS, "a] b"),
  N(VP, "a ] b"), N(VD, "a ] b"), N(VS, "a ] b"),
  N(VP, "a ]b"),  N(VD, "a ]b"),  N(VS, "a ]b"),
    })
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, comma",
R"([
 a,b,  "c,d",   'e,f',
 a, b,  "c, d",   'e, f',
 a , b,  "c , d",   'e , f',
 a ,b,  "c ,d",   'e ,f',
])",
N(SFM, L{
  N(VP, "a"), N(VP, "b"), N(VD, "c,d"),   N(VS, "e,f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c, d"),  N(VS, "e, f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c , d"), N(VS, "e , f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c ,d"),  N(VS, "e ,f"),
})
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, colon", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"([
    :a,
    :0,
    ::,
    :-,
    :*,
    :@,
    :%,
    :^,
    :$,
    :`,
    : ,    # this is a map
    :	,  # this is a map (when tab tokens are enabled)
    x:a,
    x:0,
    x::,
    x:-,
    x:*,
    x:@,
    x:%,
    x:^,
    x:$,
    x:`,
    x: ,   # this is a map
    x:	,  # this is a map (when tab tokens are enabled)
    :z:a,
    :z:0,
    :z::,
    :z:-,
    :z:*,
    :z:@,
    :z:%,
    :z:^,
    :z:$,
    :z:`,
    :z: ,   # this is a map
    :z:	,  # this is a map (when tab tokens are enabled)
]
)",
N(SFM, L{
   N(VP, ":a"),
   N(VP, ":0"),
   N(MFS, L{N(KP|VN, ":", {})}),
   N(VP, ":-"),
   N(VP, ":*"),
   N(VP, ":@"),
   N(VP, ":%"),
   N(VP, ":^"),
   N(VP, ":$"),
   N(VP, ":`"),
   N(MFS, L{KN|VN}),
   N(MFS, L{KN|VN}),
   N(VP, "x:a"),
   N(VP, "x:0"),
   N(MFS, L{N(KP|VN, "x:", {})}),
   N(VP, "x:-"),
   N(VP, "x:*"),
   N(VP, "x:@"),
   N(VP, "x:%"),
   N(VP, "x:^"),
   N(VP, "x:$"),
   N(VP, "x:`"),
   N(MFS, L{N(KP|VN, "x", "")}),
   RYML_WITH_OR_WITHOUT_TAB_TOKENS_(N(MFS, L{N(KP|VN, "x", "")}), N(VP, "x:\t")),
   N(VP, ":z:a"),
   N(VP, ":z:0"),
   N(MFS, L{N(KP|VN, ":z:", {})}),
   N(VP, ":z:-"),
   N(VP, ":z:*"),
   N(VP, ":z:@"),
   N(VP, ":z:%"),
   N(VP, ":z:^"),
   N(VP, ":z:$"),
   N(VP, ":z:`"),
   N(MFS, L{N(KP|VN, ":z", {})}),
   RYML_WITH_OR_WITHOUT_TAB_TOKENS_(N(MFS, L{N(KP|VN, ":z", "")}), N(VP, ":z:\t")),
 })
);

ADD_CASE_TO_GROUP("simple seq blck, scalars with special chars, colon", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(- :a
- :0
- ::
- :-
- :*
- :@
- :%
- :^
- :$
- :`
- :   # this is a map
#- :	  # this is a map (when tab tokens are enabled)
- x:a
- x:0
- x::
- x:-
- x:*
- x:@
- x:%
- x:^
- x:$
- x:`
- x:      # this is a map
#- x:	  # this is a map (when tab tokens are enabled)
- :z:a
- :z:0
- :z::
- :z:-
- :z:*
- :z:@
- :z:%
- :z:^
- :z:$
- :z:`
- :z:      # this is a map
#- :z:	  # this is a map (when tab tokens are enabled)
)",
N(SB, L{
 N(VP, ":a"),
 N(VP, ":0"),
 N(MB, L{N(KP|VN, ":", {})}),
 N(VP, ":-"),
 N(VP, ":*"),
 N(VP, ":@"),
 N(VP, ":%"),
 N(VP, ":^"),
 N(VP, ":$"),
 N(VP, ":`"),
 N(MB, L{KN|VN}),
// RYML_WITH_OR_WITHOUT_TAB_TOKENS_(N(MB, L{KN|VN}),
//                                  N(VP, ":")),
 N(VP, "x:a"),
 N(VP, "x:0"),
 N(MB, L{N(KP|VN, "x:", {})}),
 N(VP, "x:-"),
 N(VP, "x:*"),
 N(VP, "x:@"),
 N(VP, "x:%"),
 N(VP, "x:^"),
 N(VP, "x:$"),
 N(VP, "x:`"),
 N(MB, L{N(KP|VN, "x", {})}),
// RYML_WITH_OR_WITHOUT_TAB_TOKENS_(N(MB, L{N(KP|VN, "x", "")}),
//                                  N(VP, "x:")),
 N(VP, ":z:a"),
 N(VP, ":z:0"),
 N(MB, L{N(KP|VN, ":z:", {})}),
 N(VP, ":z:-"),
 N(VP, ":z:*"),
 N(VP, ":z:@"),
 N(VP, ":z:%"),
 N(VP, ":z:^"),
 N(VP, ":z:$"),
 N(VP, ":z:`"),
 N(MB, L{N(KP|VN, ":z", "")}),
// RYML_WITH_OR_WITHOUT_TAB_TOKENS_(N(MB, L{N(KP|VN, ":z", "")}),
//                                  N(VP, ":z:")),
})
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, colon 2",
R"([
 a:b, "c:d",   'e:f',
 a: b, "c: d",   'e: f',
 a : b,  "c : d",   'e : f',
 a :b,  "c :d",   'e :f',
])",
N(SFM, L{
  N(VP, "a:b"), N(VD, "c:d"),   N(VS, "e:f"),
  N(MFS, L{N(KP|VP, "a", "b")}), N(VD, "c: d"),  N(VS, "e: f"),
  N(MFS, L{N(KP|VP, "a", "b")}), N(VD, "c : d"), N(VS, "e : f"),
  N(VP, "a :b"), N(VD, "c :d"),  N(VS, "e :f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, colon 2",
R"(- a:b
- "c:d"
- 'e:f'
- a: b
- "c: d"
- 'e: f'
- a : b
- "c : d"
- 'e : f'
- a :b
- "c :d"
- 'e :f'
)",
N(SB, L{
  N(VP, "a:b"), N(VD, "c:d"),   N(VS, "e:f"),
  N(MB, L{N(KP|VP, "a", "b")}), N(VD, "c: d"),  N(VS, "e: f"),
  N(MB, L{N(KP|VP, "a", "b")}), N(VD, "c : d"), N(VS, "e : f"),
  N(VP, "a :b"), N(VD, "c :d"),  N(VS, "e :f"),
    })
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, cardinal",
R"([
 a#b, "c#d",   'e#f',
 a# b, "c# d",   'e# f',
 a # b, "c # d",   'e # f',
, # this is needed because of the comment above
 a #b, "c #d",   'e #f',
])",
N(SFM, L{
  N(VP, "a#b"), N(VD, "c#d"), N(VS, "e#f"),
  N(VP, "a# b"), N(VD, "c# d"), N(VS, "e# f"),
  N(VP, "a"),
  N(VP, "a"),
})
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, cardinal",
R"(- a#b
- "c#d"
- 'e#f'
- a# b
- "c# d"
- 'e# f'
- a # b
- "c # d"
- 'e # f'
- a #b
- "c #d"
- 'e #f'
)",
N(SB, L{
  N(VP, "a#b"), N(VD, "c#d"), N(VS, "e#f"),
  N(VP, "a# b"), N(VD, "c# d"), N(VS, "e# f"),
  N(VP, "a"), N(VD, "c # d"), N(VS, "e # f"),
  N(VP, "a"), N(VD, "c #d"), N(VS, "e #f"),
})
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, dash",
R"([
 a-b, "c-d",   'e-f',
 a- b, "c- d",   'e- f',
 a - b, "c - d",   'e - f',
 a -b, "c -d",   'e -f',
])",
N(SFM, L{
  N(VP, "a-b"),   N(VD, "c-d"),   N(VS, "e-f"),
  N(VP, "a- b"),  N(VD, "c- d"),  N(VS, "e- f"),
  N(VP, "a - b"), N(VD, "c - d"), N(VS, "e - f"),
  N(VP, "a -b"),  N(VD, "c -d"),  N(VS, "e -f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, dash",
R"(- a-b
- "c-d"
- 'e-f'
- a- b
- "c- d"
- 'e- f'
- a - b
- "c - d"
- 'e - f'
- a -b
- "c -d"
- 'e -f'
)",
N(SB, L{
  N(VP, "a-b"),   N(VD, "c-d"),   N(VS, "e-f"),
  N(VP, "a- b"),  N(VD, "c- d"),  N(VS, "e- f"),
  N(VP, "a - b"), N(VD, "c - d"), N(VS, "e - f"),
  N(VP, "a -b"),  N(VD, "c -d"),  N(VS, "e -f"),
    })
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, left-bracket",
R"([
 #a[b,
 "c[d",   'e[f',
 #a[ b,
 "c[ d",   'e[ f',
 #a [ b,
 "c [ d",   'e [ f',
 #a [b,
 "c [d",   'e [f',
])",
N(SFM, L{
  /*N(VP, "a[b"),  */ N(VD, "c[d"),   N(VS, "e[f"),
  /*N(VP, "a[ b"), */ N(VD, "c[ d"),  N(VS, "e[ f"),
  /*N(VP, "a [ b"),*/ N(VD, "c [ d"), N(VS, "e [ f"),
  /*N(VP, "a [b"), */ N(VD, "c [d"),  N(VS, "e [f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, left-bracket",
R"(- a[b
- "c[d"
- 'e[f'
- a[ b
- "c[ d"
- 'e[ f'
- a [ b
- "c [ d"
- 'e [ f'
- a [b
- "c [d"
- 'e [f'
)",
N(SB, L{
  N(VP, "a[b"),   N(VD, "c[d"),   N(VS, "e[f"),
  N(VP, "a[ b"),  N(VD, "c[ d"),  N(VS, "e[ f"),
  N(VP, "a [ b"), N(VD, "c [ d"), N(VS, "e [ f"),
  N(VP, "a [b"),  N(VD, "c [d"),  N(VS, "e [f"),
    })
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, right-bracket",
R"([
# a]b,
   "c]d",   'e]f',
# a] b,
   "c] d",   'e] f',
# a ] b,
   "c ] d",   'e ] f',
# a ]b,
   "c ]d",   'e ]f',
])",
N(SFM, L{
  /*N(VP, "a]b"), */  N(VD, "c]d"),   N(VS, "e]f"),
  /*N(VP, "a] b"), */ N(VD, "c] d"),  N(VS, "e] f"),
  /*N(VP, "a ] b"),*/ N(VD, "c ] d"), N(VS, "e ] f"),
  /*N(VP, "a ]b"), */ N(VD, "c ]d"),  N(VS, "e ]f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, right-bracket",
R"(- a]b
- "c]d"
- 'e]f'
- a] b
- "c] d"
- 'e] f'
- a ] b
- "c ] d"
- 'e ] f'
- a ]b
- "c ]d"
- 'e ]f'
)",
N(SB, L{
  N(VP, "a]b"),   N(VD, "c]d"),   N(VS, "e]f"),
  N(VP, "a] b"),  N(VD, "c] d"),  N(VS, "e] f"),
  N(VP, "a ] b"), N(VD, "c ] d"), N(VS, "e ] f"),
  N(VP, "a ]b"),  N(VD, "c ]d"),  N(VS, "e ]f"),
    })
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, left-curly",
R"([
# a{b,
   "c{d",   'e{f',
# a{ b,
   "c{ d",   'e{ f',
# a { b,
   "c { d",   'e { f',
# a {b,
   "c {d",   'e {f',
])",
N(SFM, L{
  /*N(VP, "a{b"), */  N(VD, "c{d"),   N(VS, "e{f"),
  /*N(VP, "a{ b"), */ N(VD, "c{ d"),  N(VS, "e{ f"),
  /*N(VP, "a { b"),*/ N(VD, "c { d"), N(VS, "e { f"),
  /*N(VP, "a {b"), */ N(VD, "c {d"),  N(VS, "e {f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, left-curly",
R"(- a{b
- "c{d"
- 'e{f'
- a{ b
- "c{ d"
- 'e{ f'
- a { b
- "c { d"
- 'e { f'
- a {b
- "c {d"
- 'e {f'
)",
N(SB, L{
  N(VP, "a{b"),   N(VD, "c{d"),   N(VS, "e{f"),
  N(VP, "a{ b"),  N(VD, "c{ d"),  N(VS, "e{ f"),
  N(VP, "a { b"), N(VD, "c { d"), N(VS, "e { f"),
  N(VP, "a {b"),  N(VD, "c {d"),  N(VS, "e {f"),
    })
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, right-curly",
R"([
# a}b,
   "c}d",   'e}f',
# a} b,
   "c} d",   'e} f',
# a } b,
   "c } d",   'e } f',
# a }b,
   "c }d",   'e }f',
])",
N(SFM, L{
  /*N(VP, "a}b"), */  N(VD, "c}d"),   N(VS, "e}f"),
  /*N(VP, "a} b"), */ N(VD, "c} d"),  N(VS, "e} f"),
  /*N(VP, "a } b"),*/ N(VD, "c } d"), N(VS, "e } f"),
  /*N(VP, "a }b"), */ N(VD, "c }d"),  N(VS, "e }f"),
    })
);

ADD_CASE_TO_GROUP("simple seq block, scalars with special chars, right-curly",
R"(- a}b
- "c}d"
- 'e}f'
- a} b
- "c} d"
- 'e} f'
- a } b
- "c } d"
- 'e } f'
- a }b
- "c }d"
- 'e }f'
)",
N(SB, L{
  N(VP, "a}b"),   N(VD, "c}d"),   N(VS, "e}f"),
  N(VP, "a} b"),  N(VD, "c} d"),  N(VS, "e} f"),
  N(VP, "a } b"), N(VD, "c } d"), N(VS, "e } f"),
  N(VP, "a }b"),  N(VD, "c }d"),  N(VS, "e }f"),
    })
);

// see https://github.com/biojppm/rapidyaml/issues/28
ADD_CASE_TO_GROUP("simple seq - indentless jumps",
R"(enemy:
- actors:
  - {name: Enemy_Bokoblin_Junior, value: 4.0}
  - {name: Enemy_Bokoblin_Middle, value: 16.0}
  - {name: Enemy_Bokoblin_Senior, value: 32.0}
  - {name: Enemy_Bokoblin_Dark, value: 48.0}
  species: BokoblinSeries
  more: attributes
- wtf enemy
enemy2:
  - actors:
      - {name: Enemy_Bokoblin_Junior, value: 4.0}
      - {name: Enemy_Bokoblin_Middle, value: 16.0}
      - {name: Enemy_Bokoblin_Senior, value: 32.0}
      - {name: Enemy_Bokoblin_Dark, value: 48.0}
    species: BokoblinSeries
    more: attributes
  - wtf enemy2
)",
N(MB, L{ // 0, map
  N(KP|SB, "enemy", L{ // 1, keyseq
    N(MB, L{ // 2, map
      N(KP|SB, "actors", L{ // 3, keyseq
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Junior"), N(KP|VP, "value", "4.0"),}), // 4, map + 5,6
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Middle"), N(KP|VP, "value", "16.0"),}), // 7, map + 8,9
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Senior"), N(KP|VP, "value", "32.0"),}), // 10, map + 11,12
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Dark"), N(KP|VP, "value", "48.0"),}), // 13, map + 14,15
      }),
      N(KP|VP, "species", "BokoblinSeries"), // 16, keyval
      N(KP|VP, "more", "attributes"), // 17, keyval
    }),
    N(VP, "wtf enemy"), // 18
  }),
  N(KP|SB, "enemy2", L{ // 19, keyseq
    N(MB, L{ // 20, map
      N(KP|SB, "actors", L{ // 21, keyseq
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Junior"), N(KP|VP, "value", "4.0"),}),
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Middle"), N(KP|VP, "value", "16.0"),}),
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Senior"), N(KP|VP, "value", "32.0"),}),
        N(MFS, L{N(KP|VP, "name", "Enemy_Bokoblin_Dark"), N(KP|VP, "value", "48.0"),}),
      }),
      N(KP|VP, "species", "BokoblinSeries"),
      N(KP|VP, "more", "attributes"),
    }),
    N(VP, "wtf enemy2"),
  }),
})
);

ADD_CASE_TO_GROUP("simple seq, invalid character 1", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
}
)",
  Location(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 2", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
]
)",
  Location(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 3", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
:
)",
  Location(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 4", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
abcdef!
)",
  Location(2, 1)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 1", EXPECT_PARSE_ERROR,
R"([,]
)",
  Location(1, 2)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 2", EXPECT_PARSE_ERROR,
R"([ ,]
)",
  Location(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 3", EXPECT_PARSE_ERROR,
R"([ , ]
)",
  Location(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 4", EXPECT_PARSE_ERROR,
R"([ , val]
)",
  Location(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 5", EXPECT_PARSE_ERROR,
R"([ , , ]
)",
  Location(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, seqimap, at line end",
R"([a: 

b]
)",
N(SFM, L{N(MFM, L{N(KP|VP, "a", "b")})})
);

ADD_CASE_TO_GROUP("generic seq v0",
R"(
- item 1
- item 2
- - item 3.1
  - item 3.2
- key 1: value 1
  key 2: value 2
)",
N(SB, L{
      N(VP, "item 1"),
      N(VP, "item 2"),
      N(SB, L{N(VP, "item 3.1"), N(VP, "item 3.2")}),
      N(MB, L{N(KP|VP, "key 1", "value 1"), N(KP|VP, "key 2", "value 2")})
  })
);

ADD_CASE_TO_GROUP("generic seq v1",
R"(
- item 1
- item 2
-
  - item 3.1
  - item 3.2
-
  key 1: value 1
  key 2: value 2
)",
N(SB, L{
      N(VP, "item 1"),
      N(VP, "item 2"),
      N(SB, L{N(VP, "item 3.1"), N(VP, "item 3.2")}),
      N(MB, L{N(KP|VP, "key 1", "value 1"), N(KP|VP, "key 2", "value 2")})
  })
);

ADD_CASE_TO_GROUP("nested seq x2, empty, oneline",
R"([[], [], []])",
N(SFS, L{SFS, SFS, SFS})
);

ADD_CASE_TO_GROUP("nested seq x2, explicit, same line",
R"([[00, 01, 02], [10, 11, 12], [20, 21, 22]])",
N(SFS, L{
      N(SFS, L{N(VP, "00"), N(VP, "01"), N(VP, "02")}),
      N(SFS, L{N(VP, "10"), N(VP, "11"), N(VP, "12")}),
      N(SFS, L{N(VP, "20"), N(VP, "21"), N(VP, "22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, explicit first+last level, same line, no spaces",
R"([[00,01,02],[10,11,12],[20,21,22]])",
N(SFS, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
          })
);

ADD_CASE_TO_GROUP("nested seq x2, explicit",
R"([
[00, 01, 02],
[10, 11, 12],
[20, 21, 22],
])",
N(SFM, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
          })
);

ADD_CASE_TO_GROUP("nested seq x2",
R"(
- - 00
  - 01
  - 02
- - 10
  - 11
  - 12
- - 20
  - 21
  - 22
)",
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, next line",
R"(
-
  - 00
  - 01
  - 02
-
  - 10
  - 11
  - 12
-
  - 20
  - 21
  - 22
)",
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, all next line",
R"(
-
  -
    00
  -
    01
  -
    02
-
  -
    10
  -
    11
  -
    12
-
  -
    20
  -
    21
  -
    22
)",
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, all next line, with whitespace",
R"(
-
  -
        00
  -
      01
  -
    02
-
  -
    10
  -
       11
  -
         12
-
  -



       20


  -


      21

  -
    22
)",
N(SB, L{
      N(SB, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SB, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SB, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);

ADD_CASE_TO_GROUP("nested seq x2, implicit first, explicit last level",
R"(
- [00, 01, 02]
- [10, 11, 12]
- [20, 21, 22]
)",
N(SB, L{
      N(SFS, L{N(VP,"00"), N(VP,"01"), N(VP,"02")}),
      N(SFS, L{N(VP,"10"), N(VP,"11"), N(VP,"12")}),
      N(SFS, L{N(VP,"20"), N(VP,"21"), N(VP,"22")}),
})
);



ADD_CASE_TO_GROUP("nested seq x3, explicit",
R"([
[[000, 001, 002], [010, 011, 012], [020, 021, 022]],
[[100, 101, 102], [110, 111, 112], [120, 121, 122]],
[[200, 201, 202], [210, 211, 212], [220, 221, 222]],
])",
N(SFM, L{
      N{SFS, L{N{SFS, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SFS, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SFS, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SFS, L{N{SFS, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SFS, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SFS, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SFS, L{N{SFS, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SFS, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SFS, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
    })
);

ADD_CASE_TO_GROUP("nested seq x3",
R"(
- - - 000
    - 001
    - 002
  - - 010
    - 011
    - 012
  - - 020
    - 021
    - 022
- - - 100
    - 101
    - 102
  - - 110
    - 111
    - 112
  - - 120
    - 121
    - 122
- - - 200
    - 201
    - 202
  - - 210
    - 211
    - 212
  - - 220
    - 221
    - 222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x3, continued on next line",
R"(
-
  -
    - 000
    - 001
    - 002
  -
    - 010
    - 011
    - 012
  -
    - 020
    - 021
    - 022
-
  -
    - 100
    - 101
    - 102
  -
    - 110
    - 111
    - 112
  -
    - 120
    - 121
    - 122
-
  -
    - 200
    - 201
    - 202
  -
    - 210
    - 211
    - 212
  -
    - 220
    - 221
    - 222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x3, all continued on next line",
R"(
-
  -
    -
      000
    -
      001
    -
      002
  -
    -
      010
    -
      011
    -
      012
  -
    -
      020
    -
      021
    -
      022
-
  -
    -
      100
    -
      101
    -
      102
  -
    -
      110
    -
      111
    -
      112
  -
    -
      120
    -
      121
    -
      122
-
  -
    -
      200
    -
      201
    -
      202
  -
    -
      210
    -
      211
    -
      212
  -
    -
      220
    -
      221
    -
      222
)",
N(SB, L{
      N{SB, L{N{SB, L{N(VP, "000"), N(VP, "001"), N(VP, "002")}}, N{SB, L{N(VP, "010"), N(VP, "011"), N(VP, "012")}}, N{SB, L{N(VP, "020"), N(VP, "021"), N(VP, "022")}}}},
      N{SB, L{N{SB, L{N(VP, "100"), N(VP, "101"), N(VP, "102")}}, N{SB, L{N(VP, "110"), N(VP, "111"), N(VP, "112")}}, N{SB, L{N(VP, "120"), N(VP, "121"), N(VP, "122")}}}},
      N{SB, L{N{SB, L{N(VP, "200"), N(VP, "201"), N(VP, "202")}}, N{SB, L{N(VP, "210"), N(VP, "211"), N(VP, "212")}}, N{SB, L{N(VP, "220"), N(VP, "221"), N(VP, "222")}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x3 with tags",
R"(
- !!seq
  - !!seq
    - foo
- bar
)",
N(SB, L{
    N(SB, TL("!!seq", L{N(SB, TL("!!seq", L{N(VP,"foo")}))})),
    N(VP,"bar"),
    })
);


ADD_CASE_TO_GROUP("nested seq x4, explicit",
R"([
[[[0000, 0001, 0002], [0010, 0011, 0012], [0020, 0021, 0022]],
 [[0100, 0101, 0102], [0110, 0111, 0112], [0120, 0121, 0122]],
 [[0200, 0201, 0202], [0210, 0211, 0212], [0220, 0221, 0222]]],

[[[1000, 1001, 1002], [1010, 1011, 1012], [1020, 1021, 1022]],
 [[1100, 1101, 1102], [1110, 1111, 1112], [1120, 1121, 1122]],
 [[1200, 1201, 1202], [1210, 1211, 1212], [1220, 1221, 1222]]],

[[[2000, 2001, 2002], [2010, 2011, 2012], [2020, 2021, 2022]],
 [[2100, 2101, 2102], [2110, 2111, 2112], [2120, 2121, 2122]],
 [[2200, 2201, 2202], [2210, 2211, 2212], [2220, 2221, 2222]]],
])",
N(SFM, L{
      N{SFM, L{N{SFS, L{N{SFS, L{N(VP, "0000"), N(VP, "0001"), N(VP, "0002")}}, N{SFS, L{N(VP, "0010"), N(VP, "0011"), N(VP, "0012")}}, N{SFS, L{N(VP, "0020"), N(VP, "0021"), N(VP, "0022")}}}}, N{SFS, L{N{SFS, L{N(VP, "0100"), N(VP, "0101"), N(VP, "0102")}}, N{SFS, L{N(VP, "0110"), N(VP, "0111"), N(VP, "0112")}}, N{SFS, L{N(VP, "0120"), N(VP, "0121"), N(VP, "0122")}}}}, N{SFS, L{N{SFS, L{N(VP, "0200"), N(VP, "0201"), N(VP, "0202")}}, N{SFS, L{N(VP, "0210"), N(VP, "0211"), N(VP, "0212")}}, N{SFS, L{N(VP, "0220"), N(VP, "0221"), N(VP, "0222")}}}}}},
      N{SFM, L{N{SFS, L{N{SFS, L{N(VP, "1000"), N(VP, "1001"), N(VP, "1002")}}, N{SFS, L{N(VP, "1010"), N(VP, "1011"), N(VP, "1012")}}, N{SFS, L{N(VP, "1020"), N(VP, "1021"), N(VP, "1022")}}}}, N{SFS, L{N{SFS, L{N(VP, "1100"), N(VP, "1101"), N(VP, "1102")}}, N{SFS, L{N(VP, "1110"), N(VP, "1111"), N(VP, "1112")}}, N{SFS, L{N(VP, "1120"), N(VP, "1121"), N(VP, "1122")}}}}, N{SFS, L{N{SFS, L{N(VP, "1200"), N(VP, "1201"), N(VP, "1202")}}, N{SFS, L{N(VP, "1210"), N(VP, "1211"), N(VP, "1212")}}, N{SFS, L{N(VP, "1220"), N(VP, "1221"), N(VP, "1222")}}}}}},
      N{SFM, L{N{SFS, L{N{SFS, L{N(VP, "2000"), N(VP, "2001"), N(VP, "2002")}}, N{SFS, L{N(VP, "2010"), N(VP, "2011"), N(VP, "2012")}}, N{SFS, L{N(VP, "2020"), N(VP, "2021"), N(VP, "2022")}}}}, N{SFS, L{N{SFS, L{N(VP, "2100"), N(VP, "2101"), N(VP, "2102")}}, N{SFS, L{N(VP, "2110"), N(VP, "2111"), N(VP, "2112")}}, N{SFS, L{N(VP, "2120"), N(VP, "2121"), N(VP, "2122")}}}}, N{SFS, L{N{SFS, L{N(VP, "2200"), N(VP, "2201"), N(VP, "2202")}}, N{SFS, L{N(VP, "2210"), N(VP, "2211"), N(VP, "2212")}}, N{SFS, L{N(VP, "2220"), N(VP, "2221"), N(VP, "2222")}}}}}},
          })
);

ADD_CASE_TO_GROUP("nested seq x4",
R"(
- - - - 0000
      - 0001
      - 0002
    - - 0010
      - 0011
      - 0012
    - - 0020
      - 0021
      - 0022
  - - - 0100
      - 0101
      - 0102
    - - 0110
      - 0111
      - 0112
    - - 0120
      - 0121
      - 0122
  - - - 0200
      - 0201
      - 0202
    - - 0210
      - 0211
      - 0212
    - - 0220
      - 0221
      - 0222
- - - - 1000
      - 1001
      - 1002
    - - 1010
      - 1011
      - 1012
    - - 1020
      - 1021
      - 1022
  - - - 1100
      - 1101
      - 1102
    - - 1110
      - 1111
      - 1112
    - - 1120
      - 1121
      - 1122
  - - - 1200
      - 1201
      - 1202
    - - 1210
      - 1211
      - 1212
    - - 1220
      - 1221
      - 1222
- - - - 2000
      - 2001
      - 2002
    - - 2010
      - 2011
      - 2012
    - - 2020
      - 2021
      - 2022
  - - - 2100
      - 2101
      - 2102
    - - 2110
      - 2111
      - 2112
    - - 2120
      - 2121
      - 2122
  - - - 2200
      - 2201
      - 2202
    - - 2210
      - 2211
      - 2212
    - - 2220
      - 2221
      - 2222
)",
N(SB, L{
      N{SB, L{N{SB, L{N{SB, L{N(VP, "0000"), N(VP, "0001"), N(VP, "0002")}}, N{SB, L{N(VP, "0010"), N(VP, "0011"), N(VP, "0012")}}, N{SB, L{N(VP, "0020"), N(VP, "0021"), N(VP, "0022")}}}}, N{SB, L{N{SB, L{N(VP, "0100"), N(VP, "0101"), N(VP, "0102")}}, N{SB, L{N(VP, "0110"), N(VP, "0111"), N(VP, "0112")}}, N{SB, L{N(VP, "0120"), N(VP, "0121"), N(VP, "0122")}}}}, N{SB, L{N{SB, L{N(VP, "0200"), N(VP, "0201"), N(VP, "0202")}}, N{SB, L{N(VP, "0210"), N(VP, "0211"), N(VP, "0212")}}, N{SB, L{N(VP, "0220"), N(VP, "0221"), N(VP, "0222")}}}}}},
      N{SB, L{N{SB, L{N{SB, L{N(VP, "1000"), N(VP, "1001"), N(VP, "1002")}}, N{SB, L{N(VP, "1010"), N(VP, "1011"), N(VP, "1012")}}, N{SB, L{N(VP, "1020"), N(VP, "1021"), N(VP, "1022")}}}}, N{SB, L{N{SB, L{N(VP, "1100"), N(VP, "1101"), N(VP, "1102")}}, N{SB, L{N(VP, "1110"), N(VP, "1111"), N(VP, "1112")}}, N{SB, L{N(VP, "1120"), N(VP, "1121"), N(VP, "1122")}}}}, N{SB, L{N{SB, L{N(VP, "1200"), N(VP, "1201"), N(VP, "1202")}}, N{SB, L{N(VP, "1210"), N(VP, "1211"), N(VP, "1212")}}, N{SB, L{N(VP, "1220"), N(VP, "1221"), N(VP, "1222")}}}}}},
      N{SB, L{N{SB, L{N{SB, L{N(VP, "2000"), N(VP, "2001"), N(VP, "2002")}}, N{SB, L{N(VP, "2010"), N(VP, "2011"), N(VP, "2012")}}, N{SB, L{N(VP, "2020"), N(VP, "2021"), N(VP, "2022")}}}}, N{SB, L{N{SB, L{N(VP, "2100"), N(VP, "2101"), N(VP, "2102")}}, N{SB, L{N(VP, "2110"), N(VP, "2111"), N(VP, "2112")}}, N{SB, L{N(VP, "2120"), N(VP, "2121"), N(VP, "2122")}}}}, N{SB, L{N{SB, L{N(VP, "2200"), N(VP, "2201"), N(VP, "2202")}}, N{SB, L{N(VP, "2210"), N(VP, "2211"), N(VP, "2212")}}, N{SB, L{N(VP, "2220"), N(VP, "2221"), N(VP, "2222")}}}}}},
          })
);


ADD_CASE_TO_GROUP("seq of empty maps, one line",
R"([{}, {}, {}])",
N(SFS, L{MFS, MFS, MFS})
);

ADD_CASE_TO_GROUP("seq of maps, one line",
R"([{name: John Smith, age: 33}, {name: Mary Smith, age: 27}])",
N(SFS, L{
      N{MFS, L{N(KP|VP, "name", "John Smith"), N(KP|VP, "age", "33")}},
      N{MFS, L{N(KP|VP, "name", "Mary Smith"), N(KP|VP, "age", "27")}}
  })
);

ADD_CASE_TO_GROUP("seq of maps, implicit seq, explicit maps",
R"(
- {name: John Smith, age: 33}
- {name: Mary Smith, age: 27}
)",
N(SB, L{
      N{MFS, L{N(KP|VP, "name", "John Smith"), N(KP|VP, "age", "33")}},
      N{MFS, L{N(KP|VP, "name", "Mary Smith"), N(KP|VP, "age", "27")}}
  })
);

ADD_CASE_TO_GROUP("seq of maps",
R"(
- name: John Smith
  age: 33
- name: Mary Smith
  age: 27
)",
N(SB,  L{
      N{MB, L{N(KP|VP, "name", "John Smith"), N(KP|VP, "age", "33")}},
      N{MB, L{N(KP|VP, "name", "Mary Smith"), N(KP|VP, "age", "27")}}
  })
);

ADD_CASE_TO_GROUP("seq of maps, next line",
R"(
- 
  name:
    John Smith
  age:
    33
- 
  name: 
    Mary Smith
  age:
    27
)",
N(SB,  L{
      N{MB, L{N(KP|VP, "name", "John Smith"), N(KP|VP, "age", "33")}},
      N{MB, L{N(KP|VP, "name", "Mary Smith"), N(KP|VP, "age", "27")}}
  })
);

ADD_CASE_TO_GROUP("seq of maps, bug #32 ex1",
R"(
- 'a': 1
  b: 2
)",
N(SB,  L{
      N{MB, L{N(KS|VP, "a", "1"), N(KP|VP, "b", "2")}}
  })
);

ADD_CASE_TO_GROUP("seq of maps, bug #32 ex2",
R"(
- a: 1
  b: 2
- b: 2
  'a': 1
- b: 2
  'a': 1
  c: 3
- {'a': 1, b: 2}
)",
N(SB,  L{
      N{MB, L{N(KP|VP, "a", "1"), N(KP|VP, "b", "2")}},
      N{MB, L{N(KP|VP, "b", "2"), N(KS|VP, "a", "1")}},
      N{MB, L{N(KP|VP, "b", "2"), N(KS|VP, "a", "1"), N(KP|VP, "c", "3")}},
      N{MFS, L{N(KS|VP, "a", "1"), N(KP|VP, "b", "2")}},
  })
);

ADD_CASE_TO_GROUP("seq of maps, bug #32 ex3",
R"(
'a': 1
b: 2
b: 2
'a': 1
)",
N(MB, L{
    N(KS|VP, "a", "1"), N(KP|VP, "b", "2"), N(KP|VP, "b", "2"), N(KS|VP, "a", "1"),
})
);


ADD_CASE_TO_GROUP("seq of maps, implicit map in seq",
R"('implicit block key' : [
  'implicit flow key 1' : value1,
  'implicit flow key 2' : value2,
  'implicit flow key 3' : value3,
  'implicit flow key m' : {key1: val1, key2: val2},
  'implicit flow key s' : [val1, val2],
 ])",
N(MB, L{
  N(KS|SFM, "implicit block key", L{
    N(MFS, L{N(KS|VP, "implicit flow key 1", "value1")}),
    N(MFS, L{N(KS|VP, "implicit flow key 2", "value2")}),
    N(MFS, L{N(KS|VP, "implicit flow key 3", "value3")}),
    N(MFS, L{N(KS|MFS, "implicit flow key m", L{N(KP|VP, "key1", "val1"), N(KP|VP, "key2", "val2")})}),
    N(MFS, L{N(KS|SFS, "implicit flow key s", L{N(VP, "val1"), N(VP, "val2")})}),
  })
})
);


ADD_CASE_TO_GROUP("seq of maps, implicit map in seq, missing scalar", NO_COMPARE_EMITTED_INTS_JSON,
R"({a : [
  : foo
],
b : [
  :
 foo
],
c : [
  :
 ,
  :
]})",
N(MFM, L{
  N(KP|SFM, "a", L{N(MFS, L{N(KN|VP, "", "foo")}),}),
  N(KP|SFM, "b", L{N(MFM, L{N(KN|VP, "", "foo")}),}),
  N(KP|SFM, "c", L{N(MFM, L{N(KN|VN, "", {})}), N(MFM, L{N(KN|VN, "", {})}),}),
})
);


ADD_CASE_TO_GROUP("seq of maps, implicit with anchors, unresolved",
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1 : w1
  *a2 : w2
  *a3 : w3
)",
N(SB, L{
  N(MB, L{N(KP|VP,  "a1", AR(KEYANCH, "a1"), "v1"), N(KP|VP,  "a2", AR(KEYANCH, "a2"), "v2"), N(KP|VP,  "a3", AR(KEYANCH, "a3"), "v3")}),
  N(MB, L{N(KEY|VP, "*a1", AR(KEYREF, "*a1"), "w1"), N(KEY|VP, "*a2", AR(KEYREF, "*a2"), "w2"), N(KEY|VP, "*a3", AR(KEYREF, "*a3"), "w3")}),
})
);


ADD_CASE_TO_GROUP("seq of maps, implicit with anchors, resolved", RESOLVE_REFS|NO_COMPARE_EMITTED_INTS,
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1 : w1
  *a2 : w2
  *a3 : w3
)",
N(SB, L{
  N(MB, L{N(KP|VP, "a1", "v1"), N(KP|VP, "a2", "v2"), N(KP|VP, "a3", "v3")}),
  N(MB, L{N(KP|VP, "a1", "w1"), N(KP|VP, "a2", "w2"), N(KP|VP, "a3", "w3")}),
})
);


ADD_CASE_TO_GROUP("seq of maps, implicit with tags",
R"(
- !!str a1: v1
  !!str a2: v2
  !!str a3: v3
- a1: !!str w1
  a2: !!str w2
  a3: !!str w3
- !foo a1: v1
  !foo a2: v2
  !foo a3: v3
)",
N(SB, L{
  N(MB, L{N(KP|VP, TS("!!str", "a1"), "v1"), N(KP|VP, TS("!!str", "a2"), "v2"), N(KP|VP, TS("!!str", "a3"), "v3")}),
  N(MB, L{N(KP|VP, "a1", TS("!!str", "w1")), N(KP|VP, "a2", TS("!!str", "w2")), N(KP|VP, "a3", TS("!!str", "w3"))}),
  N(MB, L{N(KP|VP, TS("!foo", "a1"), "v1"), N(KP|VP, TS("!foo", "a2"), "v2"), N(KP|VP, TS("!foo", "a3"), "v3")}),
})
);

#ifdef RYML_WITH_TAB_TOKENS
ADD_CASE_TO_GROUP("seq of maps, test suite 6BCT", EXPECT_PARSE_ERROR,
R"(
- foo0: bar0
-	 foo1	 :	 bar1	 
- 	foo2 	: 	bar2 	
)",
Location(3, 12)
);
#endif

ADD_CASE_TO_GROUP("github35/expected_error11", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
 MessageID1:          'MapRegion_HyrulePrairie1 '
)",
  Location(4, 1)
);

ADD_CASE_TO_GROUP("github35/expected_error12", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key2: true2
 MessageID2:          "MapRegion_HyrulePrairie2 "
)",
  Location(4, 1)
);

ADD_CASE_TO_GROUP("github35/expected_error21", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
    MessageID1:          'MapRegion_HyrulePrairie1 '
)",
  Location(4, 15)
);

ADD_CASE_TO_GROUP("github35/expected_error22", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key2: true2
    MessageID2:          "MapRegion_HyrulePrairie2 "
)",
  Location(4, 15)
);

} // CASE_GROUP()


} // namespace yml
} // namespace c4
