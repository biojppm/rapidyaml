#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------


ENGINE_TEST(TagPlacementSeqFlow,
            (""
             "[\n"
             "!tag ,\n"
             "!tag 0,\n"
             "!tag [],\n"
             "!tag {},\n"
             "]\n"
             ""
             ,
             "[!tag ,!tag 0,!tag [],!tag {}]"
                ),
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
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementSeqBlock,
            (""
             "- !tag\n"
             "- !tag 0\n"
             "- !tag []\n"
             "- !tag {}\n"
             ""
             ,
             ""
             "- !tag \n"
             "- !tag 0\n"
             "- !tag []\n"
             "- !tag {}\n"
             ""
                ),
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
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValFlow,
            (""
             "{\n"
             "x: !tag,\n"
             "a: !tag 0,\n"
             "b: !tag [],\n"
             "c: !tag {},\n"
             "}\n"
             ""
             ,
             "{x: !tag ,a: !tag 0,b: !tag [],c: !tag {}}"
                ),
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
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock,
            (""
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
                ),
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
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("d"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("f"));
    ___(ps.set_val_tag("!tag"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!tag"));
    ___(ps.set_val_scalar_plain("g"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapKeyFlow,
            (HAS_CONTAINER_KEYS, Location(34,4,7),
             ""
             "!mtag {\n"
             "!tag : x\n,"
             "!tag a: x\n,"
             "!tag []: x\n,"
             "!tag {}: x\n,"
             "!tag :\n,"
             "}\n"
             ""
                ),
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
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapKeyBlock,
            (HAS_CONTAINER_KEYS, Location(30,4,6),
             ""
             "!mtag\n"
             "!tag : x\n"
             "!tag a: x\n"
             "!tag []: x\n"
             "!tag {}: x\n"
             "!tag :\n"
             ""
                ),
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
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2_0,
            (""
             "a:\n"
             "  : \n"
             ""
             ,
             ""
             "a:\n"
             "  : \n"
                ),
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
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2_1,
            (""
             "a:\n"
             "  : \n"
             "b: c\n"
             ""
                ),
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
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("b"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapValBlock2,
            (""
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
                ),
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
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementMapComplex,
            (HAS_CONTAINER_KEYS, Location(18,3,5),
             ""
             "!mtag\n"
             "? !tag0\n"
             " !a : !b\n"
             ": !tag1\n"
             " !a : !b\n"

             "? !tag2\n"
             " !a 1: !b 2\n"
             ": !tag3\n"
             " !a : !b\n"

             "? !tag4\n"
             " - !a 1\n"
             " - !b 2\n"
             ": !tag5\n"
             " !a : !b\n"
             "? !tag6\n"
             ""
                ),
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
    ___(ps.end_map());
    ___(ps.set_val_tag("!tag1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag2"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain("1"));
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map());
    ___(ps.set_val_tag("!tag3"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag4"));
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_tag("!a"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_seq());
    ___(ps.set_val_tag("!tag5"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!a"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!b"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map());

    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag6"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());

    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagBlockSeq,
            ("- !light fluorescent\n- notag\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL <!light> :fluorescent\n"
            "=VAL :notag\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("notag"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagFlowSeq,
            ("[!light fluorescent,notag]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!light> :fluorescent\n"
            "=VAL :notag\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("notag"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveAndTag,
            ("%YAML 1.2\n"
             "---\n"
             "!light fluorescent\n"
             "...\n"
             "%TAG !m! !my-\n"
             "---\n"
             "!m!light green\n",
             "--- !light fluorescent\n"
             "...\n"
             "%TAG !m! !my-\n"
             "--- !m!light green\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!light> :fluorescent\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <!my-light> :green\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!light"));
    ___(ps.set_val_scalar_plain("fluorescent"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG !m! !my-"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!m!light"));
    ___(ps.set_val_scalar_plain("green"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERR(TagCustomNotFound,
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG !m! tag:example.com,2000:app/\n"
                "--- !n!foo \"bar\"\n")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(TagTestSuiteU99R_0, Location(2,1),
                   "- !!str, xxx\n")

ENGINE_TEST_ERRLOC(TagTestSuiteU99R_1, Location(2,1),
                   "- !str, xxx\n")

ENGINE_TEST(TagTestSuiteU99R_2,
            ("[!!str, xxx]", "[!!str ,xxx]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteU99R_2_1,
            ("[!str, xxx]","[!str ,xxx]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL <!str> :\n"
            "=VAL :xxx\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("xxx"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteU99R_3,
            ("{!!str, xxx}", "{!!str : ,xxx: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteU99R_3_1,
            ("{!str, xxx}", "{!str : ,xxx: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!str> :\n"
            "=VAL :\n"
            "=VAL :xxx\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("xxx"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_0_0_0,
            ("{foo: !!str , !!str : bar}", "{foo: !!str ,!!str : bar}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_0_0_1,
            ("foo: !!str\n!!str : bar\n", "foo: !!str \n!!str : bar\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_0_1_0,
            ("{foo: !str , !str : bar}", "{foo: !str ,!str : bar}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_0_1_1,
            ("foo: !str\n!str : bar\n", "foo: !str \n!str : bar\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str> :\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_1_0_0,
            ("{foo: !!str, !!str: bar}", "{foo: !!str ,!!str: bar: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_1_0_1,
            ("foo: !!str\n!!str: bar:\n", "foo: !!str \n!!str: bar: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <tag:yaml.org,2002:str> :\n"
            "=VAL <tag:yaml.org,2002:str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_1_1_0,
            ("{foo: !str, !str: bar}", "{foo: !str ,!str: bar: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteWZ62_1_1_1,
            ("foo: !str\n!str: bar:\n", "foo: !str \n!str: bar: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL <!str> :\n"
            "=VAL <!str:> :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_tag("!str"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!str:"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(TagTestSuiteLHL4_0, Location(3,1),
                   "---\n"
                   "!invalid{}tag scalar\n")

ENGINE_TEST_ERRLOC(TagTestSuiteLHL4_1, Location(3,1),
                   "---\n"
                   "!!invalid{}tag scalar\n")

ENGINE_TEST(TagTestSuiteUGM3,
            ("--- !<tag:clarkevans.com,2002:invoice>\n"
            "invoice: 34843\n"
            "date: 2001-01-23\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP <tag:clarkevans.com,2002:invoice>\n"
            "=VAL :invoice\n"
            "=VAL :34843\n"
            "=VAL :date\n"
            "=VAL :2001-01-23\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!<tag:clarkevans.com,2002:invoice>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("invoice"));
    ___(ps.set_val_scalar_plain("34843"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("date"));
    ___(ps.set_val_scalar_plain("2001-01-23"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuiteUKK6_02_0,
            ("!", "! \n"),
            "+STR\n"
            "+DOC\n"
            "=VAL <!> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_0,
            ("--- !foo \"bar\"\n"
             "...\n"
             "%TAG ! tag:example.com,2000:app/\n"
             "--- !foo \"bar\"\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <tag:example.com,2000:app/foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG ! tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagTestSuite6WLZ_1,
            ("--- !foo \"bar\"\n"
             "...\n"
             "%TAG ! tag:example.com,2000:app/\n"
             "--- !<tag:example.com,2000:app/foo> \"bar\"\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL <!foo> \"bar\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL <tag:example.com,2000:app/foo> \"bar\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!foo"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%TAG ! tag:example.com,2000:app/"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<tag:example.com,2000:app/foo>"));
    ___(ps.set_val_scalar_dquoted("bar"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERR(DirectiveTestSuite9MMA_YAML,
                "%YAML 1.2")

ENGINE_TEST_ERR(DirectiveTestSuite9MMA_TAG,
                "%TAG ! tag:example.com,2000:app/\n")

ENGINE_TEST_ERR(DirectiveTestSuiteMUS6_00,
                "%YAML 1.1#...\n"
                "---\n")

ENGINE_TEST_ERR(DirectiveTestSuite9HCY,
                "!foo \"bar\"\n"
                "%TAG ! tag:example.com,2000:app/\n"
                "---\n"
                "!foo \"bar\"\n")

ENGINE_TEST(DirectiveTestSuiteMUS6,
            ("%YAM 1.1\n"
             "---\n",
             "---\n"),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAM 1.1"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DirectiveMultipleYAML_W4TN,
            (""
             "%YAML 1.2\n"
             "---\n"
             "foo\n"
             "...\n"
             "%YAML 1.2\n"
             "---\n"
             "# Empty\n"
             "...\n"
             "",
             "--- foo\n"
             "---\n"
             ""),
            "+STR\n"
            "+DOC ---\n"
            "=VAL :foo\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "=VAL :\n"
            "-DOC ...\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_doc_expl());
    ___(ps.add_directive("%YAML 1.2"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST_ERR(DirectiveMultipleYAML_0,
                "%YAML 1.2\n"
                "...\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_1,
                "%YAML 1.1\n"
                "...\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_2,
                "%YAML 1.2\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveMultipleYAML_3,
                "%YAML 1.1\n"
                "%YAML 1.2\n"
                "---\n"
                "bar")

ENGINE_TEST_ERR(DirectiveWrong,
                "--- !foo \"ba\"\n"
                "...\n"
                "%TAG ,,,,,\n"
                "--- !foo \"bar\"\n")

ENGINE_TEST_ERR(DirectiveTooMany,
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG ! tag:example.com,2000:app/\n"
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG ! tag:example.com,2001:app/\n"
                "...\n"
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG ! tag:example.com,2002:app/\n"
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG ! tag:example.com,2003:app/\n"
                "--- !foo \"bar\"\n"
                "...\n"
                "%TAG ! tag:example.com,2004:app/\n"
                "--- !foo \"bar\"\n")


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptySeq0,
            ("!!seq []\n", "!!seq []"),
            "+STR\n+DOC\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptySeq1,
            (""
             "- !!seq []\n"
             ""),
            "+STR\n+DOC\n+SEQ\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptySeq2,
            (""
             "!!seq\n"
             "- !!seq []\n"
             ""),
            "+STR\n+DOC\n+SEQ <tag:yaml.org,2002:seq>\n+SEQ [] <tag:yaml.org,2002:seq>\n-SEQ\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagEmptyMap0,
            ("!!map {}\n", "!!map {}"),
            "+STR\n+DOC\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptyMap1,
            (""
             "- !!map {}\n"
             ""),
            "+STR\n+DOC\n+SEQ\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagEmptyMap2,
            (""
             "!!seq\n"
             "- !!map {}\n"
             ""),
            "+STR\n+DOC\n+SEQ <tag:yaml.org,2002:seq>\n+MAP {} <tag:yaml.org,2002:map>\n-MAP\n-SEQ\n-DOC\n-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_tag("!!map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(TagYs0,
            (""
             "!yamlscript/v0/bare\n"
             "--- !code\n"
             "--- !data\n"
             "--- !code\n"
             "--- !data\n"
             "",
             ""
             "--- !yamlscript/v0/bare \n"
             "--- !code \n"
             "--- !data \n"
             "--- !code \n"
             "--- !data \n"
             ""),
            "+STR\n"
            "+DOC\n"
            "=VAL <!yamlscript/v0/bare> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!data> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!data> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!yamlscript/v0/bare"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!data"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!data"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagYs1,
            (""
             "!yamlscript/v0/bare\n"
             "--- !code\n"
             "42\n"
             "",
             ""
             "--- !yamlscript/v0/bare \n"
             "--- !code 42\n"
             ""),
            "+STR\n"
            "+DOC\n"
            "=VAL <!yamlscript/v0/bare> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!code> :42\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!yamlscript/v0/bare"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!code"));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
