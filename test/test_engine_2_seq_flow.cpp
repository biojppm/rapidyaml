#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool multiline = true;
static constexpr const bool singleline = false;


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleSeqFlow,
            "[foo,bar,baz]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(SimpleSeqFlowErr0, Location(1,1,2), "[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr1, Location(2,1,3), "[[")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr2, Location(3,1,4), "[[]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr3, Location(5,1,6), "[[[]]")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr4, Location(4,1,5), "[foo")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr5, Location(5,1,6), "[foo,")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr6, Location(12,1,13), "[foo,bar,baz")
ENGINE_TEST_ERRLOC(SimpleSeqFlowErr7, Location(8,1,9), "[[a,b,c]")


ENGINE_TEST(NestedSeqFlowEmpty,
            "[[]]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq3FlowEmpty,
            "[[[]]]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedSeq4FlowEmpty,
            "[[[[]]]]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleSeqFlowMultiline,
            "[\nfoo\n,\nbar\n,\nbaz\n]"
            ,
            "[\n  foo,\n  bar,\n  baz\n]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :baz\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqSeqFlow,
            "[[foo1,bar1,baz1],\n[foo2,bar2,baz2]]"
            ,
            "[\n  [foo1,bar1,baz1],\n  [foo2,bar2,baz2]\n]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+SEQ []\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "=VAL :baz1\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=VAL :baz2\n"
            "-SEQ\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz1"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("foo2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("baz2"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}



//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqFlowCommentAfterComma, Location(6,1,7), "[a, b,#c\n]")

ENGINE_TEST(SeqFlowCommentAfterCommaWhitespace,
            "[a, b, #c\n]"
            ,
            "[\n  a,\n  b\n]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

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
ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_0, Location(1,33), "[a,b,c,                   d]:foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_9_1, Location(2, 1), "[a,b,c,                   d]\n:\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_0, Location(2, 1), "[a,b,c,                   d]\n: foo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_10_1, Location(2, 1), "[a,b,c,                   d]\n: \nfoo")
ENGINE_TEST_ERRLOC(tokens_after_seqflow_11_0, Location(1,34), "[a,b,c,                   d] &foo")
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


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DashScalar0_0, Location(1, 2), "[-]")
ENGINE_TEST_ERRLOC(DashScalar0_0_0, Location(1, 2), "[- ]")
ENGINE_TEST_ERRLOC(DashScalar0_0_1, Location(1, 2), "[-\t]")
ENGINE_TEST_ERRLOC(DashScalar0_0_2, Location(1, 2), "[-\n]")
ENGINE_TEST_ERRLOC(DashScalar0_0_3, Location(1, 2), "[-\r\n]")
ENGINE_TEST_ERRLOC(DashScalar0_1, Location(1, 2), "[-,-]")
ENGINE_TEST_ERRLOC(DashScalar0_1_0, Location(1, 2), "[- ,- ]")
ENGINE_TEST_ERRLOC(DashScalar0_1_1, Location(1, 2), "[-\t,-\t]")
ENGINE_TEST_ERRLOC(DashScalar0_1_2, Location(1, 2), "[-\n,-\n]")
ENGINE_TEST_ERRLOC(DashScalar0_1_3, Location(1, 2), "[-\r\n,-\r\n]")
ENGINE_TEST_ERRLOC(DashScalar0_2, Location(1, 5), "[--,-]")
ENGINE_TEST_ERRLOC(DashScalar0_2_0, Location(1, 5), "[-a,-]")
ENGINE_TEST_ERRLOC(DashScalar0_2_1, Location(1, 5), "[-a,-\t]")
ENGINE_TEST_ERRLOC(DashScalar0_2_2, Location(1, 5), "[-a,-\n]")
ENGINE_TEST_ERRLOC(DashScalar0_2_3, Location(1, 5), "[-a,-\r\n]")
ENGINE_TEST(DashScalar1_0,
            "[--]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :--\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("--"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(DashScalar1_1,
            "[--,--]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :--\n"
            "=VAL :--\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("--"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("--"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(DashScalar2_0,
            "[---]"
            ,
            "[  ---]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :---\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(DashScalar2_1,
            "[---,---]"
            ,
            "[  ---,  ---]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :---\n"
            "=VAL :---\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DocTokens0Err0_0, Location(2, 1),
                   "["         "\n"
                   "--- ,"     "\n"
                   "..."       "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err0_1, Location(2, 1),
                   "["         "\n"
                   "... ,"      "\n"
                   "---"       "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err1_0, Location(2, 1),
                   "["         "\n"
                   "---,"      "\n"
                   "..."       "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err1_1, Location(2, 1),
                   "["         "\n"
                   "...,"      "\n"
                   "---"       "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err2_0, Location(2, 1),
                   "["         "\n"
                   "---,"      "\n"
                   "..."       "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err2_1, Location(2, 1),
                   "["         "\n"
                   "...\t,"    "\n"
                   "---\t"     "\n"
                   "]"         "\n"
                   )
ENGINE_TEST_ERRLOC(DocTokens0Err3, Location(4, 1),
                   "["         "\n"
                   ""          "\n"
                   ""          "\n"
                   "--- ,"     "\n"
                   ""          "\n"
                   ""          "\n"
                   "..."       "\n"
                   ""          "\n"
                   "]"         "\n"
                   )

ENGINE_TEST(DocTokens0Ok,
            "["         "\n"
            " --- ,"     "\n"
            " ..."       "\n"
            "]"         "\n"
            ,
            "["         "\n"
            "  ---,"    "\n"
            "  ..."     "\n"
            "]"         "\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :---\n"
            "=VAL :...\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("---"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("..."));
    ___(ps.end_seq_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndentTab, Location(3, 1),
                   "- ["      "\n"
                   " foo,"     "\n"
                   "\tfoo,"   "\n"
                   " ]"       "\n"
                   )


ENGINE_TEST_ERRLOC(SeqNestedIndent0_00, Location(5, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "- ["     "\n"
                   "a"     "\n" // error here
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_0, Location(6, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "- ["     "\n"
                   " a"     "\n"
                   "]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_1, Location(3, 1),
                   "- ["     "\n"
                   " a,"     "\n"
                   "a"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_2, Location(3, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   ","     "\n" // error here
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_3, Location(3, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_4, Location(6, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"     "\n"
                   " a"     "\n"
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_0, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_1, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_2, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_0, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_1, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_2, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )


ENGINE_TEST_ERRLOC(SeqNestedIndent1_00, Location(5, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  - ["     "\n"
                   "  a"     "\n" // error here
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_0, Location(6, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  - ["     "\n"
                   "   a"     "\n"
                   "  ]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_1, Location(3, 3),
                   "- - ["     "\n"
                   "   a,"     "\n"
                   "  a"     "\n" // error here
                   "  ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_2, Location(3, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "  ,"     "\n" // error here
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_3, Location(3, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_4, Location(6, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"     "\n"
                   "   a"     "\n"
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_0, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_1, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_2, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_0, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_1, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_2, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_0, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_1, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_2, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndent2_00, Location(5, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    - ["     "\n"
                   "    a"     "\n" // error here
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_0, Location(6, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    - ["     "\n"
                   "     a"     "\n"
                   "    ]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_1, Location(3, 5),
                   "- - - ["     "\n"
                   "     a,"     "\n"
                   "    a"     "\n" // error here
                   "    ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_2, Location(3, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "    ,"     "\n" // error here
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_3, Location(3, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_4, Location(6, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_0, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_1, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_2, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_0, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_1, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_2, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_0, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_1, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_2, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(MapNestedIndent0_00, Location(5, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "k: ["     "\n"
                   "a"     "\n" // error here
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_0, Location(6, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "k: ["     "\n"
                   " a"     "\n"
                   "]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_1, Location(3, 1),
                   "k: ["     "\n"
                   " a,"     "\n"
                   "a"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_2, Location(3, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   ","     "\n" // error here
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_3, Location(3, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_4, Location(6, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"     "\n"
                   " a"     "\n"
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_0, Location(7, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_1, Location(8, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_2, Location(9, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_0, Location(7, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_1, Location(8, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_2, Location(9, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapNestedIndent1_00, Location(1 + 5, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  k: ["     "\n"
                   "  a"     "\n" // error here
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_0, Location(1 + 6, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  ]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_1, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a,"     "\n"
                   "  a"     "\n" // error here
                   "  ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_2, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  ,"     "\n" // error here
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_3, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_4, Location(1 + 6, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"     "\n"
                   "   a"     "\n"
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_0, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_1, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_2, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_0, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""        "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_1, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_2, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapNestedIndent2_00, Location(2 + 5, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    k: ["     "\n"
                   "    a"     "\n" // error here
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_0, Location(2 + 6, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "    ]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_1, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a,"     "\n"
                   "    a"     "\n" // error here
                   "    ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_2, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "    ,"     "\n" // error here
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_3, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_4, Location(2 + 6, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_0, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_1, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_2, Location(2 + 9, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_0, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""        "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_1, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_2, Location(2 + 9, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )

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
