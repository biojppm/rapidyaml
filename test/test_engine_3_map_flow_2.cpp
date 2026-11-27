#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_0, Location(5, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   "a"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_0tab, Location(5, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   "\ta"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_1tab, Location(6, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_0tab, Location(7, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_1tab, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_2_2tab, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_0tab, Location(7, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_1tab, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentKey0_3_2tab, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_0tab, Location(9, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_1tab, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_0tab, Location(11, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_1tab, Location(12, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_2_2tab, Location(13, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_0tab, Location(11, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_1tab, Location(12, 1),
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
                   "\tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentVal0_3_2tab, Location(13, 1),
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
                   "\tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentColon0_0tab, Location(8, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   "\t:"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket0_0tab, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket0_1tab, Location(11, 1),
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
                   "\t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentComma0_0tab, Location(10, 1),
                   "- {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "- {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\t,"       "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_0tab, Location(5, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "  \ta"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_1tab, Location(6, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_0tab, Location(7, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_1tab, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_2_2tab, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "      "   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_0tab, Location(7, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "  "   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_1tab, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentKey1_3_2tab, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_0tab, Location(9, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "  \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_1tab, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"     "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_0tab, Location(11, 3),
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
                   "  \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_1tab, Location(12, 3),
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
                   "  \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_2_2tab, Location(13, 3),
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
                   "  \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_0tab, Location(11, 3),
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
                   "  \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_1tab, Location(12, 3),
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
                   "  \tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentVal1_3_2tab, Location(13, 3),
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
                   "  \tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentColon1_0tab, Location(8, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "  \t:"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket1_0tab, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  \t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket1_1tab, Location(11, 3),
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
                   "  \t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentComma1_0tab, Location(10, 3),
                   "- - {"    "\n"
                   "   a"     "\n"
                   "   :"     "\n"
                   "   b"     "\n"
                   "   }"     "\n"
                   "  - {"    "\n"
                   "   a"      "\n"
                   "   :"      "\n"
                   "   b"      "\n"
                   "  \t,"       "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_0tab, Location(5, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "    \ta"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_1tab, Location(6, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_0tab, Location(7, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_1tab, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_2_2tab, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "        "   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_0tab, Location(7, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_1tab, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentKey2_3_2tab, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_0tab, Location(9, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "    \tb"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_1tab, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"     "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_0tab, Location(11, 5),
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
                   "    \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_1tab, Location(12, 5),
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
                   "    \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_2_2tab, Location(13, 5),
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
                   "    \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_0tab, Location(11, 5),
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
                   "    \tc"      "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_1tab, Location(12, 5),
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
                   "    \tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentVal2_3_2tab, Location(13, 5),
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
                   "    \tc"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentColon2_0tab, Location(8, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "    \t:"      "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket2_0tab, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    \t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentBracket2_1tab, Location(11, 5),
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
                   "    \t}"     "\n"
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

ENGINE_TEST_ERRLOC(SeqNestedIndentComma2_0tab, Location(10, 5),
                   "- - - {"    "\n"
                   "     a"     "\n"
                   "     :"     "\n"
                   "     b"     "\n"
                   "     }"     "\n"
                   "    - {"    "\n"
                   "     a"      "\n"
                   "     :"      "\n"
                   "     b"      "\n"
                   "    \t,"       "\n"
                   "     }"     "\n"
                   )

} // namespace yml
} // namespace c4
