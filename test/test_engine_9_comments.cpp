#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++-4.8 cannot accept
// them as macro arguments

#ifndef RYML_WITH_COMMENTS
#error this test requires RYML_WITH_COMMENTS to be defined
#endif

namespace c4 {
namespace yml {


#ifdef RYML_WITH_COMMENTS
#define COMMENT_TEST(name, ...) ENGINE_TEST_(name, ParserOptions{}.with_comments(true), __VA_ARGS__)
#else
#define COMMENT_TEST(name, ...)                                 \
    template<class Handler> void disabled_##name(Handler &ps);  \
    template<class Handler> void disabled_##name(Handler &ps)
#endif

constexpr const bool multiline = true;
constexpr const bool singleline = false;

COMMENT_TEST(FlowSeqMinimalBase,
             "["                           "\n"
             "  val1,"                     "\n"
             "  val2"                      "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=VAL :val1"                  "\n"
             "=VAL :val2"                  "\n"
             "-SEQ"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2"                 "\n"
             "  # 3"                       "\n"
             "  val2 # 4"                  "\n"
             "  # 5"                       "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 5"           "\n"
             "-SEQ"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqBasic,
             "# 1"                                     "\n"
             "[ # 2"                                   "\n"
             "  # 3"                                   "\n"
             "  a # 4"                                 "\n"
             "  # 5"                                   "\n"
             "  , # 6"                                 "\n"
             "  # 7"                                   "\n"
             "  b # 8"                                 "\n"
             "  # 9"                                   "\n"
             "  , # 10"                                "\n"
             "  # 11"                                  "\n"
             "] # 12"                                  "\n"
             "# 13"                                    "\n"
             ,
             "+STR"                                    "\n"
             "+DOC"                                    "\n"
             "=COMM #[LEADING] 1"                      "\n"
             "+SEQ []"                                 "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"    "\n"
             "=VAL :a"                                 "\n"
             "=COMM #[VAL_TRAILING] 4"                 "\n"
             "=COMM #[COMMA_LEADING] 5"                "\n"
             "=COMM #[TRAILING] 6"                     "\n"
             "=COMM #[LEADING] 7"                      "\n"
             "=VAL :b"                                 "\n"
             "=COMM #[VAL_TRAILING] 8"                 "\n"
             "=COMM #[COMMA_LEADING] 9"                "\n"
             "=COMM #[TRAILING] 10"                    "\n"
             "=COMM #[FOOTER] 11"                      "\n"
             "-SEQ"                                    "\n"
             "=COMM #[TRAILING] 12"                    "\n"
             "=COMM #[FOOTER] 13"                      "\n"
             "-DOC"                                    "\n"
             "-STR"                                    "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 6", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 7", COMM_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 10", COMM_TRAILING));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 12", COMM_TRAILING));
    ___(ps.add_comment(" 13", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//  equivalent to {&a !!atag a: &b !!btag b}
COMMENT_TEST(FlowSeqBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "[ # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  &a # 4"                                            "\n"
             "  # 5"                                               "\n"
             "  !atag # 6"                                         "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "] # 10"                                              "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"                     "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 4"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 5"                          "\n"
             "=COMM #[VAL_TAG_TRAILING] 6"                         "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[TRAILING] 8"                                 "\n"
             "=COMM #[FOOTER] 9"                                   "\n"
             "-SEQ"                                                "\n"
             "=COMM #[TRAILING] 10"                                "\n"
             "=COMM #[FOOTER] 11"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 4", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 6", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 10", COMM_TRAILING));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(FlowSeqMultiline0,
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "] # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMultiline1,
            "---\n"
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "] # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "[ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "] # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 5 leading comment\\n 5 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 6 bracket trailing comment\\n 6 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 7 trailing comment\\n 7 continued\n"
            "=COMM #[FOOTER] 8 footer comment\\n 8 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 5 leading comment\n 5 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 6 bracket trailing comment\n 6 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 7 trailing comment\n 7 continued", COMM_TRAILING));
    ___(ps.add_comment(" 8 footer comment\n 8 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMultiline2,
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "] # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "[ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "] # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "---\n"
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "] # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "[ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "] # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 5 leading comment\\n 5 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 6 bracket trailing comment\\n 6 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 7 trailing comment\\n 7 continued\n"
            "=COMM #[FOOTER] 8 footer comment\\n 8 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 5 leading comment\n 5 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 6 bracket trailing comment\n 6 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 7 trailing comment\n 7 continued", COMM_TRAILING));
    ___(ps.add_comment(" 8 footer comment\n 8 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

COMMENT_TEST(FlowMapMinimalBase,
             "{"                           "\n"
             "  key1: val1,"               "\n"
             "  key2: val2"                "\n"
             "}"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+MAP {}"                     "\n"
             "=VAL :key1"                  "\n"
             "=VAL :val1"                  "\n"
             "=VAL :key2"                  "\n"
             "=VAL :val2"                  "\n"
             "-MAP"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapMinimal,
             "{"                           "\n"
             "  # 1"                       "\n"
             "  key1: val1, # 2"           "\n"
             "  # 3"                       "\n"
             "  key2: val2 # 4"            "\n"
             "  # 5"                       "\n"
             "}"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+MAP {}"                     "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :key1"                  "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=VAL :key2"                  "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 5"           "\n"
             "-MAP"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapBasic,
             "# 1"                                         "\n"
             "{ # 2"                                       "\n"
             "  # 3"                                       "\n"
             "  a # 4"                                     "\n"
             "  # 5"                                       "\n"
             "  : # 6"                                     "\n"
             "  # 7"                                       "\n"
             "  b # 8"                                     "\n"
             "  # 9"                                       "\n"
             "} # 10"                                      "\n"
             "# 11"                                        "\n"
             ,
             "+STR"                                        "\n"
             "+DOC"                                        "\n"
             "=COMM #[LEADING] 1"                          "\n"
             "+MAP {}"                                     "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"             "\n"
             "=COMM #[LEADING] 3"                          "\n"
             "=VAL :a"                                     "\n"
             "=COMM #[KEY_TRAILING] 4"                     "\n"
             "=COMM #[COLON_LEADING] 5"                    "\n"
             "=COMM #[COLON_TRAILING] 6"                   "\n"
             "=COMM #[VAL_LEADING] 7"                      "\n"
             "=VAL :b"                                     "\n"
             "=COMM #[VAL_TRAILING] 8"                     "\n"
             "=COMM #[FOOTER] 9"                           "\n"
             "-MAP"                                        "\n"
             "=COMM #[TRAILING] 10"                        "\n"
             "=COMM #[FOOTER] 11"                          "\n"
             "-DOC"                                        "\n"
             "-STR"                                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 5", COMM_COLON_LEADING));
    ___(ps.add_comment(" 6", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 10", COMM_TRAILING));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//  equivalent to {!!atag &a a: !!btag &b b}
COMMENT_TEST(FlowMapBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "{ # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  &a # 4"                                            "\n"
             "  # 5"                                               "\n"
             "  !atag # 6"                                         "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "  : # 10"                                            "\n"
             "  # 11"                                              "\n"
             "  &b # 12"                                           "\n"
             "  # 13"                                              "\n"
             "  !btag # 14"                                        "\n"
             "  # 15"                                              "\n"
             "  b # 16"                                            "\n"
             "  # 17"                                              "\n"
             "} # 18"                                              "\n"
             "# 19"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+MAP {}"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"                     "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 4"                      "\n"
             "=COMM #[KEY_TAG_LEADING] 5"                          "\n"
             "=COMM #[KEY_TAG_TRAILING] 6"                         "\n"
             "=COMM #[KEY_LEADING] 7"                              "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[KEY_TRAILING] 8"                             "\n"
             "=COMM #[COLON_LEADING] 9"                            "\n"
             "=COMM #[COLON_TRAILING] 10"                          "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 11"                      "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 12"                     "\n"
             "=COMM #[VAL_TAG_LEADING] 13"                         "\n"
             "=COMM #[VAL_TAG_TRAILING] 14"                        "\n"
             "=COMM #[VAL_LEADING2] 15"                            "\n"
             "=VAL &b <!btag> :b"                                  "\n"
             "=COMM #[VAL_TRAILING] 16"                            "\n"
             "=COMM #[FOOTER] 17"                                  "\n"
             "-MAP"                                                "\n"
             "=COMM #[TRAILING] 18"                                "\n"
             "=COMM #[FOOTER] 19"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_anchor("a"));
    ___(ps.add_comment(" 4", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 5", COMM_KEY_TAG_LEADING));
    ___(ps.set_key_tag("!atag"));
    ___(ps.add_comment(" 6", COMM_KEY_TAG_TRAILING));
    ___(ps.add_comment(" 7", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 9", COMM_COLON_LEADING));
    ___(ps.add_comment(" 10", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 11", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("b"));
    ___(ps.add_comment(" 12", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 13", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!btag"));
    ___(ps.add_comment(" 14", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 15", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 16", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 17", COMM_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 18", COMM_TRAILING));
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(FlowMapMultiline0,
            "# 1 leading comment\n"
            "# 1 continued\n"
            "{ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "} # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+MAP {}\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-MAP\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapMultiline1,
            "---\n"
            "# 1 leading comment\n"
            "# 1 continued\n"
            "{ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "} # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "{ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "} # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+MAP {}\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-MAP\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 5 leading comment\\n 5 continued\n"
            "+MAP {}\n"
            "=COMM #[VAL_BRACKET_TRAILING] 6 bracket trailing comment\\n 6 continued\n"
            "-MAP\n"
            "=COMM #[TRAILING] 7 trailing comment\\n 7 continued\n"
            "=COMM #[FOOTER] 8 footer comment\\n 8 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 5 leading comment\n 5 continued", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 6 bracket trailing comment\n 6 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 7 trailing comment\n 7 continued", COMM_TRAILING));
    ___(ps.add_comment(" 8 footer comment\n 8 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapMultiline2,
            "# 1 leading comment\n"
            "# 1 continued\n"
            "{ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "} # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "{ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "} # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "---\n"
            "# 1 leading comment\n"
            "# 1 continued\n"
            "{ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "} # 3 trailing comment\n"
            "  # 3 continued\n"
            "# 4 footer comment\n"
            "# 4 continued\n"
            "---\n"
            "# 5 leading comment\n"
            "# 5 continued\n"
            "{ # 6 bracket trailing comment\n"
            "  # 6 continued\n"
            "} # 7 trailing comment\n"
            "  # 7 continued\n"
            "# 8 footer comment\n"
            "# 8 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+MAP {}\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-MAP\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\n"
            "=COMM #[FOOTER] 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "+DOC ---\n"
            "=COMM #[LEADING] 5 leading comment\\n 5 continued\n"
            "+MAP {}\n"
            "=COMM #[VAL_BRACKET_TRAILING] 6 bracket trailing comment\\n 6 continued\n"
            "-MAP\n"
            "=COMM #[TRAILING] 7 trailing comment\\n 7 continued\n"
            "=COMM #[FOOTER] 8 footer comment\\n 8 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued", COMM_TRAILING));
    ___(ps.add_comment(" 4 footer comment\n 4 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 5 leading comment\n 5 continued", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 6 bracket trailing comment\n 6 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 7 trailing comment\n 7 continued", COMM_TRAILING));
    ___(ps.add_comment(" 8 footer comment\n 8 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMap1,
             "# 0\n"
             "{ # 0.1\n"
             "  # 1\n"
             "  foo: 0, # 2\n"
             "  # 3\n"
             "  bar: 1, # 4\n"
             "  # 5\n"
             "  map: { # 6\n"
             "    # 7\n"
             "    mapchild: yes # 8\n"
             "    # 9\n"
             "  }, # 10\n"
             "  # 11\n"
             "  seq: [ # 12\n"
             "    # 13\n"
             "    seqchild # 14\n"
             "    # 15\n"
             "  ] # 16\n"
             "  # 17\n"
             "} # 18\n"
             "# 19\n"
             ,
             ""
             "+STR"                                  "\n"
             "+DOC"                                  "\n"
             "=COMM #[LEADING] 0"                    "\n"
             "+MAP {}"                               "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 0.1"     "\n"
             "=COMM #[LEADING] 1"                    "\n"
             "=VAL :foo"                             "\n"
             "=VAL :0"                               "\n"
             "=COMM #[TRAILING] 2"                   "\n"
             "=COMM #[LEADING] 3"                    "\n"
             "=VAL :bar"                             "\n"
             "=VAL :1"                               "\n"
             "=COMM #[TRAILING] 4"                   "\n"
             "=COMM #[LEADING] 5"                    "\n"
             "=VAL :map"                             "\n"
             "+MAP {}"                               "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 6"       "\n"
             "=COMM #[LEADING] 7"                    "\n"
             "=VAL :mapchild"                        "\n"
             "=VAL :yes"                             "\n"
             "=COMM #[TRAILING] 8"                   "\n"
             "=COMM #[FOOTER] 9"                     "\n"
             "-MAP"                                  "\n"
             "=COMM #[TRAILING] 10"                  "\n"
             "=COMM #[LEADING] 11"                   "\n"
             "=VAL :seq"                             "\n"
             "+SEQ []"                               "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 12"      "\n"
             "=COMM #[LEADING] 13"                   "\n"
             "=VAL :seqchild"                        "\n"
             "=COMM #[TRAILING] 14"                  "\n"
             "=COMM #[FOOTER] 15"                    "\n"
             "-SEQ"                                  "\n"
             "=COMM #[TRAILING] 16"                  "\n"
             "=COMM #[FOOTER] 17"                    "\n"
             "-MAP"                                  "\n"
             "=COMM #[TRAILING] 18"                  "\n"
             "=COMM #[FOOTER] 19"                    "\n"
             "-DOC"                                  "\n"
             "-STR"                                  "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 0", COMM_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 0.1", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 5", COMM_LEADING));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 6", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 7", COMM_LEADING));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment(" 8", COMM_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 10", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 11", COMM_LEADING));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 12", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 13", COMM_LEADING));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment(" 14", COMM_TRAILING));
    ___(ps.add_comment(" 15", COMM_FOOTER));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 16", COMM_TRAILING));
    ___(ps.add_comment(" 17", COMM_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 18", COMM_TRAILING));
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

COMMENT_TEST(BlockSeqMinimal,
             "# 1"                         "\n"
             "- val1 # 2"                  "\n"
             "# 3"                         "\n"
             "- val2 # 4"                  "\n"
             "# 5"                         "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 5"           "\n"
             "-SEQ"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasic,
             "# 1"                                 "\n"
             "- # 2"                               "\n"
             "  # 3"                               "\n"
             "  a # 4"                             "\n"
             "  # 5"                               "\n"
             "# 6"                                 "\n"
             "- # 7"                               "\n"
             "  # 8"                               "\n"
             "  b # 9"                             "\n"
             "  # 10"                              "\n"
             "# 11"                                "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=COMM #[LEADING] 1"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2"        "\n"
             "=COMM #[VAL_LEADING] 3"              "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 4"                 "\n"
             "=COMM #[FOOTER] 5"                   "\n"
             "=COMM #[LEADING] 6"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 7\\n 8"   "\n"
             "=VAL :b"                             "\n"
             "=COMM #[TRAILING] 9"                 "\n"
             "=COMM #[FOOTER] 10"                  "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 11"                  "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 2", COMM_VAL_DASH_TRAILING));
    ___(ps.add_comment(" 3", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 6", COMM_LEADING));
    ___(ps.add_comment(" 7\n 8", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9", COMM_TRAILING));
    ___(ps.add_comment(" 10", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "- # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  &aa # 4"                                           "\n"
             "  # 5"                                               "\n"
             "  !atag # 6"                                         "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "# 10"                                                "\n"
             "- # 11"                                              "\n"
             "  # 12"                                              "\n"
             "  &bb # 13"                                          "\n"
             "  # 14"                                              "\n"
             "  !btag # 15"                                        "\n"
             "  # 16"                                              "\n"
             "  b # 17"                                            "\n"
             "  # 18"                                              "\n"
             "# 19"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "+SEQ"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2"                        "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 3"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 4"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 5"                          "\n"
             "=COMM #[VAL_TAG_TRAILING] 6"                         "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &aa <!atag> :a"                                 "\n"
             "=COMM #[TRAILING] 8"                                 "\n"
             "=COMM #[FOOTER] 9"                                   "\n"
             "=COMM #[LEADING] 10"                                 "\n"
             "=COMM #[VAL_DASH_TRAILING] 11\\n 12"                 "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 13"                     "\n"
             "=COMM #[VAL_TAG_LEADING] 14"                         "\n"
             "=COMM #[VAL_TAG_TRAILING] 15"                        "\n"
             "=COMM #[VAL_LEADING2] 16"                            "\n"
             "=VAL &bb <!btag> :b"                                 "\n"
             "=COMM #[TRAILING] 17"                                "\n"
             "=COMM #[FOOTER] 18"                                  "\n"
             "-SEQ"                                                "\n"
             "=COMM #[FOOTER] 19"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 2", COMM_VAL_DASH_TRAILING));
    ___(ps.add_comment(" 3", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("aa"));
    ___(ps.add_comment(" 4", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 6", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 10", COMM_LEADING));
    ___(ps.add_comment(" 11\n 12", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_anchor("bb"));
    ___(ps.add_comment(" 13", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 14", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!btag"));
    ___(ps.add_comment(" 15", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 16", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 17", COMM_TRAILING));
    ___(ps.add_comment(" 18", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

COMMENT_TEST(BlockMapMinimal,
             "# 1"                         "\n"
             "key1: val1 # 2"              "\n"
             "# 3"                         "\n"
             "key2: val2 # 4"              "\n"
             "# 5"                         "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+MAP"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :key1"                  "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=VAL :key2"                  "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 5"           "\n"
             "-MAP"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapBasic,
             "# 1"                              "\n"
             "a: # 2"                           "\n"
             "  # 3"                            "\n"
             "  b # 4"                          "\n"
             "  # 5"                            "\n"
             "# 6"                              "\n"
             "c: # 7"                           "\n"
             "  # 8"                            "\n"
             "  d # 9"                          "\n"
             "  # 10"                           "\n"
             "# 11"                             "\n"
             ,
             "+STR"                             "\n"
             "+DOC"                             "\n"
             "+MAP"                             "\n"
             "=COMM #[LEADING] 1"               "\n"
             "=VAL :a"                          "\n"
             "=COMM #[COLON_TRAILING] 2"        "\n"
             "=COMM #[VAL_LEADING] 3"           "\n"
             "=VAL :b"                          "\n"
             "=COMM #[TRAILING] 4"              "\n"
             "=COMM #[FOOTER] 5"                "\n"
             "=COMM #[LEADING] 6"               "\n"
             "=VAL :c"                          "\n"
             "=COMM #[COLON_TRAILING] 7"        "\n"
             "=COMM #[VAL_LEADING] 8"           "\n"
             "=VAL :d"                          "\n"
             "=COMM #[TRAILING] 9"              "\n"
             "=COMM #[FOOTER] 10"               "\n"
             "-MAP"                             "\n"
             "=COMM #[FOOTER] 11"               "\n"
             "-DOC"                             "\n"
             "-STR"                             "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 2", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 3", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 6", COMM_LEADING));
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.add_comment(" 7", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.add_comment(" 9", COMM_TRAILING));
    ___(ps.add_comment(" 10", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//  equivalent to {!!atag &a a: !!btag &b b}
COMMENT_TEST(BlockMapBasicWithTagAndAnchor,
             "# 1"                               "\n"
             "? # 2"                             "\n"
             "  # 3"                             "\n"
             "  &aa # 4"                         "\n"
             "  # 5"                             "\n"
             "  !atag # 6"                       "\n"
             "  # 7"                             "\n"
             "  aaa # 8"                         "\n"
             "  # 9"                             "\n"
             "# 10"                              "\n"
             ": # 11"                            "\n"
             "  # 12"                            "\n"
             "  &bb # 13"                        "\n"
             "  # 14"                            "\n"
             "  !btag # 15"                      "\n"
             "  # 16"                            "\n"
             "  b # 17"                          "\n"
             "  # 18"                            "\n"
             "# 19"                              "\n"
             ,
             "+STR"                              "\n"
             "+DOC"                              "\n"
             "+MAP"                              "\n"
             "=COMM #[LEADING] 1"                "\n"
             "=COMM #[KEY_TRAILING_QMRK] 2"      "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 3"     "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 4"    "\n"
             "=COMM #[KEY_TAG_LEADING] 5"        "\n"
             "=COMM #[KEY_TAG_TRAILING] 6"       "\n"
             "=COMM #[KEY_LEADING] 7"            "\n"
             "=VAL &aa <!atag> :aaa"             "\n"
             "=COMM #[KEY_TRAILING] 8"           "\n"
             "=COMM #[KEY_FOOTER] 9"             "\n"
             "=COMM #[COLON_LEADING] 10"         "\n"
             "=COMM #[COLON_TRAILING] 11"        "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 12"    "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 13"   "\n"
             "=COMM #[VAL_TAG_LEADING] 14"       "\n"
             "=COMM #[VAL_TAG_TRAILING] 15"      "\n"
             "=COMM #[VAL_LEADING2] 16"          "\n"
             "=VAL &bb <!btag> :b"               "\n"
             "=COMM #[TRAILING] 17"              "\n"
             "=COMM #[FOOTER] 18"                "\n"
             "-MAP"                              "\n"
             "=COMM #[FOOTER] 19"                "\n"
             "-DOC"                              "\n"
             "-STR"                              "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 2", COMM_KEY_TRAILING_QMRK));
    ___(ps.add_comment(" 3", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("aa"));
    ___(ps.add_comment(" 4", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 5", COMM_KEY_TAG_LEADING));
    ___(ps.set_key_tag("!atag"));
    ___(ps.add_comment(" 6", COMM_KEY_TAG_TRAILING));
    ___(ps.add_comment(" 7", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("aaa"));
    ___(ps.add_comment(" 8", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 9", COMM_KEY_FOOTER));
    ___(ps.add_comment(" 10", COMM_COLON_LEADING));
    ___(ps.add_comment(" 11", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 12", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("bb"));
    ___(ps.add_comment(" 13", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 14", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!btag"));
    ___(ps.add_comment(" 15", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 16", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 17", COMM_TRAILING));
    ___(ps.add_comment(" 18", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapQmrk0NoCommentFolded,
             "? >-"                         "\n"
             "  flow seq, singleline"       "\n"
             ":"                            "\n"
             "  - 'flow val'"               "\n"
             "  - \"flow val\""             "\n"
             ,
             "+STR\n"
             "+DOC\n"
             "+MAP\n"
             "=VAL >flow seq, singleline\n"
             "+SEQ\n"
             "=VAL 'flow val\n"
             "=VAL \"flow val\n"
             "-SEQ\n"
             "-MAP\n"
             "-DOC\n"
             "-STR\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_folded("flow seq, singleline"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("flow val"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_dquoted("flow val"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk1CommentLeading,
             "?"                            "\n"
             "  # comment here"             "\n"
             "  >-"                         "\n"
             "    flow seq, singleline"     "\n"
             ":"                            "\n"
             "  - 'flow val'"               "\n"
             "  - \"flow val\""             "\n"
             ,
             "+STR\n"
             "+DOC\n"
             "+MAP\n"
             "=COMM #[KEY_LEADING] comment here"           "\n"
             "=VAL >flow seq, singleline\n"
             "+SEQ\n"
             "=VAL 'flow val\n"
             "=VAL \"flow val\n"
             "-SEQ\n"
             "-MAP\n"
             "-DOC\n"
             "-STR\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" comment here", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_folded("flow seq, singleline"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("flow val"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_dquoted("flow val"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk1CommentFooter1,
             "?"                            "\n"
             "  >-"                         "\n"
             "    flow seq, singleline"     "\n"
             "  # comment here"             "\n"
             ":"                            "\n"
             "  - 'flow val'"               "\n"
             "  - \"flow val\""             "\n"
             ,
             "? >-"                         "\n"
             "    flow seq, singleline"     "\n"
             "  # comment here"             "\n"
             ":"                            "\n"
             "  - 'flow val'"               "\n"
             "  - \"flow val\""             "\n"
             ,
             "+STR\n"
             "+DOC\n"
             "+MAP\n"
             "=VAL >flow seq, singleline\n"
             "=COMM #[KEY_FOOTER] comment here"           "\n"
             "+SEQ\n"
             "=VAL 'flow val\n"
             "=VAL \"flow val\n"
             "-SEQ\n"
             "-MAP\n"
             "-DOC\n"
             "-STR\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_folded("flow seq, singleline"));
    ___(ps.add_comment(" comment here", COMM_KEY_FOOTER));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("flow val"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_dquoted("flow val"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk1CommentFooter2,
             "? >-"                         "\n"
             "    flow seq, singleline"     "\n"
             "  # comment here"             "\n"
             ":"                            "\n"
             "  - 'flow val'"               "\n"
             "  - \"flow val\""             "\n"
             ,
             "+STR\n"
             "+DOC\n"
             "+MAP\n"
             "=VAL >flow seq, singleline\n"
             "=COMM #[KEY_FOOTER] comment here"           "\n"
             "+SEQ\n"
             "=VAL 'flow val\n"
             "=VAL \"flow val\n"
             "-SEQ\n"
             "-MAP\n"
             "-DOC\n"
             "-STR\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_folded("flow seq, singleline"));
    ___(ps.add_comment(" comment here", COMM_KEY_FOOTER));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_squoted("flow val"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_dquoted("flow val"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapQmrk2,
             "# 1"                "\n"
             "&a1 # 2"            "\n"
             "# 3"                "\n"
             "!t1 # 4"            "\n"
             "# 5"                "\n"
             "? # 6"              "\n"
             "  # 7"              "\n"
             "  &a2 # 8"          "\n"
             "  # 9"              "\n"
             "  !t2 # 10"         "\n"
             "  # 11"             "\n"
             "  k2 # 12"          "\n"
             "  # 13"             "\n"
             "# 14"               "\n"
             ": # 15"             "\n"
             "  # 16"             "\n"
             "  &a3 # 17"         "\n"
             "  # 18"             "\n"
             "  !t3 # 19"         "\n"
             "  # 20"             "\n"
             "  v3 # 21"          "\n"
             "  # 22"             "\n"
             "# 23"               "\n"
             "&a4 !t4 k4: # 24"   "\n"
             "  # 25"             "\n"
             "  &a5 # 26"         "\n"
             "  # 27"             "\n"
             "  !t5 # 28"         "\n"
             "  # 29"             "\n"
             "  v5 # 30"          "\n"
             "  # 31"             "\n"
             "# 32"               "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[LEADING] 1"                 "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 2"     "\n"
             "=COMM #[VAL_TAG_LEADING] 3"         "\n"
             "=COMM #[VAL_TAG_TRAILING] 4"        "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[LEADING] 5"                 "\n"
             "=COMM #[KEY_TRAILING_QMRK] 6\\n 7"  "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 8"     "\n"
             "=COMM #[KEY_TAG_LEADING] 9"         "\n"
             "=COMM #[KEY_TAG_TRAILING] 10"       "\n"
             "=COMM #[KEY_LEADING] 11"            "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[KEY_TRAILING] 12"           "\n"
             "=COMM #[KEY_FOOTER] 13"             "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[COLON_TRAILING] 15\\n 16"   "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 17"    "\n"
             "=COMM #[VAL_TAG_LEADING] 18"        "\n"
             "=COMM #[VAL_TAG_TRAILING] 19"       "\n"
             "=COMM #[VAL_LEADING2] 20"           "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "=COMM #[TRAILING] 21"               "\n"
             "=COMM #[FOOTER] 22"                 "\n"
             "=COMM #[LEADING] 23"                "\n"
             "=VAL &a4 <!t4> :k4"                 "\n"
             "=COMM #[COLON_TRAILING] 24"         "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 25"     "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 26"    "\n"
             "=COMM #[VAL_TAG_LEADING] 27"        "\n"
             "=COMM #[VAL_TAG_TRAILING] 28"       "\n"
             "=COMM #[VAL_LEADING2] 29"           "\n"
             "=VAL &a5 <!t5> :v5"                 "\n"
             "=COMM #[TRAILING] 30"               "\n"
             "=COMM #[FOOTER] 31"                 "\n"
             "-MAP"                               "\n"
             "=COMM #[FOOTER] 32"                 "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 2", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 3", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.add_comment(" 4", COMM_VAL_TAG_TRAILING));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 5", COMM_LEADING));
    ___(ps.set_key_anchor("a2"));
    ___(ps.add_comment(" 6\n 7", COMM_KEY_TRAILING_QMRK));
    ___(ps.add_comment(" 8", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 9", COMM_KEY_TAG_LEADING));
    ___(ps.set_key_tag("!t2"));
    ___(ps.add_comment(" 10", COMM_KEY_TAG_TRAILING));
    ___(ps.add_comment(" 11", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 12", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 13", COMM_KEY_FOOTER));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.add_comment(" 15\n 16", COMM_COLON_TRAILING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.add_comment(" 17", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 18", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!t3"));
    ___(ps.add_comment(" 19", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 20", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.add_comment(" 21", COMM_TRAILING));
    ___(ps.add_comment(" 22", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 23", COMM_LEADING));
    ___(ps.set_key_anchor("a4"));
    ___(ps.set_key_tag("!t4"));
    ___(ps.set_key_scalar_plain("k4"));
    ___(ps.add_comment(" 24", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 25", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a5"));
    ___(ps.add_comment(" 26", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 27", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!t5"));
    ___(ps.add_comment(" 28", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 29", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("v5"));
    ___(ps.add_comment(" 30", COMM_TRAILING));
    ___(ps.add_comment(" 31", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 32", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapQmrk2_1_1,
             "# 1"                "\n"
             "&a1 !t1"            "\n"
             "? # 6"              "\n"
             "  # 7"              "\n"
             "  &a2 !t2 k2"       "\n"
             "# 14"               "\n"
             ": # 15"             "\n"
             "  # 16"             "\n"
             "  &a3 !t3 v3"       "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[LEADING] 1"                 "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_TRAILING_QMRK] 6\\n 7"  "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[COLON_TRAILING] 15\\n 16"   "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.add_comment(" 6\n 7", COMM_KEY_TRAILING_QMRK));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.add_comment(" 15\n 16", COMM_COLON_TRAILING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_1_2,
             "&a1 # 2"            "\n"
             "!t1"                "\n"
             "?"                  "\n"
             "  # 7"              "\n"
             "  &a2 !t2 k2"       "\n"
             "# 14"               "\n"
             ":"                  "\n"
             "  # 16"             "\n"
             "  &a3 !t3 v3"       "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 2"     "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 7"      "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 16"     "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 2", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 7", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.add_comment(" 16", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapQmrk2_2_1,
             "&a1 # 2"            "\n"
             "!t1"                "\n"
             "?"                  "\n"
             "  # 7"              "\n"
             "  &a2 !t2 k2"       "\n"
             "# 14"               "\n"
             ":"                  "\n"
             "  # 16"             "\n"
             "  &a3 !t3 v3"       "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 2"     "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 7"      "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 16"     "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 2", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 7", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.add_comment(" 16", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_2_2,
             "&a1 # 2"            "\n"
             "!t1"                "\n"
             "? &a2 # 8"          "\n"
             "  !t2 k2"           "\n"
             "# 14"               "\n"
             ": &a3 # 17"         "\n"
             "  !t3 v3"           "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 2"     "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 8"     "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 17"    "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 2", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.add_comment(" 8", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.add_comment(" 17", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_2_3,
             "&a1 # 2"            "\n"
             "!t1"                "\n"
             "?"                  "\n"
             "  # 7"              "\n"
             "  &a2 # 8"          "\n"
             "  !t2 k2"           "\n"
             "# 14"               "\n"
             ":"                  "\n"
             "  # 16"             "\n"
             "  &a3 # 17"         "\n"
             "  !t3 v3"           "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 2"     "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 7"      "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 8"     "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 16"     "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 17"    "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 2", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 7", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("a2"));
    ___(ps.add_comment(" 8", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.add_comment(" 16", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.add_comment(" 17", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapQmrk2_3_1,
             "&a1"                "\n"
             "# 3"                "\n"
             "!t1"                "\n"
             "? &a2"              "\n"
             "  # 9"              "\n"
             "  !t2 k2"           "\n"
             "# 14"               "\n"
             ": &a3"              "\n"
             "  # 18"             "\n"
             "  !t3 v3"           "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_TAG_LEADING] 3"         "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_TAG_LEADING] 9"         "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_TAG_LEADING] 18"        "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.add_comment(" 3", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.add_comment(" 9", COMM_KEY_TAG_LEADING));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.add_comment(" 18", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_3_3,
             "&a1 !t1 # 4"        "\n"
             "? &a2 !t2 # 10"      "\n"
             "  k2"               "\n"
             "# 14"               "\n"
             ": &a3 !t3 # 19"     "\n"
             "  v3"               "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "=COMM #[VAL_TAG_TRAILING] 4"        "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_TAG_TRAILING] 10"       "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_TAG_TRAILING] 19"       "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.add_comment(" 4", COMM_VAL_TAG_TRAILING));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.add_comment(" 10", COMM_KEY_TAG_TRAILING));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.add_comment(" 19", COMM_VAL_TAG_TRAILING));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_4_1,
             "&a1 !t1"            "\n"
             "? &a2 !t2"          "\n"
             "  # 11"             "\n"
             "  k2"               "\n"
             "# 14"               "\n"
             ": &a3 !t3"          "\n"
             "  # 20"             "\n"
             "  v3"               "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_LEADING] 11"            "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_LEADING2] 20"           "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.add_comment(" 11", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.add_comment(" 20", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_4_2,
             "&a1 !t1"            "\n"
             "? &a2 !t2 k2 # 12"  "\n"
             "# 14"               "\n"
             ": &a3 !t3 v3 # 21"  "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[KEY_TRAILING] 12"           "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "=COMM #[TRAILING] 21"               "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 12", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.add_comment(" 21", COMM_TRAILING));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_4_3,
             "&a1 !t1"            "\n"
             "? &a2 !t2 k2"       "\n"
             "  # 13"             "\n"
             "# 14"               "\n"
             ": &a3 !t3 v3"       "\n"
             "# 22"               "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[KEY_FOOTER] 13"             "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "=COMM #[FOOTER] 22"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 13", COMM_KEY_FOOTER));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.add_comment(" 22", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapQmrk2_4_4,
             "&a1 !t1"            "\n"
             "? &a2 !t2"          "\n"
             "  # 11"             "\n"
             "  k2 # 12"          "\n"
             "  # 13"             "\n"
             "# 14"               "\n"
             ": &a3 !t3"          "\n"
             "  # 20"             "\n"
             "  v3 # 21"          "\n"
             "  # 22"             "\n"
             ,
             ""
             "+STR"                               "\n"
             "+DOC"                               "\n"
             "+MAP &a1 <!t1>"                     "\n"
             "=COMM #[KEY_LEADING] 11"            "\n"
             "=VAL &a2 <!t2> :k2"                 "\n"
             "=COMM #[KEY_TRAILING] 12"           "\n"
             "=COMM #[KEY_FOOTER] 13"             "\n"
             "=COMM #[COLON_LEADING] 14"          "\n"
             "=COMM #[VAL_LEADING2] 20"           "\n"
             "=VAL &a3 <!t3> :v3"                 "\n"
             "=COMM #[TRAILING] 21"               "\n"
             "=COMM #[FOOTER] 22"                 "\n"
             "-MAP"                               "\n"
             "-DOC"                               "\n"
             "-STR"                               "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!t1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a2"));
    ___(ps.set_key_tag("!t2"));
    ___(ps.add_comment(" 11", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("k2"));
    ___(ps.add_comment(" 12", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 13", COMM_KEY_FOOTER));
    ___(ps.add_comment(" 14", COMM_COLON_LEADING));
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!t3"));
    ___(ps.add_comment(" 20", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("v3"));
    ___(ps.add_comment(" 21", COMM_TRAILING));
    ___(ps.add_comment(" 22", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMap1,
             "# 1"                          "\n"
             "foo: 0 # 2"                   "\n"
             "# 3"                          "\n"
             "bar: 1 # 4"                   "\n"
             "# 5"                          "\n"
             "map: # 6"                     "\n"
             "  # 7"                        "\n"
             "  mapchild: yes # 8"          "\n"
             "  # 9"                        "\n"
             "# 10"                         "\n"
             "seq: # 11"                    "\n"
             "  # 12"                       "\n"
             "  - seqchild # 13"            "\n"
             "  # 14"                       "\n"
             "# 15"                         "\n"
             "scalar: # 16"                 "\n"
             "  # 17"                       "\n"
             "  val # 18"                   "\n"
             "  # 19"                       "\n"
             "# 20"                         "\n"
             ,
             "+STR"                         "\n"
             "+DOC"                         "\n"
             "+MAP"                         "\n"
             "=COMM #[LEADING] 1"           "\n"
             "=VAL :foo"                    "\n"
             "=VAL :0"                      "\n"
             "=COMM #[TRAILING] 2"          "\n"
             "=COMM #[LEADING] 3"           "\n"
             "=VAL :bar"                    "\n"
             "=VAL :1"                      "\n"
             "=COMM #[TRAILING] 4"          "\n"
             "=COMM #[LEADING] 5"           "\n"
             "=VAL :map"                    "\n"
             "=COMM #[COLON_TRAILING] 6"    "\n"
             "+MAP"                         "\n"
             "=COMM #[LEADING] 7"           "\n"
             "=VAL :mapchild"               "\n"
             "=VAL :yes"                    "\n"
             "=COMM #[TRAILING] 8"          "\n"
             "=COMM #[FOOTER] 9"            "\n"
             "-MAP"                         "\n"
             "=COMM #[LEADING] 10"          "\n"
             "=VAL :seq"                    "\n"
             "=COMM #[COLON_TRAILING] 11"   "\n"
             "+SEQ"                         "\n"
             "=COMM #[LEADING] 12"          "\n"
             "=VAL :seqchild"               "\n"
             "=COMM #[TRAILING] 13"         "\n"
             "=COMM #[FOOTER] 14"           "\n"
             "-SEQ"                         "\n"
             "=COMM #[LEADING] 15"          "\n"
             "=VAL :scalar"                 "\n"
             "=COMM #[COLON_TRAILING] 16"   "\n"
             "=COMM #[VAL_LEADING] 17"      "\n"
             "=VAL :val"                    "\n"
             "=COMM #[TRAILING] 18"         "\n"
             "=COMM #[FOOTER] 19"           "\n"
             "-MAP"                         "\n"
             "=COMM #[FOOTER] 20"           "\n"
             "-DOC"                         "\n"
             "-STR"                         "\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 5", COMM_LEADING));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment(" 6", COMM_COLON_TRAILING));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 7", COMM_LEADING));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment(" 8", COMM_TRAILING));
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.add_comment(" 10", COMM_LEADING));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment(" 11", COMM_COLON_TRAILING));
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 12", COMM_LEADING));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment(" 13", COMM_TRAILING));
    ___(ps.add_comment(" 14", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_sibling());
    ___(ps.add_comment(" 15", COMM_LEADING));
    ___(ps.set_key_scalar_plain("scalar"));
    ___(ps.add_comment(" 16", COMM_COLON_TRAILING));
    ___(ps.add_comment(" 17", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.add_comment(" 18", COMM_TRAILING));
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 20", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockMapMultiline0,
            "# leading\n"
            "# comment\n"
            "foo: bar # trailing\n"
            "         # comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=COMM #[LEADING] leading\\n comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COMM #[TRAILING] trailing\\n comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" leading\n comment", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" trailing\n comment", COMM_TRAILING));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapMultiline1,
            "# 1 leading\n"
            "# comment 1\n"
            "foo: # 2 trailing\n"
            "     # comment 2\n"
            "  bar # 3 trailing\n"
            "      # comment 3\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=COMM #[LEADING] 1 leading\\n comment 1\n"
            "=VAL :foo\n"
            "=COMM #[COLON_TRAILING] 2 trailing\\n comment 2\n"
            "=VAL :bar\n"
            "=COMM #[TRAILING] 3 trailing\\n comment 3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1 leading\n comment 1", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.add_comment(" 2 trailing\n comment 2", COMM_COLON_TRAILING));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" 3 trailing\n comment 3", COMM_TRAILING));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapMultiline2,
            "# leading\n"
            "# comment\n"
            "foo: bar # trailing\n"
            "         # comment\n"
            "# leading2\n"
            "# comment2\n"
            "foo2: bar2 # trailing2\n"
            "           # comment2\n"
            "# footer4\n"
            "# comment4\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=COMM #[LEADING] leading\\n comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COMM #[TRAILING] trailing\\n comment\n"
            "=COMM #[LEADING] leading2\\n comment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=COMM #[TRAILING] trailing2\\n comment2\n"
            "=COMM #[FOOTER] footer4\\n comment4\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" leading\n comment", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" trailing\n comment", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" leading2\n comment2", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment(" trailing2\n comment2", COMM_TRAILING));
    ___(ps.add_comment(" footer4\n comment4", COMM_FOOTER));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapMultiline3,
             "# leading comment for foo"      "\n"
             "# with several lines"           "\n"
             "#"                              "\n"
             "# including empty lines"        "\n"
             "  #"                            "\n"
             "  #"                            "\n"
             "  # not necessarily aligned"    "\n"
             "#"                              "\n"
             "# also not necessarily aligned" "\n"
             "foo: bar # trailing comment for bar"     "\n"
             "         # with several lines"           "\n"
             "         #"                              "\n"
             "         # including empty lines"        "\n"
             "           #"                            "\n"
             "           #"                            "\n"
             "           # not necessarily aligned"    "\n"
             "         #"                              "\n"
             "         # also not necessarily aligned" "\n"
             ,
             "# leading comment for foo"      "\n"
             "# with several lines"           "\n"
             "#"                              "\n"
             "# including empty lines"        "\n"
             "#"                              "\n"
             "#"                              "\n"
             "# not necessarily aligned"      "\n"
             "#"                              "\n"
             "# also not necessarily aligned" "\n"
             "foo: bar # trailing comment for bar"     "\n"
             "         # with several lines"           "\n"
             "         #"                              "\n"
             "         # including empty lines"        "\n"
             "         #"                              "\n"
             "         #"                              "\n"
             "         # not necessarily aligned"      "\n"
             "         #"                              "\n"
             "         # also not necessarily aligned" "\n"
             ,
             "+STR\n"
             "+DOC\n"
             "+MAP\n"
             "=COMM #[LEADING] leading comment for foo\\n with several lines\\n\\n"
                             " including empty lines\\n\\n\\n"
                             " not necessarily aligned\\n\\n"
                             " also not necessarily aligned"   "\n"
             "=VAL :foo\n"
             "=VAL :bar\n"
             "=COMM #[TRAILING] trailing comment for bar\\n with several lines\\n\\n"
                             " including empty lines\\n\\n\\n"
                             " not necessarily aligned\\n\\n"
                             " also not necessarily aligned"   "\n"
             "-MAP\n"
             "-DOC\n"
             "-STR\n"
             "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" leading comment for foo\n with several lines\n\n"
                       " including empty lines\n\n\n"
                       " not necessarily aligned\n\n"
                       " also not necessarily aligned", COMM_LEADING));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment(" trailing comment for bar\n with several lines\n\n"
                       " including empty lines\n\n\n"
                       " not necessarily aligned\n\n"
                       " also not necessarily aligned", COMM_TRAILING));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
