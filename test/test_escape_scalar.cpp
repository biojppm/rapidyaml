#ifndef RYML_SINGLE_HEADER
#include <c4/yml/escape_scalar.hpp>
#endif
#include <gtest/gtest.h>

#include "./test_lib/test_case.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

struct EscapeScalarCase
{
    const char* file;
    int line;
    csubstr scalar;
    csubstr escaped;
    bool keep_newlines;
};

class EscapeScalarTest : public testing::TestWithParam<EscapeScalarCase> {};


TEST_P(EscapeScalarTest, escape_scalar)
{
    EscapeScalarCase const& ec = GetParam();
    RYML_TRACE_FMT("defined in:\n{}:{}: \"{}\"", ec.file, ec.line, ec.escaped);
    std::string buf_;
    // empty
    substr buf = to_substr(buf_);
    EXPECT_EQ(0, buf.len);
    EXPECT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar, ec.keep_newlines));
    EXPECT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar, ec.keep_newlines)));
    if(!ec.keep_newlines)
    {
        EXPECT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar));
        EXPECT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar)));
    }
    // insufficient
    buf_.resize(ec.escaped.len / 2u);
    buf = to_substr(buf_);
    buf.fill('\0');
    EXPECT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar, ec.keep_newlines));
    buf.fill('\0');
    EXPECT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar, ec.keep_newlines)));
    // enough size
    buf_.resize(ec.escaped.len * 2u);
    buf = to_substr(buf_);
    buf.fill('\0');
    ASSERT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar, ec.keep_newlines));
    EXPECT_EQ(ec.escaped, buf.first(ec.escaped.len));
    buf.fill('\0');
    ASSERT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar, ec.keep_newlines)));
    EXPECT_EQ(ec.escaped, buf.first(ec.escaped.len));
    // dump
    auto sink = [&](csubstr s){
        buf_.append(s.str, s.len);
    };
    buf_.clear();
    dump(sink, {}, escaped_scalar(ec.scalar, ec.keep_newlines));
    ASSERT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar, ec.keep_newlines)));
    EXPECT_EQ(ec.escaped, buf_);
}

TEST_P(EscapeScalarTest, adjust_pos_with_escapes)
{
    EscapeScalarCase const& ec = GetParam();
    RYML_TRACE_FMT("defined in:\n{}:{}: \"{}\"", ec.file, ec.line, ec.escaped);
    // beginning of scalar
    EXPECT_EQ(adjust_pos_with_escapes(ec.scalar, 0), 0);
    // end of scalar, without added newlines
    for(size_t extra = 0; extra < 10; ++extra)
    {
        size_t pos = ec.scalar.len + extra;
        size_t expected = ec.escaped.len + extra;
        size_t num_newlines = ec.scalar.count('\n');
        RYML_TRACE_FMT("extra={}  pos={} expected={} num_newlines={}", extra, pos, expected, num_newlines);
        if(!ec.keep_newlines)
        {
            // end of scalar, without added newlines
            EXPECT_EQ(adjust_pos_with_escapes(ec.scalar, pos), expected);
            EXPECT_EQ(adjust_pos_with_escapes(ec.scalar, pos, false), expected);
            // end of scalar, with added newlines
            EXPECT_EQ(adjust_pos_with_escapes(ec.scalar, pos, true), expected + num_newlines);
        }
        else
        {
            EXPECT_EQ(adjust_pos_with_escapes(ec.scalar, pos, true), expected);
        }
    }
}


#define ec_(scalar, escaped) EscapeScalarCase{__FILE__, __LINE__, csubstr(scalar), csubstr(escaped), false}
#define ecn(scalar, escaped) EscapeScalarCase{__FILE__, __LINE__, csubstr(scalar), csubstr(escaped), true}
const EscapeScalarCase escape_cases[] = {
    ec_("", ""),
    ec_(" ", " "),
    ec_("a", "a"),
    ec_("\n", "\\n"),
    ecn("\n", "\\n\n"),
    ec_("\\", "\\\\"),
    ec_("\t", "\\t"),
    ec_("\r", "\\r"),
    ec_("\0", "\\0"),
    ec_("\f", "\\f"),
    ec_("\b", "\\b"),
    ec_("\a", "\\a"),
    ec_("\v", "\\v"),
    ec_("\x1b", "\\e"),
    ec_("\n\n", "\\n\\n"),
    ecn("\n\n", "\\n\n\\n\n"),
    ec_("\\\\", "\\\\\\\\"),
    ec_("\t\t", "\\t\\t"),
    ec_("\r\r", "\\r\\r"),
    ec_("\0\0", "\\0\\0"),
    ec_("\f\f", "\\f\\f"),
    ec_("\b\b", "\\b\\b"),
    ec_("\a\a", "\\a\\a"),
    ec_("\v\v", "\\v\\v"),
    ec_("\x1b\x1b", "\\e\\e"),
    ec_("\n\n\n", "\\n\\n\\n"),
    ec_("\\\\\\", "\\\\\\\\\\\\"),
    ec_("\t\t\t", "\\t\\t\\t"),
    ec_("\r\r\r", "\\r\\r\\r"),
    ec_("\0\0\0", "\\0\\0\\0"),
    ec_("\f\f\f", "\\f\\f\\f"),
    ec_("\b\b\b", "\\b\\b\\b"),
    ec_("\a\a\a", "\\a\\a\\a"),
    ec_("\v\v\v", "\\v\\v\\v"),
    ec_("\x1b\x1b\x1b", "\\e\\e\\e"),
    ec_("\xc2\xa0", "\\_"),
    ec_("\xc2\x85", "\\N"),
    ec_("\xc2\x86", "\xc2\x86"),
    ec_("\xc2\xa0\xc2\xa0", "\\_\\_"),
    ec_("\xc2\x85\xc2\x85", "\\N\\N"),
    ec_("\xc2\xa0\xc2\xa0\xc2\xa0", "\\_\\_\\_"),
    ec_("\xc2\x85\xc2\x85\xc2\x85", "\\N\\N\\N"),
    ec_("\xe2\x80\xa8", "\\L"),
    ec_("\xe2\x80\xa9", "\\P"),
    ec_("\xe2\x80\xa0", "\xe2\x80\xa0"),
    ec_("\xe2\x80\xa8\xe2\x80\xa8", "\\L\\L"),
    ec_("\xe2\x80\xa9\xe2\x80\xa9", "\\P\\P"),
    ec_("\xe2\x80\xa8\xe2\x80\xa8\xe2\x80\xa8", "\\L\\L\\L"),
    ec_("\xe2\x80\xa9\xe2\x80\xa9\xe2\x80\xa9", "\\P\\P\\P"),
    ec_("	\t\r\n\0\f\a\v\x1b\xc2\x85\xc2\xa0\xe2\x80\xa8\xe2\x80\xa9 \b",
        "\\t\\t\\r\\n\\0\\f\\a\\v\\e\\N\\_\\L\\P \\b"),
};


INSTANTIATE_TEST_SUITE_P(EscapeScalar, EscapeScalarTest, testing::ValuesIn(escape_cases));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
