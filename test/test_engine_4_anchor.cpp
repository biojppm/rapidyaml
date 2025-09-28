#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorDocVal,
            ("&val a\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL &val :a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("val"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorDocRef,
            ("*a\n"),
            "+STR\n"
            "+DOC\n"
            "=ALI *a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_ref("*a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorExplDocVal,
            ("&val a\n"),
            "+STR\n"
            "+DOC\n"
            "=VAL &val :a\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("val"));
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorSeqBlock,
            ("&seq\n- &val1 val1\n- &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "=VAL &val1 :val1\n"
            "=VAL &val2 :val2\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqMapBlock,
            ("&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- &map2\n"
             "  &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "+MAP\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "+MAP &map2\n"
            "=VAL &key10 :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("map2"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key10"));
    ___(ps.set_key_scalar_plain("key10"));
    ___(ps.set_val_anchor("val10"));
    ___(ps.set_val_scalar_plain("val10"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key20"));
    ___(ps.set_key_scalar_plain("key20"));
    ___(ps.set_val_anchor("val20"));
    ___(ps.set_val_scalar_plain("val20"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlow,
            ("&seq [&val1 val1,\n&val2 val2]", "&seq [&val1 val1,&val2 val2]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=VAL &val2 :val2\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqBlockWithRef,
            ("&seq\n- &val1 val1\n- *val1\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlowWithRef,
            ("&seq [&val1 val1,*val1]", "&seq [&val1 val1,*val1]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqFlowWithRefMultiLine,
            ("&seq\n[\n&val1 val1\n,*val1\n]", "&seq [&val1 val1,*val1]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ [] &seq\n"
            "=VAL &val1 :val1\n"
            "=ALI *val1\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_val_ref("*val1"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapBlock,
            ("&map\n&key1 key1: &val1 val1\n&key2 key2: &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapBlockWithRef,
            ("&map\n&rkey1 key1: &rval1 val1\n*rkey1 : *rval1\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapFlow,
            ("&map\n{&key1 key1: &val1 val1,&key2 key2: &val2 val2}", "&map {&key1 key1: &val1 val1,&key2 key2: &val2 val2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapFlowMultiLine,
            ("&map\n{&key1 key1: &val1 val1,\n&key2 key2: &val2 val2\n}", "&map {&key1 key1: &val1 val1,&key2 key2: &val2 val2}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

// WATCHOUT: see https://play.yaml.io/main/parser?input=Jm1hcAomcmtleTEgZm9vOiAmcnZhbDEgYmFyCipya2V5MSA6ICpydmFsMQ==
ENGINE_TEST(AnchorMapFlowWithRef,
            ("&map\n{&rkey1 key1: &rval1 val1,*rkey1 : *rval1}", "&map {&rkey1 key1: &rval1 val1,*rkey1 : *rval1}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

// WATCHOUT: see https://play.yaml.io/main/parser?input=Jm1hcAomcmtleTEgZm9vOiAmcnZhbDEgYmFyCipya2V5MSA6ICpydmFsMQ==
ENGINE_TEST(AnchorMapFlowWithRefMultiLine,
            ("&map\n{\n&rkey1 key1: &rval1 val1,\n*rkey1 : *rval1\n}", "&map {&rkey1 key1: &rval1 val1,*rkey1 : *rval1}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &rkey1 :key1\n"
            "=VAL &rval1 :val1\n"
            "=ALI *rkey1\n"
            "=ALI *rval1\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("rkey1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("rval1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_ref("*rkey1"));
    ___(ps.set_val_ref("*rval1"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorMapMapBlock,
            ("&map\n&mapkey map: &mapval\n  &key1 key1: &val1 val1\n  &key2 key2: &val2 val2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &map\n"
            "=VAL &mapkey :map\n"
            "+MAP &mapval\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("mapval"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorMapMapFlow,
            ("&map\n{&mapkey map: &mapval {&key1 key1: &val1 val1,&key2 key2: &val2 val2,key3: val3},key4: val4}",
             "&map {&mapkey map: &mapval {&key1 key1: &val1 val1,&key2 key2: &val2 val2,key3: val3},key4: val4}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {} &map\n"
            "=VAL &mapkey :map\n"
            "+MAP {} &mapval\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "=VAL :key3\n"
            "=VAL :val3\n"
            "-MAP\n"
            "=VAL :key4\n"
            "=VAL :val4\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("map"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("mapkey"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.set_val_anchor("mapval"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("key1"));
    ___(ps.set_key_scalar_plain("key1"));
    ___(ps.set_val_anchor("val1"));
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("key2"));
    ___(ps.set_key_scalar_plain("key2"));
    ___(ps.set_val_anchor("val2"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("key3"));
    ___(ps.set_val_scalar_plain("val3"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("key4"));
    ___(ps.set_val_scalar_plain("val4"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}



//-----------------------------------------------------------------------------

ENGINE_TEST(AnchorTagPlacement,
            ("- &a0 !b0 foo0: bar0\n"
             "- &a1 !b1\n"
             "  foo1: bar1\n"
             "- &a2\n"
             "  !b2 foo2: bar2\n"
             "- &a3\n"
             "  !b3\n"
             "  foo3: bar3\n"
             "- &a4\n"
             "\n"
             "  !b4\n"
             "\n"
             "  foo4: bar4\n",
             "- !b0 &a0 foo0: bar0\n"
             "- !b1 &a1\n"
             "  foo1: bar1\n"
             "- &a2\n"
             "  !b2 foo2: bar2\n"
             "- !b3 &a3\n"
             "  foo3: bar3\n"
             "- !b4 &a4\n"
             "  foo4: bar4\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "=VAL &a0 <!b0> :foo0\n"
            "=VAL :bar0\n"
            "-MAP\n"
            "+MAP &a1 <!b1>\n"
            "=VAL :foo1\n"
            "=VAL :bar1\n"
            "-MAP\n"
            "+MAP &a2\n"
            "=VAL <!b2> :foo2\n"
            "=VAL :bar2\n"
            "-MAP\n"
            "+MAP &a3 <!b3>\n"
            "=VAL :foo3\n"
            "=VAL :bar3\n"
            "-MAP\n"
            "+MAP &a4 <!b4>\n"
            "=VAL :foo4\n"
            "=VAL :bar4\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("a0"));
    ___(ps.set_key_tag("!b0"));
    ___(ps.set_key_scalar_plain("foo0"));
    ___(ps.set_val_scalar_plain("bar0"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a1"));
    ___(ps.set_val_tag("!b1"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo1"));
    ___(ps.set_val_scalar_plain("bar1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a2"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_tag("!b2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a3"));
    ___(ps.set_val_tag("!b3"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo3"));
    ___(ps.set_val_scalar_plain("bar3"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_anchor("a4"));
    ___(ps.set_val_tag("!b4"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("foo4"));
    ___(ps.set_val_scalar_plain("bar4"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(AnchorMapMapSuckerPunch,
            ("!mymap &mymap\n"
             "&fooanch !footag foo: &seq\n"
             "  &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "!bartag &baranch bar: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  &key20 !b key20: &val20 !c val20\n"
             "  key10: 20\n"
             "&bazanch !baztag baz:   &map2 !a\n"
             "    &key10 !b key10: &val10 !c val10\n"
             "    !b &key20 key20: !c &val20 val20\n"
             "    key10: 20\n"
             "brr: &map2\n"
             "  !a foo: bar\n"
             "bra: &map2\n"
             "\n"
             "\n"
             "\n"
             "  !a foo: bar\n"
             "bre: &map2\n"
             "  !a\n"
             "  \n"
             "  \n"
             "  foo: bar\n"
             "bru:   &wtf\n"
             "  foo\n"
             ,
             "!mymap &mymap\n"
             "!footag &fooanch foo: &seq\n"
             "  &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "!bartag &baranch bar: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  !b &key20 key20: !c &val20 val20\n"
             "  key10: 20\n"
             "!baztag &bazanch baz: !a &map2\n"
             "  !b &key10 key10: !c &val10 val10\n"
             "  !b &key20 key20: !c &val20 val20\n"
             "  key10: 20\n"
             "brr: &map2\n"
             "  !a foo: bar\n"
             "bra: &map2\n"
             "  !a foo: bar\n"
             "bre: !a &map2\n"
             "  foo: bar\n"
             "bru: &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &mymap <!mymap>\n"
            "=VAL &fooanch <!footag> :foo\n"
            "+MAP &seq\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "=VAL &baranch <!bartag> :bar\n"
            "+MAP &map2 <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 <!c> :val10\n"
            "=VAL &key20 <!b> :key20\n"
            "=VAL &val20 <!c> :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "=VAL &bazanch <!baztag> :baz\n"
            "+MAP &map2 <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 <!c> :val10\n"
            "=VAL &key20 <!b> :key20\n"
            "=VAL &val20 <!c> :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "=VAL :brr\n"
            "+MAP &map2\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bra\n"
            "+MAP &map2\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bre\n"
            "+MAP &map2 <!a>\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL :bru\n"
            "=VAL &wtf :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_tag("!mymap"));
    ___(ps.set_val_anchor("mymap"));
    ___(ps.begin_map_val_block());
      ___(ps.set_key_tag("!footag"));
      ___(ps.set_key_anchor("fooanch"));
      ___(ps.set_key_scalar_plain("foo"));
      ___(ps.set_val_anchor("seq"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_anchor("key1"));
        ___(ps.set_key_scalar_plain("key1"));
        ___(ps.set_val_anchor("val1"));
        ___(ps.set_val_scalar_plain("val1"));
        ___(ps.add_sibling());
        ___(ps.set_key_anchor("key2"));
        ___(ps.set_key_scalar_plain("key2"));
        ___(ps.set_val_anchor("val2"));
        ___(ps.set_val_scalar_plain("val2"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_tag("!bartag"));
      ___(ps.set_key_anchor("baranch"));
      ___(ps.set_key_scalar_plain("bar"));
      ___(ps.set_val_tag("!a"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key10"));
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val10"));
        ___(ps.set_val_scalar_plain("val10"));
        ___(ps.add_sibling());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key20"));
        ___(ps.set_key_scalar_plain("key20"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val20"));
        ___(ps.set_val_scalar_plain("val20"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_tag("!baztag"));
      ___(ps.set_key_anchor("bazanch"));
      ___(ps.set_key_scalar_plain("baz"));
      ___(ps.set_val_tag("!a"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key10"));
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val10"));
        ___(ps.set_val_scalar_plain("val10"));
        ___(ps.add_sibling());
        ___(ps.set_key_tag("!b"));
        ___(ps.set_key_anchor("key20"));
        ___(ps.set_key_scalar_plain("key20"));
        ___(ps.set_val_tag("!c"));
        ___(ps.set_val_anchor("val20"));
        ___(ps.set_val_scalar_plain("val20"));
        ___(ps.add_sibling());
        ___(ps.set_key_scalar_plain("key10"));
        ___(ps.set_val_scalar_plain("20"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("brr"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!a"));
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bra"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_tag("!a"));
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bre"));
      ___(ps.set_val_anchor("map2"));
      ___(ps.set_val_tag("!a"));
      ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("foo"));
        ___(ps.set_val_scalar_plain("bar"));
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_key_scalar_plain("bru"));
      ___(ps.set_val_anchor("wtf"));
      ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(AnchorSeqMapSuckerPunch,
            ("&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- !a &map2a\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "-   &map2x !a\n"
             "    &key10 !b key10: &val10 val10\n"
             "    &key20 key20: &val20 val20\n"
             "    key10: 20\n"
             "- &map2y\n"
             "  !a foo: bar\n"
             "- &map2z\n"
             "\n"
             "\n"
             "\n"
             "  !a foo: bar\n"
             "- &map2u\n"
             "  !a\n"
             "  \n"
             "  \n"
             "  foo: bar\n"
             "-   &wtf\n"
             "  foo\n"
             "  ",
             "&seq\n"
             "- &key1 key1: &val1 val1\n"
             "  &key2 key2: &val2 val2\n"
             "- !a &map2a\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "- !a &map2x\n"
             "  !b &key10 key10: &val10 val10\n"
             "  &key20 key20: &val20 val20\n"
             "  key10: 20\n"
             "- &map2y\n"
             "  !a foo: bar\n"
             "- &map2z\n"
             "  !a foo: bar\n"
             "- !a &map2u\n"
             "  foo: bar\n"
             "- &wtf foo\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ &seq\n"
            "+MAP\n"
            "=VAL &key1 :key1\n"
            "=VAL &val1 :val1\n"
            "=VAL &key2 :key2\n"
            "=VAL &val2 :val2\n"
            "-MAP\n"
            "+MAP &map2a <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP &map2x <!a>\n"
            "=VAL &key10 <!b> :key10\n"
            "=VAL &val10 :val10\n"
            "=VAL &key20 :key20\n"
            "=VAL &val20 :val20\n"
            "=VAL :key10\n"
            "=VAL :20\n"
            "-MAP\n"
            "+MAP &map2y\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "+MAP &map2z\n"
            "=VAL <!a> :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "+MAP &map2u <!a>\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "=VAL &wtf :foo\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("seq"));
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key1"));
            ___(ps.set_key_scalar_plain("key1"));
            ___(ps.set_val_anchor("val1"));
            ___(ps.set_val_scalar_plain("val1"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key2"));
            ___(ps.set_key_scalar_plain("key2"));
            ___(ps.set_val_anchor("val2"));
            ___(ps.set_val_scalar_plain("val2"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2a"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key10"));
            ___(ps.set_key_tag("!b"));
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_anchor("val10"));
            ___(ps.set_val_scalar_plain("val10"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key20"));
            ___(ps.set_key_scalar_plain("key20"));
            ___(ps.set_val_anchor("val20"));
            ___(ps.set_val_scalar_plain("val20"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_scalar_plain("20"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2x"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_anchor("key10"));
            ___(ps.set_key_tag("!b"));
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_anchor("val10"));
            ___(ps.set_val_scalar_plain("val10"));
            ___(ps.add_sibling());
            ___(ps.set_key_anchor("key20"));
            ___(ps.set_key_scalar_plain("key20"));
            ___(ps.set_val_anchor("val20"));
            ___(ps.set_val_scalar_plain("val20"));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("key10"));
            ___(ps.set_val_scalar_plain("20"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2y"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_tag("!a"));
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2z"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_tag("!a"));
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("map2u"));
        ___(ps.set_val_tag("!a"));
        ___(ps.begin_map_val_block());
            ___(ps.set_key_scalar_plain("foo"));
            ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_anchor("wtf"));
        ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(DocDoubleAnchorSeqMap,
            (HAS_CONTAINER_KEYS,
             "--- &docanchor\n"
             "&anchor [seq, yeah]: foo\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP &docanchor\n"
            "+SEQ [] &anchor\n"
            "=VAL :seq\n"
            "=VAL :yeah\n"
            "-SEQ\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.set_val_scalar_plain("yeah"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DoubleAnchorSeqMap,
            (HAS_CONTAINER_KEYS,
             "&docanchor\n"
             "&anchor [seq, yeah]: foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &docanchor\n"
            "+SEQ [] &anchor\n"
            "=VAL :seq\n"
            "=VAL :yeah\n"
            "-SEQ\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.set_val_scalar_plain("yeah"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DocDoubleAnchorKeyMap,
            (HAS_CONTAINER_KEYS,
             "--- &docanchor\n"
             "&anchor {key: val}: foo\n"),
            "+STR\n"
            "+DOC ---\n"
            "+MAP &docanchor\n"
            "+MAP {} &anchor\n"
            "=VAL :key\n"
            "=VAL :val\n"
            "-MAP\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc_expl());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(DoubleAnchorKeyMap,
            (HAS_CONTAINER_KEYS,
             "&docanchor\n"
             "&anchor {key: val}: foo\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP &docanchor\n"
            "+MAP {} &anchor\n"
            "=VAL :key\n"
            "=VAL :val\n"
            "-MAP\n"
            "=VAL :foo\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.set_val_anchor("docanchor"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_val_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("foo"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockMapKey_SU74_1, Location(2,10), ""
                   "key1: &alias value1\n"
                   "&b *alias : value2\n")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockMapKey_SU74_2, Location(2,8), ""
                   "key1: &alias value1\n"
                   "*alias &b: value2\n")

ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowMapKey_SU74_1, Location(2,10), ""
                   "{key1: &alias value1,\n"
                   "&b *alias : value2}\n")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowMapKey_SU74_2, Location(2,8), ""
                   "{key1: &alias value1,\n"
                   "*alias &b: value2}\n")

ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowMapVal_SU74_1, Location(2,18), ""
                   "key1: &alias value1\n"
                   "value2: &b *alias\n")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowMapVal_SU74_2, Location(2,16), ""
                   "key1: &alias value1\n"
                   "value2: *alias &b\n")

ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockMapVal_SU74_1, Location(2,18), ""
                   "{key1: &alias value1,\n"
                   "value2: &b *alias\n}")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockMapVal_SU74_2, Location(2,16), ""
                   "{key1: &alias value1,\n"
                   "value2: *alias &b\n}")

ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowSeqVal_SU74_1, Location(2,12), ""
                   "- &alias value1\n"
                   "- &b *alias\n")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsFlowSeqVal_SU74_2, Location(2,10), ""
                   "- &alias value1\n"
                   "- *alias &b\n")

ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockSeqVal_SU74_1, Location(2,10), ""
                   "[&alias value1,\n"
                   "&b *alias]\n")
ENGINE_TEST_ERRLOC(AnchorAndAliasAsBlockSeqVal_SU74_2, Location(2,8), ""
                   "[&alias value1,\n"
                   "*alias &b]\n")

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
