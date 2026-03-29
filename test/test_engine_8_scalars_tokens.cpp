#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool multiline = true;
//static constexpr const bool singleline = false; // NOLINT


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
