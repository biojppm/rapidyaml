#include "./test_group.hpp"

namespace c4 {
namespace yml {
#define PLAIN_SCALAR_CASES                                              \
    "plain scalar, 1 word only",                                        \
        "plain scalar, 1 line with spaces",                             \
        "plain scalar, multiline",                                      \
        "plain scalar, multiline, quotes, escapes",                     \
        "plain scalar, multiline, quotes, escapes, blank lines middle", \
        "plain scalar, multiline, quotes, escapes, blank lines first",  \
        "plain scalar, multiline, quotes, escapes, blank lines last",   \
        "plain scalar, example",                                        \
        "plain scalar, map example 1"/*,                                  \
        "plain scalar, map example 2"*/,                                  \
        "plain scalar, seq example 1"/*,                                  \
        "plain scalar, seq example 2"*/


CASE_GROUP(PLAIN_SCALAR)
{
    APPEND_CASES(

C("plain scalar, 1 word only",
R"(a_single_word_scalar_to_test)",
  L{N("a_single_word_scalar_to_test")}
),

C("plain scalar, 1 line with spaces",
R"(a scalar with spaces in it all in one line)",
  L{N("a scalar with spaces in it all in one line")}
),

C("plain scalar, multiline",
R"(
a scalar with several lines in it
  of course also with spaces but for now there are no quotes
  and also no blank lines to speak of)",
  L{N("a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")}
),

C("plain scalar, multiline, quotes, escapes",
R"(
a scalar with several lines in it and also 'single quotes'
  and "double quotes" and assorted escapes such as \r or \n)",
  L{N("a scalar with several lines in it and also 'single quotes' and \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines middle",
R"(
A scalar with several lines in it and also 'single quotes'.
  A blank line follows after this one.
  
  And "double quotes" and assorted escapes such as \r or \n)",
  L{N("A scalar with several lines in it and also 'single quotes'. A blank line follows after this one.\nAnd \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines first",
R"(
A scalar with several lines in it and also 'single quotes'.
  
  A blank line precedes this one.
  And "double quotes" and assorted escapes such as \r or \n)",
  L{N("A scalar with several lines in it and also 'single quotes'.\nA blank line precedes this one. And \"double quotes\" and assorted escapes such as \\r or \\n")}
),

C("plain scalar, multiline, quotes, escapes, blank lines last",
R"(
A scalar with several lines in it and also 'single quotes'.
  And "double quotes" and assorted escapes such as \r or \n.
  A blank line follows after this one.
  
  )",
  L{N("A scalar with several lines in it and also 'single quotes'. And \"double quotes\" and assorted escapes such as \\r or \\n. A blank line follows after this one.\n")}
),

C("plain scalar, example",
R"(
Several lines of text
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

C("plain scalar, map example 1",
R"(
example: Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("example", "Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

/*
C("plain scalar, map example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
example:
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("example", "Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),
*/

C("plain scalar, seq example 1",
R"(
- Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

/*
C("plain scalar, seq example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
-
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),
*/
    )
}

INSTANTIATE_GROUP(PLAIN_SCALAR);

} // namespace yml
} // namespace c4
