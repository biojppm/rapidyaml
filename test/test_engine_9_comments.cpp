#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++-4.8 cannot accept
// them as macro arguments

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
             "-SEQ"                        "\n"
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

COMMENT_TEST(BlockSeqMinimal,
             "# 1"                       "\n"
             "- val1 # 2"                 "\n"
             "# 3"                       "\n"
             "- val2 # 4"                  "\n"
             "# 5"                       "\n"
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
             "-SEQ"                        "\n"
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


#ifdef WIP
COMMENT_TEST(FlowSeqBasic,
             "# 1"                              "\n"
             "[ # 2"                              "\n"
             "  # 3"                              "\n"
             "  a # 4"                              "\n"
             "  # 5"                              "\n"
             "  , # 6"                              "\n"
             "  # 7"                              "\n"
             "  b # 8"                              "\n"
             "  # 9"                              "\n"
             "  , # 10"                              "\n"
             "  # 11"                              "\n"
             "] # 12"                              "\n"
             "# 13"                              "\n"
             ,
             "+STR"                          "\n"
             "+DOC"                          "\n"
             "=COMM #[VAL_LEADING] 1"         "\n"
             "+SEQ []"                            "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"         "\n"
             "=VAL :a"                         "\n"
             "=COMM #[VAL_TRAILING] 4"         "\n"
             "=COMM #[VAL_FOOTER] 5"         "\n"
             "=COMM #[COMMA_TRAILING] 6"         "\n"
             "=COMM #[VAL_LEADING] 7"         "\n"
             "=VAL :a"                         "\n"
             "=COMM #[VAL_TRAILING] 8"         "\n"
             "=COMM #[VAL_FOOTER] 9"         "\n"
             "=COMM #[COMMA_TRAILING] 10"         "\n"
             "=COMM #[VAL_BRACKET_LEADING] 11"         "\n"
             "-SEQ"         "\n"
             "=COMM #[VAL_TRAILING] 12"         "\n"
             "=COMM #[VAL_FOOTER] 13"         "\n"
             "-DOC"         "\n"
             "-STR"         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_FOOTER));
    ___(ps.add_comment(" 6", COMM_COMMA_TRAILING));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 7", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_VAL_FOOTER));
    ___(ps.add_comment(" 10", COMM_COMMA_TRAILING));
    ___(ps.add_comment(" 11", COMM_VAL_BRACKET_LEADING));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 12", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 13", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasic,
             "# 1"                              "\n"
             "- # 2"                              "\n"
             "  # 3"                              "\n"
             "  a # 4"                              "\n"
             "  # 5"                              "\n"
             "# 6"                              "\n"
             "- # 7"                              "\n"
             "  # 8"                              "\n"
             "  b # 9"                              "\n"
             "  # 10"                              "\n"
             "# 11"                               "\n"
             ,
             "+STR"                          "\n"
             "+DOC"                          "\n"
             "+SEQ"                            "\n"
             "=COMM #[VAL_LEADING] 1"         "\n"
             "=COMM #[VAL_TRAILING_DASH] 2\\n 3"         "\n"
             "=VAL :a"                         "\n"
             "=COMM #[VAL_TRAILING] 4"         "\n"
             "=COMM #[VAL_FOOTER] 5"         "\n"
             "=COMM #[VAL_LEADING] 6"         "\n"
             "=COMM #[VAL_TRAILING_DASH] 7\\n 8"         "\n"
             "=VAL :b"                         "\n"
             "=COMM #[VAL_TRAILING] 9"         "\n"
             "=COMM #[VAL_FOOTER] 10"         "\n"
             "-SEQ"         "\n"
             "=COMM #[VAL_FOOTER] 11"         "\n"
             "-DOC"         "\n"
             "-STR"         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING_DASH));
    ___(ps.add_comment(" 2\n 3", COMM_VAL_TRAILING_DASH));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 6", COMM_VAL_LEADING_DASH));
    ___(ps.add_comment(" 7\n 8", COMM_VAL_TRAILING_DASH));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 9", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 10", COMM_VAL_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 11", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapBasic,
             "# 1"                              "\n"
             "{ # 2"                              "\n"
             "  # 3"                              "\n"
             "  a # 4"                              "\n"
             "  # 5"                              "\n"
             "  : # 6"                              "\n"
             "  # 7"                              "\n"
             "  b # 8"                              "\n"
             "  # 9"                              "\n"
             "} # 10"                              "\n"
             "# 11"                              "\n"
             ,
             "+STR"                          "\n"
             "+DOC"                          "\n"
             "=COMM #[VAL_LEADING] 1"         "\n"
             "+MAP {}"                            "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"         "\n"
             "=VAL :a"                         "\n"
             "=COMM #[KEY_TRAILING] 4"         "\n"
             "=COMM #[FLOW_LEADING_COLON] 5"         "\n"
             "=COMM #[KEY_TRAILING_COLON] 6"         "\n"
             "=COMM #[VAL_LEADING] 7"         "\n"
             "=VAL :b"         "\n"
             "=COMM #[VAL_TRAILING] 8"         "\n"
             "=COMM #[VAL_FOOTER] 9"         "\n"
             "-MAP"         "\n"
             "=COMM #[VAL_TRAILING] 10"         "\n"
             "=COMM #[VAL_FOOTER] 11"         "\n"
             "-DOC"         "\n"
             "-STR"         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 4", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 5", COMM_KEY_LEADING_COLON));
    ___(ps.add_comment(" 6", COMM_KEY_TRAILING_COLON));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_VAL_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 10", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 11", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockMapBasic,
             "# 1"                              "\n"
             "a: # 2"                              "\n"
             "  # 3"                              "\n"
             "  b # 4"                              "\n"
             "  # 5"                              "\n"
             "# 6"                              "\n"
             "c: # 7"                              "\n"
             "  # 8"                              "\n"
             "  d # 9"                              "\n"
             "  # 10"                              "\n"
             "# 11"                              "\n"
             ,
             "+STR"                          "\n"
             "+DOC"                          "\n"
             "+MAP {}"                            "\n"
             "=COMM #[KEY_LEADING] 1"         "\n"
             "=VAL :a"                         "\n"
             "=COMM #[KEY_TRAILING] 2"         "\n"
             "=COMM #[VAL_LEADING] 3"         "\n"
             "=VAL :b"                         "\n"
             "=COMM #[VAL_TRAILING] 4"         "\n"
             "=COMM #[VAL_FOOTER] 5"         "\n"
             "=COMM #[KEY_LEADING] 6"         "\n"
             "=VAL :c"                         "\n"
             "=COMM #[VAL_LEADING] 7"         "\n"
             "=COMM #[KEY_TRAILING] 8"         "\n"
             "=VAL :d"         "\n"
             "=COMM #[VAL_TRAILING] 9"         "\n"
             "=COMM #[VAL_FOOTER] 10"         "\n"
             "-MAP"         "\n"
             "=COMM #[VAL_FOOTER] 11"         "\n"
             "-DOC"         "\n"
             "-STR"         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 2", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 3", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 4", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 6", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.add_comment(" 7", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.add_comment(" 9", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 10", COMM_VAL_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 11", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//  equivalent to {!!atag &a a: !!btag &b b}
COMMENT_TEST(FlowSeqBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "[ # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  !!atag # 4"                                        "\n"
             "  # 5"                                               "\n"
             "  &a # 6"                                            "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "] # 10"                                              "\n"
             "# 11"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[VAL_LEADING] 1"                              "\n"
             "+SEQ []"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"                "\n"
             "=COMM #[VAL_TAG_TRAILING] 4"                         "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 5"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 6"                      "\n"
             "=COMM #[VAL_LEADING] 7"                              "\n"
             "=VAL <!!atag> &a :a"                                 "\n"
             "=COMM #[VAL_TRAILING] 8"                             "\n"
             "=COMM #[VAL_FOOTER] 9"                               "\n"
             "-SEQ"                                                "\n"
             "=COMM #[VAL_TRAILING] 10"                            "\n"
             "=COMM #[VAL_FOOTER] 11"                              "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_val_tag("!!atag"));
    ___(ps.add_comment(" 4", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("a"));
    ___(ps.add_comment(" 6", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_VAL_FOOTER));
    ___(ps.end_seq_flow(multiline));
    ___(ps.add_comment(" 10", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 11", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//  equivalent to {!!atag &a a: !!btag &b b}
COMMENT_TEST(FlowMapBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "{ # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  !!atag # 4"                                        "\n"
             "  # 5"                                               "\n"
             "  &a # 6"                                            "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "  : # 10"                                            "\n"
             "  # 11"                                              "\n"
             "  !!btag # 12"                                       "\n"
             "  # 13"                                              "\n"
             "  &b # 14"                                           "\n"
             "  # 15"                                              "\n"
             "  b # 16"                                            "\n"
             "  # 17"                                              "\n"
             "} # 18"                                              "\n"
             "# 19"                                                "\n"
             ,                                                     ""
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "=COMM #[VAL_LEADING] 1"                              "\n"
             "+MAP {}"                                             "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 2\\n 3"                "\n"
             "=COMM #[KEY_TAG_TRAILING] 4"                         "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 5"                       "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 6"                      "\n"
             "=COMM #[KEY_LEADING] 7"                              "\n"
             "=VAL <!!atag> &a :a"                                 "\n"
             "=COMM #[KEY_TRAILING] 8"                             "\n"
             "=COMM #[FLOW_LEADING_COLON] 9"                       "\n"
             "=COMM #[KEY_TRAILING_COLON] 10"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 11"                         "\n"
             "=COMM #[VAL_TAG_TRAILING] 12"                        "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 13"                      "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 14"                     "\n"
             "=COMM #[VAL_LEADING] 15"                             "\n"
             "=VAL <!!btag> &b :b"                                 "\n"
             "=COMM #[VAL_TRAILING] 16"                            "\n"
             "=COMM #[VAL_FOOTER] 17"                              "\n"
             "-MAP"                                                "\n"
             "=COMM #[VAL_TRAILING] 18"                            "\n"
             "=COMM #[VAL_FOOTER] 19"                              "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 2\n 3", COMM_VAL_BRACKET_TRAILING));
    ___(ps.set_key_tag("!!atag"));
    ___(ps.add_comment(" 4", COMM_KEY_TAG_TRAILING));
    ___(ps.add_comment(" 5", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("a"));
    ___(ps.add_comment(" 6", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 7", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 9", COMM_KEY_LEADING_COLON));
    ___(ps.add_comment(" 10", COMM_KEY_TRAILING_COLON));
    ___(ps.add_comment(" 11", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!!btag"));
    ___(ps.add_comment(" 12", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 13", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("b"));
    ___(ps.add_comment(" 14", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 15", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 16", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 17", COMM_VAL_FOOTER));
    ___(ps.end_map_flow(multiline));
    ___(ps.add_comment(" 18", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 19", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(BlockSeqBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "- # 2"                                               "\n"
             "  # 3"                                               "\n"
             "  !!atag # 4"                                        "\n"
             "  # 5"                                               "\n"
             "  &aa # 6"                                           "\n"
             "  # 7"                                               "\n"
             "  a # 8"                                             "\n"
             "  # 9"                                               "\n"
             "# 10"                                                "\n"
             "- # 11"                                              "\n"
             "  # 12"                                              "\n"
             "  !!btag # 13"                                       "\n"
             "  # 14"                                              "\n"
             "  &bb # 15"                                          "\n"
             "  # 16"                                              "\n"
             "  b # 17"                                            "\n"
             "  # 18"                                              "\n"
             "# 19"                                                "\n"
             ,
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "+SEQ"                                                "\n"
             "=COMM #[VAL_LEADING] 1"                              "\n"
             "=COMM #[VAL_DASH_TRAILING] 2\\n 3"                   "\n"
             "=COMM #[VAL_TAG_TRAILING] 4"                         "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 5"                       "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 6"                      "\n"
             "=COMM #[VAL_LEADING] 7"                              "\n"
             "=VAL <!!atag> &a :a"                                 "\n"
             "=COMM #[VAL_TRAILING] 8"                             "\n"
             "=COMM #[VAL_FOOTER] 9"                               "\n"
             "-SEQ"                                                "\n"
             "=COMM #[VAL_TRAILING] 10"                            "\n"
             "=COMM #[VAL_FOOTER] 11"                              "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment(" 1", COMM_VAL_LEADING_DASH));
    ___(ps.add_comment(" 2\n 3", COMM_VAL_TRAILING_DASH));
    ___(ps.set_val_tag("!!atag"));
    ___(ps.add_comment(" 4", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 5", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("aa"));
    ___(ps.add_comment(" 6", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 7", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment(" 8", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 9", COMM_VAL_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 10", COMM_VAL_LEADING_DASH));
    ___(ps.add_comment(" 11\n 12", COMM_VAL_TRAILING_DASH));
    ___(ps.set_val_tag("!!btag"));
    ___(ps.add_comment(" 13", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 14", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("bb"));
    ___(ps.add_comment(" 15", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 16", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 17", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 18", COMM_VAL_FOOTER));
    ___(ps.end_seq_block());
    ___(ps.add_comment(" 19", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//  equivalent to {!!atag &a a: !!btag &b b}
COMMENT_TEST(BlockMapBasicWithTagAndAnchor,
             "# 1"                                                 "\n"
             "!!atag # 2"                                        "\n"
             "# 3"                                               "\n"
             "&aa # 4"                                            "\n"
             "# 5"                                               "\n"
             "aaa: # 6"                                             "\n"
             "     # 6.1"                                             "\n"
             "  # 6.2"                                             "\n"
             "  !!btag # 7"                                       "\n"
             "  # 8"                                              "\n"
             "  &bb # 9"                                           "\n"
             "  # 10"                                              "\n"
             "  b # 11"                                            "\n"
             "  # 12"                                              "\n"
             "# 13"                                                "\n"
             "!!ctag # 14"                                        "\n"
             "# 15"                                               "\n"
             "&cc # 16"                                            "\n"
             "# 17"                                               "\n"
             "ccc: # 18"                                             "\n"
             "     # 18.1"                                              "\n"
             "  # 18.2"                                              "\n"
             "  !!dtag # 19"                                       "\n"
             "  # 20"                                              "\n"
             "  &dd # 21"                                           "\n"
             "  # 22"                                              "\n"
             "  d # 23"                                            "\n"
             "  # 24"                                              "\n"
             "# 25"                                               "\n"
             ,                                                     ""
             "+STR"                                                "\n"
             "+DOC"                                                "\n"
             "+MAP"                                                "\n"
             "=COMM #[KEY_TAG_LEADING] 1"                          "\n"
             "=COMM #[KEY_TAG_TRAILING] 2"                         "\n"
             "=COMM #[KEY_ANCHOR_LEADING] 3"                       "\n"
             "=COMM #[KEY_ANCHOR_TRAILING] 4"                      "\n"
             "=COMM #[KEY_LEADING] 5"                              "\n"
             "=VAL <!!atag> &a :a"                                 "\n"
             "=COMM #[COLON_TRAILING] 6"                             "\n"
             "=COMM #[FLOW_LEADING_COLON] 9"                       "\n"
             "=COMM #[KEY_TRAILING_COLON] 10"                      "\n"
             "=COMM #[VAL_TAG_LEADING] 11"                         "\n"
             "=COMM #[VAL_TAG_TRAILING] 12"                        "\n"
             "=COMM #[VAL_ANCHOR_LEADING] 13"                      "\n"
             "=COMM #[VAL_ANCHOR_TRAILING] 14"                     "\n"
             "=COMM #[VAL_LEADING] 15"                             "\n"
             "=VAL <!!btag> &b :b"                                 "\n"
             "=COMM #[VAL_TRAILING] 16"                            "\n"
             "=COMM #[VAL_FOOTER] 17"                              "\n"
             "-MAP"                                                "\n"
             "=COMM #[VAL_TRAILING] 18"                            "\n"
             "=COMM #[VAL_FOOTER] 19"                              "\n"
             "-DOC"                                                "\n"
             "-STR"                                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment(" 1", COMM_KEY_TAG_LEADING));
    ___(ps.add_comment(" 2", COMM_KEY_TAG_TRAILING));
    ___(ps.set_key_tag("!!atag"));
    ___(ps.add_comment(" 3", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("aa"));
    ___(ps.add_comment(" 4", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 5", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("aaa"));
    ___(ps.add_comment(" 6\n 6.1", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 6.2", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!!btag"));
    ___(ps.add_comment(" 7", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 8", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("bb"));
    ___(ps.add_comment(" 9", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 10", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment(" 11", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 12", COMM_VAL_FOOTER));
    ___(ps.add_sibling());
    ___(ps.add_comment(" 13", COMM_KEY_TAG_LEADING));
    ___(ps.add_comment(" 14", COMM_KEY_TAG_TRAILING));
    ___(ps.set_key_tag("!!ctag"));
    ___(ps.add_comment(" 15", COMM_KEY_ANCHOR_LEADING));
    ___(ps.set_key_anchor("cc"));
    ___(ps.add_comment(" 16", COMM_KEY_ANCHOR_TRAILING));
    ___(ps.add_comment(" 17", COMM_KEY_LEADING));
    ___(ps.set_key_scalar_plain("ccc"));
    ___(ps.add_comment(" 18\n 18.1", COMM_KEY_TRAILING));
    ___(ps.add_comment(" 18.2", COMM_VAL_TAG_LEADING));
    ___(ps.set_val_tag("!!dtag"));
    ___(ps.add_comment(" 19", COMM_VAL_TAG_TRAILING));
    ___(ps.add_comment(" 20", COMM_VAL_ANCHOR_LEADING));
    ___(ps.set_val_anchor("dd"));
    ___(ps.add_comment(" 21", COMM_VAL_ANCHOR_TRAILING));
    ___(ps.add_comment(" 22", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.add_comment(" 23", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 24", COMM_VAL_FOOTER));
    ___(ps.end_map_block());
    ___(ps.add_comment(" 25", COMM_VAL_FOOTER));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


COMMENT_TEST(StreamDocValPlain,
             "# 1 leading stream"                             "\n"
             "--- # 2 trailing open"                          "\n"
             "# 3 val leading"                                "\n"
             "foo # 4 val trailing"                           "\n"
             "# 5 val footer"                                 "\n"
             "... # 6 trailing stream close"                  "\n"
             "# 7 footer stream close"                        "\n"
             ,//---------------------------------------------
             "+STR"                            "\n"
             "=COM #[STREAM_LEADING_OPEN] 1 leading stream"                            "\n"
             "+DOC ---"                            "\n"
             "=COM #[DOC_TRAILING_OPEN] 2 trailing open"                            "\n"
             "=COM #[VAL_LEADING] 3 leading val"                            "\n"
             "=VAL :"                            "\n"
             "=COM #[VAL_TRAILING] 4 trailing val"                            "\n"
             "=COM #[VAL_FOOTER] 5 footer val"                            "\n"
             "-DOC ..."                            "\n"
             "=COM #[STREAM_TRAILING_CLOSE] 6 trailing close"                            "\n"
             "=COM #[STREAM_FOOTER_CLOSE] 7 footer close"                            "\n"
             "-STR"                            "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.add_comment(" 1 stream leading open", COMM_STREAM_LEADING_OPEN));
    ___(ps.begin_doc_expl());
    ___(ps.add_comment(" 2 doc trailing open", COMM_DOC_TRAILING_OPEN));
    ___(ps.add_comment(" 3 val leading", COMM_VAL_LEADING));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_comment(" 4 val trailing", COMM_VAL_TRAILING));
    ___(ps.add_comment(" 5 val footer val", COMM_VAL_FOOTER));
    ___(ps.end_doc_expl());
    ___(ps.add_comment(" 6 trailing stream", COMM_STREAM_TRAILING_CLOSE));
    ___(ps.add_comment(" 7 footer stream", COMM_STREAM_FOOTER_CLOSE));
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSketch,
             "---"                                          "\n"
             "# CLV Comment Leading to Val 0"               "\n"
             "val"                                          "\n"
             "---"                                          "\n"
             "# CLK Comment Leading to Key 1.1"             "\n"
             "key: # CTK Comment Trailing Key 1.2"          "\n"
             "  # CLV Comment Leading to Val 1.3"           "\n"
             "  val # CTV Comment Trailing Val 1.4"         "\n"
             "  # CFV Comment Footer Val 1.5"               "\n"
             "# CLK 2"                                      "\n"
             "foo: # CTK 2.1"                               "\n"
             "  bar # CTV 2.2"                              "\n"
             "  # CFV 2.3"                                  "\n"
             "# CLK 2.4"                                    "\n"
             "--- baz # CTV 3"                              "\n"
    /*FIXME*/""                                             "\n"
             "--- # CTT 3.1"                                "\n"
             "bat"                                          "\n"
             "---"/*FIXME*/" "                              "\n"
             "# CFV 4"      /* CFV for the doc node */      "\n"
             "--- # CTT 5"  /* CTC for the doc node */      "\n"
             "# CLV 5.1"    /* CLV for the seq val node */  "\n"
             "- # CLV2 5.2" /* CLV2 for the seq val node */ "\n"
             "  val # CTV 6"                                "\n"
             "  # CFV 6.1"                                  "\n"
             "# CLV 7"                                      "\n"
             "--- # CTT 8"                                  "\n"
    /*FIXME*/""                                             "\n"
             "# CLV 9"                                      "\n"
             "{ # CTV 10"                                   "\n"
             "  # continued 10"                             "\n"
             "# CLK 11"                                     "\n"
             "key: # CTK 12"                                "\n"
             "     # continued 12"                          "\n"
             "# CLV 13"                                     "\n"
             "[ # CTV 14"                                   "\n"
             "  # continued 14"                             "\n"
             "# CLV 15"                                     "\n"
             "a # CTV 16"                                   "\n"
             "  # continued 16"                             "\n"
             ", # CTT 16.1"                                 "\n"
    /*FIXME*/""                                             "\n"
             "# CLV 17"                                     "\n"
             "b, # CTT 17.1"                                "\n"
    /*FIXME*/""                                             "\n"
             "# CLV 17.2"                                   "\n"
             "c # CTV 18"                                   "\n"
             "  # continued 18"                             "\n"
    /*FIXME*/""                                             "\n"
             "# CFV 19"                                     "\n"
             "] # CFV 20"                                   "\n"
             "  # continued 20"                             "\n"
    /*FIXME*/""                                             "\n"
             "# CLV 21"                                     "\n"
             "} # CFV 22"                                   "\n"
             "  # continued 22"                             "\n"
             "# CFV2 23"                                    "\n"
    /*FIXME*/""                                             "\n"
             ,//-----------------------------------------------
             "+STR"                                         "\n"
             "+DOC ---"                                     "\n"
             "=CLV # CLV Comment Leading to Val 0"          "\n"
             "=VAL :val"                                    "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "+MAP"                                         "\n"
             "=CLK # CLK Comment Leading to Key 1.1"        "\n"
             "=VAL :key"                                    "\n"
             "=CTK # CTK Comment Trailing Key 1.2"          "\n"
             "=CLV # CLV Comment Leading to Val 1.3"        "\n"
             "=VAL :val"                                    "\n"
             "=CTV # CTV Comment Trailing Val 1.4"          "\n"
             "=CFV # CFV Comment Footer Val 1.5"            "\n"
             "=CLK # CLK 2"                                 "\n"
             "=VAL :foo"                                    "\n"
             "=CTK # CTK 2.1"                               "\n"
             "=VAL :bar"                                    "\n"
             "=CTV # CTV 2.2"                               "\n"
             "=CFV # CFV 2.3"                               "\n"
             "=CLK # CLK 2.4"                               "\n"
             "-MAP"                                         "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=VAL :baz"                                    "\n"
             "=CTV # CTV 3"                                 "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=CTT # CTT 3.1"                               "\n"
             "=VAL :bat"                                    "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=VAL :"                                       "\n"
             "=CFV # CFV 4"                                 "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=CTT # CTT 5"                                 "\n"
             "+SEQ"                                         "\n"
             "=CLV # CLV 5.1"                               "\n"
             "=CLV2 # CLV2 5.2"                             "\n"
             "=VAL :val"                                    "\n"
             "=CTV # CTV 6"                                 "\n"
             "=CFV # CFV 6.1"                               "\n"
             "=CLV # CLV 7"                                 "\n"
             "-SEQ"                                         "\n"
             "-DOC"                                         "\n"
             "+DOC ---"                                     "\n"
             "=CTT # CTT 8"                                 "\n"
             "=CLV # CLV 9"                                 "\n"
             "+MAP {}"                                      "\n"
             "=CTV # CTV 10\\n continued 10"                "\n"
             "=CLK # CLK 11"                                "\n"
             "=VAL :key"                                    "\n"
             "=CTK # CTK 12\\n continued 12"                "\n"
             "=CLV # CLV 13"                                "\n"
             "+SEQ []"                                      "\n"
             "=CTV # CTV 14\\n continued 14"                "\n"
             "=CLV # CLV 15"                                "\n"
             "=VAL :a"                                      "\n"
             "=CTV # CTV 16\\n continued 16"                "\n"
             "=CTT # CTT 16.1"                              "\n"
             "=CLV # CLV 17"                                "\n"
             "=VAL :b"                                      "\n"
             "=CTT # CTT 17.1"                              "\n"
             "=CLV # CLV 17.2"                              "\n"
             "=VAL :c"                                      "\n"
             "=CTV # CTV 18\\n continued 18"                "\n"
             "=CFV # CFV 19"                                "\n"
             "-SEQ"                                         "\n"
             "=CFV # CFV 20\\n continued 20"                "\n"
             "=CLV # CLV 21"                                "\n"
             "-MAP"                                         "\n"
             "=CFV # CFV 22\\n continued 22"                "\n"
             "=CFV2 # CFV2 23"                              "\n"
             "-DOC"                                         "\n"
             "-STR"                                         "\n"
     )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" CLV Comment Leading to Val 0"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" CLK Comment Leading to Key 1.1"));
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.add_comment_trailing_key(" CTK Comment Trailing Key 1.2"));
    ___(ps.add_comment_leading_val(" CLV Comment Leading to Val 1.3"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.add_comment_trailing_val(" CTV Comment Trailing Val 1.4"));
    ___(ps.add_comment_footer_val(" CFV Comment Footer Val 1.5"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" CLK 2"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.add_comment_trailing_key(" CTK 2.1"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" CTV 2.2"));
    ___(ps.add_comment_footer_val(" CFV 2.3"));
    ___(ps.add_comment_leading_key(" CLK 2.4"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_comment_trailing_val(" CTV 3"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_trailing_token(" CTT 3.1"));
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_comment_footer_val(" CFV 4"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_trailing_token(" CTT 5"));
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment_leading_val(" CLV 5.1"));
    ___(ps.add_comment_leading_val2(" CLV2 5.2"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.add_comment_trailing_val(" CTV 6"));
    ___(ps.add_comment_footer_val(" CFV 6.1"));
    ___(ps.add_comment_leading_val(" CLV 7"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" CLV 5.1"));
    ___(ps.add_comment_leading_val2(" CLV2 5.2"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.add_comment_trailing_val(" CTV 6"));
    ___(ps.add_comment_footer_val(" CFV 6.1"));
    ___(ps.add_comment_leading_val(" CLV 7"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_trailing_token(" CTT 8"));
    ___(ps.add_comment_leading_val(" CLV 9"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 10\n continued 10"));
    ___(ps.add_comment_leading_key(" CLK 11"));
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.add_comment_trailing_key(" CTK 12\n continued 12"));
    ___(ps.add_comment_leading_val(" CLV 13"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 14\n continued 14"));
    ___(ps.add_comment_leading_val(" CLV 15"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_comment_trailing_val(" CTV 16\n continued 16"));
    ___(ps.add_comment_trailing_token(" CTT 16.1"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" CLV 17"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.add_comment_trailing_token(" CTT 17.1"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" CLV 17.2"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.add_comment_trailing_val(" CTV 18\n continued 18"));
    ___(ps.add_comment_footer_val(" CFV 19"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" CFV 20\n continued 20"));
    ___(ps.add_comment_leading_val(" CLV 21"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 22\n continued 22"));
    ___(ps.add_comment_footer_val2(" CFV2 23"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

COMMENT_TEST(CommentDocValEmptyLeading,
             "---"/*FIXME*/" "                              "\n"
             "# CLV 1"                                      "\n"
             ,//-----------------------------------------------
             "+STR"                                         "\n"
             "+DOC ---"                                     "\n"
             "=CLV # CLV 1"                                 "\n"
             "=VAL :"                                       "\n"
             "-DOC"                                         "\n"
             "-STR"                                         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" CLV 1"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentDocValEmptyFooter,
             "---"/*FIXME*/" "                              "\n"
             "# CFV 1"                                      "\n"
             ,//-----------------------------------------------
             "+STR"                                         "\n"
             "+DOC ---"                                     "\n"
             "=VAL :"                                       "\n"
             "=CFV # CFV 1"                                 "\n"
             "-DOC"                                         "\n"
             "-STR"                                         "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_comment_footer_val(" CFV 1"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

COMMENT_TEST(CommentSingle,
            "# single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleNoLeadSpace,
            "#single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV #single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleLeadSpace,
            "#  single comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV #  single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("  single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline0,
            "# single\n"
            "# comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # single\\n comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" single\n comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline1,
            "#\n"
            "# single\n"
            "# comment\n"
            "#\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV #\\n single\\n comment\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\n single\n comment\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline2,
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
            "=CLV #\\n\\n single\\n\\n\\n comment\\n\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\n\n single\n\n\n comment\n\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline3,
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
            "=CLV #\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\n\n  single  \n  \n  \n  comment  \n\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline4Indented1,
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
            "=CLK #\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.add_comment_leading_val("\n\n  single  \n  \n  \n  comment  \n\n"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSingleMultiline4Indented2,
            "a:\n"
            "  b:\n"
            "    c:\n"
            "      #\n"
            "      #\n"
            "      #  single  \n"
            "      #  \n"
            "      #  \n"
            "      #  comment  \n"
            "      #\n"
            "      #\n"
            "      d:\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :b\n"
            "+MAP\n"
            "=VAL :c\n"
            "+MAP\n"
            "=CLK #\\n\\n  single  \\n  \\n  \\n  comment  \\n\\n\n"
            "=VAL :d\n"
            "=VAL :\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
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
    ___(ps.add_comment_leading_key("\n\n  single  \n  \n  \n  comment  \n\n"));
    ___(ps.set_key_scalar_plain("d"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

COMMENT_TEST(CommentDocToken,
             // comments outside docs are ignored
            "# ignored 0"                      "\n"
            "--- # CTN 1"                      "\n"
            "# CLV 2"                          "\n"
            "{ # CTV 3"                        "\n"
            "} # CFV 4"                        "\n"
            "# CFV2 5"                         "\n"
            "... # ignored 6"                  "\n"
            "# ignored 7"                      "\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=CTN # CTN 1\n"
            "=CLV # CLV 2\n"
            "+MAP {}\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_trailing_token(" CTN 1"));
    ___(ps.add_comment_leading_val(" CLV 2"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 3"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 4"));
    ___(ps.add_comment_footer_val2(" CFV2 5"));
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentDocLeading,
            "# leading 1\n"
            "---\n"
            "# leading 2\n"
            "---\n"
            "# leading 3\n"
            ,
            "---\n"
            "# leading 1\n"
            "---\n"
            "# leading 2\n"
            "---\n"
            "# leading 3\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading 1\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading 3\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading 1"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading 3"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentFlowLeading,
            "# leading map\n"
            "{\n"
            "}\n"
            "---\n"
            "# leading seq\n"
            "[\n"
            "]\n"
            ,
            "---\n"
            "# leading map\n"
            "{}\n"
            "---\n"
            "# leading seq\n"
            "[]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading map\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading seq\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentFlowLeading3Map,
            "# leading val 1\n"
            "{ # trailing val 2\n"
            "# leading key 2.1\n"
            "foo: # trailing key 3\n"
            "# leading val 4\n"
            "bar # trailing val 5\n"
   /*FIXME*/"\n"
            "# footer val 5.1\n"
            ", # trailing comma 5.2\n"
   /*FIXME*/"\n"
            "# leading key 6\n"
            "} # footer val 7\n"
            "# footer2 val 8\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading val 1\n"
            "+MAP {}\n"
            "=CTV # trailing val 2\n"
            "=CLK # leading key 2.1\n"
            "=VAL :foo\n"
            "=CTK # trailing key 3\n"
            "=CLV # leading val 4\n"
            "=VAL :bar\n"
            "=CTV # trailing val 5\n"
            "=CFV # footer val 5.1\n"
            "=CTC # trailing comma 5.2\n"
            "=CLK # leading key 6\n"
            "-MAP\n"
            "=CFV # footer val 7\n"
            "=CFV2 # footer2 val 8\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading val 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing val 2"));
    ___(ps.add_comment_leading_key(" leading key 2.1"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.add_comment_trailing_key(" trailing key 3"));
    ___(ps.add_comment_leading_val(" leading val 4"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing val 5"));
    ___(ps.add_comment_footer_val(" footer val 5.1"));
    ___(ps.add_comment_trailing_token(" trailing comma 5.2"));
    ___(ps.add_comment_leading_key(" leading key 6"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" footer val 7"));
    ___(ps.add_comment_footer_val2(" footer2 val 8"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentFlowLeading2,
            "# leading map\n"
            "{\n"
            "  # leading map child\n"
            "}\n"
            "---\n"
            "# leading seq\n"
            "[\n"
            "  # leading seq child\n"
            "]\n"
            ,
            "---\n"
            "# leading map\n"
            "{\n"
            "  # leading map child\n"
            "}\n"
            "---\n"
            "# leading seq\n"
            "[\n"
            "  # leading seq child\n"
            "]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV #leading map\n"
            "+MAP {}\n"
            "=CLV #leading map child\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading seq\n"
            "+SEQ []\n"
            "=CLV #leading seq child\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_val("leading map child"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_leading_val("leading seq child"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapFlowTrailingMultiline,
            "{ # trailing\n"
            "  # comment\n"
            "  # with\n"
            "  # multiple\n"
            "  # lines\n"
            "}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=CTV # trailing\\n comment\\n with\\n multiple\\n lines\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing\n comment\n with\n multiple\n lines"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapFlowLeadingTrailing,
            "# leading comment\n"
            "{ # trailing comment\n"
            "}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading comment\n"
            "+MAP {}\n"
            "=CTV # trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading comment"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlockLeadingTrailing,
            "# leading comment\n"
            "foo: bar # trailing comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=CTV # trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlockLeadingTrailingMultiline,
            "# leading\n"
            "# comment\n"
            "foo: bar # trailing\n"
            "         # comment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading\\n comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=CTV # trailing\\n comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading\n comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing\n comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlockLeadingTrailingMultiline1,
            "# leading\n"
            "# comment\n"
            "foo: # trailing\n"
            "     # kcomment\n"
            "  bar # trailing\n"
            "      # vcomment\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading\\n comment\n"
            "=VAL :foo\n"
            "=CTK # trailing\\n kcomment\n"
            "=VAL :bar\n"
            "=CTV # trailing\\n vcomment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading\n comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.add_comment_trailing_key(" trailing\n kcomment"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing\n vcomment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlockLeadingTrailingMultiline2,
            "# leading\n"
            "# comment\n"
            "foo: bar # trailing\n"
            "         # comment\n"
            "# leading2\n"
            "# comment2\n"
            "foo2: bar2 # trailing2\n"
            "           # comment2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading\\n comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=CTV # trailing\\n comment\n"
            "=CLK # leading2\\n comment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=CTV # trailing2\\n comment2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading\n comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing\n comment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading2\n comment2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment_trailing_val(" trailing2\n comment2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlockLeadingTrailingMultiline3,
            "# leading\n"
            "# comment\n"
            "foo: bar # trailing\n"
            "         # comment\n"
            "# leading2\n"
            "# comment2\n"
            "foo2: bar2 # trailing2\n"
            "           # comment2\n"
            "# leading3\n"
            "# comment3\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading\\n comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=CTV # trailing\\n comment\n"
            "=CLK # leading2\\n comment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=CTV # trailing2\\n comment2\n"
            "=CLV # leading3\\n comment3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading\n comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val(" trailing\n comment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading2\n comment2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment_trailing_val(" trailing2\n comment2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" leading3\n comment3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapFlowSingleTrailingThenLeading,
            "# leading comment 1\n"
            "{ # trailing comment\n"
            "} # footer comment\n"
            "  # footer continued\n"
            "# leading comment 2\n"
            "---\n"
            "# leading comment 1\n"
            "{ # trailing comment\n"
            "} # footer comment\n"
            "# leading comment 2\n"
            "# leading continued 2\n"
            "---\n"
            "# leading comment 1\n"
            "{ # trailing comment\n"
            "} # footer comment\n"
            "  # footer continued\n"
            "# leading comment 2\n"
            "# leading continued 2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading comment 1\n"
            "+MAP {}\n"
            "=CTV # trailing comment\n"
            "-MAP\n"
            "=CFV # footer comment\\n footer continued\n"
            "=CLV # leading comment 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading comment 1\n"
            "+MAP {}\n"
            "=CTV # trailing comment\n"
            "-MAP\n"
            "=CFV # footer comment\n"
            "=CLV # leading comment 2\\n leading continued 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading comment 1\n"
            "+MAP {}\n"
            "=CTV # trailing comment\n"
            "-MAP\n"
            "=CFV # footer comment\\n footer continued\n"
            "=CLV # leading comment 2\\n leading continued 2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" footer comment\n footer continued"));
    ___(ps.add_comment_leading_val(" leading comment 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" footer comment"));
    ___(ps.add_comment_leading_val(" leading comment 2\n leading continued 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" footer comment\n footer continued"));
    ___(ps.add_comment_leading_val(" leading comment 2\n leading continued 2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSeqFlowSingleTrailingThenLeading,
            "# leading comment 1\n"
            "[ # trailing comment\n"
            "] # footer comment\n"
            "  # footer continued\n"
            "# leading comment 2\n"
            "---\n"
            "# leading comment 1\n"
            "[ # trailing comment\n"
            "] # footer comment\n"
            "# leading comment 2\n"
            "# leading continued 2\n"
            "---\n"
            "# leading comment 1\n"
            "[ # trailing comment\n"
            "] # footer comment\n"
            "  # footer continued\n"
            "# leading comment 2\n"
            "# leading continued 2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=CLV # leading comment 1\n"
            "+SEQ []\n"
            "=CTV # trailing comment\n"
            "-SEQ\n"
            "=CFV # footer comment\\n footer continued\n"
            "=CLV # leading comment 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading comment 1\n"
            "+SEQ []\n"
            "=CTV # trailing comment\n"
            "-SEQ\n"
            "=CFV # footer comment\n"
            "=CLV # leading comment 2\\n leading continued 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV # leading comment 1\n"
            "+SEQ []\n"
            "=CTV # trailing comment\n"
            "-SEQ\n"
            "=CFV # footer comment\\n footer continued\n"
            "=CLV # leading comment 2\\n leading continued 2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" footer comment\n footer continued"));
    ___(ps.add_comment_leading_val(" leading comment 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" footer comment"));
    ___(ps.add_comment_leading_val(" leading comment 2\n leading continued 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val(" leading comment 1"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" footer comment\n footer continued"));
    ___(ps.add_comment_leading_val(" leading comment 2\n leading continued 2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapFlow0,
            "{\n"
            "# leading comment for foo\n"
            "foo: 0, # trailing comment for foo\n"
            "# leading comment for bar\n"
            "bar: 1, # trailing comment for bar\n"
            "# leading comment for map\n"
            "map: { # trailing comment for map\n"
            "  # leading comment for mapchild\n"
            "  mapchild: yes # trailing comment for mapchild\n"
            "},\n"
            "# leading comment for seq\n"
            "seq: [ # trailing comment for seq\n"
            "  # leading comment for seqchild\n"
            "  - seqchild # trailing comment for seqchild\n"
            "]\n"
            "}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=CLK # leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=CTV # trailing comment for foo\n"
            "=CLK # leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=CTV # trailing comment for bar\n"
            "=CLK # leading comment for map\n"
            "=VAL :map\n"
            "+MAP {}\n"
            "=CTV # trailing comment for map\n"
            "=CLK # leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=CTV # trailing comment for mapchild\n"
            "-MAP\n"
            "=CLV # leading comment for seq\n"
            "=VAL :seq\n"
            "+SEQ []\n"
            "=CTV # trailing comment for seq\n"
            "=CLV # leading comment for seqchild\n"
            "=VAL :seqchild\n"
            "=CTV # trailing comment for seqchild\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_key(" leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val(" trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val(" trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment for map"));
    ___(ps.add_comment_leading_key(" leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val(" trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" trailing comment for seq"));
    ___(ps.add_comment_leading_val(" leading comment for seqchild"));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment_trailing_val(" trailing comment for seqchild"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapBlock0,
            "# leading comment for foo\n"
            "foo: 0 # trailing comment for foo\n"
            "# leading comment for bar\n"
            "bar: 1 # trailing comment for bar\n"
            "# leading comment for map\n"
            "map: # trailing comment for map\n"
            "  # leading comment for mapchild\n"
            "  mapchild: yes # trailing comment for mapchild\n"
            "# leading comment for seq\n"
            "seq: # trailing comment for seq\n"
            "  # leading comment for seqchild\n"
            "  - seqchild # trailing comment for seqchild\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=CTV # trailing comment for foo\n"
            "=CLK # leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=CTV # trailing comment for bar\n"
            "=CLK # leading comment for map\n"
            "=VAL :map\n"
            "=CTK # trailing comment for map\n"
            "+MAP\n"
            "=CLK # leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=CTV # trailing comment for mapchild\n"
            "-MAP\n"
            "=CLK # leading comment for seq\n"
            "=VAL :seq\n"
            "=CTK # trailing comment for seq\n"
            "+SEQ\n"
            "=CLV # leading comment for seqchild\n"
            "=VAL :seqchild\n"
            "=CTV # trailing comment for seqchild\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val(" trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val(" trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_key(" trailing comment for map"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val(" trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_key(" trailing comment for seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment_leading_val(" leading comment for seqchild"));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment_trailing_val(" trailing comment for seqchild"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef NOT_IMPLEMENTED_YET_IN_INTS_OR_TESTSUITE
// mandate handler error from this sequence of events
ENGINE_TEST_ERR_FROM_EVENTS(DupLVcommentSeq,
                            "# CLV 1"             "\n"
                            "[]"                  "\n"
                            "# CLV 2"             "\n"
                            ,
                            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" CLV 1"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.add_comment_leading_val(" CLV 2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERR_FROM_EVENTS(DupLVcommentMap,
                            "# CLV 1"             "\n"
                            "{}"                  "\n"
                            "# CLV 2"             "\n"
                            ,
                            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" CLV 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.add_comment_leading_val(" CLV 2"));
    ___(ps.end_stream());
}
#endif

COMMENT_TEST(CommentMapBlockEmptyContainers,
             "# CLK 0"                             "\n"
             "map: # CTK 0.1"                      "\n"
             "# CLV 0.2"                           "\n"
             "{ # CTV 1"                           "\n"
             "# CLK 1.1"                           "\n"
             "} # CFV 2"                           "\n"
    /*FIXME*/""                                    "\n"
    /*FIXME*/""                                    "\n"
             "# CLK 3"                             "\n"
             "seq: # CTK 3.1"                      "\n"
             "# CLV 3.2"                           "\n"
             "[ # CTV 4"                           "\n"
             "# CLK 4.1"                           "\n"
             "] # CFV 5"                           "\n"
    /*FIXME*/""                                    "\n"
             "# CLK 6"                             "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+MAP"                                "\n"
             "=CLK # CLK 0"                        "\n"
             "=VAL :map"                           "\n"
             "=CTK # CTK 0.1"                      "\n"
             "=CLV # CLV 0.2"                      "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 1"                        "\n"
             "=CLK # CLK 1.1"                      "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 2"                        "\n"
             "=CLK # CLK 3"                        "\n"
             "=VAL :seq"                           "\n"
             "=CTK # CTK 3.1"                      "\n"
             "=CLV # CLV 3.2"                      "\n"
             "+SEQ []"                             "\n"
             "=CTV # CTV 4"                        "\n"
             "=CLK # CLK 4.1"                      "\n"
             "-SEQ"                                "\n"
             "=CFV # CFV 5"                        "\n"
             "=CLK # CLK 6"                        "\n"
             "-MAP"                                "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_key(" CLK 0"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_key(" CTK 0.1"));
    ___(ps.add_comment_leading_val(" CLV 0.2"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 1"));
    ___(ps.add_comment_leading_key(" CLK 1.1"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" CLK 3"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_key(" CTK 3.1"));
    ___(ps.add_comment_leading_val(" CLV 3.2"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 4"));
    ___(ps.add_comment_leading_key(" CLK 4.1"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" CFV 5"));
    ___(ps.add_comment_leading_key(" CLK 6"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentMapFlowEmptyContainers,
             "# CLV 0"                             "\n"
             "{ # CTV 0.1"                         "\n"
             "# CLK 0.1.1"                         "\n"
             "map: # CTK 0.2"                      "\n"
             "# CLV 0.3"                           "\n"
             "{ # CTV 1"                           "\n"
             "# CLK 1.1"                           "\n"
             "} # CFV 2"                           "\n"
             ","                                   "\n"
             "# CLK 3"                             "\n"
             "seq: # CTK 3.1"                      "\n"
             "# CLV 3.2"                           "\n"
             "[ # CTV 4"                           "\n"
             "# CLK 4.1"                           "\n"
             "] # CFV 5"                           "\n"
             "# CLK 6"                             "\n"
             "} # CFV 7"                           "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "=CLV # CLV 0"                        "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 0.1"                      "\n"
             "=CLK # CLK 0.1.1"                    "\n"
             "=VAL :map"                           "\n"
             "=CTK # CTK 0.2"                      "\n"
             "=CLV # CLV 0.3"                      "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 1"                        "\n"
             "=CLK # CLK 1.1"                      "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 2"                        "\n"
             "=CLK # CLK 3"                        "\n"
             "=VAL :seq"                           "\n"
             "=CTK # CTK 3.1"                      "\n"
             "=CLV # CLV 3.2"                      "\n"
             "+SEQ []"                             "\n"
             "=CTV # CTV 4"                        "\n"
             "=CLK # CLK 4.1"                      "\n"
             "-SEQ"                                "\n"
             "=CFV # CFV 5"                        "\n"
             "=CLK # CLK 6"                        "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 7"                        "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" CLV 0"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 0.1"));
    ___(ps.add_comment_leading_key(" CLK 0.1.1"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_key(" CTK 0.2"));
    ___(ps.add_comment_leading_val(" CLV 0.3"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 1"));
    ___(ps.add_comment_leading_key(" CLK 1.1"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" CLK 3"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_key(" CTK 3.1"));
    ___(ps.add_comment_leading_val(" CLV 3.2"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 4"));
    ___(ps.add_comment_leading_key(" CLK 4.1"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" CFV 5"));
    ___(ps.add_comment_leading_key(" CLK 6"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 7"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSeqBlockEmptyContainers,
             "# CLV 0"                             "\n"
             "- # CLV 0.1"                         "\n"
             "  { # CTV 1"                         "\n"
             "  # CLK 1.1"                         "\n"
             "  } # CFV 2"                         "\n"
             "# CLV 3"                             "\n"
             "- # CLV 3.1"                         "\n"
             "  [ # CTV 4"                         "\n"
             "  # CLV 4.1"                         "\n"
             "  ] # CFV 5"                         "\n"
             "# CLV 6"                             "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "+SEQ"                                "\n"
             "=CLV # CLV 0"                        "\n"
             "=CLV2 # CLV 0.1"                     "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 1"                        "\n"
             "=CLK # CLK 1.1"                      "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 2"                        "\n"
             "=CLV # CLV 3"                        "\n"
             "=CLV2 # CLV 3.1"                     "\n"
             "+SEQ []"                             "\n"
             "=CTV # CTV 4"                        "\n"
             "=CLV # CLV 4.1"                      "\n"
             "-SEQ"                                "\n"
             "=CFV # CFV 5"                        "\n"
             "=CLV # CLV 6"                        "\n"
             "-SEQ"                                "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment_leading_val(" CLV 0"));
    ___(ps.add_comment_leading_val2(" CLV 0.1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 1"));
    ___(ps.add_comment_leading_key(" CLK 1.1"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val(" CLV 3"));
    ___(ps.add_comment_leading_val2(" CLV 3.1"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 4"));
    ___(ps.add_comment_leading_val(" CLV 4.1"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" CFV 5"));
    ___(ps.add_comment_leading_val(" CLV 6"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(CommentSeqFlowEmptyContainers,
             "# CLV 0"                             "\n"
             "{ # CTV 0.1"                         "\n"
             "# CLK 0.1.1"                         "\n"
             "map: # CTK 0.2"                      "\n"
             "# CLV 0.3"                           "\n"
             "{ # CTV 1"                           "\n"
             "# CLK 1.1"                           "\n"
             "} # CFV 2"                           "\n"
             ","                                   "\n"
             "# CLK 3"                             "\n"
             "seq: # CTK 3.1"                      "\n"
             "# CLV 3.2"                           "\n"
             "[ # CTV 4"                           "\n"
             "# CLK 4.1"                           "\n"
             "] # CFV 5"                           "\n"
    /*FIXME*/""                                    "\n"
             "# CLK 6"                             "\n"
             "} # CFV 7"                           "\n"
             ,
             "+STR"                                "\n"
             "+DOC"                                "\n"
             "=CLV # CLV 0"                        "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 0.1"                      "\n"
             "=CLK # CLK 0.1.1"                    "\n"
             "=VAL :map"                           "\n"
             "=CTK # CTK 0.2"                      "\n"
             "=CLV # CLV 0.3"                      "\n"
             "+MAP {}"                             "\n"
             "=CTV # CTV 1"                        "\n"
             "=CLK # CLK 1.1"                      "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 2"                        "\n"
             "=CLK # CLK 3"                        "\n"
             "=VAL :seq"                           "\n"
             "=CTK # CTK 3.1"                      "\n"
             "=CLV # CLV 3.2"                      "\n"
             "+SEQ []"                             "\n"
             "=CTV # CTV 4"                        "\n"
             "=CLK # CLK 4.1"                      "\n"
             "-SEQ"                                "\n"
             "=CFV # CFV 5"                        "\n"
             "=CLK # CLK 6"                        "\n"
             "-MAP"                                "\n"
             "=CFV # CFV 7"                        "\n"
             "-DOC"                                "\n"
             "-STR"                                "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" CLV 0"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 0.1"));
    ___(ps.add_comment_leading_key(" CLK 0.1.1"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_key(" CTK 0.2"));
    ___(ps.add_comment_leading_val(" CLV 0.3"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 1"));
    ___(ps.add_comment_leading_key(" CLK 1.1"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_key(" CLK 3"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_key(" CTK 3.1"));
    ___(ps.add_comment_leading_val(" CLV 3.2"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_trailing_val(" CTV 4"));
    ___(ps.add_comment_leading_key(" CLK 4.1"));
    ___(ps.end_seq());
    ___(ps.add_comment_footer_val(" CFV 5"));
    ___(ps.add_comment_leading_key(" CLK 6"));
    ___(ps.end_map());
    ___(ps.add_comment_footer_val(" CFV 7"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

COMMENT_TEST(CommentExplKey1,
             "# c1 lk"                            "\n"
             "? # c2 tt"                            "\n"
             " # c3 ..."                            "\n"
             " a # c4 lk2"                            "\n"
             " # c5 ..."                            "\n"
             "# c6 fk"                            "\n"
             ": # c7 tk"                            "\n"
             " # c8"                            "\n"
             " b # c9"                            "\n"
             " # c10"                            "\n"
             "# c11"                            "\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK # c1 lk\n"
            "=CTT # c2 tt\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
#endif

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
