#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#endif

#ifndef _C4_YML_EXTRA_SCALAR_HPP_
#include "c4/yml/extra/scalar.hpp"
#endif

#ifndef _C4_YML_EXTRA_INTS_UTILS_HPP_
#include "c4/yml/extra/ints_utils.hpp"
#endif

#ifndef _C4_BITMASK_HPP_
#include "c4/bitmask.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wold-style-cast")
// NOLINTBEGIN(hicpp-signed-bitwise)


namespace c4 {
template<>
c4::EnumSymbols<yml::extra::ievt::EventFlags> esyms<yml::extra::ievt::EventFlags>()
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
        {yml::extra::ievt::AREN, "AREN"},
        {yml::extra::ievt::PSTR, "PSTR"},
        {yml::extra::ievt::UNFILT, "UNFILT"},
        {yml::extra::ievt::YAML, "YAML"},
        {yml::extra::ievt::TAGD, "TAGD"},
        {yml::extra::ievt::TAGV, "TAGV"},
    };
    return EnumSymbols<yml::extra::ievt::EventFlags>(syms);
}
namespace yml {
namespace extra {
namespace ievt {
size_t to_chars(substr buf, ievt::DataType flags)
{
    return c4::bm2str<ievt::EventFlags>((flags & ievt::MASK), buf.str, buf.len);
}
csubstr to_chars_sub(substr buf, ievt::DataType flags)
{
    size_t reqsize = ievt::to_chars(buf, flags);
    RYML_CHECK(reqsize > 0u);
    RYML_CHECK(reqsize < buf.len);
    return buf.first(reqsize - 1u);
}
} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

void events_ints_print(csubstr parsed_yaml, csubstr arena, ievt::DataType const* evts, ievt::DataType evts_sz)
{
    char buf[200];
    for(ievt::DataType evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos += ((evts[evtpos] & ievt::WSTR) ? 3 : 1))
    {
        ievt::DataType evt = evts[evtpos];
        {
            csubstr str = ievt::to_chars_sub(buf, evt);
            printf("[%d][%d] %.*s(0x%x)", evtnumber, evtpos, (int)str.len, str.str, evt);
        }
        if (evt & ievt::WSTR)
        {
            bool in_arena = evt & ievt::AREN;
            csubstr region = !in_arena ? parsed_yaml : arena;
            bool safe = (evts[evtpos + 1] >= 0)
                && (evts[evtpos + 2] >= 0)
                && (evts[evtpos + 1] <= (int)region.len)
                && ((evts[evtpos + 1] + evts[evtpos + 2]) <= (int)region.len);
            const char *str = safe ? (region.str + evts[evtpos + 1]) : "ERR!!!";
            int len = safe ? evts[evtpos + 2] : 6;
            printf(": %d [%d]~~~%.*s~~~", evts[evtpos+1], evts[evtpos+2], len, str);
            if(in_arena)
                printf(" (arenasz=%zu)", arena.len);
            else
                printf(" (srcsz=%zu)", parsed_yaml.len);
        }
        printf("\n");
    }
}

} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
C4_SUPPRESS_WARNING_CLANG_POP
C4_SUPPRESS_WARNING_GCC_POP
