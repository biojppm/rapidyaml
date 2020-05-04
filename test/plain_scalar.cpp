#include "./test_group.hpp"

namespace c4 {
namespace yml {
#define PLAIN_SCALAR_CASES                                              \
    "plain scalar, 1 word only",                                        \
        "plain scalar, 1 line with spaces",                             \
        "plain scalar, multiline",                                      \
        "plain scalar, multiline, unindented",                          \
        "plain scalar, multiline, quotes, escapes",                     \
        "plain scalar, multiline, quotes, escapes, blank lines middle", \
        "plain scalar, multiline, quotes, escapes, blank lines first",  \
        "plain scalar, multiline, quotes, escapes, blank lines last",   \
        "plain scalar, example",                                        \
        "plain scalar, map example 1"/*,                                \
        "plain scalar, map example 2"*/,                                \
        "plain scalar, seq example 1"/*,                                \
        "plain scalar, seq example 2"*/,                                \
        "plain scalar, special characters 1",                           \
        "plain scalar, sequence ambiguity",                             \
        "plain scalar, empty lines at the beginning",                   \
        "plain scalar, empty continuation lines"


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

C("plain scalar, multiline, unindented",
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
      Additional leading whitespace is ignored.)",
  L{N("Several lines of text with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
),

C("plain scalar, map example 1",
R"(
example: Several lines of text,
 with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.

  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.

another example: Several lines of text,
   
  but the second line is empty, and _indented_.
   There are more lines that follow.

yet another example: Several lines of text,

  but the second line is empty, and _unindented_.
  There are more lines that follow.
final example: Several lines of text,


  but the second line is empty, and _unindented_.
  There are more lines that follow. And the last line
  terminates at the end of the file.)",
  L{
    N("example", "Several lines of text, with some \"quotes\" of various 'types'. "
                 "Escapes (like \\n) don't do anything.\n"
                 "Newlines can be added by leaving a blank line. "
                 "Additional leading whitespace is ignored."),
    N("another example", "Several lines of text,\n"
                         "but the second line is empty, and _indented_. "
                         "There are more lines that follow."),
    N("yet another example", "Several lines of text,\n"
                             "but the second line is empty, and _unindented_. "
                             "There are more lines that follow."),
    N("final example", "Several lines of text,\n\n"
                       "but the second line is empty, and _unindented_. "
                       "There are more lines that follow. "
                       "And the last line terminates at the end of the file."),
    }
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
      Additional leading whitespace is ignored.)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. "
      "Escapes (like \\n) don't do anything.\n"
      "Newlines can be added by leaving a blank line. "
      "Additional leading whitespace is ignored.")}
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

C("plain scalar, special characters 1",
R"(
- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  How about empty lines?
  
  Can we also have [] or {} inside?
  Guess we can.
  And how about at the beginning?
  { - for example }
  [ - for example ]
  - - for example
  ::- for example
  
  and now two empty lines -
  
  
  and now three empty lines -
  
  
  
  and an empty line, unindented -
  
  followed by more text
  and another four at the end -
  
  
  
  
)",
  L{N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'. "
      "How about empty lines?\n"
      "Can we also have [] or {} inside? Guess we can. "
      "And how about at the beginning? { - for example } [ - for example ] - - for example ::- for example\n"
      "and now two empty lines -\n\n"
      "and now three empty lines -\n\n\n"
      "and an empty line, unindented -\n"
      "followed by more text "
      "and another four at the end -\n\n\n\n"
    )}
),

// make sure there is no ambiguity with this case
C("plain scalar, sequence ambiguity",
R"(
-         - some text
          - and this is a sequence
-         some text
          - and this is /not/ a sequence
- - some text
  - and this is a sequence
- some text
  - and this is /not/ a sequence
)",
  L{
      N(L{N("some text"), N("and this is a sequence")}),
      N("some text - and this is /not/ a sequence"),
      N(L{N("some text"), N("and this is a sequence")}),
      N("some text - and this is /not/ a sequence"),
  }
),

C("plain scalar, empty lines at the beginning",
R"(
- 


  Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
- 

  Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
-
  Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
)",
  L{
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
  }
),

C("plain scalar, empty continuation lines",
R"(
- the next lines have 2cols, 0cols, 2cols,
  

  
  and this line has some text in it. -> 0

  now 0, 0, 2, 2, 0, 1, 1, 0, 4, 4, 0, 0


  
  

 
 

    
    


  and finally some more text
 # deindented comments at the end
)",
  L{
      N("the next lines have 2cols, 0cols, 2cols,"
        "\n\n\n"
        "and this line has some text in it. -> 0"
        "\n"
        "now 0, 0, 2, 2, 0, 1, 1, 0, 4, 4, 0, 0"
        "\n\n\n\n\n\n\n\n\n\n\n\n"
        "and finally some more text"),
  }
),


C("plain scalar, indented first line",
R"(
- Several lines of text, empty next -
 
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
- 

  Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
-
  Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
)",
  L{
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N("Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
  }
)
    )

}

INSTANTIATE_GROUP(PLAIN_SCALAR)

} // namespace yml
} // namespace c4
