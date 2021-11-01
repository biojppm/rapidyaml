#include "./test_group.hpp"

namespace c4 {
namespace yml {


TEST(double_quoted, test_suite_KSS4)
{
    csubstr yaml = R"(
---
'quoted
string'
--- 'quoted
string'
---
- 'quoted
  string'
---
- 'quoted
string'
---
'quoted
  string': 'quoted
  string'
---
'quoted
string': 'quoted
string'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0).val(), "quoted string");
        EXPECT_EQ(t.docref(1).val(), "quoted string");
        EXPECT_EQ(t.docref(2)[0].val(), "quoted string");
        EXPECT_EQ(t.docref(3)[0].val(), "quoted string");
        EXPECT_EQ(t.docref(4)["quoted string"].val(), "quoted string");
        EXPECT_EQ(t.docref(5)["quoted string"].val(), "quoted string");
    });
}


TEST(single_quoted, test_suite_R4YG)
{
    csubstr yaml = R"(
- '	

detected

'

)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("\t\ndetected\n"));
    });
}


TEST(single_quoted, test_suite_PRH3)
{
    csubstr yaml = R"(
- ' 1st non-empty

 2nd non-empty 
	3rd non-empty '
- ' 1st non-empty

 2nd non-empty 	
 	3rd non-empty '
- ' 1st non-empty

 2nd non-empty	 
	3rd non-empty '
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
        EXPECT_EQ(t[1].val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
        EXPECT_EQ(t[2].val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
    });
}


TEST(single_quoted, test_suite_T4YY)
{
    csubstr yaml = R"(
---
' 1st non-empty

 2nd non-empty 
 3rd non-empty '
---
'	

detected

'

)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define SINGLE_QUOTED_CASES                             \
            "squoted, only text",                       \
            "squoted, with double quotes",              \
            "squoted, with single quotes",              \
            "squoted, with single and double quotes",   \
            "squoted, with escapes",                    \
            "squoted, all",                             \
            "squoted, empty",                           \
            "squoted, blank",                           \
            "squoted, numbers",                         \
            "squoted, trailing space",                  \
            "squoted, leading space",                   \
            "squoted, trailing and leading space",      \
            "squoted, 1 squote",                        \
            "squoted, 2 squotes",                       \
            "squoted, 3 squotes",                       \
            "squoted, 4 squotes",                       \
            "squoted, 5 squotes"

CASE_GROUP(SINGLE_QUOTED)
{
    APPEND_CASES(

C("squoted, only text",
R"('Some text without any quotes.'
)",
  N(DOCVAL | VALQUO, "Some text without any quotes.")
),

C("squoted, with double quotes",
R"('Some "text" "with double quotes"')",
  N(DOCVAL | VALQUO, "Some \"text\" \"with double quotes\"")
),

C("squoted, with single quotes",
R"('Some text ''with single quotes''')",
  N(DOCVAL | VALQUO, "Some text 'with single quotes'")
),

C("squoted, with single and double quotes",
R"('Some text ''with single quotes'' "and double quotes".')",
  N(DOCVAL | VALQUO, "Some text 'with single quotes' \"and double quotes\".")
),

C("squoted, with escapes",
R"('Some text with escapes \n \r \t')",
  N(DOCVAL | VALQUO, "Some text with escapes \\n \\r \\t")
),

C("squoted, all",
R"('Several lines of text,
containing ''single quotes'' and "double quotes". Escapes (like \n) don''t do anything.

Newlines can be added by leaving a blank line.
            Leading whitespace on lines is ignored.'
)",
  N(DOCVAL | VALQUO, "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Leading whitespace on lines is ignored.")
),

C("squoted, empty",
R"('')",
  N(DOCVAL | VALQUO, "")
),

C("squoted, blank",
R"(
- ''
- ' '
- '  '
- '   '
- '    '
)",
  L{N(QV, ""), N(QV, " "), N(QV, "  "), N(QV, "   "), N(QV, "    ")}
),

C("squoted, numbers", // these should not be quoted when emitting
R"(
- -1
- -1.0
- +1.0
- 1e-2
- 1e+2
)",
  L{N("-1"), N("-1.0"), N("+1.0"), N("1e-2"), N("1e+2")}
),

C("squoted, trailing space",
R"('a aaaa  ')",
  N(DOCVAL | VALQUO, "a aaaa  ")
),

C("squoted, leading space",
R"('  a aaaa')",
  N(DOCVAL | VALQUO, "  a aaaa")
),

C("squoted, trailing and leading space",
R"('  012345  ')",
  N(DOCVAL | VALQUO, "  012345  ")
),

C("squoted, 1 squote",
R"('''')",
  N(DOCVAL | VALQUO, "'")
),

C("squoted, 2 squotes",
R"('''''')",
  N(DOCVAL | VALQUO, "''")
),

C("squoted, 3 squotes",
R"('''''''')",
  N(DOCVAL | VALQUO, "'''")
),

C("squoted, 4 squotes",
R"('''''''''')",
  N(DOCVAL | VALQUO, "''''")
),

C("squoted, 5 squotes",
R"('''''''''''')",
  N(DOCVAL | VALQUO, "'''''")
),

/*
C("squoted, example 2",
R"('This is a key

that has multiple lines

': and this is its value
)",
  L{N("This is a key\nthat has multiple lines\n", "and this is its value")}
),
*/
    )
}

INSTANTIATE_GROUP(SINGLE_QUOTED)

} // namespace yml
} // namespace c4
