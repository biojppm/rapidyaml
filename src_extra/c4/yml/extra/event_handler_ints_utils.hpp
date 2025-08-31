#pragma once
#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_UTILS_HPP_
#define _C4_YML_EXTRA_EVENT_HANDLER_INTS_UTILS_HPP_

#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif

#ifndef _C4_BITMASK_HPP_
#include "c4/bitmask.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(hicpp-signed-bitwise)


namespace c4 {
template<>
inline c4::EnumSymbols<yml::extra::ievt::EventFlags> const esyms<yml::extra::ievt::EventFlags>()
{
    static constexpr const EnumSymbols<yml::extra::ievt::EventFlags>::Sym syms[] = {
        {yml::extra::ievt::KEY_, "KEY_"},
        {yml::extra::ievt::VAL_, "VAL_"},
        {yml::extra::ievt::SCLR, "SCLR"},
        {yml::extra::ievt::BSEQ, "BSEQ"},
        {yml::extra::ievt::ESEQ, "ESEQ"},
        {yml::extra::ievt::BMAP, "BMAP"},
        {yml::extra::ievt::EMAP, "EMAP"},
        {yml::extra::ievt::ALIA, "ALIA"},
        {yml::extra::ievt::ANCH, "ANCH"},
        {yml::extra::ievt::TAG_, "TAG_"},
        {yml::extra::ievt::PLAI, "PLAI"},
        {yml::extra::ievt::SQUO, "SQUO"},
        {yml::extra::ievt::DQUO, "DQUO"},
        {yml::extra::ievt::LITL, "LITL"},
        {yml::extra::ievt::FOLD, "FOLD"},
        {yml::extra::ievt::FLOW, "FLOW"},
        {yml::extra::ievt::BLCK, "BLCK"},
        {yml::extra::ievt::BDOC, "BDOC"},
        {yml::extra::ievt::EDOC, "EDOC"},
        {yml::extra::ievt::BSTR, "BSTR"},
        {yml::extra::ievt::ESTR, "ESTR"},
        {yml::extra::ievt::EXPL, "EXPL"},
        {yml::extra::ievt::PSTR, "PSTR"},
    };
    return EnumSymbols<yml::extra::ievt::EventFlags>(syms);
}
} // namespace c4


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {
inline csubstr mkstring(yml::extra::ievt::DataType flags, substr buf)
{
    size_t reqsize = c4::bm2str<yml::extra::ievt::EventFlags>((flags & yml::extra::ievt::MASK), buf.str, buf.len);
    RYML_CHECK(reqsize > 0u);
    RYML_CHECK(reqsize < buf.len);
    return buf.first(reqsize - 1u);
}
} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

RYML_EXPORT void print_events_ints(csubstr parsed_yaml, ievt::DataType const* evts_ints, ievt::DataType evts_ints_sz);

RYML_EXPORT size_t emit_events_test_suite_from_ints(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz,
    substr evts_test_suite);

template<class Container>
void emit_events_test_suite_from_ints(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz,
    Container *evts_test_suite)
{
    substr s = to_substr(*evts_test_suite);
    size_t len = emit_events_test_suite_from_ints(parsed_yaml, evts_ints, evts_ints_sz, s);
    if(len > evts_test_suite->size())
    {
        evts_test_suite->resize(len);
        s = to_substr(*evts_test_suite);
        len = emit_events_test_suite_from_ints(parsed_yaml, evts_ints, evts_ints_sz, s);
    }
    evts_test_suite->resize(len);
}

template<class Container>
Container emit_events_test_suite_from_ints(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz)
{
    Container ret;
    emit_events_test_suite_from_ints(parsed_yaml, evts_ints, evts_ints_sz, &ret);
    return ret;
}

} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* _C4_YML_EXTRA_EVENT_HANDLER_INTS_UTILS_HPP_ */
