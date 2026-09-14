#ifndef C4_YML_EXTRA_INTS_UTILS_HPP_
#define C4_YML_EXTRA_INTS_UTILS_HPP_

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

/** @addtogroup doc_event_handlers_ints
 * @{ */

/** Convert bit mask of @ref ievt::evt_bits to text. */
RYML_EXPORT size_t to_str(substr buf, evt_bits flags) noexcept;
/** Convert bit mask of @ref ievt::evt_bits to text. */
RYML_EXPORT csubstr to_str_sub(substr buf, evt_bits flags);

/** Print integer events to stdout */
RYML_EXPORT void events_ints_print(csubstr parsed_yaml, csubstr arena,
                                   evt_bits const* evts_ints, evt_size evts_ints_sz);

/** @} */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4



/** @cond dev */
namespace c4 {
namespace yml {
namespace extra {
RYML_DEPRECATED("use ievt::events_ints_print")
inline void events_ints_print(csubstr parsed_yaml, csubstr arena,
                              ievt::evt_bits const* evts_ints,
                              ievt::evt_size evts_ints_sz)
{
    ievt::events_ints_print(parsed_yaml, arena, evts_ints, evts_ints_sz);
}
} // namespace extra
} // namespace yml
} // namespace c4
/** @endcond */

#endif /* C4_YML_EXTRA_INTS_UTILS_HPP_ */
