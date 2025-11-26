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

COMMENT_TEST(BlockSeqMinimal,
             "# 1"                         "\n"
             "- val1 # 2"                  "\n"
             "# 3"                         "\n"
             "- val2 # 4"                  "\n"
             "# 5"                         "\n"
             ,
             "# 1"                         "\n"
             "- val1 # 2"                  "\n"
             "       # 3"                  "\n"
             "- val2 # 4"                  "\n"
             "       # 5"                  "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ"                        "\n"
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
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2\n 3", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4\n 5", COMM_TRAILING));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(BlockSeqMinimalWithSep1,
             "# 1"                         "\n"
             "- val1 # 2 ~"                "\n"
             "# 3"                         "\n"
             "- val2 # 4 ~"                "\n"
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
             "-SEQ"                        "\n"
             "=COMM #[FOOTER] 5"           "\n"
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
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqMinimalWithSep2,
             "# 1"                         "\n"
             "- val1 # 2 ~"                "\n"
             "  # 2.1 ~"                   "\n"
             "# 3"                         "\n"
             "- val2 # 4 ~"                "\n"
             "  # 4.1 ~"                   "\n"
             "# 5"                         "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[FOOTER] 2.1"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 4.1"         "\n"
             "-SEQ"                        "\n"
             "=COMM #[FOOTER] 5"           "\n"
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
    ___(ps.add_comment(" 2.1", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 4.1", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqMinimalWithSep3,
             "# 1"                         "\n"
             "- # 1.1"                     "\n"
             "  val1 # 2 ~"                "\n"
             "  # 2.1 ~"                   "\n"
             "# 3"                         "\n"
             "- # 3.1"                     "\n"
             "  val2 # 4 ~"                "\n"
             "  # 4.1 ~"                   "\n"
             "# 5"                         "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+SEQ"                        "\n"
             "=COMM #[LEADING] 1"          "\n"
             "=COMM #[VAL_DASH_TRAILING] 1.1""\n"
             "=VAL :val1"                  "\n"
             "=COMM #[TRAILING] 2"         "\n"
             "=COMM #[FOOTER] 2.1"         "\n"
             "=COMM #[LEADING] 3"          "\n"
             "=COMM #[VAL_DASH_TRAILING] 3.1""\n"
             "=VAL :val2"                  "\n"
             "=COMM #[TRAILING] 4"         "\n"
             "=COMM #[FOOTER] 4.1"         "\n"
             "-SEQ"                        "\n"
             "=COMM #[FOOTER] 5"           "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 1.1", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_comment(" 2", COMM_TRAILING));
    ___(ps.add_comment(" 2.1", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 3", COMM_LEADING));
    ___(ps.add_comment(" 3.1", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 4.1", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 5", COMM_FOOTER));
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
             "# 1"                                 "\n"
             "- # 2"                               "\n"
             "  # 3"                               "\n"
             "  a # 4"                             "\n"
             "    # 5"                             "\n"
             "    # 6"                             "\n"
             "- # 7"                               "\n"
             "  # 8"                               "\n"
             "  b # 9"                             "\n"
             "    # 10"                            "\n"
             "    # 11"                            "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=COMM #[LEADING] 1"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2\\n 3"   "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 4\\n 5\\n 6"       "\n"
             "=COMM #[VAL_DASH_TRAILING] 7\\n 8"   "\n"
             "=VAL :b"                             "\n"
             "=COMM #[TRAILING] 9\\n 10\\n 11"     "\n"
             "-SEQ"                                "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 2\n 3", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4\n 5\n 6", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 7\n 8", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9\n 10\n 11", COMM_TRAILING));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithSep0,
             "# 1"                                 "\n"
             "- # 2"                               "\n"
             "  a # 4 ~"                           "\n"
             "  # 5 ~"                             "\n"
             "# 6"                                 "\n"
             "- # 7"                               "\n"
             "  b # 9 ~"                           "\n"
             "  # 10 ~"                            "\n"
             "# 11"                                "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=COMM #[LEADING] 1"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2"        "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 4"                 "\n"
             "=COMM #[FOOTER] 5"                   "\n"
             "=COMM #[LEADING] 6"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 7"        "\n"
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
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_TRAILING));
    ___(ps.add_comment(" 5", COMM_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 6", COMM_LEADING));
    ___(ps.add_comment(" 7", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9", COMM_TRAILING));
    ___(ps.add_comment(" 10", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithSep1,
             "# 1"                                 "\n"
             "- # 2 ~"                             "\n"
             "  # 3"                               "\n"
             "  a # 4 ~"                           "\n"
             "  # 5 ~"                             "\n"
             "# 6"                                 "\n"
             "- # 7 ~"                             "\n"
             "  # 8"                               "\n"
             "  b # 9 ~"                           "\n"
             "  # 10 ~"                            "\n"
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
             "=COMM #[VAL_DASH_TRAILING] 7"        "\n"
             "=COMM #[VAL_LEADING] 8"              "\n"
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
    ___(ps.add_comment(" 7", COMM_VAL_DASH_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9", COMM_TRAILING));
    ___(ps.add_comment(" 10", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 11", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqNested0WithSep,
             "# 0"                                 "\n"
             "- # dash"                            "\n"
             "  a # 1 ~"                           "\n"
             "  # 1.1 ~"                           "\n"
             "# 2"                                 "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=COMM #[LEADING] 0"                  "\n"
             "=COMM #[VAL_DASH_TRAILING] dash"     "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 1"                 "\n"
             "=COMM #[FOOTER] 1.1"                 "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 2"                   "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 0", COMM_LEADING));
    ___(ps.add_comment(" dash", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 1", COMM_TRAILING));
    ___(ps.add_comment(" 1.1", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 2", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqNested0WithSep2,
             "# 0"                           "\n"
             "- a # 1 ~"                     "\n"
             "  # 1.1 ~"                     "\n"
             "# 2"                           "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=COMM #[LEADING] 0"                  "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 1"                 "\n"
             "=COMM #[FOOTER] 1.1"                 "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 2"                   "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
            ___(ps.add_comment(" 0", COMM_LEADING));
            ___(ps.set_val_scalar_plain("a"));
            ___(ps.add_comment(" 1", COMM_TRAILING));
            ___(ps.add_comment(" 1.1", COMM_FOOTER));
          ___(ps.end_seq_block());
          ___(ps.add_comment(" 2", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqNested1WithSep,
             "- - - - a # 1 ~"                     "\n"
             "        # 1.1 ~"                     "\n"
             "      # 2 ~"                         "\n"
             "    # 3"                             "\n"
             "    - b # 4 ~"                       "\n"
             "      # 4.1 ~"                       "\n"
             "    # 5 ~"                           "\n"
             "  # 6"                               "\n"
             "  - c # 7 ~"                         "\n"
             "    # 7.1 ~"                         "\n"
             "  # 8 ~"                             "\n"
             "# 9"                                 "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "+SEQ"                                "\n"
             "+SEQ"                                "\n"
             "+SEQ"                                "\n"
             "=VAL :a"                             "\n"
             "=COMM #[TRAILING] 1"                 "\n"
             "=COMM #[FOOTER] 1.1"                 "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 2"                   "\n"
             "=COMM #[LEADING] 3"                  "\n"
             "=VAL :b"                             "\n"
             "=COMM #[TRAILING] 4"                 "\n"
             "=COMM #[FOOTER] 4.1"                 "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 5"                   "\n"
             "=COMM #[LEADING] 6"                  "\n"
             "=VAL :c"                             "\n"
             "=COMM #[TRAILING] 7"                 "\n"
             "=COMM #[FOOTER] 7.1"                 "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 8"                   "\n"
             "-SEQ"                                "\n"
             "=COMM #[FOOTER] 9"                   "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
      ___(ps.begin_seq_val_block());
        ___(ps.begin_seq_val_block());
          ___(ps.begin_seq_val_block());
            ___(ps.set_val_scalar_plain("a"));
            ___(ps.add_comment(" 1", COMM_TRAILING));
            ___(ps.add_comment(" 1.1", COMM_FOOTER));
          ___(ps.end_seq_block());
          ___(ps.add_comment(" 2", COMM_FOOTER));
          ___(ps.add_sibling());
          ___(ps.add_comment(" 3", COMM_LEADING));
          ___(ps.set_val_scalar_plain("b"));
          ___(ps.add_comment(" 4", COMM_TRAILING));
          ___(ps.add_comment(" 4.1", COMM_FOOTER));
        ___(ps.end_seq_block());
        ___(ps.add_comment(" 5", COMM_FOOTER));
        ___(ps.add_sibling());
        ___(ps.add_comment(" 6", COMM_LEADING));
        ___(ps.set_val_scalar_plain("c"));
        ___(ps.add_comment(" 7", COMM_TRAILING));
        ___(ps.add_comment(" 7.1", COMM_FOOTER));
      ___(ps.end_seq_block());
      ___(ps.add_comment(" 8", COMM_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 9", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithTagAndAnchor1,
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
             "# 1"                                                 "\n"
             "- # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  &aa # 4"                                           "\n"
             "      # 5"                                           "\n"
             "  !atag # 6"                                         "\n"
             "        # 7"                                         "\n"
             "  a # 8"                                             "\n"
             "    # 9"                                             "\n"
             "    # 10"                                            "\n"
             "- # 11"                                              "\n"
             "  # 12"                                              "\n"
             "  &bb # 13"                                          "\n"
             "      # 14"                                          "\n"
             "  !btag # 15"                                        "\n"
             "        # 16"                                        "\n"
             "  b # 17"                                            "\n"
             "    # 18"                                            "\n"
             "    # 19"                                            "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "+SEQ"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2\\n 3"                   "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 4\\n 5"                 "\n"
             "=COMM #[VAL_TAG_TRAILING] 6\\n 7"                    "\n"
             "=VAL &aa <!atag> :a"                                 "\n"
             "=COMM #[TRAILING] 8\\n 9\\n 10"                      "\n"
             "=COMM #[VAL_DASH_TRAILING] 11\\n 12"                 "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 13\\n 14"               "\n"
             "=COMM #[VAL_TAG_TRAILING] 15\\n 16"                  "\n"
             "=VAL &bb <!btag> :b"                                 "\n"
             "=COMM #[TRAILING] 17\\n 18\\n 19"                    "\n"
             "-SEQ"                                                "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_LEADING));
    ___(ps.add_comment(" 2\n 3", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_anchor("aa"));
    ___(ps.add_comment(" 4\n 5", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!atag"));
    ___(ps.add_comment(" 6\n 7", COMM_VAL_TAG_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8\n 9\n 10", COMM_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 11\n 12", COMM_VAL_DASH_TRAILING));
    ___(ps.set_val_anchor("bb"));
    ___(ps.add_comment(" 13\n 14", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.set_val_tag("!btag"));
    ___(ps.add_comment(" 15\n 16", COMM_VAL_TAG_TRAILING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 17\n 18\n 19", COMM_TRAILING));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithTagAndAnchor1WithSep,
             "# 1"                                                 "\n"
             "- # 2 ~"                                             "\n"
             "  # 3"                                               "\n"
             "  &aa # 4 ~"                                         "\n"
             "  # 5"                                               "\n"
             "  !atag # 6 ~"                                       "\n"
             "  # 7"                                               "\n"
             "  a # 8 ~"                                           "\n"
             "  # 9 ~"                                             "\n"
             "# 10"                                                "\n"
             "- # 11 ~"                                            "\n"
             "  # 12"                                              "\n"
             "  &bb # 13 ~"                                        "\n"
             "  # 14"                                              "\n"
             "  !btag # 15 ~"                                      "\n"
             "  # 16"                                              "\n"
             "  b # 17 ~"                                          "\n"
             "  # 18 ~"                                            "\n"
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
             "=COMM #[VAL_DASH_TRAILING] 11"                       "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 12"                      "\n"
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
    ___(ps.add_comment(" 11", COMM_VAL_DASH_TRAILING));
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
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 19", COMM_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithTagAndAnchor1WithSep2,
             "# 1"                                                 "\n"
             "- # 2 ~"                                             "\n"
             "  # 2.1 ~"                                           "\n"
             "  # 3"                                               "\n"
             "  &aa # 4 ~"                                         "\n"
             "  # 5"                                               "\n"
             "  !atag # 6 ~"                                       "\n"
             "  # 7"                                               "\n"
             "  a # 8 ~"                                           "\n"
             "  # 9 ~"                                             "\n"
             "# 10"                                                "\n"
             "- # 11 ~"                                            "\n"
             "  # 11.1 ~"                                          "\n"
             "  # 12"                                              "\n"
             "  &bb # 13 ~"                                        "\n"
             "  # 14"                                              "\n"
             "  !btag # 15 ~"                                      "\n"
             "  # 16"                                              "\n"
             "  b # 17 ~"                                          "\n"
             "  # 18 ~"                                            "\n"
             "# 19"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "+SEQ"                                                "\n"
             "=COMM #[LEADING] 1"                                  "\n"
             "=COMM #[VAL_DASH_TRAILING] 2"                        "\n"
             "=COMM #[VAL_LEADING] 2.1"                            "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 3"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 4"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 5"                          "\n"
             "=COMM #[VAL_TAG_TRAILING] 6"                         "\n"
             "=COMM #[VAL_LEADING2] 7"                             "\n"
             "=VAL &aa <!atag> :a"                                 "\n"
             "=COMM #[TRAILING] 8"                                 "\n"
             "=COMM #[FOOTER] 9"                                   "\n"
             "=COMM #[LEADING] 10"                                 "\n"
             "=COMM #[VAL_DASH_TRAILING] 11"                       "\n"
             "=COMM #[VAL_LEADING] 11.1"                           "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 12"                      "\n"
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
    ___(ps.add_comment(" 2.1", COMM_VAL_LEADING));
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
    ___(ps.add_comment(" 11", COMM_VAL_DASH_TRAILING));
    ___(ps.add_comment(" 11.1", COMM_VAL_LEADING));
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
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 19", COMM_FOOTER));
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
