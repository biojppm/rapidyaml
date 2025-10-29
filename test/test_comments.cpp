#ifndef RYML_SINGLE_HEADER
#include <c4/yml/std/std.hpp>
#include <c4/yml/yml.hpp>
#endif
#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"


namespace c4 {
namespace yml {

TEST(comment_list, insertion_order)
{
}


//-----------------------------------------------------------------------------

/*WIP*/GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(YmlTestCase);


// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
