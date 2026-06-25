#ifndef _C4_YML_EXTRA_INTS_UTILS_HPP_
#define _C4_YML_EXTRA_INTS_UTILS_HPP_

#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {


/** @addtogroup doc_event_handlers
 * @{ */


namespace ievt {
/** Convert bit mask of @ref ievt::EventBits to text. */
RYML_EXPORT size_t to_str(substr buf, yml::extra::ievt::evt_bits flags) noexcept;
/** Convert bit mask of @ref ievt::EventBits to text. */
RYML_EXPORT csubstr to_str_sub(substr buf, yml::extra::ievt::evt_bits flags);
} // namespace ievt


/** Print integer events to stdout */
RYML_EXPORT void events_ints_print(csubstr parsed_yaml, csubstr arena, ievt::evt_bits const* evts_ints, ievt::evt_bits evts_ints_sz);


/** @} */

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _C4_YML_EXTRA_INTS_UTILS_HPP_ */
