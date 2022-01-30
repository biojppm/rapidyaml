#include "./test_group.hpp"
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
            EXPECT_NE(expr.str, nullptr);                          \
        }                                                          \
        EXPECT_TRUE(expr_.which##_is_null());                      \
        EXPECT_LT(expr.str, arena.end());                          \
        ASSERT_GE(expr.str, arena.begin());                        \
        size_t exprpos = (size_t)(expr.str - arena.begin());       \
        EXPECT_TRUE(arena.sub(exprpos).begins_with(pattern));      \
        ASSERT_GE(arena.sub(exprpos).len, csubstr(pattern).len);   \
        EXPECT_EQ(arena.sub(exprpos).first(csubstr(pattern).len), csubstr(pattern)); \
    } while(0)


TEST(null_val, simple)
{
    Tree tree = parse_in_arena("{foo: , bar: '', baz: [,,,], bat: [ , , , ], two: [,,], one: [,], empty: []}");
    _check_null_pointing_at(tree["foo"], val, " ,", tree.arena());
    ASSERT_EQ(tree["baz"].num_children(), 3u);
    _check_null_pointing_at(tree["baz"][0], val, "[,,,]", tree.arena());
    _check_null_pointing_at(tree["baz"][1], val, ",,,]", tree.arena());
    _check_null_pointing_at(tree["baz"][2], val, ",,]", tree.arena());
    ASSERT_EQ(tree["bat"].num_children(), 3u);
    _check_null_pointing_at(tree["bat"][0], val, " , , , ]", tree.arena());
    _check_null_pointing_at(tree["bat"][1], val, " , , ]", tree.arena());
    _check_null_pointing_at(tree["bat"][2], val, " , ]", tree.arena());
    ASSERT_EQ(tree["two"].num_children(), 2u);
    _check_null_pointing_at(tree["two"][0], val, "[,,]", tree.arena());
    _check_null_pointing_at(tree["two"][1], val, ",,]", tree.arena());
    ASSERT_EQ(tree["one"].num_children(), 1u);
    _check_null_pointing_at(tree["one"][0], val, "[,]", tree.arena());
    ASSERT_EQ(tree["empty"].num_children(), 0u);
}

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
    auto after = [yaml](csubstr pattern){ return getafter(yaml, pattern); };
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
    auto after = [yaml](csubstr pattern){ return getafter(yaml, pattern); };
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
    auto tree = parse_in_arena(R"({null: null})");

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
  Parser p;
  Tree t = p.parse_in_arena("file.yml", yaml);
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
  // but empty null values currently point at the NEXT location:
  EXPECT_EQ(csubstr(t["seq"][2].val().str, 15), csubstr("-\n  # a comment"));
  EXPECT_EQ(csubstr(t["seq"][3].val().str, 6), csubstr("-\nmap:"));
  EXPECT_EQ(csubstr(t["seq"][4].val().str, 5), csubstr("\nmap:"));
  EXPECT_EQ(csubstr(t["map"]["val2"].val().str, 6), csubstr(" val3:"));
  EXPECT_EQ(csubstr(t["map"]["val3"].val().str, 6), csubstr(" val4:"));
  EXPECT_EQ(csubstr(t["map"]["val4"].val().str, 1), csubstr("val4:\n").sub(5));
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
L{
N(VAL, nullptr),
N(VAL, nullptr),
N(VAL, "null"),
N(VAL, "Null"),
N(VAL, "NULL"),
N(VAL, "~"),
N(MAP, L{N(KEYVAL, "null", "null")}),
N(MAP, L{N(KEYVAL, "Null", "Null")}),
N(MAP, L{N(KEYVAL, "NULL", "NULL")}),
N(MAP, L{N(KEYVAL, "~", "~")}),
N(MAP, L{N(KEYVAL, "~", "null")}),
N(MAP, L{N(KEYVAL, "null", "~")}),
});

ADD_CASE_TO_GROUP("null map vals, expl",
R"({foo: , bar: , baz: }
)",
L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)}
);

ADD_CASE_TO_GROUP("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)}
);

ADD_CASE_TO_GROUP("null seq vals, impl",
R"(- 
- 
- 
)",
L{N(VAL, nullptr), N(VAL, nullptr), N(VAL, nullptr)}
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
L{N("foo", L{N(VAL, nullptr), N(VAL, nullptr), N(VAL, nullptr)}), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)}
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
L{N(KEYVAL, "foo", nullptr), N("bar", L{N(VAL, nullptr), N(VAL, nullptr), N(VAL, nullptr)}), N(KEYVAL, "baz", nullptr)}
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
L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N("baz", L{N(VAL, nullptr), N(VAL, nullptr), N(VAL, nullptr)})}
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 1",
R"(
- foo:
  bar: 
  baz: 
- 
- 
)",
L{N(L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)}), N(VAL, nullptr), N(VAL, nullptr)}
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 2",
R"(
- 
- foo:
  bar: 
  baz: 
- 
)",
L{N(VAL, nullptr), N(L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)}), N(VAL, nullptr)}
);

ADD_CASE_TO_GROUP("null map vals in seq, impl, mixed 3",
R"(
- 
- 
- foo:
  bar: 
  baz: 
)",
L{N(VAL, nullptr), N(VAL, nullptr), N(L{N(KEYVAL, "foo", nullptr), N(KEYVAL, "bar", nullptr), N(KEYVAL, "baz", nullptr)})}
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
L{
N("fixed case", L{N("foo", "a"), N(KEYVAL, "bar", nullptr)}),
N("your case", L{N("foo", "a"), N(QV, "bar", "")}),
N("whatever", "baz"),
});

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
L{
N("version", "0"),
N("type", "xml"),
N("param_root", L{
    N("objects", L{
        N("System", L{
            N(QV, "SameGroupActorName", ""),
            N("IsGetItemSelf", "false")
        }),
        N("General", L{
            N("Speed", "1.0"),
            N("Life", "100"),
            N("IsLifeInfinite", "false"),
            N("ElectricalDischarge", "1.0"),
            N("IsBurnOutBorn", "false"),
            N(KEYVAL, "BurnOutBornName", nullptr),
            N("IsBurnOutBornIdent", "false"),
            N(QV, "ChangeDropTableName", ""),
        }),
    })
}),
});

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
L{
N("version", "10"),
N("type", "test"),
N("param_root", L{
    N("objects", L{
        N("TestContent", L{
            N(QV, "Str64_empty", ""),
            N(KEYVAL, "Str64_empty2", nullptr),
            N(QV, "Str64_empty3", ""),
        }),
    }),
    N(KEYMAP, "lists", L{})
}),
});

}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP
