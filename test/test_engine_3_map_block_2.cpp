#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool singleline = false;

ENGINE_TEST(ContainerKey1Block0_0,
            HAS_CONTAINER_KEYS,
            "{this: is, a: keymap}: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block0_1,
            HAS_CONTAINER_KEYS,
            "{this: is, a: keymap}: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block1_0,
            HAS_CONTAINER_KEYS,
            "[this,is,a,seq,key]: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block1_1,
            HAS_CONTAINER_KEYS,
            "[this,is,a,seq,key]: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :key\n"
            "-SEQ\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("this"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block2_0,
            HAS_CONTAINER_KEYS,
            "{this: is, a: keymap}: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block2_1,
            HAS_CONTAINER_KEYS,
            "{this: is, a: keymap}: [and,now,a,seq,val]"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block3_0,
            HAS_CONTAINER_KEYS,
            "---\n"
            "{a: map}: [a,seq]\n"
            "---\n"
            "[A,SEQ]: {A: MAP}\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(ContainerKey1Block3_1,
            HAS_CONTAINER_KEYS,
            "---\n"
            "{a: map}: [a,seq]\n"
            "---\n"
            "[A,SEQ]: {A: MAP}\n"
            ,
            "+STR\n"
            "+DOC ---\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :A\n"
            "=VAL :SEQ\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :A\n"
            "=VAL :MAP\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map_flow(singleline));
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("A"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("SEQ"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.actually_val_is_first_key_of_new_map_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("A"));
    ___(ps.set_val_scalar_plain("MAP"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

// the examples above have the starting '[' / '{' at the beginning,
// where it is parsed in UNK state. This one has those tokens already
// in RMAP|RBLCK|RKEY state, ie, they don't come first.
ENGINE_TEST(ContainerKey2Block_1,
            HAS_CONTAINER_KEYS,
            "\n"
            "foo: bar\n"
            "!maptag &mapanchor {this: is, a: keymap}: [and,now,a,seq,val]\n"
            "!seqtag &seqanchor [now, reversed]: {of: course}\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "+MAP {} &mapanchor <!maptag>\n"
            "=VAL :this\n"
            "=VAL :is\n"
            "=VAL :a\n"
            "=VAL :keymap\n"
            "-MAP\n"
            "+SEQ []\n"
            "=VAL :and\n"
            "=VAL :now\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "=VAL :val\n"
            "-SEQ\n"
            "+SEQ [] &seqanchor <!seqtag>\n"
            "=VAL :now\n"
            "=VAL :reversed\n"
            "-SEQ\n"
            "+MAP {}\n"
            "=VAL :of\n"
            "=VAL :course\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!maptag"));
    ___(ps.set_key_anchor("mapanchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("this"));
    ___(ps.set_val_scalar_plain("is"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("keymap"));
    ___(ps.end_map_flow(singleline));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("and"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!seqtag"));
    ___(ps.set_key_anchor("seqanchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("now"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("reversed"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("of"));
    ___(ps.set_val_scalar_plain("course"));
    ___(ps.end_map_flow(singleline));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(ContainerKey3Block4_C2SP_0, HAS_CONTAINER_KEYS,
            "[21]: 42\n"
            "[23]: 42\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :21\n"
            "-SEQ\n"
            "=VAL :42\n"
            "+SEQ []\n"
            "=VAL :23\n"
            "-SEQ\n"
            "=VAL :42\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("21"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.add_sibling());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("23"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_0_scalar_plain, HAS_CONTAINER_KEYS,
                    Location(2,8),
                    "multiline\n"
                    " scalar: 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_0_scalar_squo, HAS_CONTAINER_KEYS,
                    Location(2,11),
                    "'multiline\n"
                    " scalar': 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_0_scalar_dquo, HAS_CONTAINER_KEYS,
                    Location(2,11),
                    "\"multiline\n"
                    " scalar\": 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_0, HAS_CONTAINER_KEYS,
                    Location(2,4),
                    "[23\n"
                    "]: 42\n"
                    "")

ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_1_scalar_plain, HAS_CONTAINER_KEYS,
                    Location(4,8),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "multiline\n"
                    " scalar: 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_1_scalar_squo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "'multiline\n"
                    " scalar': 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_1_scalar_dquo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "\"multiline\n"
                    " scalar\": 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_1, HAS_CONTAINER_KEYS,
                    Location(3,2),
                    "[21]: 42\n"
                    "[23\n"
                    "]: 42\n"
                    "")

ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_2_scalar_plain, HAS_CONTAINER_KEYS,
                    Location(4,8),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "multiline\n"
                    " scalar: 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_2_scalar_squo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "'multiline\n"
                    " scalar': 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_2_scalar_dquo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "\"multiline\n"
                    " scalar\": 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block4_C2SP_2, HAS_CONTAINER_KEYS,
                    Location(4,2),
                    "[21]: 42\n"
                    "[22]: 42\n"
                    "[23\n"
                    "]: 42\n"
                    "")

ENGINE_TEST(ContainerKey3Block5_C2SP_0, HAS_CONTAINER_KEYS,
            "{a: b}: 42\n"
            "{c: d}: 42\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :42\n"
            "+MAP {}\n"
            "=VAL :c\n"
            "=VAL :d\n"
            "-MAP\n"
            "=VAL :42\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.add_sibling());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("c"));
    ___(ps.set_val_scalar_plain("d"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("42"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_0, HAS_CONTAINER_KEYS,
                    Location(2,4),
                    "{c: d\n"
                    "}: 42\n"
                    "")

ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_1_scalar_plain, HAS_CONTAINER_KEYS,
                    Location(3,8),
                    "{a: b}: 42\n"
                    "multiline\n"
                    " scalar: 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_1_scalar_squo, HAS_CONTAINER_KEYS,
                    Location(3,1),
                    "{a: b}: 42\n"
                    "'multiline\n"
                    " scalar': 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_1_scalar_dquo, HAS_CONTAINER_KEYS,
                    Location(3,1),
                    "{a: b}: 42\n"
                    "\"multiline\n"
                    " scalar\": 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_1, HAS_CONTAINER_KEYS,
                    Location(3,2),
                    "{a: b}: 42\n"
                    "{c: d\n"
                    "}: 42\n"
                    "")

ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_2_scalar_plain, HAS_CONTAINER_KEYS,
                    Location(4,8),
                    "{a: b}: 42\n"
                    "{c: d}: 42\n"
                    "multiline\n"
                    " scalar: 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_2_scalar_squo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "{a: b}: 42\n"
                    "{c: d}: 42\n"
                    "'multiline\n"
                    " scalar': 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_2_scalar_dquo, HAS_CONTAINER_KEYS,
                    Location(4,1),
                    "{a: b}: 42\n"
                    "{c: d}: 42\n"
                    "\"multiline\n"
                    " scalar\": 42\n"
                    "")
ENGINE_TEST_ERRLOC_(ContainerKey3Block5_C2SP_2, HAS_CONTAINER_KEYS,
                    Location(4,2),
                    "{a: b}: 42\n"
                    "{c: d}: 42\n"
                    "{e: f\n"
                    "}: 42\n"
                    "")


//-----------------------------------------------------------------------------

ENGINE_TEST(MapKeyBlock,
            HAS_CONTAINER_KEYS, Location(7,1,8),
            "? foo: bar\n"
            ": baz"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlockFlow,
            HAS_CONTAINER_KEYS, Location(2,1,3),
            "? {foo: bar}\n"
            ": baz"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_flow(singleline));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock,
            HAS_CONTAINER_KEYS, Location(3,2,2),
            "?\n"
            " - foo\n"
            " - bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlockFlow,
            HAS_CONTAINER_KEYS, Location(2,1,3),
            "? [foo, bar]\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ []\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq_flow(singleline));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqKeyBlock2_0,
            HAS_CONTAINER_KEYS, Location(2,2,1),
            "?\n"
            "- foo\n"
            "- bar\n"
            ":\n"
            "- baz\n"
            "- bat\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(SeqKeyBlock2_1, Location(5,2,4),
            "?\n"
            "foo\n"
            ":\n"
            "a\n")
ENGINE_TEST_ERRLOC(SeqKeyBlock2_1_1, Location(7,2,6),
            "?\n"
            "'foo'\n"
            ":\n"
            "a\n")

ENGINE_TEST(SeqKeyBlock3,
            HAS_CONTAINER_KEYS, Location(3,2,2),
            "?\n"
            " - foo\n"
            " - bar\n"
            ":\n"
            " - baz\n"
            " - bat\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+SEQ\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-SEQ\n"
            "+SEQ\n"
            "=VAL :baz\n"
            "=VAL :bat\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_seq_block());
    ___(ps.begin_seq_key_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("bat"));
    ___(ps.end_seq_block());
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo0,
            HAS_CONTAINER_KEYS,
            "\n"
            "? 'foo' : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Squo1,
            HAS_CONTAINER_KEYS,
            "&blockanchor\n"
            "? &mapkey\n"
            "  &scalarkey 'foo' : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey 'foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_squoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo0,
            HAS_CONTAINER_KEYS,
            "\n"
            "? \"foo\" : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Dquo1,
            HAS_CONTAINER_KEYS,
            "&blockanchor\n"
            "? &mapkey\n"
            "  &scalarkey \"foo\" : bar\n"
            ": baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP &blockanchor\n"
            "+MAP &mapkey\n"
            "=VAL &scalarkey \"foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("blockanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.begin_map_key_block());
    ___(ps.set_key_anchor("scalarkey"));
    ___(ps.set_key_scalar_dquoted("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.end_map_block());
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(MapKeyBlock4Ref0,
            "\n"
            "? *ref\n"
            ": baz\n",
            "*ref : baz\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=ALI *ref\n"
            "=VAL :baz\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("baz"));
    ___(ps.end_map_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------


ENGINE_TEST_ERRLOC(ErrSameLine0_0_0, Location(1, 9), "map: a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_0_1, Location(1, 12), "map: &a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_0_2, Location(1, 12), "map: !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_0_3, Location(1, 15), "map: &a !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_0_4, Location(1, 15), "map: !a &a a: b\n")

ENGINE_TEST_ERRLOC(ErrSameLine0_1_0, Location(1, 6), "map: - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_1_1, Location(1, 9), "map: &a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_1_2, Location(1, 9), "map: !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_1_3, Location(1, 12), "map: &a !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_1_4, Location(1, 12), "map: !a &a - a\n")

ENGINE_TEST_ERRLOC(ErrSameLine0_2_0, Location(1, 6), "map: ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_2_1, Location(1, 9), "map: &a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_2_2, Location(1, 9), "map: !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_2_3, Location(1, 12), "map: &a !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine0_2_4, Location(1, 12), "map: !a &a ? a\n")


ENGINE_TEST_ERRLOC(ErrSameLine1_0_0, Location(2, 11), "nested:\n  map: a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_0_1, Location(2, 14), "nested:\n  map: &a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_0_2, Location(2, 14), "nested:\n  map: !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_0_3, Location(2, 17), "nested:\n  map: &a !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_0_4, Location(2, 17), "nested:\n  map: !a &a a: b\n")

ENGINE_TEST_ERRLOC(ErrSameLine1_1_0, Location(2, 8), "nested:\n  map: - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_1_1, Location(2, 11), "nested:\n  map: &a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_1_2, Location(2, 11), "nested:\n  map: !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_1_3, Location(2, 14), "nested:\n  map: &a !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_1_4, Location(2, 14), "nested:\n  map: !a &a - a\n")

ENGINE_TEST_ERRLOC(ErrSameLine1_2_0, Location(2, 8), "nested:\n  map: ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_2_1, Location(2, 11), "nested:\n  map: &a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_2_2, Location(2, 11), "nested:\n  map: !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_2_3, Location(2, 14), "nested:\n  map: &a !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine1_2_4, Location(2, 14), "nested:\n  map: !a &a ? a\n")


ENGINE_TEST_ERRLOC(ErrSameLine2_0_0, Location(1, 11), "- map: a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_0_1, Location(1, 14), "- map: &a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_0_2, Location(1, 14), "- map: !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_0_3, Location(1, 17), "- map: &a !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_0_4, Location(1, 17), "- map: !a &a a: b\n")

ENGINE_TEST_ERRLOC(ErrSameLine2_1_0, Location(1, 8), "- map: - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_1_1, Location(1, 11), "- map: &a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_1_2, Location(1, 11), "- map: !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_1_3, Location(1, 14), "- map: &a !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_1_4, Location(1, 14), "- map: !a &a - a\n")

ENGINE_TEST_ERRLOC(ErrSameLine2_2_0, Location(1, 8), "- map: ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_2_1, Location(1, 11), "- map: &a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_2_2, Location(1, 11), "- map: !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_2_3, Location(1, 14), "- map: &a !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine2_2_4, Location(1, 14), "- map: !a &a ? a\n")


ENGINE_TEST_ERRLOC(ErrSameLine3_0_0, Location(1, 8), "--- a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_0_1, Location(1, 11), "--- &a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_0_2, Location(1, 11), "--- !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_0_3, Location(1, 14), "--- &a !a a: b\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_0_4, Location(1, 14), "--- !a &a a: b\n")

ENGINE_TEST_ERRLOC(ErrSameLine3_1_0, Location(1, 5), "--- - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_1_1, Location(1, 8), "--- &a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_1_2, Location(1, 8), "--- !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_1_3, Location(1, 11), "--- &a !a - a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_1_4, Location(1, 11), "--- !a &a - a\n")

ENGINE_TEST_ERRLOC(ErrSameLine3_2_0, Location(1, 5), "--- ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_2_1, Location(1, 8), "--- &a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_2_2, Location(1, 8), "--- !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_2_3, Location(1, 11), "--- &a !a ? a\n")
ENGINE_TEST_ERRLOC(ErrSameLine3_2_4, Location(1, 11), "--- !a &a ? a\n")


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
