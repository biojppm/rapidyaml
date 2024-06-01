#include "./test_suite_parts.hpp"

namespace c4 {
namespace yml {


// g++-5 does not like creating a csubstr directly from the literal.
// so we use this macro (undefined at the end) to make the
// declarations less verbose:
#define _(testcase, reason) AllowedFailure{csubstr(testcase), csubstr(reason)}


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/src
constexpr const AllowedFailure allowed_failures[] = {

    //-------------------------------------------------------------------------
    // SECTION 1. Known issues, TODO
    //
    // These tests are temporarily skipped, and cover issues that must be fixed.

    // errors that fail to materialize
    _("3HFZ-error"              , "should not accept scalar after ..."),
    _("4EJS-error"              , "should not accept tabs as indendation in a mapping"),
    _("5TRB-error"              , "should not accept document-end marker in double quoted string"),
    _("5U3A-error"              , "should not accept opening a sequence on same line as map key"),
    _("7LBH-error"              , "should not accept multiline double quoted implicit keys"),
    _("9C9N-error"              , "should not accept non-indented flow sequence"),
    _("9MQT_01-error"           , "should not accept scalars after ..."),
    _("C2SP-error"              , "should not accept flow sequence with terminating ] on the next line"),
    _("CVW2-error"              , "should not accept flow sequence with comment after ,"),
    _("CXX2-error"              , "should not accept mapping with anchor on document start line"),
    _("D49Q-error"              , "should not accept multiline single quoted implicit keys"),
    _("DK4H-error"              , "should not accept implicit key followed by newline"),
    _("DK95_01-error"           , "should not accept leading tabs in double quoted multiline scalar"),
    _("DK95_06-error"           , "should not accept tab indentation"),
    _("G5U8-error"              , "should not accept [-, -]"),
    _("H7TQ-error"              , "should not accept extra words after directive"),
    _("JKF3-error"              , "should not accept multiline unindented double quoted scalar"),
    _("N782-error"              , "TBD"),
    _("QB6E-error"              , "should not accept indented multiline quoted scalar"),
    _("QLJ7-error"              , "tag directives should apply only to the next doc (?)"),
    _("RXY3-error"              , "should not accept document-end marker in single quoted string"),
    _("S4GJ-error"              , "should not accept text after block scalar indicator"),
    _("S98Z-error"              , "should not accept block scalar with more spaces than first content line"),
    _("SY6V-error"              , "TBD"),
    _("VJP3_00-error"           , "should not accept flow collections over many lines"),
    _("X4QW-error"              , "should not accept comment without whitespace after block scalar indicator"),
    _("Y79Y_003-error"          , "should not accept leading tabs in seq elmt"),
    _("Y79Y_004-error"          , "should not accept tab after -"),
    _("Y79Y_005-error"          , "TBD"),
    _("Y79Y_006-error"          , "should not accept tab after ?"),
    _("Y79Y_007-error"          , "tabs tokens"),
    _("Y79Y_008-error"          , "TBD"),
    _("Y79Y_009-error"          , "should not accept tab after ?"),
    _("YJV2-error"              , "should not accept [-]"),
    _("ZCZ6-error"              , "should not accept invalid mapping in plain single line value"),
    _("ZL4Z-error"              , "TBD"),
    _("ZXT5-error"              , "TBD"),


    //-------------------------------------------------------------------------
    // SECTION 2. Deliberate ryml limitations.
    //
    // These tests are skipped because they cover parts of YAML that
    // are deliberately not implemented by ryml.

    #ifndef RYML_WITH_TAB_TOKENS
    // -<tab> or :<tab> are supported only when the above macro is defined
    _("6BCT-in_yaml"            , "tabs tokens"),
    _("A2M4-in_yaml"            , "tabs tokens"),
    _("DC7X-in_yaml"            , "tabs tokens"),
    _("DK95_00-in_yaml"         , "tabs tokens"),
    _("DK95_04-in_yaml"         , "tabs tokens"),
    _("J3BT-in_yaml"            , "tabs tokens"),
    _("K54U-in_yaml"            , "tabs tokens"),
    _("Y79Y_010-in_yaml"        , "tabs tokens"),
    #endif
    // container keys are deliberately not supported by the ryml tree.
    // But they ARE supported by the parse engine. So they are dealt
    // with separately using the event parser. See below.

    //-------------------------------------------------------------------------
    // SECTION 3. Problems with the test suite spec. (or our
    // understanding of it?)

    // malformed json in the test spec
    _("35KP-in_json"            , "malformed JSON from multiple documents"),
    _("5TYM-in_json"            , "malformed JSON from multiple documents"),
    _("6XDY-in_json"            , "malformed JSON from multiple documents"),
    _("6WLZ-in_json"            , "malformed JSON from multiple documents"),
    _("6ZKB-in_json"            , "malformed JSON from multiple documents"),
    _("7Z25-in_json"            , "malformed JSON from multiple documents"),
    _("9DXL-in_json"            , "malformed JSON from multiple documents"),
    _("9KAX-in_json"            , "malformed JSON from multiple documents"),
    _("9WXW-in_json"            , "malformed JSON from multiple documents"),
    _("JHB9-in_json"            , "malformed JSON from multiple documents"),
    _("KSS4-in_json"            , "malformed JSON from multiple documents"),
    _("L383-in_json"            , "malformed JSON from multiple documents"),
    _("M7A3-in_json"            , "malformed JSON from multiple documents"),
    _("PUW8-in_json"            , "malformed JSON from multiple documents"),
    _("RZT7-in_json"            , "malformed JSON from multiple documents"),
    _("U9NS-in_json"            , "malformed JSON from multiple documents"),
    _("UT92-in_json"            , "malformed JSON from multiple documents"),
    _("W4TN-in_json"            , "malformed JSON from multiple documents"),
    // malformed test spec?
    _("4ABK-out_yaml-events"    , "out-yaml contains null, while in-yaml and events contain empty scalars"),
    _("4WA9-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("652Z-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("6CA3-emit_yaml"          , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("6FWR-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("6WPF-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("9TFX-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("B3HG-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_00-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_02-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_03-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_04-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_05-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_06-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_07-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("DK95_08-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("EX5H-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("EXG3-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("L24T_00-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("L24T_01-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("M6YH-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("Q8AD-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T26H-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T4YY-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T5N4-out_yaml-events"    , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("VJP3_01-out_yaml-events" , "out-yaml test spec is missing a --- document token, which is required in the events"),
};

constexpr const AllowedFailure container_key_cases[] = {

    // these cases have container keys, and cannot be parsed into the
    // ryml tree. However, they CAN be parsed by the ryml parse engine.
    // Therefore the tests are enabled if they only use the YAML event
    // emitter based on the parse engine.

    _("4FJ6-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("4FJ6-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("6BFJ-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("6BFJ-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("6PBE-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("6PBE-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("6PBE-emit_yaml"          , "only scalar keys allowed (keys cannot be containers)"),
    _("9MMW-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("9MMW-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("KK5P-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("KK5P-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("KZN9-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("KZN9-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("LX3P-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("LX3P-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("M2N8_00-in_yaml"         , "only scalar keys allowed (keys cannot be containers)"),
    _("M2N8_00-out_yaml"        , "only scalar keys allowed (keys cannot be containers)"),
    _("M2N8_01-in_yaml"         , "only scalar keys allowed (keys cannot be containers)"),
    _("M2N8_01-out_yaml"        , "only scalar keys allowed (keys cannot be containers)"),
    _("M5DY-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("M5DY-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("Q9WF-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("Q9WF-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("RZP5-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("RZP5-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("SBG9-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("SBG9-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("V9D5-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("V9D5-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("X38W-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("X38W-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),
    _("XW4D-in_yaml"            , "only scalar keys allowed (keys cannot be containers)"),
    _("XW4D-out_yaml"           , "only scalar keys allowed (keys cannot be containers)"),

};


cspan<AllowedFailure> g_allowed_failures = allowed_failures;
cspan<AllowedFailure> g_container_key_cases = container_key_cases;

AllowedFailure is_failure_expected(csubstr casename)
{
    RYML_CHECK(casename.not_empty());
    for(AllowedFailure const& af : g_allowed_failures)
        if(af.test_name == casename || casename.begins_with(af.test_name))
            return af;
    return {};
}

AllowedFailure case_has_container_keys(csubstr casename)
{
    RYML_CHECK(casename.not_empty());
    for(AllowedFailure const& af : g_container_key_cases)
        if(af.test_name == casename || casename.begins_with(af.test_name))
            return af;
    return {};
}


} // namespace c4
} // namespace yml
