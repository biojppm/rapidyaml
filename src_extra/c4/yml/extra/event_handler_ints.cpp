#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#else
#include <c4/yml/parse_engine.def.hpp>
#endif

#ifndef C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif

// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {

// instantiate the template
template class RYML_EXPORT ParseEngine<extra::EventHandlerInts>;

namespace extra {

RYML_EXPORT evt_size estimate_events_ints_size(csubstr src)
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
static_assert((ievt::MASK & ievt::UNFILT) == ievt::UNFILT, "overflow?");


namespace ievt {
namespace detail {


evt_size find_matching_close_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size pos)
{
    evt_bits evt = evts[pos];
    RYML_ASSERT_BASIC_((evt & ievt::BEG_) && (evt & (mask_open_close & ~ievt::BEG_)));
    RYML_ASSERT_BASIC_((evt & mask_begend) != mask_begend);
    const evt_bits open = evt & mask_open_close;
    const evt_bits close = (open & ~ievt::BEG_) | ievt::END_;
    pos += ievt::nextstep(evt); // don't count the starting close token
    uint32_t count = 0;
    while(pos < sz)
    {
        evt = evts[pos];
        if((evt & open) == open)
        {
            ++count;
        }
        else if((evt & close) == close)
        {
            if(!(count--))
                return pos;
        }
        pos += ievt::nextstep(evt);
    }
    RYML_ERR_BASIC_("evt error"); // LCOV_EXCL_LINE
}

} // namespace detail
} // namespace ievt

} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(*-signed-bitwise)
