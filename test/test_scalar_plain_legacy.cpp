#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(PLAIN_SCALAR)
{
//
ADD_CASE_TO_GROUP("plain scalar, 1 word only",
R"(a_single_word_scalar_to_test)",
  N(VP, "a_single_word_scalar_to_test")
);

ADD_CASE_TO_GROUP("plain scalar, 1 line with spaces",
R"(a scalar with spaces in it all in one line)",
  N(VP, "a scalar with spaces in it all in one line")
);

ADD_CASE_TO_GROUP("plain scalar, multiline",
R"(
a scalar with several lines in it
  of course also with spaces but for now there are no quotes
  and also no blank lines to speak of)",
  N(VP, "a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")
);

ADD_CASE_TO_GROUP("plain scalar, multiline, unindented",
R"(
a scalar with several lines in it
 of course also with spaces but for now there are no quotes
 and also no blank lines to speak of)",
  N(VP, "a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")
);

ADD_CASE_TO_GROUP("plain scalar, multiline, quotes, escapes",
R"(
a scalar with several lines in it and also 'single quotes'
  and "double quotes" and assorted escapes such as \r or \n)",
  N(VP, "a scalar with several lines in it and also 'single quotes' and \"double quotes\" and assorted escapes such as \\r or \\n")
);

ADD_CASE_TO_GROUP("plain scalar, multiline, quotes, escapes, blank lines middle",
R"(
A scalar with several lines in it and also 'single quotes'.
  A blank line follows after this one.
  
  And "double quotes" and assorted escapes such as \r or \n)",
  N(VP, "A scalar with several lines in it and also 'single quotes'. A blank line follows after this one.\nAnd \"double quotes\" and assorted escapes such as \\r or \\n")
);

ADD_CASE_TO_GROUP("plain scalar, multiline, quotes, escapes, blank lines first",
R"(
A scalar with several lines in it and also 'single quotes'.
  
  A blank line precedes this one.
  And "double quotes" and assorted escapes such as \r or \n)",
  N(VP, "A scalar with several lines in it and also 'single quotes'.\nA blank line precedes this one. And \"double quotes\" and assorted escapes such as \\r or \\n")
);

ADD_CASE_TO_GROUP("plain scalar, multiline, quotes, escapes, blank lines last",
R"(
A scalar with several lines in it and also 'single quotes'.
  And "double quotes" and assorted escapes such as \r or \n.
  A blank line follows after this one.
  
  )",
  N(VP, "A scalar with several lines in it and also 'single quotes'. And \"double quotes\" and assorted escapes such as \\r or \\n. A blank line follows after this one.")
);

ADD_CASE_TO_GROUP("plain scalar, example",
R"(
Several lines of text
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.)",
  N(VP, "Several lines of text with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")
);

ADD_CASE_TO_GROUP("plain scalar, map example 1.1",
R"(
example: Several lines of text,
 with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.

  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.

)",
  N(MB, L{
    N(KP|VP, "example", "Several lines of text, with some \"quotes\" of various 'types'. "
                 "Escapes (like \\n) don't do anything.\n"
                 "Newlines can be added by leaving a blank line. "
                 "Additional leading whitespace is ignored."),
    })
);
ADD_CASE_TO_GROUP("plain scalar, map example 1.2",
R"(
another example: Several lines of text,
   
  but the second line is empty, and _indented_.
   There are more lines that follow.

)",
  N(MB, L{
    N(KP|VP, "another example", "Several lines of text,\n"
                         "but the second line is empty, and _indented_. "
                         "There are more lines that follow."),
    })
);
ADD_CASE_TO_GROUP("plain scalar, map example 1.3",
R"(
yet another example: Several lines of text,

  but the second line is empty, and _unindented_.
  There are more lines that follow.
)",
  N(MB, L{
    N(KP|VP, "yet another example", "Several lines of text,\n"
                             "but the second line is empty, and _unindented_. "
                             "There are more lines that follow."),
    })
);
ADD_CASE_TO_GROUP("plain scalar, map example 1.4",
R"(
final example: Several lines of text,


  but the second line is empty, and _unindented_.
  There are more lines that follow. And the last line
  terminates at the end of the file.)",
  N(MB, L{
    N(KP|VP, "final example", "Several lines of text,\n"
                       "\n"
                       "but the second line is empty, and _unindented_. "
                       "There are more lines that follow. "
                       "And the last line terminates at the end of the file."),
    })
);
ADD_CASE_TO_GROUP("plain scalar, map example 1",
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
  N(MB, L{
    N(KP|VP, "example", "Several lines of text, with some \"quotes\" of various 'types'. "
                 "Escapes (like \\n) don't do anything.\n"
                 "Newlines can be added by leaving a blank line. "
                 "Additional leading whitespace is ignored."),
    N(KP|VP, "another example", "Several lines of text,\n"
                         "but the second line is empty, and _indented_. "
                         "There are more lines that follow."),
    N(KP|VP, "yet another example", "Several lines of text,\n"
                             "but the second line is empty, and _unindented_. "
                             "There are more lines that follow."),
    N(KP|VP, "final example", "Several lines of text,\n\n"
                       "but the second line is empty, and _unindented_. "
                       "There are more lines that follow. "
                       "And the last line terminates at the end of the file."),
    })
);

/*
ADD_CASE_TO_GROUP("plain scalar, map example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
example:
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("example", "Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
);
*/

ADD_CASE_TO_GROUP("plain scalar, seq example 1",
R"(
- Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.)",
  N(SB, L{N(VP, "Several lines of text, with some \"quotes\" of various 'types'. "
      "Escapes (like \\n) don't do anything.\n"
      "Newlines can be added by leaving a blank line. "
      "Additional leading whitespace is ignored.")})
);

/*
ADD_CASE_TO_GROUP("plain scalar, seq example 2", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"(
-
  Several lines of text,
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.
)",
  L{N("Several lines of text, with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")}
);
*/

ADD_CASE_TO_GROUP("plain scalar, special characters 1",
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
N(SB, L{N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'. "
      "How about empty lines?\n"
      "Can we also have [] or {} inside? Guess we can. "
      "And how about at the beginning? { - for example } [ - for example ] - - for example ::- for example\n"
      "and now two empty lines -\n\n"
      "and now three empty lines -\n\n\n"
      "and an empty line, unindented -\n"
      "followed by more text "
      "and another four at the end -"
    )})
);

ADD_CASE_TO_GROUP("plain scalar, special characters 3MYT", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(---  # ZWK4
a: 1
? b
&anchor c: 3  # the anchor is for the scalar 'c'
? d
!!str e: 4
? f
---
k:#foo &a !t s
---
"k:#foo &a !t s"
---
'k:#foo &a !t s'

---  # 3MYT
k:#foo
 &a !t s
---
k:#foo
  &a !t s
---
k:#foo
   &a !t s
---
k:#foo
     &a !t s

---  # 3MYT
k:#foo
 !t s
---
k:#foo
  !t s
---
k:#foo
   !t s
---
k:#foo
     !t s
)",
  N(STREAM, L{
      N(DOC|MB, L{
              N(KP|VP, "a", "1"),
              N(KP|VN, "b", {}),
              N(KP|VP, "c", AR(KEYANCH, "anchor"), "3"),
              N(KP|VN, "d", {}),
              N(KP|VP, TS("!!str", "e"), "4"),
              N(KP|VN, "f", {}),
          }),

      N(DOC|VP, "k:#foo &a !t s"),
      N(DOC|VD, "k:#foo &a !t s"),
      N(DOC|VS, "k:#foo &a !t s"),

      N(DOC|VP, "k:#foo &a !t s"),
      N(DOC|VP, "k:#foo &a !t s"),
      N(DOC|VP, "k:#foo &a !t s"),
      N(DOC|VP, "k:#foo &a !t s"),

      N(DOC|VP, "k:#foo !t s"),
      N(DOC|VP, "k:#foo !t s"),
      N(DOC|VP, "k:#foo !t s"),
      N(DOC|VP, "k:#foo !t s"),
  })
    );

// make sure there is no ambiguity with this case
ADD_CASE_TO_GROUP("plain scalar, sequence ambiguity",
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
  N(SB, L{
      N(SB, L{N(VP, "some text"), N(VP, "and this is a sequence")}),
      N(VP, "some text - and this is /not/ a sequence"),
      N(SB, L{N(VP, "some text"), N(VP, "and this is a sequence")}),
      N(VP, "some text - and this is /not/ a sequence"),
  })
);

ADD_CASE_TO_GROUP("plain scalar, empty lines at the beginning",
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
N(SB, L{
      N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
  })
);

ADD_CASE_TO_GROUP("plain scalar, empty continuation lines",
R"(
- the next lines have 2cols, 0cols, 2cols,
  

  
  and this line has some text in it. -> 0

  now 0, 0, 2, 2, 0, 1, 1, 0, 4, 4, 0, 0


  
  

 
 

    
    


  and finally some more text
)",
N(SB, L{
      N(VP, "the next lines have 2cols, 0cols, 2cols,"
        "\n\n\n"
        "and this line has some text in it. -> 0"
        "\n"
        "now 0, 0, 2, 2, 0, 1, 1, 0, 4, 4, 0, 0"
        "\n\n\n\n\n\n\n\n\n\n\n\n"
        "and finally some more text"),
  })
);


ADD_CASE_TO_GROUP("plain scalar, indented first line",
R"(
- Several lines of text,
 
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
N(SB, L{
      N(VP, "Several lines of text,\nwith special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
      N(VP, "Several lines of text, with special:characters, like:this-or-this - - and some \"quotes\" of various 'types'."),
  })
);

ADD_CASE_TO_GROUP("plain scalar, do not accept ': ' mid line", EXPECT_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this: must cause a parse error.
)",
  Location(4, 11)
);

ADD_CASE_TO_GROUP("plain scalar, do not accept ': ' start line", EXPECT_PARSE_ERROR,
R"(
- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error -
  : foo bar
)",
  Location(6, 3)
);

ADD_CASE_TO_GROUP("plain scalar, do not accept ': ' at line end", EXPECT_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error: 
)",
  Location(4, 36)
);

ADD_CASE_TO_GROUP("plain scalar, do not accept ':' at line end", EXPECT_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error:
  - well, did it?
)",
  Location(4, 36)
);

ADD_CASE_TO_GROUP("plain scalar, accept ' #' at line start",
R"(- Several lines of text,
  and this is valid -
  #with special:characters, like:this-or-this -
)",
N(SB, L{N(VP, "Several lines of text, and this is valid -"),})
);

ADD_CASE_TO_GROUP("plain scalar, accept ' #' on first line",
R"(- Several lines of text, and this is valid -
  #with special:characters, like:this-or-this -
)",
  N(SB, L{N(VP, "Several lines of text, and this is valid -")})
);

ADD_CASE_TO_GROUP("plain scalar, accept ' #' at line end",
R"(- Several lines of text,
  with special:characters, #comment at the end
)",
  N(SB, L{N(VP,"Several lines of text, with special:characters,")})
);

ADD_CASE_TO_GROUP("plain scalar, accept '#'",
R"(
- Several lines of text, # with a comment
- Several lines of text,
  with special#characters, like#this_#_-or-#-:this -
  - and some "quotes" of various 'types'.
)",
  N(SB, L{
      N(VP, "Several lines of text,"),
      N(VP, "Several lines of text, "
        "with special#characters, like#this_#_-or-#-:this - "
        "- and some \"quotes\" of various 'types'."),
   })
);

ADD_CASE_TO_GROUP("plain scalar, flow",
R"(
[
  a plain scalar
    with several lines

    and blank lines
    
    as well
  ,
  and another plain scalar
  ,
  and yet another one



with many lines

and yet more, deindented
]
)",
  N(SFM, L{
      N(VP, "a plain scalar with several lines\nand blank lines\nas well"),
      N(VP, "and another plain scalar"),
      N(VP, "and yet another one\n\n\nwith many lines\nand yet more"),
      N(VP, "deindented"),
   })
);

ADD_CASE_TO_GROUP("plain scalar, flow, early end, seq", EXPECT_PARSE_ERROR,
R"([
  a plain scalar
    with several lines
)",
  Location(4, 1)
);

ADD_CASE_TO_GROUP("plain scalar, flow, early end, map", EXPECT_PARSE_ERROR,
R"({foo:
  a plain scalar
    with several lines
)",
  Location(4, 1)
);

ADD_CASE_TO_GROUP("plain scalar, multiple docs",
R"(---
- a plain scalar
    with several lines
---
- a second plain scalar
    with several lines
)",
  N(STREAM, L{
    N(DOC|SB, L{N(VP, "a plain scalar with several lines")}),
    N(DOC|SB, L{N(VP, "a second plain scalar with several lines")}),
  })
);

ADD_CASE_TO_GROUP("plain scalar, multiple docs, termination",
R"(---
- a plain scalar
    with several lines
...
---
- a second plain scalar
    with several lines
)",
  N(STREAM, L{
    N(DOC|SB, L{N(VP, "a plain scalar with several lines")}),
    N(DOC|SB, L{N(VP, "a second plain scalar with several lines")}),
  })
);

ADD_CASE_TO_GROUP("plain scalar, trailing whitespace",
  R"(---
foo  
---
foo  

---
foo  



)",
  N(STREAM, L{
          N(DOC|VP, "foo"),
          N(DOC|VP, "foo"),
          N(DOC|VP, "foo"),
      })
    );

ADD_CASE_TO_GROUP("plain scalar, issue 422",
R"(hello world, whats up?
it's a me, mario,
bla,
asd
)",
N(VP, "hello world, whats up? it's a me, mario, bla, asd")
);

ADD_CASE_TO_GROUP("plain scalar, issue 422, 2",
R"(hello world, whats up?
  it's a me, mario,
  bla,
  asd
)",
N(VP, "hello world, whats up? it's a me, mario, bla, asd")
);


ADD_CASE_TO_GROUP("plain scalar, mixed, doc, 1",
R"(. "dquo" plain 'squo')",
N(VP, ". \"dquo\" plain 'squo'")
);

ADD_CASE_TO_GROUP("plain scalar, mixed, doc, 2",
R"(. 'squo' "dquo" plain)",
N(VP, ". 'squo' \"dquo\" plain")
);

ADD_CASE_TO_GROUP("plain scalar, mixed, doc, 3",
R"(. plain 'squo' "dquo")",
N(VP, ". plain 'squo' \"dquo\"")
);


ADD_CASE_TO_GROUP("plain scalar, mixed, map, 1",
R"(foo: . "dquo" plain 'squo')",
N(MB, L{N(KP|VP, "foo", ". \"dquo\" plain 'squo'")})
);

ADD_CASE_TO_GROUP("plain scalar, mixed, map, 2",
R"(foo: . 'squo' "dquo" plain)",
N(MB, L{N(KP|VP, "foo", ". 'squo' \"dquo\" plain")})
);

ADD_CASE_TO_GROUP("plain scalar, mixed, map, 3",
R"(foo: . plain 'squo' "dquo")",
N(MB, L{N(KP|VP, "foo", ". plain 'squo' \"dquo\"")})
);


ADD_CASE_TO_GROUP("plain scalar, yamlscript, 1",
R"(foo:: 1 ... 2 ... 3
bar:: 5 .. 10
)",
N(MB, L{
  N(KP|VP, "foo:", "1 ... 2 ... 3"),
  N(KP|VP, "bar:", "5 .. 10"),
})
);


ADD_CASE_TO_GROUP("plain scalar trailing column 1, flow, seq", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[plain:]",
  N(SFS, L{
      N(MFS, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);

ADD_CASE_TO_GROUP("plain scalar trailing column 1, flow, seqimap", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[{plain:}]",
  N(SFS, L{
      N(MFS, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);


ADD_CASE_TO_GROUP("plain scalar trailing column 2, flow, seq", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[plain: ]",
  N(SFS, L{
      N(MFS, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);

ADD_CASE_TO_GROUP("plain scalar trailing column 2, flow, seqimap", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[{plain: }]",
  N(SFS, L{
      N(MFS, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);


ADD_CASE_TO_GROUP("plain scalar trailing column 3, flow, seq", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[plain:\r\n]",
  N(SFM, L{
      N(MFM, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);

ADD_CASE_TO_GROUP("plain scalar trailing column 3, flow, seqimap", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[{plain:\r\n}]",
  N(SFM, L{
      N(MFM, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);


ADD_CASE_TO_GROUP("plain scalar trailing column 4, flow, seq", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[plain:\n]",
  N(SFM, L{
      N(MFM, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);

ADD_CASE_TO_GROUP("plain scalar trailing column 4, flow, seqimap", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
"[{plain:\n}]",
  N(SFM, L{
      N(MFM, L{N(KP|VP|VALNIL, "plain", "")}),
   })
);


ADD_CASE_TO_GROUP("plain scalar trailing column 5, flow, seq", EXPECT_PARSE_ERROR,
"[plain:",
  Location(1, 7)
);

ADD_CASE_TO_GROUP("plain scalar trailing column 5, flow, seqimap", EXPECT_PARSE_ERROR,
"[{plain:",
  Location(1, 9)
);

} // CASE_GROUP(PLAIN_SCALAR)

} // namespace yml
} // namespace c4
