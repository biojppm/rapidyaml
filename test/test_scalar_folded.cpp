#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

struct blockfolded_case
{
    size_t indentation;
    BlockChomp_e chomp;
    csubstr input, expected;
};

void test_filter_src_dst(blockfolded_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_;
    subject_.resize(2 * blcase.input.size());
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_block_folded(blcase.input, dst, blcase.indentation, blcase.chomp);
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

void test_filter_inplace_smaller_result(blockfolded_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_block_folded_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    csubstr sresult = parser2._filter_scalar_folded(to_substr(subject_2), blcase.indentation, blcase.chomp);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    const csubstr out = result.get();
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_sub(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
}

void test_filter_inplace_larger_result_spare_size(blockfolded_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    subject_.resize(blcase.expected.len + 30);
    c4::substr dst = to_substr(subject_).first(blcase.input.len);
    c4::substr rem = to_substr(subject_).sub(blcase.expected.len);
    rem.fill('^');
    Parser::handler_type event_handler = {};
    Parser parser1(&event_handler);
    FilterResult result = parser1.filter_scalar_block_folded_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_folded(to_substr(subject_2), blcase.indentation, blcase.chomp);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    const csubstr out = result.get();
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

void test_filter_inplace_larger_result_trimmed_size(blockfolded_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    subject_.resize(blcase.expected.len);
    c4::substr dst = to_substr(subject_).first(blcase.input.len);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_block_folded_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_TRUE(result.valid());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_folded(to_substr(subject_2), blcase.indentation, blcase.chomp);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    const csubstr out = result.get();
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.str);
    EXPECT_TRUE(out.is_super(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
}

void test_filter_inplace_larger_result_insufficient_size(blockfolded_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_(blcase.input.str, blcase.input.len);
    std::string subject_2 = subject_;
    c4::substr dst = to_substr(subject_);
    Parser::handler_type event_handler = {};
    Parser proc(&event_handler);
    FilterResult result = proc.filter_scalar_block_folded_in_place(dst, subject_.size(), blcase.indentation, blcase.chomp);
    ASSERT_FALSE(result.valid());
    Parser::handler_type event_handler2 = {};
    Parser parser2(&event_handler2);
    Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
    parser2.m_evt_handler->m_tree = &tree;
    csubstr sresult = parser2._filter_scalar_folded(to_substr(subject_2), blcase.indentation, blcase.chomp);
    _c4prscalar("sresult:", sresult, true);
    EXPECT_GE(result.required_len(), blcase.expected.len);
    EXPECT_EQ(sresult.len, result.str.len);
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(result.required_len(), blcase.expected.len);
    }
    ASSERT_FALSE(result.valid());
}


//-----------------------------------------------------------------------------

struct BlockFoldedFilterTest : public ::testing::TestWithParam<blockfolded_case>
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

TEST_P(BlockFoldedFilterTest, filter_src_dst)
{
    test_filter_src_dst(GetParam());
}
TEST_P(BlockFoldedFilterTest, filter_src_dst_carriage_return)
{
    ParamType p = GetParam();
    std::string subject = add_carriage_returns(p.input);
    p.input = to_csubstr(subject);
    test_filter_src_dst(p);
}
TEST_P(BlockFoldedFilterTest, filter_inplace_smaller_result)
{
    ParamType const& p = GetParam();
    if(p.input.len >= p.expected.len)
        test_filter_inplace_smaller_result(p);
}
TEST_P(BlockFoldedFilterTest, filter_inplace_smaller_result_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len >= p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_smaller_result(p);
    }
}

TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_spare_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_spare_size(p);
}
TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_result_spare_size_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len < p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_larger_result_spare_size(p);
    }
}

TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_trimmed_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_trimmed_size(p);
}
TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_result_trimmed_size_carriage_return)
{
    ParamType p = GetParam();
    if(p.input.len < p.expected.len)
    {
        std::string subject = add_carriage_returns(p.input);
        p.input = to_csubstr(subject);
        test_filter_inplace_larger_result_trimmed_size(p);
    }
}

TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_insufficient_size)
{
    ParamType const& p = GetParam();
    if(p.input.len < p.expected.len)
        test_filter_inplace_larger_result_insufficient_size(p);
}
TEST_P(BlockFoldedFilterTest, filter_inplace_larger_result_result_insufficient_size_carriage_return)
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

blockfolded_case test_cases_filter[] = {
#define bfc(indentation, chomp, input, output) blockfolded_case{indentation, chomp, csubstr(input), csubstr(output)}
    // 0
    bfc(2, CHOMP_STRIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    bfc(2, CHOMP_CLIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    bfc(2, CHOMP_KEEP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n"),
    // 3
    bfc(2, CHOMP_STRIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    bfc(2, CHOMP_CLIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    bfc(2, CHOMP_KEEP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n"),
    // 6
    bfc(1, CHOMP_STRIP, "", ""),
    bfc(1, CHOMP_CLIP, "", ""),
    bfc(1, CHOMP_KEEP, "", ""),
    // 9
    bfc(1, CHOMP_STRIP, "\n", ""),
    bfc(1, CHOMP_CLIP, "\n", ""),
    bfc(1, CHOMP_KEEP, "\n", "\n"),
    // 12
    bfc(1, CHOMP_STRIP, "\n\n", ""),
    bfc(1, CHOMP_CLIP, "\n\n", ""),
    bfc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 15
    bfc(1, CHOMP_STRIP, "\n\n", ""),
    bfc(1, CHOMP_CLIP, "\n\n", ""),
    bfc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 18
    bfc(1, CHOMP_STRIP, "\n\n\n", ""),
    bfc(1, CHOMP_CLIP, "\n\n\n", ""),
    bfc(1, CHOMP_KEEP, "\n\n\n", "\n\n\n"),
    // 21
    bfc(1, CHOMP_STRIP, "\n\n\n\n", ""),
    bfc(1, CHOMP_CLIP, "\n\n\n\n", ""),
    bfc(1, CHOMP_KEEP, "\n\n\n\n", "\n\n\n\n"),
    // 24
    bfc(1, CHOMP_STRIP, "a", "a"),
    bfc(1, CHOMP_CLIP, "a", "a\n"),
    bfc(1, CHOMP_KEEP, "a", "a"),
    // 27
    bfc(1, CHOMP_STRIP, "a\n", "a"),
    bfc(1, CHOMP_CLIP, "a\n", "a\n"),
    bfc(1, CHOMP_KEEP, "a\n", "a\n"),
    // 30
    bfc(1, CHOMP_STRIP, "a\n\n", "a"),
    bfc(1, CHOMP_CLIP, "a\n\n", "a\n"),
    bfc(1, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 33
    bfc(0, CHOMP_STRIP, "a\n\n", "a"),
    bfc(0, CHOMP_CLIP, "a\n\n", "a\n"),
    bfc(0, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 36
    bfc(1, CHOMP_STRIP, "a\n\n\n", "a"),
    bfc(1, CHOMP_CLIP, "a\n\n\n", "a\n"),
    bfc(1, CHOMP_KEEP, "a\n\n\n", "a\n\n\n"),
    // 39
    bfc(1, CHOMP_STRIP, "a\n\n\n\n", "a"),
    bfc(1, CHOMP_CLIP, "a\n\n\n\n", "a\n"),
    bfc(1, CHOMP_KEEP, "a\n\n\n\n", "a\n\n\n\n"),
    // 42
    bfc(1, CHOMP_STRIP, " ab\n \n \n", "ab"),
    bfc(1, CHOMP_CLIP, " ab\n \n \n", "ab\n"),
    bfc(1, CHOMP_KEEP, " ab\n \n \n", "ab\n\n\n"),
    // 45
    bfc(1, CHOMP_STRIP, " ab\n \n  \n", "ab\n\n "),
    bfc(1, CHOMP_CLIP, " ab\n \n  \n", "ab\n\n \n"),
    bfc(1, CHOMP_KEEP, " ab\n \n  \n", "ab\n\n \n"),
    // 48
    bfc(0, CHOMP_STRIP, "ab\n\n \n", "ab\n\n "),
    bfc(0, CHOMP_CLIP, "ab\n\n \n", "ab\n\n \n"),
    bfc(0, CHOMP_KEEP, "ab\n\n \n", "ab\n\n \n"),
    // 51
    bfc(1, CHOMP_STRIP, "hello\nthere\n", "hello there"),
    bfc(1, CHOMP_CLIP, "hello\nthere\n", "hello there\n"),
    bfc(1, CHOMP_KEEP, "hello\nthere\n", "hello there\n"),
    // 54
    bfc(0, CHOMP_STRIP, "hello\nthere\n", "hello there"),
    bfc(0, CHOMP_CLIP, "hello\nthere\n", "hello there\n"),
    bfc(0, CHOMP_KEEP, "hello\nthere\n", "hello there\n"),
    // 57
    bfc(3, CHOMP_CLIP,
        "   There once was a short man from Ealing\n"
        "   Who got on a bus to Darjeeling\n"
        "       It said on the door\n"
        "       \"Please don't spit on the floor\"\n"
        "   So he carefully spat on the ceiling.\n",
        "There once was a short man from Ealing "
        "Who got on a bus to Darjeeling\n"
        "    It said on the door\n"
        "    \"Please don't spit on the floor\"\n"
        "So he carefully spat on the ceiling.\n"),
    bfc(3, CHOMP_CLIP,
        "   There once was a short man from Ealing\n"
        "   Who got on a bus to Darjeeling\n"
        "       It said on the door\n"
        "       extra 0\n"
        "         extra 2\n"
        "         extra 2\n"
        "           extra 4\n"
        "           extra 4\n"
        "           extra 4\n"
        "         extra 2\n"
        "       extra 0\n"
        "       \"Please don't spit on the floor\"\n"
        "   So he carefully spat on the ceiling.\n",
        "There once was a short man from Ealing "
        "Who got on a bus to Darjeeling\n"
        "    It said on the door\n"
        "    extra 0\n"
        "      extra 2\n"
        "      extra 2\n"
        "        extra 4\n"
        "        extra 4\n"
        "        extra 4\n"
        "      extra 2\n"
        "    extra 0\n"
        "    \"Please don't spit on the floor\"\n"
        "So he carefully spat on the ceiling.\n"),
    bfc(1, CHOMP_STRIP, " \n  \n   \n", ""),
    // 60
    bfc(1, CHOMP_STRIP, " \n  \n   \n", ""),
    bfc(1, CHOMP_CLIP, " \n  \n   \n", ""),
    bfc(1, CHOMP_KEEP, " \n  \n   \n", "\n\n\n"),
    // 63
    bfc(1, CHOMP_STRIP, " \n  \n   \n    ", ""),
    bfc(1, CHOMP_CLIP, " \n  \n   \n    ", ""),
    bfc(1, CHOMP_KEEP, " \n  \n   \n    ", "\n\n\n"),
    // 66
    bfc(1, CHOMP_STRIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    bfc(1, CHOMP_CLIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    bfc(1, CHOMP_KEEP, " \n  \n   \n    \n     \n      \n    \n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 69
    bfc(1, CHOMP_STRIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    bfc(1, CHOMP_CLIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    bfc(1, CHOMP_KEEP, " \n  \n   \n\n     \n      \n\n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 72
    bfc(7, CHOMP_STRIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd"),
    bfc(7, CHOMP_CLIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n"),
    bfc(7, CHOMP_KEEP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n\n\n\n\n\n"),
    // 75
    bfc(5, CHOMP_STRIP, "     asd\n     \t ", "asd\n\t "),
    bfc(5, CHOMP_CLIP, "     asd\n     \t ", "asd\n\t \n"),
    bfc(5, CHOMP_KEEP, "     asd\n     \t ", "asd\n\t "),
    // 78
    bfc(5, CHOMP_STRIP, "     asd\n     \t \n", "asd\n\t "),
    bfc(5, CHOMP_CLIP, "     asd\n     \t \n", "asd\n\t \n"),
    bfc(5, CHOMP_KEEP, "     asd\n     \t \n", "asd\n\t \n"),
    // 81
    bfc(5, CHOMP_STRIP, "     asd\n      \t ", "asd\n \t "),
    bfc(5, CHOMP_CLIP, "     asd\n      \t ", "asd\n \t \n"),
    bfc(5, CHOMP_KEEP, "     asd\n      \t ", "asd\n \t "),
    // 84
    bfc(5, CHOMP_STRIP, "     asd\n      \t \n", "asd\n \t "),
    bfc(5, CHOMP_CLIP, "     asd\n      \t \n", "asd\n \t \n"),
    bfc(5, CHOMP_KEEP, "     asd\n      \t \n", "asd\n \t \n"),
    // 87
    bfc(2, CHOMP_CLIP, "\n  foo\n  bar\n", "\nfoo bar\n"),
    bfc(2, CHOMP_CLIP, "\n\n  foo\n  bar\n", "\n\nfoo bar\n"),
    bfc(2, CHOMP_CLIP, "\n\n\n  foo\n  bar\n", "\n\n\nfoo bar\n"),
    // 90
    bfc(1, CHOMP_CLIP,
        " folded\n"
        " line\n"
        "\n"
        " next\n"
        " line\n"
        "   * bullet\n"
        "\n"
        "   * list\n"
        "   * lines\n"
        "\n"
        " last\n"
        " line\n",
        "folded line\n"
        "next line\n"
        "  * bullet\n"
        "\n"
        "  * list\n"
        "  * lines\n"
        "\n"
        "last line\n"
        ""),
    bfc(1, CHOMP_CLIP,
        " \n"
        "  \n"
        "  literal\n"
        "   \n"
        "  \n"
        "  text\n"
        "",
        "\n"
        " \n"
        " literal\n"
        "  \n"
        " \n"
        " text\n"),
    bfc(2, CHOMP_CLIP,
        " \n"
        "  \n"
        "  literal\n"
        "   \n"
        "  \n"
        "  text\n"
        "",
        "\n"
        "\n"
        "literal\n"
        " \n"
        "\n"
        "text\n"),
    // 93
    bfc(5, CHOMP_CLIP, "     asd\n      ", "asd\n \n"),
    bfc(5, CHOMP_CLIP, "     asd\n       ", "asd\n  \n"),
    bfc(5, CHOMP_CLIP, "     asd\n     \t ", "asd\n\t \n"),
    // 96
    bfc(5, CHOMP_CLIP, "     asd\n     \t \n", "asd\n\t \n"),
    bfc(5, CHOMP_CLIP, "     asd\n      \t", "asd\n \t\n"),
    bfc(5, CHOMP_CLIP, "     asd\n      \t\n", "asd\n \t\n"),
    // 99
    bfc(1, CHOMP_CLIP,
        " Sammy Sosa completed another\n"
        " fine season with great stats.\n"
        "\n"
        " 63 Home Runs\n"
        " 0.288 Batting Average\n"
        "\n"
        " What a year!\n"
        ,
        "Sammy Sosa completed another fine season with great stats.\n"
        "63 Home Runs 0.288 Batting Average\n"
        "What a year!\n"
        ),
    bfc(1, CHOMP_CLIP,
        " Sammy Sosa completed another\n"
        " fine season with great stats.\n"
        "   63 Home Runs\n"
        "   0.288 Batting Average\n"
        " What a year!\n"
        ,
        "Sammy Sosa completed another fine season with great stats.\n"
        "  63 Home Runs\n"
        "  0.288 Batting Average\n"
        "What a year!\n"
        ),
    bfc(1, CHOMP_CLIP,
        " Sammy Sosa completed another\n"
        " fine season with great stats.\n"
        "\n"
        "   63 Home Runs\n"
        "   0.288 Batting Average\n"
        "\n"
        " What a year!\n"
        ,
        "Sammy Sosa completed another fine season with great stats.\n"
        "\n"
        "  63 Home Runs\n"
        "  0.288 Batting Average\n"
        "\n"
        "What a year!\n"
        ),
    // 102
    bfc(1, CHOMP_CLIP,
        " Sammy Sosa completed another\n"
        " fine season with great stats.\n"
        "\n"
        "\n"
        "   63 Home Runs\n"
        "   0.288 Batting Average\n"
        "\n"
        "\n"
        " What a year!\n"
        ,
        "Sammy Sosa completed another fine season with great stats.\n"
        "\n"
        "\n"
        "  63 Home Runs\n"
        "  0.288 Batting Average\n"
        "\n"
        "\n"
        "What a year!\n"
        ),
    bfc(1, CHOMP_CLIP,
        " Sammy Sosa completed another\n"
        " fine season with great stats.\n"
        "\n"
        "\n"
        "\n"
        "   63 Home Runs\n"
        "   0.288 Batting Average\n"
        "\n"
        "\n"
        "\n"
        " What a year!\n"
        ,
        "Sammy Sosa completed another fine season with great stats.\n"
        "\n"
        "\n"
        "\n"
        "  63 Home Runs\n"
        "  0.288 Batting Average\n"
        "\n"
        "\n"
        "\n"
        "What a year!\n"
        ),
    bfc(2, CHOMP_CLIP,
        "   more indented\n"
        "  regular\n"
        ,
        " more indented\n"
        "regular\n"
        ),
    // 105
    bfc(2, CHOMP_CLIP,
        "\n"
        "\n"
        "   more indented\n"
        "  regular\n"
        ,
        "\n"
        "\n"
        " more indented\n"
        "regular\n"
        ),
    bfc(2, CHOMP_STRIP,
        "this is the multiline\n  \"val\" with\n\n  'empty' lines",
        "this is the multiline \"val\" with\n'empty' lines"
        ),
    #undef blc
};

INSTANTIATE_TEST_SUITE_P(block_folded_filter,
                         BlockFoldedFilterTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(block_folded, trailing_newlines_0)
{
    std::string yaml = R"(- >
- >
- >
- >
)";
    std::string expected = R"(- >-
- >-
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
        EXPECT_TRUE(t[0].type().is_val_folded());
        EXPECT_TRUE(t[1].type().is_val_folded());
        EXPECT_TRUE(t[2].type().is_val_folded());
        EXPECT_TRUE(t[3].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_folded, trailing_newlines_1)
{
    std::string yaml = R"(- >-
  a
- >
  b
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 2);
        EXPECT_EQ(t[0].val(), "a");
        EXPECT_EQ(t[1].val(), "b\n");
        EXPECT_TRUE(t[0].type().is_val_folded());
        EXPECT_TRUE(t[1].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_folded, trailing_newlines_2)
{
    std::string yaml = R"(- >+
- >+
)";
    std::string expected = R"(- >-
- >-
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 2);
        EXPECT_EQ(t[0].val(), "");
        EXPECT_EQ(t[1].val(), "");
        EXPECT_TRUE(t[0].type().is_val_folded());
        EXPECT_TRUE(t[1].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, expected);
    });
}

TEST(block_folded, trailing_newlines_3)
{
    std::string yaml = R"(- >+

- >+

)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 2);
        EXPECT_EQ(t[0].val(), "\n");
        EXPECT_EQ(t[1].val(), "\n");
        EXPECT_TRUE(t[0].type().is_val_folded());
        EXPECT_TRUE(t[1].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_folded, trailing_newlines_4)
{
    std::string yaml = R"(- >+


- >+


)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const& t){
        ConstNodeRef root = t.rootref();
        ASSERT_TRUE(root.is_seq());
        ASSERT_EQ(root.num_children(), 2);
        EXPECT_EQ(t[0].val(), "\n\n");
        EXPECT_EQ(t[1].val(), "\n\n");
        EXPECT_TRUE(t[0].type().is_val_folded());
        EXPECT_TRUE(t[1].type().is_val_folded());
        std::string out = emitrs_yaml<std::string>(t);
        EXPECT_EQ(out, yaml);
    });
}

TEST(block_folded, trailing_newlines_5_1)
{
    test_check_emit_check(R"(- >
- >)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_folded, trailing_newlines_5_2)
{
    test_check_emit_check(R"(- >-
- >-)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_folded, trailing_newlines_5_3)
{
    test_check_emit_check(R"(- >+
- >+)", [&](Tree const& t){
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
    });
}

TEST(block_folded, trailing_newlines_5_4)
{
    test_check_emit_check(R"(# no indentation: fails!
- >

- >-

- >+

)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    });
}

TEST(block_folded, trailing_newlines_5_5)
{
    test_check_emit_check(R"(
- >
  
- >-
  
- >+
  
)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    });
}

TEST(block_folded, trailing_newlines_5_6)
{
    test_check_emit_check(R"(
- >
- >-
- >+
)", [&](Tree const& t){
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    });
}


//-----------------------------------------------------------------------------

TEST(block_folded, basic)
{
    {
        const Tree t = parse_in_arena(R"(>
hello
there

got it


really
)");
        EXPECT_EQ(t.rootref().val(), csubstr("hello there\ngot it\n\nreally\n"));
    }
}

TEST(block_folded, empty_block)
{
    {
        const Tree t = parse_in_arena(R"(- >
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        const Tree t = parse_in_arena(R"(- >-
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        const Tree t = parse_in_arena(R"(- >+
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        const Tree t = parse_in_arena(R"(
- >

- >-

- >+

)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        const Tree t = parse_in_arena(R"(
- >
  
- >-
  
- >+
  
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        const Tree t = parse_in_arena(R"(
- >
- >-
- >+
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    }
}

TEST(block_folded, empty_block0)
{
    Tree t = parse_in_arena(R"(- >
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
    t = parse_in_arena(R"(- >-
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
    t = parse_in_arena(R"(- >+
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
}

TEST(block_folded, empty_block1)
{
    const Tree t = parse_in_arena(R"(
- >-
  a
- >-
  
- >-
   
- >-
  
  
  
- >-
  
   
  
)");
    EXPECT_EQ(t[0].val(), csubstr("a"));
    EXPECT_EQ(t[1].val(), csubstr(""));
    EXPECT_EQ(t[2].val(), csubstr(""));
    EXPECT_EQ(t[3].val(), csubstr(""));
    EXPECT_EQ(t[4].val(), csubstr(""));
}

TEST(block_folded, empty_block_as_container_member)
{
    // this was ok
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  d: |
  c: >
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
  d: |
  c: >
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
  d: |
  c: >
next:
  a: ""
  b: ''
  d: |
  c: >
)", [](Tree const &t){
        for(const char *name : {"map", "next"})
        {
            ASSERT_TRUE(t.rootref().has_child(to_csubstr(name))) << "name=" << name;
            ConstNodeRef node = t[to_csubstr(name)];
            EXPECT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_map());
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
  - |
  - >
next:
  - ""
  - ''
  - |
  - >
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

TEST(block_folded, issue152_not_indented)
{
    const Tree t = parse_in_arena(R"(
ok:
  - |
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  - parses - yes
ok_parses: yes
err:
  - |
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err_parses: no
err2:
  - >
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err2_parses: no
err3:
  - >-
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err3_parses: no
)");
    EXPECT_EQ(t["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_once)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  ok:
    - |
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    - parses - yes
  ok_parses: yes
  err:
    - |
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err_parses: no
  err2:
    - >
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err2_parses: no
  err3:
    - >-
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err3_parses: no
)");
    ConstNodeRef n = t["indented_once"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_twice)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  indented_twice:
    ok:
      - |
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      - parses - yes
    ok_parses: yes
    err:
      - |
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err_parses: no
    err2:
      - >
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err2_parses: no
    err3:
      - >-
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err3_parses: no
)");
    ConstNodeRef n = t["indented_once"]["indented_twice"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_thrice)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  indented_twice:
    indented_thrice:
      ok:
        - |
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
        - parses - yes
      ok_parses: yes
      err:
        - |
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err_parses: no
      err2:
        - >
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err2_parses: no
      err3:
        - >-
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err3_parses: no
)");
    ConstNodeRef n = t["indented_once"]["indented_twice"]["indented_thrice"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}


//-----------------------------------------------------------------------------

TEST(block_folded, test_suite_4QFQ_0)
{
    csubstr yaml = R"(- |1
  child2
- |2
   child2
- |3
    child2
- ' child2

'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        const csubstr v = " child2\n";
        EXPECT_EQ(t[0].val(), v);
        EXPECT_EQ(t[1].val(), v);
        EXPECT_EQ(t[2].val(), v);
        EXPECT_EQ(t[3].val(), v);
    });
}

TEST(block_folded, test_suite_4QFQ_1)
{
    csubstr yaml = R"(- >1
  child2
- >2
   child2
- >3
    child2
- ' child2

'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        const csubstr v = " child2\n";
        EXPECT_EQ(t[0].val(), v);
        EXPECT_EQ(t[1].val(), v);
        EXPECT_EQ(t[2].val(), v);
        EXPECT_EQ(t[3].val(), v);
    });
}
TEST(block_folded, test_suite_4QFQ_2)
{
    std::string yaml = R"(>
 
  
  # child1
)";
    std::string emitted = R"(>


  # child1
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        ConstNodeRef n = t.rootref();
        EXPECT_EQ(n.val(), csubstr("\n\n# child1\n"));
        EXPECT_EQ(emitrs_yaml<std::string>(t), emitted);
    });
}
TEST(block_folded, test_suite_4QFQ_3)
{
    csubstr yaml = R"(- |
 child0
- >
 
  
  # child1
- |1
  child2
- >
 child3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef n = t.rootref();
        ASSERT_TRUE(n.is_seq());
        ASSERT_EQ(n.num_children(), 4u);
        EXPECT_EQ(n[0].val(), csubstr("child0\n"));
        EXPECT_EQ(n[1].val(), csubstr("\n\n# child1\n"));
        EXPECT_EQ(n[2].val(), csubstr(" child2\n"));
        EXPECT_EQ(n[3].val(), csubstr("child3\n"));
    });
}
TEST(block_folded, test_suite_4QFQ_4)
{
    csubstr yaml = R"(foo:
  - |
   child0
  - >
   
    
    # child1
  - |2
     child2
  - >
   child3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef n = t["foo"];
        ASSERT_TRUE(n.is_seq());
        ASSERT_EQ(n.num_children(), 4u);
        EXPECT_EQ(n[0].val(), csubstr("child0\n"));
        EXPECT_EQ(n[1].val(), csubstr("\n\n# child1\n"));
        EXPECT_EQ(n[2].val(), csubstr(" child2\n"));
        EXPECT_EQ(n[3].val(), csubstr("child3\n"));
    });
}


//-----------------------------------------------------------------------------

TEST(block_folded, test_suite_6VJK_0)
{
    csubstr yaml = R"(>
 Sammy Sosa completed another
 fine season with great stats.

 63 Home Runs
 0.288 Batting Average

 What a year!
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr(
                      "Sammy Sosa completed another fine season with great stats.\n"
                      "63 Home Runs 0.288 Batting Average\n"
                      "What a year!\n"));
    });
}
TEST(block_folded, test_suite_6VJK_1)
{
    csubstr yaml = R"(>
 Sammy Sosa completed another
 fine season with great stats.
   63 Home Runs
   0.288 Batting Average
 What a year!
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr(
                      "Sammy Sosa completed another fine season with great stats.\n"
                      "  63 Home Runs\n"
                      "  0.288 Batting Average\n"
                      "What a year!\n"));
    });
}
TEST(block_folded, test_suite_6VJK_2)
{
    csubstr yaml = R"(>
 Sammy Sosa completed another
 fine season with great stats.

   63 Home Runs
   0.288 Batting Average

 What a year!
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr(
                      "Sammy Sosa completed another fine season with great stats.\n"
                      "\n"
                      "  63 Home Runs\n"
                      "  0.288 Batting Average\n"
                      "\n"
                      "What a year!\n"));
    });
}
TEST(block_folded, test_suite_6VJK_3)
{
    csubstr yaml = R"(>
 Sammy Sosa completed another
 fine season with great stats.


   63 Home Runs
   0.288 Batting Average


 What a year!
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr(
                      "Sammy Sosa completed another fine season with great stats.\n"
                      "\n"
                      "\n"
                      "  63 Home Runs\n"
                      "  0.288 Batting Average\n"
                      "\n"
                      "\n"
                      "What a year!\n"));
    });
}
TEST(block_folded, test_suite_6VJK_4)
{
    csubstr yaml = R"(>
 Sammy Sosa completed another
 fine season with great stats.



   63 Home Runs
   0.288 Batting Average



 What a year!
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr(
                      "Sammy Sosa completed another fine season with great stats.\n"
                      "\n"
                      "\n"
                      "\n"
                      "  63 Home Runs\n"
                      "  0.288 Batting Average\n"
                      "\n"
                      "\n"
                      "\n"
                      "What a year!\n"));
    });
}
TEST(block_folded, test_suite_6VJK_5)
{
    csubstr yaml = R"(>-
 No folding needed
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr("No folding needed"));
    });
}
TEST(block_folded, test_suite_6VJK_6)
{
    csubstr yaml = R"(>
 No folding needed)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr("No folding needed\n"));
    });
}


//-----------------------------------------------------------------------------

TEST(block_folded, test_suite_7T8X)
{
    csubstr yaml = R"(>

 folded
 line

 next
 line
   * bullet

   * list
   * lines

 last
 line

# Comment
)";
    csubstr val = R"(
folded line
next line
  * bullet

  * list
  * lines

last line
)";
    std::string emitted = R"(>

  folded line

  next line
    * bullet

    * list
    * lines

  last line
)";
    test_check_emit_check(yaml, [&](Tree const &t){
        EXPECT_EQ(t.rootref().val(), val);
        EXPECT_EQ(emitrs_yaml<std::string>(t), emitted);
    });
}

TEST(block_folded, test_suite_A6F9)
{
    csubstr yaml = R"(
strip: |-
  text
clip: |
  text
keep: |+
  text
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["strip"].val(), "text");
        EXPECT_EQ(t["clip"].val(), "text\n");
        EXPECT_EQ(t["keep"].val(), "text\n");
    });
}

TEST(block_folded, test_suite_B3HG)
{
    csubstr yaml = R"(
--- >
 folded
 text


--- >
 folded
 text
--- >
  folded text
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0).val(), csubstr("folded text\n"));
        EXPECT_EQ(t.docref(1).val(), csubstr("folded text\n"));
        EXPECT_EQ(t.docref(2).val(), csubstr("folded text\n"));
    });
}

TEST(block_folded, test_suite_D83L)
{
    csubstr yaml = R"(
- |2-
  explicit indent and chomp
- |-2
  chomp and explicit indent
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_seq());
        EXPECT_EQ(t[0].val(), csubstr("explicit indent and chomp"));
        EXPECT_EQ(t[1].val(), csubstr("chomp and explicit indent"));
    });
}

TEST(block_folded, test_suite_DWX9)
{
    csubstr yaml = R"(
|
 
  
  literal
   
  
  text

 # Comment
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr("\n\nliteral\n \n\ntext\n"));
    });
}

TEST(block_folded, test_suite_F6MC)
{
    csubstr yaml = R"(
a: >2
   a more indented
  a regular
b: >2


   b more indented
  b regular
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["a"].val(), csubstr(" a more indented\na regular\n"));
        EXPECT_EQ(t["b"].val(), csubstr("\n\n b more indented\nb regular\n"));
    });
}

TEST(block_folded, test_suite_K858)
{
    csubstr yaml = R"(---
# strip
- >-

# clip
- >

# keep
- |+

---
strip: >-

clip: >

keep: |+

)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_EQ(t.docref(0).num_children(), 3u);
        EXPECT_EQ(t.docref(0)[0].val(), csubstr{});
        EXPECT_EQ(t.docref(0)[1].val(), csubstr{});
        EXPECT_EQ(t.docref(0)[2].val(), csubstr("\n"));
        ASSERT_TRUE(t.docref(1).has_child("strip"));
        ASSERT_TRUE(t.docref(1).has_child("keep"));
        ASSERT_TRUE(t.docref(1).has_child("clip"));
        EXPECT_EQ(t.docref(1)["strip"].val(), csubstr{});
        EXPECT_EQ(t.docref(1)["clip"].val(), csubstr{});
        EXPECT_EQ(t.docref(1)["keep"].val(), csubstr("\n"));
    });
}


TEST(block_folded, test_suite_MJS9)
{
    csubstr yaml = R"(
- >
  foo 
 
  	 bar

  baz
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("foo \n\n\t bar\n\nbaz\n"));
    });
}

TEST(block_folded, test_suite_P2AD)
{
    csubstr yaml = R"(
- | # Empty header↓
 literal
- >1 # Indentation indicator↓
  folded
- |+ # Chomping indicator↓
 keep

- >1- # Both indicators↓
  strip
- >-1 # Both indicators↓
  strip
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 5u);
        EXPECT_EQ(t[0].val(), csubstr("literal\n"));
        EXPECT_EQ(t[1].val(), csubstr(" folded\n"));
        EXPECT_EQ(t[2].val(), csubstr("keep\n\n"));
        EXPECT_EQ(t[3].val(), csubstr(" strip"));
        EXPECT_EQ(t[4].val(), csubstr(" strip"));
    });
}


TEST(block_folded, test_suite_R4YG)
{
    csubstr yaml = R"(
- |
 detected0
- >
 
  
  # detected1
- |1
  explicit2
- >
 	
 detected3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 4u);
        EXPECT_EQ(t[0].val(), csubstr("detected0\n"));
        EXPECT_EQ(t[1].val(), csubstr("\n\n# detected1\n"));
        EXPECT_EQ(t[2].val(), csubstr(" explicit2\n"));
        EXPECT_EQ(t[3].val(), csubstr("\t\ndetected3\n"));
    });
}


TEST(block_folded, test_suite_T26H)
{
    csubstr yaml = R"(
--- |
 
  
  literal
   
  
  text

 # Comment
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("\n\nliteral\n \n\ntext\n"));
    });
}


TEST(block_folded, test_suite_T5N4)
{
    csubstr yaml = R"(
--- |
 literal
 	text


)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("literal\n\ttext\n"));
    });
}


TEST(block_folded, test_suite_W4TN)
{
    csubstr yaml = R"(
--- |
 %!PS-Adobe-2.0
...
--- >
 %!PS-Adobe-2.0
...
--- |
%!PS-Adobe-2.0
...
--- >
%!PS-Adobe-2.0
...
---
# Empty
...
--- |
 %!PS-Adobe-2.0
--- >
 %!PS-Adobe-2.0
--- |
%!PS-Adobe-2.0
--- >
%!PS-Adobe-2.0
---
# empty
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef r = t.rootref();
        ASSERT_TRUE(r.is_stream());
        ASSERT_EQ(r.num_children(), 10u);
        ASSERT_TRUE(r.doc(0).is_doc());
        ASSERT_TRUE(r.doc(0).is_val());
        EXPECT_EQ(r.doc(0).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(1).is_doc());
        ASSERT_TRUE(r.doc(1).is_val());
        EXPECT_EQ(r.doc(1).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(2).is_doc());
        ASSERT_TRUE(r.doc(2).is_val());
        EXPECT_EQ(r.doc(2).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(3).is_doc());
        ASSERT_TRUE(r.doc(3).is_val());
        EXPECT_EQ(r.doc(3).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(4).is_doc());
        ASSERT_TRUE(r.doc(4).is_val());
        EXPECT_EQ(r.doc(4).val(), csubstr{});
        ASSERT_TRUE(r.doc(5).is_doc());
        ASSERT_TRUE(r.doc(5).is_val());
        EXPECT_EQ(r.doc(5).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(6).is_doc());
        ASSERT_TRUE(r.doc(6).is_val());
        EXPECT_EQ(r.doc(6).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(7).is_doc());
        ASSERT_TRUE(r.doc(7).is_val());
        EXPECT_EQ(r.doc(7).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(8).is_doc());
        ASSERT_TRUE(r.doc(8).is_val());
        EXPECT_EQ(r.doc(8).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(4).is_doc());
        ASSERT_TRUE(r.doc(4).is_val());
        EXPECT_EQ(r.doc(4).val(), csubstr{});
    });
}


TEST(block_folded, error_on_folded_in_seqflow)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("[\n  >\n    a\n,]");
    });
}


TEST(block_folded, error_on_folded_in_mapflow)
{
    Tree t;
    ExpectError::check_error(&t, [&t]{
        t = parse_in_arena("{\n b: >\n    a\n,}");
    });
}

TEST(block_literal, indentation_indicator_0)
{
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena(">0");
        });
    }
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena(">10");
        });
    }
    {
        Tree t = parse_in_arena(">1");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">3");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">4");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">5");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">6");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">7");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">8");
        EXPECT_EQ(t.rootref().val(), "");
    }
    {
        Tree t = parse_in_arena(">9");
        EXPECT_EQ(t.rootref().val(), "");
    }
}

TEST(block_literal, indentation_indicator_1)
{
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("--- >0");
        });
    }
    {
        Tree t;
        ExpectError::check_error(&t, [&t]{
            t = parse_in_arena("--- >10");
        });
    }
    {
        Tree t = parse_in_arena("--- >1");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >3");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >4");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >5");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >6");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >7");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >8");
        EXPECT_EQ(t.docref(0).val(), "");
    }
    {
        Tree t = parse_in_arena("--- >9");
        EXPECT_EQ(t.docref(0).val(), "");
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_FOLDED)
{
//
ADD_CASE_TO_GROUP("indentation requirements",
R"(---
>
hello
there
---
>
 hello
 there
---
>
  hello
  there
---
>
ciao
qua
---
>
    ciao
    qua
---
>
      ciao
      qua
---
- >
 hello
 there
- >
 ciao
 qua
---
foo: >
 hello
 there
bar: >
 ciao
 qua
)",
N(STREAM, L{
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|SB, L{N(VF, "hello there\n"), N(VF, "ciao qua\n")}),
        N(DOC|MB, L{N(KP|VF, "foo", "hello there\n"), N(KP|VF, "bar", "ciao qua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- >
hello
there
- >
ciao
qua
)");

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: >
hello
there
bar: >
ciao
qua
)"
);

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- >2
 hello
 there
)");

ADD_CASE_TO_GROUP("foo without space after",
R"(>
  foo
)",
N(VF, "foo\n"));

ADD_CASE_TO_GROUP("foo with space after",
R"(>
  foo
  
)",
N(VF, "foo\n"));

ADD_CASE_TO_GROUP("simple with indents",
R"(>
  foo
     
    bar
)",
N(VF, "foo\n   \n  bar\n"));


ADD_CASE_TO_GROUP("7T8X",
R"(- >
    
    folded
    line
    
    next
    line
      * bullet
    
      * list
      * lines
    
    last
    line

# Comment

##### this is the original scalar:
- >

 folded
 line

 next
 line
   * bullet

   * list
   * lines

 last
 line

# Comment

##### without any indentation
- >

    folded
    line

    next
    line
      * bullet

      * list
      * lines

    last
    line

# Comment
)",
N(SB, L{
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
  })
);


ADD_CASE_TO_GROUP("block folded as seq val, implicit indentation 2",
R"(
- >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.


- another val
)",
N(SB, L{
    N(VF, "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(VP,"another val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2",
R"(
example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2, chomp=keep",
R"(
example: >+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2, chomp=strip",
R"(
example: >-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2",
R"(
example: >2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2, chomp=keep",
R"(
example: >+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
example2: >2+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N(KP|VF, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2, chomp=strip",
R"(
example: >-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
example2: >2-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N(KP|VF, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 3",
R"(
example: >
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 3",
R"(
example: >3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 4",
R"(
example: >
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 4",
R"(
example: >4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 9",
R"(
example: >
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 9",
R"(
example: >9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);


ADD_CASE_TO_GROUP("block folded as map entry",
R"(
data: >
   Wrapped text
   will be folded
   into a single
   paragraph

   Blank lines denote
   paragraph breaks
)",
  N(MB, L{N(KP|VF, "data", "Wrapped text will be folded into a single paragraph\nBlank lines denote paragraph breaks\n")})
);

ADD_CASE_TO_GROUP("block folded, no chomp, no indentation",
R"(example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
  N(MB, L{
      N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
      N(KP|VP, "another", "text"),
      })
);

ADD_CASE_TO_GROUP("block folded with tab and spaces",
R"(>
       	  )",
  N(VF, "\t  \n")
    );


ADD_CASE_TO_GROUP("block folded with empty docval 1",
R"(>)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 2",
R"(>
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 3",
R"(>
  )",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 4",
R"(>
  
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 5",
R"(>
    
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 8",
R"(>

)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 9",
R"(>



)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 10",
R"(>




)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 11",
R"(>
 
  
   
    )",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 12",
R"(>
 
  
   
    
     
      
    
   
 
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 13",
R"(>
 
  

   

    

)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 0",
R"(>
  asd)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 1",
R"(>
  asd
)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 2",
R"(>
  asd

)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 3",
R"(>
  asd
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 4",
R"(>
  asd
  
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5",
R"(>
     asd
   
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.1",
R"(>
       asd
     
   
     
  
 
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.2",
R"(>
       asd
     
   
       
  
 
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.3",
R"(>
       asd
     
   
        
  
 
  )",
  N(VF, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 6",
R"(>
     asd
      )",
  N(VF, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 7",
R"(>
     asd
      
)",
  N(VF, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 8",
R"(>
     asd
       )",
  N(VF, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 9",
R"(>
     asd
       
)",
  N(VF, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 10",
R"(>
     asd
     	 )",
  N(VF, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 11",
R"(>
     asd
      	 )",
  N(VF, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 12",
R"(>
     asd
     	 
)",
  N(VF, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 13",
R"(>
     asd
      	 
)",
  N(VF, "asd\n \t \n")
    );


ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 0",
R"(>+)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1",
R"(>+
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.1",
R"(>+
  )",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.2",
R"(>+
  asd)",
  N(VF, "asd")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.3",
R"(>+
  asd
)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.4",
R"(>+
  asd
  
)",
  N(VF, "asd\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2",
R"(>+

)",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2.1",
R"(>+

  )",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 3",
R"(>+


)",
  N(VF, "\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 4",
R"(>+



)",
  N(VF, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 5",
R"(>+




)",
  N(VF, "\n\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 0",
R"(- >+
  
- >+
  )",
N(SB, L{N(VF, "\n"), N(VF, "\n"),}));

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 1",
R"(- >+
  
- >+
  
)",
N(SB, L{N(VF, "\n"), N(VF, "\n"),}));

}

} // namespace yml
} // namespace c4
