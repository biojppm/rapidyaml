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

#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif

namespace c4 {
namespace yml {

// instantiate the template
template class ParseEngine<extra::EventHandlerInts>;

namespace extra {

int32_t estimate_events_ints_size(csubstr src)
{
    int32_t count = 7; // BSTR + BDOC + =VAL + EDOC + ESTR
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
        case '?':
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
        }
    }
    return count;
}

// ensure the ievt flags work despite being signed
namespace ievt {
static_assert((MASK & BSTR) == BSTR, "overflow?");
static_assert((MASK & ESTR) == ESTR, "overflow?");
static_assert((MASK & BDOC) == BDOC, "overflow?");
static_assert((MASK & EDOC) == EDOC, "overflow?");
static_assert((MASK & BMAP) == BMAP, "overflow?");
static_assert((MASK & EMAP) == EMAP, "overflow?");
static_assert((MASK & BSEQ) == BSEQ, "overflow?");
static_assert((MASK & ESEQ) == ESEQ, "overflow?");
static_assert((MASK & ANCH) == ANCH, "overflow?");
static_assert((MASK & ALIA) == ALIA, "overflow?");
static_assert((MASK & TAG_) == TAG_, "overflow?");
static_assert((MASK & PLAI) == PLAI, "overflow?");
static_assert((MASK & SQUO) == SQUO, "overflow?");
static_assert((MASK & DQUO) == DQUO, "overflow?");
static_assert((MASK & LITL) == LITL, "overflow?");
static_assert((MASK & FOLD) == FOLD, "overflow?");
static_assert((MASK & FLOW) == FLOW, "overflow?");
static_assert((MASK & BLCK) == BLCK, "overflow?");
static_assert((MASK & KEY_) == KEY_, "overflow?");
static_assert((MASK & VAL_) == VAL_, "overflow?");
static_assert((MASK & EXPL) == EXPL, "overflow?");
static_assert((MASK & YAML) == YAML, "overflow?");
static_assert((MASK & TAGD) == TAGD, "overflow?");
static_assert((MASK & TAGV) == TAGV, "overflow?");
static_assert((MASK & AREN) == AREN, "overflow?");
static_assert((MASK & PSTR) == PSTR, "overflow?");
static_assert((MASK & UNFILT) == UNFILT, "overflow?");
static_assert((MASK & LAST) == LAST, "overflow?");
static_assert((MASK & MASK) == MASK, "overflow?");
static_assert((MASK & WSTR) == WSTR, "overflow?");
} // namespace ievt

} // namespace extra
} // namespace yml
} // namespace c4
