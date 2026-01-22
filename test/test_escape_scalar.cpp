#ifndef RYML_SINGLE_HEADER
#include <c4/yml/escape_scalar.hpp>
#endif
#include <gtest/gtest.h>

#include "./test_lib/test_case.hpp"

namespace c4 {
namespace yml {

struct EscapeScalarCase
{
    const char* file;
    int line;
    csubstr scalar;
    csubstr escaped;
};

class EscapeScalarTest : public testing::TestWithParam<EscapeScalarCase> {};


TEST_P(EscapeScalarTest, escape_scalar)
{
    EscapeScalarCase const& ec = GetParam();
    printf("%s:%d: %s\n", ec.file, ec.line, ec.escaped.str);
    RYML_TRACE_FMT("defined in:\n{}:{}: {}", ec.file, ec.line, ec.escaped);
    std::string buf_;
    // empty
    substr buf = to_substr(buf_);
    EXPECT_EQ(0, buf.len);
    EXPECT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar));
    EXPECT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar)));
    // insufficient
    buf_.resize(ec.escaped.len / 2u);
    buf = to_substr(buf_);
    buf.fill('\0');
    EXPECT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar));
    buf.fill('\0');
    EXPECT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar)));
    // enough size
    buf_.resize(ec.escaped.len * 2u);
    buf = to_substr(buf_);
    buf.fill('\0');
    ASSERT_EQ(ec.escaped.len, escape_scalar(buf, ec.scalar));
    EXPECT_EQ(ec.escaped, buf.first(ec.escaped.len));
    buf.fill('\0');
    ASSERT_EQ(ec.escaped.len, to_chars(buf, escaped_scalar(ec.scalar)));
    EXPECT_EQ(ec.escaped, buf.first(ec.escaped.len));
}

#define _ec(scalar, escaped) EscapeScalarCase{__FILE__, __LINE__, csubstr(scalar), csubstr(escaped)}
const EscapeScalarCase escape_cases[] = {
    _ec("", ""),
    _ec(" ", " "),
    _ec("a", "a"),
    _ec("\n", "\\n"),
    _ec("\\", "\\\\"),
    _ec("\t", "\\t"),
    _ec("\r", "\\r"),
    _ec("\0", "\\0"),
    _ec("\f", "\\f"),
    _ec("\b", "\\b"),
    _ec("\a", "\\a"),
    _ec("\v", "\\v"),
    _ec("\x1b", "\\e"),
    _ec("\xc2\xa0", "\\_"),
    _ec("\xc2\x85", "\\N"),
    _ec("\xc2\x86", "\xc2\x86"),
    _ec("\xe2\x80\xa8", "\\L"),
    _ec("\xe2\x80\xa9", "\\P"),
    _ec("\xe2\x80\xa0", "\xe2\x80\xa0"),
    _ec("	\t\r\n\0\f\a\v\x1b\xc2\x85\xc2\xa0\xe2\x80\xa8\xe2\x80\xa9 \b",
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
