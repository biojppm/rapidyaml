#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool multiline = true;


ENGINE_TEST(PlainScalarUnfiltered,
            "foo: bar\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.mark_key_scalar_unfiltered());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.mark_val_scalar_unfiltered());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarWithColon0,
            "a::\n"
            ,
            "a:: \n"
            ,
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
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColon1,
            "key ends with two colons::: value\n"
            ,
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
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColonSeq,
            "- ::\n"
            "- x::\n"
            "- :x::\n"
            ,
            "- :: \n"
            "- x:: \n"
            "- :x:: \n"
            ,
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
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("x:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain(":x:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(PlainScalarWithColonSeqFlow, HAS_MULTILINE_SCALAR,
            "[while\n:validplain]"
            ,
            "[\n  while :validplain\n]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :while :validplain\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("while :validplain"));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(BlockPlainScalarCommaDoc,
            "a, b\n"
            ,
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
            "a, b: c, d\n"
            ,
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
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(BlockPlainScalarCommaSeq,
            "- a, b\n- c, d\n"
            ,
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
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(ExtraTokensScalar0, Location(2,6), "word1  # comment\nword2")


//-----------------------------------------------------------------------------

ENGINE_TEST(PlainScalarDoc0,
            "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"
            ,
            "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"
            ,
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
            "- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~"
            ,
            "- a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"
            ,
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
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(PlainScalarBlockMap0,
            "key: a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n"
            "a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~: val\n"
            ,
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
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_0, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq0_1, Location(1,29), "[a!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_0, Location(2,28), "[\na!\"#$%&'()*+,-./09:;<=>?@AZ[\\]^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow0Seq1_1, Location(2,28), "[\na!\"#$%&'()*+,-./09:;<=>?@AZ{|}^_`az{|}~\n]")
ENGINE_TEST_ERRLOC(PlainScalarFlow1Seq0, Location(1,38), "[a!\"#$%&'()*+,-./09:;<=>?@]^_`az{|}~]")
ENGINE_TEST(PlainScalarFlow0Seq1,
            "[\n"
            "a!\"#$%&'()*+,-./09:;<=>?@AZ\\\n]"
            ,
            "[\n  a!\"#$%&'()*+,\n  -./09:;<=>?@AZ\\\n]\n"
            ,
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
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(BlockIndentationInvalid0Litl, Location(1,1), "|0\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid0Fold, Location(1,1), ">0\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid1Litl, Location(1,1), "|10\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid1Fold, Location(1,1), ">10\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl0, Location(1,1), "|-1-\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold0, Location(1,1), ">-1-\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl1, Location(1,1), "|+1+\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold1, Location(1,1), ">+1+\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl2, Location(1,1), "|--1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold2, Location(1,1), ">--1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl3, Location(1,1), "|++1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold3, Location(1,1), ">++1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl4, Location(1,1), "|1--\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold4, Location(1,1), ">1--\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl5, Location(1,1), "|1++\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold5, Location(1,1), ">1++\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl6, Location(1,1), "|---\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold6, Location(1,1), ">---\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Litl7, Location(1,1), "|+++\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid2Fold7, Location(1,1), ">+++\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid3Litl0, Location(1,1), "| no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid3Fold0, Location(1,1), "> no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid3Litl1, Location(1,1), "| \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid3Fold1, Location(1,1), "> \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid4Litl0, Location(1,1), "|1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid4Fold0, Location(1,1), ">1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid4Litl1, Location(1,1), "|1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid4Fold1, Location(1,1), ">1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid5Litl0, Location(1,1), "|1+ no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid5Fold0, Location(1,1), ">1+ no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid5Litl1, Location(1,1), "|1+ \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid5Fold1, Location(1,1), ">1+ \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid6Litl0, Location(1,1), "|1- no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid6Fold0, Location(1,1), ">1- no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid6Litl1, Location(1,1), "|1- \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid6Fold1, Location(1,1), ">1- \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid7Litl0, Location(1,1), "|+1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid7Fold0, Location(1,1), ">+1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid7Litl1, Location(1,1), "|+1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid7Fold1, Location(1,1), ">+1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid8Litl0, Location(1,1), "|-1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid8Fold0, Location(1,1), ">-1 no comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid8Litl1, Location(1,1), "|-1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid8Fold1, Location(1,1), ">-1 \tno comment\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Litl0, Location(1,1), "|#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Fold0, Location(1,1), ">#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Litl1, Location(1,1), "|1#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Fold1, Location(1,1), ">1#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Litl2, Location(1,1), "|-1#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Fold2, Location(1,1), ">-1#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Litl3, Location(1,1), "|1-#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid9Fold3, Location(1,1), ">1-#\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Litl0, Location(1,1), "||\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Fold0, Location(1,1), ">>\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Litl1, Location(1,1), "||1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Fold1, Location(1,1), ">>1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Litl2, Location(1,1), "||-1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Fold2, Location(1,1), ">>-1\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Litl3, Location(1,1), "||1-\n  1")
ENGINE_TEST_ERRLOC(BlockIndentationInvalid10Fold3, Location(1,1), ">>1-\n  1")

} // namespace yml
} // namespace c4
