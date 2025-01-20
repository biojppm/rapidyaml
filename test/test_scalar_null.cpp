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


TEST(issue480, deserialize_empty_val)
{
    csubstr yaml = R"(
flow: {
  dquoted: "",
  squoted: '',
  plain: ,
  enull: null,
  tilde: ~,
}
block:
  dquoted: ""
  squoted: ''
  literal: |
  folded: >
  plain:
  enull: null
  tilde: ~
)";
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, yaml);
    ConstNodeRef fdquoted = t["flow" ][0];
    ConstNodeRef bdquoted = t["block"][0];
    ConstNodeRef fsquoted = t["flow" ][1];
    ConstNodeRef bsquoted = t["block"][1];
    ConstNodeRef bliteral = t["block"][2];
    ConstNodeRef bfolded  = t["block"][3];
    ConstNodeRef fplain   = t["flow" ][2];
    ConstNodeRef bplain   = t["block"][4];
    ConstNodeRef fenull   = t["flow" ][3];
    ConstNodeRef benull   = t["block"][5];
    ConstNodeRef ftilde   = t["flow" ][4];
    ConstNodeRef btilde   = t["block"][6];
    //
    // check also locations because nullity may influence the search
    // for location
    EXPECT_EQ(parser.location(fdquoted).line,  2u);
    EXPECT_EQ(parser.location(bdquoted).line,  9u);
    EXPECT_EQ(parser.location(fsquoted).line,  3u);
    EXPECT_EQ(parser.location(bsquoted).line, 10u);
    EXPECT_EQ(parser.location(bliteral).line, 11u);
    EXPECT_EQ(parser.location(bfolded ).line, 12u);
    EXPECT_EQ(parser.location(fplain  ).line,  4u);
    EXPECT_EQ(parser.location(bplain  ).line, 13u);
    EXPECT_EQ(parser.location(fenull  ).line,  5u);
    EXPECT_EQ(parser.location(benull  ).line, 14u);
    EXPECT_EQ(parser.location(ftilde  ).line,  6u);
    EXPECT_EQ(parser.location(btilde  ).line, 15u);
    //
    EXPECT_TRUE(fdquoted.has_val());
    EXPECT_TRUE(bdquoted.has_val());
    EXPECT_TRUE(fsquoted.has_val());
    EXPECT_TRUE(bsquoted.has_val());
    EXPECT_TRUE(bliteral.has_val());
    EXPECT_TRUE(bfolded .has_val());
    EXPECT_TRUE(fplain  .has_val());
    EXPECT_TRUE(bplain  .has_val());
    EXPECT_TRUE(fenull  .has_val());
    EXPECT_TRUE(benull  .has_val());
    EXPECT_TRUE(ftilde  .has_val());
    EXPECT_TRUE(btilde  .has_val());
    //
    EXPECT_FALSE(fdquoted.val_is_null());
    EXPECT_FALSE(bdquoted.val_is_null());
    EXPECT_FALSE(fsquoted.val_is_null());
    EXPECT_FALSE(bsquoted.val_is_null());
    EXPECT_FALSE(bliteral.val_is_null());
    EXPECT_FALSE(bfolded .val_is_null());
    EXPECT_TRUE (fplain  .val_is_null());
    EXPECT_TRUE (bplain  .val_is_null());
    EXPECT_TRUE (fenull  .val_is_null());
    EXPECT_TRUE (benull  .val_is_null());
    EXPECT_TRUE (ftilde  .val_is_null());
    EXPECT_TRUE (btilde  .val_is_null());
    //
    EXPECT_EQ(fdquoted, "");
    EXPECT_EQ(bdquoted, "");
    EXPECT_EQ(fsquoted, "");
    EXPECT_EQ(bsquoted, "");
    EXPECT_EQ(bliteral, "");
    EXPECT_EQ(bfolded , "");
    EXPECT_EQ(fplain  , "");
    EXPECT_EQ(bplain  , "");
    EXPECT_EQ(fenull  , "null");
    EXPECT_EQ(benull  , "null");
    EXPECT_EQ(ftilde  , "~");
    EXPECT_EQ(btilde  , "~");
    //
    EXPECT_NE(fdquoted.val().str, nullptr);
    EXPECT_NE(bdquoted.val().str, nullptr);
    EXPECT_NE(fsquoted.val().str, nullptr);
    EXPECT_NE(bsquoted.val().str, nullptr);
    EXPECT_NE(bliteral.val().str, nullptr);
    EXPECT_NE(bfolded .val().str, nullptr);
    EXPECT_EQ(fplain  .val().str, nullptr);
    EXPECT_EQ(bplain  .val().str, nullptr);
    EXPECT_NE(fenull  .val().str, nullptr);
    EXPECT_NE(benull  .val().str, nullptr);
    EXPECT_NE(ftilde  .val().str, nullptr);
    EXPECT_NE(btilde  .val().str, nullptr);
    //
    EXPECT_EQ(fdquoted.val().len, 0);
    EXPECT_EQ(bdquoted.val().len, 0);
    EXPECT_EQ(fsquoted.val().len, 0);
    EXPECT_EQ(bsquoted.val().len, 0);
    EXPECT_EQ(bliteral.val().len, 0);
    EXPECT_EQ(bfolded .val().len, 0);
    EXPECT_EQ(fplain  .val().len, 0);
    EXPECT_EQ(bplain  .val().len, 0);
    EXPECT_EQ(fenull  .val().len, 4);
    EXPECT_EQ(benull  .val().len, 4);
    EXPECT_EQ(ftilde  .val().len, 1);
    EXPECT_EQ(btilde  .val().len, 1);
    //
    EXPECT_EQ(fdquoted, csubstr{});
    EXPECT_EQ(bdquoted, csubstr{});
    EXPECT_EQ(fsquoted, csubstr{});
    EXPECT_EQ(bsquoted, csubstr{});
    EXPECT_EQ(bliteral, csubstr{});
    EXPECT_EQ(bfolded , csubstr{});
    EXPECT_EQ(fplain  , csubstr{});
    EXPECT_EQ(bplain  , csubstr{});
    EXPECT_NE(fenull  , csubstr{});
    EXPECT_NE(benull  , csubstr{});
    EXPECT_NE(ftilde  , csubstr{});
    EXPECT_NE(btilde  , csubstr{});
    //
    EXPECT_NE(fdquoted, nullptr);
    EXPECT_NE(bdquoted, nullptr);
    EXPECT_NE(fsquoted, nullptr);
    EXPECT_NE(bsquoted, nullptr);
    EXPECT_NE(bliteral, nullptr);
    EXPECT_NE(bfolded , nullptr);
    EXPECT_NE(fplain  , nullptr);
    EXPECT_NE(bplain  , nullptr);
    EXPECT_NE(fenull  , nullptr);
    EXPECT_NE(benull  , nullptr);
    EXPECT_NE(ftilde  , nullptr);
    EXPECT_NE(btilde  , nullptr);
    //
    std::string s;
    s = "asd"; fdquoted >> s; EXPECT_EQ(s, "");
    s = "asd"; bdquoted >> s; EXPECT_EQ(s, "");
    s = "asd"; fsquoted >> s; EXPECT_EQ(s, "");
    s = "asd"; bsquoted >> s; EXPECT_EQ(s, "");
    s = "asd"; bliteral >> s; EXPECT_EQ(s, "");
    s = "asd"; bfolded  >> s; EXPECT_EQ(s, "");
    s = "asd"; ExpectError::check_error(&t, [&]{ fplain >> s; });
    s = "asd"; ExpectError::check_error(&t, [&]{ bplain >> s; });
    s = "asd"; fenull >> s; EXPECT_EQ(s, "null");
    s = "asd"; benull >> s; EXPECT_EQ(s, "null");
    s = "asd"; ftilde >> s; EXPECT_EQ(s, "~");
    s = "asd"; btilde >> s; EXPECT_EQ(s, "~");
    // check error also for integral and float types
    ExpectError::check_error(&t, [&]{ int   val = 0; fplain >> val; });
    ExpectError::check_error(&t, [&]{ int   val = 0; bplain >> val; });
    ExpectError::check_error(&t, [&]{ float val = 0; fplain >> val; });
    ExpectError::check_error(&t, [&]{ float val = 0; bplain >> val; });
}

TEST(issue480, deserialize_empty_key)
{
    csubstr yaml = R"(
flow: {
  "": dquoted,
  '': squoted,
  : plain,
  null: enull,
  ~: tilde,
}
block:
  "": dquoted
  '': squoted
  ? |
  : literal
  ? >
  : folded
  : plain
  null: enull
  ~: tilde
)";
    ParserOptions opts = ParserOptions().locations(true);
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    Tree t = parse_in_arena(&parser, yaml);
    ConstNodeRef fdquoted = t["flow" ][0];
    ConstNodeRef bdquoted = t["block"][0];
    ConstNodeRef fsquoted = t["flow" ][1];
    ConstNodeRef bsquoted = t["block"][1];
    ConstNodeRef bliteral = t["block"][2];
    ConstNodeRef bfolded  = t["block"][3];
    ConstNodeRef fplain   = t["flow" ][2];
    ConstNodeRef bplain   = t["block"][4];
    ConstNodeRef fenull   = t["flow" ][3];
    ConstNodeRef benull   = t["block"][5];
    ConstNodeRef ftilde   = t["flow" ][4];
    ConstNodeRef btilde   = t["block"][6];
    //
    // check also locations because nullity may influence the search
    // for location
    EXPECT_EQ(parser.location(fdquoted).line,  2u);
    EXPECT_EQ(parser.location(bdquoted).line,  9u);
    EXPECT_EQ(parser.location(fsquoted).line,  3u);
    EXPECT_EQ(parser.location(bsquoted).line, 10u);
    EXPECT_EQ(parser.location(bliteral).line, 12u);
    EXPECT_EQ(parser.location(bfolded ).line, 14u);
    EXPECT_EQ(parser.location(fplain  ).line,  4u);
    EXPECT_EQ(parser.location(bplain  ).line, 15u);
    EXPECT_EQ(parser.location(fenull  ).line,  5u);
    EXPECT_EQ(parser.location(benull  ).line, 16u);
    EXPECT_EQ(parser.location(ftilde  ).line,  6u);
    EXPECT_EQ(parser.location(btilde  ).line, 17u);
    //
    EXPECT_TRUE(fdquoted.has_key());
    EXPECT_TRUE(bdquoted.has_key());
    EXPECT_TRUE(fsquoted.has_key());
    EXPECT_TRUE(bsquoted.has_key());
    EXPECT_TRUE(bliteral.has_key());
    EXPECT_TRUE(bfolded .has_key());
    EXPECT_TRUE(fplain  .has_key());
    EXPECT_TRUE(bplain  .has_key());
    EXPECT_TRUE(fenull  .has_key());
    EXPECT_TRUE(benull  .has_key());
    EXPECT_TRUE(ftilde  .has_key());
    EXPECT_TRUE(btilde  .has_key());
    //
    EXPECT_FALSE(fdquoted.key_is_null());
    EXPECT_FALSE(bdquoted.key_is_null());
    EXPECT_FALSE(fsquoted.key_is_null());
    EXPECT_FALSE(bsquoted.key_is_null());
    EXPECT_FALSE(bliteral.key_is_null());
    EXPECT_FALSE(bfolded .key_is_null());
    EXPECT_TRUE (fplain  .key_is_null());
    EXPECT_TRUE (bplain  .key_is_null());
    EXPECT_TRUE (fenull  .key_is_null());
    EXPECT_TRUE (benull  .key_is_null());
    EXPECT_TRUE (ftilde  .key_is_null());
    EXPECT_TRUE (btilde  .key_is_null());
    //
    EXPECT_EQ(fdquoted.key(), "");
    EXPECT_EQ(bdquoted.key(), "");
    EXPECT_EQ(fsquoted.key(), "");
    EXPECT_EQ(bsquoted.key(), "");
    EXPECT_EQ(bliteral.key(), "");
    EXPECT_EQ(bfolded.key() , "");
    EXPECT_EQ(fplain.key()  , "");
    EXPECT_EQ(bplain.key()  , "");
    EXPECT_EQ(fenull.key()  , "null");
    EXPECT_EQ(benull.key()  , "null");
    EXPECT_EQ(ftilde.key()  , "~");
    EXPECT_EQ(btilde.key()  , "~");
    //
    EXPECT_NE(fdquoted.key().str, nullptr);
    EXPECT_NE(bdquoted.key().str, nullptr);
    EXPECT_NE(fsquoted.key().str, nullptr);
    EXPECT_NE(bsquoted.key().str, nullptr);
    EXPECT_NE(bliteral.key().str, nullptr);
    EXPECT_NE(bfolded .key().str, nullptr);
    EXPECT_EQ(fplain  .key().str, nullptr);
    EXPECT_EQ(bplain  .key().str, nullptr);
    EXPECT_NE(fenull  .key().str, nullptr);
    EXPECT_NE(benull  .key().str, nullptr);
    EXPECT_NE(ftilde  .key().str, nullptr);
    EXPECT_NE(btilde  .key().str, nullptr);
    //
    EXPECT_EQ(fdquoted.key().len, 0);
    EXPECT_EQ(bdquoted.key().len, 0);
    EXPECT_EQ(fsquoted.key().len, 0);
    EXPECT_EQ(bsquoted.key().len, 0);
    EXPECT_EQ(bliteral.key().len, 0);
    EXPECT_EQ(bfolded .key().len, 0);
    EXPECT_EQ(fplain  .key().len, 0);
    EXPECT_EQ(bplain  .key().len, 0);
    EXPECT_EQ(fenull  .key().len, 4);
    EXPECT_EQ(benull  .key().len, 4);
    EXPECT_EQ(ftilde  .key().len, 1);
    EXPECT_EQ(btilde  .key().len, 1);
    //
    EXPECT_EQ(fdquoted.key(), csubstr{});
    EXPECT_EQ(bdquoted.key(), csubstr{});
    EXPECT_EQ(fsquoted.key(), csubstr{});
    EXPECT_EQ(bsquoted.key(), csubstr{});
    EXPECT_EQ(bliteral.key(), csubstr{});
    EXPECT_EQ(bfolded.key() , csubstr{});
    EXPECT_EQ(fplain.key()  , csubstr{});
    EXPECT_EQ(bplain.key()  , csubstr{});
    EXPECT_NE(fenull.key()  , csubstr{});
    EXPECT_NE(benull.key()  , csubstr{});
    EXPECT_NE(ftilde.key()  , csubstr{});
    EXPECT_NE(btilde.key()  , csubstr{});
    //
    std::string s;
    s = "asd"; fdquoted >> key(s); EXPECT_EQ(s, "");
    s = "asd"; bdquoted >> key(s); EXPECT_EQ(s, "");
    s = "asd"; fsquoted >> key(s); EXPECT_EQ(s, "");
    s = "asd"; bsquoted >> key(s); EXPECT_EQ(s, "");
    s = "asd"; bliteral >> key(s); EXPECT_EQ(s, "");
    s = "asd"; bfolded  >> key(s); EXPECT_EQ(s, "");
    s = "asd"; ExpectError::check_error(&t, [&]{ fplain >> key(s); });
    s = "asd"; ExpectError::check_error(&t, [&]{ bplain >> key(s); });
    s = "asd"; fenull >> key(s); EXPECT_EQ(s, "null");
    s = "asd"; benull >> key(s); EXPECT_EQ(s, "null");
    s = "asd"; ftilde >> key(s); EXPECT_EQ(s, "~");
    s = "asd"; btilde >> key(s); EXPECT_EQ(s, "~");
    // check error also for integral and float types
    ExpectError::check_error(&t, [&]{ int   k = 0; fplain >> key(k); });
    ExpectError::check_error(&t, [&]{ int   k = 0; bplain >> key(k); });
    ExpectError::check_error(&t, [&]{ float k = 0; fplain >> key(k); });
    ExpectError::check_error(&t, [&]{ float k = 0; bplain >> key(k); });
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
N(VN, nullptr),
N(VN, nullptr),
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
N(MFS, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null map vals, impl",
R"(
foo: 
bar: 
baz: 
)",
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
);

ADD_CASE_TO_GROUP("null seq vals, impl",
R"(- 
- 
- 
)",
N(SB, L{N(VN, nullptr), N(VN, nullptr), N(VN, nullptr)})
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
N(MB, L{N(KP|SB, "foo", L{VN, VN, VN}), N(KP|VN, "bar", nullptr), N(KP|VN, "baz", nullptr)})
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
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|SB, "bar", L{VN, VN, VN}), N(KP|VN, "baz", nullptr)})
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
N(MB, L{N(KP|VN, "foo", nullptr), N(KP|VN, "bar", nullptr), N(KP|SB, "baz", L{VN, VN, VN})})
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
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
  }),
  VN,
  VN
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
  VN,
  N(MB, L{
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
  }),
  VN
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
  VN,
  VN,
  N(MB, L{
    N(KP|VN, "foo", nullptr),
    N(KP|VN, "bar", nullptr),
    N(KP|VN, "baz", nullptr)
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
  N(KP|MB, "fixed case", L{N(KP|VP, "foo", "a"), N(KP|VN, "bar", nullptr)}),
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
              N(KP|VN, "BurnOutBornName", nullptr),
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
              N(KP|VN, "Str64_empty2", nullptr),
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
