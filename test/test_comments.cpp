#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"


namespace c4 {
namespace yml {

struct CommentCase
{
    csubstr input, expected;
    int line;
};

struct Filter : public ::testing::TestWithParam<CommentCase>
{
};

static void test_filter(CommentCase const& cc)
{
    ASSERT_LE(cc.expected.len, cc.input.len);
    std::string buf_(cc.input.str, cc.input.len);
    c4::substr buf = to_substr(buf_);
    Parser::handler_type event_handler = {};
    Parser parser(&event_handler);
    csubstr actual = parser._filter_comment(buf);
    ASSERT_TRUE(actual.is_sub(buf));
    if(actual != cc.expected)
    {
        RYML_TRACE_FMT("see:\n{}:{}: case:\ninp=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~\nact=[{}]~~~{}~~~",
                       __FILE__, cc.line,
                       cc.input.len, cc.input,
                       cc.expected.len, cc.expected,
                       actual.len, actual);
        std::string expected_str(cc.expected.str, cc.expected.len);
        std::string actual_str(actual.str, actual.len);
        EXPECT_EQ(expected_str, actual_str);
    }
}

TEST_P(Filter, test)
{
    test_filter(GetParam());
}

static CommentCase test_cases_filter[] = {
    #define commcase(input, output) CommentCase{csubstr(input), csubstr(output), __LINE__}
    commcase("#", ""),
    commcase("#\n", ""),
    commcase("#\n#", "\n"),
    commcase("#\n#\n", "\n"),
    commcase("#\n#\n#", "\n\n"),
    commcase("#\n#\n#\n", "\n\n"),
    commcase("#", ""),
    commcase("#\n", ""),
    commcase("#\n #", "\n"),
    commcase("#\n #\n", "\n"),
    commcase("#\n #\n #", "\n\n"),
    commcase("#\n #\n #\n", "\n\n"),
    commcase("#", ""),
    commcase("#\n", ""),
    commcase("#\n   #", "\n"),
    commcase("#\n   #\n", "\n"),
    commcase("#\n   #\n   #", "\n\n"),
    commcase("#\n   #\n   #\n", "\n\n"),
    commcase("#a", "a"),
    commcase("#a\n", "a"),
    commcase("#a\n#b", "a\nb"),
    commcase("#a\n#b\n", "a\nb"),
    commcase("#a\n#b\n#", "a\nb\n"),
    commcase("#a\n#b\n#c", "a\nb\nc"),
    commcase("#a\n#b\n#c\n", "a\nb\nc"),
    commcase("#a\n#b\n#c\n#", "a\nb\nc\n"),
    commcase("#a\n#b\n#c\n#\n", "a\nb\nc\n"),
    commcase("#a", "a"),
    commcase("#a\n", "a"),
    commcase("#a\n #b", "a\nb"),
    commcase("#a\n #b\n", "a\nb"),
    commcase("#a\n #b\n #", "a\nb\n"),
    commcase("#a\n #b\n #c", "a\nb\nc"),
    commcase("#a\n #b\n #c\n", "a\nb\nc"),
    commcase("#a\n #b\n #c\n#", "a\nb\nc\n"),
    commcase("#a\n #b\n #c\n#\n", "a\nb\nc\n"),
    commcase("#a\n   #b", "a\nb"),
    commcase("#a\n   #b\n", "a\nb"),
    commcase("#a\n   #b\n   #", "a\nb\n"),
    commcase("#a\n   #b\n   #c", "a\nb\nc"),
    commcase("#a\n   #b\n   #c\n", "a\nb\nc"),
    commcase("#a\n   #b\n   #c\n#", "a\nb\nc\n"),
    commcase("#a\n   #b\n   #c\n#\n", "a\nb\nc\n"),
    commcase("#aaaa", "aaaa"),
    commcase("#aaaa\n", "aaaa"),
    commcase("#aaaa\n#", "aaaa\n"),
    commcase("#aaaa\n #bbbb", "aaaa\nbbbb"),
    commcase("#aaaa\n #bbbb\n", "aaaa\nbbbb"),
    commcase("#aaaa\n #bbbb\n #", "aaaa\nbbbb\n"),
    commcase("#aaaa\n #bbbb\n #cccc", "aaaa\nbbbb\ncccc"),
    commcase("#aaaa\n #bbbb\n #cccc\n", "aaaa\nbbbb\ncccc"),
    commcase("#aaaa\n #bbbb\n #cccc\n #", "aaaa\nbbbb\ncccc\n"),
    commcase("#aaaa\n #bbbb\n #cccc\n #\n", "aaaa\nbbbb\ncccc\n"),
    commcase("#aaaa\n   #bbbb", "aaaa\nbbbb"),
    commcase("#aaaa\n   #bbbb\n", "aaaa\nbbbb"),
    commcase("#aaaa\n   #bbbb\n   #", "aaaa\nbbbb\n"),
    commcase("#aaaa\n   #bbbb\n   #cccc", "aaaa\nbbbb\ncccc"),
    commcase("#aaaa\n   #bbbb\n   #cccc\n", "aaaa\nbbbb\ncccc"),
    commcase("#aaaa\n   #bbbb\n   #cccc\n   #", "aaaa\nbbbb\ncccc\n"),
    commcase("#aaaa\n   #bbbb\n   #cccc\n   #\n", "aaaa\nbbbb\ncccc\n"),
    commcase("#aa#aa#", "aa#aa#"),
    commcase("#aa#aa#\n", "aa#aa#"),
    commcase("#aa#aa#\n#", "aa#aa#\n"),
    commcase("#aa#aa#\n #bb#bb#", "aa#aa#\nbb#bb#"),
    commcase("#aa#aa#\n #bb#bb#\n", "aa#aa#\nbb#bb#"),
    commcase("#aa#aa#\n #bb#bb#\n #", "aa#aa#\nbb#bb#\n"),
    commcase("#aa#aa#\n #bb#bb#\n #cc#cc#", "aa#aa#\nbb#bb#\ncc#cc#"),
    commcase("#aa#aa#\n #bb#bb#\n #cc#cc#\n", "aa#aa#\nbb#bb#\ncc#cc#"),
    commcase("#aa#aa#\n #bb#bb#\n #cc#cc#\n #", "aa#aa#\nbb#bb#\ncc#cc#\n"),
    commcase("#aa#aa#\n #bb#bb#\n #cc#cc#\n #\n", "aa#aa#\nbb#bb#\ncc#cc#\n"),
    commcase("#aa#aa#\n   #bb#bb#", "aa#aa#\nbb#bb#"),
    commcase("#aa#aa#\n   #bb#bb#\n", "aa#aa#\nbb#bb#"),
    commcase("#aa#aa#\n   #bb#bb#\n   #", "aa#aa#\nbb#bb#\n"),
    commcase("#aa#aa#\n   #bb#bb#\n   #cc#cc#", "aa#aa#\nbb#bb#\ncc#cc#"),
    commcase("#aa#aa#\n   #bb#bb#\n   #cc#cc#\n", "aa#aa#\nbb#bb#\ncc#cc#"),
    commcase("#aa#aa#\n   #bb#bb#\n   #cc#cc#\n   #", "aa#aa#\nbb#bb#\ncc#cc#\n"),
    commcase("#aa#aa#\n   #bb#bb#\n   #cc#cc#\n   #\n", "aa#aa#\nbb#bb#\ncc#cc#\n"),
    // we don't skip leading whitespace
    commcase("#  aa#aa#", "  aa#aa#"),
    commcase("#  aa#aa#\n", "  aa#aa#"),
    commcase("#  aa#aa#\n#", "  aa#aa#\n"),
    commcase("#  aa#aa#\n#  ", "  aa#aa#\n  "),
    commcase("#  aa#aa#\n #  bb#bb#", "  aa#aa#\n  bb#bb#"),
    commcase("#  aa#aa#\n #  bb#bb#\n", "  aa#aa#\n  bb#bb#"),
    commcase("#  aa#aa#\n #  bb#bb#\n #", "  aa#aa#\n  bb#bb#\n"),
    commcase("#  aa#aa#\n #  bb#bb#\n #  ", "  aa#aa#\n  bb#bb#\n  "),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#", "  aa#aa#\n  bb#bb#\n  cc#cc#"),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#\n", "  aa#aa#\n  bb#bb#\n  cc#cc#"),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#\n #", "  aa#aa#\n  bb#bb#\n  cc#cc#\n"),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#\n #\n", "  aa#aa#\n  bb#bb#\n  cc#cc#\n"),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#\n #  ", "  aa#aa#\n  bb#bb#\n  cc#cc#\n  "),
    commcase("#  aa#aa#\n #  bb#bb#\n #  cc#cc#\n #  \n", "  aa#aa#\n  bb#bb#\n  cc#cc#\n  "),
    commcase("#  aa#aa#\n   #  bb#bb#", "  aa#aa#\n  bb#bb#"),
    commcase("#  aa#aa#\n   #  bb#bb#\n", "  aa#aa#\n  bb#bb#"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #", "  aa#aa#\n  bb#bb#\n"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #\n", "  aa#aa#\n  bb#bb#\n"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  ", "  aa#aa#\n  bb#bb#\n  "),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  \n", "  aa#aa#\n  bb#bb#\n  "),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#", "  aa#aa#\n  bb#bb#\n  cc#cc#"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#\n", "  aa#aa#\n  bb#bb#\n  cc#cc#"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#\n   #", "  aa#aa#\n  bb#bb#\n  cc#cc#\n"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#\n   #\n", "  aa#aa#\n  bb#bb#\n  cc#cc#\n"),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#\n   #  ", "  aa#aa#\n  bb#bb#\n  cc#cc#\n  "),
    commcase("#  aa#aa#\n   #  bb#bb#\n   #  cc#cc#\n   #  \n", "  aa#aa#\n  bb#bb#\n  cc#cc#\n  "),
    // we don't skip trailing whitespace
    commcase("#aa#aa#  ", "aa#aa#  "),
    commcase("#aa#aa#  \n", "aa#aa#  "),
    commcase("#aa#aa#  \n#", "aa#aa#  \n"),
    commcase("#aa#aa#  \n#  ", "aa#aa#  \n  "),
    commcase("#aa#aa#  \n #bb#bb#  ", "aa#aa#  \nbb#bb#  "),
    commcase("#aa#aa#  \n #bb#bb#  \n", "aa#aa#  \nbb#bb#  "),
    commcase("#aa#aa#  \n #bb#bb#  \n #", "aa#aa#  \nbb#bb#  \n"),
    commcase("#aa#aa#  \n #bb#bb#  \n #  ", "aa#aa#  \nbb#bb#  \n  "),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  ", "aa#aa#  \nbb#bb#  \ncc#cc#  "),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  \n", "aa#aa#  \nbb#bb#  \ncc#cc#  "),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  \n #", "aa#aa#  \nbb#bb#  \ncc#cc#  \n"),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  \n #\n", "aa#aa#  \nbb#bb#  \ncc#cc#  \n"),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  \n #  ", "aa#aa#  \nbb#bb#  \ncc#cc#  \n  "),
    commcase("#aa#aa#  \n #bb#bb#  \n #cc#cc#  \n #  \n", "aa#aa#  \nbb#bb#  \ncc#cc#  \n  "),
    commcase("#aa#aa#  \n   #bb#bb#  ", "aa#aa#  \nbb#bb#  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n", "aa#aa#  \nbb#bb#  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #", "aa#aa#  \nbb#bb#  \n"),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #  ", "aa#aa#  \nbb#bb#  \n  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #  \n", "aa#aa#  \nbb#bb#  \n  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  ", "aa#aa#  \nbb#bb#  \ncc#cc#  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  \n", "aa#aa#  \nbb#bb#  \ncc#cc#  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  \n   #", "aa#aa#  \nbb#bb#  \ncc#cc#  \n"),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  \n   #\n", "aa#aa#  \nbb#bb#  \ncc#cc#  \n"),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  \n   #  ", "aa#aa#  \nbb#bb#  \ncc#cc#  \n  "),
    commcase("#aa#aa#  \n   #bb#bb#  \n   #cc#cc#  \n   #  \n", "aa#aa#  \nbb#bb#  \ncc#cc#  \n  "),
    #undef commcase
};


INSTANTIATE_TEST_SUITE_P(Comments,
                         Filter,
                         testing::ValuesIn(test_cases_filter),
                         [](const testing::TestParamInfo<CommentCase>& nfo) {
                             return c4::catrs<std::string>("line", nfo.param.line);
                         });



//-----------------------------------------------------------------------------

/*WIP*/GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(YmlTestCase);


// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
