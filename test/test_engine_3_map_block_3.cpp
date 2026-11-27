#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {
using Loc = Location;

ENGINE_TEST(Ind0ScalarOk,
            "map:\n a\n"
            ,
            "map: a\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "=VAL :a\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(Ind0Scalar, Loc(2, 2), "map:\na")
ENGINE_TEST_ERRLOC(Ind0Anch, Loc(3, 1), "map:\n&a")
ENGINE_TEST_ERRLOC(Ind0AnchTag, Loc(3, 1), "map:\n&a !t")
ENGINE_TEST_ERRLOC(Ind0AnchTag1, Loc(4, 1), "map:\n&a\n!t")
ENGINE_TEST_ERRLOC(Ind0TagAnch, Loc(3, 1), "map:\n!t &a")
ENGINE_TEST_ERRLOC(Ind0TagAnch1, Loc(4, 1), "map:\n!t\n&a")
ENGINE_TEST_ERRLOC(Ind0AnchScalar, Loc(2, 5), "map:\n&a v")
ENGINE_TEST_ERRLOC(Ind0AnchScalar1, Loc(3, 2), "map:\n&a\nv")
ENGINE_TEST_ERRLOC(Ind0TagScalar, Loc(2, 5), "map:\n!t v")
ENGINE_TEST_ERRLOC(Ind0TagScalar1, Loc(3, 2), "map:\n!t\nv")
ENGINE_TEST_ERRLOC(Ind0AnchTagScalar, Loc(2, 8), "map:\n&a !t v")
ENGINE_TEST_ERRLOC(Ind0TagAnchScalar, Loc(2, 8), "map:\n!t &a v")
ENGINE_TEST_ERRLOC(Ind0AnchTagScalar1, Loc(4, 2), "map:\n&a\n!t\nv")
ENGINE_TEST_ERRLOC(Ind0TagAnchScalar1, Loc(4, 2), "map:\n!t\n&a\nv")

ENGINE_TEST_ERRLOC(Ind0ScalarDoc, Loc(2, 2), "map:\na\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchDoc, Loc(3, 1), "map:\n&a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagDoc, Loc(3, 1), "map:\n&a !t\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchDoc, Loc(3, 1), "map:\n!t &a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagDoc1, Loc(4, 1), "map:\n&a\n!t\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchDoc1, Loc(4, 1), "map:\n!t\n&a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchScalarDoc, Loc(2, 5), "map:\n&a v\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagScalarDoc, Loc(3, 1), "map:\n!t v--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchScalarDoc1, Loc(3, 2), "map:\n&a\nv\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagScalarDoc1, Loc(4, 1), "map:\n!t\nv--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagScalarDoc, Loc(3, 1), "map:\n&a !t v--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchScalarDoc, Loc(3, 1), "map:\n!t &a v--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagScalarDoc1, Loc(5, 1), "map:\n&a\n!t\nv--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchScalarDoc1, Loc(5, 1), "map:\n!t\n&a\nv--- a\n")


ENGINE_TEST(Ind0Map,
            "map:\n a: b\n"
            ,
            "map:\n  a: b\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(Ind0AnchMap, Loc(3, 2), "map:\n&a\na:\n")
ENGINE_TEST_ERRLOC(Ind0TagMap, Loc(3, 2), "map:\n!t\na:\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagMap, Loc(3, 2), "map:\n&a !t\na:\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchMap, Loc(3, 2), "map:\n!t &a\na:\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagMap1, Loc(4, 2), "map:\n&a\n!t\na:\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchMap1, Loc(4, 2), "map:\n!t\n&a\na:\n")

ENGINE_TEST_ERRLOC(Ind0AnchMapDoc, Loc(3, 2), "map:\n&a\na:\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagMapDoc, Loc(3, 2), "map:\n!t\na:\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagMapDoc, Loc(3, 2), "map:\n&a !t\na:\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchMapDoc, Loc(3, 2), "map:\n!t &a\na:\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagMap1Doc, Loc(4, 2), "map:\n&a\n!t\na:\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchMap1Doc, Loc(4, 2), "map:\n!t\n&a\na:\n--- a\n")


ENGINE_TEST(Ind0IndlSeq,
            "map:\n- a\n"
            ,
            "map:\n  - a\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(Ind0AnchIndlSeq, Loc(3, 1), "map:\n&a\n- a\n")
ENGINE_TEST_ERRLOC(Ind0TagIndlSeq, Loc(3, 1), "map:\n!t\n- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagIndlSeq, Loc(3, 1), "map:\n&a !t\n- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagIndlSeq1, Loc(4, 1), "map:\n&a\n!t\n- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchIndlSeq, Loc(3, 1), "map:\n!t &a\n- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchIndlSeq1, Loc(4, 1), "map:\n!t\n&a\n- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchIndlSeqDoc, Loc(3, 1), "map:\n&a\n- a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagIndlSeqDoc, Loc(3, 1), "map:\n!t\n- a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagIndlSeqDoc, Loc(3, 1), "map:\n&a !t\n- a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0AnchTagIndlSeq1Doc, Loc(4, 1), "map:\n&a\n!t\n- a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchIndlSeqDoc, Loc(3, 1), "map:\n!t &a\n- a\n--- a\n")
ENGINE_TEST_ERRLOC(Ind0TagAnchIndlSeq1Doc, Loc(4, 1), "map:\n!t\n&a\n- a\n--- a\n")


ENGINE_TEST(Ind0AnchIndlSeqOk,
            "map:\n &a\n - a\n"
            ,
            "map: &a\n  - a\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+SEQ &a\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("a"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Ind0AnchTagIndlSeqOk,
            "map:\n &a !t\n - a\n"
            ,
            "map: &a !t\n  - a\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+SEQ &a <!t>\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("a"));
    ___(ps.set_val_tag("!t"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Ind0TagAnchIndlSeqOk,
            "map:\n !t\n &a\n - a\n"
            ,
            "map: &a !t\n  - a\n"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+SEQ &a <!t>\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("a"));
    ___(ps.set_val_tag("!t"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


} // namespace yml
} // namespace c4
