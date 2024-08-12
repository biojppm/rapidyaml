#include "./test_lib/test_case.hpp"
#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

struct squoted_case
{
    csubstr input, output;
};

void test_filter(csubstr input, csubstr expected)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", input.len, input, expected.len, expected);
    ASSERT_LE(expected.len, input.len);
    std::string subject_;
    subject_.resize(2 * input.size());
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_squoted(input, dst);
    ASSERT_TRUE(result.valid());
    csubstr out = result.get();
    if(input != expected)
    {
        EXPECT_TRUE(out.is_sub(dst));// << "\ninput=" << input << "\nexpected=" << expected;
    }
    EXPECT_EQ(out, expected);
    std::cout << "OK! ~~~" << input << "~~~   --->  ~~~" << out << "~~~\n";
}

void test_filter_inplace(csubstr input, csubstr expected)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", input.len, input, expected.len, expected);
    ASSERT_LE(expected.len, input.len);
    std::string subject_(input.str, input.len);
    std::string subject_2 = subject_;
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler1 = {};
    Parser parser1(&event_handler1);
    FilterResult result = parser1.filter_scalar_squoted_in_place(dst, subject_.size());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    csubstr sresult = parser2._filter_scalar_squot(to_substr(subject_2));
    EXPECT_GE(result.required_len(), expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    ASSERT_TRUE(result.valid());
    csubstr out = result.get();
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_sub(dst));// << "\ninput=" << input << "\nexpected=" << expected;
    EXPECT_EQ(out, expected);
    std::cout << "OK! ~~~" << input << "~~~   --->  ~~~" << out << "~~~\n";
}

struct SQuotedFilterTest : public ::testing::TestWithParam<squoted_case>
{
};

TEST_P(SQuotedFilterTest, filter)
{
    squoted_case sqc = GetParam();
    test_filter(sqc.input, sqc.output);
}
TEST_P(SQuotedFilterTest, filter_inplace)
{
    squoted_case sqc = GetParam();
    test_filter_inplace(sqc.input, sqc.output);
}

squoted_case test_cases_filter[] = {
    #define sqc(input, output) squoted_case{csubstr(input), csubstr(output)}
    // 0
    sqc("", ""),
    sqc(" ", " "),
    sqc("  ", "  "),
    sqc("   ", "   "),
    sqc("    ", "    "),
    // 5
    sqc("foo", "foo"),
    sqc("quoted\nstring", "quoted string"),
    sqc("quoted\n\nstring", "quoted\nstring"),
    sqc("quoted\n\n\nstring", "quoted\n\nstring"),
    sqc("quoted\n\n\n\nstring", "quoted\n\n\nstring"),
    // 10
    sqc("quoted\n  string", "quoted string"),
    sqc("\"Howdy!\" he cried.", "\"Howdy!\" he cried."),
    sqc(" # Not a ''comment''.", " # Not a 'comment'."),
    sqc("|\\-*-/|", "|\\-*-/|"),
    sqc("\t\n\ndetected\n\n", "\t\ndetected\n"),
    // 15
    sqc(" 1st non-empty\n\n 2nd non-empty \n 3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    sqc(" 1st non-empty\n\n 2nd non-empty \t\n \t3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    sqc(" 1st non-empty\n\n 2nd non-empty\t \n\t 3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    sqc("Several lines of text,\ncontaining ''single quotes'' and \"double quotes\". Escapes (like \\n) don''t do anything.\n\nNewlines can be added by leaving a blank line.\n            Leading whitespace on lines is ignored.",
        "Several lines of text, containing 'single quotes' and \"double quotes\". Escapes (like \\n) don't do anything.\nNewlines can be added by leaving a blank line. Leading whitespace on lines is ignored."),
    sqc(R"(Some text ''with single quotes'' "and double quotes".)", "Some text 'with single quotes' \"and double quotes\"."),
    // 20
    sqc(R"(Some text with escapes \n \r \t)", "Some text with escapes \\n \\r \\t"),
    sqc("''", "'"),
    sqc("''''", "''"),
    sqc("''''''", "'''"),
    sqc("''''''''", "''''"),
    // 25
    sqc("''''''''''", "'''''"),
    sqc("''''''''''''", "''''''"),
    sqc(R"(a aaaa )",  "a aaaa "),
    sqc(R"(a aaaa  )",  "a aaaa  "),
    sqc(R"(a aaaa   )",  "a aaaa   "),
    // 30
    sqc(R"(a aaaa    )",  "a aaaa    "),
    sqc(R"(a aaaa     )",  "a aaaa     "),
    sqc(R"( a aaaa)",  " a aaaa"),
    sqc(R"(  a aaaa)",  "  a aaaa"),
    sqc(R"(   a aaaa)",  "   a aaaa"),
    // 35
    sqc(R"(    a aaaa)",  "    a aaaa"),
    sqc(R"(     a aaaa)",  "     a aaaa"),
    sqc(R"( a aaaa )",  " a aaaa "),
    sqc(R"(  a aaaa  )",  "  a aaaa  "),
    sqc(R"(   a aaaa   )",  "   a aaaa   "),
    // 40
    sqc(R"(    a aaaa    )",  "    a aaaa    "),
    sqc(R"(     a aaaa     )",  "     a aaaa     "),
    sqc(R"(x\ny:z\tx $%^&*()x)", "x\\ny:z\\tx $%^&*()x"),
    #undef sqc
};

INSTANTIATE_TEST_SUITE_P(single_quoted_filter,
                         SQuotedFilterTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(single_quoted, test_suite_KSS4)
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
    csubstr yaml = R"(---
' 1st non-empty

 2nd non-empty 
	3rd non-empty '
---
' 1st non-empty

 2nd non-empty 	
 	3rd non-empty '
---
' 1st non-empty

 2nd non-empty	 
	3rd non-empty '
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0).val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
        EXPECT_EQ(t.docref(1).val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
        EXPECT_EQ(t.docref(2).val(), csubstr(" 1st non-empty\n2nd non-empty 3rd non-empty "));
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

TEST(single_quoted, test_suite_G4RS)
{
    csubstr yaml = R"(
single: '"Howdy!" he cried.'
quoted: ' # Not a ''comment''.'
tie-fighter: '|\-*-/|'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["single"].val()     , csubstr(R"("Howdy!" he cried.)"));
        EXPECT_EQ(t["quoted"].val()     , csubstr(R"( # Not a 'comment'.)"));
        EXPECT_EQ(t["tie-fighter"].val(), csubstr(R"(|\-*-/|)"));
    });
}

TEST(single_quoted, quotes_are_preserved)
{
    csubstr yaml = R"(
'%ROOT': '%VAL'
'%ROOT2':
  - '%VAL'
  - '%VAL'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("%ROOT"));
        ASSERT_TRUE(t.rootref().has_child("%ROOT2"));
        ASSERT_EQ(t["%ROOT2"].num_children(), 2u);
        EXPECT_TRUE(t["%ROOT"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT"].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT2"][0].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"][1].is_val_quoted());
    });
}


//-----------------------------------------------------------------------------

void verify_error_is_reported(csubstr case_name, csubstr yaml, Location loc={})
{
    SCOPED_TRACE(case_name);
    SCOPED_TRACE(yaml);
    Tree tree;
    ExpectError::check_error(&tree, [&](){
        parse_in_arena(yaml, &tree);
    }, loc);
}

void verify_filter_error_is_reported(csubstr case_name, csubstr scalar_, Location loc={})
{
    SCOPED_TRACE(case_name);
    SCOPED_TRACE(scalar_);
    {
        Tree t;
        ExpectError::check_error(&t, [&](){
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            evt_handler.reset(&t, t.root_id());
            std::string buf(scalar_.begin(), scalar_.end());
            buf.resize(scalar_.len * 2);
            substr scalar = to_substr(buf).first(scalar_.len);
            to_substr(buf).sub(scalar_.len).fill('_');
            FilterResult result = parser.filter_scalar_squoted_in_place(scalar, buf.size());
            return result;
        }, loc);
    }
    {
        Tree t;
        ExpectError::check_error(&t, [&](){
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            evt_handler.reset(&t, t.root_id());
            std::string buf;
            buf.resize(scalar_.len * 2);
            to_substr(buf).fill('_');
            FilterResult result = parser.filter_scalar_squoted(scalar_, to_substr(buf));
            return result;
        }, loc);
    }
}

TEST(single_quoted, error_on_unmatched_quotes)
{
    verify_error_is_reported("map block", R"(foo: '"
bar: '')");
    verify_error_is_reported("seq block", R"(- '"
- '')");
    verify_error_is_reported("map flow", R"({foo: '", bar: ''})");
    verify_error_is_reported("seq flow", R"(['", ''])");
}

TEST(single_quoted, error_on_unmatched_quotes_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: '''"
bar: '')");
    verify_error_is_reported("seq block", R"(- '''"
- '')");
    verify_error_is_reported("map flow", R"({foo: '''", bar: ''})");
    verify_error_is_reported("seq flow", R"(['''", ''])");
}

TEST(single_quoted, error_on_unmatched_quotes_at_end)
{
    verify_error_is_reported("map block", R"(foo: ''
bar: '")");
    verify_error_is_reported("seq block", R"(- ''
- '")");
    verify_error_is_reported("map flow", R"({foo: '', bar: '"})");
    verify_error_is_reported("seq flow", R"(['', '"])");
}

TEST(single_quoted, error_on_unmatched_quotes_at_end_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: ''
bar: '''")");
    verify_error_is_reported("seq block", R"(- ''
- '''")");
    verify_error_is_reported("map flow", R"({foo: '', bar: '''"})");
    verify_error_is_reported("seq flow", R"(['', '''"])");
}

TEST(single_quoted, error_on_unclosed_quotes)
{
    verify_error_is_reported("map block", R"(foo: ',
bar: what)");
    verify_error_is_reported("seq block", R"(- '
- what)");
    verify_error_is_reported("map flow", R"({foo: ', bar: what})");
    verify_error_is_reported("seq flow", R"([', what])");
}

TEST(single_quoted, error_on_unclosed_quotes_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: ''',
bar: what)");
    verify_error_is_reported("seq block", R"(- '''
- what)");
    verify_error_is_reported("map flow", R"({foo: ''', bar: what})");
    verify_error_is_reported("seq flow", R"([''', what])");
}

TEST(single_quoted, error_on_unclosed_quotes_at_end)
{
    verify_error_is_reported("map block", R"(foo: what
bar: ')");
    verify_error_is_reported("seq block", R"(- what
- ')");
    verify_error_is_reported("map flow", R"({foo: what, bar: '})");
    verify_error_is_reported("seq flow", R"([what, '])");
}

TEST(single_quoted, error_on_unclosed_quotes_at_end_with_escapes)
{
    verify_error_is_reported("map block", R"(foo: what
bar: ''')");
    verify_error_is_reported("seq block", R"(- what
- ''')");
    verify_error_is_reported("map flow", R"({foo: what, bar: '''})");
    verify_error_is_reported("seq flow", R"([what, '''])");
}

TEST(single_quoted, error_on_isolated_quotes)
{
    verify_filter_error_is_reported("isolated quotes", "a'a'a'a");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

ADD_CASE_TO_GROUP("squoted, empty",
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
R"('This is a key

that has multiple lines

': and this is its value
)",
N(MB, L{N(KS|VP, "This is a key\nthat has multiple lines\n", "and this is its value")})
);

ADD_CASE_TO_GROUP("squoted indentation, 0",
R"(' 1st non-empty

 2nd non-empty
	3rd non-empty '
)",
N(VS, " 1st non-empty\n2nd non-empty 3rd non-empty ")
);

/* FIXME - tab is invalid indentation on line 4 (before 3rd non-empty)
ADD_CASE_TO_GROUP("squoted indentation, 1", EXPECT_PARSE_ERROR,
R"(- ' 1st non-empty

 2nd non-empty
	3rd non-empty '
)",
LineCol(1,1)
);
*/

ADD_CASE_TO_GROUP("squoted indentation, 2",
R"(- ' 1st non-empty

 2nd non-empty
 	3rd non-empty '
)",
N(SB, L{N(VS, " 1st non-empty\n2nd non-empty 3rd non-empty ")})
);

}

} // namespace yml
} // namespace c4
