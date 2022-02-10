#include "./test_group.hpp"
#include "test_case.hpp"

namespace c4 {
namespace yml {

TEST(seq_of_map, with_anchors)
{
    {
        // this case is vanilla:
        csubstr yaml = R"(- a0: v0
  &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- a0: w0
  *a1: w1
  *a2: w2
  *a3: w3
- &seq
  a4: v4
)";
        Tree t = parse_in_arena(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
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
- *a1: w1
  *a2: w2
  *a3: w3
)";
        Tree t = parse_in_arena(yaml);
        EXPECT_EQ(emitrs<std::string>(t), yaml);
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
        EXPECT_EQ(emitrs<std::string>(t), yaml);
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
        EXPECT_EQ(emitrs<std::string>(t), yaml);
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

TEST(seq_of_map, missing_scalars_v2)
{
    Tree t = parse_in_arena(R"(a:
  - : 
  - : 
)");
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    ASSERT_EQ(t["a"].num_children(), 2u);
    ASSERT_EQ(t["a"][0].num_children(), 1u);
    EXPECT_EQ(t["a"][0].first_child().key(), nullptr);
    EXPECT_EQ(t["a"][0].first_child().val(), nullptr);
    ASSERT_EQ(t["a"][1].num_children(), 1u);
    EXPECT_EQ(t["a"][1].first_child().key(), nullptr);
    EXPECT_EQ(t["a"][1].first_child().val(), nullptr);
}

TEST(seq_of_map, missing_scalars_v3)
{
    Tree t = parse_in_arena(R"(a:
  - :
  - :
)");
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    ASSERT_EQ(t["a"].num_children(), 2u);
    ASSERT_EQ(t["a"][0].num_children(), 1u);
    EXPECT_EQ(t["a"][0].first_child().key(), nullptr);
    EXPECT_EQ(t["a"][0].first_child().val(), nullptr);
    ASSERT_EQ(t["a"][1].num_children(), 1u);
    EXPECT_EQ(t["a"][1].first_child().key(), nullptr);
    EXPECT_EQ(t["a"][1].first_child().val(), nullptr);
}

#ifdef RYML_WITH_TAB_TOKENS
TEST(seq_of_map, test_suite_6BCT)
{
    Tree t = parse_in_arena(R"(
- foo0: bar0
-	 foo1	 :	 bar1	 
- 	foo2 	: 	bar2 	
)");
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    ASSERT_TRUE(t[0].is_map());
    ASSERT_TRUE(t[1].is_map());
    ASSERT_TRUE(t[2].is_map());
    EXPECT_EQ(t[0]["foo0"].val(), csubstr("bar0"));
    EXPECT_EQ(t[1]["foo1"].val(), csubstr("bar1"));
    EXPECT_EQ(t[2]["foo2"].val(), csubstr("bar2"));
}
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(SEQ_OF_MAP)
{

ADD_CASE_TO_GROUP("seq of empty maps, one line",
R"([{}, {}, {}])",
  L{MAP, MAP, MAP}
);

ADD_CASE_TO_GROUP("seq of maps, one line",
R"([{name: John Smith, age: 33}, {name: Mary Smith, age: 27}])",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
);

ADD_CASE_TO_GROUP("seq of maps, implicit seq, explicit maps",
R"(
- {name: John Smith, age: 33}
- {name: Mary Smith, age: 27}
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
);

ADD_CASE_TO_GROUP("seq of maps",
R"(
- name: John Smith
  age: 33
- name: Mary Smith
  age: 27
)",
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
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
  L{
      N{L{N("name", "John Smith"), N("age", "33")}},
      N{L{N("name", "Mary Smith"), N("age", "27")}}
  }
);

ADD_CASE_TO_GROUP("seq of maps, bug #32 ex1",
R"(
- 'a': 1
  b: 2
)",
  L{
      N{L{N(QK, "a", "1"), N("b", "2")}}
  }
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
  L{
      N{L{N("a", "1"), N("b", "2")}},
      N{L{N("b", "2"), N(QK, "a", "1")}},
      N{L{N("b", "2"), N(QK, "a", "1"), N("c", "3")}},
      N{L{N(QK, "a", "1"), N("b", "2")}},
  }
);

ADD_CASE_TO_GROUP("seq of maps, bug #32 ex3",
R"(
'a': 1
b: 2
b: 2
'a': 1
)",
L{
    N(QK, "a", "1"), N("b", "2"), N("b", "2"), N(QK, "a", "1"),
});


ADD_CASE_TO_GROUP("seq of maps, implicit map in seq",
R"('implicit block key' : [
  'implicit flow key 1' : value1,
  'implicit flow key 2' : value2,
  'implicit flow key 3' : value3,
  'implicit flow key m' : {key1: val1, key2: val2},
  'implicit flow key s' : [val1, val2],
])",
L{N(KEYSEQ|KEYQUO, "implicit block key", L{
  N(L{N(KEYVAL|KEYQUO, "implicit flow key 1", "value1")}),
  N(L{N(KEYVAL|KEYQUO, "implicit flow key 2", "value2")}),
  N(L{N(KEYVAL|KEYQUO, "implicit flow key 3", "value3")}),
  N(L{N(KEYMAP|KEYQUO, "implicit flow key m", L{N("key1", "val1"), N("key2", "val2")})}),
  N(L{N(KEYSEQ|KEYQUO, "implicit flow key s", L{N("val1"), N("val2")})}),
})});


ADD_CASE_TO_GROUP("seq of maps, implicit map in seq, missing scalar",
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
L{
  N("a", L{N(MAP, L{N("", "foo")}),}),
  N("b", L{N(MAP, L{N("", "foo")}),}),
  N("c", L{N(MAP, L{N(KEYVAL, "", {})}), N(MAP, L{N(KEYVAL, "", {})}),}),
});


ADD_CASE_TO_GROUP("seq of maps, implicit with anchors, unresolved",
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1: w1
  *a2: w2
  *a3: w3
)",
L{
  N(L{N( "a1", AR(KEYANCH, "a1"), "v1"), N( "a2", AR(KEYANCH, "a2"), "v2"), N( "a3", AR(KEYANCH, "a3"), "v3")}),
  N(L{N("*a1", AR(KEYREF, "*a1"), "w1"), N("*a2", AR(KEYREF, "*a2"), "w2"), N("*a3", AR(KEYREF, "*a3"), "w3")}),
});


ADD_CASE_TO_GROUP("seq of maps, implicit with anchors, resolved", RESOLVE_REFS,
R"(
- &a1 a1: v1
  &a2 a2: v2
  &a3 a3: v3
- *a1: w1
  *a2: w2
  *a3: w3
)",
L{
  N(L{N("a1", "v1"), N("a2", "v2"), N("a3", "v3")}),
  N(L{N("a1", "w1"), N("a2", "w2"), N("a3", "w3")}),
});


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
L{
  N(L{N(TS("!!str", "a1"), "v1"), N(TS("!!str", "a2"), "v2"), N(TS("!!str", "a3"), "v3")}),
  N(L{N("a1", TS("!!str", "w1")), N("a2", TS("!!str", "w2")), N("a3", TS("!!str", "w3"))}),
  N(L{N(TS("!foo", "a1"), "v1"), N(TS("!foo", "a2"), "v2"), N(TS("!foo", "a3"), "v3")}),
});
}

} // namespace yml
} // namespace c4
