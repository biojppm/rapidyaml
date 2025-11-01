#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++-4.8 cannot accept
// them as macro arguments

#ifndef RYML_WITH_COMMENTS
#error this test requires RYML_WITH_COMMENTS to be defined
#endif

namespace c4 {
namespace yml {

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

COMMENT_TEST(FlowMapSingle0,
             "{ # 1"                       "\n"
             "  # 2"                       "\n"
             "}"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+MAP {}"                     "\n"
             "=COMM #[VAL_BRACKET_TRAILING] 1\\n 2"          "\n"
             "-MAP"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 1\n 2", COMM_VAL_BRACKET_TRAILING));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

COMMENT_TEST(FlowMapSingle1,
             "{"                           "\n"
             "  # 1"                       "\n"
             "  # 2"                       "\n"
             "}"                           "\n"
             ,
             "+STR"                        "\n"
             "+DOC"                        "\n"
             "+MAP {}"                     "\n"
             "=COMM #[VAL_BRACKET_LEADING] 1\\n 2"          "\n"
             "-MAP"                        "\n"
             "-DOC"                        "\n"
             "-STR"                        "\n"
    )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment(" 1\n 2", COMM_VAL_BRACKET_LEADING));
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
             "=COMM #[VAL_BRACKET_LEADING] 5"           "\n"
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
    ___(ps.add_comment(" 5", COMM_VAL_BRACKET_LEADING));
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
