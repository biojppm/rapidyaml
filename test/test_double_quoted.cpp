#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define DOUBLE_QUOTED_CASES                             \
            "dquoted, only text",                       \
            "dquoted, with single quotes",              \
            "dquoted, with double quotes",              \
            "dquoted, with single and double quotes",   \
            "dquoted, with escapes",                    \
            "dquoted, with newline",                    \
            "dquoted, with tabs",                       \
            "dquoted, with tabs 4ZYM",                  \
            "dquoted, with tabs 7A4E",                  \
            "dquoted, with tabs TL85",                  \
            "dquoted, all",                             \
            "dquoted, empty",                           \
            "dquoted, blank",                           \
            "dquoted, numbers",                         \
            "dquoted, trailing space",                  \
            "dquoted, leading space",                   \
            "dquoted, trailing and leading space",      \
            "dquoted, 1 dquote",                        \
            "dquoted, 2 dquotes",                       \
            "dquoted, 3 dquotes",                       \
            "dquoted, 4 dquotes",                       \
            "dquoted, example 2",                       \
            "dquoted, example 2.1"

CASE_GROUP(DOUBLE_QUOTED)
{
    APPEND_CASES(

C("dquoted, only text",
R"("Some text without any quotes."
)",
  N(DOCVAL, "Some text without any quotes.")
),

C("dquoted, with single quotes",
R"("Some text 'with single quotes'")",
  N(DOCVAL, "Some text 'with single quotes'")
),

C("dquoted, with double quotes",
R"("Some \"text\" \"with double quotes\"")",
  N(DOCVAL, "Some \"text\" \"with double quotes\"")
),

C("dquoted, with single and double quotes",
R"("Some text 'with single quotes' \"and double quotes\".")",
  N(DOCVAL, "Some text 'with single quotes' \"and double quotes\".")
),

C("dquoted, with escapes",
R"("Some text with escapes \\n \\r \\t")",
  N(DOCVAL, "Some text with escapes \\n \\r \\t")
),

C("dquoted, with newline",
R"("Some text with\nnewline")",
  N(DOCVAL, "Some text with\nnewline")
),

C("dquoted, with tabs",
R"("\tSome\ttext\twith\ttabs\t")",
  N(DOCVAL, "\tSome\ttext\twith\ttabs\t")
),

C("dquoted, with tabs 4ZYM",
R"(plain: text
  lines
quoted: "text
  	lines"
block: |
  text
   	lines
)",
  L{N("plain", "text lines"), N(KEYVAL|VALQUO, "quoted", "text lines"), N(KEYVAL|VALQUO,"block", "text\n \tlines\n")}
),

C("dquoted, with tabs 7A4E",
R"(" 1st non-empty

 2nd non-empty 
	3rd non-empty ")",
  N(DOCVAL, " 1st non-empty\n2nd non-empty 3rd non-empty ")
),

C("dquoted, with tabs TL85",
R"("
  foo 
 
  	 bar

  baz
")", N(DOCVAL, " foo\nbar\nbaz ")),

C("dquoted, all",
R"("Several lines of text,
containing 'single quotes' and \"double quotes\". \
Escapes (like \\n) work.\nIn addition,
newlines can be esc\
aped to prevent them from being converted to a space.

Newlines can also be added by leaving a blank line.
    Leading whitespace on lines is ignored."
)",
  N(DOCVAL, "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) work.\nIn addition, newlines can be escaped to prevent them from being converted to a space.\nNewlines can also be added by leaving a blank line. Leading whitespace on lines is ignored.")
),

C("dquoted, empty",
R"("")",
  N(DOCVAL, "")
),

C("dquoted, blank",
R"(
- ""
- " "
- "  "
- "   "
- "    "
)",
  L{N(QV, ""), N(QV, " "), N(QV, "  "), N(QV, "   "), N(QV, "    ")}
),

C("dquoted, numbers", // these should not be quoted when emitting
R"(
- -1
- -1.0
- +1.0
- 1e-2
- 1e+2
)",
  L{N("-1"), N("-1.0"), N("+1.0"), N("1e-2"), N("1e+2")}
),

C("dquoted, trailing space",
R"('a aaaa  ')",
  N(DOCVAL, "a aaaa  ")
),

C("dquoted, leading space",
R"('  a aaaa')",
  N(DOCVAL, "  a aaaa")
),

C("dquoted, trailing and leading space",
R"('  012345  ')",
  N(DOCVAL, "  012345  ")
),

C("dquoted, 1 dquote",
R"("\"")",
  N(DOCVAL, "\"")
),

C("dquoted, 2 dquotes",
R"("\"\"")",
  N(DOCVAL, "\"\"")
),

C("dquoted, 3 dquotes",
R"("\"\"\"")",
  N(DOCVAL, "\"\"\"")
),

C("dquoted, 4 dquotes",
R"("\"\"\"\"")",
  N(DOCVAL, "\"\"\"\"")
),

C("dquoted, 5 dquotes",
R"("\"\"\"\"\"")",
  N(DOCVAL, "\"\"\"\"\"")
),

C("dquoted, example 2",
R"("This is a key\nthat has multiple lines\n": and this is its value
)",
  L{N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")}
),

C("dquoted, example 2.1", IGNORE_LIBYAML_PARSE_FAIL|IGNORE_YAMLCPP_PARSE_FAIL,
R"("This is a key

that has multiple lines

": and this is its value
)",
  L{N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")}
),
    )
}

INSTANTIATE_GROUP(DOUBLE_QUOTED)

} // namespace yml
} // namespace c4
