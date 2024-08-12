#include "./test_lib/test_case.hpp"
#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace c4 {
namespace yml {

struct dquoted_case
{
    csubstr input, output;
};


// double quoted filtering can result in an output larger than the input.
// so we ensure adequate test covering by using different sizes.
// test also cases where the destination string is not large
// enough to accomodate the filtered string.

/** when filtering from src to dst, specifying the dst sz is enough to
 * cover the different cases */
void test_filter_src_dst(csubstr input, csubstr expected, size_t dst_sz)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~\nsz={}", input.len, input, expected.len, expected, dst_sz);
    // fill the dst buffer with a ref char to ensure there is no
    // write overflow.
    const size_t actual_sz = size_t(30) + (dst_sz > expected.len ? dst_sz : expected.len);
    std::string subject_;
    subject_.resize(actual_sz);
    const substr full = to_substr(subject_);
    // fill the canary region
    const char refchar = '`';
    full.sub(dst_sz).fill(refchar);
    // filter now
    const substr dst = full.first(dst_sz);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_dquoted(input, dst);
    // check the result
    EXPECT_EQ(result.required_len(), expected.len);
    if(result.valid())
    {
        const csubstr out = result.get();
        RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
        RYML_TRACE_FMT("\nout.str=[{}]{}\ndst.str=[{}]{}", out.len,(void const*)out.str, dst.len,(void const*)dst.str);
        EXPECT_TRUE(out.is_sub(dst));
        EXPECT_EQ(out, expected);
        // check the fill character in the canary region
        EXPECT_GT(full.sub(dst_sz).len, 0u);
    }
    EXPECT_EQ(full.sub(dst_sz).first_not_of(refchar), csubstr::npos);
}


void test_filter_inplace(csubstr input, csubstr expected, csubstr leading_input, csubstr leading_expected)
{
    // fill the dst buffer with a ref char to ensure there is no
    // write overflow.
    const size_t input_sz = leading_input.len + input.len;
    const size_t expected_sz = leading_expected.len + expected.len;
    const size_t max_sz = (input_sz > expected_sz ? input_sz : expected_sz);
    const size_t full_sz = max_sz + size_t(30);
    std::string expected_(leading_expected.str, leading_expected.len);
    expected_ += std::string(expected.str, expected.len);
    RYML_TRACE_FMT("\ninp=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~\nlead=[{}]~~~{}~~~\nlead_exp=[{}]~~~{}~~~\nmax_sz={}", input.len, input, expected.len, expected, leading_input.len, leading_input, leading_expected.len, leading_expected, max_sz);
    auto run = [&](size_t cap){
        // create the string
        std::string subject_(leading_input.str, leading_input.len);
        subject_.append(input.str, input.len);
        std::string subject_2 = subject_;
        subject_.resize(full_sz);
        // fill the canary region
        const char refchar = '`';
        const substr full = to_substr(subject_);
        full.sub(max_sz).fill(refchar);
        substr dst = full.first(input_sz);
        // filter now
        Parser::handler_type event_handler1 = {};
        Parser parser1(&event_handler1);
        FilterResultExtending result = parser1.filter_scalar_dquoted_in_place(dst, cap);
        Parser::handler_type event_handler2 = {};
        Parser parser2(&event_handler2);
        Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
        ASSERT_EQ(parser2.m_evt_handler, &event_handler2);
        parser2.m_evt_handler->m_tree = &tree;
        csubstr sresult = parser2._filter_scalar_dquot(to_substr(subject_2));
        EXPECT_GE(result.required_len(), expected_sz);
        EXPECT_EQ(sresult.len, result.str.len);
        if(result.valid())
        {
            const csubstr out = result.get();
            EXPECT_EQ(out, expected_);
            EXPECT_EQ(sresult, expected_);
            EXPECT_EQ(sresult, out);
            // check the fill character in the canary region.
            EXPECT_GT(full.sub(max_sz).len, 0u);
            EXPECT_EQ(full.first_not_of(refchar, max_sz), csubstr::npos);
        }
    };
    if(input_sz >= expected_sz)
    {
        RYML_TRACE_FMT("all good: input_sz={} >= expected_sz={}", input_sz, expected_sz);
        run(input_sz);
    }
    else // input_sz < expected_sz
    {
        RYML_TRACE_FMT("expanding: input_sz={} < expected_sz={}", input_sz, expected_sz);
        {
            RYML_TRACE_FMT("expanding.1: up to larger expected_sz={}", expected_sz);
            run(expected_sz);
        }
        // there is no room to filter if we pass input_sz as the capacity.
        {
            RYML_TRACE_FMT("expanding.2: up to smaller input_sz={}", input_sz);
            run(input_sz);
        }
    }
}


//-----------------------------------------------------------------------------

// some strings cannot be portably declared in double quotes in C++,
// so we use this helper macro, which creates an char array and
// associated csubstr.
#define DECLARE_CSUBSTR_FROM_CHAR_ARR(name, ...) \
    const char name##_[] = { __VA_ARGS__ }; \
    csubstr name = {name##_, C4_COUNTOF(name##_)}

C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4566) // 4566: character represented by universal-character-name '\u263A' cannot be represented in the current code page (1252)

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
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         // \N
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         // \L
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         // \P
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_underscore,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_underscore2,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_underscore3,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_underscore4,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x60, 0xa0),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_N,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_N2,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_N3,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_N4,
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
         _RYML_CHCONST(-0x3e, 0xc2), _RYML_CHCONST(-0x7b, 0x85),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L2,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L3,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L4,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P2,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P3,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P4,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );

// declare double quoted test cases
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
    dqc(R"(\_\_)", dqesc_underscore2),
    dqc(R"(\_\_\_)", dqesc_underscore3),
    dqc(R"(\_\_\_\_)", dqesc_underscore4),
    dqc(R"(\N)", dqesc_N),
    // 40
    dqc(R"(\N\N)", dqesc_N2),
    dqc(R"(\N\N\N)", dqesc_N3),
    dqc(R"(\N\N\N\N)", dqesc_N4),
    dqc(R"(\L)", dqesc_L),
    dqc(R"(\L\L)", dqesc_L2),
    // 45
    dqc(R"(\L\L\L)", dqesc_L3),
    dqc(R"(\L\L\L\L)", dqesc_L4),
    dqc(R"(\P)", dqesc_P),
    dqc(R"(\P\P)", dqesc_P2),
    dqc(R"(\P\P\P)", dqesc_P3),
    // 50
    dqc(R"(\P\P\P\P)", dqesc_P4),
    dqc(R"(\\\"\n\r\t\	\/\ \0\b\f\a\v\e\_\N\L\P)", dqescparsed),
    dqc(R"(\u263A)", R"(☺)"),
    dqc(R"(\u263a)", R"(☺)"),
    dqc(R"(\u2705)", R"(✅)"),
    // 55
    dqc(R"(\u2705\u2705)", R"(✅✅)"),
    dqc(R"(\u2705\u2705\u2705)", R"(✅✅✅)"),
    dqc(R"(\u2705\u2705\u2705\u2705)", R"(✅✅✅✅)"),
    dqc(R"(\U0001D11E)", R"(𝄞)"),
    dqc(R"(\U0001d11e)", R"(𝄞)"),
    // 60
    dqc(R"(\U0001d11e\U0001D11E)", R"(𝄞𝄞)"),
    dqc(R"(\U0001d11e\U0001D11E\U0001D11E)", R"(𝄞𝄞𝄞)"),
    dqc(R"(\U0001d11e\U0001D11E\U0001D11E\U0001D11E)", R"(𝄞𝄞𝄞𝄞)"),
    dqc(R"(\u263A\u2705\U0001D11E)", R"(☺✅𝄞)"),
    dqc(R"(\b1998\t1999\t2000\n)", "\b1998\t1999\t2000\n"),
    // 65
    dqc(R"(\x0d\x0a is \r\n)", "\r\n is \r\n"),
    dqc("\n  foo\n\n    bar\n\n  baz\n", " foo\nbar\nbaz "),
    dqc(" 1st non-empty\n\n 2nd non-empty \n 3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc(" 1st non-empty\n\n 2nd non-empty \n	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc(" 1st non-empty\n\n 2nd non-empty 	\n 	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    // 70
    dqc(" 1st non-empty\n\n 2nd non-empty	 \n	3rd non-empty ", " 1st non-empty\n2nd non-empty 3rd non-empty "),
    dqc("\n  ", " "),
    dqc("  \n  ", " "),
    dqc("\n\n  ", "\n"),
    dqc("\n\n\n  ", "\n\n"),
    // 75
    dqc("folded \nto a space,	\n \nto a line feed, or 	\\\n \\ 	non-content", "folded to a space,\nto a line feed, or \t \tnon-content"),
    dqc("folded \nto a space,\n \nto a line feed, or 	\\\n \\ 	non-content", "folded to a space,\nto a line feed, or \t \tnon-content"),
    //dqc("	\n\ndetected\n\n", "\t\ndetected\n"), // this case cannot be prefixed with anything.
    dqc(R"(This is a key\nthat has multiple lines\n)", "This is a key\nthat has multiple lines\n"),
    dqc("This is a key\n\nthat has multiple lines\n\n", "This is a key\nthat has multiple lines\n"),
    #undef dqc
};
C4_SUPPRESS_WARNING_MSVC_POP


//-----------------------------------------------------------------------------

TEST(double_quoted_filter, leading_tab)
{
}


//-----------------------------------------------------------------------------

struct DQuotedFilterSrcDstTest : public ::testing::TestWithParam<dquoted_case>
{
};


TEST_P(DQuotedFilterSrcDstTest, dst_is_same_size)
{
    dquoted_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.output, /*dst_sz*/dqc.output.len);
}

TEST_P(DQuotedFilterSrcDstTest, dst_is_larger_size)
{
    dquoted_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.output, /*sz*/dqc.output.len + 2u);
    test_filter_src_dst(dqc.input, dqc.output, /*sz*/dqc.output.len + 100u);
}

TEST_P(DQuotedFilterSrcDstTest, dst_is_smaller_size)
{
    dquoted_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.output, /*sz*/dqc.output.len / 2u);
}

TEST_P(DQuotedFilterSrcDstTest, dst_is_zero_size)
{
    dquoted_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.output, /*sz*/0u);
}



struct DQuotedFilterInplaceTest : public ::testing::TestWithParam<dquoted_case>
{
};


TEST_P(DQuotedFilterInplaceTest, dst_is_same_size)
{
    dquoted_case dqc = GetParam();
    test_filter_inplace(dqc.input, dqc.output, /*leading*/"", /*leading_expected*/"");
}

TEST_P(DQuotedFilterInplaceTest, dst_is_smaller_size)
{
    // test also with an expanding leading string ("\\L" expands from
    // two to three bytes). This ensures coverage of cases where
    // expected.len > capacity.
    dquoted_case dqc = GetParam();
    test_filter_inplace(dqc.input, dqc.output, /*leading*/"\\L\\L\\L\\L", /*leading_expected*/dqesc_L4);
}


INSTANTIATE_TEST_SUITE_P(double_quoted_filter,
                         DQuotedFilterSrcDstTest,
                         testing::ValuesIn(test_cases_filter));

INSTANTIATE_TEST_SUITE_P(double_quoted_filter,
                         DQuotedFilterInplaceTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------

TEST(double_quoted, leading_whitespace)
{
    csubstr val = "\n \tfoo";
    std::string emitted;
    {
        Tree t = parse_in_arena("\"\"");
        ASSERT_TRUE(t.rootref().is_val());
        ASSERT_TRUE(t.rootref().type().is_val_dquo());
        t.rootref() = val;
        emitrs_yaml<std::string>(t, &emitted);
        _c4dbgpf("emitted: ~~~{}~~~", to_csubstr(emitted));
    }
    test_check_emit_check(to_csubstr(emitted), [&](Tree const &t){
        EXPECT_EQ(t.rootref().val(), val);
    });
}

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
hex esc: "\x0d\x0a is \r\n"
---
- "\x0d\x0a is \r\n"
---
{hex esc: "\x0d\x0a is \r\n"}
---
["\x0d\x0a is \r\n"]
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0)["unicode"].val(), csubstr(R"(☺✅𝄞)"));
        EXPECT_EQ(t.docref(0)["control"].val(), csubstr("\b1998\t1999\t2000\n"));
        EXPECT_EQ(t.docref(0)["hex esc"].val(), csubstr("\r\n is \r\n"));
        EXPECT_EQ(t.docref(1)[0].val(), csubstr("\r\n is \r\n"));
        EXPECT_EQ(t.docref(2)[0].val(), csubstr("\r\n is \r\n"));
        EXPECT_EQ(t.docref(3)[0].val(), csubstr("\r\n is \r\n"));
    });
}

TEST(double_quoted, test_suite_H2RW_0)
{
    std::string yaml = R"("a\n  \nb\n\nc\n\nd\n")";
    std::string emitted = R"("a\n  \nb\n\nc\n\nd\n"
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        ASSERT_EQ(t.rootref().val(), csubstr("a\n  \nb\n\nc\n\nd\n"));
        ASSERT_EQ(emitrs_yaml<std::string>(t), emitted);
    });
    test_check_emit_check(to_csubstr(emitted), [&](Tree const &t){
        ASSERT_EQ(t.rootref().val(), csubstr("a\n  \nb\n\nc\n\nd\n"));
        ASSERT_EQ(emitrs_yaml<std::string>(t), emitted);
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

TEST(double_quoted, test_suite_L24T)
{
    csubstr yaml = R"(foo: "x\n \n")";
    std::string expected = "foo: \"x\\n \\n\"\n";
    test_check_emit_check(yaml, [&](Tree const &t){
        EXPECT_EQ(t["foo"].val(), csubstr("x\n \n"));
        EXPECT_EQ(emitrs_yaml<std::string>(t), expected);
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
        root["t"] |= VAL_DQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        Tree tree2 = parse_in_arena(to_csubstr(s));
        EXPECT_EQ(tree2["t"].val(), tree["t"].val());
    }
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["t"] = "t't\\nt";
        root["t"] |= VAL_SQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        Tree tree2 = parse_in_arena(to_csubstr(s));
        EXPECT_EQ(tree2["t"].val(), tree["t"].val());
    }
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["s"] = "t\rt";
        root["s"] |= VAL_DQUO;
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

ADD_CASE_TO_GROUP("dquoted, empty",
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

": and this is its value
)",
N(MB, L{N(KD|VP, "This is a key\nthat has multiple lines\n", "and this is its value")})
);
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_CLANG_POP
