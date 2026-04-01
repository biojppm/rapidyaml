#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool multiline = true; // NOLINT
static constexpr const bool singleline = false; // NOLINT


ENGINE_TEST(SimpleMapFlowEmpty,
            "{}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlowEmptyMultiline,
            "{\n}"
            ,
            "{\n}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlow,
            "{foo: bar,foo2: bar2}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlowMultiline0,
            "{foo: bar,foo2: bar2\n}"
            ,
            "{\n  foo: bar,\n  foo2: bar2\n}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMapFlow,
            HAS_CONTAINER_KEYS
            ,
            Location(1,1,2)
            ,
            "{{}: {}}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(NestedMap3FlowEmpty,
            HAS_CONTAINER_KEYS
            ,
            Location(1,1,2)
            ,
            "{{{}: {}}: {{}: {}}}",
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "-MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SimpleMapFlowMultiline,
            "{\nfoo:\n bar\n,\nfoo2:\nbar2\n}"
            ,
            "{\n  foo: bar,\n  foo2: bar2\n}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(SimpleMapFlowErr0, Location(1,1,2), "{")
ENGINE_TEST_ERRLOC(SimpleMapFlowErr1, Location(5,1,6), "{a: b")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SimpleMapFlowCommentAfterComma, Location(6,1,7), "{a: b,#c\n}")

ENGINE_TEST(SimpleMapFlowCommentAfterCommaWhitespace,
            "{a: b, #c\n}"
            ,
            "{\n  a: b\n}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC(SimpleMapFlowCommentAfterComma1, Location(3,1,4), "{a,#c\n}")
ENGINE_TEST(SimpleMapFlowCommentAfterCommaWhitespace1,
            "{a, #c\n}"
            ,
            "{\n  a: \n}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(multiline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SimpleMapContainerKeyFlow,
            HAS_CONTAINER_KEYS,
            "{{this: is, a: keymap}: [and,now,a,seq,val]}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(MapKeyFlow,
            HAS_CONTAINER_KEYS, Location(1, 1, 2),
            "{{foo: bar}: baz}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
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
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqKeyFlow,
            HAS_CONTAINER_KEYS, Location(1,1,2),
            "{[foo, bar]: baz}"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}



//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(tokens_after_0_0, Location(1,32), "{ first: Sammy, last: Sosa }foo")
ENGINE_TEST_ERRLOC(tokens_after_0_1, Location(2, 4), "{ first: Sammy, last: Sosa }\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_2_0, Location(1,29), "{ first: Sammy, last: Sosa }- foo")
ENGINE_TEST_ERRLOC(tokens_after_2_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_3_0, Location(1,29), "{ first: Sammy, last: Sosa }[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_3_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_4_0, Location(1,29), "{ first: Sammy, last: Sosa }{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_4_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_5_0, Location(1,29), "{ first: Sammy, last: Sosa }'foo'")
ENGINE_TEST_ERRLOC(tokens_after_5_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_6_0, Location(1,29), "{ first: Sammy, last: Sosa }|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_6_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_7_0, Location(1,29), "{ first: Sammy, last: Sosa }>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_7_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_8_0, Location(1,33), "{ first: Sammy, last: Sosa }?foo")
ENGINE_TEST_ERRLOC(tokens_after_8_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_9_0, Location(1,33), "{ first: Sammy, last: Sosa }:foo")
ENGINE_TEST_ERRLOC(tokens_after_9_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n:\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_10_0, Location(2, 1), "{ first: Sammy, last: Sosa }\n: foo")
ENGINE_TEST_ERRLOC(tokens_after_10_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n: \nfoo")
ENGINE_TEST_ERRLOC(tokens_after_11_0, Location(1,34), "{ first: Sammy, last: Sosa } &foo")
ENGINE_TEST_ERRLOC(tokens_after_11_1, Location(2, 5), "{ first: Sammy, last: Sosa }\n&foo")

ENGINE_TEST_ERRLOC(ExtraTokens1_1, Location(1, 8), "{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokens1_2, Location(1, 8), "{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokens1_3, Location(1, 7), "{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokens1_4, Location(1, 7), "{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokens1_5, Location(1, 7), "{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokens1_1_0, Location(1,9), "{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokens1_2_0, Location(1,9), "{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokens1_3_0, Location(1,8), "{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokens1_4_0, Location(1,8), "{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokens1_5_0, Location(1,8), "{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokens1_1_1, Location(2,2), "{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokens1_2_1, Location(2,2), "{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokens1_3_1, Location(2,1), "{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokens1_4_1, Location(2,1), "{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokens1_5_1, Location(2,1), "{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokens2_1, Location(2, 8), "---\n{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokens2_2, Location(2, 8), "---\n{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokens2_3, Location(2, 7), "---\n{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokens2_4, Location(2, 7), "---\n{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokens2_5, Location(2, 7), "---\n{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokens2_1_0, Location(2,9), "---\n{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokens2_2_0, Location(2,9), "---\n{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokens2_3_0, Location(2,8), "---\n{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokens2_4_0, Location(2,8), "---\n{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokens2_5_0, Location(2,8), "---\n{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokens2_1_1, Location(3,2), "---\n{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokens2_2_1, Location(3,2), "---\n{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokens2_3_1, Location(3,1), "---\n{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokens2_4_1, Location(3,1), "---\n{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokens2_5_1, Location(3,1), "---\n{a: b}\n-")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_0, Location(5, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   "a"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_1, Location(6, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_0, Location(7, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_1, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_2, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_0, Location(7, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_1, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_2, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_0, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_1, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_0, Location(11, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_1, Location(12, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_2, Location(13, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_0, Location(11, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_1, Location(12, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_2, Location(13, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentColon0_0, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   ":"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket0_0, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "}"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket0_1, Location(11, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " ,"     "\n"
                   "}"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentComma0_0, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ","       "\n"
                   " }"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_0, Location(5, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "  a"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_1, Location(6, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_0, Location(7, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_1, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_2, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_0, Location(7, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "  "   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_1, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_2, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  b"      "\n"
                   "   }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_0, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "  b"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_1, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_0, Location(11, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "      "   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_1, Location(12, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_2, Location(13, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_0, Location(11, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  "   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_1, Location(12, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_2, Location(13, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  c"      "\n"
                   "   }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentColon1_0, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "  :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket1_0, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket1_1, Location(11, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   ,"     "\n"
                   "  }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentComma1_0, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  ,"       "\n"
                   "   }"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_0, Location(5, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "    a"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_1, Location(6, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_0, Location(7, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_1, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_2, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_0, Location(7, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_1, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_2, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    b"      "\n"
                   "     }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_0, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "    b"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_1, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_0, Location(11, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "        "   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_1, Location(12, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_2, Location(13, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_0, Location(11, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_1, Location(12, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_2, Location(13, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    c"      "\n"
                   "     }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentColon2_0, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "    :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket2_0, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket2_1, Location(11, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     ,"     "\n"
                   "    }"     "\n"
                   )

ENGINE_TEST_ERRLOC(SeqNestedIndentComma2_0, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    ,"       "\n"
                   "     }"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_0, Location(5, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   "a"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_1, Location(6, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_0, Location(7, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_1, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_2, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_0, Location(7, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_1, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_2, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "b"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_0, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   "b"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_1, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_0, Location(11, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_1, Location(12, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_2, Location(13, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_0, Location(11, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_1, Location(12, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_2, Location(13, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "c"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapValNestedIndentColon0_0, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   ":"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapValNestedIndentBracket0_0, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "}"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapValNestedIndentBracket0_1, Location(11, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " ,"     "\n"
                   "}"     "\n"
                   )

ENGINE_TEST_ERRLOC(MapValNestedIndentComma0_0, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   ","       "\n"
                   " }"     "\n"
                   )


//-----------------------------------------------------------------------------


const TestCaseFlags_e keyflags = (TestCaseFlags_e)(HAS_CONTAINER_KEYS|NO_TREE_PARSE|EXPECT_PARSE_ERROR);

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_0, keyflags, ExpectedErrorType::err_parse,
                    Location(6, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "?: {"    "\n"
                    "a"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_1, keyflags, ExpectedErrorType::err_parse,
                    Location(7, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_0, keyflags, ExpectedErrorType::err_parse,
                    Location(8, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "    "   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_1, keyflags, ExpectedErrorType::err_parse,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_2, keyflags, ExpectedErrorType::err_parse,
                    Location(10, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_0, keyflags, ExpectedErrorType::err_parse,
                    Location(8, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    ""   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_1, keyflags, ExpectedErrorType::err_parse,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_2, keyflags, ExpectedErrorType::err_parse,
                    Location(10, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_0, keyflags, ExpectedErrorType::err_parse,
                    Location(10, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"      "\n"
                    " :"      "\n"
                    "b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_1, keyflags, ExpectedErrorType::err_parse,
                    Location(11, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_0, keyflags, ExpectedErrorType::err_parse,
                    Location(12, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    "    "   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_1, keyflags, ExpectedErrorType::err_parse,
                    Location(13, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_2, keyflags, ExpectedErrorType::err_parse,
                    Location(14, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_0, keyflags, ExpectedErrorType::err_parse,
                    Location(12, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    ""   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_1, keyflags, ExpectedErrorType::err_parse,
                    Location(13, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_2, keyflags, ExpectedErrorType::err_parse,
                    Location(14, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    " :"      "\n"
                    " b"      "\n"
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    "c"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentColon0_0, keyflags, ExpectedErrorType::err_parse,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"      "\n"
                    ":"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_0, keyflags, ExpectedErrorType::err_parse,
                    Location(11, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"     "\n"
                    " a"      "\n"
                    " :"      "\n"
                    " b"      "\n"
                    "}"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_1, keyflags, ExpectedErrorType::err_parse,
                    Location(12, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"      "\n"
                    " :"      "\n"
                    " b"      "\n"
                    " ,"     "\n"
                    "}"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentComma0_0, keyflags, ExpectedErrorType::err_parse,
                    Location(11, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"      "\n"
                    " :"      "\n"
                    " b"      "\n"
                    ","       "\n"
                    " }"     "\n"
                    ": v"     "\n"
)



//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndentKeyTab, Location(3, 1),
                   "- {"      "\n"
                   " foo,"     "\n"
                   "\tfoo,"   "\n"
                   " }"       "\n"
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
