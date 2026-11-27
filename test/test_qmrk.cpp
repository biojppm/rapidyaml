#include "./test_lib/test_helpers_tree.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

TEST(qmrk, test_suite_5WE3)
{
    csubstr yaml = R"(
? explicit key # Empty value
? |
  block key
: - one # Explicit compact
  - two # block value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_NE(t.find_child(t.root_id(), "explicit key"), (size_t)NONE);
        ASSERT_NE(t.find_child(t.root_id(), "block key\n"), (size_t)NONE);
        EXPECT_EQ(t["explicit key"].val(), csubstr{});
        EXPECT_TRUE(t["block key\n"].is_seq());
        EXPECT_EQ(t["block key\n"][0].val(), csubstr("one"));
        EXPECT_EQ(t["block key\n"][1].val(), csubstr("two"));
    });
}
TEST(qmrk, test_suite_5WE3_with_leading_space)
{
    csubstr yaml = R"(
? key
:   - one # Explicit compact
    - two # block value
)";
    Tree tree = parse_in_arena(yaml);
    EXPECT_TRUE(tree["key"].is_seq());
    EXPECT_EQ(tree["key"][0].val(), "one");
    EXPECT_EQ(tree["key"][1].val(), "two");
}
TEST(qmrk, test_suite_5WE3_with_leading_tab)
{
    csubstr yaml = R"(
? key
: \t - one # Explicit compact
    - two # block value
)";
    Tree tree;
    ExpectError::check_error_parse(&tree, [&]{
        parse_in_arena(yaml, &tree);
    }, Location(4, 5));
}

TEST(qmrk, test_suite_652Z)
{
    csubstr yaml = R"(
?foo: bar  # not an explicit key in RUNK
?bar: 42   # not an explicit key in RMAP|RKEY
?baz:
   ?bat: 24  # not an explicit key in RMAP|RVAL
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef r = t.rootref();
        ASSERT_TRUE(r.has_child("?foo"));
        ASSERT_TRUE(r.has_child("?bar"));
        ASSERT_TRUE(r.has_child("?baz"));
        EXPECT_EQ(r["?foo"].val(), "bar");
        EXPECT_EQ(r["?bar"].val(), "42");
        ASSERT_TRUE(r["?baz"].has_child("?bat"));
        ASSERT_EQ(r["?baz"]["?bat"].val(), "24");
    });
}


TEST(qmrk, test_suite_DFF7_v1)
{
    csubstr yaml = R"(
{
? explicit: entry,
implicit: entry,
?
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_EQ(t.rootref().num_children(), 3u);
        ASSERT_TRUE(t.rootref().has_child("explicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("entry"));
        ASSERT_TRUE(t.rootref().has_child("implicit"));
        EXPECT_EQ(t["explicit"].val(), csubstr("entry"));
        ASSERT_TRUE(t.rootref().has_child(csubstr{}));
        EXPECT_EQ(t[csubstr{}].val(), csubstr{});
    });
}

TEST(qmrk, test_suite_DFF7_v2)
{
    csubstr yaml = R"(
{
?
  key on next line
:
  val on next line
,
?
  # no key
:
  val on next line
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_EQ(t.rootref().num_children(), 2u);
        ASSERT_TRUE(t.rootref().has_child("key on next line"));
        EXPECT_EQ(t[0].key(), "key on next line");
        EXPECT_EQ(t[0].val(), "val on next line");
        EXPECT_EQ(t[1].key(), csubstr{});
        EXPECT_EQ(t[1].val(), "val on next line");
    });
}


TEST(qmrk, test_suite_FRK4)
{
    csubstr yaml = R"(
{
  ? foo :,
  : bar,
}
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("foo"));
        EXPECT_EQ(t["foo"].val(), csubstr{});
        ASSERT_TRUE(t.rootref().has_child(csubstr{}));
        EXPECT_EQ(t[csubstr{}].val(), csubstr("bar"));
    });
}

} // namespace yml
} // namespace c4
