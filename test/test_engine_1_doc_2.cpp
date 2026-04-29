#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

ENGINE_TEST(DocScalarNotDocToken0, HAS_MULTILINE_SCALAR,
            "multiline\n"
            "-- not a doc token\n"
            "- not a doc token\n"
            ".. not a doc token\n"
            ". not a doc token\n"
            ,
            "multiline -- not a doc token - not a doc token .. not a doc token . not a doc token\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :multiline -- not a doc token - not a doc token .. not a doc token . not a doc token\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("multiline -- not a doc token - not a doc token .. not a doc token . not a doc token"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocScalarNotDocToken1, HAS_MULTILINE_SCALAR,
            "multiline\n"
            "---not a doc token\n"
            "...not a doc token\n"
            ,
            "multiline ---not a doc token ...not a doc token\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :multiline ---not a doc token ...not a doc token\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("multiline ---not a doc token ...not a doc token"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocStream,
            "--- doc0\n--- 'doc1'\n--- \"doc2\"\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "=VAL :doc0\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL 'doc1\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL \"doc2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("doc0"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_squoted("doc1"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_dquoted("doc2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(DocStreamImplicitDocFirstVal,
            "doc0\n--- doc1\n"
            ,
            "--- doc0\n--- doc1\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL :doc0\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL :doc1\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_scalar_plain("doc0"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_scalar_plain("doc1"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstAnchor,
            "&anch1\n"
            "--- &anch2\n"
            ,
            "--- &anch1\n"
            "--- &anch2\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL &anch1 :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL &anch2 :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("anch1"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_anchor("anch2"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstTag,
            "!!str\n"
            "--- !!str\n"
            ,
            "--- !!str\n"
            "--- !!str\n"
            ,
            "+STR\n"
            "+DOC\n"
            "=VAL <!!str> :\n"
            "-DOC\n"
            "+DOC ---\n"
            "=VAL <!!str> :\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!!str"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstSeqFlowEmpty,
            "[]\n"
            "--- []\n"
            ,
            "--- []\n"
            "--- []\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(false));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq_flow(false));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstSeqFlow,
            "[a]\n"
            "--- [b]\n"
            ,
            "--- [a]\n"
            "--- [b]\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ []\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_flow(false));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_flow(false));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstMapEmpty,
            "{}\n"
            "--- {}\n"
            ,
            "--- {}\n"
            "--- {}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(false));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.end_map_flow(false));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocStreamImplicitDocFirstMap,
            "{a: b}\n"
            "--- {c: d}\n"
            ,
            "--- {a: b}\n"
            "--- {c: d}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP {}\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(false));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_flow(false));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SuddenDoc0,
            "- map:\n"
            "   foo: bar\n"
            "...\n"
            "seq:\n"
            "  - a\n"
            "  - b\n"
            "...\n"
            ,
            "---\n"
            "- map:\n"
            "    foo: bar\n"
            "---\n"
            "seq:\n"
            "  - a\n"
            "  - b\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :seq\n"
            "+SEQ\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.end_map_block());
    ___(ps.end_seq_block());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}

ENGINE_TEST(SuddenDoc1,
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "      - now inside a seq:\n"
            "          another map:\n"
            "          - another seq\n"
            "          - etc\n"
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "      - now inside a seq:\n"
            "          another map: the value\n"
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "      - now inside a seq:\n"
            "          another map:\n"
            "          - another seq\n"
            "          - etc\n"
            "...\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "      - now inside a seq:\n"
            "          another map: the value\n"
            "...\n",
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "        - now inside a seq:\n"
            "            another map:\n"
            "              - another seq\n"
            "              - etc\n"
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "        - now inside a seq:\n"
            "            another map: the value\n"
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "        - now inside a seq:\n"
            "            another map:\n"
            "              - another seq\n"
            "              - etc\n"
            "---\n"
            "map:\n"
            "  nested:\n"
            "    nested more:\n"
            "      nested even more:\n"
            "        - now inside a seq:\n"
            "            another map: the value\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "+SEQ\n"
            "=VAL :another seq\n"
            "=VAL :etc\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "=VAL :the value\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "+SEQ\n"
            "=VAL :another seq\n"
            "=VAL :etc\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "=VAL :nested\n"
            "+MAP\n"
            "=VAL :nested more\n"
            "+MAP\n"
            "=VAL :nested even more\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL :now inside a seq\n"
            "+MAP\n"
            "=VAL :another map\n"
            "=VAL :the value\n"
            "-MAP\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC ...\n"
            "-STR\n")
{
    auto build_map = [&ps]{
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("map"));
            ___(ps.begin_map_val_block());
                ___(ps.set_key_scalar_plain("nested"));
                ___(ps.begin_map_val_block());
                    ___(ps.set_key_scalar_plain("nested more"));
                    ___(ps.begin_map_val_block());
                        ___(ps.set_key_scalar_plain("nested even more"));
                        ___(ps.begin_seq_val_block());
                            ___(ps.begin_map_val_block());
                                ___(ps.set_key_scalar_plain("now inside a seq"));
                                ___(ps.begin_map_val_block());
                                    ___(ps.set_key_scalar_plain("another map"));
                                    ___(ps.begin_seq_val_block());
                                        ___(ps.set_val_scalar_plain("another seq"));
                                        ___(ps.add_sibling());
                                        ___(ps.set_val_scalar_plain("etc"));
                                    ___(ps.end_seq_block());
                                ___(ps.end_map_block());
                            ___(ps.end_map_block());
                        ___(ps.end_seq_block());
                    ___(ps.end_map_block());
                ___(ps.end_map_block());
            ___(ps.end_map_block());
        ___(ps.end_map_block());
    };
    auto build_seq = [&ps]{
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("map"));
            ___(ps.begin_map_val_block());
                ___(ps.set_key_scalar_plain("nested"));
                ___(ps.begin_map_val_block());
                    ___(ps.set_key_scalar_plain("nested more"));
                    ___(ps.begin_map_val_block());
                        ___(ps.set_key_scalar_plain("nested even more"));
                        ___(ps.begin_seq_val_block());
                            ___(ps.begin_map_val_block());
                                ___(ps.set_key_scalar_plain("now inside a seq"));
                                ___(ps.begin_map_val_block());
                                    ___(ps.set_key_scalar_plain("another map"));
                                    ___(ps.set_val_scalar_plain("the value"));
                                ___(ps.end_map_block());
                            ___(ps.end_map_block());
                        ___(ps.end_seq_block());
                    ___(ps.end_map_block());
                ___(ps.end_map_block());
            ___(ps.end_map_block());
        ___(ps.end_map_block());
    };
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    build_map();
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    build_seq();
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    build_map();
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    build_seq();
    ___(ps.end_doc_expl());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine0, Location(1, 5),
                   "--- - a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine0_1, Location(1, 5),
                   "--- - &a a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine0_2, Location(1, 5),
                   "--- - !t a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine0_3, Location(1, 5),
                   "--- - &a !t a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine0_4, Location(1, 5),
                   "--- - !t &a a\n"
    )

ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine1_1, Location(1, 8),
                   "--- &a - a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine1_2, Location(1, 8),
                   "--- !t - a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine1_3, Location(1, 11),
                   "--- &a !t - a\n"
    )
ENGINE_TEST_ERRLOC(DocErrSeqBlckSameLine1_4, Location(1, 11),
                   "--- !t &a - a\n"
    )


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DocErrMapBlckSameLine0, Location(1, 8),
                   "--- a: b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLine1, Location(1, 11),
                   "--- &a a: b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLine2, Location(1, 11),
                   "--- !t a: b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLine3, Location(1, 14),
                   "--- &a !t a: b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLine4, Location(1, 14),
                   "--- !t &a a: b\n"
    )

ENGINE_TEST_ERRLOC(DocErrMapBlckSameLineNoKey0, Location(1, 5),
                   "--- : b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLineNoKey1, Location(1, 8),
                   "--- &a : b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLineNoKey2, Location(1, 8),
                   "--- !t : b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLineNoKey3, Location(1, 11),
                   "--- &a !t : b\n"
    )
ENGINE_TEST_ERRLOC(DocErrMapBlckSameLineNoKey4, Location(1, 11),
                   "--- !t &a : b\n"
    )


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DocErrQmrkSameLine0, Location(1, 5),
                   "--- ? a\n"
                   "    : b\n"
    )

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
