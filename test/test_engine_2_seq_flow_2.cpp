#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_00tab, Location(5, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "- ["     "\n"
                   "\ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_0tab, Location(6, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "- ["     "\n"
                   " a"     "\n"
                   "\t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_1, Location(3, 1),
                   "- ["     "\n"
                   " a,"     "\n"
                   "a"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_1tab, Location(3, 1),
                   "- ["     "\n"
                   " a,"     "\n"
                   "\ta"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_2, Location(3, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   ","     "\n" // error here
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_2tab, Location(3, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   "\t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_3tab, Location(3, 1),
                   "- ["     "\n"
                   " a"     "\n"
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_4tab, Location(6, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"     "\n"
                   " a"     "\n"
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_0tab, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_1tab, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_5_2tab, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "    "   "\n" // important to test this case as well
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_0tab, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_1tab, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent0_6_2tab, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_0, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_0tab, Location(7, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_1, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_1tab, Location(8, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_2, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent0_7_2tab, Location(9, 1),
                   "- ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   " \t\t\t\t\t\t\t\t\t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndent1_00, Location(5, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  - ["     "\n"
                   "  a"     "\n" // error here
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_00tab, Location(5, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  - ["     "\n"
                   "  \ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_0tab, Location(6, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  - ["     "\n"
                   "   a"     "\n"
                   "  \t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_1, Location(3, 3),
                   "- - ["     "\n"
                   "   a,"     "\n"
                   "  a"     "\n" // error here
                   "  ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_1tab, Location(3, 3),
                   "- - ["     "\n"
                   "   a,"     "\n"
                   "  \ta"     "\n" // error here
                   "  ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_2, Location(3, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "  ,"     "\n" // error here
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_2tab, Location(3, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "  \t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_3tab, Location(3, 3),
                   "- - ["     "\n"
                   "   a"     "\n"
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_4tab, Location(6, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"     "\n"
                   "   a"     "\n"
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_0tab, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_1tab, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_5_2tab, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "      "   "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_0tab, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_1tab, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_6_2tab, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_0tab, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_1tab, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent1_7_2tab, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "  \tb"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_0, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t"       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_0tab, Location(7, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"       "\n" // important to test this case as well
                   "  \tb"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_1, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_1tab, Location(8, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "  \tb"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_2, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent1_8_2tab, Location(9, 3),
                   "- - ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "   \t\t\t"       "\n" // important to test this case as well
                   "  \tb"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqNestedIndent2_00, Location(5, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    - ["     "\n"
                   "    a"     "\n" // error here
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_00tab, Location(5, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    - ["     "\n"
                   "    \ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_0tab, Location(6, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   "    - ["     "\n"
                   "     a"     "\n"
                   "    \t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_1, Location(3, 5),
                   "- - - ["     "\n"
                   "     a,"     "\n"
                   "    a"     "\n" // error here
                   "    ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_1tab, Location(3, 5),
                   "- - - ["     "\n"
                   "     a,"     "\n"
                   "    \ta"     "\n" // error here
                   "    ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_2, Location(3, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "    ,"     "\n" // error here
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_2tab, Location(3, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "    \t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_3tab, Location(3, 5),
                   "- - - ["     "\n"
                   "     a"     "\n"
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_4tab, Location(6, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_0tab, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_1tab, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_5_2tab, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "        "   "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_0tab, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_1tab, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_6_2tab, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_0tab, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_1tab, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(SeqNestedIndent2_7_2tab, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   " "       "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_0, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_0tab, Location(7, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_1, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_1tab, Location(8, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_2, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(SeqNestedIndent2_8_2tab, Location(9, 5),
                   "- - - ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )

} // namespace yml
} // namespace c4
