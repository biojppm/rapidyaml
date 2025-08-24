#ifndef _TEST_EVENTS_INTS_HPP_
#define _TEST_EVENTS_INTS_HPP_

#include <c4/yml/evt/extra/event_handler_ints.hpp>
#include <c4/bitmask.hpp>
#include <gtest/gtest.h>

namespace c4 {
using EventFlags = c4::yml::evt::extra::ievt::EventFlags;
template<>
inline c4::EnumSymbols<EventFlags> const esyms<EventFlags>()
{
    using namespace yml::evt::extra::ievt;
    static_assert(offsetof(symbol, value) == offsetof(c4::EnumSymbols<EventFlags>::Sym, value), "wrong offset");
    static_assert(offsetof(symbol, name) == offsetof(c4::EnumSymbols<EventFlags>::Sym, name), "wrong offset");
    size_t numsyms = 0;
    symbol const* syms = symbols(&numsyms);
    return c4::EnumSymbols<EventFlags>((c4::EnumSymbols<EventFlags>::Sym const*)syms, numsyms);
}
inline csubstr mkstring(c4::yml::evt::extra::ievt::DataType flags, substr buf)
{
    using namespace yml::evt::extra;
    size_t reqsize = c4::bm2str<ievt::EventFlags>((flags & ievt::MASK), buf.str, buf.len);
    C4_CHECK(reqsize > 0u);
    C4_CHECK(reqsize < buf.len);
    return buf.offs(0, 1);
}
} // namespace c4


namespace c4 {
namespace yml {
namespace evt {
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
    size_t required_size() const { return (flags & ievt::HAS_STR) ? 3u : 1u; }
};


inline C4_NO_INLINE size_t num_ints(IntEventWithScalar const *evt, size_t evt_size)
{
    size_t sz = 0;
    for(size_t i = 0; i < evt_size; ++i)
        sz += evt[i].required_size();
    return sz;
}

inline C4_NO_INLINE void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                                          ievt::DataType const* actual, size_t actual_sz,
                                          csubstr yaml,
                                          csubstr parsed_source,
                                          const char *file, int line)
{
    int status = true;
    size_t num_ints_expected = num_ints(expected, expected_sz);

    EXPECT_EQ(actual_sz, num_ints_expected);
    status = (actual_sz == num_ints_expected);

    char actualbuf[100];
    char expectedbuf[100];
    for(size_t i = 0, ie = 0; ie < expected_sz; ++ie)
    {
        EXPECT_LT(i, actual_sz);
        if (i >= actual_sz)
            break;
        csubstr actual_str = mkstring(actual[i], actualbuf);
        csubstr expected_str = mkstring(expected[ie].flags, expectedbuf);
#define _test_eq(fmt, lhs, rhs, ...)                                \
    do                                                          \
    {                                                           \
        _c4dbgpf("status={} cmp={} evt={} i={}: {}={} {}={} " fmt, status, (lhs == rhs), ie, i, #lhs, lhs, #rhs, rhs, __VA_ARGS__); \
        status &= (lhs == rhs);                                 \
        EXPECT_EQ(lhs, rhs);                                    \
    } while(0)
        _test_eq("actual={} expected={}", actual[i], expected[ie].flags, actual_str, expected_str);
        if((expected[ie].flags & ievt::HAS_STR) && (actual[i] & ievt::HAS_STR))
        {
            _test_eq("", expected[ie].str_start, actual[i + 1], 0);
            _test_eq("", expected[ie].str_len, actual[i + 2], 0);
            bool safeactual = (i + 2 < actual_sz) && (actual[i + 1] < (int)parsed_source.len && actual[i + 1] + actual[i + 2] <= (int)parsed_source.len);
            bool safeexpected = (expected[ie].str_start < (int)parsed_source.len && expected[ie].str_start + expected[ie].str_len <= (int)parsed_source.len);
            _test_eq("", safeactual, true, 0);
            _test_eq("", safeactual, safeexpected, 0);
            if(safeactual && safeexpected)
            {
                csubstr evtstr = parsed_source.sub((size_t)expected[ie].str_start, (size_t)expected[ie].str_len);
                csubstr actualstr = parsed_source.sub((size_t)actual[i + 1], (size_t)actual[i + 2]);
                _test_eq("   ref=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                         expected[ie].scalar, actualstr,
                         expected[ie].scalar.len, expected[ie].scalar,
                         actualstr.len, actualstr);
                if( ! expected[ie].needs_filter)
                {
                    _test_eq("   exp=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                             evtstr, actualstr,
                             evtstr.len, evtstr,
                             actualstr.len, actualstr);
                }
            }
        }
        i += (actual[i] & ievt::HAS_STR) ? 3u : 1u;
    }
    RYML_TRACE_FMT("defined in:\n{}:{}\n"
                   "input:[{}]~~~{}~~~\n"
                   "parsed:[{}]~~~{}~~~\n",
                   file, line,
                   yaml.len, yaml,
                   parsed_source.len, parsed_source);
    EXPECT_TRUE(status);
}


} // namespace extra
} // namespace evt
} // namespace yml
} // namespace c4

#endif /* _TEST_EVENTS_INTS_HPP_ */
