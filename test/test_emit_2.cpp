#ifndef RYML_SINGLE_HEADER
#include "c4/yml/emit.hpp"
#include <c4/yml/parse.hpp>
#endif

#include "./test_emits/test_emits_common.hpp"

#include <gtest/gtest.h>

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

// NOLINTBEGIN(*-signed-bitwise)



//-----------------------------------------------------------------------------

static void test_emit_seq_node(TreeAndInts & ti, std::string const& yaml)
{
    RYML_TRACE_FMT("yaml:\n~~~{}\n~~~\n", yaml);
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("t[0]");
        auto testfoo = [&]{
            {
                SCOPED_TRACE("orig");
                test_emits_(ti, t[0], 3, "foo", "\"foo\"");
            }
            {
                SCOPED_TRACE("plain");
                TMPSTY(plai, ti, t[0], 3);
                test_emits_(ti, t[0], 3, "foo", "\"foo\"");
            }
            {
                SCOPED_TRACE("squo");
                TMPSTY(squo, ti, t[0], 3);
                test_emits_(ti, t[0], 3, "'foo'", "\"foo\"");
            }
            {
                SCOPED_TRACE("dquo");
                TMPSTY(dquo, ti, t[0], 3);
                test_emits_(ti, t[0], 3, "\"foo\"", "\"foo\"");
            }
            {
                SCOPED_TRACE("fold");
                TMPSTY(fold, ti, t[0], 3);
                test_emits_(ti, t[0], 3, ">-\n  foo", "\"foo\"");
            }
            {
                SCOPED_TRACE("litl");
                TMPSTY(litl, ti, t[0], 3);
                test_emits_(ti, t[0], 3, "|-\n  foo", "\"foo\"");
            }
            {
                SCOPED_TRACE("nosty");
                TMPSTY(nosty, ti, t[0], 3);
                test_emits_(ti, t[0], 3, "foo", "\"foo\"");
            }
        };
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            testfoo();
        }
    }
    {
        SCOPED_TRACE("t[1]=bar");
        auto testbar = [&]{
            {
                SCOPED_TRACE("orig");
                test_emits_(ti, t[1], 6, "bar", "\"bar\"");
            }
            {
                SCOPED_TRACE("plain");
                TMPSTY(plai, ti, t[1], 6);
                test_emits_(ti, t[1], 6, "bar", "\"bar\"");
            }
            {
                SCOPED_TRACE("squo");
                TMPSTY(squo, ti, t[1], 6);
                test_emits_(ti, t[1], 6, "'bar'", "\"bar\"");
            }
            {
                SCOPED_TRACE("dquo");
                TMPSTY(dquo, ti, t[1], 6);
                test_emits_(ti, t[1], 6, "\"bar\"", "\"bar\"");
            }
            {
                SCOPED_TRACE("fold");
                TMPSTY(fold, ti, t[1], 6);
                test_emits_(ti, t[1], 6, ">-\n  bar", "\"bar\"");
            }
            {
                SCOPED_TRACE("litl");
                TMPSTY(litl, ti, t[1], 6);
                test_emits_(ti, t[1], 6, "|-\n  bar", "\"bar\"");
            }
            {
                SCOPED_TRACE("nosty");
                TMPSTY(nosty, ti, t[1], 6);
                test_emits_(ti, t[1], 6, "bar", "\"bar\"");
            }
        };
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            testbar();
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            testbar();
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            testbar();
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            testbar();
        }
    }
    {
        SCOPED_TRACE("t[2]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "- nested\n- seq\n",
                        "[\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[nested,seq]",
                        "[\"nested\",\"seq\"]");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[\n  nested,\n  seq\n]\n",
                        "[\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t[2], 9);
            test_emits_(ti, t[2], 9,
                        "[\n  nested,seq\n]\n",
                        "[\n  \"nested\",\"seq\"\n]\n");
        }
    }
    {
        SCOPED_TRACE("t[3]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "nested: map\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{nested: map}",
                        "{\"nested\": \"map\"}");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{\n  nested: map\n}\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t[3], 17);
            test_emits_(ti, t[3], 17,
                        "{\n  nested: map\n}\n",
                        "{\n  \"nested\": \"map\"\n}\n");
        }
    }
    {
        SCOPED_TRACE("root");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            TMPSTY(blk, ti, t[2], 9);
            TMPSTY(blk, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n",
                        "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 9);
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "[foo,bar,[nested,seq],{nested: map}]",
                        "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
            test_emits_(ti, t, 0,
                        "[foo, bar, [nested, seq], {nested: map}]",
                        "[\"foo\", \"bar\", [\"nested\", \"seq\"], {\"nested\": \"map\"}]",
                        with_flow_spc);
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 9);
            TMPSTY(flowml1, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 2,
                            "[\nfoo,\nbar,\n[\nnested,\nseq\n],\n{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            TMPSTY(flowmln, ti, t[2], 9);
            TMPSTY(flowmln, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,bar,[\n    nested,seq\n  ],{\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\"bar\",[\n    \"nested\",\"seq\"\n  ],{\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "[\nfoo,bar,[\nnested,seq\n],{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\"bar\",[\n\"nested\",\"seq\"\n],{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
            {
                SCOPED_TRACE("spc");
                test_emits_(ti, t, 0,
                            "[\n  foo, bar, [\n    nested, seq\n  ], {\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\", \"bar\", [\n    \"nested\", \"seq\"\n  ], {\n    \"nested\": \"map\"\n  }\n]\n",
                            with_flow_spc);
            }
        }
        {
            SCOPED_TRACE("block-flow-ml");
            {
                TMPSTY(blk, ti, t, 2);
                TMPSTY(flowml1, ti, t[2], 9);
                TMPSTY(flowml1, ti, t[3], 17);
                {
                    SCOPED_TRACE("indent");
                    test_emits_(ti, t, 0,
                                "- foo\n- bar\n- [\n    nested,\n    seq\n  ]\n- {\n    nested: map\n  }\n",
                                "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
                }
                {
                    SCOPED_TRACE("no indent");
                    test_emits_(ti, t, 0,
                                "- foo\n- bar\n- [\n  nested,\n  seq\n  ]\n- {\n  nested: map\n  }\n",
                                "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                                noindent);
                }
            }
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 9);
            TMPSTY(flowml1, ti, t[3], 17);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                            "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
            }
            {
                SCOPED_TRACE("no indent ml");
                test_emits_(ti, t, 0,
                            "[\nfoo,\nbar,\n[\nnested,\nseq\n],\n{\nnested: map\n}\n]\n",
                            "[\n\"foo\",\n\"bar\",\n[\n\"nested\",\n\"seq\"\n],\n{\n\"nested\": \"map\"\n}\n]\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 9);
            TMPSTY(flowsl, ti, t[3], 17);
            test_emits_(ti, t, 0,
                        "[foo,bar,[nested,seq],{nested: map}]",
                        "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
            test_emits_(ti, t, 0,
                        "[foo, bar, [nested, seq], {nested: map}]",
                        "[\"foo\", \"bar\", [\"nested\", \"seq\"], {\"nested\": \"map\"}]",
                        with_flow_spc);
        }
    }
}

TEST(emit, existing_seq_node_flow_sl)
{
    const std::string yaml = "[foo,bar,[nested,seq],{nested: map}]";
    SCOPED_TRACE(yaml);
    TreeAndInts ti(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "[foo,bar,[nested,seq],{nested: map}]",
                    "[\"foo\",\"bar\",[\"nested\",\"seq\"],{\"nested\": \"map\"}]");
        test_emits_(ti, ti.tree, 0,
                    "[foo, bar, [nested, seq], {nested: map}]",
                    "[\"foo\", \"bar\", [\"nested\", \"seq\"], {\"nested\": \"map\"}]",
                    with_flow_spc);
    }
    test_emit_seq_node(ti, yaml);
}

TEST(emit, existing_seq_node_flow_ml)
{
    const std::string yaml = "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n";
    SCOPED_TRACE(yaml);
    TreeAndInts ti(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "[\n  foo,\n  bar,\n  [\n    nested,\n    seq\n  ],\n  {\n    nested: map\n  }\n]\n",
                    "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
    }
    test_emit_seq_node(ti, yaml);
}

TEST(emit, existing_seq_node_block)
{
    const std::string yaml = "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n";
    SCOPED_TRACE(yaml);
    TreeAndInts ti(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, ti.tree, 0,
                    "- foo\n- bar\n- - nested\n  - seq\n- nested: map\n",
                    "[\n  \"foo\",\n  \"bar\",\n  [\n    \"nested\",\n    \"seq\"\n  ],\n  {\n    \"nested\": \"map\"\n  }\n]\n");
    }
    test_emit_seq_node(ti, yaml);
}


//-----------------------------------------------------------------------------


static void test_emit_map_node(TreeAndInts & ti, std::string const& yaml)
{
    SCOPED_TRACE(yaml);
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("root");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            TMPSTY(blk, ti, t[2], 18);
            TMPSTY(blk, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n",
                        "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                        R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
            test_emits_(ti, t, 0,
                        "{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}",
                        R"({"0": "foo", "1": "bar", "2": ["nested", "seq"], "3": {"nested": "map"}})",
                        with_flow_spc);
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 18);
            TMPSTY(flowml1, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_ml1-flowsl");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [nested,seq],\n3: {nested: map}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\"nested\",\"seq\"],\n\"3\": {\"nested\": \"map\"}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_mln-flowsl");
            TMPSTY(flowmln, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,1: bar,2: [nested,seq],3: {nested: map}\n}\n",
                            "{\n  \"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,1: bar,2: [nested,seq],3: {nested: map}\n}\n",
                            "{\n\"0\": \"foo\",\"1\": \"bar\",\"2\": [\"nested\",\"seq\"],\"3\": {\"nested\": \"map\"}\n}\n",
                            noindent);
            }
        }
    }
    {
        SCOPED_TRACE("full-all");
        {
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            TMPSTY(flowml1, ti, t[2], 18);
            TMPSTY(flowml1, ti, t[3], 29);
            {
                SCOPED_TRACE("indent");
                test_emits_(ti, t, 0,
                            "{\n  0: foo,\n  1: bar,\n  2: [\n    nested,\n    seq\n  ],\n  3: {\n    nested: map\n  }\n}\n",
                            "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
            }
            {
                SCOPED_TRACE("no indent");
                test_emits_(ti, t, 0,
                            "{\n0: foo,\n1: bar,\n2: [\nnested,\nseq\n],\n3: {\nnested: map\n}\n}\n",
                            "{\n\"0\": \"foo\",\n\"1\": \"bar\",\n\"2\": [\n\"nested\",\n\"seq\"\n],\n\"3\": {\n\"nested\": \"map\"\n}\n}\n",
                            noindent);
            }
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            TMPSTY(flowsl, ti, t[2], 18);
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t, 0,
                        "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                        R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
            test_emits_(ti, t, 0,
                        "{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}",
                        R"({"0": "foo", "1": "bar", "2": ["nested", "seq"], "3": {"nested": "map"}})",
                        with_flow_spc);
        }
    }
    {
        SCOPED_TRACE("t[0]");
        auto testfoo = [&]{
            {
                SCOPED_TRACE("orig");
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "0: foo\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, "foo", "\"foo\"", without_key);
                }
            }
            {
                SCOPED_TRACE("plai");
                TMPSTY(plaikv, ti, t[0], 3, 6);
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "0: foo\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, "foo", "\"foo\"", without_key);
                }
            }
            {
                SCOPED_TRACE("squo");
                TMPSTY(squokv, ti, t[0], 3, 6);
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "'0': 'foo'\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, "'foo'", "\"foo\"", without_key);
                }
            }
            {
                SCOPED_TRACE("dquo");
                TMPSTY(dquokv, ti, t[0], 3, 6);
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "\"0\": \"foo\"\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, "\"foo\"", "\"foo\"", without_key);
                }
            }
            {
                SCOPED_TRACE("litl");
                TMPSTY(litlkv, ti, t[0], 3, 6);
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "? |-\n  0\n: |-\n    foo\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, "|-\n  foo", "\"foo\"", without_key);
                }
            }
            {
                SCOPED_TRACE("fold");
                TMPSTY(foldkv, ti, t[0], 3, 6);
                {
                    SCOPED_TRACE("key");
                    test_emits_(ti, t[0], 3, "? >-\n  0\n: >-\n    foo\n", "\"0\": \"foo\"\n");
                }
                {
                    SCOPED_TRACE("nokey");
                    test_emits_(ti, t[0], 6, ">-\n  foo", "\"foo\"", without_key);
                }
            }
        };
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_mln");
            TMPSTY(flowmln, ti, t, 2);
            testfoo();
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            testfoo();
        }
    }
    {
        SCOPED_TRACE("t[1]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t, 2);
            test_emits_(ti, t[1], 9,
                        "1: bar\n",
                        "\"1\": \"bar\"\n");
        }
    }
    {
        SCOPED_TRACE("t[2]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2:\n  - nested\n  - seq\n",
                        "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_ml1");
            TMPSTY(flowml1, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2: [\n    nested,\n    seq\n  ]\n", // FIXME should be indented one level, not two
                        "\"2\": [\n  \"nested\",\n  \"seq\"\n]\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[2], 18);
            test_emits_(ti, t[2], 15,
                        "2: [nested,seq]\n",
                        "\"2\": [\"nested\",\"seq\"]\n");
            test_emits_(ti, t[2], 15,
                        "2: [nested, seq]\n",
                        "\"2\": [\"nested\", \"seq\"]\n",
                        with_flow_spc);
        }
    }
    {
        SCOPED_TRACE("t[3]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3:\n  nested: map\n",
                        "\"3\": {\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_ml");
            TMPSTY(flowml1, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3: {\n    nested: map\n  }\n", // FIXME should be indented one level, not two
                        "\"3\": {\n  \"nested\": \"map\"\n}\n");
        }
        {
            SCOPED_TRACE("flow_sl");
            TMPSTY(flowsl, ti, t[3], 29);
            test_emits_(ti, t[3], 26,
                        "3: {nested: map}\n",
                        "\"3\": {\"nested\": \"map\"}\n");
        }
    }
}


TEST(emit, existing_map_node_flow_sl)
{
    const std::string yaml = "{0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}}";
    TreeAndInts ti(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "{0: foo,1: bar,2: [nested,seq],3: {nested: map}}",
                    R"({"0": "foo","1": "bar","2": ["nested","seq"],"3": {"nested": "map"}})");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}

TEST(emit, existing_map_node_flow_ml)
{
    const std::string yaml = "{\n 0: foo, 1: bar, 2: [nested, seq], 3: {nested: map}\n}\n";
    TreeAndInts ti(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "{\n  0: foo,\n  1: bar,\n  2: [nested,seq],\n  3: {nested: map}\n}\n",
                    "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\"nested\",\"seq\"],\n  \"3\": {\"nested\": \"map\"}\n}\n");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}

TEST(emit, existing_map_node_block)
{
    const std::string yaml = "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n";
    TreeAndInts ti(to_csubstr(yaml));
    Tree & t = ti.tree;
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, t, 0,
                    "0: foo\n1: bar\n2:\n  - nested\n  - seq\n3:\n  nested: map\n",
                    "{\n  \"0\": \"foo\",\n  \"1\": \"bar\",\n  \"2\": [\n    \"nested\",\n    \"seq\"\n  ],\n  \"3\": {\n    \"nested\": \"map\"\n  }\n}\n");
    }
    {
        SCOPED_TRACE("here");
        test_emit_map_node(ti, yaml);
    }
}


//-----------------------------------------------------------------------------

TEST(emit, container_key_seq_flow)
{
    SCOPED_TRACE("container_key_map_flow");
    IntBufs ints;
    std::string yaml = "[key, is, a, seq]: [val, is, a, seq]";
    parse_ints(to_substr(yaml), &ints);
    test_emits_ints_nojson_(ints, 0, "? [key,is,a,seq]\n: [val,is,a,seq]\n");
    {
        SCOPED_TRACE("root-blk");
        TMPSTY(blk, ints, 2);
        auto dotest = [&](strref keyval, strref key, strref val){
            test_emits_ints_nojson_(ints, 0, keyval);
            test_emits_ints_nojson_(ints, 1, keyval);
            test_emits_ints_nojson_(ints, 2, keyval);
            test_emits_ints_nojson_(ints, 2, keyval, without_key);
            test_emits_ints_nojson_(ints, 3, keyval);
            test_emits_ints_nojson_(ints, 3, key, without_key);
            test_emits_ints_nojson_(ints, 17, val);
            test_emits_ints_nojson_(ints, 17, val, without_key);
        };
        {
            SCOPED_TRACE("kv-blk");
            TMPSTY(blk, ints, ints, 3, 17);
            dotest("?\n  - key\n  - is\n  - a\n  - seq\n:\n  - val\n  - is\n  - a\n  - seq\n",
                   "- key\n- is\n- a\n- seq\n",
                   "- val\n- is\n- a\n- seq\n");
        }
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("? [\n    key,\n    is,\n    a,\n    seq\n  ]\n: [\n    val,\n    is,\n    a,\n    seq\n  ]\n",
                   "[\n  key,\n  is,\n  a,\n  seq\n]\n",
                   "[\n  val,\n  is,\n  a,\n  seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("? [\n    key,is,a,seq\n  ]\n: [\n    val,is,a,seq\n  ]\n",
                   "[\n  key,is,a,seq\n]\n",
                   "[\n  val,is,a,seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("? [\n    key, is, a, seq\n  ]\n: [\n    val, is, a, seq\n  ]\n",
                   "[\n  key, is, a, seq\n]\n",
                   "[\n  val, is, a, seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("? [key,is,a,seq]\n: [val,is,a,seq]\n",
                   "[key,is,a,seq]",
                   "[val,is,a,seq]");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("? [key, is, a, seq]\n: [val, is, a, seq]\n",
                   "[key, is, a, seq]",
                   "[val, is, a, seq]");
        }
    }
    auto dotest = [&](strref full, strref keyval, strref key, strref val){
        test_emits_ints_nojson_(ints, 0, full);
        test_emits_ints_nojson_(ints, 1, full);
        test_emits_ints_nojson_(ints, 2, full);
        test_emits_ints_nojson_(ints, 2, full, without_key);
        test_emits_ints_nojson_(ints, 3, keyval);
        test_emits_ints_nojson_(ints, 3, key, without_key);
        test_emits_ints_nojson_(ints, 17, val);
        test_emits_ints_nojson_(ints, 17, val, without_key);
    };
    {
        SCOPED_TRACE("root-flowsl");
        TMPSTY(flowsl, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{? [\n  key,\n  is,\n  a,\n  seq\n]: [\n  val,\n  is,\n  a,\n  seq\n]}",
                   "? [\n    key,\n    is,\n    a,\n    seq\n  ]\n: [\n    val,\n    is,\n    a,\n    seq\n  ]\n",
                   "[\n  key,\n  is,\n  a,\n  seq\n]\n",
                   "[\n  val,\n  is,\n  a,\n  seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{? [\n  key,is,a,seq\n]: [\n  val,is,a,seq\n]}",
                   "? [\n    key,is,a,seq\n  ]\n: [\n    val,is,a,seq\n  ]\n",
                   "[\n  key,is,a,seq\n]\n",
                   "[\n  val,is,a,seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("{? [\n  key, is, a, seq\n]: [\n  val, is, a, seq\n]}",
                   "? [\n    key, is, a, seq\n  ]\n: [\n    val, is, a, seq\n  ]\n",
                   "[\n  key, is, a, seq\n]\n",
                   "[\n  val, is, a, seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{? [key,is,a,seq]: [val,is,a,seq]}",
                   "? [key,is,a,seq]\n: [val,is,a,seq]\n",
                   "[key,is,a,seq]",
                   "[val,is,a,seq]");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("{? [key, is, a, seq]: [val, is, a, seq]}",
                   "? [key, is, a, seq]\n: [val, is, a, seq]\n",
                   "[key, is, a, seq]",
                   "[val, is, a, seq]");
        }
    }
    {
        SCOPED_TRACE("root-flowml1");
        TMPSTY(flowml1, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{\n  ? [\n    key,\n    is,\n    a,\n    seq\n  ]: [\n    val,\n    is,\n    a,\n    seq\n  ]\n}\n",
                   "? [\n    key,\n    is,\n    a,\n    seq\n  ]\n: [\n    val,\n    is,\n    a,\n    seq\n  ]\n",
                   "[\n  key,\n  is,\n  a,\n  seq\n]\n",
                   "[\n  val,\n  is,\n  a,\n  seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{\n  ? [\n    key,is,a,seq\n  ]: [\n    val,is,a,seq\n  ]\n}\n",
                   "? [\n    key,is,a,seq\n  ]\n: [\n    val,is,a,seq\n  ]\n",
                   "[\n  key,is,a,seq\n]\n",
                   "[\n  val,is,a,seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? [\n    key, is, a, seq\n  ]: [\n    val, is, a, seq\n  ]\n}\n",
                   "? [\n    key, is, a, seq\n  ]\n: [\n    val, is, a, seq\n  ]\n",
                   "[\n  key, is, a, seq\n]\n",
                   "[\n  val, is, a, seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{\n  ? [key,is,a,seq]: [val,is,a,seq]\n}\n",
                   "? [key,is,a,seq]\n: [val,is,a,seq]\n",
                   "[key,is,a,seq]",
                   "[val,is,a,seq]");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? [key, is, a, seq]: [val, is, a, seq]\n}\n",
                   "? [key, is, a, seq]\n: [val, is, a, seq]\n",
                   "[key, is, a, seq]",
                   "[val, is, a, seq]");
        }
    }
    {
        SCOPED_TRACE("root-flowmln");
        TMPSTY(flowmln, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{\n  ? [\n    key,is,a,seq\n  ]: [\n    val,is,a,seq\n  ]\n}\n",
                   "? [\n    key,\n    is,\n    a,\n    seq\n  ]\n: [\n    val,\n    is,\n    a,\n    seq\n  ]\n",
                   "[\n  key,\n  is,\n  a,\n  seq\n]\n",
                   "[\n  val,\n  is,\n  a,\n  seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{\n  ? [\n    key,is,a,seq\n  ]: [\n    val,is,a,seq\n  ]\n}\n",
                   "? [\n    key,is,a,seq\n  ]\n: [\n    val,is,a,seq\n  ]\n",
                   "[\n  key,is,a,seq\n]\n",
                   "[\n  val,is,a,seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            // FIXME!
            dotest("{\n  ? [\n    key,is,a,seq\n  ]: [\n    val,is,a,seq\n  ]\n}\n",
                   "? [\n    key, is, a, seq\n  ]\n: [\n    val, is, a, seq\n  ]\n",
                   "[\n  key, is, a, seq\n]\n",
                   "[\n  val, is, a, seq\n]\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{\n  ? [key,is,a,seq]: [val,is,a,seq]\n}\n",
                   "? [key,is,a,seq]\n: [val,is,a,seq]\n",
                   "[key,is,a,seq]",
                   "[val,is,a,seq]");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            // FIXME!
            dotest("{\n  ? [key,is,a,seq]: [val,is,a,seq]\n}\n",
                   "? [key, is, a, seq]\n: [val, is, a, seq]\n",
                   "[key, is, a, seq]",
                   "[val, is, a, seq]");
        }
    }
}

TEST(emit, container_key_map_flow)
{
    SCOPED_TRACE("container_key_map_flow");
    IntBufs ints;
    std::string yaml = "{key: is, a: map}: {val: is, a: map}";
    parse_ints(to_substr(yaml), &ints);
    test_emits_ints_nojson_(ints, 0, "? {key: is,a: map}\n: {val: is,a: map}\n");
    {
        SCOPED_TRACE("error on key container");
        test_emit_error_json_ints(ints, 3);
    }
    {
        SCOPED_TRACE("root-blk");
        TMPSTY(blk, ints, 2);
        auto dotest = [&](strref keyval, strref key, strref val){
            test_emits_ints_nojson_(ints, 0, keyval);
            test_emits_ints_nojson_(ints, 1, keyval);
            test_emits_ints_nojson_(ints, 2, keyval);
            test_emits_ints_nojson_(ints, 2, keyval, without_key);
            test_emits_ints_nojson_(ints, 3, keyval);
            test_emits_ints_nojson_(ints, 3, key, without_key);
            test_emits_ints_nojson_(ints, 17, val);
            test_emits_ints_nojson_(ints, 17, val, without_key);
        };
        {
            SCOPED_TRACE("kv-blk");
            TMPSTY(blk, ints, ints, 3, 17);
            dotest("?\n  key: is\n  a: map\n:\n  val: is\n  a: map\n",
                   "key: is\na: map\n",
                   "val: is\na: map\n");
        }
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("? {\n    key: is,\n    a: map\n  }\n: {\n    val: is,\n    a: map\n  }\n",
                   "{\n  key: is,\n  a: map\n}\n",
                   "{\n  val: is,\n  a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("? {\n    key: is,a: map\n  }\n: {\n    val: is,a: map\n  }\n",
                   "{\n  key: is,a: map\n}\n",
                   "{\n  val: is,a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("? {\n    key: is, a: map\n  }\n: {\n    val: is, a: map\n  }\n",
                   "{\n  key: is, a: map\n}\n",
                   "{\n  val: is, a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("? {key: is,a: map}\n: {val: is,a: map}\n",
                   "{key: is,a: map}",
                   "{val: is,a: map}");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("? {key: is, a: map}\n: {val: is, a: map}\n",
                   "{key: is, a: map}",
                   "{val: is, a: map}");
        }
    }
    auto dotest = [&](strref full, strref keyval, strref key, strref val){
        test_emits_ints_nojson_(ints, 0, full);
        test_emits_ints_nojson_(ints, 1, full);
        test_emits_ints_nojson_(ints, 2, full);
        test_emits_ints_nojson_(ints, 2, full, without_key);
        test_emits_ints_nojson_(ints, 3, keyval);
        test_emits_ints_nojson_(ints, 3, key, without_key);
        test_emits_ints_nojson_(ints, 17, val);
        test_emits_ints_nojson_(ints, 17, val, without_key);
    };
    {
        SCOPED_TRACE("root-flowsl");
        TMPSTY(flowsl, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{? {\n  key: is,\n  a: map\n}: {\n  val: is,\n  a: map\n}}",
                   "? {\n    key: is,\n    a: map\n  }\n: {\n    val: is,\n    a: map\n  }\n",
                   "{\n  key: is,\n  a: map\n}\n",
                   "{\n  val: is,\n  a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{? {\n  key: is,a: map\n}: {\n  val: is,a: map\n}}",
                   "? {\n    key: is,a: map\n  }\n: {\n    val: is,a: map\n  }\n",
                   "{\n  key: is,a: map\n}\n",
                   "{\n  val: is,a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("{? {\n  key: is, a: map\n}: {\n  val: is, a: map\n}}",
                   "? {\n    key: is, a: map\n  }\n: {\n    val: is, a: map\n  }\n",
                   "{\n  key: is, a: map\n}\n",
                   "{\n  val: is, a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{? {key: is,a: map}: {val: is,a: map}}",
                   "? {key: is,a: map}\n: {val: is,a: map}\n",
                   "{key: is,a: map}",
                   "{val: is,a: map}");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("{? {key: is, a: map}: {val: is, a: map}}",
                   "? {key: is, a: map}\n: {val: is, a: map}\n",
                   "{key: is, a: map}",
                   "{val: is, a: map}");
        }
    }
    {
        SCOPED_TRACE("root-flowml1");
        TMPSTY(flowml1, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{\n  ? {\n    key: is,\n    a: map\n  }: {\n    val: is,\n    a: map\n  }\n}\n",
                   "? {\n    key: is,\n    a: map\n  }\n: {\n    val: is,\n    a: map\n  }\n",
                   "{\n  key: is,\n  a: map\n}\n",
                   "{\n  val: is,\n  a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{\n  ? {\n    key: is,a: map\n  }: {\n    val: is,a: map\n  }\n}\n",
                   "? {\n    key: is,a: map\n  }\n: {\n    val: is,a: map\n  }\n",
                   "{\n  key: is,a: map\n}\n",
                   "{\n  val: is,a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? {\n    key: is, a: map\n  }: {\n    val: is, a: map\n  }\n}\n",
                   "? {\n    key: is, a: map\n  }\n: {\n    val: is, a: map\n  }\n",
                   "{\n  key: is, a: map\n}\n",
                   "{\n  val: is, a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{\n  ? {key: is,a: map}: {val: is,a: map}\n}\n",
                   "? {key: is,a: map}\n: {val: is,a: map}\n",
                   "{key: is,a: map}",
                   "{val: is,a: map}");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? {key: is, a: map}: {val: is, a: map}\n}\n",
                   "? {key: is, a: map}\n: {val: is, a: map}\n",
                   "{key: is, a: map}",
                   "{val: is, a: map}");
        }
    }
    {
        SCOPED_TRACE("root-flowmln");
        TMPSTY(flowmln, ints, 2);
        {
            SCOPED_TRACE("kv-flowml1");
            TMPSTY(flowml1, ints, ints, 3, 17);
            dotest("{\n  ? {\n    key: is,a: map\n  }: {\n    val: is,a: map\n  }\n}\n",
                   "? {\n    key: is,\n    a: map\n  }\n: {\n    val: is,\n    a: map\n  }\n",
                   "{\n  key: is,\n  a: map\n}\n",
                   "{\n  val: is,\n  a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln");
            TMPSTY(flowmln, ints, ints, 3, 17);
            dotest("{\n  ? {\n    key: is,a: map\n  }: {\n    val: is,a: map\n  }\n}\n",
                   "? {\n    key: is,a: map\n  }\n: {\n    val: is,a: map\n  }\n",
                   "{\n  key: is,a: map\n}\n",
                   "{\n  val: is,a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowmln-spc");
            TMPSTY(flowmln, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? {\n    key: is,a: map\n  }: {\n    val: is,a: map\n  }\n}\n",
                   "? {\n    key: is, a: map\n  }\n: {\n    val: is, a: map\n  }\n",
                   "{\n  key: is, a: map\n}\n",
                   "{\n  val: is, a: map\n}\n");
        }
        {
            SCOPED_TRACE("kv-flowsl");
            TMPSTY(flowsl, ints, ints, 3, 17);
            dotest("{\n  ? {key: is,a: map}: {val: is,a: map}\n}\n",
                   "? {key: is,a: map}\n: {val: is,a: map}\n",
                   "{key: is,a: map}",
                   "{val: is,a: map}");
        }
        {
            SCOPED_TRACE("kv-flowsl-spc");
            TMPSTY(flowsl, ints, ints, 3, 17, xievt::FSPC);
            dotest("{\n  ? {key: is,a: map}: {val: is,a: map}\n}\n",
                   "? {key: is, a: map}\n: {val: is, a: map}\n",
                   "{key: is, a: map}",
                   "{val: is, a: map}");
        }
    }
}

// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
