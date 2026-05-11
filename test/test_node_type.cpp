#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_case.hpp"

#include <gtest/gtest.h>
#include "./test_lib/callbacks_tester.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(NodeType, type_str_preset)
{
    // avoid coverage misses
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL).type_str()), "KEYVAL");
    EXPECT_EQ(to_csubstr(NodeType(KEY).type_str()), "KEY");
    EXPECT_EQ(to_csubstr(NodeType(VAL).type_str()), "VAL");
    EXPECT_EQ(to_csubstr(NodeType(MAP).type_str()), "MAP");
    EXPECT_EQ(to_csubstr(NodeType(SEQ).type_str()), "SEQ");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP).type_str()), "KEYMAP");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ).type_str()), "KEYSEQ");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ).type_str()), "DOCSEQ");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP).type_str()), "DOCMAP");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL).type_str()), "DOCVAL");
    EXPECT_EQ(to_csubstr(NodeType(DOC).type_str()), "DOC");
    EXPECT_EQ(to_csubstr(NodeType(STREAM).type_str()), "STREAM");
    EXPECT_EQ(to_csubstr(NodeType(NOTYPE).type_str()), "NOTYPE");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYNIL).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|VALNIL).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|VALREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|VALANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYREF|VALANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYANCH|VALREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|VALREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|VALANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYREF|VALANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYANCH|VALREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|VALREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|VALANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYREF|VALANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYANCH|VALREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ|VALANCH).type_str()), "DOCSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ|VALREF).type_str()), "DOCSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP|VALANCH).type_str()), "DOCMAP***");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP|VALREF).type_str()), "DOCMAP***");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL|VALANCH).type_str()), "DOCVAL***");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL|VALREF).type_str()), "DOCVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEY|KEYREF).type_str()), "KEY***");
    EXPECT_EQ(to_csubstr(NodeType(KEY|KEYANCH).type_str()), "KEY***");
    EXPECT_EQ(to_csubstr(NodeType(VAL|VALREF).type_str()), "VAL***");
    EXPECT_EQ(to_csubstr(NodeType(VAL|VALANCH).type_str()), "VAL***");
    EXPECT_EQ(to_csubstr(NodeType(MAP|VALREF).type_str()), "MAP***");
    EXPECT_EQ(to_csubstr(NodeType(MAP|VALANCH).type_str()), "MAP***");
    EXPECT_EQ(to_csubstr(NodeType(SEQ|VALREF).type_str()), "SEQ***");
    EXPECT_EQ(to_csubstr(NodeType(SEQ|VALANCH).type_str()), "SEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOC|VALREF).type_str()), "DOC***");
    EXPECT_EQ(to_csubstr(NodeType(DOC|VALANCH).type_str()), "DOC***");
    EXPECT_EQ(to_csubstr(NodeType(KEYREF).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(VALREF).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(KEYANCH).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(VALANCH).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(NOTYPE).type_str()), "NOTYPE");
}

TEST(NodeType, type_str)
{
    {
        char bufsmall[2];
        EXPECT_EQ(NodeType().type_str_sub(bufsmall).len, 6); // NOTYPE
        EXPECT_EQ(NodeType().type_str_sub(bufsmall).str, nullptr); // NOTYPE
        EXPECT_EQ(NodeType(VAL).type_str_sub(bufsmall).len, 3);
        EXPECT_EQ(NodeType(VAL).type_str_sub(bufsmall).str, nullptr);
        EXPECT_EQ(NodeType(KEYVAL).type_str_sub(bufsmall).len, 7);
        EXPECT_EQ(NodeType(KEYVAL|KEYANCH|VALANCH).type_str_sub(bufsmall).len, 19);
        EXPECT_EQ(NodeType(KEYVAL|KEYANCH|VALANCH).type_str_sub(bufsmall).str, nullptr);
    }
#define teststr(bits, str)                                        \
    {                                                             \
        char buf[128] = {0};                                      \
        memset(buf, 1, sizeof(buf));                              \
        csubstr expected = str;                                   \
        csubstr actual = NodeType(bits).type_str_sub(buf);        \
        ASSERT_LT(actual.len, C4_COUNTOF(buf));                   \
        EXPECT_EQ(actual, expected);                              \
        EXPECT_EQ(NodeType(bits).type_str_sub(buf), expected);    \
    }
    teststr(0, "NOTYPE")
    teststr(NOTYPE, "NOTYPE")
    teststr(STREAM, "STREAM")
    teststr(DOC, "DOC")
    teststr(KEY, "KEY")
    teststr(KEYNIL, "KNIL")
    teststr(KEYTAG, "KTAG")
    teststr(KEYANCH, "KANCH")
    teststr(KEYREF, "KREF")
    teststr(KEY_LITERAL, "KLITERAL")
    teststr(KEY_FOLDED, "KFOLDED")
    teststr(KEY_SQUO, "KSQUO")
    teststr(KEY_DQUO, "KDQUO")
    teststr(KEY_PLAIN, "KPLAIN")
    teststr(KEY_UNFILT, "KUNFILT")
    teststr(VAL, "VAL")
    teststr(VALNIL, "VNIL")
    teststr(VALTAG, "VTAG")
    teststr(VALANCH, "VANCH")
    teststr(VALREF, "VREF")
    teststr(VAL_LITERAL, "VLITERAL")
    teststr(VAL_FOLDED, "VFOLDED")
    teststr(VAL_SQUO, "VSQUO")
    teststr(VAL_DQUO, "VDQUO")
    teststr(VAL_PLAIN, "VPLAIN")
    teststr(VAL_UNFILT, "VUNFILT")
    teststr(MAP, "MAP")
    teststr(SEQ, "SEQ")
    teststr(FLOW_SL, "FLOWSL")
    teststr(FLOW_ML, "FLOWML")
    teststr(BLOCK, "BLCK")
    teststr(KEYVAL, "KEY|VAL")
    teststr(KEYMAP, "KEY|MAP")
    teststr(KEYVAL|KEYANCH|VALANCH, "KEY|KANCH|VAL|VANCH")
#undef teststr
}

TEST(NodeType, scalar_style_choose_json)
{
    EXPECT_EQ(scalar_style_json_choose("true"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("false"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("null"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("-0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("+0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose(".1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("+.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("-.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_json_choose("01"), SCALAR_DQUO);
    EXPECT_EQ(scalar_style_json_choose("foo"), SCALAR_DQUO);
    EXPECT_EQ(scalar_style_json_choose("bar"), SCALAR_DQUO);
}

TEST(NodeType, scalar_style_choose)
{
    EXPECT_EQ(scalar_style_choose(" \n\t"), SCALAR_DQUO);
    EXPECT_EQ(scalar_style_choose("-.inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-.INF"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-.034"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-.034x"), SCALAR_SQUO);
    EXPECT_EQ(scalar_style_choose("2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+2.35e-10"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0.1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0x1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0o1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0b1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0B1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("0B1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0B1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("-0B1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+01"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0B1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0X1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0O1"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose("+0B1"), SCALAR_PLAIN);
}

TEST(NodeType, scalar_style_query_plain)
{
    EXPECT_TRUE(scalar_style_query_plain("-.inf"));
    EXPECT_TRUE(scalar_style_query_plain("-.INF"));
    EXPECT_TRUE(scalar_style_query_plain("-.034"));
    EXPECT_FALSE(scalar_style_query_plain("-.034x"));
    EXPECT_TRUE(scalar_style_query_plain("2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain("-2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain("+2.35e-10"));
    EXPECT_TRUE(scalar_style_query_plain("0.1"));
    EXPECT_TRUE(scalar_style_query_plain("-0.1"));
    EXPECT_TRUE(scalar_style_query_plain("+0.1"));
    EXPECT_TRUE(scalar_style_query_plain("01"));
    EXPECT_TRUE(scalar_style_query_plain("0x1"));
    EXPECT_TRUE(scalar_style_query_plain("0o1"));
    EXPECT_TRUE(scalar_style_query_plain("0b1"));
    EXPECT_TRUE(scalar_style_query_plain("0x1"));
    EXPECT_TRUE(scalar_style_query_plain("0o1"));
    EXPECT_TRUE(scalar_style_query_plain("0b1"));
    EXPECT_TRUE(scalar_style_query_plain("+01"));
    EXPECT_TRUE(scalar_style_query_plain("+0x1"));
    EXPECT_TRUE(scalar_style_query_plain("+0o1"));
    EXPECT_TRUE(scalar_style_query_plain("+0b1"));
    EXPECT_TRUE(scalar_style_query_plain("+0x1"));
    EXPECT_TRUE(scalar_style_query_plain("+0o1"));
    EXPECT_TRUE(scalar_style_query_plain("+0b1"));
    EXPECT_TRUE(scalar_style_query_plain("-01"));
    EXPECT_TRUE(scalar_style_query_plain("-0x1"));
    EXPECT_TRUE(scalar_style_query_plain("-0o1"));
    EXPECT_TRUE(scalar_style_query_plain("-0b1"));
    EXPECT_TRUE(scalar_style_query_plain("-0x1"));
    EXPECT_TRUE(scalar_style_query_plain("-0o1"));
    EXPECT_TRUE(scalar_style_query_plain("-0b1"));
    EXPECT_TRUE(scalar_style_query_plain("0X1"));
    EXPECT_TRUE(scalar_style_query_plain("0O1"));
    EXPECT_TRUE(scalar_style_query_plain("0B1"));
    EXPECT_TRUE(scalar_style_query_plain("0X1"));
    EXPECT_TRUE(scalar_style_query_plain("0O1"));
    EXPECT_TRUE(scalar_style_query_plain("0B1"));
    EXPECT_TRUE(scalar_style_query_plain("+01"));
    EXPECT_TRUE(scalar_style_query_plain("+0X1"));
    EXPECT_TRUE(scalar_style_query_plain("+0O1"));
    EXPECT_TRUE(scalar_style_query_plain("+0B1"));
    EXPECT_TRUE(scalar_style_query_plain("+0X1"));
    EXPECT_TRUE(scalar_style_query_plain("+0O1"));
    EXPECT_TRUE(scalar_style_query_plain("+0B1"));
    EXPECT_TRUE(scalar_style_query_plain("-01"));
    EXPECT_TRUE(scalar_style_query_plain("-0X1"));
    EXPECT_TRUE(scalar_style_query_plain("-0O1"));
    EXPECT_TRUE(scalar_style_query_plain("-0B1"));
    EXPECT_TRUE(scalar_style_query_plain("-0X1"));
    EXPECT_TRUE(scalar_style_query_plain("-0O1"));
    EXPECT_TRUE(scalar_style_query_plain("-0B1"));
}


TEST(NodeType, is_stream)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_stream());
    EXPECT_TRUE(NodeType(STREAM).is_stream());
}

TEST(NodeType, is_doc)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_doc());
    EXPECT_TRUE(NodeType(DOC).is_doc());
}

TEST(NodeType, is_container)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_container());
    EXPECT_FALSE(NodeType(VAL).is_container());
    EXPECT_FALSE(NodeType(KEY).is_container());
    EXPECT_FALSE(NodeType(KEYVAL).is_container());
    EXPECT_TRUE(NodeType(MAP).is_container());
    EXPECT_TRUE(NodeType(SEQ).is_container());
    EXPECT_TRUE(NodeType(KEYMAP).is_container());
    EXPECT_TRUE(NodeType(KEYSEQ).is_container());
    EXPECT_TRUE(NodeType(DOCMAP).is_container());
    EXPECT_TRUE(NodeType(DOCSEQ).is_container());
}

TEST(NodeType, is_map)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_map());
    EXPECT_FALSE(NodeType(VAL).is_map());
    EXPECT_FALSE(NodeType(KEY).is_map());
    EXPECT_TRUE(NodeType(MAP).is_map());
    EXPECT_TRUE(NodeType(KEYMAP).is_map());
    EXPECT_FALSE(NodeType(SEQ).is_map());
    EXPECT_FALSE(NodeType(KEYSEQ).is_map());
}

TEST(NodeType, is_seq)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_seq());
    EXPECT_FALSE(NodeType(VAL).is_seq());
    EXPECT_FALSE(NodeType(KEY).is_seq());
    EXPECT_FALSE(NodeType(MAP).is_seq());
    EXPECT_FALSE(NodeType(KEYMAP).is_seq());
    EXPECT_TRUE(NodeType(SEQ).is_seq());
    EXPECT_TRUE(NodeType(KEYSEQ).is_seq());
}

TEST(NodeType, has_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_val());
    EXPECT_FALSE(NodeType(KEY).has_val());
    EXPECT_TRUE(NodeType(VAL).has_val());
    EXPECT_TRUE(NodeType(DOCVAL).has_val());
    EXPECT_TRUE(NodeType(KEYVAL).has_val());
    EXPECT_FALSE(NodeType(KEYMAP).has_val());
    EXPECT_FALSE(NodeType(KEYSEQ).has_val());
}

TEST(NodeType, is_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_val());
    EXPECT_FALSE(NodeType(KEY).is_val());
    EXPECT_TRUE(NodeType(VAL).is_val());
    EXPECT_TRUE(NodeType(DOCVAL).is_val());
    EXPECT_FALSE(NodeType(KEYVAL).is_val());
    EXPECT_FALSE(NodeType(KEYMAP).is_val());
    EXPECT_FALSE(NodeType(KEYSEQ).is_val());
}

TEST(NodeType, has_null_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).val_is_null());
    EXPECT_FALSE(NodeType(VAL).val_is_null());
    EXPECT_FALSE(NodeType(KEY|KEYNIL).val_is_null());
    EXPECT_TRUE(NodeType(VAL|VALNIL).val_is_null());
    EXPECT_FALSE(NodeType(KEYVAL).val_is_null());
    EXPECT_FALSE(NodeType(KEYMAP).val_is_null());
    EXPECT_FALSE(NodeType(KEYSEQ).val_is_null());
    EXPECT_TRUE(NodeType(KEYVAL|VALNIL).val_is_null());
    EXPECT_TRUE(NodeType(KEYMAP|VALNIL).val_is_null());
    EXPECT_TRUE(NodeType(KEYSEQ|VALNIL).val_is_null());
}

TEST(NodeType, has_key)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_key());
    EXPECT_TRUE(NodeType(KEY).has_key());
    EXPECT_FALSE(NodeType(VAL).has_key());
    EXPECT_TRUE(NodeType(KEYVAL).has_key());
    EXPECT_TRUE(NodeType(KEYMAP).has_key());
    EXPECT_TRUE(NodeType(KEYSEQ).has_key());
}

TEST(NodeType, key_is_null)
{
    EXPECT_FALSE(NodeType(NOTYPE).key_is_null());
    EXPECT_FALSE(NodeType(KEY).key_is_null());
    EXPECT_TRUE(NodeType(KEY|KEYNIL).key_is_null());
    EXPECT_FALSE(NodeType(VAL|VALNIL).key_is_null());
    EXPECT_FALSE(NodeType(KEYVAL).key_is_null());
    EXPECT_FALSE(NodeType(KEYMAP).key_is_null());
    EXPECT_FALSE(NodeType(KEYSEQ).key_is_null());
    EXPECT_TRUE(NodeType(KEYVAL|KEYNIL).key_is_null());
    EXPECT_TRUE(NodeType(KEYMAP|KEYNIL).key_is_null());
    EXPECT_TRUE(NodeType(KEYSEQ|KEYNIL).key_is_null());
}

TEST(NodeType, is_keyval)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_keyval());
    EXPECT_FALSE(NodeType(KEY).is_keyval());
    EXPECT_FALSE(NodeType(VAL).is_keyval());
    EXPECT_TRUE(NodeType(KEYVAL).is_keyval());
    EXPECT_FALSE(NodeType(DOCVAL).is_keyval());
    EXPECT_FALSE(NodeType(KEYMAP).is_keyval());
    EXPECT_FALSE(NodeType(KEYSEQ).is_keyval());
}

TEST(NodeType, has_key_tag)
{
    EXPECT_FALSE(NodeType().has_key_tag());
    EXPECT_TRUE(NodeType(KEYTAG).has_key_tag());
    EXPECT_TRUE(NodeType(KEY|KEYTAG).has_key_tag());
}

TEST(NodeType, has_val_tag)
{
    EXPECT_FALSE(NodeType().has_val_tag());
    EXPECT_TRUE(NodeType(VALTAG).has_val_tag());
    EXPECT_TRUE(NodeType(VAL|VALTAG).has_val_tag());
}

TEST(NodeType, has_key_anchor)
{
    EXPECT_FALSE(NodeType().has_key_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_key_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_key_anchor());
}

TEST(NodeType, has_val_anchor)
{
    EXPECT_FALSE(NodeType().has_val_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_val_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_val_anchor());
}

TEST(NodeType, has_anchor)
{
    EXPECT_FALSE(NodeType().has_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_anchor());
}

TEST(NodeType, is_key_ref)
{
    EXPECT_FALSE(NodeType().is_key_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_key_ref());
    EXPECT_TRUE(NodeType(KEY|KEYREF).is_key_ref());
}

TEST(NodeType, is_val_ref)
{
    EXPECT_FALSE(NodeType().is_val_ref());
    EXPECT_TRUE(NodeType(VALREF).is_val_ref());
    EXPECT_TRUE(NodeType(VAL|VALREF).is_val_ref());
}

TEST(NodeType, is_ref)
{
    EXPECT_FALSE(NodeType().is_ref());
    EXPECT_FALSE(NodeType(KEYVAL).is_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_ref());
    EXPECT_TRUE(NodeType(VALREF).is_ref());
    EXPECT_TRUE(NodeType(KEY|VALREF).is_ref());
    EXPECT_TRUE(NodeType(VAL|KEYREF).is_ref());
    EXPECT_TRUE(NodeType(KEYREF|VALREF).is_ref());
}

TEST(NodeType, is_key_quoted)
{
    EXPECT_FALSE(NodeType().is_key_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_key_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_key_quoted());
}

TEST(NodeType, is_val_quoted)
{
    EXPECT_FALSE(NodeType().is_val_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_val_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_val_quoted());
}

TEST(NodeType, is_quoted)
{
    EXPECT_FALSE(NodeType().is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|KEYQUO).is_quoted());
}


//-----------------------------------------------------------------------------

namespace {
struct scalar_style_spec
{
    csubstr scalar;
    NodeType style_flow;
    NodeType style_block;
    NodeType style_json;
    int elm;
    int line;
    int entry;
};
csubstr mknullstr() { csubstr s; s.str = nullptr; s.len = 0; return s; }
const csubstr zerolen("", size_t(0));

const NodeType P = SCALAR_PLAIN;
const NodeType S = SCALAR_SQUO;
const NodeType D = SCALAR_DQUO;
int elmcount = 0;
int entrycount = 0;
const scalar_style_spec scalars[] = {
    #define _(s, flow, block, json) scalar_style_spec{csubstr(s), flow, block, json, elmcount++, __LINE__, entrycount++}
    #define __(s, flow, block, json) scalar_style_spec{csubstr(s), flow, block, json, elmcount++, __LINE__, (entrycount = 0, entrycount++)}
    #define ___(s, flow, block, json) scalar_style_spec{s, flow, block, json, elmcount++, __LINE__, entrycount++}
    // special
    ___(mknullstr(), P, P, D),
    ___(zerolen, S, S, D),
    // regular scalar
    __("foo", P, P, D),
    __("foo", P, P, D),
    __("bar", P, P, D),
    __("foo space", P, P, D),
    __("bar space", P, P, D),
    // whitespace
    __(" ", S, S, D),
    __("\t", S, S, D),
    __("\n", S, S, D),
    // numbers
    __("0", P, P, P), _("00", P, P, D), _("000", P, P, D),
    __("1", P, P, P), _("01", P, P, D), _("001", P, P, D),
    __("10", P, P, P), _("010", P, P, D), _("0010", P, P, D),
    __("0.1", P, P, P), _("-0.1", P, P, P), _("+0.1", P, P, P),
    __(".1", P, P, P), _("-.1", P, P, P), _("+.1", P, P, P),
    __("01", P, P, D),  _("0x1", P, P, D), _("0o1", P, P, D), _("0b1", P, P, D), _("0x1", P, P, D), _("0o1", P, P, D), _("0b1", P, P, D),
    __("01", P, P, D),  _("0X1", P, P, D), _("0O1", P, P, D), _("0B1", P, P, D), _("0X1", P, P, D), _("0O1", P, P, D), _("0B1", P, P, D),
    __("-01", P, P, P), _("-0x1", P, P, P), _("-0o1", P, P, P), _("-0b1", P, P, P), _("-0x1", P, P, P), _("-0o1", P, P, P), _("-0b1", P, P, P),
    __("-01", P, P, P), _("-0X1", P, P, P), _("-0O1", P, P, P), _("-0B1", P, P, P), _("-0X1", P, P, P), _("-0O1", P, P, P), _("-0B1", P, P, P),
    __("+01", P, P, P), _("+0x1", P, P, P), _("+0o1", P, P, P), _("+0b1", P, P, P), _("+0x1", P, P, P), _("+0o1", P, P, P), _("+0b1", P, P, P),
    __("+01", P, P, P), _("+0X1", P, P, P), _("+0O1", P, P, P), _("+0B1", P, P, P), _("+0X1", P, P, P), _("+0O1", P, P, P), _("+0B1", P, P, P),
    __("2.35e-10", P, P, P),
    __("-2.35e-10", P, P, P),
    __("+2.35e-10", P, P, P),
    __("0.034", P, P, P),
    __("0.034", P, P, P),
    __("-0.034", P, P, P),
    __("-0.034", P, P, P),
    __("+0.034", P, P, P),
    __("+0.034", P, P, P),
    __(".034x", P, P, D),
    __("-.0123", P, P, P),
    __("+.0123", P, P, P),
    __("-.034x", P, P, D),
    __("+.034x", P, P, D),
    // inf
    __(".inf", P, P, P), _(".INF", P, P, P), _(".Inf", P, P, P),
    __("-.inf", P, P, P), _("-.INF", P, P, P), _("-.Inf", P, P, P),
    __("+.inf", P, P, P), _("+.INF", P, P, P), _("+.Inf", P, P, P),
    __(".infnot", P, P, D), _(".INFnot", P, P, D), _(".Infnot", P, P, D),
    __("-.infnot", P, P, D), _("-.INFnot", P, P, D), _("-.Infnot", P, P, D),
    __("+.infnot", P, P, D), _("+.INFnot", P, P, D), _("+.Infnot", P, P, D),
    // nan
    __(".nan", P, P, P), _(".NAN", P, P, P), _(".Nan", P, P, P), _(".NaN", P, P, P),
    __("-.nan", P, P, D), _("-.NAN", P, P, D), _("-.Nan", P, P, D), _("-.NaN", P, P, D),
    __("+.nan", P, P, D), _("+.NAN", P, P, D), _("+.Nan", P, P, D), _("+.NaN", P, P, D),
    __(".nannot", P, P, D), _(".NANnot", P, P, D), _(".Nannot", P, P, D),
    // bool
    __("true", P, P, P),
    __("false", P, P, P),
    __("True", P, P, D),
    __("False", P, P, D),
    // dubious
    __("+.notinf", P, P, D), _("+.:", S, S, D),
    __("-.notinf", P, P, D), _("-:", S, S, D),
    __(":", S, S, D), _(": ", S, S, D), _(":\t", S, S, D),
    __("-", S, S, D), _("- ", S, S, D), _("-\t", S, S, D),
    __("?", S, S, D), _("? ", S, S, D), _("?\t", S, S, D),
    __(":a", P, P, D), _(":ab", P, P, D), _(":a b", P, P, D), _(":a\tb", P, P, D),
    __("-a", P, P, D), _("-ab", P, P, D), _("-a b", P, P, D), _("-a\tb", P, P, D),
    __("?a", P, P, D), _("?ab", P, P, D), _("?a b", P, P, D), _("?a\tb", P, P, D),
    __(":a,", S, P, D), _(":ab,", S, P, D), _(":a\tb,", S, P, D),
    __("-a,", S, P, D), _("-ab,", S, P, D), _("-a\tb,", S, P, D),
    __("?a,", S, P, D), _("?ab,", S, P, D), _("?a\tb,", S, P, D),
    __("b[c]d", S, P, D), _("b{c}d", S, P, D), _("b,c,d", S, P, D),
    __("(0,1)", S, P, D), _("-0,1", S, P, D),
    __("::", S, S, D), _(":: ", S, S, D), _("::\t", S, S, D),
    __("--", P, P, D), _("-- ", S, S, D), _("--\t", S, S, D),
    __("??", P, P, D), _("?? ", S, S, D), _("??\t", S, S, D),
    // tags
    __("!!str", S, S, D),
    __("!str", S, S, D),
    __("!<!foo>", S, S, D),
    __("<!foo>", P, P, D),
    __("<!foo> scalar", P, P, D),
    // anchors
    __("&notanchor", S, S, D),
    // invalid characters at beginning
    __("%invalidplain", S, S, D), _("but%validplain", P, P, D),
    __("`invalidplain", S, S, D), _("but`validplain", P, P, D),
    __("#invalidplain", S, S, D), _("but#validplain", P, P, D),
    __(",invalidplain", S, S, D), _("but,validplain", S, P, D),
    __("- invalidplain", S, S, D), _("but- validplain", P, P, D),
    __(": invalidplain", S, S, D), _("also: invalidplain", S, S, D),
    __("? invalidplain", S, S, D), _("but? invalidplain", P, P, D),
    __("# invalidplain", S, S, D), _("also# invalidplain", S, S, D),
};
std::string namefor(scalar_style_spec const& param)
{
    return c4::formatrs<std::string>("elm{}__line{}__entry{}",
                                     fmt::zpad(param.elm, 3),
                                     fmt::zpad(param.line, 3),
                                     param.entry);
}
template<class T>
std::string namefor(testing::TestParamInfo<T> const& pinfo)
{
    return namefor(pinfo.param);
}

struct TestScalarStyle : public testing::TestWithParam<scalar_style_spec> {};

INSTANTIATE_TEST_SUITE_P(_, TestScalarStyle, testing::ValuesIn(scalars), namefor<scalar_style_spec>);

struct showtype_ { NodeType ty; };
showtype_ showtype(NodeType ty) { return {ty}; }
size_t to_chars(substr buf, showtype_ s)
{
    size_t pos = s.ty.type_str(buf);
    buf = buf.sub(pos < buf.len ? pos : buf.len);
    pos += format(buf, "({})", s.ty.type);
    return pos;
}

const csubstr plain_flow_invalid_at_beginning[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("!"), csubstr("&"), csubstr("*"), csubstr(","),
    csubstr("\""), csubstr("'"), csubstr("{"), csubstr("}"),
};
const csubstr plain_flow_invalid_in_bulk[] = {
    csubstr(": "), csubstr(":\t"),
    csubstr("# "), csubstr("#\t"),
};
const csubstr plain_flow_invalid_at_end[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr(","), csubstr("{"), csubstr("}"), csubstr("["), csubstr("]"),
    csubstr("# "), csubstr("#\t"), csubstr("#"),
    csubstr(": "), csubstr(":\t"), csubstr(":"),
};

const csubstr plain_block_invalid_at_beginning[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("!"), csubstr("&"), csubstr("*"), csubstr(","),
    csubstr("\""), csubstr("'"), csubstr("{"), csubstr("}"),
};
const csubstr plain_block_invalid_in_bulk[] = {
    csubstr(": "), csubstr(":\t"),
    csubstr("# "), csubstr("#\t"),
};
const csubstr plain_block_invalid_at_end[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("# "), csubstr("#\t"), csubstr("#"),
    csubstr(": "), csubstr(":\t"), csubstr(":"),
};

#define SHOWPARAM(p, fmt, ...)                                      \
    RYML_TRACE_FMT("test\n"                                         \
                   "{}:{}: scalar=[{}]~~~{}~~~\n"                   \
                   "{}:{}: " fmt,                                   \
                   __FILE__, (p).line, (p).scalar.len, (p).scalar,  \
                   __FILE__, (p).line, __VA_ARGS__)

} // namespace

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated-declarations")

TEST_P(TestScalarStyle, query_plain_flow)
{
    scalar_style_spec const& p = GetParam();
    SHOWPARAM(p, "expected_flow={}", showtype(p.style_flow.type));
    EXPECT_EQ(scalar_style_query_plain_flow(p.scalar), p.style_flow == P);
    EXPECT_EQ(scalar_style_query_plain(p.scalar, /*flow*/true), p.style_flow == P);
    if(p.style_flow == P)
    {
        std::string buf;
        for(csubstr invalid : plain_flow_invalid_at_beginning)
        {
            catrs(&buf, invalid, p.scalar);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_flow(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/true));
        }
        for(csubstr invalid : plain_flow_invalid_in_bulk)
        {
            catrs(&buf, p.scalar, invalid, p.scalar);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_flow(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/true));
        }
        for(csubstr invalid : plain_flow_invalid_at_end)
        {
            catrs(&buf, p.scalar, invalid);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_flow(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/true));
        }
    }
}

TEST_P(TestScalarStyle, query_plain_block)
{
    scalar_style_spec const& p = GetParam();
    SHOWPARAM(p, "expected_block={}", showtype(p.style_block.type));
    EXPECT_EQ(scalar_style_query_plain_block(p.scalar), p.style_block == P);
    EXPECT_EQ(scalar_style_query_plain(p.scalar, /*flow*/false), p.style_block == P);
    if(p.style_flow == P)
    {
        std::string buf;
        for(csubstr invalid : plain_block_invalid_at_beginning)
        {
            catrs(&buf, invalid, p.scalar);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_block(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/false));
        }
        for(csubstr invalid : plain_block_invalid_in_bulk)
        {
            catrs(&buf, p.scalar, invalid, p.scalar);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_block(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/false));
        }
        for(csubstr invalid : plain_block_invalid_at_end)
        {
            catrs(&buf, p.scalar, invalid);
            RYML_TRACE_FMT("invalid='{}' scalar='{}' buf='{}'", invalid, p.scalar, buf);
            EXPECT_FALSE(scalar_style_query_plain_block(to_csubstr(buf)));
            EXPECT_FALSE(scalar_style_query_plain(to_csubstr(buf), /*flow*/false));
        }
    }
}

TEST_P(TestScalarStyle, choose_flow)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_choose_flow(p.scalar);
    NodeType expected = p.style_flow.type;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
}

TEST_P(TestScalarStyle, choose_block)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_choose_block(p.scalar);
    NodeType expected = p.style_block.type;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
}

TEST_P(TestScalarStyle, choose_json)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_json_choose(p.scalar);
    NodeType expected = p.style_json.type;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
}


//-----------------------------------------------------------------------------

void test_scalar_roundtrip_on_seq(csubstr scalar, NodeType seq_style, NodeType expected_style)
{
    if(!scalar.str)
        return;
    Tree t;
    NodeRef r = t.rootref();
    r |= SEQ|seq_style;
    for(size_t i = 0; i < 4; ++i)
    {
        r.append_child() = scalar;
        EXPECT_FALSE(r.type().is_val_styled()); // no style is set
    }
    std::string emitted = emitrs_yaml<std::string>(t);
    const Tree parsed = parse_in_arena(to_csubstr(emitted));
    for(ConstNodeRef ch : parsed.rootref().children())
    {
        EXPECT_EQ(ch.val(), scalar);
        EXPECT_TRUE(ch.type().is_val_styled()); // style is set on the roundtrip
        NodeType actual_style = ch.type() & expected_style;
        RYML_COMPARE_NODE_TYPE(expected_style, actual_style, ==, EQ);
    }
    if(testing::Test::HasFailure())
    {
        printf("emitted=~~~\n%s\n~~~\n", emitted.c_str());
        print_tree("orig", t);
        print_tree("roundtrip", parsed);
    }
}

void test_scalar_roundtrip_on_map(csubstr scalar, NodeType map_style, NodeType expected_style_key, NodeType expected_style_val)
{
    if(!scalar.str)
        return;
    Tree t;
    NodeRef r = t.rootref();
    r |= MAP|map_style;
    NodeRef vals = r["vals"];
    NodeRef keys = r["keys"];
    vals |= MAP|map_style;
    keys |= MAP|map_style;
    csubstr names[] = {"0", "1", "2", "3"};
    for(csubstr name : names)
    {
        NodeRef k = keys.append_child();
        NodeRef v = vals.append_child();
        k.set_key(scalar);
        v.set_key(name);
        k.set_val(name);
        v.set_val(scalar);
        EXPECT_FALSE(k.type().is_key_styled()); // no style is set
        EXPECT_FALSE(v.type().is_val_styled()); // no style is set
    }
    std::string emitted = emitrs_yaml<std::string>(t);
    const Tree parsed = parse_in_arena(to_csubstr(emitted));
    for(ConstNodeRef ch : parsed["keys"].children())
    {
        EXPECT_EQ(ch.key(), scalar);
        EXPECT_TRUE(ch.type().is_key_styled()); // style is set on the roundtrip
        NodeType actual_style = ch.type() & expected_style_key;
        RYML_COMPARE_NODE_TYPE(expected_style_key, actual_style, ==, EQ);
    }
    for(ConstNodeRef ch : parsed["vals"].children())
    {
        EXPECT_EQ(ch.val(), scalar);
        EXPECT_TRUE(ch.type().is_val_styled()); // style is set on the roundtrip
        NodeType actual_style = ch.type() & expected_style_val;
        RYML_COMPARE_NODE_TYPE(expected_style_val, actual_style, ==, EQ);
    }
    if(testing::Test::HasFailure())
    {
        printf("emitted=~~~\n%s\n~~~\n", emitted.c_str());
        print_tree("orig", t);
        print_tree("roundtrip", parsed);
    }
}

TEST_P(TestScalarStyle, roundtrip_flow_seq)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_seq(p.scalar, FLOW_ML, p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_block_seq)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_seq(p.scalar, BLOCK, p.style_block & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_flow_map)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_map(p.scalar, FLOW_ML,
                                 p.style_flow & KEY_STYLE,
                                 p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_block_map)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_map(p.scalar, BLOCK,
                                 p.style_block & KEY_STYLE,
                                 p.style_block & VAL_STYLE);
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4
