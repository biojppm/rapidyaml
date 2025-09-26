#ifndef C4_YML_TEST_SUITE_PARTS_HPP_
#define C4_YML_TEST_SUITE_PARTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/common.hpp>
#include <c4/span.hpp>
#endif
#include <c4/log/log.hpp>

namespace c4 {
namespace yml {

struct AllowedFailure
{
    csubstr test_name;
    csubstr reason;
    operator bool() const { return !test_name.empty(); }
};

AllowedFailure is_failure_expected(csubstr casename);
AllowedFailure case_has_container_keys(csubstr casename);

} // namespace c4
} // namespace yml


#endif /* C4_YML_TEST_SUITE_PARTS_HPP_ */
