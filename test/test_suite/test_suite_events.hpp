#ifndef C4_YML_TEST_SUITE_EVENTS_HPP_
#define C4_YML_TEST_SUITE_EVENTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/tree.hpp>
#endif

namespace c4 {
namespace yml {

struct EventsParser
{
    void parse(csubstr src, Tree *C4_RESTRICT tree);
};

} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_EVENTS_HPP_ */
