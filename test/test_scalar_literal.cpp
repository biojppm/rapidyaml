#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"


namespace c4 {
namespace yml {


struct blocklit_case
{
    size_t indentation;
    BlockChomp_e chomp;
    csubstr input;
    csubstr expected;
};

void test_filter_src_dst(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_;
    subject_.resize(2 * blcase.input.size());
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_block_literal(blcase.input, dst, blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    const csubstr out = result.get();
    _c4prscalar("input:", blcase.input, true);
    _c4prscalar("result:", out, true);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.is_sub(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
}

void test_filter_inplace_smaller_result(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    _c4prscalar("input:", blcase.input, true);
    FilterResult result = proc.filter_scalar_block_literal_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    const csubstr out = result.get();
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    csubstr sresult = parser2._filter_scalar_literal(to_substr(subject_2), blcase.indentation, blcase.chomp);
    _c4prscalar("result:", out, true);
    _c4prscalar("sresult:", sresult, true);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_sub(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
    EXPECT_EQ(sresult, blcase.expected);
}

void test_filter_inplace_larger_result_spare_size(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    subject_.resize(blcase.expected.len + 30);
    c4::substr dst = to_substr(subject_).first(blcase.input.len);
    c4::substr rem = to_substr(subject_).sub(blcase.expected.len);
    rem.fill('^');
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    _c4prscalar("input:", blcase.input, true);
    FilterResult result = proc.filter_scalar_block_literal_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    const csubstr out = result.get();
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_literal(to_substr(subject_2), blcase.indentation, blcase.chomp);
    _c4prscalar("result:", out, true);
    _c4prscalar("sresult:", sresult, true);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_super(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
    EXPECT_EQ(rem.first_not_of('^'), npos);
}

void test_filter_inplace_larger_result_trimmed_size(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    subject_.resize(blcase.expected.len);
    c4::substr dst = to_substr(subject_).first(blcase.input.len);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    _c4prscalar("input:", blcase.input, true);
    FilterResult result = proc.filter_scalar_block_literal_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    const csubstr out = result.get();
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_literal(to_substr(subject_2), blcase.indentation, blcase.chomp);
    _c4prscalar("result:", out, true);
    _c4prscalar("sresult:", sresult, true);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_super(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
}

void test_filter_inplace_larger_result_insufficient_size(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    _c4prscalar("input:", blcase.input, true);
    FilterResult result = proc.filter_scalar_block_literal_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_FALSE(result.valid());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_literal(to_substr(subject_2), blcase.indentation, blcase.chomp);
    _c4prscalar("sresult:", sresult, true);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(result.required_len(), blcase.expected.len);
    }
}


//-----------------------------------------------------------------------------

struct BlockLitFilterTest : public ::testing::TestWithParam<blocklit_case>
{
};

std::string add_carriage_returns(csubstr input)
{
    std::string result;
    result.reserve(input.len + input.count('\n'));
    for(const char c : input)
    {
        if(c == '\n')
            result += '\r';
        result += c;
    }
    return result;
}

TEST_P(BlockLitFilterTest, filter_src_dst)
{
    test_filter_src_dst(GetParam());
}
TEST_P(BlockLitFilterTest, filter_src_dst_carriage_return)
{
    ParamType p = GetParam();
    std::string subject = add_carriage_returns(p.input);
    p.input = to_csubstr(subject);
    test_filter_src_dst(p);
}

TEST_P(BlockLitFilterTest, filter_inplace_smaller_result)
{
    ParamType const& p = GetParam();
    if(p.input.len >= p.expected.len)
        test_filter_inplace_smaller_result(p);
}
TEST_P(BlockLitFilterTest, filter_inplace_smaller_result_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len >= p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_smaller_result(p);
    }
}

TEST_P(BlockLitFilterTest, filter_inplace_larger_result_spare_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_spare_size(p);
}
TEST_P(BlockLitFilterTest, filter_inplace_larger_result_result_spare_size_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len < p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_larger_result_spare_size(p);
    }
}

TEST_P(BlockLitFilterTest, filter_inplace_larger_result_trimmed_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_trimmed_size(p);
}
TEST_P(BlockLitFilterTest, filter_inplace_larger_result_result_trimmed_size_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len < p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_larger_result_trimmed_size(p);
    }
}

TEST_P(BlockLitFilterTest, filter_inplace_larger_result_insufficient_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_insufficient_size(p);
}
TEST_P(BlockLitFilterTest, filter_inplace_larger_result_result_insufficient_size_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len < p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_larger_result_insufficient_size(p);
    }
}


//-----------------------------------------------------------------------------

blocklit_case test_cases_filter[] = {
#define blc(indentation, chomp, input, output) blocklit_case{indentation, chomp, csubstr(input), csubstr(output)}
    // 0
    blc(2, CHOMP_STRIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    blc(2, CHOMP_CLIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    blc(2, CHOMP_KEEP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n"),
    // 3
    blc(2, CHOMP_STRIP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    blc(2, CHOMP_KEEP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n  \n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n"),
    // 6
    blc(1, CHOMP_STRIP, "", ""),
    blc(1, CHOMP_CLIP, "", ""),
    blc(1, CHOMP_KEEP, "", ""),
    // 9
    blc(1, CHOMP_STRIP, "\n", ""),
    blc(1, CHOMP_CLIP, "\n", ""),
    blc(1, CHOMP_KEEP, "\n", "\n"),
    // 12
    blc(1, CHOMP_STRIP, "\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 15
    blc(1, CHOMP_STRIP, "\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 18
    blc(1, CHOMP_STRIP, "\n\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n\n", "\n\n\n"),
    // 21
    blc(1, CHOMP_STRIP, "\n\n\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n\n\n", "\n\n\n\n"),
    // 24
    blc(1, CHOMP_STRIP, "a", "a"),
    blc(1, CHOMP_CLIP, "a", "a\n"),
    blc(1, CHOMP_KEEP, "a", "a"),
    // 27
    blc(1, CHOMP_STRIP, "a\n", "a"),
    blc(1, CHOMP_CLIP, "a\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n", "a\n"),
    // 30
    blc(1, CHOMP_STRIP, "a\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 33
    blc(0, CHOMP_STRIP, "a\n\n", "a"),
    blc(0, CHOMP_CLIP, "a\n\n", "a\n"),
    blc(0, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 36
    blc(1, CHOMP_STRIP, "a\n\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n\n", "a\n\n\n"),
    // 39
    blc(1, CHOMP_STRIP, "a\n\n\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n\n\n", "a\n\n\n\n"),
    // 42
    blc(1, CHOMP_STRIP, " ab\n \n \n", "ab"),
    blc(1, CHOMP_CLIP, " ab\n \n \n", "ab\n"),
    blc(1, CHOMP_KEEP, " ab\n \n \n", "ab\n\n\n"),
    // 45
    blc(1, CHOMP_STRIP, " ab\n \n  \n", "ab\n\n "),
    blc(1, CHOMP_CLIP, " ab\n \n  \n", "ab\n\n \n"),
    blc(1, CHOMP_KEEP, " ab\n \n  \n", "ab\n\n \n"),
    // 48
    blc(0, CHOMP_STRIP, "ab\n\n \n", "ab\n\n "),
    blc(0, CHOMP_CLIP, "ab\n\n \n", "ab\n\n \n"),
    blc(0, CHOMP_KEEP, "ab\n\n \n", "ab\n\n \n"),
    // 51
    blc(1, CHOMP_STRIP, "hello\nthere\n", "hello\nthere"),
    blc(1, CHOMP_CLIP, "hello\nthere\n", "hello\nthere\n"),
    blc(1, CHOMP_KEEP, "hello\nthere\n", "hello\nthere\n"),
    // 54
    blc(0, CHOMP_STRIP, "hello\nthere\n", "hello\nthere"),
    blc(0, CHOMP_CLIP, "hello\nthere\n", "hello\nthere\n"),
    blc(0, CHOMP_KEEP, "hello\nthere\n", "hello\nthere\n"),
    // 57
    blc(3, CHOMP_CLIP,
        "   There once was a short man from Ealing\n"
        "   Who got on a bus to Darjeeling\n"
        "       It said on the door\n"
        "       \"Please don't spit on the floor\"\n"
        "   So he carefully spat on the ceiling.\n",
        "There once was a short man from Ealing\n"
        "Who got on a bus to Darjeeling\n"
        "    It said on the door\n"
        "    \"Please don't spit on the floor\"\n"
        "So he carefully spat on the ceiling.\n"),
    blc(8, CHOMP_CLIP,
        "        <blockquote style=\"font: italic 12pt Times\">\n"
        "        <p>\"Three is always greater than two,\n"
        "           even for large values of two\"</p>\n"
        "        <p>--Author Unknown</p>\n"
        "        </blockquote>",
        "<blockquote style=\"font: italic 12pt Times\">\n"
        "<p>\"Three is always greater than two,\n"
        "   even for large values of two\"</p>\n"
        "<p>--Author Unknown</p>\n"
        "</blockquote>\n"),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "  \n"
        "  plus another line at the end.\n",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        "\n"
        "plus another line at the end.\n"),
    // 60
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "   \n"
        "  plus another line at the end.",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        " \n"
        "plus another line at the end.\n"),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "    \n"
        "  plus another line at the end.",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        "  \n"
        "plus another line at the end.\n"),
    blc(4, CHOMP_CLIP,
        "    #include \"{{hdr.filename}}\"\n  \n    {{src.gencode}}",
        "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n"),
    // 63
    blc(1, CHOMP_STRIP, " \n  \n   \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n", "\n\n\n"),
    // 66
    blc(1, CHOMP_STRIP, " \n  \n   \n    ", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n    ", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n    ", "\n\n\n"),
    // 69
    blc(1, CHOMP_STRIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n    \n     \n      \n    \n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 72
    blc(1, CHOMP_STRIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n\n     \n      \n\n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 75
    blc(7, CHOMP_STRIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd"),
    blc(7, CHOMP_CLIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n"),
    blc(7, CHOMP_KEEP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n\n\n\n\n\n"),
    // 78
    blc(5, CHOMP_STRIP, "     asd\n     \t ", "asd\n\t "),
    blc(5, CHOMP_CLIP, "     asd\n     \t ", "asd\n\t \n"),
    blc(5, CHOMP_KEEP, "     asd\n     \t ", "asd\n\t "),
    // 81
    blc(5, CHOMP_STRIP, "     asd\n     \t \n", "asd\n\t "),
    blc(5, CHOMP_CLIP, "     asd\n     \t \n", "asd\n\t \n"),
    blc(5, CHOMP_KEEP, "     asd\n     \t \n", "asd\n\t \n"),
    // 84
    blc(5, CHOMP_STRIP, "     asd\n      \t ", "asd\n \t "),
    blc(5, CHOMP_CLIP, "     asd\n      \t ", "asd\n \t \n"),
    blc(5, CHOMP_KEEP, "     asd\n      \t ", "asd\n \t "),
    // 87
    blc(5, CHOMP_STRIP, "     asd\n      \t \n", "asd\n \t "),
    blc(5, CHOMP_CLIP, "     asd\n      \t \n", "asd\n \t \n"),
    blc(5, CHOMP_KEEP, "     asd\n      \t \n", "asd\n \t \n"),
    // 90
    blc(5, CHOMP_CLIP, "     asd\n      ", "asd\n \n"),
    blc(5, CHOMP_CLIP, "     asd\n       ", "asd\n  \n"),
    blc(5, CHOMP_CLIP, "     asd\n     \t ", "asd\n\t \n"),
    // 93
    blc(5, CHOMP_CLIP, "     asd\n      \t", "asd\n \t\n"),
    blc(2, CHOMP_CLIP, " ", ""),
    blc(2, CHOMP_KEEP, " ", "\n"),
    // 96
    blc(2, CHOMP_CLIP, " ", ""),
    blc(2, CHOMP_STRIP, " ", ""),

    #undef blc
};

INSTANTIATE_TEST_SUITE_P(block_literal_filter,
                         BlockLitFilterTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(block_literal, trailing_newlines_0)
{
    std::string yaml = R"(- |
- |
- |
- |
- >
- >
- >
- >
)";
    std::string expected = R"(- |-
- |-
- |-
- |-
- >-
- >-
- >-
- >-
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 8);
        EXPECT_EQ(t[0].val(), "");
        EXPECT_EQ(t[1].val(), "");
        EXPECT_EQ(t[2].val(), "");
        EXPECT_EQ(t[3].val(), "");
        EXPECT_EQ(t[4].val(), "");
        EXPECT_EQ(t[5].val(), "");
        EXPECT_EQ(t[6].val(), "");
        EXPECT_EQ(t[7].val(), "");
        EXPECT_TRUE(t[0].type().is_val_literal());
        EXPECT_TRUE(t[1].type().is_val_literal());
        EXPECT_TRUE(t[2].type().is_val_literal());
        EXPECT_TRUE(t[3].type().is_val_literal());
        EXPECT_TRUE(t[4].type().is_val_folded());
        EXPECT_TRUE(t[5].type().is_val_folded());
        EXPECT_TRUE(t[6].type().is_val_folded());
        EXPECT_TRUE(t[7].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, trailing_newlines_1)
{
    std::string yaml = R"(- |-
  a
- |
  b
- >-
  c
- >
  d
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 4);
        EXPECT_EQ(t[0].val(), "a");
        EXPECT_EQ(t[1].val(), "b\n");
        EXPECT_EQ(t[2].val(), "c");
        EXPECT_EQ(t[3].val(), "d\n");
        EXPECT_TRUE(t[0].type().is_val_literal());
        EXPECT_TRUE(t[1].type().is_val_literal());
        EXPECT_TRUE(t[2].type().is_val_folded());
        EXPECT_TRUE(t[3].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_literal, trailing_newlines_2)
{
    std::string yaml = R"(- |+
- |+
- >+
- >+
)";
    std::string expected = R"(- |-
- |-
- >-
- >-
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 4);
        EXPECT_EQ(t[0].val(), "");
        EXPECT_EQ(t[1].val(), "");
        EXPECT_EQ(t[2].val(), "");
        EXPECT_EQ(t[3].val(), "");
        EXPECT_TRUE(t[0].type().is_val_literal());
        EXPECT_TRUE(t[1].type().is_val_literal());
        EXPECT_TRUE(t[2].type().is_val_folded());
        EXPECT_TRUE(t[3].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, trailing_newlines_3)
{
    std::string yaml = R"(- |+

- |+

- >+

- >+

)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 4);
        EXPECT_EQ(t[0].val(), "\n");
        EXPECT_EQ(t[1].val(), "\n");
        EXPECT_EQ(t[2].val(), "\n");
        EXPECT_EQ(t[3].val(), "\n");
        EXPECT_TRUE(t[0].type().is_val_literal());
        EXPECT_TRUE(t[1].type().is_val_literal());
        EXPECT_TRUE(t[2].type().is_val_folded());
        EXPECT_TRUE(t[3].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_literal, trailing_newlines_4)
{
    std::string yaml = R"(- |+


- |+


- >+


- >+


)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 4);
        EXPECT_EQ(t[0].val(), "\n\n");
        EXPECT_EQ(t[1].val(), "\n\n");
        EXPECT_EQ(t[2].val(), "\n\n");
        EXPECT_EQ(t[3].val(), "\n\n");
        EXPECT_TRUE(t[0].type().is_val_literal());
        EXPECT_TRUE(t[1].type().is_val_literal());
        EXPECT_TRUE(t[2].type().is_val_folded());
        EXPECT_TRUE(t[3].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_literal, trailing_newlines_5_1)
{
    test_check_emit_check(R"(- |
- |)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_literal, trailing_newlines_5_2)
{
    test_check_emit_check(R"(- |-
- |-)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_literal, trailing_newlines_5_3)
{
    test_check_emit_check(R"(- |+
- |+)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_literal, trailing_newlines_5_4)
{
    test_check_emit_check(R"(# no indentation: fails!
- |

- |-

- |+

)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    });
}

TEST(block_literal, trailing_newlines_5_5)
{
    test_check_emit_check(R"(
- |
  
- |-
  
- |+
  
)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    });
}

TEST(block_literal, trailing_newlines_5_6)
{
    test_check_emit_check(R"(
- |
- |-
- |+
)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    });
}


//-----------------------------------------------------------------------------

TEST(block_literal, empty_block_as_container_member)
{
    // this was ok
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
  e:
)", [](Tree const &t){
        EXPECT_TRUE(t["map"].has_key());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_EQ(t["map"].num_children(), 5u);
        for(const auto &child : t["map"].children())
        {
            EXPECT_EQ(child.val(), "");
            if(child.key() != "e")
            {
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
    // this was ok
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
)", [](Tree const &t){
        EXPECT_TRUE(t["map"].has_key());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_EQ(t["map"].num_children(), 4u);
        for(const auto &child : t["map"].children())
        {
            EXPECT_EQ(child.val(), "");
            EXPECT_TRUE(child.type().is_val_quoted());
            EXPECT_FALSE(child.val_is_null());
        }
    });
    // this was not ok! the block literal before next is extended: to
    // include the YAML for next!
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
next:
  a: ""
  b: ''
  c: >
  d: |
)", [](Tree const &t){
        for(const char *name : {"map", "next"})
        {
            ASSERT_TRUE(t.rootref().has_child(to_csubstr(name))) << "name=" << name;
            ConstNodeRef node = t[to_csubstr(name)];
            EXPECT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_map());
            ASSERT_EQ(node.num_children(), 4u);
            for(const auto &child : node.children())
            {
                EXPECT_EQ(child.val(), "");
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
    test_check_emit_check(R"(
seq:
  - ""
  - ''
  - >
  - |
next:
  - ""
  - ''
  - >
  - |
)", [](Tree const &t){
        for(const char *name : {"seq", "next"})
        {
            ASSERT_TRUE(t.rootref().has_child(to_csubstr(name))) << "name=" << name;
            ConstNodeRef node = t[to_csubstr(name)];
            EXPECT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_seq());
            ASSERT_EQ(node.num_children(), 4u);
            for(const auto &child : node.children())
            {
                EXPECT_EQ(child.val(), "");
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
}


TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_1_0)
{
    Tree t;
    NodeRef r = t.rootref();
    r |= SEQ|BLOCK;
    csubstr v0 = "\n";
    csubstr v1 = "\n";
    csubstr v2 = "last";
    r.append_child() = v0;
    r.append_child() = v1;
    r.append_child() = v2;
    test_check_emit_check(t, [&](Tree const& out){
        EXPECT_EQ(out[0].val(), v0);
        EXPECT_EQ(out[1].val(), v1);
        EXPECT_EQ(out[2].val(), v2);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_1)
{
    Tree t;
    NodeRef r = t.rootref();
    r |= SEQ|BLOCK;
    csubstr v0 = "\n\n";
    csubstr v1 = "\n\n";
    csubstr v2 = "last";
    r.append_child() = v0;
    r.append_child() = v1;
    r.append_child() = v2;
    test_check_emit_check(t, [&](Tree const& out){
        EXPECT_EQ(out[0].val(), v0);
        EXPECT_EQ(out[1].val(), v1);
        EXPECT_EQ(out[2].val(), v2);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2_0)
{
    csubstr yaml = R"(--- |+
ab)";
    std::string expected = R"(--- |-
  ab
)";
    test_check_emit_check(yaml, [&](Tree const& t){
        EXPECT_EQ(t.docref(0).val(), csubstr("ab"));
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2_1)
{
    csubstr yaml = R"(--- |+
ab
)";
    std::string expected = R"(--- |
  ab
)";
    test_check_emit_check(yaml, [&](Tree const& t){
        EXPECT_EQ(t.docref(0).val(), csubstr("ab\n"));
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2_2)
{
    csubstr yaml = R"(--- |+
ab

)";
    std::string expected = R"(--- |+
  ab

)";
    test_check_emit_check(yaml, [&](Tree const& t){
        EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n"));
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2_3)
{
    csubstr yaml = R"(--- |+
ab


)";
    std::string expected = R"(--- |+
  ab


)";
    test_check_emit_check(yaml, [&](Tree const& t){
        EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n\n"));
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2_4)
{
    csubstr yaml = R"(--- |+
  ab



)";
    std::string expected = R"(--- |+
  ab



)";
    test_check_emit_check(yaml, [&](Tree const& t){
        EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n\n\n"));
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
    EXPECT_EQ(csubstr("ab\n\n \n").trimr(" \t\n"), csubstr("ab"));
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_3)
{
    std::string yaml = R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.



- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

- last
)";
    std::string expected = R"(- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.

- last
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
        EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_literal, carriage_return)
{
    std::string yaml = "with: |\r\n"
"  text\r\n"
"   	lines\r\n"
"without: |\n"
"  text\n"
"   	lines\n";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
        EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
    });
}

#ifdef JAVAI
TEST(block_literal, errors_on_tab_indents)
{
    Tree tree;
    ExpectError::check_error(&tree, [&]{
        parse_in_arena("foo: |4\n    this is foo\n    now with tab-\n \t \tmust not work\n", &tree);
    });
}
#endif

TEST(block_literal, error_on_bad_spec_0)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("- |012abc\n  must have errors above\n");
    });
}

TEST(block_literal, error_on_bad_spec_1)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("- |0\n  a\n");
    });
}

TEST(block_literal, error_on_literal_in_seqflow)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("[\n  |\n    a\n,]");
    });
}

TEST(block_literal, error_on_literal_in_mapflow)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("{\n b: |\n    a\n,}");
    });
}

TEST(block_literal, indentation_indicator_0)
{
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("|0");
        });
    }
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("|10");
        });
    }
    {
        Tree t = parse_in_arena("|1");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|3");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|4");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|5");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|6");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|7");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|8");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena("|9");
        EXPECT_EQ(t.rootref().val(), "");
    }
}

TEST(block_literal, indentation_indicator_1)
{
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("--- |0");
        });
    }
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("--- |10");
        });
    }
    {
        Tree t = parse_in_arena("--- |1");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |3");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |4");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |5");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |6");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |7");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |8");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- |9");
        EXPECT_EQ(t.docref(0).val(), "");
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_LITERAL)
{

ADD_CASE_TO_GROUP("trailing newlines 0_0",
R"(- |
- |

- |


- |



- |




- |





)",
N(SB, L{
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
})
);

ADD_CASE_TO_GROUP("trailing newlines 0_1",
R"(- |-
- |-

- |-


- |-



- |-




- |-





)",
N(SB, L{
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
})
);

ADD_CASE_TO_GROUP("trailing newlines 0_2",
R"(- |+
- |+

- |+


- |+



- |+




- |+





)",
N(SB, L{
  N(VL, ""),
  N(VL, "\n"),
  N(VL, "\n\n"),
  N(VL, "\n\n\n"),
  N(VL, "\n\n\n\n"),
  N(VL, "\n\n\n\n\n"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_0",
R"(- |
  a
- |
  b

- |
  c


- |
  d



- |
  e




- |
  f





)",
N(SB, L{
  N(VL, "a\n"),
  N(VL, "b\n"),
  N(VL, "c\n"),
  N(VL, "d\n"),
  N(VL, "e\n"),
  N(VL, "f\n"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_1",
R"(- |-
  a
- |-
  b

- |-
  c


- |-
  d



- |-
  e




- |-
  f





)",
N(SB, L{
  N(VL, "a"),
  N(VL, "b"),
  N(VL, "c"),
  N(VL, "d"),
  N(VL, "e"),
  N(VL, "f"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_2",
R"(- |+
  a
- |+
  b

- |+
  c


- |+
  d



- |+
  e




- |+
  f





)",
N(SB, L{
  N(VL, "a\n"),
  N(VL, "b\n\n"),
  N(VL, "c\n\n\n"),
  N(VL, "d\n\n\n\n"),
  N(VL, "e\n\n\n\n\n"),
  N(VL, "f\n\n\n\n\n\n"),
})
);

ADD_CASE_TO_GROUP("trailing whitespace", R"(
|
 a
  
   
    
)",
N(VL, "a\n \n  \n   \n")
);

ADD_CASE_TO_GROUP("indentation requirements",
R"(---
|
hello0
there
---
|
 hello1
 there
---
|
  hello2
  there
---
|
ciao3
qua
---
|
    ciao4
    qua
---
|
      ciao5
      qua
---
- |
 hello6
 there
- |
 ciao7
 qua
---
foo: |
 hello8
 there
bar: |
 ciao9
 qua
)",
N(STREAM, L{
        N(DOC|VL, "hello0\nthere\n"),
        N(DOC|VL, "hello1\nthere\n"),
        N(DOC|VL, "hello2\nthere\n"),
        N(DOC|VL, "ciao3\nqua\n"),
        N(DOC|VL, "ciao4\nqua\n"),
        N(DOC|VL, "ciao5\nqua\n"),
        N(DOC|SB, L{N(VL, "hello6\nthere\n"), N(VL, "ciao7\nqua\n")}),
        N(DOC|MB, L{N(KP|VL, "foo", "hello8\nthere\n"), N(KP|VL, "bar", "ciao9\nqua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err, 0", EXPECT_PARSE_ERROR,
R"(|0)",
LineCol(1, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err, 0.1", EXPECT_PARSE_ERROR,
R"(|0
)",
LineCol(1, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- |
hello
there
- |
ciao
qua
)",
LineCol(2, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: |
hello
there
bar: |
ciao
qua
)",
LineCol(2, 6)
);

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- |2
 hello
 there
)");

ADD_CASE_TO_GROUP("empty, specs only 2G84_02",
"--- |1-",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03",
"--- |1+",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx",
"--- |+",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02_1",
"|1-",
N(VL, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03_1",
"|1+",
N(VL, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx_1",
"|+",
N(VL, {}));

ADD_CASE_TO_GROUP("block literal as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       "Please don't spit on the floor"
   So he carefully spat on the ceiling
)",
  N(MB, {
     N(KP|VL, "data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n")
      })
);

ADD_CASE_TO_GROUP("block literal and two scalars",
R"(
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style="font: italic 12pt Times">
        <p>"Three is always greater than two,
           even for large values of two"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     N(MB, L{
          N(KP|VF, "example", "HTML goes into YAML without modification\n"),
          N(KP|VL, "message", R"(<blockquote style="font: italic 12pt Times">
<p>"Three is always greater than two,
   even for large values of two"</p>
<p>--Author Unknown</p>
</blockquote>
)"),
          N(KP|VP, "date", "2007-06-01"),
              })
);

ADD_CASE_TO_GROUP("block literal no chomp, no indentation",
R"(example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     N(MB, L{
      N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
      N(KP|VP, "another", "text"),
          })
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(VP, "another val")
  })
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=keep",
R"(
- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N(VP, "another val")
  })
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=strip",
R"(
- |-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N(VP, "another val")
  })
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 4",
R"(
- |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2",
R"(
example: |2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=keep",
R"(
example: |+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=strip",
R"(
example: |-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 3",
R"(
example: |
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 3",
R"(
example: |3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 4",
R"(
example: |4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 9",
R"(
example: |
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 9",
R"(
example: |9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, without quotes",
    R"(tpl:
  src: |
    #include <{{hdr.filename}}>
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include <{{hdr.filename}}>\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with double quotes",
    R"(tpl:
  src: |
    #include "{{hdr.filename}}"
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with single quotes",
    R"(tpl:
  src: |
    #include '{{hdr.filename}}'
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include '{{hdr.filename}}'\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with same indentation level 0",
R"(
aaa: |2
  xxx
bbb: |
  yyy
)",
N(MB, L{
  N(KP|VL, "aaa", "xxx\n"),
  N(KP|VL, "bbb", "yyy\n")
})
);

ADD_CASE_TO_GROUP("block literal with same indentation level 1",
R"(
- aaa: |2
    xxx
  bbb: |
    yyy
)",
N(SB, L{
  N(MB, L{
    N(KP|VL, "aaa", "xxx\n"),
    N(KP|VL, "bbb", "yyy\n"),
  })
})
);

ADD_CASE_TO_GROUP("block literal with tab and spaces",
R"(|
       	  )",
  N(VL, "\t  \n")
    );


ADD_CASE_TO_GROUP("block literal with empty docval 1",
R"(|)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 2",
R"(|
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 3",
R"(|
  )",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 4",
R"(|
  
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 5",
R"(|
    
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 8",
R"(|


)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 9",
R"(|



)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 10",
R"(|




)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 11",
R"(|
 
  
   
    )",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 12",
R"(|
 
  
   
    
     
      
    
   
 
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 13",
R"(|
 
  

   

    

)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0",
R"(- |+
)",
  N(SB, L{N(VL, "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.1",
R"(- |+
 )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.2",
R"(- |+
   )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.3",
R"(- |+

- |+

)",
  N(SB, L{N(VL, "\n"), N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.4",
R"(- |+


- |+


)",
  N(SB, L{N(VL, "\n\n"), N(VL, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.5",
R"(- |+



- |+



)",
  N(SB, L{N(VL, "\n\n\n"), N(VL, "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1",
R"(foo: |+
)",
  N(MB, L{N(KP|VL, "foo", "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.1",
R"(foo: |+
 )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.2",
R"(foo: |+
  )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2",
R"(|+
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.1",
R"(|+
 )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.2",
R"(|+
   )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0",
R"(- |+

)",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0.1",
R"(- |+

  )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1",
R"(foo: |+

)",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1.1",
R"(foo: |+

  )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2",
R"(|+

)",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2.1",
R"(|+

  )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16",
R"(|+


)",
  N(VL, "\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.1",
R"(foo: |+


)",
  N(MB, L{N(KP|VL, "foo", "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.2",
R"(- |+


)",
  N(SB, L{N(VL, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17",
R"(|+



)",
  N(VL, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.1",
R"(foo: |+



)",
  N(MB, L{N(KP|VL, "foo", "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.2",
R"(- |+



)",
  N(SB, L{N(VL, "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 0",
R"(|
  asd)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1",
R"(|
  asd
)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.1",
R"(|
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.2",
R"(|+
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 2",
R"(|
  asd

)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 3",
R"(|
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 4",
R"(|
  asd
  
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5",
R"(|
     asd
   
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.1",
R"(|
       asd
     
   
     
  
 
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.2",
R"(|
       asd
     
   
       
  
 
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.3",
R"(|
       asd
     
   
        
  
 
  )",
  N(VL, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 6",
R"(|
     asd
      )",
  N(VL, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 7",
R"(|
     asd
      
)",
  N(VL, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 8",
R"(|
     asd
       )",
  N(VL, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 9",
R"(|
     asd
       
)",
  N(VL, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 10",
R"(|
     asd
     	 )",
  N(VL, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 11",
R"(|
     asd
      	 )",
  N(VL, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 12",
R"(|
     asd
     	 
)",
  N(VL, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 13",
R"(|
     asd
      	 
)",
  N(VL, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 0",
R"(- |+
  
- |+
  )",
N(SB, L{N(VL, "\n"), N(VL, "\n"),}));

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 1",
R"(- |+
  
- |+
  
)",
N(SB, L{N(VL, "\n"), N(VL, "\n"),}));

}


} // namespace yml
} // namespace c4
