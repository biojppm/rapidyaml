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
