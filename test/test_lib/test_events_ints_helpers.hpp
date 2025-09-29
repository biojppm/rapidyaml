#ifndef _TEST_EVENTS_INTS_HPP_
#define _TEST_EVENTS_INTS_HPP_

#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/event_handler_testsuite.hpp>
#include <gtest/gtest.h>



namespace c4 {
namespace yml {
namespace extra {

// provide a structured input for the event integers, grouping the
// relevant data for the event in a single structure to simplify
// specifying events in tests
struct IntEventWithScalar
{
    ievt::DataType flags, str_start, str_len;
    csubstr scalar;
    bool needs_filter;
    IntEventWithScalar(ievt::DataType t, ievt::DataType start=0, ievt::DataType len=0, csubstr sclr={}, bool needs_filter_=false)
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
                      ievt::DataType const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      csubstr arena);

void test_events_ints_invariants(
    csubstr parsed_yaml,
    csubstr arena,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz);

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _TEST_EVENTS_INTS_HPP_ */
