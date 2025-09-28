#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#else
#include <c4/yml/node.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif

#ifndef _C4_YML_EXTRA_EVENT_HANDLER_TESTSUITE_HPP_
#include "c4/yml/extra/event_handler_testsuite.hpp"
#endif
#ifndef _C4_YML_EXTRA_SCALAR_HPP_
#include "c4/yml/extra/scalar.hpp"
#endif
#ifndef _C4_YML_EXTRA_SCALAR_HPP_
#include "c4/yml/extra/string.hpp"
#endif


namespace c4 {
namespace yml {
namespace extra {

void append_scalar_escaped(extra::string *es, csubstr val)
{
    size_t orig = es->size();
    es->resize(es->capacity());
    size_t sz = escape_scalar(substr(*es).sub(orig), val);
    if (orig + sz > es->size())
    {
        es->resize(orig + sz);
        sz = escape_scalar(substr(*es).sub(orig), val);
    }
    es->resize(orig + sz);
}

} // namespace extra

// instantiate the template
template class ParseEngine<extra::EventHandlerTestSuite>;

} // namespace yml
} // namespace c4
