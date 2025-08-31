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
    csubstr yaml = \
R"(flow: {dquoted: "", squoted: '', plain: , enull: null, tilde: ~,}
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
    test_check_emit_check_with_parser(yaml, opts, [](Tree const &t, Parser const& parser){
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
        EXPECT_EQ(fdquoted.location(parser).line, 0u);
        EXPECT_EQ(bdquoted.location(parser).line, 2u);
        EXPECT_EQ(fsquoted.location(parser).line, 0u);
        EXPECT_EQ(bsquoted.location(parser).line, 3u);
        EXPECT_EQ(bliteral.location(parser).line, 4u);
        EXPECT_EQ(bfolded .location(parser).line, 5u);
        EXPECT_EQ(fplain  .location(parser).line, 0u);
        EXPECT_EQ(bplain  .location(parser).line, 6u);
        EXPECT_EQ(fenull  .location(parser).line, 0u);
        EXPECT_EQ(benull  .location(parser).line, 7u);
        EXPECT_EQ(ftilde  .location(parser).line, 0u);
        EXPECT_EQ(btilde  .location(parser).line, 8u);
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
    });
}

TEST(issue480, deserialize_empty_key)
{
    csubstr yaml = R"(flow: {"": dquoted, '': squoted, : plain, null: enull, ~: tilde}
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
    test_check_emit_check_with_parser(yaml, ParserOptions().locations(true), [](Tree const &t, Parser const& parser){
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
        EXPECT_EQ(fdquoted.location(parser).line,  0u);
        EXPECT_EQ(bdquoted.location(parser).line,  2u);
        EXPECT_EQ(fsquoted.location(parser).line,  0u);
        EXPECT_EQ(bsquoted.location(parser).line,  3u);
        EXPECT_EQ(bliteral.location(parser).line,  5u);
        EXPECT_EQ(bfolded .location(parser).line,  7u);
        EXPECT_EQ(fplain  .location(parser).line,  0u);
        EXPECT_EQ(bplain  .location(parser).line,  8u);
        EXPECT_EQ(fenull  .location(parser).line,  0u);
        EXPECT_EQ(benull  .location(parser).line,  9u);
        EXPECT_EQ(ftilde  .location(parser).line,  0u);
        EXPECT_EQ(btilde  .location(parser).line, 10u);
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
    });
}


//-----------------------------------------------------------------------------

TEST(empty_scalar, issue471_val)
{
    csubstr flow = R"({
  a: ,
  b:
})";
    csubstr blck = R"(
a:
b:
)";
    for(csubstr yaml : {flow, blck})
    {
        test_check_emit_check(yaml, [](Tree const& t){
            EXPECT_TRUE(t[0].val_is_null());
            EXPECT_TRUE(t[1].val_is_null());
            ExpectError::check_error(&t, [&] { std::string s; t["a"] >> s; });
            ExpectError::check_error(&t, [&] { int s; t["a"] >> s; });
            ExpectError::check_error(&t, [&] { float s; t["a"] >> s; });
        });
    }
}
TEST(empty_scalar, issue471_key)
{
    csubstr flow = R"({
  : a,
  : b
})";
    csubstr blck = R"(
: a
: b
)";
    for(csubstr yaml : {flow, blck})
    {
        test_check_emit_check(yaml, [](Tree const& t){
            EXPECT_TRUE(t[0].key_is_null());
            EXPECT_TRUE(t[1].key_is_null());
            ExpectError::check_error(&t, [&] { std::string s; t[0] >> key(s); });
            ExpectError::check_error(&t, [&] { int s; t[0] >> key(s); });
            ExpectError::check_error(&t, [&] { float s; t[0] >> key(s); });
        });
    }
}

TEST(empty_scalar, issue259)
{
    csubstr yaml = R"(
{
? explicit key1 : explicit value,
? explicit key2 : , # Explicit empty
? explicit key3,     # Empty value
simple key1 : explicit value,
simple key2 : ,     # Explicit empty
simple key3,         # Empty value
}
)";
    test_check_emit_check(yaml, [](Tree const& t){
        EXPECT_EQ(t["explicit key1"].key(), "explicit key1");
        EXPECT_EQ(t["explicit key1"].val(), "explicit value");
        EXPECT_EQ(t["explicit key2"].key(), "explicit key2");
        EXPECT_EQ(t["explicit key2"].val(), "");
        EXPECT_TRUE(t["explicit key2"].val_is_null());
        EXPECT_EQ(t["explicit key3"].key(), "explicit key3");
        EXPECT_EQ(t["explicit key3"].val(), "");
        EXPECT_TRUE(t["explicit key3"].val_is_null());
        EXPECT_EQ(t["simple key1"].key(), "simple key1");
        EXPECT_EQ(t["simple key1"].val(), "explicit value");
        EXPECT_EQ(t["simple key2"].key(), "simple key2");
        EXPECT_EQ(t["simple key2"].val(), "");
        EXPECT_TRUE(t["simple key2"].val_is_null());
        EXPECT_EQ(t["simple key3"].key(), "simple key3");
        EXPECT_EQ(t["simple key3"].val(), "");
        EXPECT_TRUE(t["simple key3"].val_is_null());
    });
}

// See also:
// https://github.com/biojppm/rapidyaml/issues/263
// https://github.com/biojppm/rapidyaml/pull/264

TEST(empty_scalar, parse_zero_length_strings)
{
    char inp[] = R"(
seq:
  - ""
  - ''
  - >
  - |
map:
  a: ""
  b: ''
  c: >
  d: |
)";
    const Tree tr = parse_in_place(inp);
    EXPECT_TRUE(tr["seq"].has_key());
    EXPECT_TRUE(tr["map"].has_key());
    EXPECT_TRUE(tr["seq"].is_seq());
    EXPECT_TRUE(tr["map"].is_map());
    for(const char *name : {"seq", "map"})
    {
        ConstNodeRef node = tr[to_csubstr(name)];
        ASSERT_EQ(node.num_children(), 4);
        for(const auto &child : node.children())
        {
            EXPECT_TRUE(child.is_val_quoted());
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_NE(child.val().str, nullptr);
            EXPECT_NE(child.val(), nullptr);
            EXPECT_EQ(child.val(), "");
            EXPECT_FALSE(child.val_is_null());
        }
    }
}

void test_empty_squo(ConstNodeRef ch)
{
    SCOPED_TRACE(ch.id());
    EXPECT_NE((ch.type() & VALQUO), 0u);
    EXPECT_NE((ch.type() & VAL_SQUO), 0u);
    EXPECT_TRUE((ch.type() & VAL_SQUO) == VAL_SQUO);
    EXPECT_TRUE(ch.tree()->is_val_quoted(ch.id()));
    EXPECT_TRUE(ch.is_val_quoted());
    EXPECT_FALSE(ch.val_is_null());
    EXPECT_EQ(ch.val().len, 0);
    EXPECT_NE(ch.val().str, nullptr);
    EXPECT_NE(ch.val(), nullptr);
}
TEST(empty_scalar, flow_seq0)
{
    test_check_emit_check("['', '']", [&](Tree const &t){
        EXPECT_TRUE(t.rootref().has_children());
        EXPECT_EQ(t.rootref().num_children(), 2);
        for(ConstNodeRef ch : t.rootref().children())
            test_empty_squo(ch);
    });
}
TEST(empty_scalar, flow_seq1)
{
    test_check_emit_check("['', ]", [&](Tree const &t){
        EXPECT_TRUE(t.rootref().has_children());
        EXPECT_EQ(t.rootref().num_children(), 1);
        for(ConstNodeRef ch : t.rootref().children())
            test_empty_squo(ch);
    });
}

TEST(empty_scalar, parse_empty_strings)
{
    // use multiple empty entries to ensure the parser
    // correctly deals with the several cases
    char inp[] = R"(
seq:
  -
  -
  -
  -
map:
  a:
  b:
  c:
  d:
)";
    const Tree tr = parse_in_place(inp);
    #ifdef RYML_DBG
    print_tree(tr);
    #endif
    for(const char *name : {"seq", "map"})
    {
        SCOPED_TRACE(name);
        ConstNodeRef node = tr[to_csubstr(name)];
        ASSERT_EQ(node.num_children(), 4);
        size_t pos = 0;
        for(const auto &child : node.children())
        {
            SCOPED_TRACE(pos);
            EXPECT_FALSE(child.type().is_val_quoted());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_TRUE(child.val_is_null());
            ++pos;
        }
    }
}

TEST(empty_scalar, std_string)
{
    std::string stdstr;
    csubstr stdss = to_csubstr(stdstr);
    csubstr nullss;
    EXPECT_NE(stdss, nullptr);
    EXPECT_NE(stdss.str, nullptr);
    EXPECT_EQ(stdss.len, 0u);
    EXPECT_EQ(nullss, nullptr);
    EXPECT_EQ(nullss.str, nullptr);
    EXPECT_EQ(nullss.len, 0u);
    Tree tree = parse_in_arena("{ser: {}, eq: {}}");
    tree["ser"]["stdstr"] << stdss;
    tree["ser"]["nullss"] << nullss;
    tree["eq"]["stdstr"] = stdss;
    tree["eq"]["nullss"] = nullss;
    EXPECT_EQ(emitrs_yaml<std::string>(tree),
              "{ser: {stdstr: '',nullss: },eq: {stdstr: '',nullss: }}");
}

TEST(empty_scalar, to_arena)
{
    Tree tr;
    {
        const char *val = "";
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        char buf_[10];
        csubstr serialized = to_chars_sub(buf_, val);
        EXPECT_EQ(serialized.len, 0);
        EXPECT_NE(serialized.str, nullptr);
        EXPECT_NE(serialized, nullptr);
        csubstr r = tr.to_arena("");
        EXPECT_EQ(r.len, 0u);
        EXPECT_NE(r.str, nullptr);
        EXPECT_NE(r, nullptr);
    }
    {
        const char *val = nullptr;
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        char buf_[10];
        csubstr serialized = to_chars_sub(buf_, val);
        EXPECT_EQ(serialized.len, 0);
        EXPECT_NE(serialized.str, nullptr);
        EXPECT_NE(serialized, nullptr);
        csubstr r = tr.to_arena("");
        EXPECT_EQ(r.len, 0u);
        EXPECT_NE(r.str, nullptr);
        EXPECT_NE(r, nullptr);
        r = tr.to_arena(val);
        EXPECT_EQ(r.len, 0u);
        EXPECT_EQ(r.str, nullptr);
        EXPECT_EQ(r, nullptr);
    }
    {
        std::nullptr_t val = nullptr;
        size_t num = to_chars(substr{}, val);
        ASSERT_EQ(num, 0u);
        csubstr r = tr.to_arena(val);
        EXPECT_EQ(r.len, 0u);
        EXPECT_EQ(r.str, nullptr);
        EXPECT_EQ(r, nullptr);
    }
}

TEST(empty_scalar, gcc_error)
{
    Tree tr;
    csubstr nullstr = {};
    ASSERT_EQ(nullstr.str, nullptr);
    ASSERT_EQ(nullstr.len, 0);
    {
        SCOPED_TRACE("serializing with empty arena");
        csubstr result = tr.to_arena(nullstr);
        EXPECT_EQ(result.str, nullptr); // fails!
        EXPECT_EQ(result.len, 0);
    }
    {
        SCOPED_TRACE("serializing with nonempty arena");
        csubstr result = tr.to_arena(nullstr);
        EXPECT_EQ(result.str, nullptr); // fails!
        EXPECT_EQ(result.len, 0);
    }
}

TEST(empty_scalar, build_zero_length_string)
{
    Tree tr;
    NodeRef root = tr.rootref();
    root |= MAP;
    auto addseq = [&root](csubstr name) { NodeRef n = root[name]; n |= SEQ; return n; };

    // try both with nonnull-zero-length and null-zero-length
    std::string stdstr;
    csubstr stdss = to_csubstr(stdstr);
    csubstr empty = csubstr("nonempty").first(0);
    csubstr nullss = {};

    // these are the conditions we wish to cover:
    ASSERT_TRUE(stdss.str != nullptr);
    ASSERT_TRUE(stdss.len == 0u);
    ASSERT_TRUE(empty.str != nullptr);
    ASSERT_TRUE(empty.len == 0u);
    ASSERT_TRUE(nullss.str == nullptr);
    ASSERT_TRUE(nullss.len == 0u);

    // = and << must have exactly the same behavior where nullity is
    // regarded

    {
        NodeRef quoted = addseq("s-quoted");
        {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r = stdss   ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted.append_child(); r << stdss  ; r.set_type(r.type() | VAL_SQUO);}
    }
    {
        NodeRef quoted = addseq("d-quoted");
        {NodeRef r = quoted.append_child(); r = ""      ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << ""     ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r = empty   ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << empty  ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r = stdss   ; r.set_type(r.type() | VAL_DQUO);}
        {NodeRef r = quoted.append_child(); r << stdss  ; r.set_type(r.type() | VAL_DQUO);}
    }
    {
        NodeRef quoted_null = addseq("quoted_null");
        {NodeRef r = quoted_null.append_child(); r = nullss  ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullss ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r = nullptr ; r.set_type(r.type() | VAL_SQUO);}
        {NodeRef r = quoted_null.append_child(); r << nullptr; r.set_type(r.type() | VAL_SQUO);}
    }
    {
        NodeRef non_quoted = addseq("nonquoted");
        non_quoted.append_child() = "";
        non_quoted.append_child() << "";
        non_quoted.append_child() = empty;
        non_quoted.append_child() << empty;
        non_quoted.append_child() = stdss;
        non_quoted.append_child() << stdss;
    }
    {
        NodeRef non_quoted_null = addseq("nonquoted_null");
        non_quoted_null.append_child() = nullss;
        non_quoted_null.append_child() << nullss;
        non_quoted_null.append_child() = nullptr;
        non_quoted_null.append_child() << nullptr;
    }

    // quoted cases will never be null, regardless of the
    // incoming scalar
    auto test_quoted_empty = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        {
            size_t pos = 0;
            for(ConstNodeRef child : node.cchildren())
            {
                SCOPED_TRACE(pos);
                EXPECT_TRUE(child.is_val_quoted());
                EXPECT_EQ(child.val().len, 0u);
                EXPECT_NE(child.val().str, nullptr);
                EXPECT_NE(child.val(), nullptr);
                EXPECT_EQ(child.val(), "");
                EXPECT_FALSE(child.val_is_null());
                pos++;
            }
        }
    };
    auto test_quoted_null = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val_quoted());
            EXPECT_FALSE(child.val_is_null()); // because it's quoted
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val(), "");
            pos++;
        }
    };
    // ... but according to the incoming scalar, non quoted cases may
    // or may not be null
    auto test_non_quoted_empty = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val());
            EXPECT_FALSE(child.val_is_null()); // because it's quoted
            EXPECT_EQ(child.val(), "");
            EXPECT_NE(child.val(), nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_NE(child.val().str, nullptr);
            ++pos;
        }
    };
    auto test_non_quoted_null = [](ConstNodeRef node){
        SCOPED_TRACE(node.key());
        ASSERT_TRUE(node.has_children());
        size_t pos = 0;
        for(ConstNodeRef child : node.cchildren())
        {
            SCOPED_TRACE(pos);
            EXPECT_TRUE(child.is_val());
            EXPECT_EQ(child.val(), "");
            EXPECT_EQ(child.val(), nullptr);
            EXPECT_EQ(child.val().len, 0u);
            EXPECT_EQ(child.val().str, nullptr);
            EXPECT_TRUE(child.val_is_null());
            ++pos;
        }
    };

    std::string yaml = emitrs_yaml<std::string>(tr);
    #ifdef RYML_DBG
    printf("~~~~~\n%.*s~~~~\n", (int)yaml.size(), yaml.c_str());
    print_tree(tr);
    #endif

    {
        SCOPED_TRACE("input tree");
        test_quoted_empty(tr["s-quoted"]);
        test_quoted_empty(tr["d-quoted"]);
        // in the built tree, the values will be quoted and null
        test_quoted_null(tr["quoted_null"]);
        test_non_quoted_empty(tr["nonquoted"]);
        test_non_quoted_null(tr["nonquoted_null"]);
    }

    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        SCOPED_TRACE("output tree");
        test_quoted_empty(t["s-quoted"]);
        test_quoted_empty(t["d-quoted"]);
        // after a roundtrip, they will be nonnull, because the quotes win.
        test_quoted_empty(t["quoted_null"]);
        test_non_quoted_empty(t["nonquoted"]);
        test_non_quoted_null(t["nonquoted_null"]);
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(NULL_VAL)
{

ADD_CASE_TO_GROUP("empty scalar, single quoted",
                  "''",
                  N(VS, "")
);

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
