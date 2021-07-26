#include "./test_suite_parts.hpp"

namespace c4 {
namespace yml {


constexpr const CasePart_e eIN_________ = CPART_IN_YAML_EVENTS;
constexpr const CasePart_e e___OUT_____ =                      CPART_OUT_YAML_EVENTS;
constexpr const CasePart_e e_______EMIT =                                            CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e eIN_OUT_____ = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS;
constexpr const CasePart_e eIN_____EMIT = CPART_IN_YAML_EVENTS|                      CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e e___OUT_EMIT =                      CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e eIN_OUT_EMIT = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML_EVENTS;


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/test/
constexpr const AllowedFailure allowed_failures[] = {

    //-------------------------------------------------------------------------
    // SECTION 1. Known issues, TODO
    //
    // These tests are temporarily skipped, and cover issues that must be fixed.

    // block scalars
    {"3MYT", eIN_________, "emitting block scalars is not idempotent"},
    {"4CQQ", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"4QFQ", eIN_OUT_EMIT, "emitting block scalars is not idempotent"},
    {"5WE3", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"6HB6", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"6JQW", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"6VJK", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"7TMG", eIN_________, "multiline scalar is parsed wrong"},
    {"7W2P", eIN_________, "multiline scalar is parsed wrong"},
    {"A6F9", eIN_________, "emitting block scalars is not idempotent"},
    {"A984", eIN_________, "emitting block scalars is not idempotent"},
    {"AB8U", eIN_________, "emitting block scalars is not idempotent"},
    {"D83L", eIN_________, "block scalars: multiline problems"},
    {"DWX9", eIN_____EMIT, "block scalars: multiline problems"},
    {"5GBF", eIN_________, "block scalars: multiline problems"},
    {"R4YG", eIN_OUT_____, "block scalars: multiline problems"},
    {"RZT7", eIN_OUT_____, "block scalars: multiline problems"},
    {"T26H", eIN_OUT_EMIT, "block scalars: multiline problems"},
    {"W4TN", eIN_OUT_____, "block scalars: multiline problems"},
    {"DK3J", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"F6MC", eIN_____EMIT, "emitting block scalars is not idempotent"},
    {"FP8R", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"JDH8", eIN_________, "emitting block scalars is not idempotent"},
    {"K527", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"KSS4", eIN_________, "emitting block scalars is not idempotent"},
    {"M7A3", CPART_IN_YAML|e_______EMIT, "emitting block scalars is not idempotent"},
    {"MJS9", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"K858", CPART_OUT_YAML|CPART_IN_JSON|eIN_________, "emitting block scalars is not idempotent"},
    {"NAT4", CPART_IN_YAML|CPART_EMIT_YAML|CPART_IN_JSON, "emitting block scalars is not idempotent"},
    {"NJ66", eIN_________, "emitting block scalars is not idempotent"},
    {"P2AD", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"T4YY", eIN_OUT_EMIT, "emitting block scalars is not idempotent"},
    {"T5N4", eIN_OUT_EMIT, "emitting block scalars is not idempotent"},
    {"TS54", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"UT92", eIN_OUT_____, "emitting block scalars is not idempotent"},
    {"XLQ9", eIN_________, "emitting block scalars is not idempotent"},
    // plain scalars
    {"RZP5", CPART_IN_YAML|eIN_OUT_____, "plain scalar block parsing, anchors"},
    {"735Y", CPART_IN_YAML, "plain scalar parsing"},
    {"7T8X", CPART_IN_YAML|CPART_OUT_YAML, "scalar block parsing"},
    {"82AN", CPART_IN_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"9YRD", CPART_IN_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"EXG3", CPART_IN_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"EX5H", CPART_IN_YAML|CPART_EMIT_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"NB6Z", eIN_________, "plain scalar parsing, same indentation on next line is problematic"},
    {"HS5T", CPART_IN_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"35KP", eIN_OUT_____, "plain scalar is wrongly parsed with trailing newline"},
    {"36F6", eIN_OUT_____, "plain scalar is wrongly parsed with trailing newline"},
    {"4ZYM", eIN_OUT_EMIT, "plain scalar is wrongly parsed with trailing newline"},
    {"H2RW", eIN_____EMIT, "plain scalar is wrongly parsed with trailing newline"},
    {"UGM3", eIN_________, "plain scalar is wrongly parsed with trailing newline"},
    // quoted scalars
    {"6SLA", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"6WPF", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"7A4E", eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"9TFX", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"CPZ3", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"NP9H", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"PRH3", eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"Q8AD", eIN_OUT_EMIT, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"TL85", eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    // implicit keys
    {"DFF7", eIN_________, "problem with implicit key"},
    {"FH7J", CPART_IN_YAML|CPART_OUT_YAML, "implicit keys"},
    {"FRK4", eIN_________, "implicit key is wrongly parsed"},
    {"V9D5", eIN_________, "null key is wrongly parsed"},
    {"X8DW", eIN_________, "null key is wrongly parsed"},
    {"ZWK4", eIN_________, "null key is wrongly parsed"},
    {"3UYS", CPART_IN_YAML, "no need to escape the slash in \"a\\/b\""},
    {"4ABK", CPART_IN_YAML_EVENTS, "key is wrongly serialized: 'omitted value:'"},
    {"PW8X", CPART_IN_YAML|CPART_OUT_YAML, "anchors with implicit key"},
    // other
    {"6BCT", CPART_IN_YAML, "allow tabs after - or :"},
    {"J3BT", eIN_OUT_____, "allow tabs after - or :"},
    {"K54U", eIN_________, "problem with tab after ---"},
    {"DC7X", CPART_IN_YAML, "improve handling of tab characters"},
    {"6FWR", CPART_EMIT_YAML|eIN_OUT_____, "fail to parse"},
    {"9MMW", CPART_IN_YAML, "re the json/yaml incompatibility where a space is required after :"},
    {"A2M4", eIN_________, "fails to parse the value sequence, parses as scalar"},
    {"CN3R", CPART_IN_YAML|CPART_OUT_YAML, "anchors + maps nested in seqs"},
    {"G5U8", CPART_ALL, "sequences with -"},
    {"G4RS", eIN_OUT_____, "need to unescape the utf8 characters"},
    {"H3Z8", e___OUT_____, "need to unescape the utf8 characters"},
    {"PUW8", eIN_OUT_____, "empty doc must have an empty val"},
    {"RTP8", eIN_________, "empty doc wrongly added at end"},
    {"Q5MG", eIN_________, "'   {}' is wrongly parsed as a val"},
    {"6XDY", eIN_OUT_____, "empty doc does not get an empty value added to it"},
    {"6ZKB", eIN_____EMIT, "document handling"},
    {"9DXL", eIN_____EMIT, "document handling"},
    {"B3HG", e___OUT_____, "document handling with folded scalar"},
    {"EHF6", eIN_________, "tag is incorrectly assigned to the child node"},


    //-------------------------------------------------------------------------
    // SECTION 2. Known ryml limitations.
    //
    // These tests are skipped as they cover parts of YAML that are deliberately
    // not implemented by ryml.

    // we do not accept container keys
    {"4FJ6", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"6BFJ", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"6PBE", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"KK5P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"KZN9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"LX3P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"M5DY", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"Q9WF", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"SBG9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"X38W", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"XW4D", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    // we do not accept anchors with :
    {"2SXE", CPART_IN_YAML|CPART_OUT_YAML, "weird characters in anchors, anchors must not end with :"},
    {"W5VH", CPART_IN_YAML, "weird characters in anchors"},
    // tags are parsed as-is
    {"5TYM", eIN_________, "we do not do tag lookup"},
    {"6CK3", eIN_________, "we do not do tag lookup"},
    {"6WLZ", eIN_________, "we do not do tag lookup"},
    {"9WXW", eIN_________, "we do not do tag lookup"},
    {"C4HZ", eIN_________, "we do not do tag lookup"},
    {"CC74", eIN_________, "we do not do tag lookup"},
    {"P76L", eIN_________, "we do not do tag lookup"},
    {"U3C3", eIN_________, "we do not do tag lookup"},
    {"Z9M4", eIN_________, "we do not do tag lookup"},
    // malformed json
    {"35KP", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"6XDY", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"6ZKB", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"7Z25", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"9DXL", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"9KAX", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"JHB9", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"KSS4", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"M7A3", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"RZT7", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"U9NS", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"W4TN", CPART_IN_JSON, "malformed JSON from multiple documents"},

    // malformed test spec?
    {"EX5H", e___OUT_____, "out-yaml is missing the --- document token, which is required in the events"},
    {"EXG3", e___OUT_____, "out-yaml is missing the --- document token, which is required in the events"},
};


cspan<AllowedFailure> g_allowed_failures = allowed_failures;


} // namespace c4
} // namespace yml
