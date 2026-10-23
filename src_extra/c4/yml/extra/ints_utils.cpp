#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#endif

#ifndef C4_YML_EXTRA_INTS_UTILS_HPP_
#include "c4/yml/extra/ints_utils.hpp"
#endif
#ifndef C4_YML_EXTRA_INTS_FORMAT_HPP_
#include "c4/yml/extra/ints_format.hpp"
#endif
#ifndef C4_YML_WRITER_FILE_HPP_
#include "c4/yml/writer_file.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wold-style-cast")
// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

namespace {
struct FlagSym { const char *str; evt_bits flags; };
const FlagSym flag_syms_[] = {
    {"KEY_", KEY_},
    {"VAL_", VAL_},
    {"BDOC|EXPL", BDOC|EXPL},
    {"EDOC|EXPL", EDOC|EXPL},
    {"BSEQ|FLOW", BSEQ|FLOW},
    {"BMAP|FLOW", BMAP|FLOW},
    {"BSEQ|BLCK", BSEQ},
    {"BMAP|BLCK", BMAP},
    {"BDOC", BDOC},
    {"EDOC", EDOC},
    {"SCLR", SCLR},
    {"ESEQ", ESEQ},
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
    {"BSTR", BSTR},
    {"ESTR", ESTR},
    {"AREN", AREN},
    {"YAML", YAML},
    {"TAGH", TAGH},
    {"TAGP", TAGP},
    {"FSL_", FSL_},
    {"FML1", FML1},
    {"FMLN", FMLN},
    {"FSPC", FSPC},
    {"RREF", RREF},
    {"RREF", RREF},
    {"PEX2", PEX2},
    {"JUMP", JUMP},
    {"QWRD", QWRD},
    {"UNFL", UNFL},
};
} // namespace

size_t to_str(substr buf, evt_bits flags) noexcept
{
    yml::detail::SubstrWriter_ writer(buf);
    for(const FlagSym sym : flag_syms_)
    {
        if((flags & sym.flags) == sym.flags)
        {
            if(writer.pos)
                writer.append('|');
            writer.append(sym.str);
            flags &= ~sym.flags;
        }
    }
    if(!writer.pos)
        writer.append("NONE");
    if(writer.pos < buf.len)
        buf[writer.pos] = '\0';
    return writer.pos;
}

csubstr to_str_sub(substr buf, evt_bits flags)
{
    size_t reqsize = ievt::to_str(buf, flags);
    RYML_CHECK_BASIC_(reqsize > 0u);
    RYML_CHECK_BASIC_(reqsize < buf.len);
    return buf.first(reqsize);
}

void events_ints_print(csubstr parsed_yaml, csubstr arena, evt_bits const* evts, evt_size evts_sz, bool indent, FILE *file)
{
    WriterFile writer(file);
    format_ints(writer, parsed_yaml, arena, evts, evts_sz, indent);
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_CLANG_POP
C4_SUPPRESS_WARNING_GCC_POP
