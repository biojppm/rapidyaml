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
        // this has strings preceding/following it
        case ':':
        case ',': // overestimate, assume map
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

} // namespace extra
} // namespace yml
} // namespace c4
