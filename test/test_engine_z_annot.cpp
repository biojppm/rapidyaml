#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc0_0, Location(1, 7), "&a &b x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc0_0, Location(1, 7), "!a !b x")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc0_1, Location(1, 7), "&a &b [x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc0_1, Location(1, 7), "!a !b [x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc0_2, Location(1, 7), "&a &b {a: b}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc0_2, Location(1, 7), "!a !b {a: b}")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc1_0, Location(1, 7), "&a &b &c x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc1_0, Location(1, 7), "!a !b !c x")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc1_1, Location(1, 7), "&a &b &c [x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc1_1, Location(1, 7), "!a !b !c [x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc1_2, Location(1, 7), "&a &b &c {a: b}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc1_2, Location(1, 7), "!a !b !c {a: b}")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc2_0, Location(3, 2), "&a\n&b\nx")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc2_0, Location(3, 2), "!a\n!b\nx")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc2_1, Location(/*fixme*/), "&a\n&b\n[x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc2_1, Location(/*fixme*/), "!a\n!b\n[x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc2_2, Location(/*fixme*/), "&a\n&b\n{a: b}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc2_2, Location(/*fixme*/), "!a\n!b\n{a: b}")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc3_0, Location(3, 3), "&a\n&b\n&c\nx")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc3_0, Location(3, 3), "!a\n!b\n!c\nx")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc3_1, Location(3, 3), "&a\n&b\n&c\n[x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc3_1, Location(3, 3), "!a\n!b\n!c\n[x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsDoc3_2, Location(3, 3), "&a\n&b\n&c\n{a: b}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___Doc3_2, Location(3, 3), "!a\n!b\n!c\n{a: b}")


ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow0_0, Location(1, 9), "[&a &b x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqFlow0_0, Location(1, 9), "[!a !b x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow0_1, Location(1, 8), "[&a &b [x]]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqFlow0_1, Location(1, 8), "[!a !b [x]]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock0_0, Location(1, 9), "- &a &b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock0_0, Location(1, 9), "- !a !b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock0_1, Location(1, 8), "- &a &b\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock0_1, Location(1, 8), "- !a !b\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock0_0nl, Location(4, 1), "-\n  &a\n  &b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock0_0nl, Location(4, 1), "-\n  !a\n  !b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock0_1nl, Location(4, 3), "-\n  &a\n  &b\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock0_1nl, Location(4, 3), "-\n  !a\n  !b\n  - x")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow1_0, Location(1, 11), "[&a &b &c x]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqFlow1_0, Location(1, 11), "[!a !b !c x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow1_1, Location(1, 11), "[&a &b &c [x]]")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqFlow1_1, Location(1, 11), "[!a !b !c [x]]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock1_0, Location(1, 9), "- &a &b &c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock1_0, Location(1, 9), "- !a !b !c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock1_1, Location(1, 9), "- &a &b &c\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock1_1, Location(1, 9), "- !a !b !c\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock1_0nl, Location(3, 5), "- &a\n  &b\n  &c\n  x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock1_0nl, Location(3, 5), "- !a\n  !b\n  !c\n  x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock1_1nl, Location(3, 5), "- &a\n  &b\n  &c\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___SeqBlock1_1nl, Location(3, 5), "- !a\n  !b\n  !c\n  - x")


ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow0_0, Location(1, 14), "{val: &a &b x}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValFlow0_0, Location(1, 14), "{val: !a !b x}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow0_1, Location(1, 13), "{val: &a &b [x]}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValFlow0_1, Location(1, 13), "{val: !a !b [x]}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock0_0, Location(1, 12), "val: &a &b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValBlock0_0, Location(1, 12), "val: !a !b x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock0_1, Location(1, 11), "val: &a &b\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValBlock0_1, Location(1, 11), "val: !a !b\n  - x")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow1_0, Location(1, 16), "{val: &a &b &c x}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValFlow1_0, Location(1, 16), "{val: !a !b !c x}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow1_1, Location(1, 16), "{val: &a &b &c [x]}")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValFlow1_1, Location(1, 16), "{val: !a !b !c [x]}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock1_0, Location(1, 12), "val: &a &b &c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValBlock1_0, Location(1, 12), "val: !a !b !c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock1_1, Location(1, 12), "val: &a &b &c\n  - x")
ENGINE_TEST_ERRLOC(Github680TooManyTags___MapValBlock1_1, Location(1, 12), "val: !a !b !c\n  - x")

} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
