#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

TEST(simple_seq, bad_seq1)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
        parse_in_arena(R"(
---
[ a, b, c ] ]
)", &tree);
    });
}

TEST(simple_seq, bad_seq2)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
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
        ExpectError::check_error(&tree, [&]{
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
    ExpectError::check_error([]{
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
        ExpectError::check_error(&t, [&]{
            parse_in_arena("[a: ");
        });
    }
    {
        SCOPED_TRACE("no space after");
        Tree t;
        ExpectError::check_error(&t, [&]{
            parse_in_arena("[a:");
        });
    }
}


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
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"},})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, unindented, trailcomma",
R"([
0,
1,
2,
3,
])",
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, comments inline",
R"([
0,  # bla0
1,  # bla1
2,  # bla2
3   # bla3
])",
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
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
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
);

ADD_CASE_TO_GROUP("simple seq, flow, multiline, indented",
R"([
  0,
  1,
  2,
  3
])",
N(SFS, L{N{VP, "0"}, N{VP, "1"}, N{VP, "2"}, N{VP, "3"}})
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

ADD_CASE_TO_GROUP("simple seq, empty elements",
R"(-
-
- 
-  
- # with comments
- # more comments
)",
N(SB, L{VN, VN, VN, VN, VN, VN})
);

ADD_CASE_TO_GROUP("simple seq, empty elements with non-empty first",
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
N(SFS, L{
  N(VP, "a"), N(VP, "b"), N(VD, "c,d"),   N(VS, "e,f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c, d"),  N(VS, "e, f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c , d"), N(VS, "e , f"),
  N(VP, "a"), N(VP, "b"), N(VD, "c ,d"),  N(VS, "e ,f"),
})
);

ADD_CASE_TO_GROUP("simple seq flow, scalars with special chars, colon",
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
N(SFS, L{
   N(VP, ":a"),
   N(VP, ":0"),
   N(VP, "::"),
   N(VP, ":-"),
   N(VP, ":*"),
   N(VP, ":@"),
   N(VP, ":%"),
   N(VP, ":^"),
   N(VP, ":$"),
   N(VP, ":`"),
   N(MFS, L{KN|VN}),
   _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MFS, L{KN|VN}), N(VP, ":\t")),
   N(VP, "x:a"),
   N(VP, "x:0"),
   N(VP, "x::"),
   N(VP, "x:-"),
   N(VP, "x:*"),
   N(VP, "x:@"),
   N(VP, "x:%"),
   N(VP, "x:^"),
   N(VP, "x:$"),
   N(VP, "x:`"),
   N(MFS, L{N(KP|VN, "x", "")}),
   _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MFS, L{N(KP|VN, "x", "")}), N(VP, "x:\t")),
   N(VP, ":z:a"),
   N(VP, ":z:0"),
   N(VP, ":z::"),
   N(VP, ":z:-"),
   N(VP, ":z:*"),
   N(VP, ":z:@"),
   N(VP, ":z:%"),
   N(VP, ":z:^"),
   N(VP, ":z:$"),
   N(VP, ":z:`"),
   N(MFS, L{N(KP|VN, ":z", "")}),
   _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MFS, L{N(KP|VN, ":z", "")}), N(VP, ":z:\t")),
 })
);

ADD_CASE_TO_GROUP("simple seq blck, scalars with special chars, colon",
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
// _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MB, L{KN|VN}),
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
// _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MB, L{N(KP|VN, "x", "")}),
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
// _RYML_WITH_OR_WITHOUT_TAB_TOKENS(N(MB, L{N(KP|VN, ":z", "")}),
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
N(SFS, L{
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
N(SFS, L{
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
N(SFS, L{
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
N(SFS, L{
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
N(SFS, L{
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
N(SFS, L{
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
N(SFS, L{
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

ADD_CASE_TO_GROUP("simple seq flow, missing val, 1", EXPECT_PARSE_ERROR,
R"([,]
)",
  LineCol(1, 2)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 2", EXPECT_PARSE_ERROR,
R"([ ,]
)",
  LineCol(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 3", EXPECT_PARSE_ERROR,
R"([ , ]
)",
  LineCol(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 4", EXPECT_PARSE_ERROR,
R"([ , val]
)",
  LineCol(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, missing val, 5", EXPECT_PARSE_ERROR,
R"([ , , ]
)",
  LineCol(1, 3)
);

ADD_CASE_TO_GROUP("simple seq flow, seqimap, at line end",
R"([a: 

b]
)",
N(SFS, L{N(MFS, L{N(KP|VP, "a", "b")})})
);

}

} // namespace yml
} // namespace c4
