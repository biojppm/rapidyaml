#ifndef RYML_SINGLE_HEADER
#include <c4/yml/node.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif
#include <c4/yml/extra/event_handler_testsuite.hpp>
#include <c4/yml/extra/scalar.hpp>


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
