#include "./test_suite_parts.hpp"

namespace c4 {
namespace yml {

constexpr const CasePart_e IN______________ = CPART_IN_YAML;
//constexpr const CasePart_e ___OUT______JSON =               CPART_OUT_YAML                |CPART_IN_JSON;
//constexpr const CasePart_e IN_____EMIT_____ = CPART_IN_YAML               |CPART_EMIT_YAML;
//constexpr const CasePart_e IN_____EMIT_JSON = CPART_IN_YAML               |CPART_EMIT_YAML|CPART_IN_JSON;

constexpr const CasePart_e eIN_________ = CPART_IN_YAML_EVENTS;
constexpr const CasePart_e e___OUT_____ =                      CPART_OUT_YAML_EVENTS;
//constexpr const CasePart_e e_______EMIT =                                            CPART_EMIT_YAML_EVENTS;
constexpr const CasePart_e eIN_OUT_____ = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS;
//constexpr const CasePart_e eIN_____EMIT = CPART_IN_YAML_EVENTS|                      CPART_EMIT_YAML_EVENTS;
//constexpr const CasePart_e eIN_OUT_EMIT = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML_EVENTS;


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/test/
constexpr const AllowedFailure allowed_failures[] = {

    //-------------------------------------------------------------------------
    // SECTION 1. Known issues, TODO
    //
    // These tests are temporarily skipped, and cover issues that must be fixed.

    // block scalars
    {"K858",                  eIN_________, "emitting block scalars is not idempotent"},
    // explicit keys
    {"5WE3",                  eIN_________, "explicit key is wrongly parsed"},
    {"DFF7",                  eIN_________, "problem with missing explicit key"},
    {"FRK4",                  eIN_________, "explicit key is wrongly parsed"},
    {"NJ66",                  eIN_________, "explicit key is wrongly parsed"},
    // other
    {"9MMW", IN______________|eIN_________, "re the json/yaml incompatibility where a space is required after :"},
    {"A2M4",                  eIN_________, "fails to parse the value sequence, parses as scalar"},
    // problems reading the test spec
    {"G4RS",                  eIN_OUT_____, "need to unescape the utf8 characters"},
    {"H3Z8",                  e___OUT_____, "need to unescape the utf8 characters"},


    //-------------------------------------------------------------------------
    // SECTION 2. Expected errors that fail to materialize.

    {"236B", CPART_IN_YAML_ERRORS, "should not accept final scalar in a map"},
    {"3HFZ", CPART_IN_YAML_ERRORS, "should not accept scalar after ..."},
    {"4EJS", CPART_IN_YAML_ERRORS, "should not accept double anchor for scalar"},
    {"4JVG", CPART_IN_YAML_ERRORS, "should not accept double anchor for scalar"},
    {"55WF", CPART_IN_YAML_ERRORS, "should not accept invalid escape in double quoted string"},
    {"5LLU", CPART_IN_YAML_ERRORS, "should not accept folded scalar with wrong indented line after spaces only"},
    {"5TRB", CPART_IN_YAML_ERRORS, "should not accept document-end marker in double quoted string"},
    {"5U3A", CPART_IN_YAML_ERRORS, "should not accept opening a sequence on same line as map key"},
    {"62EZ", CPART_IN_YAML_ERRORS, "should not accept invalid block mapping key on same line as previous key"},
    {"6JTT", CPART_IN_YAML_ERRORS, "should not accept flow sequence without terminating ]"},
    {"7LBH", CPART_IN_YAML_ERRORS, "should not accept multiline double quoted implicit keys"},
    {"7MNF", CPART_IN_YAML_ERRORS, "should not accept final scalar in a map"},
    {"8XDJ", CPART_IN_YAML_ERRORS, "should not accept comment in multiline scalar"},
    {"9C9N", CPART_IN_YAML_ERRORS, "should not accept non-indented flow sequence"},
    {"9CWY", CPART_IN_YAML_ERRORS, "should not accept final scalar in a map"},
    {"9HCY", CPART_IN_YAML_ERRORS, "should not accept tag directive in non-doc scope"},
    {"9JBA", CPART_IN_YAML_ERRORS, "should not accept comment after flow seq terminating ]"},
    {"9MAG", CPART_IN_YAML_ERRORS, "should not accept flow sequence with invalid comma at the beginning"},
    {"B63P", CPART_IN_YAML_ERRORS, "should not accept directive without doc"},
    {"BU8L", CPART_IN_YAML_ERRORS, "should not accept node properties spread over multiple lines"},
    {"CML9", CPART_IN_YAML_ERRORS, "should not accept comment inside flow scalar"},
    {"CTN5", CPART_IN_YAML_ERRORS, "should not accept flow sequence with missing elements"},
    {"CVW2", CPART_IN_YAML_ERRORS, "should not accept flow sequence with comment after ,"},
    {"CXX2", CPART_IN_YAML_ERRORS, "should not accept mapping with anchor on document start line"},
    {"D49Q", CPART_IN_YAML_ERRORS, "should not accept multiline single quoted implicit keys"},
    {"DK4H", CPART_IN_YAML_ERRORS, "should not accept implicit key followed by newline"},
    {"EB22", CPART_IN_YAML_ERRORS, "should not accept missing document-end marker before directive"},
    {"GDY7", CPART_IN_YAML_ERRORS, "should not accept comment that looks like a mapping key"},
    {"HRE5", CPART_IN_YAML_ERRORS, "should not accept double quoted scalar with escaped single quote"},
    {"JY7Z", CPART_IN_YAML_ERRORS, "should not accept trailing content that looks like a mapping"},
    {"KS4U", CPART_IN_YAML_ERRORS, "should not accept item after end of flow sequence"},
    {"LHL4", CPART_IN_YAML_ERRORS, "should not accept tag"},
    {"N782", CPART_IN_YAML_ERRORS, "should not accept document markers in flow style"},
    {"P2EQ", CPART_IN_YAML_ERRORS, "should not accept sequence item on same line as previous item"},
    {"QB6E", CPART_IN_YAML_ERRORS, "should not accept indented multiline quoted scalar"},
    {"RHX7", CPART_IN_YAML_ERRORS, "should not accept directive without document end marker"},
    {"RXY3", CPART_IN_YAML_ERRORS, "should not accept document-end marker in single quoted string"},
    {"S4GJ", CPART_IN_YAML_ERRORS, "should not accept text after block scalar indicator"},
    {"S98Z", CPART_IN_YAML_ERRORS, "should not accept block scalar with more spaces than first content line"},
    {"SF5V", CPART_IN_YAML_ERRORS, "should not accept duplicate YAML directive"},
    {"SU5Z", CPART_IN_YAML_ERRORS, "should not accept comment without whitespace after double quoted scalar"},
    {"SU74", CPART_IN_YAML_ERRORS, "should not accept anchor and alias as mapping key"},
    {"SY6V", CPART_IN_YAML_ERRORS, "should not accept anchor before sequence entry on same line"},
    {"T833", CPART_IN_YAML_ERRORS, "should not accept flow mapping missing a separating comma"},
    {"X4QW", CPART_IN_YAML_ERRORS, "should not accept comment without whitespace after block scalar indicator"},
    {"ZCZ6", CPART_IN_YAML_ERRORS, "should not accept invalid mapping in plain single line value"},


    //-------------------------------------------------------------------------
    // SECTION 3. Known, deliberate ryml limitations.
    //
    // These tests are skipped because they cover parts of YAML that
    // are deliberately not implemented by ryml.

    // container keys are not supportted
    {"4FJ6", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"6BFJ", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"6PBE", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"KK5P", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"KZN9", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"LX3P", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"M5DY", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"Q9WF", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"RZP5", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"SBG9", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"V9D5", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"X38W", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    {"XW4D", CPART_ALL, "only scalar keys allowed (keys cannot be maps or seqs)"},
    // tabs after - or : are not supported
    {"6BCT", IN______________             , "tabs after - or :"},
    {"J3BT",                  eIN_OUT_____, "tabs after - or :"},
    // anchors with : are not supported
    {"2SXE", CPART_IN_YAML|CPART_OUT_YAML, "weird characters in anchors, anchors must not end with :"},
    {"W5VH", CPART_IN_YAML, "weird characters in anchors"},
    // tags are parsed as-is; tag lookup is not supported
    {"5TYM", eIN_________, "tag lookup is not supported"},
    {"6CK3", eIN_________, "tag lookup is not supported"},
    {"6WLZ", eIN_________, "tag lookup is not supported"},
    {"9WXW", eIN_________, "tag lookup is not supported"},
    {"C4HZ", eIN_________, "tag lookup is not supported"},
    {"CC74", eIN_________, "tag lookup is not supported"},
    {"P76L", eIN_________, "tag lookup is not supported"},
    {"QLJ7", CPART_IN_YAML_ERRORS, "tag lookup is not supported"},
    {"U3C3", eIN_________, "tag lookup is not supported"},
    {"Z9M4", eIN_________, "tag lookup is not supported"},
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
    {"6FWR", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"6WPF", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"9TFX", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"B3HG", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"EX5H", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"EXG3", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"Q8AD", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"T26H", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"T4YY", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
    {"T5N4", e___OUT_____, "out-yaml test spec is missing a --- document token, which is required in the events"},
};


cspan<AllowedFailure> g_allowed_failures = allowed_failures;


} // namespace c4
} // namespace yml
