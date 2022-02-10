#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(simple_seq, bad_seq1)
{
    Tree tree;
    ExpectError::do_check(&tree, [&]{
        parse_in_arena(R"(
---
[ a, b, c ] ]
)", &tree);
    });
}

TEST(simple_seq, bad_seq2)
{
    Tree tree;
    ExpectError::do_check(&tree, [&]{
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
    for(size_t num_brackets : {4u, 8u, 32u})
    {
        SCOPED_TRACE(num_brackets);
        for(size_t i = src.size(); i < num_brackets; ++i)
            src += '[';
        Tree tree;
        Location loc = {};
        loc.line = 1;
        loc.col = num_brackets + 1u;
        ExpectError::do_check(&tree, [&]{
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
        size_t doc = 0;
        EXPECT_TRUE(t.docref(doc)["top1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"][0].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"][1].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"][0].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"][1].is_val_quoted());
        ++doc;
        EXPECT_TRUE(t.docref(doc)["top1"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"].is_key_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"][0].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top1"][1].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"][0].is_val_quoted());
        EXPECT_TRUE(t.docref(doc)["top2"][1].is_val_quoted());
    });
}

TEST(simple_seq, deeply_nested_to_cover_parse_stack_resizes)
{
    csubstr yaml = R"(
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[0, 1, 2, 3, 4, 5, 6, 7]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
)";
    Tree t = parse_in_arena(yaml);
    size_t id = t.root_id();
    while(t.has_children(id))
        id = t.first_child(id);
    ASSERT_TRUE(t.ref(id).has_parent());
    NodeRef seq = t.ref(id).parent();
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
TEST(simple_seq, block_tab_tokens)
{
    Tree tree = parse_in_arena(R"(
--- # block, spaces only
- 0
- 1
- 2
--- # block, tabs after
-	0
-	1
-	2
--- # block, tabs after token, and after val
-	0	
-	1	
-	2	
)");
    EXPECT_EQ(tree.docref(0)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(0)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(0)[2].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(1)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(1)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(1)[2].val(), csubstr("2"));
}

TEST(simple_seq, flow_tab_tokens)
{
    Tree tree = parse_in_arena(R"(
--- # flow, no tabs
[0, 1, 2]
--- # flow, tabs after
[0,	1,	2]
--- # flow, tabs before and after
[0	,	1	,	2]
--- # flow, tabs everywhere
	[	
	0	,	
	1	,	
	2	, 	
	]	
)");
    EXPECT_EQ(tree.docref(0)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(0)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(0)[2].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(1)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(1)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(1)[2].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(2)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(2)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(2)[2].val(), csubstr("2"));
    EXPECT_EQ(tree.docref(3)[0].val(), csubstr("0"));
    EXPECT_EQ(tree.docref(3)[1].val(), csubstr("1"));
    EXPECT_EQ(tree.docref(3)[2].val(), csubstr("2"));
}
#endif // RYML_WITH_TAB_TOKENS


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(SIMPLE_SEQ)
{

ADD_CASE_TO_GROUP("simple seq",
R"(- 0
- 1
- 2
- 3
)",
L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);


ADD_CASE_TO_GROUP("simple seq, explicit, single line",
"[0, 1, 2, 3]",
L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, explicit, single line, trailcomma",
"[0, 1, 2, 3,]",
L{N{"0"}, N{"1"}, N{"2"}, N{"3"},}
);

ADD_CASE_TO_GROUP("simple seq, explicit, multiline, unindented",
R"([
0,
1,
2,
3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, explicit, multiline, unindented, trailcomma",
R"([
0,
1,
2,
3,
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, explicit, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, explicit, multiline, comments prev line",
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
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, explicit, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
);

ADD_CASE_TO_GROUP("simple seq, comments inline",
R"(
- 0   # this is a foo
- 1   # this is a bar
- 2   # this is a bar
- 3   # this is a bar
)",
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
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
    L{N{"0"}, N{"1"}, N{"2"}, N{"3"}}
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
L{N{"a,b"}, N{"c,d"}, N{"e,f"},
  N{"a, b"}, N{"c, d"}, N{"e, f"},
  N{"a , b"}, N{"c , d"}, N{"e , f"},
  N{"a ,b"}, N{"c ,d"}, N{"e ,f"},
    }
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
L{
  N("a:b"), N(QV, "c:d"), N(QV, "e:f"),
  N("a :b"), N(QV, "c :d"), N(QV, "e :f"),
  N(L{N("a", "b")}), N(QV, "c : d"), N(QV, "e : f"),
  N(L{N("a", "b")}), N(QV, "c: d"), N(QV, "e: f"),
    }
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
L{
  N{"a#b"}, N{QV, "a#b"}, N{QV, "a#b"},
  N{"a# b"}, N{QV, "a# b"}, N{QV, "a# b"},
  N{"a"}, N{QV, "a # b"}, N{QV, "a # b"},
  N{"a"}, N{QV, "a #b"}, N{QV, "a #b"},
    }
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
L{
 N{"a-b"},   N{QV, "a-b"},   N{QV, "a-b"},
 N{"a- b"},  N{QV, "a- b"},  N{QV, "a- b"},
 N{"a - b"}, N{QV, "a - b"}, N{QV, "a - b"},
 N{"a -b"},  N{QV, "a -b"},  N{QV, "a -b"},
    }
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
L{
  N{"a{b"},   N{QV, "a{b"},   N{QV, "a{b"},
  N{"a{ b"},  N{QV, "a{ b"},  N{QV, "a{ b"},
  N{"a { b"}, N{QV, "a { b"}, N{QV, "a { b"},
  N{"a {b"},  N{QV, "a {b"},  N{QV, "a {b"},
    }
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
L{
  N{"a}b"},   N{QV, "a}b"},   N{QV, "a}b"},
  N{"a} b"},  N{QV, "a} b"},  N{QV, "a} b"},
  N{"a } b"}, N{QV, "a } b"}, N{QV, "a } b"},
  N{"a }b"},  N{QV, "a }b"},  N{QV, "a }b"},
    }
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
L{
  N{"a[b"},   N{QV, "a[b"},   N{QV, "a[b"},
  N{"a[ b"},  N{QV, "a[ b"},  N{QV, "a[ b"},
  N{"a [ b"}, N{QV, "a [ b"}, N{QV, "a [ b"},
  N{"a [b"},  N{QV, "a [b"},  N{QV, "a [b"},
    }
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
L{
  N{"a]b"},   N{QV, "a]b"},   N{QV, "a]b"},
  N{"a] b"},  N{QV, "a] b"},  N{QV, "a] b"},
  N{"a ] b"}, N{QV, "a ] b"}, N{QV, "a ] b"},
  N{"a ]b"},  N{QV, "a ]b"},  N{QV, "a ]b"},
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, comma",
R"([
 a,b,  "c,d",   'e,f',
 a, b,  "c, d",   'e, f',
 a , b,  "c , d",   'e , f',
 a ,b,  "c ,d",   'e ,f',
])",
L{
  N{"a"}, N("b"), N(QV, "c,d"),   N(QV, "e,f"),
  N{"a"}, N("b"), N(QV, "c, d"),  N(QV, "e, f"),
  N{"a"}, N("b"), N(QV, "c , d"), N(QV, "e , f"),
  N{"a"}, N("b"), N(QV, "c ,d"),  N(QV, "e ,f"),
    }
);

#ifdef RYML_WITH_TAB_TOKENS
#define _ryml_with_or_without_tabs(with, without) with
#else
#define _ryml_with_or_without_tabs(with, without) without
#endif
ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, colon",
R"(
- [[], :@]
- [[], :%]
- [[], :^]
- [[], :$]
- [[], ::]
- [[], :	]
- [[], :`]
)",
L{
   N(L{N(SEQ), N(":@")}),
   N(L{N(SEQ), N(":%")}),
   N(L{N(SEQ), N(":^")}),
   N(L{N(SEQ), N(":$")}),
   N(L{N(SEQ), N("::")}),
   N(L{N(SEQ), _ryml_with_or_without_tabs(N(MAP, L{N("", "")}), N(":	"))}),
   N(L{N(SEQ), N(":`")}),
}
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, colon 2",
R"([
# a:b,  # not legal
  "c:d",   'e:f',
# a: b,  # not legal
  "c: d",   'e: f',
# a : b,  # not legal
  "c : d",   'e : f',
# a :b,  # not legal
  "c :d",   'e :f',
])",
L{/*...not legal...*/
  /*N{"a"}, N("b"),*/ N(QV, "c:d"),   N(QV, "e:f"),
  /*N{"a"}, N("b"),*/ N(QV, "c: d"),  N(QV, "e: f"),
  /*N{"a"}, N("b"),*/ N(QV, "c : d"), N(QV, "e : f"),
  /*N{"a"}, N("b"),*/ N(QV, "c :d"),  N(QV, "e :f"),
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, cardinal",
R"([
 a#b, "c#d",   'e#f',
 a# b, "c# d",   'e# f',
 a # b, "c # d",   'e # f',
, # this is needed because of the comment above
 a #b, "c #d",   'e #f',
])",
L{
  N{"a#b"}, N(QV, "c#d"), N(QV, "e#f"),
  N{"a# b"}, N(QV, "c# d"), N(QV, "e# f"),
  N{"a"},
  N{"a"},
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, dash",
R"([
 a-b, "c-d",   'e-f',
 a- b, "c- d",   'e- f',
 a - b, "c - d",   'e - f',
 a -b, "c -d",   'e -f',
])",
L{
  N{"a-b"},   N(QV, "c-d"),   N(QV, "e-f"),
  N{"a- b"},  N(QV, "c- d"),  N(QV, "e- f"),
  N{"a - b"}, N(QV, "c - d"), N(QV, "e - f"),
  N{"a -b"},  N(QV, "c -d"),  N(QV, "e -f"),
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, left-bracket",
R"([
# a[b,
   "c[d",   'e[f',
# a[ b,
   "c[ d",   'e[ f',
# a [ b,
   "c [ d",   'e [ f',
# a [b,
   "c [d",   'e [f',
])",
L{
  /*N{"a[b"}, */  N(QV, "c[d"),   N(QV, "e[f"),
  /*N{"a[ b"}, */ N(QV, "c[ d"),  N(QV, "e[ f"),
  /*N{"a [ b"},*/ N(QV, "c [ d"), N(QV, "e [ f"),
  /*N{"a [b"}, */ N(QV, "c [d"),  N(QV, "e [f"),
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, right-bracket",
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
L{
  /*N{"a]b"}, */  N(QV, "c]d"),   N(QV, "e]f"),
  /*N{"a] b"}, */ N(QV, "c] d"),  N(QV, "e] f"),
  /*N{"a ] b"},*/ N(QV, "c ] d"), N(QV, "e ] f"),
  /*N{"a ]b"}, */ N(QV, "c ]d"),  N(QV, "e ]f"),
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, left-curly",
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
L{
  /*N{"a{b"}, */  N(QV, "c{d"),   N(QV, "e{f"),
  /*N{"a{ b"}, */ N(QV, "c{ d"),  N(QV, "e{ f"),
  /*N{"a { b"},*/ N(QV, "c { d"), N(QV, "e { f"),
  /*N{"a {b"}, */ N(QV, "c {d"),  N(QV, "e {f"),
    }
);

ADD_CASE_TO_GROUP("simple seq expl, scalars with special chars, right-curly",
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
L{
  /*N{"a}b"}, */  N(QV, "c}d"),   N(QV, "e}f"),
  /*N{"a} b"}, */ N(QV, "c} d"),  N(QV, "e} f"),
  /*N{"a } b"},*/ N(QV, "c } d"), N(QV, "e } f"),
  /*N{"a }b"}, */ N(QV, "c }d"),  N(QV, "e }f"),
    }
);

ADD_CASE_TO_GROUP("simple seq, issue 28",
R"(# was failing on https://github.com/biojppm/rapidyaml/issues/28
enemy:
- actors:
  - {name: Enemy_Bokoblin_Junior, value: 4.0}
  - {name: Enemy_Bokoblin_Middle, value: 16.0}
  - {name: Enemy_Bokoblin_Senior, value: 32.0}
  - {name: Enemy_Bokoblin_Dark, value: 48.0}
  species: BokoblinSeries
enemy2:
- actors:
    - {name: Enemy_Bokoblin_Junior, value: 4.0}
    - {name: Enemy_Bokoblin_Middle, value: 16.0}
    - {name: Enemy_Bokoblin_Senior, value: 32.0}
    - {name: Enemy_Bokoblin_Dark, value: 48.0}
  species: BokoblinSeries
)",
L{
   N("enemy", L{N(L{
     N("actors", L{
       N(L{N("name", "Enemy_Bokoblin_Junior"), N("value", "4.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Middle"), N("value", "16.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Senior"), N("value", "32.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Dark"), N("value", "48.0"),}),
     }),
     N("species", "BokoblinSeries"),
     })
   }),
   N("enemy2", L{N(L{
     N("actors", L{
       N(L{N("name", "Enemy_Bokoblin_Junior"), N("value", "4.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Middle"), N("value", "16.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Senior"), N("value", "32.0"),}),
       N(L{N("name", "Enemy_Bokoblin_Dark"), N("value", "48.0"),}),
     }),
     N("species", "BokoblinSeries"),
     })
   }),
});

ADD_CASE_TO_GROUP("simple seq, invalid character 1", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
}
)",
  LineCol(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 2", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
]
)",
  LineCol(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 3", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
:
)",
  LineCol(2, 1)
);

ADD_CASE_TO_GROUP("simple seq, invalid character 4", EXPECT_PARSE_ERROR,
R"(- 0   # this is a foo
abcdef!
)",
  LineCol(2, 1)
);
}

} // namespace yml
} // namespace c4
