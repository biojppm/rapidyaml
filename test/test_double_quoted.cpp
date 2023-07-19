#include "./test_group.hpp"

namespace c4 {
namespace yml {

struct dquoted_case
{
    csubstr input, output;
};

size_t needed_size(csubstr input)
{
    return 4 * input.size() + 4;
}

void test_filter(csubstr input, csubstr expected)
{
    SCOPED_TRACE(input);
    SCOPED_TRACE(expected);
    std::string subject_;
    subject_.resize(needed_size(input));
    c4::substr dst = to_substr(subject_);
    ScalarFilterProcessor proc = {};
    csubstr out = proc.filter_dquoted(input, dst, Location{});
    if(input != expected)
    {
        EXPECT_TRUE(out.is_sub(dst));// << "\ninput=" << input << "\nexpected=" << expected;
    }
    EXPECT_EQ(out, expected);
}

void test_filter_inplace(csubstr input, csubstr expected)
{
    SCOPED_TRACE(input);
    SCOPED_TRACE(expected);
    std::string subject_(input.str, input.len);
    subject_.reserve(needed_size(input));
    c4::substr dst = to_substr(subject_);
    ScalarFilterProcessor proc = {};
    csubstr out = proc.filter_dquoted(dst, subject_.capacity(), Location{});
    EXPECT_TRUE(out.is_sub(dst));// << "\ninput=" << input << "\nexpected=" << expected;
    EXPECT_EQ(out, expected);
    std::cout << "OK! ~~~" << input << "~~~   --->  ~~~" << out << "~~~\n";
}


struct DQuotedFilterTest : public ::testing::TestWithParam<dquoted_case>
{
};

TEST_P(DQuotedFilterTest, filter)
{
    dquoted_case dqc = GetParam();
    test_filter(dqc.input, dqc.output);
}

TEST_P(DQuotedFilterTest, filter_inplace)
{
    dquoted_case dqc = GetParam();
    test_filter_inplace(dqc.input, dqc.output);
}


#define DECLARE_CSUBSTR_FROM_CHAR_ARR(name, ...) \
const char name##_[] = { __VA_ARGS__ }; \
csubstr name = {name##_, C4_COUNTOF(name##_)}

DECLARE_CSUBSTR_FROM_CHAR_ARR(dqescparsed,
         '\\',
         '"',
         '\n',
         '\r',
         '\t',
         '\t',
         '/',
         ' ',
         '\0',
         '\b',
         '\f',
         '\a',
         '\v',
         INT8_C(0x1b),
          // \_
         _RYML_CHCONST(-0x3e, 0xc2),
         _RYML_CHCONST(-0x60, 0xa0),
         // \N
         _RYML_CHCONST(-0x3e, 0xc2),
         _RYML_CHCONST(-0x7b, 0x85),
         // \L
         _RYML_CHCONST(-0x1e, 0xe2),
         _RYML_CHCONST(-0x80, 0x80),
         _RYML_CHCONST(-0x58, 0xa8),
         // \P
         _RYML_CHCONST(-0x1e, 0xe2),
         _RYML_CHCONST(-0x80, 0x80),
         _RYML_CHCONST(-0x57, 0xa9),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_underscore,
         _RYML_CHCONST(-0x3e, 0xc2),
         _RYML_CHCONST(-0x60, 0xa0),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_N,
         _RYML_CHCONST(-0x3e, 0xc2),
         _RYML_CHCONST(-0x7b, 0x85),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L,
         _RYML_CHCONST(-0x1e, 0xe2),
         _RYML_CHCONST(-0x80, 0x80),
         _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P,
         _RYML_CHCONST(-0x1e, 0xe2),
         _RYML_CHCONST(-0x80, 0x80),
         _RYML_CHCONST(-0x57, 0xa9),
    );
dquoted_case test_cases_filter[] = {
    #define dqc(input, ...) dquoted_case{csubstr(input), csubstr(__VA_ARGS__)}
    // 0
    dqc("", ""),
    dqc(" ", " "),
    dqc("  ", "  "),
    dqc("   ", "   "),
    dqc("    ", "    "),
    // 5
    dqc("foo", "foo"),
    dqc("foo bar", "foo bar"),
    dqc("1 leading\n   \\ttab", "1 leading \ttab"),
    dqc("2 leading\n    \\	tab", "2 leading \ttab"),
    dqc("3 leading\n    	tab", "3 leading tab"),
    // 10
    dqc("4 leading\n    \\t  tab", "4 leading \t  tab"),
    dqc("5 leading\n    \\	  tab", "5 leading \t  tab"),
    dqc("6 leading\n    	  tab", "6 leading tab"),
    dqc("Empty line\n\n  as a line feed", "Empty line\nas a line feed"),
    dqc(R"(foo\nbar:baz\tx \\$%^&*()x)", "foo\nbar:baz\tx \\$%^&*()x"),
    // 15
    dqc(R"(\)", ""),
    dqc(R"(\\)", "\\"),
    dqc(R"(\\\)", "\\"),
    dqc(R"(\\\\)", "\\\\"),
    dqc(R"(\\\\\)", "\\\\"),
    // 20
    dqc(R"(\	)", "\t"),
    dqc(R"(\t)", "\t"),
    dqc(R"(\ )", " "),
    dqc(R"(\\ )", "\\ "),
    dqc(R"(\")", "\""),
    // 25
    dqc(R"(\"\")", "\"\""),
    dqc(R"(\n)", "\n"),
    dqc(R"(\r)", "\r"),
    dqc(R"(\t)", "\t"),
    dqc(R"(\0)", "\0"),
    // 30
    dqc(R"(\b)", "\b"),
    dqc(R"(\f)", "\f"),
    dqc(R"(\a)", "\a"),
    dqc(R"(\v)", "\v"),
    dqc(R"(\e)", "\x1b"),
    // 35
    dqc(R"(\_)", dqesc_underscore),
    dqc(R"(\N)", dqesc_N),
    dqc(R"(\L)", dqesc_L),
    dqc(R"(\P)", dqesc_P),
    dqc(R"(\\\"\n\r\t\	\/\ \0\b\f\a\v\e\_\N\L\P)", dqescparsed),
    // 40
    dqc("\u263A", R"(☺)"),
    dqc("\u263a", R"(☺)"),
    dqc("\u2705", R"(✅)"),
    dqc("\U0001D11E", R"(𝄞)"),
    dqc("\U0001d11e", R"(𝄞)"),
    // 45
    dqc("\u263A\u2705\U0001D11E", R"(☺✅𝄞)"),
    dqc(R"(\b1998\t1999\t2000\n)", "\b1998\t1999\t2000\n"),
    dqc(R"(\x0d\x0a is \r\n)", "\r\n is \r\n"),
    dqc("\n  foo\n\n    bar\n\n  baz\n", " foo\nbar\nbaz "),
    dqc(" 1st non-empty\n\n 2nd non-empty \n 3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    // 50
    dqc(" 1st non-empty\n\n 2nd non-empty \n	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc(" 1st non-empty\n\n 2nd non-empty 	\n 	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc(" 1st non-empty\n\n 2nd non-empty	 \n	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc("\n  ", " "),
    dqc("  \n  ", " "),
    // 55
    dqc("\n\n  ", "\n"),
    dqc("\n\n\n  ", "\n\n"),
    dqc("folded \nto a space,	\n \nto a line feed, or 	\\\n \\ 	non-content", "folded to a space,\nto a line feed, or \t \tnon-content"),
    dqc("folded \nto a space,\n \nto a line feed, or 	\\\n \\ 	non-content", "folded to a space,\nto a line feed, or \t \tnon-content"),
    dqc("	\n\ndetected\n\n", "\t\ndetected\n"),
    #undef dqc
};

INSTANTIATE_TEST_SUITE_P(double_quoted_filter,
                         DQuotedFilterTest,
                         testing::ValuesIn(test_cases_filter));


TEST(double_quoted, escaped_chars)
{
    csubstr yaml = R"("\\\"\n\r\t\	\/\ \0\b\f\a\v\e\_\N\L\P")";
    Tree t = parse_in_arena(yaml);
    csubstr v = t.rootref().val();
    std::string actual = {v.str, v.len};
    // build the string like this because some of the characters are
    // filtered out under the double quotes
    EXPECT_EQ(actual, std::string(dqescparsed.str, dqescparsed.len));
}

TEST(double_quoted, test_suite_3RLN)
{
    csubstr yaml = R"(---
"1 leading
   \ttab"
---
"2 leading
    \	tab"
---
"3 leading
    	tab"
---
"4 leading
    \t  tab"
---
"5 leading
    \	  tab"
---
"6 leading
    	  tab"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0).val(), "1 leading \ttab");
        EXPECT_EQ(t.docref(1).val(), "2 leading \ttab");
        EXPECT_EQ(t.docref(2).val(), "3 leading tab");
        EXPECT_EQ(t.docref(3).val(), "4 leading \t  tab");
        EXPECT_EQ(t.docref(4).val(), "5 leading \t  tab");
        EXPECT_EQ(t.docref(5).val(), "6 leading tab");
    });
}

TEST(double_quoted, test_suite_5GBF)
{
    csubstr yaml = R"(
Folding:
  "Empty line

  as a line feed"
Folding2:
  "Empty line

  as a line feed"
Folding3:
  "Empty line

  as a line feed"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        EXPECT_EQ(t["Folding"].val(), csubstr("Empty line\nas a line feed"));
        EXPECT_EQ(t["Folding2"].val(), csubstr("Empty line\nas a line feed"));
        EXPECT_EQ(t["Folding3"].val(), csubstr("Empty line\nas a line feed"));
    });
}

TEST(double_quoted, test_suite_6SLA)
{
    csubstr yaml = R"(
"foo\nbar:baz\tx \\$%^&*()x": 23
'x\ny:z\tx $%^&*()x': 24
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("foo\nbar:baz\tx \\$%^&*()x"));
        ASSERT_TRUE(t.rootref().has_child("x\\ny:z\\tx $%^&*()x"));
        ASSERT_EQ(t["foo\nbar:baz\tx \\$%^&*()x"].val(), csubstr("23"));
        ASSERT_EQ(t["x\\ny:z\\tx $%^&*()x"].val(), csubstr("24"));
    });
}

TEST(double_quoted, test_suite_6WPF)
{
    csubstr yaml = R"(
"
  foo 
 
    bar

  baz
"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr(" foo\nbar\nbaz "));
    });
}

TEST(double_quoted, test_suite_9TFX)
{
    csubstr yaml = R"(
" 1st non-empty

 2nd non-empty 
 3rd non-empty "
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
    });
}

TEST(double_quoted, test_suite_G4RS)
{
    csubstr yaml = R"(---
unicode: "\u263A\u2705\U0001D11E"
control: "\b1998\t1999\t2000\n"
#hex esc: "\x0d\x0a is \r\n"
#---
#- "\x0d\x0a is \r\n"
#---
#{hex esc: "\x0d\x0a is \r\n"}
#---
#["\x0d\x0a is \r\n"]
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0)["unicode"].val(), csubstr(R"(☺✅𝄞)"));
        EXPECT_EQ(t.docref(0)["control"].val(), csubstr("\b1998\t1999\t2000\n"));
        //EXPECT_EQ(t.docref(0)["hex esc"].val(), csubstr("\r\n is \r\n")); TODO
        //EXPECT_EQ(t.docref(1)[0].val(), csubstr("\r\n is \r\n"));
        //EXPECT_EQ(t.docref(2)[0].val(), csubstr("\r\n is \r\n"));
        //EXPECT_EQ(t.docref(3)[0].val(), csubstr("\r\n is \r\n"));
    });
}

TEST(double_quoted, test_suite_KSS4)
{
    csubstr yaml = R"(
---
"quoted
string"
--- "quoted
string"
---
- "quoted
  string"
---
- "quoted
string"
---
"quoted
  string": "quoted
  string"
---
"quoted
string": "quoted
string"
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

TEST(double_quoted, test_suite_NAT4)
{
    csubstr yaml = R"(
a: '
  '
b: '  
  '
c: "
  "
d: "  
  "
e: '

  '
f: "

  "
g: '


  '
h: "


  "
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["a"].val(), csubstr(" "));
        EXPECT_EQ(t["b"].val(), csubstr(" "));
        EXPECT_EQ(t["c"].val(), csubstr(" "));
        EXPECT_EQ(t["d"].val(), csubstr(" "));
        EXPECT_EQ(t["e"].val(), csubstr("\n"));
        EXPECT_EQ(t["f"].val(), csubstr("\n"));
        EXPECT_EQ(t["g"].val(), csubstr("\n\n"));
        EXPECT_EQ(t["h"].val(), csubstr("\n\n"));
    });
}

TEST(double_quoted, test_suite_NP9H)
{
    csubstr yaml = R"(
"folded 
to a space,	
 
to a line feed, or 	\
 \ 	non-content"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("folded to a space,\nto a line feed, or \t \tnon-content"));
    });
}

TEST(double_quoted, test_suite_Q8AD)
{
    csubstr yaml = R"(
"folded 
to a space,
 
to a line feed, or 	\
 \ 	non-content"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("folded to a space,\nto a line feed, or \t \tnon-content"));
    });
}

TEST(double_quoted, test_suite_R4YG)
{
    csubstr yaml = R"(
- "	

detected

"

)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("\t\ndetected\n"));
    });
}


//-----------------------------------------------------------------------------

void verify_error_is_reported(csubstr case_name, csubstr yaml, Location loc={})
{
    SCOPED_TRACE(case_name);
    SCOPED_TRACE(yaml);
    Tree tree;
    ExpectError::do_check(&tree, [&](){
        parse_in_arena(yaml, &tree);
    }, loc);
}

TEST(double_quoted, error_on_unmatched_quotes)
{
    verify_error_is_reported("map block", R"(foo: "'
bar: "")");
    verify_error_is_reported("seq block", R"(- "'
- "")");
    verify_error_is_reported("map flow", R"({foo: "', bar: ""})");
    verify_error_is_reported("seq flow", R"(["', ""])");
}

TEST(double_quoted, error_on_unmatched_quotes_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: "\"'
bar: "")");
    verify_error_is_reported("seq block", R"(- "\"'
- "")");
    verify_error_is_reported("map flow", R"({foo: "\"', bar: ""})");
    verify_error_is_reported("seq flow", R"(["\"', ""])");
}

TEST(double_quoted, error_on_unmatched_quotes_at_end)
{
    verify_error_is_reported("map block", R"(foo: ""
bar: "')");
    verify_error_is_reported("seq block", R"(- ""
- "')");
    verify_error_is_reported("map flow", R"({foo: "", bar: "'})");
    verify_error_is_reported("seq flow", R"(["", "'])");
}

TEST(double_quoted, error_on_unmatched_quotes_at_end_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: ""
bar: "\"')");
    verify_error_is_reported("seq block", R"(- ""
- "\"')");
    verify_error_is_reported("map flow", R"({foo: "", bar: "\"'})");
    verify_error_is_reported("seq flow", R"(["", "\"'])");
}

TEST(double_quoted, error_on_unclosed_quotes)
{
    verify_error_is_reported("map block", R"(foo: ",
bar: what)");
    verify_error_is_reported("seq block", R"(- "
- what)");
    verify_error_is_reported("map flow", R"({foo: ", bar: what})");
    verify_error_is_reported("seq flow", R"([", what])");
}

TEST(double_quoted, error_on_unclosed_quotes_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: "\",
bar: what)");
    verify_error_is_reported("seq block", R"(- "\"
- what)");
    verify_error_is_reported("map flow", R"({foo: "\", bar: what})");
    verify_error_is_reported("seq flow", R"(["\", what])");
}

TEST(double_quoted, error_on_unclosed_quotes_at_end)
{
    verify_error_is_reported("map block", R"(foo: what
bar: ")");
    verify_error_is_reported("seq block", R"(- what
- ")");
    verify_error_is_reported("map flow", R"({foo: what, bar: "})");
    verify_error_is_reported("seq flow", R"([what, "])");
}

TEST(double_quoted, error_on_unclosed_quotes_at_end_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: what
bar: "\")");
    verify_error_is_reported("seq block", R"(- what
- "\")");
    verify_error_is_reported("map flow", R"({foo: what, bar: "\"})");
    verify_error_is_reported("seq flow", R"([what, "\"])");
}

TEST(double_quoted, error_on_bad_utf_codepoints)
{
    verify_error_is_reported("incomplete \\x 0", R"(foo: "\x")");
    verify_error_is_reported("incomplete \\x 1", R"(foo: "\x1")");
    verify_error_is_reported("bad value  \\x"  , R"(foo: "\xko")");
    verify_error_is_reported("incomplete \\u 0", R"(foo: "\u")");
    verify_error_is_reported("incomplete \\u 1", R"(foo: "\u1")");
    verify_error_is_reported("incomplete \\u 2", R"(foo: "\u12")");
    verify_error_is_reported("incomplete \\u 3", R"(foo: "\u123")");
    verify_error_is_reported("bad value  \\u"  , R"(foo: "\ukoko")");
    verify_error_is_reported("incomplete \\U 0", R"(foo: "\U")");
    verify_error_is_reported("incomplete \\U 1", R"(foo: "\U1")");
    verify_error_is_reported("incomplete \\U 2", R"(foo: "\U12")");
    verify_error_is_reported("incomplete \\U 3", R"(foo: "\U123")");
    verify_error_is_reported("incomplete \\U 4", R"(foo: "\U1234")");
    verify_error_is_reported("incomplete \\U 5", R"(foo: "\U12345")");
    verify_error_is_reported("incomplete \\U 6", R"(foo: "\U123456")");
    verify_error_is_reported("incomplete \\U 7", R"(foo: "\U1234567")");
    verify_error_is_reported("bad value  \\U"  , R"(foo: "\Ukokokoko")");
}

TEST(double_quoted, github253)
{
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["t"] = "t't\\nt";
        root["t"] |= _WIP_VAL_DQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        Tree tree2 = parse_in_arena(to_csubstr(s));
        EXPECT_EQ(tree2["t"].val(), tree["t"].val());
    }
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["t"] = "t't\\nt";
        root["t"] |= _WIP_VAL_SQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        Tree tree2 = parse_in_arena(to_csubstr(s));
        EXPECT_EQ(tree2["t"].val(), tree["t"].val());
    }
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["s"] = "t\rt";
        root["s"] |= _WIP_VAL_DQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        EXPECT_EQ(s, "s: \"t\\rt\"\n");
        Tree tree2 = parse_in_arena(to_csubstr(s));
        EXPECT_EQ(tree2["s"].val(), tree["s"].val());
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(DOUBLE_QUOTED)
{

ADD_CASE_TO_GROUP("dquoted, only text",
R"("Some text without any quotes."
)",
  N(DOCVAL | VALQUO, "Some text without any quotes.")
);

ADD_CASE_TO_GROUP("dquoted, with single quotes",
R"("Some text 'with single quotes'")",
  N(DOCVAL|VALQUO, "Some text 'with single quotes'")
);

ADD_CASE_TO_GROUP("dquoted, with double quotes",
R"("Some \"text\" \"with double quotes\"")",
  N(DOCVAL|VALQUO, "Some \"text\" \"with double quotes\"")
);

ADD_CASE_TO_GROUP("dquoted, with single and double quotes",
R"("Some text 'with single quotes' \"and double quotes\".")",
  N(DOCVAL|VALQUO, "Some text 'with single quotes' \"and double quotes\".")
);

ADD_CASE_TO_GROUP("dquoted, with escapes",
R"("Some text with escapes \\n \\r \\t")",
  N(DOCVAL|VALQUO, "Some text with escapes \\n \\r \\t")
);

ADD_CASE_TO_GROUP("dquoted, with newline",
R"("Some text with\nnewline")",
  N(DOCVAL|VALQUO, "Some text with\nnewline")
);

ADD_CASE_TO_GROUP("dquoted, with tabs",
R"("\tSome\ttext\twith\ttabs\t")",
  N(DOCVAL|VALQUO, "\tSome\ttext\twith\ttabs\t")
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
  L{N("plain", "text lines"),
    N(KEYVAL|VALQUO, "quoted", "text lines"),
    N(KEYVAL|VALQUO,"block", "text\n \tlines\n")}
);

ADD_CASE_TO_GROUP("dquoted, with tabs 7A4E",
R"(" 1st non-empty

 2nd non-empty 
	3rd non-empty ")",
  N(DOCVAL|VALQUO, " 1st non-empty\n2nd non-empty 3rd non-empty ")
);

ADD_CASE_TO_GROUP("dquoted, with tabs TL85",
R"("
  foo 
 
  	 bar

  baz
")", N(DOCVAL|VALQUO, " foo\nbar\nbaz "));

ADD_CASE_TO_GROUP("dquoted, all",
R"("Several lines of text,
containing 'single quotes' and \"double quotes\". \
Escapes (like \\n) work.\nIn addition,
newlines can be esc\
aped to prevent them from being converted to a space.

Newlines can also be added by leaving a blank line.
    Leading whitespace on lines is ignored."
)",
  N(DOCVAL|VALQUO, "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) work.\nIn addition, newlines can be escaped to prevent them from being converted to a space.\nNewlines can also be added by leaving a blank line. Leading whitespace on lines is ignored.")
);

ADD_CASE_TO_GROUP("dquoted, empty",
R"("")",
  N(DOCVAL|VALQUO, "")
);

ADD_CASE_TO_GROUP("dquoted, blank",
R"(
- ""
- " "
- "  "
- "   "
- "    "
)",
  L{N(QV, ""), N(QV, " "), N(QV, "  "), N(QV, "   "), N(QV, "    ")}
);

ADD_CASE_TO_GROUP("dquoted, numbers", // these should not be quoted when emitting
R"(
- -1
- -1.0
- +1.0
- 1e-2
- 1e+2
)",
  L{N("-1"), N("-1.0"), N("+1.0"), N("1e-2"), N("1e+2")}
);

ADD_CASE_TO_GROUP("dquoted, trailing space",
R"('a aaaa  ')",
  N(DOCVAL|VALQUO, "a aaaa  ")
);

ADD_CASE_TO_GROUP("dquoted, leading space",
R"('  a aaaa')",
  N(DOCVAL|VALQUO, "  a aaaa")
);

ADD_CASE_TO_GROUP("dquoted, trailing and leading space",
R"('  012345  ')",
  N(DOCVAL|VALQUO, "  012345  ")
);

ADD_CASE_TO_GROUP("dquoted, 1 dquote",
R"("\"")",
  N(DOCVAL|VALQUO, "\"")
);

ADD_CASE_TO_GROUP("dquoted, 2 dquotes",
R"("\"\"")",
  N(DOCVAL|VALQUO, "\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 3 dquotes",
R"("\"\"\"")",
  N(DOCVAL|VALQUO, "\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 4 dquotes",
R"("\"\"\"\"")",
  N(DOCVAL|VALQUO, "\"\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, 5 dquotes",
R"("\"\"\"\"\"")",
  N(DOCVAL|VALQUO, "\"\"\"\"\"")
);

ADD_CASE_TO_GROUP("dquoted, example 2",
R"("This is a key\nthat has multiple lines\n": and this is its value
)",
  L{N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")}
);

ADD_CASE_TO_GROUP("dquoted, example 2.1",
R"("This is a key

that has multiple lines

": and this is its value
)",
  L{N(QK, "This is a key\nthat has multiple lines\n", "and this is its value")}
);
}

} // namespace yml
} // namespace c4
