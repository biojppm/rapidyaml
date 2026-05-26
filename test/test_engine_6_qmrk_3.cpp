#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


//static constexpr const bool multiline = true;
static constexpr const bool singleline = false;


ENGINE_TEST(QmrkFlow1Anch,
            "{ ? &anch , }"
            ,
            "{&anch : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Tag,
            "{ ? !tag , }"
            ,
            "{!tag : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> :\n"
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
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Squo,
            "{ ? 'squo', }"
            ,
            "{'squo': }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchSquo,
            "{ ? &anch 'squo', }"
            ,
            "{&anch 'squo': }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagSquo,
            "{ ? !tag 'squo', }"
            ,
            "{!tag 'squo': }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Dquo,
            "{ ? \"dquo\", }"
            ,
            "{\"dquo\": }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchDquo,
            "{ ? &anch \"dquo\", }"
            ,
            "{&anch \"dquo\": }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagDquo,
            "{ ? !tag \"dquo\", }"
            ,
            "{!tag \"dquo\": }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Ref,
            "{ ? *ref , }"
            ,
            "{*ref : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=ALI *ref\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlowSeq1,
            HAS_CONTAINER_KEYS,
            "{ ? [a, b]: c , }"
            ,
            "{? [a, b] : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlowSeq1Double,
            HAS_CONTAINER_KEYS,
            "{ ? [a, b]: c ,  ? [a, b]: c }"
            ,
            "{? [a, b] : c,? [a, b]: c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.add_sibling());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlowSeq1Anchor,
            HAS_CONTAINER_KEYS,
            "{ ? &anchor [a, b]: c , }"
            ,
            "{? &anchor [a, b] : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlowSeq1Tag,
            HAS_CONTAINER_KEYS,
            "{ ? !tag [a, b]: c , }"
            ,
            "{? !tag [a, b] : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Map,
            HAS_CONTAINER_KEYS,
            "{ ? {a: b}: c , }"
            ,
            "{? {a: b} : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapAnchor,
            HAS_CONTAINER_KEYS,
            "{ ? &anchor {a: b}: c , }"
            ,
            "{? &anchor {a: b} : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapTag,
            HAS_CONTAINER_KEYS,
            "{ ? !tag {a: b}: c , }"
            ,
            "{? !tag {a: b} : c}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkNestedSamelineTag0_00, HAS_CONTAINER_KEYS,
            "? ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP"        "\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSamelineTag0_01, HAS_CONTAINER_KEYS,
            "? &a !t\n"
            "  ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP &a <!t>""\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSamelineTag0_02, HAS_CONTAINER_KEYS,
            "? &a !t ? &a !t"   "\n"
            ": a"         "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "+MAP &a <!t>""\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "-MAP"        "\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkNestedSamelineTag0_03,
            "? &a !t\n"
            "? &b !u"   "\n"
            ": a"         "\n"
            ,
            "&a !t : \n"
            "&b !u : a"   "\n"
            ,
            "+STR"        "\n"
            "+DOC"        "\n"
            "+MAP"        "\n"
            "=VAL &a <!t> :""\n"
            "=VAL :"      "\n"
            "=VAL &b <!u> :""\n"
            "=VAL :a"      "\n"
            "-MAP"        "\n"
            "-DOC"        "\n"
            "-STR"        "\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a"));
    ___(ps.set_key_tag("!t"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("b"));
    ___(ps.set_key_tag("!u"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}



//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock4Plain1_0,
            HAS_CONTAINER_KEYS,
            "\n"
            "? &mapanchor\n"
            "  foo : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(MapKeyBlock4Plain1_0_tag,
            HAS_CONTAINER_KEYS,
            "\n"
            "? !maptag\n"
            "  foo : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP <!maptag>\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!maptag"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(MapKeyBlock4Plain1_1,
            HAS_CONTAINER_KEYS,
            "\n"
            "? \n"
            "  &keyanchor foo : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL &keyanchor :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("keyanchor"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(MapKeyBlock4Plain1_2,
            HAS_CONTAINER_KEYS,
            "\n"
            "? &keyanchor foo : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL &keyanchor :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("keyanchor"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(MapKeyBlock4Ref1_0,
            HAS_CONTAINER_KEYS,
            "\n"
            "? &mapanchor\n"
            "  *ref : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor\n"
            "=ALI *ref\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock5AllPlain,
            HAS_CONTAINER_KEYS,
            "? &mapanchor !maptag\n"
            "  &keyanchor !keytag foo: bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor <!maptag>\n"
            "=VAL &keyanchor <!keytag> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.set_key_tag("!maptag"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("keyanchor"));
    ___(ps.set_key_tag("!keytag"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock5AllSquo,
            HAS_CONTAINER_KEYS,
            "\n"
            "? &mapanchor !maptag\n"
            "  &keyanchor !keytag 'foo': bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor <!maptag>\n"
            "=VAL &keyanchor <!keytag> 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.set_key_tag("!maptag"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("keyanchor"));
    ___(ps.set_key_tag("!keytag"));
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock5AllDquo,
            HAS_CONTAINER_KEYS,
            "\n"
            "? &mapanchor !maptag\n"
            "  &keyanchor !keytag \"foo\": bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP &mapanchor <!maptag>\n"
            "=VAL &keyanchor <!keytag> \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.set_key_tag("!maptag"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("keyanchor"));
    ___(ps.set_key_tag("!keytag"));
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock6_0_plain,
            "&ma !mt\n"
            "&ka1 !kt1 k: c\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &ma <!mt>\n"
            "=VAL &ka1 <!kt1> :k\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("ma"));
    ___(ps.set_val_tag("!mt"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ka1"));
    ___(ps.set_key_tag("!kt1"));
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_0_squoted,
            "&ma !mt\n"
            "&ka1 !kt1 'k': c\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &ma <!mt>\n"
            "=VAL &ka1 <!kt1> 'k\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("ma"));
    ___(ps.set_val_tag("!mt"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ka1"));
    ___(ps.set_key_tag("!kt1"));
    ___(ps.set_key_scalar_squoted("k"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_0_dquoted,
            "&ma !mt\n"
            "&ka1 !kt1 \"k\": c\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &ma <!mt>\n"
            "=VAL &ka1 <!kt1> \"k\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("ma"));
    ___(ps.set_val_tag("!mt"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ka1"));
    ___(ps.set_key_tag("!kt1"));
    ___(ps.set_key_scalar_dquoted("k"));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_0_flowseq,
            HAS_CONTAINER_KEYS,
            "&ma !mt\n"
            "&ka0 !kt0 []: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &ma <!mt>\n"
            "+SEQ [] &ka0 <!kt0>\n"
            "-SEQ\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("ma"));
    ___(ps.set_val_tag("!mt"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ka0"));
    ___(ps.set_key_tag("!kt0"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_0_flowmap,
            HAS_CONTAINER_KEYS,
            "&ma !mt\n"
            "&ka1 !kt1 {}: c\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &ma <!mt>\n"
            "+MAP {} &ka1 <!kt1>\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("ma"));
    ___(ps.set_val_tag("!mt"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ka1"));
    ___(ps.set_key_tag("!kt1"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock6_1_plain,
            "? &ma !mt\n"
            "&ka !kt k: b\n"
            ,
            "&ma !mt : \n"
            "&ka !kt k: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL :\n"
            "=VAL &ka <!kt> :k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_1_squoted,
            "? &ma !mt\n"
            "&ka !kt 'k': b\n"
            ,
            "&ma !mt : \n"
            "&ka !kt 'k': b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL :\n"
            "=VAL &ka <!kt> 'k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_squoted("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_1_dquoted,
            "? &ma !mt\n"
            "&ka !kt \"k\": b\n"
            ,
            "&ma !mt : \n"
            "&ka !kt \"k\": b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL :\n"
            "=VAL &ka <!kt> \"k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_dquoted("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_1_flowseq,
            HAS_CONTAINER_KEYS,
            "? &ma !mt\n"
            "&ka !kt []: b\n"
            ,
            "&ma !mt : \n"
            "&ka !kt []: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL :\n"
            "+SEQ [] &ka <!kt>\n"
            "-SEQ\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_1_flowmap,
            HAS_CONTAINER_KEYS,
            "? &ma !mt\n"
            "&ka !kt {}: b\n"
            ,
            "&ma !mt : \n"
            "&ka !kt {}: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL :\n"
            "+MAP {} &ka <!kt>\n"
            "-MAP\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock6_2_plain,
            "? &ma !mt\n"
            ": &va !vt\n"
            "&ka !kt k: b\n"
            ,
            "&ma !mt : &va !vt \n"
            "&ka !kt k: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL &va <!vt> :\n"
            "=VAL &ka <!kt> :k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("va"));
    ___(ps.set_val_tag("!vt"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_plain("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_2_squoted,
            "? &ma !mt\n"
            ": &va !vt\n"
            "&ka !kt 'k': b\n"
            ,
            "&ma !mt : &va !vt \n"
            "&ka !kt 'k': b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL &va <!vt> :\n"
            "=VAL &ka <!kt> 'k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("va"));
    ___(ps.set_val_tag("!vt"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_squoted("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_2_dquoted,
            "? &ma !mt\n"
            ": &va !vt\n"
            "&ka !kt \"k\": b\n"
            ,
            "&ma !mt : &va !vt \n"
            "&ka !kt \"k\": b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL &va <!vt> :\n"
            "=VAL &ka <!kt> \"k\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("va"));
    ___(ps.set_val_tag("!vt"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.set_key_scalar_dquoted("k"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_2_flowseq,
            HAS_CONTAINER_KEYS,
            "? &ma !mt\n"
            ": &va !vt\n"
            "&ka !kt []: b\n"
            ,
            "&ma !mt : &va !vt \n"
            "&ka !kt []: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL &va <!vt> :\n"
            "+SEQ [] &ka <!kt>\n"
            "-SEQ\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("va"));
    ___(ps.set_val_tag("!vt"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock6_2_flowmap,
            HAS_CONTAINER_KEYS,
            "? &ma !mt\n"
            ": &va !vt\n"
            "&ka !kt {}: b\n"
            ,
            "&ma !mt : &va !vt \n"
            "&ka !kt {}: b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL &ma <!mt> :\n"
            "=VAL &va <!vt> :\n"
            "+MAP {} &ka <!kt>\n"
            "-MAP\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("ma"));
    ___(ps.set_key_tag("!mt"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("va"));
    ___(ps.set_val_tag("!vt"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("ka"));
    ___(ps.set_key_tag("!kt"));
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
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

C4_SUPPRESS_WARNING_MSVC_POP
