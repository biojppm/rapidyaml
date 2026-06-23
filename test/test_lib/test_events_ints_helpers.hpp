#ifndef _TEST_EVENTS_INTS_HPP_
#define _TEST_EVENTS_INTS_HPP_

#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <gtest/gtest.h>



namespace c4 {
namespace yml {
namespace extra {

// provide a structured input for the event integers, grouping the
// relevant data for the event in a single structure to simplify
// specifying events in tests
struct IntEventWithScalar
{
    ievt::evt_bits flags, str_start, str_len;
    csubstr scalar;
    bool needs_filter;
    IntEventWithScalar(ievt::evt_bits t, ievt::evt_bits start=0, ievt::evt_bits len=0, csubstr sclr={}, bool needs_filter_=false)
        : flags(t)
        , str_start(start)
        , str_len(len)
        , scalar(sclr)
        , needs_filter(needs_filter_)
    {
    }
    size_t required_size() const { return (flags & ievt::WSTR) ? 3u : 1u; }
};


size_t num_ints(IntEventWithScalar const *evt, size_t evt_size);


void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      ievt::evt_bits const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      csubstr arena);

void test_events_ints_invariants(
    csubstr parsed_yaml,
    csubstr arena,
    ievt::evt_bits const* evts_ints,
    ievt::evt_bits evts_ints_sz);

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _TEST_EVENTS_INTS_HPP_ */
