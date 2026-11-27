#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_0, Location(5, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   "a"      "\n"
                   " }"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_0tab, Location(5, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   "\ta"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_1tab, Location(6, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_0tab, Location(7, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_1tab, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_2_2tab, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "    "   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_0tab, Location(7, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_1tab, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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

ENGINE_TEST_ERRLOC(MapValNestedIndentKey0_3_2tab, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   ""   "\n" // important to test all line with spaces
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_0tab, Location(9, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   "\tb"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_1tab, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_0tab, Location(11, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_1tab, Location(12, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_2_2tab, Location(13, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_0tab, Location(11, 1),
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
                   "\tc"      "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_1tab, Location(12, 1),
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
                   "\tc"      "\n"
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

ENGINE_TEST_ERRLOC(MapValNestedIndentVal0_3_2tab, Location(13, 1),
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
                   "\tc"      "\n"
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

ENGINE_TEST_ERRLOC(MapValNestedIndentColon0_0tab, Location(8, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   "\t:"      "\n"
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

ENGINE_TEST_ERRLOC(MapValNestedIndentBracket0_0tab, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\t}"     "\n"
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

ENGINE_TEST_ERRLOC(MapValNestedIndentBracket0_1tab, Location(11, 1),
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
                   "\t}"     "\n"
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
ENGINE_TEST_ERRLOC(MapValNestedIndentComma0_0tab, Location(10, 1),
                   "k: {"    "\n"
                   " a"     "\n"
                   " :"     "\n"
                   " b"     "\n"
                   " }"     "\n"
                   "k: {"    "\n"
                   " a"      "\n"
                   " :"      "\n"
                   " b"      "\n"
                   "\t,"       "\n"
                   " }"     "\n"
                   )


//-----------------------------------------------------------------------------


const TestCaseFlags_e keyflags = (TestCaseFlags_e)(HAS_CONTAINER_KEYS|EXPECT_PARSE_ERROR);

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_0tab, keyflags,
                    Location(6, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "?: {"    "\n"
                    "\ta"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_1tab, keyflags,
                    Location(7, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_0tab, keyflags,
                    Location(8, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "    "   "\n" // important to test all line with spaces
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_1tab, keyflags,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    "    "   "\n" // important to test all line with spaces
                    "    "   "\n" // important to test all line with spaces
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_2, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_2_2tab, keyflags,
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
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_0tab, keyflags,
                    Location(8, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    ""   "\n" // important to test all line with spaces
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_1tab, keyflags,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"     "\n"
                    ""   "\n" // important to test all line with spaces
                    ""   "\n" // important to test all line with spaces
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_2, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentKey0_3_2tab, keyflags,
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
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_0tab, keyflags,
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
                    "\tb"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_1tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_0tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_1tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_2, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_2_2tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_0, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_0tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_1, keyflags,
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
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_1tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)
ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_2, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentVal0_3_2tab, keyflags,
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
                    "\tc"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentColon0_0, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentColon0_0tab, keyflags,
                    Location(9, 1),
                    "? {"    "\n"
                    " a"     "\n"
                    " :"     "\n"
                    " b"     "\n"
                    " }"     "\n"
                    ": v"     "\n"
                    "? {"    "\n"
                    " a"      "\n"
                    "\t:"      "\n"
                    " b"      "\n"
                    " }"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_0, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_0tab, keyflags,
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
                    "\t}"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_1, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentBracket0_1tab, keyflags,
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
                    "\t}"     "\n"
                    ": v"     "\n"
)

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentComma0_0, keyflags,
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

ENGINE_TEST_ERRLOC_(MapKeyNestedIndentComma0_0tab, keyflags,
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
                    "\t,"       "\n"
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
