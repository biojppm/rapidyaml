#ifndef _TEST_EVENTS_INTS_HPP_
#define _TEST_EVENTS_INTS_HPP_

#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/event_handler_test_suite.hpp>
#include <c4/bitmask.hpp>
#include <gtest/gtest.h>


namespace c4 {
using EventFlags = c4::yml::extra::ievt::EventFlags;
template<>
inline c4::EnumSymbols<EventFlags> const esyms<EventFlags>()
{
    using namespace yml::extra::ievt;
    static_assert(offsetof(symbol, value) == offsetof(c4::EnumSymbols<EventFlags>::Sym, value), "wrong offset");
    static_assert(offsetof(symbol, name) == offsetof(c4::EnumSymbols<EventFlags>::Sym, name), "wrong offset");
    size_t numsyms = 0;
    symbol const* syms = symbols(&numsyms);
    return c4::EnumSymbols<EventFlags>((c4::EnumSymbols<EventFlags>::Sym const*)syms, numsyms);
}
inline csubstr mkstring(c4::yml::extra::ievt::DataType flags, substr buf)
{
    using namespace yml::extra;
    size_t reqsize = c4::bm2str<ievt::EventFlags>((flags & ievt::MASK), buf.str, buf.len);
    C4_CHECK(reqsize > 0u);
    C4_CHECK(reqsize < buf.len);
    return buf.first(reqsize -1u);
}
} // namespace c4


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


inline C4_NO_INLINE size_t num_ints(IntEventWithScalar const *evt, size_t evt_size)
{
    size_t sz = 0;
    for(size_t i = 0; i < evt_size; ++i)
        sz += evt[i].required_size();
    return sz;
}

void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      ievt::DataType const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      const char *file, int line);

void test_events_ints_invariants(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz);

size_t emit_events_test_suite_from_ints(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz,
    substr evts_test_suite);

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _TEST_EVENTS_INTS_HPP_ */
