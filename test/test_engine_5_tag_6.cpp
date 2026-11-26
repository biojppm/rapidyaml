#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {



static const ParserOptions resolve_all_tags = ParserOptions{}.resolve_tags(true).resolve_tags_all(true);


//-----------------------------------------------------------------------------


ENGINE_TEST(TagTestSuiteQLJ7_0_custom,
            "--- !A\n"
            "a: b\n"
            "--- !B\n"
            "c: d\n"
            "--- !C\n"
            "e: f\n"
            ""
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <!A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!A"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!B"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("!C"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteQLJ7_0_custom_all, resolve_all_tags,
            "--- !A\n"
            "a: b\n"
            "--- !B\n"
            "c: d\n"
            "--- !C\n"
            "e: f\n"
            ""
            ,
            "--- !<!A>\n"
            "a: b\n"
            "--- !<!B>\n"
            "c: d\n"
            "--- !<!C>\n"
            "e: f\n"
            ""
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <!A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP <!C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            "")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!A>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!B>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<!C>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_(TagTestSuiteQLJ7_0_correct, resolve_all_tags,
            "%TAG !prefix! doc0:\n"
            "--- !prefix!A\n"
            "a: b\n"
            "...\n"
            "%TAG !prefix! doc1:\n"
            "--- !prefix!B\n"
            "c: d\n"
            "...\n"
            "%TAG !prefix! doc2:\n"
            "--- !prefix!C\n"
            "e: f\n"
            ,
            "%TAG !prefix! doc0:\n"
            "--- !<doc0:A>\n"
            "a: b\n"
            "...\n"
            "%TAG !prefix! doc1:\n"
            "--- !<doc1:B>\n"
            "c: d\n"
            "...\n"
            "%TAG !prefix! doc2:\n"
            "--- !<doc2:C>\n"
            "e: f\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP <doc0:A>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP <doc1:B>\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC ---\n"
            "+MAP <doc2:C>\n"
            "=VAL :e\n"
            "=VAL :f\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.add_directive_tag("!prefix!", "doc0:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc0:A>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!prefix!", "doc1:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc1:B>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_block());
    ___(ps.end_doc_expl());
    ___(ps.add_directive_tag("!prefix!", "doc2:"));
    ___(ps.begin_doc_expl());
    ___(ps.set_val_tag("<doc2:C>"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("e"));
    ___(ps.set_val_scalar_plain("f"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_0_no_directive, resolve_all_tags, Location(1, 14),
                   "--- !prefix!A\n"
                   "a: b\n"
                   "--- !prefix!B\n"
                   "c: d\n"
                   "--- !prefix!C\n"
                   "e: f\n"
                   "")

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_1, resolve_all_tags, Location(4, 14),
                   "%TAG !prefix! tag:example.com,2011:\n"
                   "--- !prefix!A\n"
                   "a: b\n"
                   "--- !prefix!B\n" // error here. %TAG applies only to next doc
                   "c: d\n"
                   "--- !prefix!C\n"
                   "e: f\n"
                   "")

ENGINE_TEST_ERROPT_(TagTestSuiteQLJ7_2, resolve_all_tags, Location(8, 14),
                   "%TAG !prefix! doc0:\n"
                   "--- !prefix!A\n"
                   "a: b\n"
                   "...\n"
                   "%TAG !prefix! doc1:\n"
                   "--- !prefix!B\n"
                   "c: d\n"
                   "--- !prefix!C\n" // error here. %TAG applies only to next doc
                   "e: f\n"
                   "")


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_0, Location(1, 7), ""
                   "[!t !u]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_1, Location(1, 9), ""
                   "[!t !u c]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_2, Location(1, 7), ""
                   "[!t !u,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_3, Location(1, 9), ""
                   "[!t !u c,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_4, Location(1, 10), ""
                   "[0, !t !u]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_5, Location(1, 12), ""
                   "[0, !t !u c]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_6, Location(1, 10), ""
                   "[0, !t !u,]")
ENGINE_TEST_ERRLOC(DoubleTagSeqFlow0_7, Location(1, 12), ""
                   "[0, !t !u c,]")


ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_0, Location(1, 8), ""
                   "- !t !u")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_1, Location(1, 9), ""
                   "- !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_2, Location(1, 8), ""
                   "- !t !u\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_3, Location(1, 9), ""
                   "- !t !u c\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_4, Location(2, 8), ""
                   "- 0\n- !t !u")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_5, Location(2, 9), ""
                   "- 0\n- !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_6, Location(2, 8), ""
                   "- 0\n- !t !u\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_7, Location(2, 9), ""
                   "- 0\n- !t !u c\n-")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_8, Location(1, 8), ""
                   "- !t !u\n  - nested")
ENGINE_TEST_ERRLOC(DoubleTagSeqBlck0_9, Location(1, 8), ""
                   "- !t !u\n  nested: map")


ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_0, Location(1, 12), ""
                   "{!t !u: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_1, Location(1, 9), ""
                   "{!t !u c: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_4, Location(1, 15), ""
                   "{0, !t !u: val}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowKey0_5, Location(1, 12), ""
                   "{0, !t !u c: val}")

ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_0, Location(1, 12), ""
                   "{key: !t !u}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_1, Location(1, 14), ""
                   "{key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_4, Location(1, 20), ""
                   "{0: 1, key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_5, Location(1, 20), ""
                   "{0: 1, key: !t !u c}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_6, Location(1, 12), ""
                   "{key: !t !u,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_7, Location(1, 14), ""
                   "{key: !t !u c,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_8, Location(1, 20), ""
                   "{0: 1, key: !t !u c,}")
ENGINE_TEST_ERRLOC(DoubleTagMapflowVal0_9, Location(1, 20), ""
                   "{0: 1, key: !t !u c,}")


ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_0, Location(1, 8), ""
                   "!t !u: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_1, Location(1, 7), ""
                   "!t !u c: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_4, Location(2, 6), ""
                   "0\n!t !u: val")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckKey0_5, Location(2, 8), ""
                   "0\n!t !u c: val")

ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_0, Location(1, 11), ""
                   "key: !t !u")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_1, Location(1, 12), ""
                   "key: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_4, Location(2, 12), ""
                   "0: 1\nkey: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_5, Location(2, 12), ""
                   "0: 1\nkey: !t !u c")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_6, Location(1, 11), ""
                   "key: !t !u\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_7, Location(1, 12), ""
                   "key: !t !u c\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_8, Location(2, 12), ""
                   "0: 1\nkey: !t !u c\n")
ENGINE_TEST_ERRLOC(DoubleTagMapBlckVal0_9, Location(2, 12), ""
                   "0: 1\nkey: !t !u c\n")

} // namespace yml
} // namespace c4
