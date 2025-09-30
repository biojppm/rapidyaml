#include "test_lib/test_case.hpp"
#include "test_lib/test_events_ints_helpers.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {


struct IntEventsCase
{
    const char *file;
    const int line;
    ParserOptions opts;
    csubstr yaml;
    const std::vector<IntEventWithScalar> evt;

    void testeq(csubstr parsed_source, csubstr arena, ievt::DataType const* actual, size_t actual_size) const
    {
        RYML_TRACE_FMT("defined in:\n{}:{}: (here)\n", file, line);
        #ifdef RYML_DBG
        events_ints_print(parsed_source, arena, actual, (extra::ievt::DataType)actual_size);
        #endif
        test_events_ints_invariants(parsed_source, arena, actual, (ievt::DataType)actual_size);
        test_events_ints(evt.data(), evt.size(), actual, actual_size, yaml, parsed_source, arena);
    }
};
// this is required to work around a valgrind problem in gtest's
// printing of the test byte contents. We use the opportunity to print
// a line showing the location where the test case was defined.
std::ostream& operator<<(std::ostream& os, const IntEventsCase& e)
{
    return os << "line[" << e.line << "]:\n" << e.file << ":" << e.line << ": (here)";
}


//-----------------------------------------------------------------------------
#define DECLARE_CSUBSTR_FROM_CHAR_ARR(name, ...) \
    const char name##_[] = { __VA_ARGS__ }; \
    csubstr name = {name##_, C4_COUNTOF(name##_)}

DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L6,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P6,
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
         _RYML_CHCONST(-0x1e, 0xe2), _RYML_CHCONST(-0x80, 0x80), _RYML_CHCONST(-0x57, 0xa9),
    );

using namespace ievt;
const bool needs_filter = true;
const IntEventsCase test_cases[] = {
    // make the declarations shorter
    #define e(...) IntEventWithScalar{__VA_ARGS__}
    #define tc_(opts, ys, ...) IntEventsCase{__FILE__, __LINE__, opts, ys, std::initializer_list<IntEventWithScalar>(__VA_ARGS__)}
    #define tc(ys, ...) tc_(ParserOptions{}, ys, __VA_ARGS__)
    // case -------------------------------------------------
    tc("!yamlscript/v0/bare\n--- !code\n42\n",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|TAG_, 0, 19, "!yamlscript/v0/bare"),
           e(VAL_|SCLR|PLAI|PSTR, 0, 0, ""),
           e(EDOC|PSTR),
           e(BDOC|EXPL),
           e(VAL_|TAG_, 24, 5, "!code"),
           e(VAL_|SCLR|PLAI|PSTR, 30, 2, "42"),
           e(EDOC|PSTR),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("a: 1",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 0, 1, "a"),
           e(VAL_|SCLR|PLAI|PSTR, 3, 1, "1"),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 5, 5, "2 + 2"),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "✅"),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 7, 1, "c"),
           e(ESEQ|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "b"),
           e(EMAP|PSTR),
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
           e(VAL_|TAG_, 4, 14, "!yamlscript/v0"),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 19, 3, "foo"),
           e(VAL_|TAG_|PSTR, 24, 1, "!"),
           e(VAL_|BSEQ|BLCK|PSTR),
           e(VAL_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 29, 1, "x"),
           e(VAL_|SCLR|PLAI|PSTR, 32, 1, "y"),
           e(EMAP|PSTR),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 38, 1, "x"),
           e(VAL_|SCLR|PLAI|PSTR, 41, 1, "y"),
           e(ESEQ|PSTR),
           e(VAL_|SCLR|PLAI, 46, 3, "foo"),
           e(VAL_|SCLR|SQUO|PSTR, 53, 3, "foo"),
           e(VAL_|SCLR|DQUO|PSTR, 61, 3, "foo"),
           e(VAL_|SCLR|LITL|PSTR, 70, 4, "foo\n", needs_filter),
           e(VAL_|SCLR|FOLD|PSTR, 80, 4, "foo\n", needs_filter),
           e(VAL_|BSEQ|FLOW|PSTR),
           e(VAL_|SCLR|PLAI, 89, 1, "1"),
           e(VAL_|SCLR|PLAI|PSTR, 92, 1, "2"),
           e(VAL_|SCLR|PLAI|PSTR, 95, 4, "true"),
           e(VAL_|SCLR|PLAI|PSTR, 101, 5, "false"),
           e(VAL_|SCLR|PLAI|PSTR, 108, 4, "null"),
           e(ESEQ|PSTR),
           e(VAL_|TAG_, 126, 6, "!tag-1"),
           e(VAL_|ANCH|PSTR, 117, 8, "anchor-1"),
           e(VAL_|SCLR|PLAI|PSTR, 133, 6, "foobar"),
           e(ESEQ|PSTR),
           e(EMAP),
           e(EDOC),
           e(BDOC|EXPL),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|PLAI, 144, 7, "another"),
           e(VAL_|SCLR|PLAI|PSTR, 153, 3, "doc"),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 7, 10, "well a b c"),
           e(KEY_|SCLR|PLAI|PSTR, 24, 4, "squo"),
           e(VAL_|SCLR|SQUO|PSTR, 31, 12, "single'quote", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 46, 4, "dquo"),
           e(VAL_|SCLR|DQUO|PSTR, 53, 4, "x\t\ny", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 61, 3, "lit"),
           e(VAL_|SCLR|LITL|PSTR, 68, 6, "X\nY\nZ\n", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 89, 4, "fold"),
           e(VAL_|SCLR|FOLD|PSTR, 97, 6, "U V W\n", needs_filter),
           e(EMAP|PSTR),
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
           e(VAL_|BSEQ|FLOW|PSTR),
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
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 32, 12, "when session"),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 50, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL|PSTR, 83, 54, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n", needs_filter),
           e(EMAP|PSTR),
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
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 53, 14, "session-text ="),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 73, 28, "when session && session:fs-e"),
           e(VAL_|SCLR|PLAI|PSTR, 0, 0, ""), // note empty scalar pointing at the front
           e(EMAP|PSTR),
           e(KEY_|SCLR|PLAI, 106, 8, "answer ="),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 120, 4, "cond"),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 132, 22, "api-model =~ /^dall-e/"),
           e(VAL_|SCLR|PLAI|PSTR, 164, 31, "openai-image(prompt).data.0.url"),
           e(KEY_|SCLR|PLAI|PSTR, 202, 31, "api-model.in?(anthropic-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 243, 42, "anthropic(prompt):anthropic-message:format"),
           e(KEY_|SCLR|PLAI|PSTR, 292, 26, "api-model.in?(groq-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 328, 45, "groq(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI|PSTR, 380, 28, "api-model.in?(openai-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 418, 52, "openai-chat(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI|PSTR, 477, 4, "else"),
           e(VAL_|SCLR|PLAI|PSTR, 483, 5, "die()"),
           e(EMAP|PSTR),
           e(EMAP),
           e(KEY_|SCLR|PLAI, 492, 3, "say"),
           e(VAL_|SCLR|PLAI|PSTR, 497, 6, "answer"),
           e(KEY_|SCLR|PLAI|PSTR, 507, 12, "when session"),
           e(VAL_|BMAP|BLCK|PSTR),
           e(KEY_|SCLR|PLAI, 525, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL|PSTR, 558, 55, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n\n", needs_filter),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 11, 5, "a b\nc"),
           e(EMAP|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "map"),
           e(EMAP|PSTR),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 13, 3, "seq"),
           e(VAL_|SCLR|PLAI|PSTR, 18, 3, "val"),
           e(ESEQ|PSTR),
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
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "seq"),
           e(ESEQ|PSTR),
           e(VAL_|BMAP|FLOW),
           e(KEY_|SCLR|PLAI, 13, 3, "map"),
           e(VAL_|SCLR|PLAI|PSTR, 18, 3, "val"),
           e(EMAP|PSTR),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[b,c]: d"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 1, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 3, 1, "c"),
           e(ESEQ|PSTR),
           e(VAL_|SCLR|PLAI, 7, 1, "d"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[[b,c]]: d"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|BSEQ|FLOW),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 2, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "c"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 9, 1, "d"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[ a, [ [[b,c]]: d, e]]: 23"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 2, 1, "a"),
           e(VAL_|BSEQ|FLOW|PSTR),
           e(VAL_|BMAP|FLOW),
           e(KEY_|BSEQ|FLOW),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|PLAI, 9, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 11, 1, "c"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 16, 1, "d"),
           e(EMAP|PSTR),
           e(VAL_|SCLR|PLAI, 19, 1, "e"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 24, 2, "23"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // this requires the arena:
    // every filtered \L or \P expands 2->3 bytes
    tc("[\"\\L\\L\\L\\L\\L\\L\", \"\\P\\P\\P\\P\\P\\P\"]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW),
           e(VAL_|SCLR|DQUO|AREN, 0, 18, dqesc_L6),
           e(VAL_|SCLR|DQUO|AREN|PSTR, 18, 18, dqesc_P6),
           e(ESEQ|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // this requires the arena: directives cause the tag to expand
    tc("%YAML 1.2\n"
       "---\n"
       "- !light fluorescent\n"
       "- !m something\n"
       "...\n"
       "%TAG !m! !extraordinary-\n"
       "---\n"
       "- !m!light green\n"
       "- !!int 2\n"
       ,
       {
           e(BSTR),
           e(YAML,                  6,  3, "1.2"),
           e(BDOC|EXPL|PSTR),
           e(VAL_|BSEQ|BLCK),
           e(VAL_|TAG_,            16,  6, "!light"),
           e(VAL_|SCLR|PLAI|PSTR,  23, 11, "fluorescent"),
           e(VAL_|TAG_|PSTR,       37,  2, "!m"),
           e(VAL_|SCLR|PLAI|PSTR,  40,  9, "something"),
           e(ESEQ|PSTR),
           e(EDOC|EXPL),
           e(TAGD,                 59,  3, "!m!"),
           e(TAGV|PSTR,            63, 15, "!extraordinary-"),
           e(BDOC|EXPL|PSTR),
           e(VAL_|BSEQ|BLCK),
           e(VAL_|TAG_|AREN,        0, 20, "!extraordinary-light"),
           e(VAL_|SCLR|PLAI|PSTR,  94,  5, "green"),
           e(VAL_|TAG_|PSTR,      102,  5, "!!int"),
           e(VAL_|SCLR|PLAI|PSTR, 108,  1, "2"),
           e(ESEQ|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("\"\\\"foo\\\"\": \"\\\"bar\\\"\"",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|DQUO,       1, 5, "\"foo\""),
           e(VAL_|SCLR|DQUO|PSTR, 12, 5, "\"bar\""),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc_(ParserOptions{}.scalar_filtering(false),
       "\"\\\"foo\\\"\": \"\\\"bar\\\"\"",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP|BLCK),
           e(KEY_|SCLR|DQUO|UNFILT,       1, 7, "\\\"foo\\\""),
           e(VAL_|SCLR|DQUO|UNFILT|PSTR, 12, 7, "\\\"bar\\\""),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // tests an extending scalar
    tc("|\n"
       "abc"  // no newline at end
       ,
       {
           e(BSTR),
           e(BDOC),
           // instead of this (in the arena):
           //e(VAL_|SCLR|LITL|AREN, 0, 4, "abc\n"), // result has additional newline
           // we get this (shifted left by 1, in the source code)
           e(VAL_|SCLR|LITL, 1, 4, "abc\n"), // result has additional newline
           e(EDOC|PSTR),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // tests an extending scalar
    tc(">\n"
       "abc"  // no newline at end
       ,
       {
           e(BSTR),
           e(BDOC),
           // instead of this (in the arena):
           //e(VAL_|SCLR|FOLD|AREN, 0, 4, "abc\n"), // result has additional newline
           // we get this (shifted left by 1, in the source code)
           e(VAL_|SCLR|FOLD, 1, 4, "abc\n"), // result has additional newline
           e(EDOC|PSTR),
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
    std::string arena;
    IntEventsTestHelper(IntEventsCase const& ec_)
        : ec(ec_)
        , required_size_expected(num_ints(ec.evt.data(), ec.evt.size()))
        , handler()
        , parser(&handler, ec_.opts)
        , src_copy()
        , actual()
    {
    }
    void run_with_size(size_t sz, size_t arena_sz)
    {
        src_copy.assign(ec.yaml.str, ec.yaml.len);
        actual.resize(sz);
        arena.resize(arena_sz);
        handler.reset(to_substr(src_copy), to_substr(arena), actual.data(), (int)actual.size());
        parser.parse_in_place_ev("(testyaml)", to_substr(src_copy));
        required_size_actual = (size_t)handler.required_size_events();
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
    h.run_with_size(2u * h.required_size_expected, 2u * h.ec.yaml.len);
    ASSERT_LT(h.required_size_actual, h.actual.size());
    ASSERT_LT(h.handler.required_size_arena(), h.arena.size());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
}

TEST_P(IntEventsTest, size_too_small)
{
    IntEventsTestHelper h(GetParam());
    size_t small = h.required_size_expected / 2u;
    h.run_with_size(small, 0u);
    ASSERT_EQ(h.actual.size(), small);
    ASSERT_EQ(h.arena.size(), 0u);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    ASSERT_FALSE(h.handler.fits_buffers());
    _c4dbgpf("retry! reqbuf={} reqarena={}", h.required_size_actual, h.handler.required_size_arena());
    h.run_with_size(h.required_size_actual, h.handler.required_size_arena());
    ASSERT_EQ(h.actual.size(), h.handler.required_size_events());
    ASSERT_EQ(h.arena.size(), h.handler.required_size_arena());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
}

TEST_P(IntEventsTest, size_null)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(0, 0);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    h.run_with_size(h.required_size_actual, h.handler.required_size_arena());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
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
RYML_EXPORT Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
