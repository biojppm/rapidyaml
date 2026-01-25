#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++-4.8 cannot accept
// them as macro arguments

#ifndef RYML_WITH_COMMENTS
#error this test requires RYML_WITH_COMMENTS to be defined
#endif

namespace c4 {
namespace yml {

constexpr const bool multiline = true;
constexpr const bool singleline = false;

COMMENT_TEST(DocEmptyMinimal,
             "# 1"                                      "\n"
             ,//-----------------------------------------------
             "+STR"                                     "\n"
             "+DOC"                                     "\n"
             "=COMM #[LEADING] 1"                       "\n"
             "=VAL :"                                   "\n"
             "-DOC"                                     "\n"
             "-STR"                                     "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

COMMENT_TEST(DocValMinimal,
             "# 1"                       "\n"
             "foo # 2"                   "\n"
             "# 3"                       "\n"
             ,
             "+STR"                      "\n"
             "+DOC"                      "\n"
             "=COMM #[LEADING] 1"        "\n"
             "=VAL :foo"                 "\n"
             "=COMM #[TRAILING] 2"       "\n"
             "=COMM #[FOOTER] 3"         "\n"
             "-DOC"                      "\n"
             "-STR"                      "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_comment(" 3", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(DocSeqMinimal,
             "# 1"                       "\n"
             "[ # 2"                     "\n"
             "] # 3"                     "\n"
             "# 4"                       "\n"
             ,
             "+STR"                      "\n"
             "+DOC"                      "\n"
             "=COMM #[LEADING] 1"        "\n"
             "+SEQ []"                   "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"   "\n"
             "-SEQ"                      "\n"
             "=COMM #[TRAILING] 3"       "\n"
             "=COMM #[FOOTER] 4"         "\n"
             "-DOC"                      "\n"
             "-STR"                      "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 3", COMM_TRAILING));
    ___(ps.add_comment(" 4", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(DocMapMinimal,
             "# 1"                       "\n"
             "{ # 2"                     "\n"
             "} # 3"                     "\n"
             "# 4"                       "\n"
             ,
             "+STR"                      "\n"
             "+DOC"                      "\n"
             "=COMM #[LEADING] 1"        "\n"
             "+MAP {}"                   "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"   "\n"
             "-MAP"                      "\n"
             "=COMM #[TRAILING] 3"       "\n"
             "=COMM #[FOOTER] 4"         "\n"
             "-DOC"                      "\n"
             "-STR"                      "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 3", COMM_TRAILING));
    ___(ps.add_comment(" 4", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

COMMENT_TEST(StreamMinimalBase,
             "--- # 2"                   "\n"
             "# 3"                       "\n"
             "foo # 4"                   "\n"
             "# 5"                       "\n"
             ,
             "+STR"                      "\n"
             "+DOC ---"                  "\n"
             "=COMM #[DOC_TRAILING] 2"   "\n"
             "=COMM #[LEADING] 3"        "\n"
             "=VAL :foo"                 "\n"
             "=COMM #[TRAILING] 4"       "\n"
             "=COMM #[FOOTER] 5"         "\n"
             "-DOC"                      "\n"
             "-STR"                      "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 2", COMM_DOC_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamMinimal,
             "# 1"                       "\n"
             "--- # 2"                   "\n"
             "# 3"                       "\n"
             "foo # 4"                   "\n"
             "# 5"                       "\n"
             "... # 6"                   "\n"
             "# 7"                       "\n"
             ,
             "+STR"                      "\n"
             "=COMM #[LEADING] 1"        "\n"
             "+DOC ---"                  "\n"
             "=COMM #[DOC_TRAILING] 2"   "\n"
             "=COMM #[LEADING] 3"        "\n"
             "=VAL :foo"                 "\n"
             "=COMM #[TRAILING] 4"       "\n"
             "=COMM #[FOOTER] 5"         "\n"
             "-DOC ..."                  "\n"
             "=COMM #[TRAILING] 6"       "\n"
             "=COMM #[FOOTER] 7"         "\n"
             "-STR"                      "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 2", COMM_DOC_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_doc_expl());
    ___(ps.add_comment(" 6", COMM_TRAILING));
    ___(ps.add_comment(" 7", COMM_FOOTER));
    ___(ps.end_stream());
}

COMMENT_TEST(DocValEmptyLeading,
             "# 1"                              "\n"
             ,//-----------------------------------------------
             "+STR"                             "\n"
             "+DOC"                             "\n"
             "=COMM #[LEADING] 1"               "\n"
             "=VAL :"                           "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValEmptyLeading,
             "---"                              "\n"
             "# 1"                              "\n"
             ,//-----------------------------------------------
             "+STR"                             "\n"
             "+DOC ---"                         "\n"
             "=COMM #[LEADING] 1"               "\n"
             "=VAL :"                           "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValNoComment,
             "---"                              "\n"
             "foo"                              "\n"
             ,//-----------------------------------------------
             "--- foo"                          "\n"
             ,//-----------------------------------------------
             "+STR"                             "\n"
             "+DOC ---"                         "\n"
             "=VAL :foo"                        "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValTrailing,
             "---"                              "\n"
             "foo # 1"                          "\n"
             ,//-----------------------------------------------
             "--- foo # 1"                      "\n"
             ,//-----------------------------------------------
             "+STR"                             "\n"
             "+DOC ---"                         "\n"
             "=VAL :foo"                        "\n"
             "=COMM #[TRAILING] 1"              "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValFooter,
             "---"                              "\n"
             "foo"                              "\n"
             "# 1"                              "\n"
             ,//-----------------------------------------------
             "--- foo"                          "\n"
             "# 1"                              "\n"
             ,//-----------------------------------------------
             "+STR"                             "\n"
             "+DOC ---"                         "\n"
             "=VAL :foo"                        "\n"
             "=COMM #[FOOTER] 1"                "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(DocValNoComment,
             "foo"                                "\n"
             ,//-----------------------------------------------
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=VAL :foo"                          "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(DocValTrailing,
             "foo # 1"                            "\n"
             ,//-----------------------------------------------
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=VAL :foo"                          "\n"
             "=COMM #[TRAILING] 1"                "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(DocValFooter,
             "foo"                         "\n"
             "# 1"                         "\n"
             ,//-----------------------------------------------
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "=VAL :foo"                   "\n"
             "=COMM #[FOOTER] 1"           "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValEmptyLeadingMulti,
             "---"                           "\n"
             "# 1"                           "\n"
             "---"                           "\n"
             "# 2"                           "\n"
             "---"                           "\n"
             "# 3"                           "\n"
             ,//-----------------------------------------------
             "+STR"                                         "\n"
             "+DOC ---"                                     "\n"
             "=COMM #[LEADING] 1"                           "\n"
             "=VAL :"                                       "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=COMM #[LEADING] 2"                           "\n"
             "=VAL :"                                       "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=COMM #[LEADING] 3"                           "\n"
             "=VAL :"                                       "\n"
             "-DOC"                                         "\n"
             "-STR"                                         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 2", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValNoComent,
             "---"                    "\n"
             "foo"                    "\n"
             "---"                    "\n"
             "bar"                    "\n"
             "---"                    "\n"
             "baz"                    "\n"
             ,//-----------------------------------------------
             "--- foo"                "\n"
             "--- bar"                "\n"
             "--- baz"                "\n"
             ,//-----------------------------------------------
             "+STR"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :foo"                     "\n"
             "-DOC"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :bar"                     "\n"
             "-DOC"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :baz"                     "\n"
             "-DOC"                          "\n"
             "-STR"                          "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValTrailingMulti,
             "---"                          "\n"
             "foo # 1"                      "\n"
             "---"                          "\n"
             "bar # 2"                      "\n"
             "---"                          "\n"
             "baz # 3"                      "\n"
             ,//-----------------------------------------------
             "--- foo # 1"                                      "\n"
             "--- bar # 2"                                      "\n"
             "--- baz # 3"                                      "\n"
             ,//-----------------------------------------------
             "+STR"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :foo"                     "\n"
             "=COMM #[TRAILING] 1"           "\n"
             "-DOC"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :bar"                     "\n"
             "=COMM #[TRAILING] 2"           "\n"
             "-DOC"                          "\n"
             "+DOC ---"                      "\n"
             "=VAL :baz"                     "\n"
             "=COMM #[TRAILING] 3"           "\n"
             "-DOC"                          "\n"
             "-STR"                          "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_comment(" 3", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamValFooterMulti,
             "---"                         "\n"
             "foo"                         "\n"
             "# 1"                         "\n"
             "---"                         "\n"
             "bar"                         "\n"
             "# 2"                         "\n"
             "---"                         "\n"
             "baz"                         "\n"
             "# 3"                         "\n"
             ,//-----------------------------------------------
             "--- foo"                     "\n"
             "# 1"                         "\n"
             "--- bar"                     "\n"
             "# 2"                         "\n"
             "--- baz"                     "\n"
             "# 3"                         "\n"
             ,//-----------------------------------------------
             "+STR"                        "\n"
             "+DOC ---"                    "\n"
             "=VAL :foo"                   "\n"
             "=COMM #[FOOTER] 1"           "\n"
             "-DOC"                        "\n"
             "+DOC ---"                    "\n"
             "=VAL :bar"                   "\n"
             "=COMM #[FOOTER] 2"           "\n"
             "-DOC"                        "\n"
             "+DOC ---"                    "\n"
             "=VAL :baz"                   "\n"
             "=COMM #[FOOTER] 3"           "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 1", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" 2", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_comment(" 3", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(StreamMultiDoc,
             "# 1"                        "\n"
             "--- # 2"                    "\n"
             "# 3"                        "\n"
             "foo # 4"                    "\n"
             "# 5"                        "\n"
             "--- # 6"                    "\n"
             "# 7"                        "\n"
             "bar # 8"                    "\n"
             "# 9"                        "\n"
             "--- # 10"                   "\n"
             "# 11"                       "\n"
             "baz # 12"                   "\n"
             "# 13"                       "\n"
             "... # 14"                   "\n"
             "# 15"                       "\n"
             ,
             "+STR"                       "\n"
             "=COMM #[LEADING] 1"         "\n"
             "+DOC ---"                   "\n"
             "=COMM #[DOC_TRAILING] 2"    "\n"
             "=COMM #[LEADING] 3"         "\n"
             "=VAL :foo"                  "\n"
             "=COMM #[TRAILING] 4"        "\n"
             "=COMM #[FOOTER] 5"          "\n"
             "-DOC"                       "\n"
             "+DOC ---"                   "\n"
             "=COMM #[DOC_TRAILING] 6"    "\n"
             "=COMM #[LEADING] 7"         "\n"
             "=VAL :bar"                  "\n"
             "=COMM #[TRAILING] 8"        "\n"
             "=COMM #[FOOTER] 9"          "\n"
             "-DOC"                       "\n"
             "+DOC ---"                   "\n"
             "=COMM #[DOC_TRAILING] 10"   "\n"
             "=COMM #[LEADING] 11"        "\n"
             "=VAL :baz"                  "\n"
             "=COMM #[TRAILING] 12"       "\n"
             "=COMM #[FOOTER] 13"         "\n"
             "-DOC ..."                   "\n"
             "=COMM #[TRAILING] 14"       "\n"
             "=COMM #[FOOTER] 15"         "\n"
             "-STR"                       "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 2", COMM_DOC_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 6", COMM_DOC_TRAILING));
    ___(ps.add_comment(" 7", COMM_LEADING));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" 8", COMM_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 10", COMM_DOC_TRAILING));
    ___(ps.add_comment(" 11", COMM_LEADING));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_comment(" 12", COMM_TRAILING));
    ___(ps.add_comment(" 13", COMM_FOOTER));
    ___(ps.end_doc_expl());
    ___(ps.add_comment(" 14", COMM_TRAILING));
    ___(ps.add_comment(" 15", COMM_FOOTER));
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------


COMMENT_TEST(Single,
            "# single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] single comment\n"
            "=VAL :"                  "\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" single comment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleNoLeadSpace,
            "#single comment\n"
            ,
            "# single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING]single comment\n"
            "=VAL :"                  "\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("single comment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleLeadSpace,
            "#   single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING]   single comment\n"
            "=VAL :"                  "\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("   single comment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline0,
            "# single"     "\n"
            "# comment"     "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING] single\\n comment"             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" single\n comment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline0NoLeadSpace,
            "#single"     "\n"
            "#comment"     "\n"
            ,
            "# single"     "\n"
            "# comment"     "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING]single\\ncomment"             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("single\ncomment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline0LeadSpace,
            "#   single"     "\n"
            "#   comment"     "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING]   single\\n   comment"             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("   single\n   comment", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline1,
            "#"             "\n"
            "# single"     "\n"
            "# comment"     "\n"
            "#"             "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING]\\n single\\n comment\\n"             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("\n single\n comment\n", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline1NoLeadSpace,
            "#"             "\n"
            "#single"     "\n"
            "#comment"     "\n"
            "#"             "\n"
            ,
            "#"             "\n"
            "# single"     "\n"
            "# comment"     "\n"
            "#"             "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING]\\nsingle\\ncomment\\n"             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("\nsingle\ncomment\n", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline1LeadSpace,
            "#   "             "\n"
            "#   single"     "\n"
            "#   comment"     "\n"
            "#   "             "\n"
            ,
            "+STR"             "\n"
            "+DOC"             "\n"
            "=COMM #[LEADING]   \\n   single\\n   comment\\n   "             "\n"
            "=VAL :"                  "\n"
            "-DOC"             "\n"
            "-STR"             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("   \n   single\n   comment\n   ", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline2,
            "#\n"
            "#\n"
            "# single\n"
            "#\n"
            "#\n"
            "# comment\n"
            "#\n"
            "#\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING]\\n\\n single\\n\\n\\n comment\\n\\n"   "\n"
            "=VAL :"                  "\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("\n\n single\n\n\n comment\n\n", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline3,
            "#\n"
            "#\n"
            "#  single  \n"
            "#  \n"
            "#  \n"
            "#  comment  \n"
            "#\n"
            "#\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING]\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n\n"
            "=VAL :"                  "\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment("\n\n  single  \n  \n  \n  comment  \n\n", COMM_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

COMMENT_TEST(SingleMultiline4Indented1,
            "a:\n"
            "  #\n"
            "  #\n"
            "  #  single  \n"
            "  #  \n"
            "  #  \n"
            "  #  comment  \n"
            "  #\n"
            "  #\n"
            "  b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "=COMM #[VAL_LEADING]\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment("\n\n  single  \n  \n  \n  comment  \n\n", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(SingleMultiline4Indented2,
            "a:"                         "\n"
            "  b:"                       "\n"
            "    c:"                     "\n"
            "      #"                    "\n"
            "      #"                    "\n"
            "      #  single  "          "\n"
            "      #  "                  "\n"
            "      #  "                  "\n"
            "      #  comment  "         "\n"
            "      #"                    "\n"
            "      #"                    "\n"
            "      d: "                  "\n"
            ,
            "+STR"                                                  "\n"
            "+DOC"                                                  "\n"
            "+MAP"                                                  "\n"
            "=VAL :a"                                               "\n"
            "+MAP"                                                  "\n"
            "=VAL :b"                                               "\n"
            "+MAP"                                                  "\n"
            "=VAL :c"                                               "\n"
            "+MAP"                                                  "\n"
            "=COMM #[LEADING]\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n"  "\n"
            "=VAL :d"                                               "\n"
            "=VAL :"                                                "\n"
            "-MAP"                                                  "\n"
            "-MAP"                                                  "\n"
            "-MAP"                                                  "\n"
            "-MAP"                                                  "\n"
            "-DOC"                                                  "\n"
            "-STR"                                                  "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment("\n\n  single  \n  \n  \n  comment  \n\n", COMM_LEADING));
    ___(ps.set_key_scalar_plain("d"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
