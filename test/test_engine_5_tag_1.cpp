#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static constexpr const bool multiline = true;
static constexpr const bool singleline = false;

static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(TagUnterminated0, Location(2,3),
                   "- !<foo> xxx\n"
                   "- !<foo")
ENGINE_TEST_ERRLOC(TagUnterminated0Unk, Location(2,1),
                   "\n"
                   "!<foo xxx\n"
                   "")

ENGINE_TEST_ERRLOC(TagInvalid0, Location(1,3),
                   "- !a[b xxx\n"
                   "")
ENGINE_TEST_ERRLOC(TagInvalid0Unk, Location(2,1),
                   "\n"
                   "!a[b xxx\n"
                   "")

ENGINE_TEST_ERRLOC(TagInvalid1, Location(1,3),
                   "- !a[b xxx\n"
                   "- !<foo")
ENGINE_TEST_ERRLOC(TagInvalid1Unk, Location(2,1),
                   "\n"
                   "!a[b xxx\n"
                   "")

ENGINE_TEST_ERRLOC(TagInvalid2, Location(1,3),
                   "- !a{b xxx\n"
                   "- !<foo")
ENGINE_TEST_ERRLOC(TagInvalid2Unk, Location(2,1),
                   "\n"
                   "!a{b xxx\n"
                   "")


//-----------------------------------------------------------------------------

ENGINE_TEST(TagPlacementSeqFlow0,
            ""
            "[\n"
            "!tag ,\n"
            "!tag 0,\n"
            "!tag [],\n"
            "!tag {},\n"
            "!tag\t,\n"
            "!tag\n,\n"
            "!tag\r\n,\n"
            "!tag,\n"
            "!tag]\n"
            ""
            ,
            "[\n"
            "  !tag ,\n"
            "  !tag 0,\n"
            "  !tag [],\n"
            "  !tag {},\n"
            "  !tag ,\n"
            "  !tag ,\n"
            "  !tag ,\n"
            "  !tag ,\n"
            "  !tag \n"
            "]\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :0\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(TagPlacementSeqFlow0_resolved, resolve_all_tags,
            ""
            "[\n"
            "!tag ,\n"
            "!tag 0,\n"
            "!tag [],\n"
            "!tag {},\n"
            "!tag\t,\n"
            "!tag\n,\n"
            "!tag\r\n,\n"
            "!tag,\n"
            "!tag]\n"
            ""
            ,
            "[\n"
            "  !<!tag> ,\n"
            "  !<!tag> 0,\n"
            "  !<!tag> [],\n"
            "  !<!tag> {},\n"
            "  !<!tag> ,\n"
            "  !<!tag> ,\n"
            "  !<!tag> ,\n"
            "  !<!tag> ,\n"
            "  !<!tag> \n"
            "]\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :0\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<!tag>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementSeqFlow1,
            ""
            "[\n"
            "!!str ,\n"
            "!!str 0,\n"
            "!!str [],\n"
            "!!str {},\n"
            "!!str\t,\n"
            "!!str\n,\n"
            "!!str\r\n,\n"
            "!!str,\n"
            "!!str]\n"
            ""
            ,
            "[\n"
            "  !!str ,\n"
            "  !!str 0,\n"
            "  !!str [],\n"
            "  !!str {},\n"
            "  !!str ,\n"
            "  !!str ,\n"
            "  !!str ,\n"
            "  !!str ,\n"
            "  !!str \n"
            "]\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :0\n"
            "+SEQ [] <!!str>\n"
            "-SEQ\n"
            "+MAP {} <!!str>\n"
            "-MAP\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "=VAL <!!str> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_(TagPlacementSeqFlow1_resolve, resolve_all_tags,
            ""
            "[\n"
            "!!str ,\n"
            "!!str 0,\n"
            "!!str [],\n"
            "!!str {},\n"
            "!!str\t,\n"
            "!!str\n,\n"
            "!!str\r\n,\n"
            "!!str,\n"
            "!!str]\n"
            ""
            ,
            "[\n"
            "  !<tag:yaml.org,2002:str> ,\n"
            "  !<tag:yaml.org,2002:str> 0,\n"
            "  !<tag:yaml.org,2002:str> [],\n"
            "  !<tag:yaml.org,2002:str> {},\n"
            "  !<tag:yaml.org,2002:str> ,\n"
            "  !<tag:yaml.org,2002:str> ,\n"
            "  !<tag:yaml.org,2002:str> ,\n"
            "  !<tag:yaml.org,2002:str> ,\n"
            "  !<tag:yaml.org,2002:str> \n"
            "]\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :0\n"
            "+SEQ [] <tag:yaml.org,2002:str>\n"
            "-SEQ\n"
            "+MAP {} <tag:yaml.org,2002:str>\n"
            "-MAP\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<tag:yaml.org,2002:str>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementSeqFlow2,
            ""
            "[\n"
            "!<foo> ,\n"
            "!<foo> 0,\n"
            "!<foo> [],\n"
            "!<foo> {},\n"
            "!<foo>\t,\n"
            "!<foo>\n,\n"
            "!<foo>\r\n,\n"
            "!<foo>,\n"
            "!<foo>]\n"
            ""
            ,
            "[\n"
            "  !<foo> ,\n"
            "  !<foo> 0,\n"
            "  !<foo> [],\n"
            "  !<foo> {},\n"
            "  !<foo> ,\n"
            "  !<foo> ,\n"
            "  !<foo> ,\n"
            "  !<foo> ,\n"
            "  !<foo> \n"
            "]\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <foo> :\n"
            "=VAL <foo> :0\n"
            "+SEQ [] <foo>\n"
            "-SEQ\n"
            "+MAP {} <foo>\n"
            "-MAP\n"
            "=VAL <foo> :\n"
            "=VAL <foo> :\n"
            "=VAL <foo> :\n"
            "=VAL <foo> :\n"
            "=VAL <foo> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("<foo>"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagPlacementSeqBlock,
            ""
            "- !tag\n"
            "- !tag 0\n"
            "- !tag []\n"
            "- !tag {}\n"
            "- !tag\t\n"
            "- !tag\n"
            "- !tag\r\n"
            ""
            ,
            ""
            "- !tag \n"
            "- !tag 0\n"
            "- !tag []\n"
            "- !tag {}\n"
            "- !tag \n"
            "- !tag \n"
            "- !tag \n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :0\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "=VAL <!tag> :\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(TagPlacementMapFlow0_err0, Location(2, 10), ""
            "{\n"
            "!tag !tag,\n"
            "}\n"
            "")
ENGINE_TEST_ERRLOC(TagPlacementMapFlow0_err0_anchors, Location(2, 12), ""
            "{\n"
            "&anch &anch,\n"
            "}\n"
            "")

ENGINE_TEST(TagPlacementMapFlow0, HAS_CONTAINER_KEYS,
            ""
            "{\n"
            "!tag,\n"
            "!tag0 0,\n"
            "!tags [],\n"
            "!tagm {},\n"
            "!tag1\t,\n"
            "!tag2\n,\n"
            "!tag3\r\n,\n"
            "!tag4 's',\n"
            "!tag5 \"d\",\n"
            "!tag6}\n"
            ""
            ,
            "{\n"
            "  !tag : ,\n"
            "  !tag0 0: ,\n"
            "  ? !tags []: ,\n"
            "  ? !tagm {}: ,\n"
            "  !tag1 : ,\n"
            "  !tag2 : ,\n"
            "  !tag3 : ,\n"
            "  !tag4 's': ,\n"
            "  !tag5 \"d\": ,\n"
            "  !tag6 : \n"
            "}\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> :\n"
            "=VAL :\n"
            "=VAL <!tag0> :0\n"
            "=VAL :\n"
            "+SEQ [] <!tags>\n"
            "-SEQ\n"
            "=VAL :\n"
            "+MAP {} <!tagm>\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL <!tag1> :\n"
            "=VAL :\n"
            "=VAL <!tag2> :\n"
            "=VAL :\n"
            "=VAL <!tag3> :\n"
            "=VAL :\n"
            "=VAL <!tag4> 's\n"
            "=VAL :\n"
            "=VAL <!tag5> \"d\n"
            "=VAL :\n"
            "=VAL <!tag6> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag0"));
    ___(ps.set_key_scalar_plain("0"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tags"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tagm"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag1"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag2"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag3"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag4"));
    ___(ps.set_key_scalar_squoted("s"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag5"));
    ___(ps.set_key_scalar_dquoted("d"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag6"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapFlow1,
            ""
            "{\n"
            " 0: !tag,\n"
            " 1: !tag0 0,\n"
            " 2: !tags [],\n"
            " 3: !tagm {},\n"
            " 4: !tag1\t,\n"
            " 5: !tag2\n,\n"
            " 6: !tag3\r\n,\n"
            " 7: !tag4 's',\n"
            " 8: !tag5 \"d\",\n"
            " 9: !tag6}\n"
            ""
            ,
            "{\n"
            "  0: !tag ,\n"
            "  1: !tag0 0,\n"
            "  2: !tags [],\n"
            "  3: !tagm {},\n"
            "  4: !tag1 ,\n"
            "  5: !tag2 ,\n"
            "  6: !tag3 ,\n"
            "  7: !tag4 's',\n"
            "  8: !tag5 \"d\",\n"
            "  9: !tag6 \n"
            "}\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :0\n"
            "=VAL <!tag> :\n"
            "=VAL :1\n"
            "=VAL <!tag0> :0\n"
            "=VAL :2\n"
            "+SEQ [] <!tags>\n"
            "-SEQ\n"
            "=VAL :3\n"
            "+MAP {} <!tagm>\n"
            "-MAP\n"
            "=VAL :4\n"
            "=VAL <!tag1> :\n"
            "=VAL :5\n"
            "=VAL <!tag2> :\n"
            "=VAL :6\n"
            "=VAL <!tag3> :\n"
            "=VAL :7\n"
            "=VAL <!tag4> 's\n"
            "=VAL :8\n"
            "=VAL <!tag5> \"d\n"
            "=VAL :9\n"
            "=VAL <!tag6> :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("0"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("1"));
    ___(ps.set_val_tag("!tag0"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("2"));
    ___(ps.set_val_tag("!tags"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("3"));
    ___(ps.set_val_tag("!tagm"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("4"));
    ___(ps.set_val_tag("!tag1"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("5"));
    ___(ps.set_val_tag("!tag2"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("6"));
    ___(ps.set_val_tag("!tag3"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("7"));
    ___(ps.set_val_tag("!tag4"));
    ___(ps.set_val_scalar_squoted("s"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("8"));
    ___(ps.set_val_tag("!tag5"));
    ___(ps.set_val_scalar_dquoted("d"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("9"));
    ___(ps.set_val_tag("!tag6"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValFlow,
            ""
            "{\n"
            "x: !tag,\n"
            "a: !tag 0,\n"
            "b: !tag [],\n"
            "c: !tag {},\n"
            "}\n"
            ""
            ,
            "{\n"
            "  x: !tag ,\n"
            "  a: !tag 0,\n"
            "  b: !tag [],\n"
            "  c: !tag {}\n"
            "}\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :x\n"
            "=VAL <!tag> :\n"
            "=VAL :a\n"
            "=VAL <!tag> :0\n"
            "=VAL :b\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "=VAL :c\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("x"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock,
            ""
            "x: !tag\n"
            "a: !tag 0\n"
            "b: !tag []\n"
            "c: !tag {}\n"
            "d: !tag\n"
            "  e: !tag\n"
            "f: !tag\n"
            "  - !tag g\n"
            ""
            ,
            ""
            "x: !tag \n"
            "a: !tag 0\n"
            "b: !tag []\n"
            "c: !tag {}\n"
            "d: !tag\n"
            "  e: !tag \n"
            "f: !tag\n"
            "  - !tag g\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :x\n"
            "=VAL <!tag> :\n"
            "=VAL :a\n"
            "=VAL <!tag> :0\n"
            "=VAL :b\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "=VAL :c\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL :d\n"
            "+MAP <!tag>\n"
            "=VAL :e\n"
            "=VAL <!tag> :\n"
            "-MAP\n"
            "=VAL :f\n"
            "+SEQ <!tag>\n"
            "=VAL <!tag> :g\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("x"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("d"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("f"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("g"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapKeyFlow,
            HAS_CONTAINER_KEYS, Location(34,4,7),
            ""
            "!mtag {\n"
            "!tag : x\n,"
            "!tag a: x\n,"
            "!tag []: x\n,"
            "!tag {}: x\n,"
            "!tag :\n,"
            "}\n"
            ""
            ,
            ""
            "!mtag {\n"
            "  !tag : x,\n"
            "  !tag a: x,\n"
            "  ? !tag []: x,\n"
            "  ? !tag {}: x,\n"
            "  !tag : \n"
            "}\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {} <!mtag>\n"
            "=VAL <!tag> :\n"
            "=VAL :x\n"
            "=VAL <!tag> :a\n"
            "=VAL :x\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "=VAL :x\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL :x\n"
            "=VAL <!tag> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mtag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapKeyBlock,
            HAS_CONTAINER_KEYS, Location(30,4,6),
            ""
            "!mtag\n"
            "!tag : x\n"
            "!tag a: x\n"
            "!tag []: x\n"
            "!tag {}: x\n"
            "!tag :\n"
            ""
            ,
            ""
            "!mtag\n"
            "!tag : x\n"
            "!tag a: x\n"
            "? !tag []\n"
            ": x\n"
            "? !tag {}\n"
            ": x\n"
            "!tag : \n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP <!mtag>\n"
            "=VAL <!tag> :\n"
            "=VAL :x\n"
            "=VAL <!tag> :a\n"
            "=VAL :x\n"
            "+SEQ [] <!tag>\n"
            "-SEQ\n"
            "=VAL :x\n"
            "+MAP {} <!tag>\n"
            "-MAP\n"
            "=VAL :x\n"
            "=VAL <!tag> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mtag"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2_0,
            ""
            "a:\n"
            "  : \n"
            ""
            ,
            ""
            "a:\n"
            "  : \n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2_1,
            ""
            "a:\n"
            "  : \n"
            "b: c\n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :b\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2,
            ""
            "!mtag\n"
            "!tag0 : !tag1\n"
            "  !tag2 : !tag3\n"
            "a:\n"
            "  : \n"
            ""
            ,
            ""
            "!mtag\n"
            "!tag0 : !tag1\n"
            "  !tag2 : !tag3 \n"
            "a:\n"
            "  : \n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP <!mtag>\n"
            "=VAL <!tag0> :\n"
            "+MAP <!tag1>\n"
            "=VAL <!tag2> :\n"
            "=VAL <!tag3> :\n"
            "-MAP\n"
            "=VAL :a\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mtag"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!tag0"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!tag2"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag3"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapComplex,
            HAS_CONTAINER_KEYS, Location(18,3,5),
            ""
            "!mtag\n"
            "? !tag0\n"
            " !a : !b\n"
            ": !tag1\n"
            " !a : !b\n"
             ""
            "? !tag2\n"
            " !a 1: !b 2\n"
            ": !tag3\n"
            " !a : !b\n"
            ""
            "? !tag4\n"
            " - !a 1\n"
            " - !b 2\n"
            ": !tag5\n"
            " !a : !b\n"
            "? !tag6\n"
            ""
            ,
            ""
            "!mtag\n"
            "? !tag0\n"
            "  !a : !b \n"
            ": !tag1\n"
            "  !a : !b \n"
            ""
            "? !tag2\n"
            "  !a 1: !b 2\n"
            ": !tag3\n"
            "  !a : !b \n"
            ""
            "? !tag4\n"
            "  - !a 1\n"
            "  - !b 2\n"
            ": !tag5\n"
            "  !a : !b \n"
            ""
            "!tag6 : \n"
            ""
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP <!mtag>\n"
            "+MAP <!tag0>\n"
            "=VAL <!a> :\n"
            "=VAL <!b> :\n"
            "-MAP\n"
            "+MAP <!tag1>\n"
            "=VAL <!a> :\n"
            "=VAL <!b> :\n"
            "-MAP\n"
            "+MAP <!tag2>\n"
            "=VAL <!a> :1\n"
            "=VAL <!b> :2\n"
            "-MAP\n"
            "+MAP <!tag3>\n"
            "=VAL <!a> :\n"
            "=VAL <!b> :\n"
            "-MAP\n"
            "+SEQ <!tag4>\n"
            "=VAL <!a> :1\n"
            "=VAL <!b> :2\n"
            "-SEQ\n"
            "+MAP <!tag5>\n"
            "=VAL <!a> :\n"
            "=VAL <!b> :\n"
            "-MAP\n"
            "=VAL <!tag6> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mtag"));
    ___(ps.begin_map_val_block());

    ___(ps.set_key_tag("!tag0"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_tag("!tag1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag2"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain("1"));
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map_block());
    ___(ps.set_val_tag("!tag3"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag4"));
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_tag("!a"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_seq_block());
    ___(ps.set_val_tag("!tag5"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag6"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());

    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
