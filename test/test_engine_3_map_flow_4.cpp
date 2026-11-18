#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_a, Location(1, 7+3), ""
                   "{c: d} &a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_t, Location(1, 7+3), ""
                   "{c: d} !t")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_q, Location(1, 5+3), ""
                   "{c: d} ? q")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_m, Location(1, 5+3), ""
                   "{c: d} {a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_s, Location(1, 5+3), ""
                   "{c: d} [a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_0, Location(1, 6+3), ""
                   "{c: d} b")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_1, Location(1, 5+3), ""
                   "{c: d} 'b'")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_2, Location(1, 5+3), ""
                   "{c: d} \"b\"")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_3, Location(2, 1), ""
                   "{c: d}\n|\n  a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal0_4, Location(2, 1), ""
                   "{c: d}\n>\n  a")

ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_a, Location(1, 11+3), ""
                   "--- {c: d} &a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_t, Location(1, 11+3), ""
                   "--- {c: d} !t")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_q, Location(1, 9+3), ""
                   "--- {c: d} ? a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_s, Location(1, 9+3), ""
                   "--- {c: d} [a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_m, Location(1, 9+3), ""
                   "--- {c: d} {a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_0, Location(1, 10+3), ""
                   "--- {c: d} b")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_1, Location(1, 9+3), ""
                   "--- {c: d} 'b'")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_2, Location(1, 9+3), ""
                   "--- {c: d} \"b\"")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_3, Location(2, 1), ""
                   "--- {c: d}\n|\n  a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal1_4, Location(2, 1), ""
                   "--- {c: d}\n>\n  a")

ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_a, Location(2, 7+3), ""
                   "---\n{c: d} &a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_t, Location(2, 7+3), ""
                   "---\n{c: d} !t")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_q, Location(2, 5+3), ""
                   "---\n{c: d} ? q")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_s, Location(2, 5+3), ""
                   "---\n{c: d} [q]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_m, Location(2, 5+3), ""
                   "---\n{c: d} {a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_0, Location(2, 6+3), ""
                   "---\n{c: d} b")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_1, Location(2, 5+3), ""
                   "---\n{c: d} 'b'")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_2, Location(2, 5+3), ""
                   "---\n{c: d} \"b\"")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_3, Location(3, 1), ""
                   "---\n{c: d}\n|\n  a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal2_4, Location(3, 1), ""
                   "---\n{c: d}\n>\n  a")

ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_a, Location(2, 7+3), ""
                   "...\n{c: d} &a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_t, Location(2, 7+3), ""
                   "...\n{c: d} !t")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_q, Location(2, 5+3), ""
                   "...\n{c: d} ? q")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_s, Location(2, 5+3), ""
                   "...\n{c: d} [a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_m, Location(2, 5+3), ""
                   "...\n{c: d} {a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_0, Location(2, 6+3), ""
                   "...\n{c: d} b")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_1, Location(2, 5+3), ""
                   "...\n{c: d} 'b'")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_2, Location(2, 5+3), ""
                   "...\n{c: d} \"b\"")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_3, Location(3, 1), ""
                   "...\n{c: d}\n|\n  a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarDocVal3_4, Location(3, 1), ""
                   "...\n{c: d}\n>\n  a")


ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_a, Location(1, 7+3), ""
                   "- {c: d} &a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_t, Location(1, 7+3), ""
                   "- {c: d} !t")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_q, Location(1, 7+3), ""
                   "- {c: d} ? q")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_s, Location(1, 7+3), ""
                   "- {c: d} [a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_m, Location(1, 7+3), ""
                   "- {c: d} {a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_0, Location(1, 7+3), ""
                   "- {c: d} b")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_1, Location(1, 7+3), ""
                   "- {c: d} 'b'")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_2, Location(1, 7+3), ""
                   "- {c: d} \"b\"")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_3, Location(2, 1), ""
                   "- {c: d}\n"
                   "  |\n  a")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqBlck_4, Location(2, 1), ""
                   "- {c: d}\n"
                   "  >\n  a")

ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_a, Location(1, 9+3), ""
                   "[0, {c: d} &a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_t, Location(1, 9+3), ""
                   "[0, {c: d} !t]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_q, Location(1, 9+3), ""
                   "[0, {c: d} ? q]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_s, Location(1, 9+3), ""
                   "[0, {c: d} [a]]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_m, Location(1, 9+3), ""
                   "[0, {c: d} {a: b}]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_0, Location(1, 9+3), ""
                   "[0, {c: d} b]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_1, Location(1, 9+3), ""
                   "[0, {c: d} 'b']")
ENGINE_TEST_ERRLOC(MapFlowAndScalarSeqFlow_2, Location(1, 9+3), ""
                   "[0, {c: d} \"b\"]")

ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_a, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} &a\n: val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_t, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} !t\n: val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_q, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} !t\n: val? q")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_s, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} !t\n: val[a]")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_m, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} !t\n: val{a: b}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_0, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} b\n: val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_1, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} 'b'\n: val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_2, HAS_CONTAINER_KEYS, Location(1, 7+3), ""
                   "? {c: d} \"b\"\n: val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_3, HAS_CONTAINER_KEYS, Location(2, 1), ""
                   "? {c: d}\n"
                    " |\n"
                    "   a\n"
                    ": val\n")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyBlck_4, HAS_CONTAINER_KEYS, Location(2, 1), ""
                   "? {c: d}\n"
                    " >\n"
                    "   a\n"
                    ": val\n")

ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_a, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} &a: val}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_t, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} !t: val}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_q, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} !t:? qal}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_s, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} !t:[a]}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_m, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} !t:{a: b}al}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_0, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} b: val}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_1, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} 'b': val}")
ENGINE_TEST_ERRLOC_(MapFlowAndScalarMapKeyFlow_2, HAS_CONTAINER_KEYS, Location(1, 8+3), ""
                   "{? {c: d} \"b\": val}")


ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_a, Location(1, 10+3), ""
                   "foo: {c: d} &a\n")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_t, Location(1, 10+3), ""
                   "foo: {c: d} !t\n")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_q, Location(1, 10+3), ""
                   "foo: {c: d} !t? q")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_s, Location(1, 10+3), ""
                   "foo: {c: d} !t[a]")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_m, Location(1, 10+3), ""
                   "foo: {c: d} !t{a: b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_0, Location(1, 10+3), ""
                   "foo: {c: d} b\n")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_1, Location(1, 10+3), ""
                   "foo: {c: d} 'b'\n")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_2, Location(1, 10+3), ""
                   "foo: {c: d} \"b\"\n")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_3, Location(2, 5), ""
                   "foo: {c: d}\n"
                    "    |\n"
                    "      a\n"
                    "")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValBlck_4, Location(2, 5), ""
                   "foo: {c: d}\n"
                    "    |\n"
                    "      a\n"
                    "")

ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_a, Location(1, 11+3), ""
                   "{foo: {c: d} &a}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_t, Location(1, 11+3), ""
                   "{foo: {c: d} !t}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_q, Location(1, 11+3), ""
                   "{foo: {c: d} ? q}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_s, Location(1, 11+3), ""
                   "{foo: {c: d} [a]}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_m, Location(1, 11+3), ""
                   "{foo: {c: d} {a: b}}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_0, Location(1, 11+3), ""
                   "{foo: {c: d} b}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_1, Location(1, 11+3), ""
                   "{foo: {c: d} 'b'}")
ENGINE_TEST_ERRLOC(MapFlowAndScalarMapValFlow_2, Location(1, 11+3), ""
                   "{foo: {c: d} \"b\"}")

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
