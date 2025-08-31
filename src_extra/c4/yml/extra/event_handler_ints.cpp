#include "c4/yml/extra/event_handler_ints.hpp"
#include <c4/yml/parse_engine.def.hpp>

namespace c4 {
namespace yml {

// instantiate the template
template class ParseEngine<extra::EventHandlerInts>;

namespace extra {
void print_events_ints(csubstr parsed_yaml, ievt::DataType const* evts, ievt::DataType evts_sz)
{
    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wold-style-cast")
    char buf[100];
    for(ievt::DataType evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos += ((evts[evtpos] & ievt::WSTR) ? 3 : 1))
    {
        ievt::DataType evt = evts[evtpos];
        {
            csubstr str = ievt::mkstring(evt, buf);
            printf("[%d][%d] %.*s(0x%x)", evtnumber, evtpos, (int)str.len, str.str, evt);
        }
        if (evt & ievt::WSTR)
        {
            bool safe = (evts[evtpos + 1] >= 0)
                && (evts[evtpos + 2] >= 0)
                && (evts[evtpos + 1] < (int)parsed_yaml.len)
                && ((evts[evtpos + 1] + evts[evtpos + 2]) <= (int)parsed_yaml.len);
            const char *str = safe ? (parsed_yaml.str + evts[evtpos + 1]) : "ERR!!!";
            int len = safe ? evts[evtpos + 2] : 3;
            printf(": %d [%d]~~~%.*s~~~ (srcsz=%zu)",
                   evts[evtpos+1], evts[evtpos+2],
                   len, str, parsed_yaml.len);
        }
        printf("\n");
    }
    C4_SUPPRESS_WARNING_GCC_POP
}
} // namespace extra
} // namespace yml
} // namespace c4
