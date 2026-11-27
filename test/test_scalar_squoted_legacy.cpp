#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(SINGLE_QUOTED)
{

ADD_CASE_TO_GROUP("squoted, only text",
R"('Some text without any quotes.'
)",
  N(VS, "Some text without any quotes.")
);

ADD_CASE_TO_GROUP("squoted, with double quotes",
R"('Some "text" "with double quotes"')",
  N(VS, "Some \"text\" \"with double quotes\"")
);

ADD_CASE_TO_GROUP("squoted, with single quotes",
R"('Some text ''with single quotes''')",
  N(VS, "Some text 'with single quotes'")
);

ADD_CASE_TO_GROUP("squoted, with single and double quotes",
R"('Some text ''with single quotes'' "and double quotes".')",
  N(VS, "Some text 'with single quotes' \"and double quotes\".")
);

ADD_CASE_TO_GROUP("squoted, with escapes",
R"('Some text with escapes \n \r \t')",
  N(VS, "Some text with escapes \\n \\r \\t")
);

ADD_CASE_TO_GROUP("squoted, all",
R"('Several lines of text,
containing ''single quotes'' and "double quotes". Escapes (like \n) don''t do anything.

Newlines can be added by leaving a blank line.
            Leading whitespace on lines is ignored.'
)",
  N(VS, "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Leading whitespace on lines is ignored.")
);

ADD_CASE_TO_GROUP("squoted, empty", NO_COMPARE_EMITTED_INTS,//FIXME
R"('')",
  N(VS, "")
);

ADD_CASE_TO_GROUP("squoted, blank",
R"(
- ''
- ' '
- '  '
- '   '
- '    '
)",
N(SB, L{N(VS, ""), N(VS, " "), N(VS, "  "), N(VS, "   "), N(VS, "    ")})
);

ADD_CASE_TO_GROUP("squoted, numbers", // these should not be quoted when emitting
R"(
- -1
- -1.0
- +1.0
- 1e-2
- 1e+2
)",
N(SB, L{N(VP,"-1"), N(VP,"-1.0"), N(VP,"+1.0"), N(VP,"1e-2"), N(VP,"1e+2")})
);

ADD_CASE_TO_GROUP("squoted, trailing space",
R"('a aaaa  ')",
  N(VS, "a aaaa  ")
);

ADD_CASE_TO_GROUP("squoted, leading space",
R"('  a aaaa')",
  N(VS, "  a aaaa")
);

ADD_CASE_TO_GROUP("squoted, trailing and leading space",
R"('  012345  ')",
  N(VS, "  012345  ")
);

ADD_CASE_TO_GROUP("squoted, 1 squote",
R"('''')",
  N(VS, "'")
);

ADD_CASE_TO_GROUP("squoted, 2 squotes",
R"('''''')",
  N(VS, "''")
);

ADD_CASE_TO_GROUP("squoted, 3 squotes",
R"('''''''')",
  N(VS, "'''")
);

ADD_CASE_TO_GROUP("squoted, 4 squotes",
R"('''''''''')",
  N(VS, "''''")
);

ADD_CASE_TO_GROUP("squoted, 5 squotes",
R"('''''''''''')",
  N(VS, "'''''")
);

ADD_CASE_TO_GROUP("squoted, example 2",
R"('This is a scalar

that has multiple lines

'
)",
N(VS, "This is a scalar\nthat has multiple lines\n")
);

ADD_CASE_TO_GROUP("squoted indentation, 0",
R"(' 1st non-empty

 2nd non-empty
	3rd non-empty '
)",
N(VS, " 1st non-empty\n2nd non-empty 3rd non-empty ")
);


ADD_CASE_TO_GROUP("squoted indentation, 1", EXPECT_PARSE_ERROR,
R"(- ' 1st non-empty

 2nd non-empty
	3rd non-empty '
)",
Location(4,1)
);


ADD_CASE_TO_GROUP("squoted indentation, 2",
R"(- ' 1st non-empty

 2nd non-empty
 	3rd non-empty '
)",
N(SB, L{N(VS, " 1st non-empty\n2nd non-empty 3rd non-empty ")})
);

ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0", EXPECT_PARSE_ERROR,
R"(
- - 'bar
bar'
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 1", EXPECT_PARSE_ERROR,
R"(
- - 'bar
 bar'
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 2", EXPECT_PARSE_ERROR,
R"(
- - 'bar
  bar'
)",
  Location(3, 1)
);

ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 4",
R"(
- - 'bar
   bar'
)",
  N(SB, L{N(SB, L{N(VS, "bar bar")})})
);

ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, empty", EXPECT_PARSE_ERROR,
R"(
- - 'bar

bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces lt indent", EXPECT_PARSE_ERROR,
R"(
- - 'bar
  
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces eq indent", EXPECT_PARSE_ERROR,
R"(
- - 'bar
   
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces gt indent", EXPECT_PARSE_ERROR,
R"(
- - 'bar
    
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces gt indent + tabs", EXPECT_PARSE_ERROR,
R"(
- - 'bar
      	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces eq indent + tabs 1", EXPECT_PARSE_ERROR,
R"(
- - 'bar
    	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces eq indent + tabs 2", EXPECT_PARSE_ERROR,
R"(
- - 'bar
   	
bar"
)",
  Location(4, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces lt indent + tabs 1", EXPECT_PARSE_ERROR,
R"(
- - 'bar
  	
bar'
)",
  Location(3, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces lt indent + tabs 2", EXPECT_PARSE_ERROR,
R"(
- - 'bar
 	
bar'
)",
  Location(3, 1)
);
ADD_CASE_TO_GROUP("squoted, invalid indentation JKF3, 0, spaces lt indent + tabs 3", EXPECT_PARSE_ERROR,
R"(
- - 'bar
	
bar'
)",
  Location(3, 1)
);

} // CASE_GROUP()


} // namespace yml
} // namespace c4
