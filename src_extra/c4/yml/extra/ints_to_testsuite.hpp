#ifndef _C4_YML_EXTRA_INTS_TO_TESTSUITE_HPP_
#define _C4_YML_EXTRA_INTS_TO_TESTSUITE_HPP_

#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#include "c4/yml/extra/event_handler_ints.hpp"
#endif


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {


/** @addtogroup doc_event_handlers
 * @{ */

/** Create a testsuite event string from integer events.
 *
 * This overload receives a buffer where the string should be written,
 * and returns the size needed for the buffer. If that size is larger
 * than the buffer's size, the user must resize the buffer and call
 * again. */
RYML_EXPORT C4_NODISCARD
size_t events_ints_to_testsuite(csubstr parsed_yaml,
                                csubstr arena,
                                ievt::DataType const* evts_ints,
                                ievt::DataType evts_ints_sz,
                                substr evts_testsuite);

/** Create a testsuite event string from integer events, writing into
 * an output container. */
template<class Container>
void events_ints_to_testsuite(csubstr parsed_yaml,
                              csubstr arena,
                              ievt::DataType const* evts_ints,
                              ievt::DataType evts_ints_sz,
                              Container *evts_testsuite)
{
    size_t len = events_ints_to_testsuite(parsed_yaml, arena, evts_ints, evts_ints_sz, to_substr(*evts_testsuite));
    if(len > evts_testsuite->size())
    {
        evts_testsuite->resize(len);
        len = events_ints_to_testsuite(parsed_yaml, arena, evts_ints, evts_ints_sz, to_substr(*evts_testsuite));
    }
    evts_testsuite->resize(len);
}

/** Create a testsuite event string from integer events, returning a
 * new container with the result. */
template<class Container>
Container events_ints_to_testsuite(csubstr parsed_yaml,
                                   csubstr arena,
                                   ievt::DataType const* evts_ints,
                                   ievt::DataType evts_ints_sz)
{
    Container ret;
    events_ints_to_testsuite(parsed_yaml, arena, evts_ints, evts_ints_sz, &ret);
    return ret;
}

/** @} */

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* _C4_YML_EXTRA_INTS_TO_TESTSUITE_UTILS_HPP_ */
