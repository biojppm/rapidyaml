#ifndef C4_YML_TEST_SUITE_PARTS_HPP_
#define C4_YML_TEST_SUITE_PARTS_HPP_


#include <c4/yml/common.hpp>
#include <c4/span.hpp>
#include <c4/log/log.hpp>


namespace c4 {
namespace yml {

/* Each case from the test suite contains:
 *
 *  - (awkward) input yaml (in_yaml)
 *  - (somewhat standard) output equivalent (out_yaml)
 *  - (when meaningful/possible) json equivalent (in_json)
 *  - yaml parsing events (events)
 *
 * Running a test consists of parsing the contents above into a data
 * structure, and then repeatedly parsing and emitting yaml in a sort
 * of pipe. Ie, (eg for in_yaml) parse in_yaml, emit corresponding
 * yaml, then parse this emitted yaml, and so on. Each parse/emit pair
 * is named a processing level in this test. */


typedef enum {
    CPART_NONE             = 0,
    CPART_IN_YAML          = 1 << 0,
    CPART_IN_YAML_EVENTS   = 1 << 1,
    CPART_IN_YAML_ERRORS   = 1 << 2,
    CPART_OUT_YAML         = 1 << 3,
    CPART_OUT_YAML_EVENTS  = 1 << 4,
    CPART_EMIT_YAML        = 1 << 5,
    CPART_EMIT_YAML_EVENTS = 1 << 6,
    CPART_IN_JSON          = 1 << 7,
    CPART_EVENTS           = CPART_IN_YAML_EVENTS|CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML_EVENTS,
    CPART_ALL              = CPART_IN_YAML|CPART_IN_YAML_EVENTS|CPART_OUT_YAML|CPART_OUT_YAML_EVENTS|CPART_EMIT_YAML|CPART_EMIT_YAML_EVENTS|CPART_IN_JSON,
    CPART_ANY              = CPART_ALL,
} CasePart_e;
constexpr CasePart_e operator| (CasePart_e lhs, CasePart_e rhs) noexcept { return (CasePart_e)((int)lhs|(int)rhs); }

inline CasePart_e events_part(CasePart_e cp)
{
    if(cp == CPART_IN_JSON)
        return CPART_IN_JSON;
    RYML_ASSERT(cp == CPART_IN_YAML || cp == CPART_OUT_YAML || cp == CPART_EMIT_YAML);
    return (CasePart_e)((int)cp << 1);
}

inline csubstr to_csubstr(CasePart_e cp) noexcept
{
    if(cp == CPART_NONE) return {"NONE"};
    else if(cp == CPART_IN_YAML) return {"IN_YAML"};
    else if(cp == CPART_IN_YAML_EVENTS) return {"IN_YAML_EVENTS"};
    else if(cp == CPART_IN_YAML_ERRORS) return {"IN_YAML_ERRORS"};
    else if(cp == CPART_OUT_YAML) return {"OUT_YAML"};
    else if(cp == CPART_OUT_YAML_EVENTS) return {"OUT_YAML_EVENTS"};
    else if(cp == CPART_EMIT_YAML) return {"EMIT_YAML"};
    else if(cp == CPART_EMIT_YAML_EVENTS) return {"EMIT_YAML_EVENTS"};
    else if(cp == CPART_IN_JSON) return {"IN_JSON"};
    else if(cp == CPART_ALL) return {"ALL"};
    return {"<unknown>"};
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct AllowedFailure
{
    csubstr test_code;
    CasePart_e contexts;
    csubstr reason;
    constexpr AllowedFailure() : test_code(), contexts(), reason() {}
    constexpr AllowedFailure(csubstr tc, CasePart_e ctx, csubstr r) : test_code(tc), contexts(ctx), reason(r) {}
    operator bool() const { return test_code.not_empty(); }
    bool skip() const { return skip(CPART_ALL); }
    bool skip(CasePart_e case_part) const
    {
        if((case_part != CPART_ALL && (contexts & case_part))
           ||
           (case_part == CPART_ALL && (contexts == CPART_ALL)))
        {
            c4::log("skipping test {} in {}: matches {}. Reason: {}", test_code, to_csubstr(case_part), to_csubstr(contexts), reason);
            return true;
        }
        return false;
    }
};


extern c4::cspan<AllowedFailure> g_allowed_failures;

inline AllowedFailure is_failure_expected(csubstr filename, CasePart_e parts)
{
    csubstr test_code = filename.basename().name_wo_extshort();
    RYML_ASSERT(test_code.not_empty());
    for(AllowedFailure const& af : g_allowed_failures)
        if(af.test_code == test_code)
            if(af.skip(parts))
                return af;
    return {};
}

} // namespace c4
} // namespace yml


#endif /* C4_YML_TEST_SUITE_PARTS_HPP_ */
