#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include "c4/error.hpp"

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

csubstr getafter(csubstr yaml, csubstr pattern)
{
    size_t pos = yaml.find(pattern);
    RYML_ASSERT(pos != npos);
    RYML_ASSERT(yaml.sub(pos).begins_with(pattern));
    return yaml.sub(pos + pattern.len);
}

#define _check_null_pointing_at(expr_, which, pattern, arena)      \
    do                                                             \
    {                                                              \
        csubstr expr = expr_.which();                              \
        if(expr.empty())                                           \
        {                                                          \
            EXPECT_EQ(expr, nullptr);                              \
            EXPECT_EQ(expr.len, 0u);                               \
            EXPECT_EQ(expr.str, nullptr);                          \
        }                                                          \
        EXPECT_TRUE(expr_.which##_is_null());                      \
    } while(0)


TEST(null_val, block_seq)
{
    csubstr yaml = R"(
# nospace
-
-
-
# onespace
- 
- 
- 
# null
- null
- null
- null
- ~
)";
    ASSERT_EQ(yaml.count('\r'), 0u);
    Tree tree = parse_in_arena(yaml);
    ASSERT_EQ(tree.rootref().num_children(), 10u);
    // FIXME: empty vals in block seqs are pointing at the next item!
    _check_null_pointing_at(tree[0], val, after("nospace\n-\n"), tree.arena());
    _check_null_pointing_at(tree[1], val, after("nospace\n-\n-\n"), tree.arena());
    _check_null_pointing_at(tree[2], val, after("nospace\n-\n-\n-\n# onespace\n"), tree.arena());
    _check_null_pointing_at(tree[3], val, after("onespace\n- \n"), tree.arena());
    _check_null_pointing_at(tree[4], val, after("onespace\n- \n- \n"), tree.arena());
    _check_null_pointing_at(tree[5], val, after("onespace\n- \n- \n- \n# null\n"), tree.arena());
    // but explicitly null vals are ok:
    _check_null_pointing_at(tree[6], val, "null\n- null\n- null\n- ~\n", tree.arena());
    _check_null_pointing_at(tree[7], val, "null\n- null\n- ~", tree.arena());
    _check_null_pointing_at(tree[8], val, "null\n- ~\n", tree.arena());
    _check_null_pointing_at(tree[9], val, "~\n", tree.arena());
}

TEST(null_val, block_map)
{
    csubstr yaml = R"(
# nospace
val0:
val1:
val2:
# onespace
val3: 
val4: 
val5: 
# null
val6: null
val7: null
val8: null
val9: ~
)";
    ASSERT_EQ(yaml.count('\r'), 0u);
    Tree tree = parse_in_arena(yaml);
    ASSERT_EQ(tree.rootref().num_children(), 10u);
    // FIXME: empty vals in block seqs are pointing at the next item!
    _check_null_pointing_at(tree["val0"], val, after("val0:"), tree.arena());
    _check_null_pointing_at(tree["val1"], val, after("val1:"), tree.arena());
    _check_null_pointing_at(tree["val2"], val, after("val2:\n# onespace"), tree.arena());
    _check_null_pointing_at(tree["val3"], val, after("val3: "), tree.arena());
    _check_null_pointing_at(tree["val4"], val, after("val4: "), tree.arena());
    _check_null_pointing_at(tree["val5"], val, after("val5: \n# null"), tree.arena());
    // but explicitly null vals are ok:
    _check_null_pointing_at(tree["val6"], val, "null\nval7:", tree.arena());
    _check_null_pointing_at(tree["val7"], val, "null\nval8:", tree.arena());
    _check_null_pointing_at(tree["val8"], val, "null\nval9:", tree.arena());
    _check_null_pointing_at(tree["val9"], val, "~\n", tree.arena());
}


TEST(null_val, issue103)
{
    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
    Tree tree;

    tree = parse_in_arena(R"({null: null})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "null");
    EXPECT_EQ(tree.val(1), "null");
    EXPECT_TRUE(tree.key_is_null(1));
    EXPECT_TRUE(tree.val_is_null(1));
    EXPECT_TRUE(tree[0].key_is_null());
    EXPECT_TRUE(tree[0].val_is_null());
    _check_null_pointing_at(tree[0], key, "null:", tree.arena());
    _check_null_pointing_at(tree[0], val, "null}", tree.arena());

    tree = parse_in_arena(R"({Null: Null})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "Null");
    EXPECT_EQ(tree.val(1), "Null");
    EXPECT_TRUE(tree.key_is_null(1));
    EXPECT_TRUE(tree.val_is_null(1));
    EXPECT_TRUE(tree[0].key_is_null());
    EXPECT_TRUE(tree[0].val_is_null());
    _check_null_pointing_at(tree[0], key, "Null:", tree.arena());
    _check_null_pointing_at(tree[0], val, "Null}", tree.arena());

    tree = parse_in_arena(R"({NULL: NULL})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "NULL");
    EXPECT_EQ(tree.val(1), "NULL");
    EXPECT_TRUE(tree.key_is_null(1));
    EXPECT_TRUE(tree.val_is_null(1));
    EXPECT_TRUE(tree[0].key_is_null());
    EXPECT_TRUE(tree[0].val_is_null());
    _check_null_pointing_at(tree[0], key, "NULL:", tree.arena());
    _check_null_pointing_at(tree[0], val, "NULL}", tree.arena());

    tree = parse_in_arena(R"({ : })");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), nullptr);
    EXPECT_EQ(tree.val(1), nullptr);
    EXPECT_TRUE(tree.key_is_null(1));
    EXPECT_TRUE(tree.val_is_null(1));
    EXPECT_TRUE(tree[0].key_is_null());
    EXPECT_TRUE(tree[0].val_is_null());
    _check_null_pointing_at(tree[0], key, ": }", tree.arena());
    _check_null_pointing_at(tree[0], val, " }", tree.arena());

    tree = parse_in_arena(R"({~: ~})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "~");
    EXPECT_EQ(tree.val(1), "~");
    EXPECT_TRUE(tree.key_is_null(1));
    EXPECT_TRUE(tree.val_is_null(1));
    EXPECT_TRUE(tree[0].key_is_null());
    EXPECT_TRUE(tree[0].val_is_null());
    _check_null_pointing_at(tree[0], key, "~:", tree.arena());
    _check_null_pointing_at(tree[0], val, "~}", tree.arena());

    tree = parse_in_arena(R"({"~": "~"})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "~");
    EXPECT_EQ(tree.val(1), "~");
    EXPECT_NE(tree.key(1), nullptr);
    EXPECT_NE(tree.val(1), nullptr);
    EXPECT_FALSE(tree.key_is_null(1));
    EXPECT_FALSE(tree.val_is_null(1));
    EXPECT_FALSE(tree[0].key_is_null());
    EXPECT_FALSE(tree[0].val_is_null());

    tree = parse_in_arena(R"({"null": "null"})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "null");
    EXPECT_EQ(tree.val(1), "null");
    EXPECT_NE(tree.key(1), nullptr);
    EXPECT_NE(tree.val(1), nullptr);
    EXPECT_FALSE(tree[0].key_is_null());
    EXPECT_FALSE(tree[0].val_is_null());

    tree = parse_in_arena(R"({"Null": "Null"})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "Null");
    EXPECT_EQ(tree.val(1), "Null");
    EXPECT_NE(tree.key(1), nullptr);
    EXPECT_NE(tree.val(1), nullptr);
    EXPECT_FALSE(tree.key_is_null(1));
    EXPECT_FALSE(tree.val_is_null(1));
    EXPECT_FALSE(tree[0].key_is_null());
    EXPECT_FALSE(tree[0].val_is_null());

    tree = parse_in_arena(R"({"NULL": "NULL"})");
    ASSERT_EQ(tree.size(), 2u);
    EXPECT_EQ(tree.root_id(), 0u);
    EXPECT_EQ(tree.first_child(0), 1u);
    EXPECT_TRUE(tree.type(1).is_keyval());
    EXPECT_EQ(tree.key(1), "NULL");
    EXPECT_EQ(tree.val(1), "NULL");
    EXPECT_NE(tree.key(1), nullptr);
    EXPECT_NE(tree.val(1), nullptr);
    EXPECT_FALSE(tree.key_is_null(1));
    EXPECT_FALSE(tree.val_is_null(1));
    EXPECT_FALSE(tree[0].key_is_null());
    EXPECT_FALSE(tree[0].val_is_null());

    C4_SUPPRESS_WARNING_GCC_POP
}


TEST(null_val, null_key)
{
    Tree tree = parse_in_arena(R"({null: null})");
    ASSERT_EQ(tree.size(), 2u);
    _check_null_pointing_at(tree[0], key, "null: ", tree.arena());
    _check_null_pointing_at(tree[0], val, "null}", tree.arena());
}


TEST(null_val, readme_example)
{
  csubstr yaml = R"(
seq:
  - ~
  - null
  -
  -
  # a comment
  -
map:
  val0: ~
  val1: null
  val2:
  val3:
  # a comment
  val4:
)";
  Tree t = parse_in_arena("file.yml", yaml);
  // as expected: (len is null, str is pointing at the value where the node starts)
  EXPECT_EQ(t["seq"][0].val(), "~");
  EXPECT_EQ(t["seq"][1].val(), "null");
  EXPECT_EQ(t["seq"][2].val(), nullptr);
  EXPECT_EQ(t["seq"][3].val(), nullptr);
  EXPECT_EQ(t["seq"][4].val(), nullptr);
  EXPECT_EQ(t["map"][0].val(), "~");
  EXPECT_EQ(t["map"][1].val(), "null");
  EXPECT_EQ(t["map"][2].val(), nullptr);
  EXPECT_EQ(t["map"][3].val(), nullptr);
  EXPECT_EQ(t["map"][4].val(), nullptr);
  // standard null values point at the expected location:
  EXPECT_EQ(csubstr(t["seq"][0].val().str, 1), csubstr("~"));
  EXPECT_EQ(csubstr(t["seq"][1].val().str, 4), csubstr("null"));
  EXPECT_EQ(csubstr(t["map"]["val0"].val().str, 1), csubstr("~"));
  EXPECT_EQ(csubstr(t["map"]["val1"].val().str, 4), csubstr("null"));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(NULL_VAL)
{

ADD_CASE_TO_GROUP("all null",
R"(
-
- # with space
- null
- Null
- NULL
- ~
- null: null
- Null: Null
- NULL: NULL
- ~: ~
- ~: null
- null: ~
)",
N(SB, L{
N(VP, nullptr),
N(VP, nullptr),
N(VP, "null"),
N(VP, "Null"),
N(VP, "NULL"),
N(VP, "~"),
N(MB, L{N(KP|VP, "null", "null")}),
N(MB, L{N(KP|VP, "Null", "Null")}),
N(MB, L{N(KP|VP, "NULL", "NULL")}),
N(MB, L{N(KP|VP, "~", "~")}),
N(MB, L{N(KP|VP, "~", "null")}),
N(MB, L{N(KP|VP, "null", "~")}),
})
);

ADD_CASE_TO_GROUP("null map vals, expl",
R"({foo: , bar: , baz: }
)",
N(MFS, L{N(KP|VP, "foo", nullptr), N(KP|VP, "bar", nullptr), N(KP|VP, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
N(MB, L{N(KP|VP, "foo", nullptr), N(KP|VP, "bar", nullptr), N(KP|VP, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals, impl",
R"(- 
- 
- 
)",
N(SB, L{N(VP, nullptr), N(VP, nullptr), N(VP, nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 1",
R"(
foo:
  - 
  - 
  - 
bar: 
baz: 
)",
N(MB, L{N(KP|SB, "foo", L{N(VP, nullptr), N(VP, nullptr), N(VP, nullptr)}), N(KP|VP, "bar", nullptr), N(KP|VP, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 2",
R"(
foo:
bar: 
  - 
  - 
  - 
baz: 
)",
N(MB, L{N(KP|VP, "foo", nullptr), N(KP|SB, "bar", L{N(VP, nullptr), N(VP, nullptr), N(VP, nullptr)}), N(KP|VP, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals in map, impl, mixed 3",
R"(
foo:
bar: 
baz: 
  - 
  - 
  - 
)",
N(MB, L{N(KP|VP, "foo", nullptr), N(KP|VP, "bar", nullptr), N(KP|SB, "baz", L{N(VP, nullptr), N(VP, nullptr), N(VP, nullptr)})})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 1",
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
N(SB, L{
  N(MB, L{
    N(KP|VP, "foo", nullptr),
    N(KP|VP, "bar", nullptr),
    N(KP|VP, "baz", nullptr)
  }),
  N(VP, nullptr),
  N(VP, nullptr)
})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 2",
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
N(SB, L{
  N(VP, nullptr),
  N(MB, L{
    N(KP|VP, "foo", nullptr),
    N(KP|VP, "bar", nullptr),
    N(KP|VP, "baz", nullptr)
  }),
  N(VP, nullptr)
})
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 3",
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
N(SB, L{
  N(VP, nullptr),
  N(VP, nullptr),
  N(MB, L{
    N(KP|VP, "foo", nullptr),
    N(KP|VP, "bar", nullptr),
    N(KP|VP, "baz", nullptr)
  }),
})
);

ADD_CASE_TO_GROUP("issue84.1",
R"(
fixed case:
  foo: a
  bar: 
your case:
  foo: a
  bar: ''
whatever: baz
)",
N(MB, L{
  N(KP|MB, "fixed case", L{N(KP|VP, "foo", "a"), N(KP|VP, "bar", nullptr)}),
  N(KP|MB, "your case", L{N(KP|VP, "foo", "a"), N(KP|VS, "bar", "")}),
  N(KP|VP, "whatever", "baz"),
})
);

ADD_CASE_TO_GROUP("issue84.2",
R"(
version: 0
type: xml
param_root:
  objects:
    System: {SameGroupActorName: '', IsGetItemSelf: false}
    General:
      Speed: 1.0
      Life: 100
      IsLifeInfinite: false
      ElectricalDischarge: 1.0
      IsBurnOutBorn: false
      BurnOutBornName: 
      IsBurnOutBornIdent: false
      ChangeDropTableName: ''
)",
N(MB, L{
  N(KP|VP, "version", "0"),
  N(KP|VP, "type", "xml"),
  N(KP|MB, "param_root", L{
      N(KP|MB, "objects", L{
          N(KP|MFS, "System", L{
              N(KP|VS, "SameGroupActorName", ""),
              N(KP|VP, "IsGetItemSelf", "false")
          }),
          N(KP|MB, "General", L{
              N(KP|VP, "Speed", "1.0"),
              N(KP|VP, "Life", "100"),
              N(KP|VP, "IsLifeInfinite", "false"),
              N(KP|VP, "ElectricalDischarge", "1.0"),
              N(KP|VP, "IsBurnOutBorn", "false"),
              N(KP|VP, "BurnOutBornName", nullptr),
              N(KP|VP, "IsBurnOutBornIdent", "false"),
              N(KP|VS, "ChangeDropTableName", ""),
          }),
      })
  }),
})
);

ADD_CASE_TO_GROUP("issue84.3",
R"(
version: 10
type: test
param_root:
  objects:
    TestContent:
      Str64_empty: ''
      Str64_empty2:
      Str64_empty3: ''
  lists: {}
)",
N(MB, L{
  N(KP|VP, "version", "10"),
  N(KP|VP, "type", "test"),
  N(KP|MB, "param_root", L{
      N(KP|MB, "objects", L{
          N(KP|MB, "TestContent", L{
              N(KP|VS, "Str64_empty", ""),
              N(KP|VP, "Str64_empty2", nullptr),
              N(KP|VS, "Str64_empty3", ""),
          }),
      }),
      N(KP|MFS, "lists", L{})
  }),
})
);

}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP
