#ifndef C4_YML_PARSE_ENGINE_DEF_HPP_
#include <c4/yml/parse_engine.def.hpp>
#endif
#ifndef C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif

namespace c4 {
namespace yml {

// instantiate the template
template class RYML_EXPORT ParseEngine<extra::EventHandlerInts>;

} // namespace yml
} // namespace c4
