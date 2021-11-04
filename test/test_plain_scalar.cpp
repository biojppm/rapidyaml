#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(plain_scalar, issue153_seq)
{
    Tree t = parse("- A\n \n");
    EXPECT_EQ(t[0].val(), "A");
}

TEST(plain_scalar, issue153_map)
{
    Tree t = parse("foo: A\n \n");
    EXPECT_EQ(t["foo"].val(), "A");
}


TEST(plain_scalar, test_suite_7TMG)
{
    csubstr yaml = R"(---
word1
# comment
---
# first value is NOT a multiline plain scalar
[ word1
# comment
, word2]
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_stream());
        NodeRef doc = t.rootref().first_child();
        ASSERT_TRUE(doc.is_doc());
        ASSERT_TRUE(doc.is_val());
        EXPECT_EQ(doc.val(), "word1");
        doc = t.rootref().child(1);
        ASSERT_TRUE(doc.is_doc());
        ASSERT_TRUE(doc.is_seq());
        EXPECT_EQ(doc[0].val(), "word1");
        EXPECT_EQ(doc[1].val(), "word2");
    });
}


TEST(plain_scalar, test_suite_82AN)
{
    csubstr yaml = R"(
---word1
word2
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_doc());
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("---word1 word2"));
    });
}

TEST(plain_scalar, test_suite_EXG3)
{
    csubstr yaml = R"(
---
---word1
word2
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        ASSERT_TRUE(t.rootref().first_child().is_val());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("---word1 word2"));
    });
}


TEST(plain_scalar, test_suite_9YRD)
{
    csubstr yaml = R"(
a
b  
  c
d

e
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_doc());
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("a b c d\ne"));
    });
}

TEST(plain_scalar, test_suite_EX5H)
{
    csubstr yaml = R"(
---
a
b  
  c
d

e
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().child(0).is_doc());
        ASSERT_TRUE(t.rootref().child(0).is_val());
        EXPECT_EQ(t.rootref().child(0).val(), csubstr("a b c d\ne"));
    });
}


TEST(plain_scalar, test_suite_M7A3)
{
    csubstr yaml = R"(
Bare
document
...
# No document
...
|
%!PS-Adobe-2.0 # Not the first line
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_EQ(t.rootref().num_children(), 2u);
        EXPECT_EQ(t.rootref().child(0).val(), csubstr("Bare document"));
        EXPECT_EQ(t.rootref().child(1).val(), csubstr("%!PS-Adobe-2.0 # Not the first line\n"));
    });
}


TEST(plain_scalar, test_suite_HS5T)
{
    csubstr yaml = R"(
1st non-empty

 2nd non-empty 
   	3rd non-empty
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_doc());
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("1st non-empty\n2nd non-empty 3rd non-empty"));
    });
}

TEST(plain_scalar, test_suite_NB6Z)
{
    csubstr yaml = R"(
key:
  value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
   	
key1:
  value
  with

  tabs
  tabs

    foo

      bar
        baz

key2: something
      else
key3: something
  else
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("key"));
        ASSERT_TRUE(t.rootref().has_child("key1"));
        ASSERT_TRUE(t.rootref().has_child("key2"));
        ASSERT_TRUE(t.rootref().has_child("key3"));
        EXPECT_EQ(t["key"].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t["key1"].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t["key2"].val(), csubstr("something else"));
        EXPECT_EQ(t["key3"].val(), csubstr("something else"));
    });
}

TEST(plain_scalar, test_suite_NB6Z_seq)
{
    csubstr yaml = R"(
- value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
   	
- value
  with

  tabs
  tabs

    foo

      bar
        baz

- more
  value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 3u);
        EXPECT_EQ(t[0].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t[1].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t[2].val(), csubstr("more value"));
    });
}

TEST(plain_scalar, test_suite_NB6Z_docval)
{
    csubstr yaml = R"(
value
with
 	
tabs
tabs
 	
  foo
 	
    bar
      baz
 	
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_doc());
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define PLAIN_SCALAR_CASES                                          \
    "plain scalar, 1 word only",                                    \
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
    "plain scalar, special characters 3MYT",                        \
    "plain scalar, sequence ambiguity",                             \
    "plain scalar, empty lines at the beginning",                   \
    "plain scalar, empty continuation lines",                       \
    "plain scalar, do not accept ': ' mid line",                    \
    "plain scalar, do not accept ': ' start line",                  \
    "plain scalar, do not accept ': ' at line end",                 \
    "plain scalar, do not accept ':' at line end",                  \
    "plain scalar, accept ' #' at line start",                      \
    "plain scalar, accept ' #' on first line",                      \
    "plain scalar, accept ' #' at line end",                        \
    "plain scalar, accept '#'",                                     \
    "plain scalar, explicit",                                       \
    "plain scalar, explicit, early end, seq",                       \
    "plain scalar, explicit, early end, map",                       \
    "plain scalar, multiple docs",                                  \
    "plain scalar, multiple docs, termination",                     \
    "plain scalar, trailing whitespace"


CASE_GROUP(PLAIN_SCALAR)
{
    APPEND_CASES(

C("plain scalar, 1 word only",
R"(a_single_word_scalar_to_test)",
  N(DOCVAL, "a_single_word_scalar_to_test")
),

C("plain scalar, 1 line with spaces",
R"(a scalar with spaces in it all in one line)",
  N(DOCVAL, "a scalar with spaces in it all in one line")
),

C("plain scalar, multiline",
R"(
a scalar with several lines in it
  of course also with spaces but for now there are no quotes
  and also no blank lines to speak of)",
  N(DOCVAL, "a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")
),

C("plain scalar, multiline, unindented",
R"(
a scalar with several lines in it
 of course also with spaces but for now there are no quotes
 and also no blank lines to speak of)",
  N(DOCVAL, "a scalar with several lines in it of course also with spaces but for now there are no quotes and also no blank lines to speak of")
),

C("plain scalar, multiline, quotes, escapes",
R"(
a scalar with several lines in it and also 'single quotes'
  and "double quotes" and assorted escapes such as \r or \n)",
  N(DOCVAL, "a scalar with several lines in it and also 'single quotes' and \"double quotes\" and assorted escapes such as \\r or \\n")
),

C("plain scalar, multiline, quotes, escapes, blank lines middle",
R"(
A scalar with several lines in it and also 'single quotes'.
  A blank line follows after this one.
  
  And "double quotes" and assorted escapes such as \r or \n)",
  N(DOCVAL, "A scalar with several lines in it and also 'single quotes'. A blank line follows after this one.\nAnd \"double quotes\" and assorted escapes such as \\r or \\n")
),

C("plain scalar, multiline, quotes, escapes, blank lines first",
R"(
A scalar with several lines in it and also 'single quotes'.
  
  A blank line precedes this one.
  And "double quotes" and assorted escapes such as \r or \n)",
  N(DOCVAL, "A scalar with several lines in it and also 'single quotes'.\nA blank line precedes this one. And \"double quotes\" and assorted escapes such as \\r or \\n")
),

C("plain scalar, multiline, quotes, escapes, blank lines last",
R"(
A scalar with several lines in it and also 'single quotes'.
  And "double quotes" and assorted escapes such as \r or \n.
  A blank line follows after this one.
  
  )",
  N(DOCVAL, "A scalar with several lines in it and also 'single quotes'. And \"double quotes\" and assorted escapes such as \\r or \\n. A blank line follows after this one.")
),

C("plain scalar, example",
R"(
Several lines of text
  with some "quotes" of various 'types'.
  Escapes (like \n) don't do anything.
  
  Newlines can be added by leaving a blank line.
      Additional leading whitespace is ignored.)",
  N(DOCVAL, "Several lines of text with some \"quotes\" of various 'types'. Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Additional leading whitespace is ignored.")
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
      "and another four at the end -"
    )}
),

C("plain scalar, special characters 3MYT",
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
      N(DOCMAP, L{
              N("a", "1"),
              N(KEYVAL, "b", {}),
              N("c", AR(KEYANCH, "anchor"), "3"),
              N(KEYVAL, "d", {}),
              N(TS("!!str", "e"), "4"),
              N(KEYVAL, "f", {}),
          }),

      N(DOCVAL, "k:#foo &a !t s"),
      N(DOCVAL|VALQUO, "k:#foo &a !t s"),
      N(DOCVAL|VALQUO, "k:#foo &a !t s"),

      N(DOCVAL, "k:#foo &a !t s"),
      N(DOCVAL, "k:#foo &a !t s"),
      N(DOCVAL, "k:#foo &a !t s"),
      N(DOCVAL, "k:#foo &a !t s"),

      N(DOCVAL, "k:#foo !t s"),
      N(DOCVAL, "k:#foo !t s"),
      N(DOCVAL, "k:#foo !t s"),
      N(DOCVAL, "k:#foo !t s"),
  })
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
),

C("plain scalar, do not accept ': ' mid line", HAS_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this: must cause a parse error.
)",
  LineCol(4, 11)
),

C("plain scalar, do not accept ': ' start line", HAS_PARSE_ERROR,
R"(
- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error -
  : foo bar
)",
  LineCol(6, 3)
),

C("plain scalar, do not accept ': ' at line end", HAS_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error: 
)",
  LineCol(4, 36)
),

C("plain scalar, do not accept ':' at line end", HAS_PARSE_ERROR,
R"(- Several lines of text,
  with special:characters, like:this-or-this -
  - and some "quotes" of various 'types'.
  But this must cause a parse error:
  - well, did it?
)",
  LineCol(4, 36)
),

C("plain scalar, accept ' #' at line start",
R"(- Several lines of text,
  and this is valid -
  #with special:characters, like:this-or-this -
)",
  L{N("Several lines of text, and this is valid -"),}
),

C("plain scalar, accept ' #' on first line",
R"(- Several lines of text, and this is valid -
  #with special:characters, like:this-or-this -
)",
  L{N("Several lines of text, and this is valid -")}
),

C("plain scalar, accept ' #' at line end",
R"(- Several lines of text,
  with special:characters, #comment at the end
)",
  L{N("Several lines of text, with special:characters,")}
),

C("plain scalar, accept '#'",
R"(
- Several lines of text, # with a comment
- Several lines of text,
  with special#characters, like#this_#_-or-#-:this -
  - and some "quotes" of various 'types'.
)",
  L{
      N("Several lines of text,"),
      N("Several lines of text, "
        "with special#characters, like#this_#_-or-#-:this - "
        "- and some \"quotes\" of various 'types'."),
   }
),

C("plain scalar, explicit",
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
  L{
      N("a plain scalar with several lines\nand blank lines\nas well"),
      N("and another plain scalar"),
      N("and yet another one\n\n\nwith many lines\nand yet more"),
      N("deindented"),
   }
),

C("plain scalar, explicit, early end, seq", HAS_PARSE_ERROR,
R"([
  a plain scalar
    with several lines
)",
  LineCol(4, 1)
),

C("plain scalar, explicit, early end, map", HAS_PARSE_ERROR,
R"({foo:
  a plain scalar
    with several lines
)",
  LineCol(4, 1)
),

C("plain scalar, multiple docs",
R"(---
- a plain scalar
    with several lines
---
- a second plain scalar
    with several lines
)",
  N(STREAM, L{
    N(DOCSEQ, L{N("a plain scalar with several lines")}),
    N(DOCSEQ, L{N("a second plain scalar with several lines")}),
  })
),

C("plain scalar, multiple docs, termination",
R"(---
- a plain scalar
    with several lines
...
---
- a second plain scalar
    with several lines
)",
  N(STREAM, L{
    N(DOCSEQ, L{N("a plain scalar with several lines")}),
    N(DOCSEQ, L{N("a second plain scalar with several lines")}),
  })
),

C("plain scalar, trailing whitespace",
  R"(---
foo  
---
foo  

---
foo  



)",
  N(STREAM, L{
          N(DOCVAL, "foo"),
          N(DOCVAL, "foo"),
          N(DOCVAL, "foo"),
      })
    )
    )

}

INSTANTIATE_GROUP(PLAIN_SCALAR)

} // namespace yml
} // namespace c4
