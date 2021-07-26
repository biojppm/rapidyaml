#ifndef C4_YML_TEST_SUITE_EVENTS_HPP_
#define C4_YML_TEST_SUITE_EVENTS_HPP_

#include <c4/yml/tree.hpp>

namespace c4 {
namespace yml {

struct EventsParser
{
    void parse(csubstr src, Tree *C4_RESTRICT tree);
};

} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_EVENTS_HPP_ */
