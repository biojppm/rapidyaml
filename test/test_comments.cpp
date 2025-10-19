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
    #ifdef RYML_WITH_COMMENTS
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
    #endif
}

#define verify_comment(t, node_id, comm_id, comm_ty, prev_comm_id, next_comm_id, txt) \
    do {                                                                \
        if(comm_id != NONE)                                             \
        {                                                               \
            ASSERT_LT(comm_id, t.m_comments_size);                      \
            CommentData const* comm = &t.m_comments_buf[comm_id];       \
            EXPECT_EQ(comm->m_type, comm_ty);                           \
            EXPECT_EQ(comm->m_prev, prev_comm_id);                      \
            EXPECT_EQ(comm->m_next, next_comm_id);                      \
            EXPECT_EQ(comm->m_text, txt);                               \
        }                                                               \
    } while(0)

#define _check_comment_invariants(t, ...)                               \
    do { SCOPED_TRACE(__VA_ARGS__); test_comment_invariants(t); } while(0)

TEST(comment_list, insertion_order_2)
{
#ifdef RYML_WITH_COMMENTS
    const Tree orig = parse_in_arena("{}");
    id_type node = 0;
    EXPECT_EQ(orig._p(node)->m_first_comment, NONE);
    EXPECT_EQ(orig._p(node)->m_last_comment, NONE);
    verify_comment(orig, node, NONE, COMM_NONE, NONE, NONE, "");

    {
        SCOPED_TRACE("insert in order");
        Tree t = orig;

        ASSERT_LT(COMM_LV, COMM_TT);
        t.set_comment(node, COMM_LV, " CLV 2");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_LV, NONE, NONE, " CLV 2");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TV, " CTV 3");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 1);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, NONE, " CTV 3");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV, " CFV 4");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 2);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, NONE, " CFV 4");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV2, " CFV2 5");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 3);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, 3, " CFV 4");
        verify_comment(t, node, 3, COMM_FV2, 2, NONE, " CFV2 5");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TT, " CTN 1");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 4);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, 3, " CFV 4");
        verify_comment(t, node, 3, COMM_FV2, 2, 4, " CFV2 5");
        verify_comment(t, node, 4, COMM_TT, 3, NONE, " CTN 1");
        _check_comment_invariants(t, "here");
    }

    {
        SCOPED_TRACE("final order: 0-1-2-4-3");
        Tree t = orig;

        ASSERT_LT(COMM_LV, COMM_TT);
        t.set_comment(node, COMM_LV, " CLV 2");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_LV, NONE, NONE, " CLV 2");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TV, " CTV 3");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 1);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, NONE, " CTV 3");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV, " CFV 4");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 2);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, NONE, " CFV 4");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TT, " CTN 1");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 3);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, 3, " CFV 4");
        verify_comment(t, node, 3, COMM_TT, 2, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV2, " CFV2 5");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 3);
        verify_comment(t, node, 0, COMM_LV, NONE, 1, " CLV 2");
        verify_comment(t, node, 1, COMM_TV, 0, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 1, 4, " CFV 4");
        verify_comment(t, node, 4, COMM_FV2, 2, 3, " CFV2 5");
        verify_comment(t, node, 3, COMM_TT, 4, NONE, " CTN 1");
        _check_comment_invariants(t, "here");
    }

    {
        SCOPED_TRACE("final order: 1-2-3-4-0");
        Tree t = orig;

        t.set_comment(node, COMM_TT, " CTN 1");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, NONE, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_LV, " CLV 2");
        EXPECT_EQ(t._p(node)->m_first_comment, 1);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, 1, NONE, " CTN 1");
        verify_comment(t, node, 1, COMM_LV, NONE, 0, " CLV 2");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TV, " CTV 3");
        EXPECT_EQ(t._p(node)->m_first_comment, 1);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, 2, NONE, " CTN 1");
        verify_comment(t, node, 1, COMM_LV, NONE, 2, " CLV 2");
        verify_comment(t, node, 2, COMM_TV, 1, 0, " CTV 3");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV, " CFV 4");
        EXPECT_EQ(t._p(node)->m_first_comment, 1);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, 3, NONE, " CTN 1");
        verify_comment(t, node, 1, COMM_LV, NONE, 2, " CLV 2");
        verify_comment(t, node, 2, COMM_TV, 1, 3, " CTV 3");
        verify_comment(t, node, 3, COMM_FV, 2, 0, " CFV 4");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV2, " CFV2 5");
        EXPECT_EQ(t._p(node)->m_first_comment, 1);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, 4, NONE, " CTN 1");
        verify_comment(t, node, 1, COMM_LV, NONE, 2, " CLV 2");
        verify_comment(t, node, 2, COMM_TV, 1, 3, " CTV 3");
        verify_comment(t, node, 3, COMM_FV, 2, 4, " CFV 4");
        verify_comment(t, node, 4, COMM_FV2, 3, 0, " CFV2 5");
        _check_comment_invariants(t, "here");
    }

    {
        SCOPED_TRACE("4-3-2-1-0");
        Tree t = orig;

        ASSERT_LT(COMM_LV, COMM_TT);
        t.set_comment(node, COMM_TT, " CTN 1");
        EXPECT_EQ(t._p(node)->m_first_comment, 0);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 0, COMM_TT, NONE, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV2, " CFV2 5");
        EXPECT_EQ(t._p(node)->m_first_comment, 1);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 1, COMM_FV2, NONE, 0, " CFV2 5");
        verify_comment(t, node, 0, COMM_TT, 1, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_FV, " CFV 4");
        EXPECT_EQ(t._p(node)->m_first_comment, 2);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 2, COMM_FV, NONE, 1, " CFV 4");
        verify_comment(t, node, 1, COMM_FV2, 2, 0, " CFV2 5");
        verify_comment(t, node, 0, COMM_TT, 1, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_TV, " CTV 3");
        EXPECT_EQ(t._p(node)->m_first_comment, 3);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 3, COMM_TV, NONE, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 3, 1, " CFV 4");
        verify_comment(t, node, 1, COMM_FV2, 2, 0, " CFV2 5");
        verify_comment(t, node, 0, COMM_TT, 1, NONE, " CTN 1");
        _check_comment_invariants(t, "here");

        t.set_comment(node, COMM_LV, " CLV 2");
        EXPECT_EQ(t._p(node)->m_first_comment, 4);
        EXPECT_EQ(t._p(node)->m_last_comment, 0);
        verify_comment(t, node, 4, COMM_LV, NONE, 3, " CLV 2");
        verify_comment(t, node, 3, COMM_TV, 4, 2, " CTV 3");
        verify_comment(t, node, 2, COMM_FV, 3, 1, " CFV 4");
        verify_comment(t, node, 1, COMM_FV2, 2, 0, " CFV2 5");
        verify_comment(t, node, 0, COMM_TT, 1, NONE, " CTN 1");
        _check_comment_invariants(t, "here");
    }
#endif
}


//-----------------------------------------------------------------------------

/*WIP*/GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(YmlTestCase);
#ifdef WIP

TEST(comments, trailing_doc_1)
{
    csubstr yaml =
        // comments outside docs are ignored
        "# ignored 0"                      "\n"
        "--- # CTN 1"                      "\n"
        "# CLV 2"                          "\n"
        "{ # CTV 3"                        "\n"
        "} # CFV 4"                        "\n"
        "# CFV2 5"                         "\n"
        "... # ignored 6"                  "\n"
        "# ignored 7"                      "\n";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef r = t.rootref();
        ASSERT_TRUE(r.type().is_stream());
        ConstNodeRef d = t.docref(0);
        ASSERT_TRUE(d.type().is_stream());
        ASSERT_TRUE(d.comment(COMM_TT));
        ASSERT_TRUE(d.comment(COMM_LV));
        ASSERT_TRUE(d.comment(COMM_TV));
        ASSERT_TRUE(d.comment(COMM_FV));
        ASSERT_TRUE(d.comment(COMM_FV2));
        EXPECT_EQ(d.comment(COMM_TT)->m_text, " CTN 1");
        EXPECT_EQ(d.comment(COMM_LV)->m_text, " CLV 2");
        EXPECT_EQ(d.comment(COMM_TV)->m_text, " CTV 3");
        EXPECT_EQ(d.comment(COMM_FV)->m_text, " CFV 4");
        EXPECT_EQ(d.comment(COMM_FV2)->m_text, " CFV2 5");
    });
}

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
#endif


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
