#include <c4/yml/evt/extra/event_handler_ints.hpp>
#include <c4/bitmask.hpp>
#include <vector>
#include "test_lib/test_case.hpp"

namespace c4 {

using EventFlags = c4::yml::evt::extra::ievt::EventFlags;

template<>
c4::EnumSymbols<EventFlags> const esyms<EventFlags>()
{
    static constexpr typename c4::EnumSymbols<EventFlags>::Sym syms[] = {
        {yml::evt::extra::ievt::KEY_, "KEY_"},
        {yml::evt::extra::ievt::VAL_, "VAL_"},
        {yml::evt::extra::ievt::SCLR, "SCLR"},
        {yml::evt::extra::ievt::BSEQ, "BSEQ"},
        {yml::evt::extra::ievt::ESEQ, "ESEQ"},
        {yml::evt::extra::ievt::BMAP, "BMAP"},
        {yml::evt::extra::ievt::EMAP, "EMAP"},
        {yml::evt::extra::ievt::ALIA, "ALIA"},
        {yml::evt::extra::ievt::ANCH, "ANCH"},
        {yml::evt::extra::ievt::TAG_, "TAG_"},
        {yml::evt::extra::ievt::PLAI, "PLAI"},
        {yml::evt::extra::ievt::SQUO, "SQUO"},
        {yml::evt::extra::ievt::DQUO, "DQUO"},
        {yml::evt::extra::ievt::LITL, "LITL"},
        {yml::evt::extra::ievt::FOLD, "FOLD"},
        {yml::evt::extra::ievt::FLOW, "FLOW"},
        {yml::evt::extra::ievt::BLCK, "BLCK"},
        {yml::evt::extra::ievt::BDOC, "BDOC"},
        {yml::evt::extra::ievt::EDOC, "EDOC"},
        {yml::evt::extra::ievt::BSTR, "BSTR"},
        {yml::evt::extra::ievt::ESTR, "ESTR"},
        {yml::evt::extra::ievt::EXPL, "EXPL"},
    };
    return c4::EnumSymbols<EventFlags>(syms);
}


namespace yml {
namespace evt {
namespace extra {


// provide a structured input for the events, grouping the relevant
// data in a single structure
struct IntEventWithScalar
{
    ievt::DataType flags, str_start, str_len;
    csubstr scalar;
    bool needs_filter;
    IntEventWithScalar(ievt::DataType t, ievt::DataType start=0, ievt::DataType len=0, csubstr sclr={}, bool needs_filter_=false)
        : flags(t)
        , str_start(start)
        , str_len(len)
        , scalar(sclr)
        , needs_filter(needs_filter_)
    {
    }
    size_t required_size() const { return (flags & ievt::HAS_STR) ? 3u : 1u; }
};


struct IntEventsCase
{
    const char *file;
    const int line;
    csubstr yaml;
    const std::vector<IntEventWithScalar> evt;

    size_t expected_size() const
    {
        size_t sz = 0;
        for(IntEventWithScalar const& e : evt)
            sz += e.required_size();
        return sz;
    }
    void testeq(ievt::DataType const* actual, size_t actual_size, csubstr parsed_source) const
    {
        int status = true;
        size_t num_events_expected = evt.size();
        size_t num_ints_expected = expected_size();

        EXPECT_EQ(actual_size, num_ints_expected);
        status = (actual_size == num_ints_expected);

        char actualbuf[100];
        char expectedbuf[100];
        for(size_t i = 0, ie = 0; ie < num_events_expected; ++ie)
        {
            EXPECT_LT(i, actual_size);
            if (i >= actual_size)
                break;
            size_t reqsize_actual = c4::bm2str<ievt::EventFlags>(actual[i] & ievt::MASK, actualbuf, sizeof(actualbuf));
            size_t reqsize_expected = c4::bm2str<ievt::EventFlags>(evt[ie].flags & ievt::MASK, expectedbuf, sizeof(expectedbuf));
            ASSERT_GT(reqsize_actual, 0u);
            ASSERT_GT(reqsize_expected, 0u);
            ASSERT_LT(reqsize_actual, sizeof(actualbuf));
            ASSERT_LT(reqsize_expected, sizeof(expectedbuf));
            csubstr actual_str = {actualbuf, reqsize_actual-1u};
            csubstr expected_str = {expectedbuf, reqsize_expected-1u};
#define _test_eq(fmt, lhs, rhs, ...)                                \
        do                                                          \
        {                                                           \
            _c4dbgpf("status={} cmp={} evt={} i={}: {}={} {}={} " fmt, status, (lhs == rhs), ie, i, #lhs, lhs, #rhs, rhs, __VA_ARGS__); \
            status &= (lhs == rhs);                                 \
            EXPECT_EQ(lhs, rhs);                                    \
        } while(0)
            _test_eq("actual={} expected={}", actual[i], evt[ie].flags, actual_str, expected_str);
            if((evt[ie].flags & ievt::HAS_STR) && (actual[i] & ievt::HAS_STR))
            {
                _test_eq("", evt[ie].str_start, actual[i + 1], 0);
                _test_eq("", evt[ie].str_len, actual[i + 2], 0);
                bool safeactual = (i + 2 < actual_size) && (actual[i + 1] < (int)parsed_source.len && actual[i + 1] + actual[i + 2] <= (int)parsed_source.len);
                bool safeexpected = (evt[ie].str_start < (int)parsed_source.len && evt[ie].str_start + evt[ie].str_len <= (int)parsed_source.len);
                _test_eq("", safeactual, true, 0);
                _test_eq("", safeactual, safeexpected, 0);
                if(safeactual && safeexpected)
                {
                    csubstr evtstr = parsed_source.sub((size_t)evt[ie].str_start, (size_t)evt[ie].str_len);
                    csubstr actualstr = parsed_source.sub((size_t)actual[i + 1], (size_t)actual[i + 2]);
                    _test_eq("   ref=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                             evt[ie].scalar, actualstr,
                             evt[ie].scalar.len, evt[ie].scalar,
                             actualstr.len, actualstr);
                    if( ! evt[ie].needs_filter)
                    {
                        _test_eq("   exp=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                                 evtstr, actualstr,
                                 evtstr.len, evtstr,
                                 actualstr.len, actualstr);
                    }
                }
            }
            i += (actual[i] & ievt::HAS_STR) ? 3u : 1u;
        }
        RYML_TRACE_FMT("defined in:\n{}:{}\n"
                       "input:[{}]~~~{}~~~\n"
                       "parsed:[{}]~~~{}~~~\n",
                       file, line,
                       yaml.len, yaml,
                       parsed_source.len, parsed_source);
        EXPECT_TRUE(status);
    }
};



//-----------------------------------------------------------------------------

// make the declarations shorter
#define tc(ys, ...) IntEventsCase{__FILE__, __LINE__, ys, std::vector<IntEventWithScalar>(__VA_ARGS__)}
#define e(...) IntEventWithScalar{__VA_ARGS__}
using namespace ievt;
const bool needs_filter = true;
const IntEventsCase test_cases[] = {
    // case -------------------------------------------------
    tc("!yamlscript/v0/bare\n--- !code\n42\n",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|TAG_, 1, 18, "yamlscript/v0/bare"),
           e(VAL_|SCLR|PLAI, 0, 0, ""),
           e(EDOC),
           e(BDOC|EXPL),
           e(VAL_|TAG_, 25, 4, "code"),
           e(VAL_|SCLR|PLAI, 30, 2, "42"),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("a: 1",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 1, "a"),
           e(VAL_|SCLR|PLAI, 3, 1, "1"),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("say: 2 + 2",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 3, "say"),
           e(VAL_|SCLR|PLAI, 5, 5, "2 + 2"),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("𝄞: ✅",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 4, "𝄞"),
           e(VAL_|SCLR|PLAI, 6, 3, "✅"),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[a, b, c]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 1, 1, "a"),
           e(VAL_|SCLR|PLAI, 4, 1, "b"),
           e(VAL_|SCLR|PLAI, 7, 1, "c"),
           e(ESEQ),
           e(EDOC),
           e(ESTR),
       }),
    // case ------------------------------
    tc("[a: b]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 1, 1, "a"),
           e(VAL_|SCLR|PLAI, 4, 1, "b"),
           e(EMAP),
           e(ESEQ),
           e(EDOC),
           e(ESTR),
       }),
    // case ------------------------------
    tc(R"(--- !yamlscript/v0
foo: !
- {x: y}
- [x, y]
- foo
- 'foo'
- "foo"
- |
  foo
- >
  foo
- [1, 2, true, false, null]
- &anchor-1 !tag-1 foobar
---
another: doc
)",
       {
           e(BSTR),
           e(BDOC|EXPL),
           e(VAL_|TAG_, 5, 13, "yamlscript/v0"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 19, 3, "foo"),
           e(VAL_|TAG_, 25, 0, ""),
           e(VAL_|BSEQ|BLCK),
           e(VAL_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 29, 1, "x"),
           e(VAL_|SCLR|PLAI, 32, 1, "y"),
           e(EMAP),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 38, 1, "x"),
           e(VAL_|SCLR|PLAI, 41, 1, "y"),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 46, 3, "foo"),
           e(VAL_|SCLR|SQUO, 53, 3, "foo"),
           e(VAL_|SCLR|DQUO, 61, 3, "foo"),
           e(VAL_|SCLR|LITL, 70, 4, "foo\n", needs_filter),
           e(VAL_|SCLR|FOLD, 80, 4, "foo\n", needs_filter),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 89, 1, "1"),
           e(VAL_|SCLR|PLAI, 92, 1, "2"),
           e(VAL_|SCLR|PLAI, 95, 4, "true"),
           e(VAL_|SCLR|PLAI, 101, 5, "false"),
           e(VAL_|SCLR|PLAI, 108, 4, "null"),
           e(ESEQ),
           e(VAL_|TAG_, 127, 5, "tag-1"),
           e(VAL_|ANCH, 117, 8, "anchor-1"),
           e(VAL_|SCLR|PLAI, 133, 6, "foobar"),
           e(ESEQ),
           e(EMAP),
           e(EDOC),
           e(BDOC|EXPL),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 144, 7, "another"),
           e(VAL_|SCLR|PLAI, 153, 3, "doc"),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc(R"(plain: well
  a
  b
  c
squo: 'single''quote'
dquo: "x\t\ny"
lit: |
     X
     Y
     Z
fold: >
     U
     V
     W
)",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 5, "plain"),
           e(VAL_|SCLR|PLAI, 7, 10, "well a b c"),
           e(KEY_|SCLR|PLAI, 24, 4, "squo"),
           e(VAL_|SCLR|SQUO, 31, 12, "single'quote", needs_filter),
           e(KEY_|SCLR|PLAI, 46, 4, "dquo"),
           e(VAL_|SCLR|DQUO, 53, 4, "x\t\ny", needs_filter),
           e(KEY_|SCLR|PLAI, 61, 3, "lit"),
           e(VAL_|SCLR|LITL, 68, 6, "X\nY\nZ\n", needs_filter),
           e(KEY_|SCLR|PLAI, 89, 4, "fold"),
           e(VAL_|SCLR|FOLD, 97, 6, "U V W\n", needs_filter),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("- !!seq []",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|BLCK),
           e(VAL_|TAG_, 2, 5, "!!seq"),
           e(VAL_|BSEQ|FLOW),
           e(ESEQ),
           e(ESEQ),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc(R"_(defn run(prompt session=nil):
  when session:
    write session _ :append true: |+
      Q: $(orig-prompt:trim)
      A ($api-model):
      $(answer:trim)
)_",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 28, "defn run(prompt session=nil)"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 32, 12, "when session"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 50, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL, 83, 54, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n", needs_filter),
           e(EMAP),
           e(EMAP),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc(R"_(#!/usr/bin/env ys-0

defn run(prompt session=nil):
  session-text =:
    when session && session:fs-e:

  answer =:
    cond:
      api-model =~ /^dall-e/:
        openai-image(prompt).data.0.url
      api-model.in?(anthropic-models):
        anthropic(prompt):anthropic-message:format
      api-model.in?(groq-models):
        groq(prompt).choices.0.message.content:format
      api-model.in?(openai-models):
        openai-chat(prompt).choices.0.message.content:format
      else: die()

  say: answer

  when session:
    write session _ :append true: |+
      Q: $(orig-prompt:trim)
      A ($api-model):
      $(answer:trim)

)_",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 21, 28, "defn run(prompt session=nil)"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 53, 14, "session-text ="),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 73, 28, "when session && session:fs-e"),
           e(VAL_|SCLR|PLAI, 0, 0, ""), // note empty scalar pointing at the front
           e(EMAP),
           e(KEY_|SCLR|PLAI, 106, 8, "answer ="),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 120, 4, "cond"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 132, 22, "api-model =~ /^dall-e/"),
           e(VAL_|SCLR|PLAI, 164, 31, "openai-image(prompt).data.0.url"),
           e(KEY_|SCLR|PLAI, 202, 31, "api-model.in?(anthropic-models)"),
           e(VAL_|SCLR|PLAI, 243, 42, "anthropic(prompt):anthropic-message:format"),
           e(KEY_|SCLR|PLAI, 292, 26, "api-model.in?(groq-models)"),
           e(VAL_|SCLR|PLAI, 328, 45, "groq(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI, 380, 28, "api-model.in?(openai-models)"),
           e(VAL_|SCLR|PLAI, 418, 52, "openai-chat(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI, 477, 4, "else"),
           e(VAL_|SCLR|PLAI, 483, 5, "die()"),
           e(EMAP),
           e(EMAP),
           e(KEY_|SCLR|PLAI, 492, 3, "say"),
           e(VAL_|SCLR|PLAI, 497, 6, "answer"),
           e(KEY_|SCLR|PLAI, 507, 12, "when session"),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 525, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL, 558, 55, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n\n", needs_filter),
           e(EMAP),
           e(EMAP),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct IntEventsTestHelper
{
    IntEventsCase const& ec;
    size_t required_size_expected;
    size_t required_size_actual;
    evt::extra::EventHandlerInts handler;
    ParseEngine<evt::extra::EventHandlerInts> parser;
    std::string src_copy;
    std::vector<ievt::DataType> actual;
    IntEventsTestHelper(IntEventsCase const& ec_)
        : ec(ec_)
        , required_size_expected(ec.expected_size())
        , handler()
        , parser(&handler)
        , src_copy()
        , actual()
    {
    }
    void run_with_size(size_t const sz)
    {
        src_copy.assign(ec.yaml.str, ec.yaml.len);
        if (!sz)
        {
            handler.reset(to_substr(src_copy), nullptr, 0);
        }
        else
        {
            actual.resize(sz);
            handler.reset(to_substr(src_copy), actual.data(), (int)actual.size());
        }
        parser.parse_in_place_ev("(testyaml)", to_substr(src_copy));
        required_size_actual = (size_t)handler.m_evt_curr;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class IntEventsTest : public testing::TestWithParam<IntEventsCase> {};

TEST_P(IntEventsTest, size_large_enough)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(2u * h.required_size_expected);
    ASSERT_LT(h.required_size_actual, h.actual.size());
    h.ec.testeq(h.actual.data(), h.required_size_actual, to_csubstr(h.src_copy));
}

TEST_P(IntEventsTest, size_too_small)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(h.required_size_expected / 2u);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    h.run_with_size(h.required_size_actual);
    h.ec.testeq(h.actual.data(), h.required_size_actual, to_csubstr(h.src_copy));
}

TEST_P(IntEventsTest, size_null)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(0);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    h.run_with_size(h.required_size_actual);
    h.ec.testeq(h.actual.data(), h.required_size_actual, to_csubstr(h.src_copy));
}

INSTANTIATE_TEST_SUITE_P(IntEvents, IntEventsTest, testing::ValuesIn(test_cases));


} // namespace extra
} // namespace evt


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
