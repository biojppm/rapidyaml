#ifndef C4_YML_EXTRA_INTS_FORMAT_HPP_
#define C4_YML_EXTRA_INTS_FORMAT_HPP_

/** @file event_ints.hpp */

#ifndef RYML_SINGLE_HEADER
#ifndef C4_FORMAT_HPP_
#include <c4/format.hpp>
#endif
#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include <c4/yml/extra/event_ints.hpp>
#endif
#ifndef C4_YML_EXTRA_INTS_UTILS_HPP_
#include <c4/yml/extra/ints_utils.hpp>
#endif
#endif

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)

template<class Writer>
C4_NO_INLINE
void format_ints(Writer &writer,
                 csubstr parsed_yaml,
                 csubstr arena,
                 evt_bits const* evts, evt_size evts_sz,
                 bool indent)
{
    if(!evts_sz)
        return;
    char buf[200];
    evt_size level = 0;
    bool uses_arena = false;
    for(evt_bits evtpos = 0, evtnumber = 0;
        true;
        ++evtnumber,
            evtpos = ievt::nextpos(evts, evtpos))
    {
        evt_bits evt = evts[evtpos];
        writer.append('[');
        writer.append(to_chars_sub(buf, evtnumber));
        writer.append("][");
        writer.append(to_chars_sub(buf, evtpos));
        writer.append("] ");
        if(indent)
        {
            if(evt & ievt::END_)
                --level;
            writer.append(' ', (size_t)(2*level)); // NOLINT
            if(evt & ievt::BEG_)
                ++level;
        }
        writer.append(ievt::to_str_sub(buf, evt));
        writer.append('(');
        writer.append(to_chars_sub(buf, c4::fmt::hex(evt)));
        writer.append(')');
        if(evt & ievt::WSTR)
        {
            evt_size offs = evts[evtpos + 1];
            evt_size len = evts[evtpos + 2];
            bool in_arena = evt & ievt::AREN;
            uses_arena = uses_arena || in_arena;
            csubstr region = !in_arena ? parsed_yaml : arena;
            bool safe = (offs >= 0) && (len >= 0)
                && (offs <= (evt_bits)region.len) // NOLINT
                && (len <= ((evt_bits)region.len - evts[evtpos + 1]));
            writer.append(": ");
            writer.append(to_chars_sub(buf, offs));
            writer.append(" [");
            writer.append(to_chars_sub(buf, len));
            writer.append("]~~~");
            if(safe)
                writer.append(region.sub((size_t)offs, (size_t)len));
            else
                writer.append("ERR!!!"); // LCOV_EXCL_LINE
            writer.append("~~~");
        }
        if(evt & ievt::RREF)
        {
            evt_size begin = evts[evtpos + 1];
            evt_size end = evts[evtpos + 2];
            writer.append(": ");
            writer.append(to_chars_sub(buf, begin));
            writer.append(':');
            writer.append(to_chars_sub(buf, end));
        }
        writer.append('\n');
        if(evts[evtpos] == ievt::ESTR)
            break;
    }
    writer.append("src.len=");
    writer.append(to_chars_sub(buf, parsed_yaml.len));
    if(uses_arena)
        writer.append(" arena.len=");
    else
        writer.append(" arena.len(unused)=");
    writer.append(to_chars_sub(buf, arena.len));
    writer.append('\n');
}

C4_SUPPRESS_WARNING_POP
// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4


#endif /* C4_YML_EXTRA_INTS_FORMAT_HPP_ */
