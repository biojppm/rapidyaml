#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include "./test_lib/callbacks_tester.hpp"

#include <gtest/gtest.h>

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
        EXPECT_EQ(NodeType().type_str(bufsmall).len, 1 + 6); // NOTYPE
        EXPECT_EQ(NodeType().type_str(bufsmall).str, nullptr); // NOTYPE
        EXPECT_EQ(NodeType(VAL).type_str(bufsmall).len, 1 + 3);
        EXPECT_EQ(NodeType(VAL).type_str(bufsmall).str, nullptr);
        EXPECT_EQ(NodeType(KEYVAL).type_str(bufsmall).len, 1 + 7);
        EXPECT_EQ(NodeType(KEYVAL|KEYANCH|VALANCH).type_str(bufsmall).len, 1 + 19);
        EXPECT_EQ(NodeType(KEYVAL|KEYANCH|VALANCH).type_str(bufsmall).str, nullptr);
    }
#define teststr(bits, str)                                        \
    {                                                             \
        char buf[128] = {0};                                      \
        memset(buf, 1, sizeof(buf));                              \
        csubstr expected = str;                                   \
        csubstr actual = NodeType(bits).type_str(buf);            \
        ASSERT_LT(actual.len + 1, C4_COUNTOF(buf));               \
        EXPECT_EQ(actual, expected);                              \
        EXPECT_EQ(NodeType(bits).type_str(buf), expected);        \
        EXPECT_EQ(buf[expected.len], '\0');                       \
    }
    teststr(0, "NOTYPE")
    teststr(NOTYPE, "NOTYPE")
    teststr(STREAM, "STREAM")
    teststr(DOC, "DOC")
    teststr(KEY, "KEY")
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

TEST(NodeType, has_key)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_key());
    EXPECT_TRUE(NodeType(KEY).has_key());
    EXPECT_FALSE(NodeType(VAL).has_key());
    EXPECT_TRUE(NodeType(KEYVAL).has_key());
    EXPECT_TRUE(NodeType(KEYMAP).has_key());
    EXPECT_TRUE(NodeType(KEYSEQ).has_key());
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

} // namespace yml
} // namespace c4
