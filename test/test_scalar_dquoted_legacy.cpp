#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(DOUBLE_QUOTED)
{

ADD_CASE_TO_GROUP("dquoted, only text",
R"("Some text without any quotes."
)",
  N(VD, "Some text without any quotes.")
);

ADD_CASE_TO_GROUP("dquoted, with single quotes",
R"("Some text 'with single quotes'")",
  N(VD, "Some text 'with single quotes'")
);

ADD_CASE_TO_GROUP("dquoted, with double quotes",
R"("Some \"text\" \"with double quotes\"")",
  N(VD, "Some \"text\" \"with double quotes\"")
);

ADD_CASE_TO_GROUP("dquoted, with single and double quotes",
R"("Some text 'with single quotes' \"and double quotes\".")",
  N(VD, "Some text 'with single quotes' \"and double quotes\".")
);

ADD_CASE_TO_GROUP("dquoted, with escapes",
R"("Some text with escapes \\n \\r \\t")",
  N(VD, "Some text with escapes \\n \\r \\t")
);

ADD_CASE_TO_GROUP("dquoted, with newline",
R"("Some text with\nnewline")",
  N(VD, "Some text with\nnewline")
);

ADD_CASE_TO_GROUP("dquoted, with tabs",
R"("\tSome\ttext\twith\ttabs\t")",
  N(VD, "\tSome\ttext\twith\ttabs\t")
);

ADD_CASE_TO_GROUP("dquoted, with tabs 4ZYM",
R"(plain: text
  lines
quoted: "text
  	lines"
block: |
  text
   	lines
)",
N(MB, L{
  N(KP|VP, "plain", "text lines"),
  N(KP|VD, "quoted", "text lines"),
  N(KP|VL, "block", "text\n \tlines\n")
})
);

ADD_CASE_TO_GROUP("dquoted, with tabs 7A4E",
R"(" 1st non-empty

 2nd non-empty 
	3rd non-empty ")",
  N(VD, " 1st non-empty\n2nd non-empty 3rd non-empty ")
);

ADD_CASE_TO_GROUP("dquoted, with tabs TL85",
R"("
  foo 
 
  	 bar

  baz
")", N(VD, " foo\nbar\nbaz "));

ADD_CASE_TO_GROUP("dquoted, all",
R"("Several lines of text,
containing 'single quotes' and \"double quotes\". \
Escapes (like \\n) work.\nIn addition,
newlines can be esc\
aped to prevent them from being converted to a space.

Newlines can also be added by leaving a blank line.
    Leading whitespace on lines is ignored."
)",
  N(VD, "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) work.\nIn addition, newlines can be escaped to prevent them from being converted to a space.\nNewlines can also be added by leaving a blank line. Leading whitespace on lines is ignored.")
);

ADD_CASE_TO_GROUP("dquoted, empty", NO_COMPARE_EMITTED_INTS,//FIXME
R"("")",
  N(VD, "")
);

ADD_CASE_TO_GROUP("dquoted, blank",
R"(
- ""
- " "
- "  "
- "   "
- "    "
)",
N(SB, L{N(VD, ""), N(VD, " "), N(VD, "  "), N(VD, "   "), N(VD, "    ")})
);

ADD_CASE_TO_GROUP("dquoted, numbers", // these should not be quoted when emitting
R"(
- -1
- -1.0
- +1.0
- 1e-2
- 1e+2
)",
N(SB, L{N(VP, "-1"), N(VP, "-1.0"), N(VP, "+1.0"), N(VP, "1e-2"), N(VP, "1e+2")})
);

ADD_CASE_TO_GROUP("dquoted, trailing space",
R"("a aaaa  ")",
  N(VD, "a aaaa  ")
);

ADD_CASE_TO_GROUP("dquoted, leading space",
R"("  a aaaa")",
  N(VD, "  a aaaa")
);

ADD_CASE_TO_GROUP("dquoted, trailing and leading space",
R"("  012345  ")",
  N(VD, "  012345  ")
);

ADD_CASE_TO_GROUP("dquoted, 1 dquote",
R"("\"")",
  N(VD, "\"")
);

ADD_CASE_TO_GROUP("dquoted, 2 dquotes",
R"("\"\"")",
  N(VD, "\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 3 dquotes",
R"("\"\"\"")",
  N(VD, "\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 4 dquotes",
R"("\"\"\"\"")",
  N(VD, "\"\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 5 dquotes",
R"("\"\"\"\"\"")",
  N(VD, "\"\"\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, example 2",
R"("This is a key\nthat has multiple lines\n": and this is its value
)",
N(MB, L{N(KD|VP, "This is a key\nthat has multiple lines\n", "and this is its value")})
);

ADD_CASE_TO_GROUP("dquoted, example 2.1",
R"("This is a key

that has multiple lines

"
)",
N(VD, "This is a key\nthat has multiple lines\n")
);

ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0", EXPECT_PARSE_ERROR,
R"(
- - "bar
bar"
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 1", EXPECT_PARSE_ERROR,
R"(
- - "bar
 bar"
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 2", EXPECT_PARSE_ERROR,
R"(
- - "bar
  bar"
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 4",
R"(
- - "bar
   bar"
)",
  N(SB, L{N(SB, L{N(VD, "bar bar")})})
);

ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, empty", EXPECT_PARSE_ERROR,
R"(
- - "bar

bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces lt indent", EXPECT_PARSE_ERROR,
R"(
- - "bar
  
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces eq indent", EXPECT_PARSE_ERROR,
R"(
- - "bar
   
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces gt indent", EXPECT_PARSE_ERROR,
R"(
- - "bar
    
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces gt indent + tabs", EXPECT_PARSE_ERROR,
R"(
- - "bar
      	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces eq indent + tabs 1", EXPECT_PARSE_ERROR,
R"(
- - "bar
    	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces eq indent + tabs 2", EXPECT_PARSE_ERROR,
R"(
- - "bar
   	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces lt indent + tabs 1", EXPECT_PARSE_ERROR,
R"(
- - "bar
  	
bar"
)",
  Location(3, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces lt indent + tabs 2", EXPECT_PARSE_ERROR,
R"(
- - "bar
 	
bar"
)",
  Location(3, 1)
);
ADD_CASE_TO_GROUP("dquoted, invalid indentation JKF3, 0, spaces lt indent + tabs 3", EXPECT_PARSE_ERROR,
R"(
- - "bar
	
bar"
)",
  Location(3, 1)
);

} // CASE_GROUP(DOUBLE_QUOTED)

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP
