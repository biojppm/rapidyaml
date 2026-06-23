#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#endif

#ifndef _C4_YML_EXTRA_INTS_UTILS_HPP_
#include "c4/yml/extra/ints_utils.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wold-style-cast")
// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

namespace {
struct FlagSym { const char *str; EventFlags flags; };
const FlagSym flag_syms_[] = {
    {"KEY_", KEY_},
    {"VAL_", VAL_},
    {"SCLR", SCLR},
    {"BSEQ", BSEQ},
    {"ESEQ", ESEQ},
    {"BMAP", BMAP},
    {"EMAP", EMAP},
    {"ALIA", ALIA},
    {"ANCH", ANCH},
    {"TAG_", TAG_},
    {"PLAI", PLAI},
    {"SQUO", SQUO},
    {"DQUO", DQUO},
    {"LITL", LITL},
    {"FOLD", FOLD},
    {"FLOW", FLOW},
    {"BLCK", BLCK},
    {"BDOC", BDOC},
    {"EDOC", EDOC},
    {"BSTR", BSTR},
    {"ESTR", ESTR},
    {"EXPL", EXPL},
    {"AREN", AREN},
    {"PSTR", PSTR},
    {"UNFILT", UNFILT},
    {"YAML", YAML},
    {"TAGH", TAGH},
    {"TAGP", TAGP},
};
} // namespace

size_t to_str(substr buf, ievt::evt_bits flags) noexcept
{
    detail::_SubstrWriter writer(buf);
    for(const FlagSym sym : flag_syms_)
    {
        if(flags & sym.flags)
        {
            if(writer.pos)
                writer.append('|');
            writer.append(sym.str);
            flags &= ~sym.flags;
        }
    }
    if(!writer.pos)
        writer.append("NONE");
    if(buf.len > writer.pos)
        buf[writer.pos] = '\0';
    return writer.pos;
}

csubstr to_str_sub(substr buf, ievt::evt_bits flags)
{
    size_t reqsize = ievt::to_str(buf, flags);
    _RYML_CHECK_BASIC(reqsize > 0u);
    _RYML_CHECK_BASIC(reqsize < buf.len);
    return buf.first(reqsize);
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

void events_ints_print(csubstr parsed_yaml, csubstr arena, ievt::evt_bits const* evts, ievt::evt_bits evts_sz)
{
    char buf[200];
    for(ievt::evt_bits evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos += ((evts[evtpos] & ievt::WSTR) ? 3 : 1))
    {
        ievt::evt_bits evt = evts[evtpos];
        csubstr flags = ievt::to_str_sub(buf, evt);
        printf("[%d][%d] %.*s(0x%x)", evtnumber, evtpos, (int)flags.len, flags.str, evt);
        if (evt & ievt::WSTR)
        {
            bool in_arena = evt & ievt::AREN;
            csubstr region = !in_arena ? parsed_yaml : arena;
            bool safe = (evts[evtpos + 1] >= 0)
                && (evts[evtpos + 2] >= 0)
                && (evts[evtpos + 1] <= (ievt::evt_bits)region.len) // NOLINT
                && (evts[evtpos + 2] <= ((ievt::evt_bits)region.len - evts[evtpos + 1]));
            const char *str = safe ? (region.str + evts[evtpos + 1]) : "ERR!!!";
            ievt::evt_bits len = safe ? evts[evtpos + 2] : 6;
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

// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_CLANG_POP
C4_SUPPRESS_WARNING_GCC_POP
