#ifndef _C4_YML_EXTRA_SCALAR_HPP_
#define _C4_YML_EXTRA_SCALAR_HPP_

#ifndef _C4_SUBSTR_HPP_
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
namespace extra {

/** @addtogroup doc_event_handlers
 * @{ */

size_t escape_scalar(substr s, csubstr val);

/** @} */

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _C4_YML_EVT_EXTRA_SCALAR_HPP_ */
