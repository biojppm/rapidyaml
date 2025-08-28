#include "test_lib/test_case.hpp"
#include "test_lib/test_events_int.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>


namespace c4 {
namespace yml {
namespace extra {


struct IntEventsCase
{
    const char *file;
    const int line;
    csubstr yaml;
    const std::vector<IntEventWithScalar> evt;

    void testeq(ievt::DataType const* actual, size_t actual_size, csubstr parsed_source) const
    {
        test_events_ints(evt.data(), evt.size(), actual, actual_size, yaml, parsed_source, file, line);
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
    tc(""
       "--- !yamlscript/v0\n"
       "foo: !\n"
       "- {x: y}\n"
       "- [x, y]\n"
       "- foo\n"
       "- 'foo'\n"
       "- \"foo\"\n"
       "- |\n"
       "  foo\n"
       "- >\n"
       "  foo\n"
       "- [1, 2, true, false, null]\n"
       "- &anchor-1 !tag-1 foobar\n"
       "---\n"
       "another: doc\n"
       "",
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
    tc("plain: well\n"
       "  a\n"
       "  b\n"
       "  c\n"
       "squo: 'single''quote'\n"
       "dquo: \"x\\t\\ny\"\n"
       "lit: |\n"
       "     X\n"
       "     Y\n"
       "     Z\n"
       "fold: >\n"
       "     U\n"
       "     V\n"
       "     W\n"
       ,
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
    tc(""
       "defn run(prompt session=nil):\n"
       "  when session:\n"
       "    write session _ :append true: |+\n"
       "      Q: $(orig-prompt:trim)\n"
       "      A ($api-model):\n"
       "      $(answer:trim)\n"
       ""
       ,
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
    tc("#!/usr/bin/env ys-0\n"
       "\n"
       "defn run(prompt session=nil):\n"
       "  session-text =:\n"
       "    when session && session:fs-e:\n"
       "\n"
       "  answer =:\n"
       "    cond:\n"
       "      api-model =~ /^dall-e/:\n"
       "        openai-image(prompt).data.0.url\n"
       "      api-model.in?(anthropic-models):\n"
       "        anthropic(prompt):anthropic-message:format\n"
       "      api-model.in?(groq-models):\n"
       "        groq(prompt).choices.0.message.content:format\n"
       "      api-model.in?(openai-models):\n"
       "        openai-chat(prompt).choices.0.message.content:format\n"
       "      else: die()\n"
       "\n"
       "  say: answer\n"
       "\n"
       "  when session:\n"
       "    write session _ :append true: |+\n"
       "      Q: $(orig-prompt:trim)\n"
       "      A ($api-model):\n"
       "      $(answer:trim)\n"
       "\n"
       ,
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
    // case -------------------------------------------------
    tc(""
       "---\n"
       "plain: a\n"
       " b\n"
       "\n"
       " c\n"
       ,
       {
           e(BSTR),
           e(BDOC|EXPL),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 4, 5, "plain"),
           e(VAL_|SCLR|PLAI, 11, 5, "a b\nc"),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("{key: map}: [seq, val]"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 1, 3, "key"),
           e(VAL_|SCLR|PLAI, 6, 3, "map"),
           e(EMAP),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 13, 3, "seq"),
           e(VAL_|SCLR|PLAI, 18, 3, "val"),
           e(ESEQ),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[key, seq]: {map: val}"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 1, 3, "key"),
           e(VAL_|SCLR|PLAI, 6, 3, "seq"),
           e(ESEQ),
           e(VAL_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 13, 3, "map"),
           e(VAL_|SCLR|PLAI, 18, 3, "val"),
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
    extra::EventHandlerInts handler;
    ParseEngine<extra::EventHandlerInts> parser;
    std::string src_copy;
    std::vector<DataType> actual;
    IntEventsTestHelper(IntEventsCase const& ec_)
        : ec(ec_)
        , required_size_expected(num_ints(ec.evt.data(), ec.evt.size()))
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

struct IntEventsTest : public testing::TestWithParam<IntEventsCase>
{
    // force an ascii name (some characters in the parameter are UTF8)
    static std::string name2str(const testing::TestParamInfo<ParamType>& info)
    {
        std::string s = c4::catrs<std::string>("line_", info.param.line);
        for (char &c : s)
            if (!std::isalnum(c))
                c = '_';
        return s;
    }
};

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

INSTANTIATE_TEST_SUITE_P(IntEvents, IntEventsTest, testing::ValuesIn(test_cases), &IntEventsTest::name2str);


} // namespace extra


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
