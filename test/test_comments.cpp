#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"


namespace c4 {
namespace yml {

TEST(comment_list, insertion_order)
{
    id_type node = 1;
    Tree orig = parse_in_arena("foo: bar");
    ASSERT_EQ(orig.key(node), "foo");
    ASSERT_EQ(orig.val(node), "bar");
    {
        Tree t = orig;
        ASSERT_EQ(t.comment(node), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        t.set_comment(node, COMM_LK, " leading key");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        t.set_comment(node, COMM_LV, " leading val");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        EXPECT_EQ(t.comment(node, COMM_LK)->m_text, " leading key");
        EXPECT_EQ(t.comment(node, COMM_LV)->m_text, " leading val");
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(# leading key
foo: 
  # leading val
  bar
)");
        t.set_comment(node, COMM_TK, " trailing key");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_NE(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        t.set_comment(node, COMM_TV, " trailing val");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_NE(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_NE(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        EXPECT_EQ(t.comment(node, COMM_LK)->m_text, " leading key");
        EXPECT_EQ(t.comment(node, COMM_LV)->m_text, " leading val");
        EXPECT_EQ(t.comment(node, COMM_TK)->m_text, " trailing key");
        EXPECT_EQ(t.comment(node, COMM_TV)->m_text, " trailing val");
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(# leading key
foo: # trailing key
  # leading val
  bar # trailing val
)");
    }
    {
        Tree t = orig;
        ASSERT_EQ(t.comment(node), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        t.set_comment(node, COMM_LV, " leading val");
        ASSERT_NE(t.comment(node), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_LK), nullptr);
        t.set_comment(node, COMM_LK, " leading key");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        EXPECT_EQ(t.comment(node, COMM_LK)->m_text, " leading key");
        EXPECT_EQ(t.comment(node, COMM_LV)->m_text, " leading val");
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(# leading key
foo: 
  # leading val
  bar
)");
        t.set_comment(node, COMM_TV, " trailing val");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_NE(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        t.set_comment(node, COMM_TK, " trailing key");
        ASSERT_NE(t.comment(node, COMM_LK), nullptr);
        ASSERT_NE(t.comment(node, COMM_TK), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FK), nullptr);
        ASSERT_NE(t.comment(node, COMM_LV), nullptr);
        ASSERT_NE(t.comment(node, COMM_TV), nullptr);
        ASSERT_EQ(t.comment(node, COMM_FV), nullptr);
        EXPECT_EQ(t.comment(node, COMM_LK)->m_text, " leading key");
        EXPECT_EQ(t.comment(node, COMM_LV)->m_text, " leading val");
        EXPECT_EQ(t.comment(node, COMM_TK)->m_text, " trailing key");
        EXPECT_EQ(t.comment(node, COMM_TV)->m_text, " trailing val");
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(# leading key
foo: # trailing key
  # leading val
  bar # trailing val
)");
    }
}


//-----------------------------------------------------------------------------

TEST(comments, leading_single_line)
{
    csubstr yaml = R"(
# leading comment for foo
foo: 0
# leading comment for bar
bar: 1
# leading comment at the end
)";
    test_check_emit_check(yaml, [](Tree const &t){
        (void)t;
    });
}

TEST(comments, leading_multi_line)
{
    csubstr yaml = R"(
# leading comment for foo
# with several lines
#
# including empty lines
  #
  #
  # not necessary aligned
#
# also not necessary aligned
foo: bar
)";
    test_check_emit_check(yaml, [](Tree const &t){
        (void)t;
    });
}

TEST(comments, trailing_single_line)
{
    csubstr yaml = R"(
foo: 0 # trailing comment for foo
bar: 1 # trailing comment for bar
)";
    test_check_emit_check(yaml, [](Tree const &t){
        (void)t;
    });
}

TEST(comments, trailing_multi_line)
{
    csubstr yaml = R"(
foo: 0 # trailing comment for foo
       # with several lines
       #
       # including empty lines
         #
         #
         # not necessary aligned
       #
       # also not necessary aligned
bar: 1 # trailing comment for bar
       # with several lines
       #
       # including empty lines
         #
         #
         # not necessary aligned
       #
       # also not necessary aligned
)";
    test_check_emit_check(yaml, [](Tree const &t){
        (void)t;
    });
}


// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
