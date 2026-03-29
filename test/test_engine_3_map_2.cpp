#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

//static constexpr const bool multiline = true; // NOLINT
//static constexpr const bool singleline = false; // NOLINT


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_0,
                   Location(2, 1),
                   "foo\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_1,
                   Location(2, 3),
                   "foo\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_2,
                   Location(2, 1),
                   "foo\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_3,
                   Location(2, 2),
                   "foo\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_plain_0_4,
                   Location(2, 3),
                   "foo\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_0,
                   Location(2, 4),
                   "foo: bar\n"
                   "bad\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_1,
                   Location(3, 2),
                   "foo: bar\n"
                   "bad\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_2,
                   Location(3, 3),
                   "foo: bar\n"
                   "bad\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_plain_0_3,
                   Location(3, 4),
                   "foo: bar\n"
                   "bad\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_0,
                   Location(2, 1),
                   "'foo'\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_1,
                   Location(2, 3),
                   "'foo'\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_2,
                   Location(2, 1),
                   "'foo'\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_3,
                   Location(2, 2),
                   "'foo'\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_squo_0_4,
                   Location(2, 3),
                   "'foo'\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_0,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_1,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_2,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_squo_0_3,
                   Location(2, 6),
                   "'foo': bar\n"
                   "'bad'\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_0,
                   Location(2, 1),
                   "\"foo\"\n"
                   ":\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_1,
                   Location(2, 3),
                   "\"foo\"\n"
                   "  :\n"
                   " bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_2,
                   Location(2, 1),
                   "\"foo\"\n"
                   ": bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_3,
                   Location(2, 2),
                   "\"foo\"\n"
                   " : bar")
ENGINE_TEST_ERRLOC(colon_on_newl_after_first_dquo_0_4,
                   Location(2, 3),
                   "\"foo\"\n"
                   "  : bar")

ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_0,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   ": yes\n")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_1,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   " : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_2,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   "  : yes")
ENGINE_TEST_ERRLOC(colon_on_newl_after_second_dquo_0_3,
                   Location(2, 6),
                   "\"foo\": bar\n"
                   "\"bad\"\n"
                   "   : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_0, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    ": yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_1, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    " : yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_seq_0_2, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 6),
                    "? [seq]\n"
                    ": bar\n"
                    "[bad]\n"
                    "  : yes")

ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_0, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    ": yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_1, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    " : yes")
ENGINE_TEST_ERRLOC_(colon_on_newl_after_second_map_0_2, HAS_CONTAINER_KEYS, ExpectedErrorType::err_parse,
                    Location(3, 11),
                    "? {foo: bar}\n"
                    ": bar\n"
                    "{foo: bar}\n"
                    "  : yes")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_0, Location(1,32), "{ first: Sammy, last: Sosa }foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_0_1, Location(2, 4), "{ first: Sammy, last: Sosa }\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_0, Location(1,29), "{ first: Sammy, last: Sosa }- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_2_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n- foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_0, Location(1,29), "{ first: Sammy, last: Sosa }[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_3_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n[foo,bar]")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_0, Location(1,29), "{ first: Sammy, last: Sosa }{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_4_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n{foo: bar}")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_0, Location(1,29), "{ first: Sammy, last: Sosa }'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_5_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n'foo'")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_0, Location(1,29), "{ first: Sammy, last: Sosa }|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_6_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n|\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_0, Location(1,29), "{ first: Sammy, last: Sosa }>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_7_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n>\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_0, Location(1,33), "{ first: Sammy, last: Sosa }?foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_8_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n?\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_0, Location(1,33), "{ first: Sammy, last: Sosa }:foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_9_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n:\nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_0, Location(2, 1), "{ first: Sammy, last: Sosa }\n: foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_10_1, Location(2, 1), "{ first: Sammy, last: Sosa }\n: \nfoo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_0, Location(1,34), "{ first: Sammy, last: Sosa } &foo")
ENGINE_TEST_ERRLOC(tokens_after_mapflow_11_1, Location(2, 5), "{ first: Sammy, last: Sosa }\n&foo")

ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1, Location(1, 8), "{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2, Location(1, 8), "{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3, Location(1, 7), "{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4, Location(1, 7), "{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5, Location(1, 7), "{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_0, Location(1,9), "{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_0, Location(1,9), "{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_0, Location(1,8), "{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_0, Location(1,8), "{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_0, Location(1,8), "{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map1_1, Location(2,2), "{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map2_1, Location(2,2), "{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map3_1, Location(2,1), "{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map4_1, Location(2,1), "{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow1Map5_1, Location(2,1), "{a: b}\n-")

ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1, Location(2, 8), "---\n{a: b}]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2, Location(2, 8), "---\n{a: b}}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3, Location(2, 7), "---\n{a: b}{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4, Location(2, 7), "---\n{a: b}[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5, Location(2, 7), "---\n{a: b}-")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_0, Location(2,9), "---\n{a: b} ]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_0, Location(2,9), "---\n{a: b} }")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_0, Location(2,8), "---\n{a: b} {")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_0, Location(2,8), "---\n{a: b} [")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_0, Location(2,8), "---\n{a: b} -")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map1_1, Location(3,2), "---\n{a: b}\n]")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map2_1, Location(3,2), "---\n{a: b}\n}")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map3_1, Location(3,1), "---\n{a: b}\n{")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map4_1, Location(3,1), "---\n{a: b}\n[")
ENGINE_TEST_ERRLOC(ExtraTokensFlow2Map5_1, Location(3,1), "---\n{a: b}\n-")


} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
