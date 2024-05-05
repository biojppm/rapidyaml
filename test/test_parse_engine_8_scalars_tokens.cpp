#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarWithColon0,
            ("a::\n", "a:: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a:\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColon1,
            ("key ends with two colons::: value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :key ends with two colons::\n"
            "=VAL :value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("key ends with two colons::"));
    ___(ps.set_val_scalar_plain("value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColonSeq,
            ("- ::\n"
             "- x::\n"
             "- :x::\n",
             "- :: \n"
             "- x:: \n"
             "- :x:: \n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL ::\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :x:\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL ::x:\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain(":"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("x:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain(":x:"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(BlockPlainScalarCommaDoc,
            ("a, b\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL :a, b\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a, b"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaMap,
            ("a, b: c, d\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a, b\n"
            "=VAL :c, d\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a, b"));
    ___(ps.set_val_scalar_plain("c, d"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaSeq,
            ("- a, b\n- c, d\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a, b\n"
            "=VAL :c, d\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a, b"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("c, d"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------


ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_0, Location(   1,32), "{ first: Sammy, last: Sosa }foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_1, Location(   2, 4), "{ first: Sammy, last: Sosa }\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_0, Location(   1,29), "{ first: Sammy, last: Sosa }- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_0, Location(   1,29), "{ first: Sammy, last: Sosa }[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_0, Location(   1,29), "{ first: Sammy, last: Sosa }{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_0, Location(   1,29), "{ first: Sammy, last: Sosa }'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_0, Location(   1,29), "{ first: Sammy, last: Sosa }|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_0, Location(   1,29), "{ first: Sammy, last: Sosa }>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_0, Location(   1,33), "{ first: Sammy, last: Sosa }?foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_0, Location(   1,33), "{ first: Sammy, last: Sosa }:foo")
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_1, Location(   2, 1), "{ first: Sammy, last: Sosa }\n:\nfoo")//fixme
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_0, Location(  1,29), "{ first: Sammy, last: Sosa }: foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_1, Location(  2, 1), "{ first: Sammy, last: Sosa }\n: \nfoo")//fixme
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_0, Location(  1,34), "{ first: Sammy, last: Sosa } &foo")// fixme
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_1, Location(  2, 5), "{ first: Sammy, last: Sosa }\n&foo")


ENGINE_TEST_ERRLOC(tokens_after_seqflow_0_0, Location(1,32), "[a,b,c,                   d]foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_0_1, Location(2, 4), "[a,b,c,                   d]\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_2_0, Location(1,29), "[a,b,c,                   d]- foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_2_1, Location(2, 1), "[a,b,c,                   d]\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_3_0, Location(1,29), "[a,b,c,                   d][foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_3_1, Location(2, 1), "[a,b,c,                   d]\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_4_0, Location(1,29), "[a,b,c,                   d]{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_4_1, Location(2, 1), "[a,b,c,                   d]\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_5_0, Location(1,29), "[a,b,c,                   d]'foo'")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_5_1, Location(2, 1), "[a,b,c,                   d]\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_6_0, Location(1,29), "[a,b,c,                   d]|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_6_1, Location(2, 1), "[a,b,c,                   d]\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_7_0, Location(1,29), "[a,b,c,                   d]>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_7_1, Location(2, 1), "[a,b,c,                   d]\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_8_0, Location(1,33), "[a,b,c,                   d]?foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_8_1, Location(2, 1), "[a,b,c,                   d]\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_0, Location(1,33), "[a,b,c,                   d]:foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_1, Location(2, 1), "[a,b,c,                   d]\n:\nfoo")//fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_0, Location(1,29), "[a,b,c,                   d]: foo")// fixme
//ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_1, Location(2, 1), "[a,b,c,                   d]\n: \nfoo")//fixme
ENGINE_TEST_ERRLOC(tokens_after_seqflow_11_0, Location(1,34), "[a,b,c,                   d] &foo")// fixme
ENGINE_TEST_ERRLOC(tokens_after_seqflow_11_1, Location(2, 5), "[a,b,c,                   d]\n&foo")

ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1, Location(1, 9), "[a,b,c]]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2, Location(1, 9), "[a,b,c]}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3, Location(1, 8), "[a,b,c]{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4, Location(1, 8), "[a,b,c][")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5, Location(1, 8), "[a,b,c]-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1_0, Location(1,10), "[a,b,c] ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2_0, Location(1,10), "[a,b,c] }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3_0, Location(1,9), "[a,b,c] {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4_0, Location(1,9), "[a,b,c] [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5_0, Location(1,9), "[a,b,c] -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq1_1, Location(2,2), "[a,b,c]\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq2_1, Location(2,2), "[a,b,c]\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq3_1, Location(2,1), "[a,b,c]\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq4_1, Location(2,1), "[a,b,c]\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Seq5_1, Location(2,1), "[a,b,c]\n-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1, Location(1, 8), "{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2, Location(1, 8), "{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3, Location(1, 7), "{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4, Location(1, 7), "{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5, Location(1, 7), "{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_0, Location(1,9), "{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_0, Location(1,9), "{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_0, Location(1,8), "{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_0, Location(1,8), "{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_0, Location(1,8), "{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_1, Location(2,2), "{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_1, Location(2,2), "{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_1, Location(2,1), "{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_1, Location(2,1), "{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_1, Location(2,1), "{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1, Location(2, 9), "---\n[a,b,c]]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2, Location(2, 9), "---\n[a,b,c]}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3, Location(2, 8), "---\n[a,b,c]{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4, Location(2, 8), "---\n[a,b,c][")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5, Location(2, 8), "---\n[a,b,c]-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1_0, Location(2,10), "---\n[a,b,c] ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2_0, Location(2,10), "---\n[a,b,c] }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3_0, Location(2,9), "---\n[a,b,c] {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4_0, Location(2,9), "---\n[a,b,c] [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5_0, Location(2,9), "---\n[a,b,c] -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq1_1, Location(3,2), "---\n[a,b,c]\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq2_1, Location(3,2), "---\n[a,b,c]\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq3_1, Location(3,1), "---\n[a,b,c]\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq4_1, Location(3,1), "---\n[a,b,c]\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Seq5_1, Location(3,1), "---\n[a,b,c]\n-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1, Location(2, 8), "---\n{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2, Location(2, 8), "---\n{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3, Location(2, 7), "---\n{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4, Location(2, 7), "---\n{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5, Location(2, 7), "---\n{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_0, Location(2,9), "---\n{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_0, Location(2,9), "---\n{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_0, Location(2,8), "---\n{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_0, Location(2,8), "---\n{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_0, Location(2,8), "---\n{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_1, Location(3,2), "---\n{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_1, Location(3,2), "---\n{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_1, Location(3,1), "---\n{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_1, Location(3,1), "---\n{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_1, Location(3,1), "---\n{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokensScalar0, Location(2,6), "word1  # comment\nword2")

ENGINE_TEST(ExtraTokensNoFalseError0,
            ("---\n"
             "map : {foo: bar, notag: none}\n"
             "seq : [foo, bar]\n"
             "...\n",
             "---\n"
             "map: {foo: bar,notag: none}\n"
             "seq: [foo,bar]\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=VAL :seq\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("seq"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(ExtraTokensNoFalseError1,
            ("---\n"
             "*mapref : {foo: bar, notag: none}\n"
             "*seqref : [foo, bar]\n"
             "...\n",
             "---\n"
             "*mapref : {foo: bar,notag: none}\n"
             "*seqref : [foo,bar]\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "=ALI *mapref\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :notag\n"
            "=VAL :none\n"
            "-MAP\n"
            "=ALI *seqref\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_ref("*mapref"));
        ___(ps.begin_map_val_flow());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("notag"));
            ___(ps.set_val_scalar_plain("none"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_key_ref("*seqref"));
        ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("foo"));
            ___(ps.add_sibling());
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarDoc0,
            ("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~",
             "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockSeq0,
            ("- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~",
             "- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockMap0,
            ("key: a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"
             "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~: val\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :key\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "=VAL :a!\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\n"
            "=VAL :val\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_0, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_1, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_0, Location(2,28), "[\n"
                   "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_1, Location(2,28), "[\n"
                   "a!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow1Seq0, Location(1,38), "[a!\"#$%&'()*+,-./09:;<=>?@]^_`az{|}~]")
ENGINE_TEST(PlainScalarFlow0Seq1,
            ("[\n"
             "a!\"#$%&'()*+,-./09:;<=>?@AZ\\\n]",
             "[a!\"#$%&'()*+,-./09:;<=>?@AZ\\]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :a!\"#$%&'()*+\n"
            "=VAL :-./09:;<=>?@AZ\\\\\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a!\"#$%&'()*+"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("-./09:;<=>?@AZ\\"));
    ___(ps.end_seq());
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
