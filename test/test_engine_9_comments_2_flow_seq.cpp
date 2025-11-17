#include "./test_lib/test_engine.hpp"

#ifndef RYML_WITH_COMMENTS
#error this test requires RYML_WITH_COMMENTS to be defined
#endif

namespace c4 {
namespace yml {

constexpr const bool multiline = true;
constexpr const bool singleline = false;

// WARNING: don't use raw string literals -- g++-4.8 cannot accept
// them as macro arguments

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

COMMENT_TEST(FlowSeqMinimalBaseSingleLine1,
             "# 1"                         "\n"
             "[val1, val2] # 2"            "\n"
             "# 3"                         "\n"
             ,
             "# 1"                         "\n"
             "[val1,val2] # 2"             "\n"
             "            # 3"             "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "+SEQ []"                     "\n"
             "=VAL :val1"                  "\n"
             "=VAL :val2"                  "\n"
             "-SEQ"                        "\n"
             "=COMM #[TRAILING] 2\\n 3"    "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_comment(" 2\n 3", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimalBaseSingleLine1WithSep,
             "# 1"                         "\n"
             "[val1, val2] # 2 ~"          "\n"
             "# 3"                         "\n"
             ,
             "# 1"                         "\n"
             "[val1,val2] # 2 ~"           "\n"
             "# 3"                         "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "+SEQ []"                     "\n"
             "=VAL :val1"                  "\n"
             "=VAL :val2"                  "\n"
             "-SEQ"                        "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[FOOTER] 3"           "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_comment(" 3", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimalBaseSingleLine2,
             "# 1"                         "\n"
             "[val1,val2] # 2"             "\n"
             "            # 3"             "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "+SEQ []"                     "\n"
             "=VAL :val1"                  "\n"
             "=VAL :val2"                  "\n"
             "-SEQ"                        "\n"
             "=COMM #[TRAILING] 2\\n 3"    "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_comment(" 2\n 3", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqSingle0,
             "[ # 1"                       "\n"
             "  # 2"                       "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 1\\n 2"          "\n"
             "-SEQ"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 1\n 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqSingle1,
             "["                           "\n"
             "  # 1"                       "\n"
             "  # 2"                       "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=COMM #[VAL_BRACKET_LEADING] 1\\n 2"          "\n"
             "-SEQ"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 1\n 2", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal0,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2"                 "\n"
             "  # 3"                       "\n"
             "  val2 # 4"                  "\n"
             "  # 5"                       "\n"
             "]"                           "\n"
             ,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2"                 "\n"
             "        # 3"                 "\n"
             "  val2 # 4"                  "\n"
             "       # 5"                  "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2\\n 3"    "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4\\n 5"    "\n"
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
    ___(ps.add_comment(" 2\n 3", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4\n 5", COMM_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal0WithSep,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2 ~"               "\n"
             "  # 3"                       "\n"
             "  val2 # 4 ~"                "\n"
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
             "=COMM #[VAL_BRACKET_LEADING] 5"    "\n"
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
    ___(ps.add_comment(" 5", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal0WithSepAndComma,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2 ~"               "\n"
             "  # 3"                       "\n"
             "  val2 # 4 ~"                "\n"
             "  # 5"                       "\n"
             "  , # 6 ~"                   "\n"
             "  # 7"                       "\n"
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
             "=COMM #[VAL_TRAILING] 4"     "\n"
             "=COMM #[COMMA_LEADING] 5"    "\n"
             "=COMM #[TRAILING] 6"         "\n"
             "=COMM #[VAL_BRACKET_LEADING] 7"    "\n"
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
    ___(ps.add_comment(" 4", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 6", COMM_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal1WithSep,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2 ~"               "\n"
             "  # 3"                       "\n"
             "  val2 # 4 ~"                "\n"
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
             "=COMM #[VAL_BRACKET_LEADING] 5" "\n"
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
    ___(ps.add_comment(" 5", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal1WithTrailingComma,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2"                 "\n"
             "  # 3"                       "\n"
             "  val2, # 4"                 "\n" // note the trailing comma
             "  # 5"                       "\n"
             "]"                           "\n"
             ,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2"                 "\n"
             "        # 3"                 "\n"
             "  val2 # 4"                  "\n"
             "       # 5"                  "\n"
             "]"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ []"                     "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2\\n 3"    "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4\\n 5"    "\n"
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
    ___(ps.add_comment(" 2\n 3", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4\n 5", COMM_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMinimal1WithTrailingCommaWithSep,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2 ~"               "\n"
             "  # 3"                       "\n"
             "  val2, # 4 ~"               "\n" // note the trailing comma
             "  # 5"                       "\n"
             "]"                           "\n"
             ,
             "["                           "\n"
             "  # 1"                       "\n"
             "  val1, # 2 ~"               "\n"
             "  # 3"                       "\n"
             "  val2 # 4 ~"                "\n"
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
             "=COMM #[VAL_BRACKET_LEADING] 5"          "\n"
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
    ___(ps.add_comment(" 5", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqBasic0,
             "# 1"                                     "\n"
             "[ # 2"                                   "\n"
             "  # 3"                                   "\n"
             "  a, # 6"                                "\n"
             "     # 7"                                "\n"
             "  b # 10"                                "\n"
             "    # 11"                                "\n"
             "] # 12"                                  "\n"
             "  # 13"                                  "\n"
             ,
             "+STR"                                    "\n"
             "+DOC"                                    "\n"
             "=COMM #[LEADING] 1"                      "\n"
             "+SEQ []"                                 "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"    "\n"
             "=VAL :a"                                 "\n"
             "=COMM #[TRAILING] 6\\n 7"                "\n"
             "=VAL :b"                                 "\n"
             "=COMM #[TRAILING] 10\\n 11"              "\n"
             "-SEQ"                                    "\n"
             "=COMM #[TRAILING] 12\\n 13"              "\n"
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
    ___(ps.add_comment(" 6\n 7", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 10\n 11", COMM_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 12\n 13", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqBasic1,
             "# 1"                                     "\n"
             "[ # 2"                                   "\n"
             "  # 3"                                   "\n"
             "  a # 4"                                 "\n"
             "    # 5"                                 "\n"
             "  , # 6"                                 "\n"
             "    # 7"                                 "\n"
             "  b # 8"                                 "\n"
             "    # 9"                                 "\n"
             "  , # 10"                                "\n"
             "    # 11"                                "\n"
             "] # 12"                                  "\n"
             "  # 13"                                  "\n"
             ,
             "+STR"                                    "\n"
             "+DOC"                                    "\n"
             "=COMM #[LEADING] 1"                      "\n"
             "+SEQ []"                                 "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"    "\n"
             "=VAL :a"                                 "\n"
             "=COMM #[VAL_TRAILING] 4\\n 5"            "\n"
             "=COMM #[TRAILING] 6\\n 7"                "\n"
             "=VAL :b"                                 "\n"
             "=COMM #[VAL_TRAILING] 8\\n 9"            "\n"
             "=COMM #[TRAILING] 10\\n 11"              "\n"
             "-SEQ"                                    "\n"
             "=COMM #[TRAILING] 12\\n 13"              "\n"
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
    ___(ps.add_comment(" 4\n 5", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 6\n 7", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 8\n 9", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 10\n 11", COMM_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 12\n 13", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqBasic1WithSep,
             "# 1"                                     "\n"
             "[ # 2 ~"                                 "\n"
             "  # 3"                                   "\n"
             "  a # 4 ~"                               "\n"
             "  # 5"                                   "\n"
             "  , # 6 ~"                               "\n"
             "  # 7"                                   "\n"
             "  b # 8 ~"                               "\n"
             "  # 9"                                   "\n"
             "  , # 10 ~"                              "\n"
             "  # 11"                                  "\n"
             "] # 12 ~"                                "\n"
             "# 13"                                    "\n"
             ,
             "+STR"                                    "\n"
             "+DOC"                                    "\n"
             "=COMM #[LEADING] 1"                      "\n"
             "+SEQ []"                                 "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"         "\n"
             "=COMM #[LEADING] 3"                      "\n"
             "=VAL :a"                                 "\n"
             "=COMM #[VAL_TRAILING] 4"                 "\n"
             "=COMM #[COMMA_LEADING] 5"                "\n"
             "=COMM #[TRAILING] 6"                     "\n"
             "=COMM #[LEADING] 7"                      "\n"
             "=VAL :b"                                 "\n"
             "=COMM #[VAL_TRAILING] 8"                 "\n"
             "=COMM #[COMMA_LEADING] 9"                "\n"
             "=COMM #[TRAILING] 10"                    "\n"
             "=COMM #[VAL_BRACKET_LEADING] 11"         "\n"
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
    ___(ps.add_comment(" 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 3", COMM_LEADING));
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
    ___(ps.add_comment(" 11", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 12", COMM_TRAILING));
    ___(ps.add_comment(" 13", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqBasic2,
             "# 1"                                     "\n"
             "["                                       "\n"
             "  # 3"                                   "\n"
             "  a"                                     "\n"
             "  # 5"                                   "\n"
             "  ,"                                     "\n"
             "  # 7"                                   "\n"
             "  b"                                     "\n"
             "  # 9"                                   "\n"
             "  ,"                                     "\n"
             "  # 11"                                  "\n"
             "]"                                       "\n"
             "# 13"                                    "\n"
             ,
             "+STR"                                    "\n"
             "+DOC"                                    "\n"
             "=COMM #[LEADING] 1"                      "\n"
             "+SEQ []"                                 "\n"
             "=COMM #[LEADING] 3"                      "\n"
             "=VAL :a"                                 "\n"
             "=COMM #[COMMA_LEADING] 5"                "\n"
             "=COMM #[LEADING] 7"                      "\n"
             "=VAL :b"                                 "\n"
             "=COMM #[COMMA_LEADING] 9"                "\n"
             "=COMM #[VAL_BRACKET_LEADING] 11"         "\n"
             "-SEQ"                                    "\n"
             "=COMM #[FOOTER] 13"                      "\n"
             "-DOC"                                    "\n"
             "-STR"                                    "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 5", COMM_COMMA_LEADING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 7", COMM_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 11", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 13", COMM_FOOTER));
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
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment\n"
            "  # 2 continued\n"
            "] # 3 trailing comment\n"
            "  # 3 continued\n"
            "  # 4 footer comment\n"
            "  # 4 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\\n 2 continued\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 3 trailing comment\\n 3 continued\\n 4 footer comment\\n 4 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment\n 2 continued", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 3 trailing comment\n 3 continued\n 4 footer comment\n 4 continued", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqMultiline0WithSep,
            "# 1 leading comment\n"
            "# 1 continued\n"
            "[ # 2 bracket trailing comment ~\n"
            "  # 3 bracket leading comment\n"
            "] # 4 trailing comment\n"
            "  # 4 continued ~\n"
            "# 5 footer comment\n"
            "# 5 continued\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=COMM #[LEADING] 1 leading comment\\n 1 continued\n"
            "+SEQ []\n"
            "=COMM #[VAL_BRACKET_TRAILING] 2 bracket trailing comment\n"
            "=COMM #[VAL_BRACKET_LEADING] 3 bracket leading comment\n"
            "-SEQ\n"
            "=COMM #[TRAILING] 4 trailing comment\\n 4 continued\n"
            "=COMM #[FOOTER] 5 footer comment\\n 5 continued\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1 leading comment\n 1 continued", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2 bracket trailing comment", COMM_VAL_BRACKET_TRAILING));
    ___(ps.add_comment(" 3 bracket leading comment", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 4 trailing comment\n 4 continued", COMM_TRAILING));
    ___(ps.add_comment(" 5 footer comment\n 5 continued", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

COMMENT_TEST(FlowSeqWithTagAndAnchor1,
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
             "# 1"                                                 "\n"
             "[ # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  &a # 4"                                            "\n"
             "     # 5"                                            "\n"
             "  !atag # 6"                                         "\n"
             "        # 7"                                         "\n"
             "  a # 8"                                             "\n"
             "    # 9"                                             "\n"
             "] # 10"                                              "\n"
             "  # 11"                                              "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"                "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 4\\n 5"                 "\n"
             "=COMM #[VAL_TAG_TRAILING] 6\\n 7"                    "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[TRAILING] 8\\n 9"                            "\n"
             "-SEQ"                                                "\n"
             "=COMM #[TRAILING] 10\\n 11"                          "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 4\n 5", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 6\n 7", COMM_VAL_TAG_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8\n 9", COMM_TRAILING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 10\n 11", COMM_TRAILING));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor1WithSep,
             "# 1"                                                 "\n"
             "[ # 2 ~"                                             "\n"
             "  # 3 ~"                                             "\n"
             "  # 4"                                               "\n"
             "  &a # 5 ~"                                          "\n"
             "  # 6"                                               "\n"
             "  !atag # 7 ~"                                       "\n"
             "  # 8"                                               "\n"
             "  a # 9 ~"                                           "\n"
             "  # 10"                                              "\n"
             "  , # 11 ~"                                          "\n"
             "  # 12"                                              "\n"
             "] # 13 ~"                                            "\n"
             "# 14"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"                     "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 4"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 5"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 6"                          "\n"
             "=COMM #[VAL_TAG_TRAILING] 7"                         "\n"
             "=COMM #[VAL_LEADING2] 8"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_TRAILING] 9"                             "\n"
             "=COMM #[COMMA_LEADING] 10"                           "\n"
             "=COMM #[TRAILING] 11"                                "\n"
             "=COMM #[VAL_BRACKET_LEADING] 12"                     "\n"
             "-SEQ"                                                "\n"
             "=COMM #[TRAILING] 13"                                "\n"
             "=COMM #[FOOTER] 14"                                  "\n"
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
    ___(ps.add_comment(" 4", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 5", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 6", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 7", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 10", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 11", COMM_TRAILING));
    ___(ps.add_comment(" 12", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 13", COMM_TRAILING));
    ___(ps.add_comment(" 14", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor1WithSep2,
             "# 1"                                                 "\n"
             "[ # 2 ~"                                             "\n"
             "  # 3 ~"                                             "\n"
             "  # 6"                                               "\n"
             "  !atag # 7 ~"                                       "\n"
             "  # 4"                                               "\n"
             "  &a # 5 ~"                                          "\n"
             "  # 8"                                               "\n"
             "  a # 9 ~"                                           "\n"
             "  # 10"                                              "\n"
             "  , # 11 ~"                                          "\n"
             "  # 12"                                              "\n"
             "] # 13 ~"                                            "\n"
             "# 14"                                                "\n"
             ,
             "# 1"                                                 "\n"
             "[ # 2 ~"                                             "\n"
             "  # 3 ~"                                             "\n"
             "  # 4"                                               "\n"
             "  &a # 5 ~"                                          "\n"
             "  # 6"                                               "\n"
             "  !atag # 7 ~"                                       "\n"
             "  # 8"                                               "\n"
             "  a # 9 ~"                                           "\n"
             "  # 10"                                              "\n"
             "  , # 11 ~"                                          "\n"
             "  # 12"                                              "\n"
             "] # 13 ~"                                            "\n"
             "# 14"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2"                     "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[VAL_TAG_LEADING] 6"                          "\n"
             "=COMM #[VAL_TAG_TRAILING] 7"                         "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 4"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 5"                      "\n"
             "=COMM #[VAL_LEADING2] 8"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_TRAILING] 9"                             "\n"
             "=COMM #[COMMA_LEADING] 10"                           "\n"
             "=COMM #[TRAILING] 11"                                "\n"
             "=COMM #[VAL_BRACKET_LEADING] 12"                     "\n"
             "-SEQ"                                                "\n"
             "=COMM #[TRAILING] 13"                                "\n"
             "=COMM #[FOOTER] 14"                                  "\n"
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
    ___(ps.add_comment(" 6", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 7", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 4", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 5", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 10", COMM_COMMA_LEADING));
    ___(ps.add_comment(" 11", COMM_TRAILING));
    ___(ps.add_comment(" 12", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 13", COMM_TRAILING));
    ___(ps.add_comment(" 14", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor2,
             "# 1"                                                 "\n"
             "["                                                   "\n"
             "  # 3"                                               "\n"
             "  &a"                                                "\n"
             "  # 5"                                               "\n"
             "  !atag"                                             "\n"
             "  # 7"                                               "\n"
             "  a"                                                 "\n"
             "  # 9"                                               "\n"
             "]"                                                   "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[VAL_TAG_LEADING] 5"                          "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_BRACKET_LEADING] 9"                      "\n"
             "-SEQ"                                                "\n"
             "=COMM #[FOOTER] 11"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 5", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor3,
             "# 1"                                                 "\n"
             "["                                                   "\n"
             "  # 3"                                               "\n"
             "  &a !atag"                                          "\n"
             "  # 7"                                               "\n"
             "  a"                                                 "\n"
             "  # 9"                                               "\n"
             "]"                                                   "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[LEADING] 3"                                  "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_BRACKET_LEADING] 9"                      "\n"
             "-SEQ"                                                "\n"
             "=COMM #[FOOTER] 11"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor4,
             "# 1"                                                 "\n"
             "["                                                   "\n"
             "  &a"                                                "\n"
             "  # 5"                                               "\n"
             "  !atag"                                             "\n"
             "  # 7"                                               "\n"
             "  a"                                                 "\n"
             "  # 9"                                               "\n"
             "]"                                                   "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_TAG_LEADING] 5"                          "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_BRACKET_LEADING] 9"                      "\n"
             "-SEQ"                                                "\n"
             "=COMM #[FOOTER] 11"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 5", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowSeqWithTagAndAnchor5,
             "# 1"                                                 "\n"
             "["                                                   "\n"
             "  !atag"                                             "\n"
             "  # 5"                                               "\n"
             "  &a"                                                "\n"
             "  # 7"                                               "\n"
             "  a"                                                 "\n"
             "  # 9"                                               "\n"
             "]"                                                   "\n"
             "# 11"                                                "\n"
             ,
             "# 1"                                                 "\n"
             "["                                                   "\n"
             "  # 5"                                               "\n"
             "  &a !atag"                                          "\n"
             "  # 7"                                               "\n"
             "  a"                                                 "\n"
             "  # 9"                                               "\n"
             "]"                                                   "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[LEADING] 5"                                  "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &a <!atag> :a"                                  "\n"
             "=COMM #[VAL_BRACKET_LEADING] 9"                      "\n"
             "-SEQ"                                                "\n"
             "=COMM #[FOOTER] 11"                                  "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 5", COMM_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING2));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 9", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 11", COMM_FOOTER));
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
