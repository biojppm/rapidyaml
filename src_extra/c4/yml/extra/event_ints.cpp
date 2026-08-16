#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

RYML_EXPORT evt_size estimate_events_size(csubstr src)
{
    evt_size count = 7; // BSTR + BDOC + =VAL + EDOC + ESTR
    for(size_t i = 0; i < src.len; ++i)
    {
        switch(src.str[i])
        {
        case ':': // this has strings preceding/following it
        case ',': // overestimate, assume map
        case '%': // assume TAGD->string + TAGV->string
            count += 6;
            break;
        // these have (or are likely to have) a string following it
        case '-':
        case '&':
        case '*':
        case '<':
        case '!':
        case '\'':
        case '"':
        case '|':
        case '>':
        case '\n':
            count += 3;
            break;
        case '[':
        case ']':
            count += 4;
            break;
        case '{':
        case '}':
            count += 7;
            break;
        case '?':
            count += 5;
            break;
        }
    }
    return count;
}

// NOLINTBEGIN(hicpp-signed-bitwise)
// ensure the ievt flags work despite being signed
static_assert((ievt::MASK & ievt::BSTR) == ievt::BSTR, "overflow?");
static_assert((ievt::MASK & ievt::ESTR) == ievt::ESTR, "overflow?");
static_assert((ievt::MASK & ievt::BDOC) == ievt::BDOC, "overflow?");
static_assert((ievt::MASK & ievt::EDOC) == ievt::EDOC, "overflow?");
static_assert((ievt::MASK & ievt::BMAP) == ievt::BMAP, "overflow?");
static_assert((ievt::MASK & ievt::EMAP) == ievt::EMAP, "overflow?");
static_assert((ievt::MASK & ievt::BSEQ) == ievt::BSEQ, "overflow?");
static_assert((ievt::MASK & ievt::ESEQ) == ievt::ESEQ, "overflow?");
static_assert((ievt::MASK & ievt::ANCH) == ievt::ANCH, "overflow?");
static_assert((ievt::MASK & ievt::ALIA) == ievt::ALIA, "overflow?");
static_assert((ievt::MASK & ievt::TAG_) == ievt::TAG_, "overflow?");
static_assert((ievt::MASK & ievt::PLAI) == ievt::PLAI, "overflow?");
static_assert((ievt::MASK & ievt::SQUO) == ievt::SQUO, "overflow?");
static_assert((ievt::MASK & ievt::DQUO) == ievt::DQUO, "overflow?");
static_assert((ievt::MASK & ievt::LITL) == ievt::LITL, "overflow?");
static_assert((ievt::MASK & ievt::FOLD) == ievt::FOLD, "overflow?");
static_assert((ievt::MASK & ievt::FLOW) == ievt::FLOW, "overflow?");
static_assert((ievt::MASK & ievt::BLCK) == ievt::BLCK, "overflow?");
static_assert((ievt::MASK & ievt::KEY_) == ievt::KEY_, "overflow?");
static_assert((ievt::MASK & ievt::VAL_) == ievt::VAL_, "overflow?");
static_assert((ievt::MASK & ievt::EXPL) == ievt::EXPL, "overflow?");
static_assert((ievt::MASK & ievt::YAML) == ievt::YAML, "overflow?");
static_assert((ievt::MASK & ievt::TAGH) == ievt::TAGH, "overflow?");
static_assert((ievt::MASK & ievt::TAGP) == ievt::TAGP, "overflow?");
static_assert((ievt::MASK & ievt::AREN) == ievt::AREN, "overflow?");
static_assert((ievt::MASK & ievt::PSTR) == ievt::PSTR, "overflow?");
static_assert((ievt::MASK & ievt::WSTR) == ievt::WSTR, "overflow?");
static_assert((ievt::MASK & ievt::LAST) == ievt::LAST, "overflow?");
static_assert((ievt::MASK & ievt::JUMP) == ievt::JUMP, "overflow?");
static_assert((ievt::MASK & ievt::PJUMP) == ievt::PJUMP, "overflow?");
static_assert((ievt::MASK & ievt::FSL_) == ievt::FSL_, "overflow?");
static_assert((ievt::MASK & ievt::FML1) == ievt::FML1, "overflow?");
static_assert((ievt::MASK & ievt::FMLN) == ievt::FMLN, "overflow?");
static_assert((ievt::MASK & ievt::FMLX) == ievt::FMLX, "overflow?");
static_assert((ievt::MASK & ievt::LAST) == ievt::LAST, "overflow?");
static_assert((ievt::MASK & ievt::UNFILT) == ievt::UNFILT, "overflow?");
// NOLINTEND(hicpp-signed-bitwise)

} // namespace ievt
} // namespace extra


namespace detail {
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wcast-qual")
extra::ievt::evtbuf resize(extra::ievt::evtbuf buf, extra::ievt::evt_size cap, Callbacks const& cb)
{
    if(cap > buf.cap)
    {
        buf.ptr = yml::detail::grow_buf(buf.ptr, (size_t)buf.len, (size_t)buf.cap, (size_t)cap, cb);
        buf.cap = cap;
    }
    return buf;
}
substr resize(substr buf, size_t sz, Callbacks const& cb)
{
    if(sz > buf.len)
    {
        buf.str = yml::detail::grow_buf(buf.str, buf.len, sz, cb);
        buf.len = sz;
    }
    return buf;
}
C4_SUPPRESS_WARNING_GCC_CLANG_POP
} // namespace detail

} // namespace yml
} // namespace c4
