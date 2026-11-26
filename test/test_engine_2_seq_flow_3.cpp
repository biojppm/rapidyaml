#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


ENGINE_TEST_ERRLOC(MapNestedIndent0_00, Location(5, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "k: ["     "\n"
                   "\ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_0tab, Location(6, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   "k: ["     "\n"
                   " a"     "\n"
                   "\t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_1, Location(3, 1),
                   "k: ["     "\n"
                   " a,"     "\n"
                   "a"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_1tab, Location(3, 1),
                   "k: ["     "\n"
                   " a,"     "\n"
                   "a"     "\n" // error here
                   "]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_2, Location(3, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   "\t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_3tab, Location(3, 1),
                   "k: ["     "\n"
                   " a"     "\n"
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_4tab, Location(6, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"     "\n"
                   " a"     "\n"
                   "\tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_0tab, Location(7, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_1tab, Location(8, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_5_2tab, Location(9, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_0tab, Location(7, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_1tab, Location(8, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_6_2tab, Location(9, 1),
                   "k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_0, Location(7, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_0tab, Location(7, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_1, Location(8, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_1tab, Location(8, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_2, Location(9, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   "b"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent0_7_2tab, Location(9, 1),
                   "k: ["     "\n"
                   "  a"     "\n"
                   " b"     "\n"
                   " ,"      "\n"
                   " a"     "\n"
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   " \t\t\t"       "\n" // important to test this case as well
                   "\tb"     "\n"
                   ","     "\n"
                   " a"     "\n"
                   " ]"     "\n"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(MapNestedIndent1_00, Location(1 + 5, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  k: ["     "\n"
                   "  a"     "\n" // error here
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_00tab, Location(1 + 5, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  k: ["     "\n"
                   "  \ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_0tab, Location(1 + 6, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  \t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_1, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a,"     "\n"
                   "  a"     "\n" // error here
                   "  ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_1tab, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a,"     "\n"
                   "  a"     "\n" // error here
                   "  \t]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_2, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  ,"     "\n" // error here
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_2tab, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  \t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_3tab, Location(1 + 3, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "   a"     "\n"
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_4tab, Location(1 + 6, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"     "\n"
                   "   a"     "\n"
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_0tab, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_1tab, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_5_2tab, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_0tab, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   ""        "\n" // important to test this case as well
                   "  \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_1tab, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_6_2tab, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_0, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"        "\n" // important to test this case as well
                   "  b"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_0tab, Location(1 + 7, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
                   "    a"     "\n"
                   "   b"     "\n"
                   "   ,"      "\n"
                   "   a"     "\n"
                   "   \t\t\t"        "\n" // important to test this case as well
                   "  \tb"     "\n"
                   "  ,"     "\n"
                   "   a"     "\n"
                   "   ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_1, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_1tab, Location(1 + 8, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_2, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent1_7_2tab, Location(1 + 9, 1 + 2),
                   "k:"     "\n"
                   "  k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_00tab, Location(2 + 5, 1 + 4),
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
                   "    \t]"     "\n" // error here
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_1, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a,"     "\n"
                   "    a"     "\n" // error here
                   "    ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_1tab, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a,"     "\n"
                   "    \ta"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_2tab, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "    \t,"     "\n" // error here
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_3tab, Location(2 + 3, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "     a"     "\n"
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_4tab, Location(2 + 6, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_0tab, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_1tab, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_5_2tab, Location(2 + 9, 1 + 4),
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
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_0tab, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""        "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_1tab, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   ""       "\n" // important to test this case as well
                   ""       "\n" // important to test this case as well
                   "    \tb"     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_6_2tab, Location(2 + 9, 1 + 4),
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
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_0, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"        "\n" // important to test this case as well
                   "    b"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_0tab, Location(2 + 7, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"        "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "    ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_1, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    b"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_1tab, Location(2 + 8, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
                   "      a"     "\n"
                   "     b"     "\n"
                   "     ,"      "\n"
                   "     a"     "\n"
                   "     \t\t\t"       "\n" // important to test this case as well
                   "     \t\t\t"       "\n" // important to test this case as well
                   "    \tb"     "\n"
                   "     ,"     "\n"
                   "     a"     "\n"
                   "     ]"     "\n"
                   )
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_2, Location(2 + 9, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
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
ENGINE_TEST_ERRLOC(MapNestedIndent2_7_2tab, Location(2 + 9, 1 + 4),
                   "k:"     "\n"
                   "  k:"     "\n"
                   "    k: ["     "\n"
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
