#include "./test_suite_parts.hpp"

namespace c4 {
namespace yml {


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/src
constexpr const AllowedFailure allowed_failures[] = {

    // g++-5 does not like creating a csubstr directly from the literal.
    // so we use this macro to remove cruft from the code:
    #define _(testcase, reason) AllowedFailure{csubstr(testcase), csubstr(reason)}

    //-------------------------------------------------------------------------
    // SECTION 1. Known issues, TODO
    //
    // These tests are temporarily skipped, and cover issues that must be fixed.

    // double quoted scalars
    _("G4RS-in_json"            , "special characters must be emitted in double quoted style"),
    _("G4RS-in_yaml"            , "special characters must be emitted in double quoted style"),
    _("G4RS-out_yaml"           , "special characters must be emitted in double quoted style"),
    // other
    _("UKK6_01-in_yaml"         , "fails to parse double :: in UNK state"),
    _("UKK6_02-in_yaml-events"  , "fails to parse double :: in UNK state"),
    // tabs
    _("6CA3-in_yaml"            , "indentation with tabs causes problems"),
    _("6CA3-emit_yaml"          , "indentation with tabs causes problems"),
    _("A2M4-in_yaml-events"     , "indentation with tabs causes problems"),
    _("DK95_00-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_01-error"           , "indentation with tabs causes problems"),
    _("DK95_02-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_03-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_04-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_05-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_06-error"           , "indentation with tabs causes problems"),
    _("DK95_06-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_07-emit_yaml-events", "indentation with tabs causes problems"),
    _("DK95_08-emit_yaml-events", "indentation with tabs causes problems"),
    _("Y79Y"                    , "tab problems"),


    //-------------------------------------------------------------------------
    // SECTION 2. Expected errors that fail to materialize.

    _("236B-error"   , "should not accept final scalar in a map"),
    _("2G84_00-error", "should not accept the block literal spec"),
    _("2G84_01-error", "should not accept the block literal spec"),
    _("3HFZ-error"   , "should not accept scalar after ..."),
    _("4EJS-error"   , "should not accept double anchor for scalar"),
    _("4JVG-error"   , "should not accept double anchor for scalar"),
    _("55WF-error"   , "should not accept invalid escape in double quoted string"),
    _("5LLU-error"   , "should not accept folded scalar with wrong indented line after spaces only"),
    _("5TRB-error"   , "should not accept document-end marker in double quoted string"),
    _("5U3A-error"   , "should not accept opening a sequence on same line as map key"),
    _("62EZ-error"   , "should not accept invalid block mapping key on same line as previous key"),
    _("6JTT-error"   , "should not accept flow sequence without terminating ]"),
    _("7LBH-error"   , "should not accept multiline double quoted implicit keys"),
    _("7MNF-error"   , "should not accept final scalar in a map"),
    _("8XDJ-error"   , "should not accept comment in multiline scalar"),
    _("9C9N-error"   , "should not accept non-indented flow sequence"),
    _("9CWY-error"   , "should not accept final scalar in a map"),
    _("9HCY-error"   , "should not accept tag directive in non-doc scope"),
    _("9JBA-error"   , "should not accept comment after flow seq terminating ]"),
    _("9MAG-error"   , "should not accept flow sequence with invalid comma at the beginning"),
    _("9MMA-error"   , "should not accept empty doc after %YAML directive"),
    _("9MQT_01-error", "should not accept scalars after ..."),
    _("B63P-error"   , "should not accept directive without doc"),
    _("BU8L-error"   , "should not accept node properties spread over multiple lines"),
    _("CML9-error"   , "should not accept comment inside flow scalar"),
    _("CTN5-error"   , "should not accept flow sequence with missing elements"),
    _("CVW2-error"   , "should not accept flow sequence with comment after ,"),
    _("CXX2-error"   , "should not accept mapping with anchor on document start line"),
    _("D49Q-error"   , "should not accept multiline single quoted implicit keys"),
    _("DK4H-error"   , "should not accept implicit key followed by newline"),
    _("EB22-error"   , "should not accept missing document-end marker before directive"),
    _("G5U8-error"   , "should not accept [-, -]"),
    _("GDY7-error"   , "should not accept comment that looks like a mapping key"),
    _("HRE5-error"   , "should not accept double quoted scalar with escaped single quote"),
    _("H7TQ-error"   , "should not accept extra words after directive"),
    _("JKF3-error"   , "should not accept multiline unindented double quoted scalar"),
    _("JY7Z-error"   , "should not accept trailing content that looks like a mapping"),
    _("KS4U-error"   , "should not accept item after end of flow sequence"),
    _("LHL4-error"   , "should not accept tag"),
    _("MUS6_00-error", "should not accept #... at the end of %YAML directive"),
    _("MUS6_01-error", "should not accept #... at the end of %YAML directive"),
    _("N782-error"   , "should not accept document markers in flow style"),
    _("P2EQ-error"   , "should not accept sequence item on same line as previous item"),
    _("QB6E-error"   , "should not accept indented multiline quoted scalar"),
    _("RHX7-error"   , "should not accept directive without document end marker"),
    _("RXY3-error"   , "should not accept document-end marker in single quoted string"),
    _("S4GJ-error"   , "should not accept text after block scalar indicator"),
    _("S98Z-error"   , "should not accept block scalar with more spaces than first content line"),
    _("SF5V-error"   , "should not accept duplicate YAML directive"),
    _("SU5Z-error"   , "should not accept comment without whitespace after double quoted scalar"),
    _("SU74-error"   , "should not accept anchor and alias as mapping key"),
    _("SY6V-error"   , "should not accept anchor before sequence entry on same line"),
    _("T833-error"   , "should not accept flow mapping missing a separating comma"),
    _("U99R-error"   , "should not accept comma in a tag"),
    _("VJP3_00-error", "should not accept flow collections over many lines"),
    _("X4QW-error"   , "should not accept comment without whitespace after block scalar indicator"),
    _("YJV2-error"   , "should not accept [-]"),
    _("ZCZ6-error"   , "should not accept invalid mapping in plain single line value"),


    //-------------------------------------------------------------------------
    // SECTION 3. Deliberate ryml limitations.
    //
    // These tests are skipped because they cover parts of YAML that
    // are deliberately not implemented by ryml.

    // container keys are not supported
    _("4FJ6-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("4FJ6-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("6BFJ-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("6BFJ-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("6PBE-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("6PBE-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("6PBE-emit_yaml"         , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("9MMW-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("9MMW-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("KK5P-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("KK5P-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("KZN9-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("KZN9-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("LX3P-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("LX3P-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("M2N8_00-in_yaml"        , "only scalar keys allowed (keys cannot be maps or seqs)"),// FIXME but only case 1
    _("M2N8_00-out_yaml"       , "only scalar keys allowed (keys cannot be maps or seqs)"),// FIXME but only case 1
    _("M2N8_01-in_yaml-events" , "only scalar keys allowed (keys cannot be maps or seqs)"),// FIXME but only case 1
    _("M2N8_01-out_yaml-events", "only scalar keys allowed (keys cannot be maps or seqs)"),// FIXME but only case 1
    _("M5DY-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("M5DY-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("Q9WF-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("Q9WF-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("RZP5-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("RZP5-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("SBG9-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("SBG9-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("V9D5-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("V9D5-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("X38W-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("X38W-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("XW4D-in_yaml"           , "only scalar keys allowed (keys cannot be maps or seqs)"),
    _("XW4D-out_yaml"          , "only scalar keys allowed (keys cannot be maps or seqs)"),
    // tabs after - or : are not supported
    _("6BCT-in_yaml"           , "tabs after - or :"),
    _("J3BT-in_yaml-events"    , "tabs after - or :"),
    // anchors with : are not supported
    _("2SXE-in_yaml-events"    , "weird characters in anchors, anchors must not end with :"),
    // tags are parsed as-is; tag lookup is not supported
    _("5TYM-in_yaml-events"    , "tag lookup is not supported"),
    _("6CK3-in_yaml-events"    , "tag lookup is not supported"),
    _("6WLZ-in_yaml-events"    , "tag lookup is not supported"),
    _("9WXW-in_yaml-events"    , "tag lookup is not supported"),
    _("C4HZ-in_yaml-events"    , "tag lookup is not supported"),
    _("CC74-in_yaml-events"    , "tag lookup is not supported"),
    _("P76L-in_yaml-events"    , "tag lookup is not supported"),
    _("QLJ7-error"             , "tag lookup is not supported"),
    _("U3C3-in_yaml-events"    , "tag lookup is not supported"),
    _("Z9M4-in_yaml-events"    , "tag lookup is not supported"),
    // malformed json in the test spec
    _("35KP-in_json"           , "malformed JSON from multiple documents"),
    _("5TYM-in_json"           , "malformed JSON from multiple documents"),
    _("6XDY-in_json"           , "malformed JSON from multiple documents"),
    _("6WLZ-in_json"           , "malformed JSON from multiple documents"),
    _("6ZKB-in_json"           , "malformed JSON from multiple documents"),
    _("7Z25-in_json"           , "malformed JSON from multiple documents"),
    _("9DXL-in_json"           , "malformed JSON from multiple documents"),
    _("9KAX-in_json"           , "malformed JSON from multiple documents"),
    _("9WXW-in_json"           , "malformed JSON from multiple documents"),
    _("JHB9-in_json"           , "malformed JSON from multiple documents"),
    _("KSS4-in_json"           , "malformed JSON from multiple documents"),
    _("L383-in_json"           , "malformed JSON from multiple documents"),
    _("M7A3-in_json"           , "malformed JSON from multiple documents"),
    _("RZT7-in_json"           , "malformed JSON from multiple documents"),
    _("U9NS-in_json"           , "malformed JSON from multiple documents"),
    _("W4TN-in_json"           , "malformed JSON from multiple documents"),
    // malformed test spec?
    _("4ABK-out_yaml-events"   , "out-yaml contains null, while in-yaml and events contain empty scalars"),
    _("4WA9-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("652Z-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("6FWR-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("6WPF-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("9TFX-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("B3HG-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("EX5H-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("EXG3-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("L24T_00-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("L24T_01-emit_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("M6YH-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("Q8AD-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T26H-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T4YY-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("T5N4-out_yaml-events"   , "out-yaml test spec is missing a --- document token, which is required in the events"),
    _("VJP3_01-out_yaml-events", "out-yaml test spec is missing a --- document token, which is required in the events"),

    #undef _
};


cspan<AllowedFailure> g_allowed_failures = allowed_failures;

AllowedFailure is_failure_expected(csubstr casename)
{
    RYML_CHECK(casename.not_empty());
    for(AllowedFailure const& af : g_allowed_failures)
        if(af.test_name == casename || casename.begins_with(af.test_name))
            return af;
    return {};
}


} // namespace c4
} // namespace yml
