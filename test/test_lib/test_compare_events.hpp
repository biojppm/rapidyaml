#ifndef C4_YML_TEST_COMPARE_EVENTS_HPP_
#define C4_YML_TEST_COMPARE_EVENTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/error.hpp>
#endif

namespace c4 {
namespace yml {

struct OptionalScalar
{
    csubstr val = {};
    bool was_set = false;
    operator csubstr() const { return get(); }
    operator bool() const { return was_set; }
    void operator= (csubstr v) { val = v; was_set = true; } // NOLINT
    csubstr get() const { RYML_ASSERT_BASIC_(was_set); return val; }
    csubstr maybe_get() const { return was_set ? val : csubstr(""); }
};

csubstr parse_anchor_and_tag(csubstr tokens, OptionalScalar *anchor, OptionalScalar *tag);

void test_compare_events(csubstr ref_evts,
                         csubstr emt_evts,
                         bool ignore_doc_style,
                         bool ignore_container_style,
                         bool ignore_scalar_style,
                         bool ignore_tag_normalization);

} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_COMPARE_EVENTS_HPP_ */
