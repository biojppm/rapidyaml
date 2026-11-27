#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow0, Location(1, 11), ""
                   "[&a &b &c x]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqFlow1, Location(1, 11), ""
                   "[&a &b &c [x]]")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock0, Location(1, 9), ""
                   "- &a &b &c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsSeqBlock1, Location(1, 9), ""
                   "- &a &b &c\n"
                   "  - x")

ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow0, Location(1, 16), ""
                   "{val: &a &b &c x}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValFlow1, Location(1, 16), ""
                   "{val: &a &b &c [x]}")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock0, Location(1, 12), ""
                   "val: &a &b &c x\n")
ENGINE_TEST_ERRLOC(Github680TooManyAnchorsMapValBlock1, Location(1, 12), ""
                   "val: &a &b &c\n"
                   "  - x")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(RefAndAnchorDocVal0_0, Location(1, 6), ""
                   "*a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorDocVal0_1, Location(1, 6), ""
                   "&b *a")

ENGINE_TEST_ERRLOC(RefAndAnchorDocVal1_0, Location(1, 10), ""
                   "--- *a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorDocVal1_1, Location(1, 10), ""
                   "--- &b *a")

ENGINE_TEST_ERRLOC(RefAndAnchorDocVal2_0, Location(2, 6), ""
                   "---\n"
                   "*a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorDocVal2_1, Location(2, 6), ""
                   "---\n"
                   "&b *a")

ENGINE_TEST_ERRLOC(RefAndAnchorDocVal3_0, Location(2, 6), ""
                   "...\n"
                   "*a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorDocVal3_1, Location(2, 6), ""
                   "...\n"
                   "&b *a")


ENGINE_TEST_ERRLOC(RefAndAnchorSeqBlck0_0, Location(1, 6), ""
                   "- *a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorSeqBlck0_1, Location(1, 8), ""
                   "- &b *a")
ENGINE_TEST_ERRLOC(RefAndAnchorSeqFlow0_0, Location(1, 7), ""
                   "[0,*a &b]")
ENGINE_TEST_ERRLOC(RefAndAnchorSeqFlow0_1, Location(1, 9), ""
                   "[0,&b *a]")


ENGINE_TEST_ERRLOC(RefAndAnchorMapBlckKey0_0, Location(1, 7),""
                   "*a &b : foo")
ENGINE_TEST_ERRLOC(RefAndAnchorMapBlckKey0_1, Location(1, 9), ""
                   "&b *a : foo")
ENGINE_TEST_ERRLOC(RefAndAnchorMapFlowKey0_0, Location(1, 5), ""
                   "{*a &b : foo}")
ENGINE_TEST_ERRLOC(RefAndAnchorMapFlowKey0_1, Location(1, 7), ""
                   "{&b *a : foo}")

ENGINE_TEST_ERRLOC(RefAndAnchorMapBlckVal0_0, Location(1, 9), ""
                   "foo: *a &b")
ENGINE_TEST_ERRLOC(RefAndAnchorMapBlckVal0_1, Location(1, 11), ""
                   "foo: &b *a")
ENGINE_TEST_ERRLOC(RefAndAnchorMapFlowVal0_0, Location(1, 10), ""
                   "{foo: *a &b}")
ENGINE_TEST_ERRLOC(RefAndAnchorMapFlowVal0_1, Location(1, 12), ""
                   "{foo: &b *a}")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorDocVal0_0, Location(1, 6+1), ""
                   "[a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefDocVal0_0, Location(1, 6+1), ""
                   "[a] *b")

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorDocVal1_0, Location(1, 1+10), ""
                   "--- [a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefDocVal1_0, Location(1, 1+10), ""
                   "--- [a] *b")

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorDocVal2_0, Location(2, 6+1), ""
                   "---\n"
                   "[a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefDocVal2_0, Location(2, 6+1), ""
                   "---\n"
                   "[a] *b")

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorDocVal3_0, Location(2, 6+1), ""
                   "...\n"
                   "[a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefDocVal3_0, Location(2, 6+1), ""
                   "...\n"
                   "[a] *b")

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorSeqBlck0_0, Location(1, 6+1), ""
                   "- [a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefSeqBlck0_0, Location(1, 6+1), ""
                   "- [a] *b")
ENGINE_TEST_ERRLOC(SeqFlowAndAnchorSeqFlow0_0, Location(1, 7+1), ""
                   "[0,[a] &b]")
ENGINE_TEST_ERRLOC(SeqFlowAndRefSeqFlow0_0, Location(1, 7+1), ""
                   "[0,[a] *b]")


ENGINE_TEST_ERRLOC_(SeqFlowAndAnchorMapBlckKey0_0, HAS_CONTAINER_KEYS, Location(1, 8),""
                   "[a] &b : foo")
ENGINE_TEST_ERRLOC_(SeqFlowAndRefMapBlckKey0_0, HAS_CONTAINER_KEYS, Location(1, 7),""
                   "[a] *b : foo")
ENGINE_TEST_ERRLOC_(SeqFlowAndAnchorMapFlowKey0_0, HAS_CONTAINER_KEYS, Location(1, 6), ""
                   "{[a] &b : foo}")
ENGINE_TEST_ERRLOC_(SeqFlowAndRefMapFlowKey0_0, HAS_CONTAINER_KEYS, Location(1, 6), ""
                   "{[a] *b : foo}")

ENGINE_TEST_ERRLOC(SeqFlowAndAnchorMapBlckVal0_0, Location(1, 10), ""
                   "foo: [a] &b")
ENGINE_TEST_ERRLOC(SeqFlowAndRefMapBlckVal0_0, Location(1, 10), ""
                   "foo: [a] *b")
ENGINE_TEST_ERRLOC(SeqFlowAndAnchorMapFlowVal0_0, Location(1, 11), ""
                   "{foo: [a] &b}")
ENGINE_TEST_ERRLOC(SeqFlowAndRefMapFlowVal0_0, Location(1, 11), ""
                   "{foo: [a] *b}")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(MapFlowAndAnchorDocVal0_0, Location(1, 10), ""
                   "{d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefDocVal0_0, Location(1, 10), ""
                   "{d: c} *b")

ENGINE_TEST_ERRLOC(MapFlowAndAnchorDocVal1_0, Location(1, 14), ""
                   "--- {d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefDocVal1_0, Location(1, 14), ""
                   "--- {d: c} *b")

ENGINE_TEST_ERRLOC(MapFlowAndAnchorDocVal2_0, Location(2, 10), ""
                   "---\n"
                   "{d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefDocVal2_0, Location(2, 10), ""
                   "---\n"
                   "{d: c} *b")

ENGINE_TEST_ERRLOC(MapFlowAndAnchorDocVal3_0, Location(2, 10), ""
                   "...\n"
                   "{d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefDocVal3_0, Location(2, 10), ""
                   "...\n"
                   "{d: c} *b")

ENGINE_TEST_ERRLOC(MapFlowAndAnchorSeqBlck0_0, Location(1, 10), ""
                   "- {d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefSeqBlck0_0, Location(1, 10), ""
                   "- {d: c} *b")
ENGINE_TEST_ERRLOC(MapFlowAndAnchorSeqFlow0_0, Location(1, 11), ""
                   "[0,{d: c} &b]")
ENGINE_TEST_ERRLOC(MapFlowAndRefSeqFlow0_0, Location(1, 11), ""
                   "[0,{d: c} *b]")


ENGINE_TEST_ERRLOC_(MapFlowAndAnchorMapBlckKey0_0, HAS_CONTAINER_KEYS, Location(1, 12),""
                   "{d: c} &b: foo")
ENGINE_TEST_ERRLOC_(MapFlowAndRefMapBlckKey0_0, HAS_CONTAINER_KEYS, Location(1, 11),""
                   "{d: c} *b: foo")
ENGINE_TEST_ERRLOC_(MapFlowAndAnchorMapFlowKey0_0, HAS_CONTAINER_KEYS, Location(1, 9), ""
                   "{{d: c} &b: foo}")
ENGINE_TEST_ERRLOC_(MapFlowAndRefMapFlowKey0_0, HAS_CONTAINER_KEYS, Location(1, 9), ""
                   "{{d: c} *b: foo}")

ENGINE_TEST_ERRLOC(MapFlowAndAnchorMapBlckVal0_0, Location(1, 13), ""
                   "foo: {d: c} &b")
ENGINE_TEST_ERRLOC(MapFlowAndRefMapBlckVal0_0, Location(1, 13), ""
                   "foo: {d: c} *b")
ENGINE_TEST_ERRLOC(MapFlowAndAnchorMapFlowVal0_0, Location(1, 14), ""
                   "{foo: {d: c} &b}")
ENGINE_TEST_ERRLOC(MapFlowAndRefMapFlowVal0_0, Location(1, 14), ""
                   "{foo: {d: c} *b}")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_0, Location(1, 7), ""
                   "[&a &b]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_1, Location(1, 9), ""
                   "[&a &b c]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_2, Location(1, 7), ""
                   "[&a &b,]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_3, Location(1, 9), ""
                   "[&a &b c,]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_4, Location(1, 10), ""
                   "[0, &a &b]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_5, Location(1, 12), ""
                   "[0, &a &b c]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_6, Location(1, 10), ""
                   "[0, &a &b,]")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqFlow0_7, Location(1, 12), ""
                   "[0, &a &b c,]")


ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_0, Location(1, 8), ""
                   "- &a &b")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_1, Location(1, 9), ""
                   "- &a &b c")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_2, Location(1, 8), ""
                   "- &a &b\n-")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_3, Location(1, 9), ""
                   "- &a &b c\n-")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_4, Location(2, 8), ""
                   "- 0\n- &a &b")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_5, Location(2, 9), ""
                   "- 0\n- &a &b c")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_6, Location(2, 8), ""
                   "- 0\n- &a &b\n-")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_7, Location(2, 9), ""
                   "- 0\n- &a &b c\n-")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_8, Location(1, 8), ""
                   "- &a &b\n  - nested")
ENGINE_TEST_ERRLOC(DoubleAnchorSeqBlck0_9, Location(1, 8), ""
                   "- &a &b\n  nested: map")


ENGINE_TEST_ERRLOC(DoubleAnchorMapflowKey0_0, Location(1, 12), ""
                   "{&a &b: val}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowKey0_1, Location(1, 9), ""
                   "{&a &b c: val}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowKey0_4, Location(1, 15), ""
                   "{0, &a &b: val}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowKey0_5, Location(1, 12), ""
                   "{0, &a &b c: val}")

ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_0, Location(1, 12), ""
                   "{key: &a &b}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_1, Location(1, 14), ""
                   "{key: &a &b c}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_4, Location(1, 20), ""
                   "{0: 1, key: &a &b c}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_5, Location(1, 20), ""
                   "{0: 1, key: &a &b c}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_6, Location(1, 12), ""
                   "{key: &a &b,}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_7, Location(1, 14), ""
                   "{key: &a &b c,}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_8, Location(1, 20), ""
                   "{0: 1, key: &a &b c,}")
ENGINE_TEST_ERRLOC(DoubleAnchorMapflowVal0_9, Location(1, 20), ""
                   "{0: 1, key: &a &b c,}")


ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckKey0_0, Location(1, 8), ""
                   "&a &b: val")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckKey0_1, Location(1, 7), ""
                   "&a &b c: val")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckKey0_4, Location(2, 6), ""
                   "0\n&a &b: val")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckKey0_5, Location(2, 8), ""
                   "0\n&a &b c: val")

ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_0, Location(1, 11), ""
                   "key: &a &b")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_1, Location(1, 12), ""
                   "key: &a &b c")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_4, Location(2, 12), ""
                   "0: 1\nkey: &a &b c")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_5, Location(2, 12), ""
                   "0: 1\nkey: &a &b c")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_6, Location(1, 11), ""
                   "key: &a &b\n")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_7, Location(1, 12), ""
                   "key: &a &b c\n")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_8, Location(2, 12), ""
                   "0: 1\nkey: &a &b c\n")
ENGINE_TEST_ERRLOC(DoubleAnchorMapBlckVal0_9, Location(2, 12), ""
                   "0: 1\nkey: &a &b c\n")


} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
