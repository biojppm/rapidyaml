#include "./test_suite_parts.hpp"

namespace c4 {
namespace yml {

constexpr const CasePart_e IN______________ = CPART_IN_YAML;
constexpr const CasePart_e ___OUT______JSON =               CPART_OUT_YAML                |CPART_IN_JSON;
constexpr const CasePart_e IN_____EMIT_____ = CPART_IN_YAML               |CPART_EMIT_YAML;
constexpr const CasePart_e IN_____EMIT_JSON = CPART_IN_YAML               |CPART_EMIT_YAML|CPART_IN_JSON;

constexpr const CasePart_e eIN_________ = CPART_IN_YAML_EVENTS;
constexpr const CasePart_e e___OUT_____ =                      CPART_OUT_YAML_EVENTS;
//constexpr const CasePart_e e_______EMIT =                                            CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e eIN_OUT_____ = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS;
constexpr const CasePart_e eIN_____EMIT = CPART_IN_YAML_EVENTS|                      CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e eIN_OUT_EMIT = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML_EVENTS;


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/test/
constexpr const AllowedFailure allowed_failures[] = {

    //-------------------------------------------------------------------------
    // SECTION 1. Known issues, TODO
    //
    // These tests are temporarily skipped, and cover issues that must be fixed.

    // plain scalars (ie, not quoted, not folded)
    {"735Y", IN______________             , "plain scalar parsing"},
    {"82AN", IN______________             , "plain scalar parsing, same indentation on next line is problematic"},
    {"9YRD", IN______________             , "plain scalar parsing, same indentation on next line is problematic"},
    {"M7A3", IN______________             , "plain scalar parsing, same indentation on next line is problematic"},
    {"EXG3", IN______________             , "plain scalar parsing, same indentation on next line is problematic"},
    {"EX5H", IN_____EMIT_____             , "plain scalar parsing, same indentation on next line is problematic"},
    {"HS5T", IN______________             , "plain scalar parsing, same indentation on next line is problematic"},
    {"NB6Z",                  eIN_________, "plain scalar parsing, same indentation on next line is problematic"},
    {"35KP",                  eIN_OUT_____, "plain scalar is wrongly parsed with trailing newline"},
    {"36F6",                  eIN_________, "plain scalar is wrongly parsed with trailing newline"},
    {"H2RW",                  eIN_____EMIT, "plain scalar is wrongly parsed with trailing newline"},
    {"UGM3",                  eIN_________, "plain scalar is wrongly parsed with trailing newline"},
    // folded scalars
    {"3MYT",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"4QFQ",                  eIN_____EMIT, "folded scalars: multiline problems"},
    {"5GBF",                  eIN_________, "folded scalars: multiline problems"},
    {"5WE3",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"6VJK",                  eIN_OUT_____, "emitting folded scalars is not idempotent"},
    {"7T8X",                  eIN_OUT_____, "folded scalars: multiline problems"},
    {"7TMG",                  eIN_________, "multiline scalar is parsed wrong"},
    {"7W2P",                  eIN_________, "multiline scalar is parsed wrong"},
    {"A6F9",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"A984",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"AB8U",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"D83L",                  eIN_________, "folded scalars: multiline problems"},
    {"DWX9",                  eIN_____EMIT, "folded scalars: multiline problems"},
    {"F6MC",                  eIN_____EMIT, "emitting folded scalars is not idempotent"},
    {"JDH8",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"K858", ___OUT______JSON|eIN_________, "emitting folded scalars is not idempotent"},
    {"KSS4",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"MJS9",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"NAT4", IN_____EMIT_JSON             , "emitting folded scalars is not idempotent"},
    {"NJ66",                  eIN_________, "emitting folded scalars is not idempotent"},
    {"P2AD",                  eIN_OUT_____, "emitting folded scalars is not idempotent"},
    {"R4YG",                  eIN_OUT_____, "folded scalars: multiline problems"},
    {"RZT7",                  eIN_OUT_____, "folded scalars: multiline problems"},
    {"T26H",                  eIN_OUT_EMIT, "folded scalars: multiline problems"},
    {"T4YY",                  eIN_OUT_____, "emitting folded scalars is not idempotent"},
    {"T5N4",                  e___OUT_____, "emitting folded scalars is not idempotent"},
    {"UT92",                  eIN_OUT_____, "emitting folded scalars is not idempotent"},
    {"W4TN",                  eIN_OUT_____, "folded scalars: multiline problems"},
    {"XLQ9",                  eIN_________, "emitting folded scalars is not idempotent"},
    // quoted scalars
    {"4ZYM",                  eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"6SLA",                  eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"6WPF",                  eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"7A4E",                  eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"9TFX",                  eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"NP9H",                  eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"PRH3",                  eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"Q8AD",                  eIN_OUT_____, "quoted scalars: differences with \n,\t in single,double quotes"},
    {"TL85",                  eIN_________, "quoted scalars: differences with \n,\t in single,double quotes"},
    // implicit keys
    {"DFF7",                  eIN_________, "problem with implicit key"},
    {"FRK4",                  eIN_________, "implicit key is wrongly parsed"},
    {"V9D5",                  eIN_________, "null key is wrongly parsed"},
    {"X8DW",                  eIN_________, "null key is wrongly parsed"},
    {"ZWK4",                  eIN_________, "null key is wrongly parsed"},
    {"4ABK",                  eIN_________, "key is wrongly serialized: 'omitted value:'"},
    // document handling
    {"6XDY",                  eIN_OUT_____, "empty doc does not get an empty value added to it"},
    {"6ZKB",                  eIN_____EMIT, "document handling"},
    {"9DXL",                  eIN_____EMIT, "document handling"},
    {"B3HG",                  e___OUT_____, "document handling with folded scalar"},
    {"PUW8",                  eIN_OUT_____, "empty doc must have an empty val"},
    // other
    {"9MMW", IN______________             , "re the json/yaml incompatibility where a space is required after :"},
    {"A2M4",                  eIN_________, "fails to parse the value sequence, parses as scalar"},
    {"G4RS",                  eIN_OUT_____, "need to unescape the utf8 characters"},
    {"H3Z8",                  e___OUT_____, "need to unescape the utf8 characters"},
    // tabs
    {"6BCT", IN______________             , "tabs after - or :"},
    {"J3BT",                  eIN_OUT_____, "tabs after - or :"},


    //-------------------------------------------------------------------------
    // SECTION 2. Expected errors that fail to materialize.

    {"236B", CPART_IN_YAML_ERRORS, "wrongly accepts final scalar in a map"},
    {"3HFZ", CPART_IN_YAML_ERRORS, "wrongly accepts scalar after ..."},
    {"4EJS", CPART_IN_YAML_ERRORS, "wrongly accepts double anchor for scalar"},
    {"4JVG", CPART_IN_YAML_ERRORS, "wrongly accepts double anchor for scalar"},
    {"55WF", CPART_IN_YAML_ERRORS, "wrongly accepts invalid escape in double quoted string"},
    {"5LLU", CPART_IN_YAML_ERRORS, "wrongly accepts folded scalar with wrong indented line after spaces only"},
    {"5TRB", CPART_IN_YAML_ERRORS, "wrongly accepts document-end marker in double quoted string"},
    {"5U3A", CPART_IN_YAML_ERRORS, "wrongly accepts opening a sequence on same line as map key"},
    {"62EZ", CPART_IN_YAML_ERRORS, "wrongly accepts invalid block mapping key on same line as previous key"},
    {"6JTT", CPART_IN_YAML_ERRORS, "wrongly accepts flow sequence without terminating ]"},
    {"7LBH", CPART_IN_YAML_ERRORS, "wrongly accepts multiline double quoted implicit keys"},
    {"7MNF", CPART_IN_YAML_ERRORS, "wrongly accepts final scalar in a map"},
    {"8XDJ", CPART_IN_YAML_ERRORS, "wrongly accepts comment in multiline scalar"},
    {"9C9N", CPART_IN_YAML_ERRORS, "wrongly accepts non-indented flow sequence"},
    {"9CWY", CPART_IN_YAML_ERRORS, "wrongly accepts final scalar in a map"},
    {"9HCY", CPART_IN_YAML_ERRORS, "wrongly accepts tag directive in non-doc scope"},
    {"9JBA", CPART_IN_YAML_ERRORS, "wrongly accepts comment after flow seq terminating ]"},
    {"9MAG", CPART_IN_YAML_ERRORS, "wrongly accepts flow sequence with invalid comma at the beginning"},
    {"B63P", CPART_IN_YAML_ERRORS, "wrongly accepts directive without doc"},
    {"BU8L", CPART_IN_YAML_ERRORS, "wrongly accepts node properties spread over multiple lines"},
    {"CML9", CPART_IN_YAML_ERRORS, "wrongly accepts comment inside flow scalar"},
    {"CTN5", CPART_IN_YAML_ERRORS, "wrongly accepts flow sequence with missing elements"},
    {"CVW2", CPART_IN_YAML_ERRORS, "wrongly accepts flow sequence with comment after ,"},
    {"CXX2", CPART_IN_YAML_ERRORS, "wrongly accepts mapping with anchor on document start line"},
    {"D49Q", CPART_IN_YAML_ERRORS, "wrongly accepts multiline single quoted implicit keys"},
    {"DK4H", CPART_IN_YAML_ERRORS, "wrongly accepts implicit key followed by newline"},
    {"EB22", CPART_IN_YAML_ERRORS, "wrongly accepts missing document-end marker before directive"},
    {"GDY7", CPART_IN_YAML_ERRORS, "wrongly accepts comment that looks like a mapping key"},
    {"HRE5", CPART_IN_YAML_ERRORS, "wrongly accepts double quoted scalar with escaped single quote"},
    {"JY7Z", CPART_IN_YAML_ERRORS, "wrongly accepts trailing content that looks like a mapping"},
    {"KS4U", CPART_IN_YAML_ERRORS, "wrongly accepts item after end of flow sequence"},
    {"LHL4", CPART_IN_YAML_ERRORS, "wrongly accepts tag"},
    {"N782", CPART_IN_YAML_ERRORS, "wrongly accepts document markers in flow style"},
    {"P2EQ", CPART_IN_YAML_ERRORS, "wrongly accepts sequence item on same line as previous item"},
    {"QB6E", CPART_IN_YAML_ERRORS, "wrongly accepts indented multiline quoted scalar"},
    {"RHX7", CPART_IN_YAML_ERRORS, "wrongly accepts directive without document end marker"},
    {"RXY3", CPART_IN_YAML_ERRORS, "wrongly accepts document-end marker in single quoted string"},
    {"S4GJ", CPART_IN_YAML_ERRORS, "wrongly accepts text after block scalar indicator"},
    {"S98Z", CPART_IN_YAML_ERRORS, "wrongly accepts block scalar with more spaces than first content line"},
    {"SF5V", CPART_IN_YAML_ERRORS, "wrongly accepts duplicate YAML directive"},
    {"SU5Z", CPART_IN_YAML_ERRORS, "wrongly accepts comment without whitespace after double quoted scalar"},
    {"SU74", CPART_IN_YAML_ERRORS, "wrongly accepts anchor and alias as mapping key"},
    {"SY6V", CPART_IN_YAML_ERRORS, "wrongly accepts anchor before sequence entry on same line"},
    {"T833", CPART_IN_YAML_ERRORS, "wrongly accepts flow mapping missing a separating comma"},
    {"X4QW", CPART_IN_YAML_ERRORS, "wrongly accepts comment without whitespace after block scalar indicator"},
    {"ZCZ6", CPART_IN_YAML_ERRORS, "wrongly accepts invalid mapping in plain single line value"},


    //-------------------------------------------------------------------------
    // SECTION 3. Known, deliberate ryml limitations.
    //
    // These tests are skipped because they cover parts of YAML that
    // are deliberately not implemented by ryml.

    // container keys are not supportted
    {"4FJ6", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"6BFJ", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"6PBE", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"KK5P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"KZN9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"LX3P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"M5DY", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"Q9WF", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"RZP5", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"SBG9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"X38W", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    {"XW4D", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs)"},
    // anchors with : are not supported
    {"2SXE", CPART_IN_YAML|CPART_OUT_YAML, "weird characters in anchors, anchors must not end with :"},
    {"W5VH", CPART_IN_YAML, "weird characters in anchors"},
    // tags are parsed as-is; tag lookup is not supported
    {"5TYM", eIN_________, "we do not do tag lookup"},
    {"6CK3", eIN_________, "we do not do tag lookup"},
    {"6WLZ", eIN_________, "we do not do tag lookup"},
    {"9WXW", eIN_________, "we do not do tag lookup"},
    {"C4HZ", eIN_________, "we do not do tag lookup"},
    {"CC74", eIN_________, "we do not do tag lookup"},
    {"P76L", eIN_________, "we do not do tag lookup"},
    {"QLJ7", CPART_IN_YAML_ERRORS, "we do not do tag lookup"},
    {"U3C3", eIN_________, "we do not do tag lookup"},
    {"Z9M4", eIN_________, "we do not do tag lookup"},
    // malformed json in the test spec
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
    {"6FWR", e___OUT_____, "out-yaml is missing the --- document token, which is required in the events"},
    {"EX5H", e___OUT_____, "out-yaml is missing the --- document token, which is required in the events"},
    {"EXG3", e___OUT_____, "out-yaml is missing the --- document token, which is required in the events"},
};


cspan<AllowedFailure> g_allowed_failures = allowed_failures;


} // namespace c4
} // namespace yml
