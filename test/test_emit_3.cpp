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

TEST(emit, stream_docval)
{
    SCOPED_TRACE("stream_docval");
    std::string yaml = R"(doc0
---
doc1
---
doc2
)";
    std::string expected_json = R"([
  "doc0",
  "doc1",
  "doc2"
]
)";
    TreeAndInts ti(to_csubstr(yaml));
    test_emits_tree_(ti.tree.rootref(), "--- doc0\n--- doc1\n--- doc2\n", expected_json);
    test_emits_ints_(ti.ints, 0, "doc0\n--- doc1\n--- doc2\n", expected_json);
    test_emits_tree_(ti.tree.docref(0), "--- doc0", "\"doc0\""); // FIXME newline
    test_emits_ints_(ti.ints, 1, "doc0\n", "\"doc0\"");
    test_emits_ints_(ti.ints, 2, "doc0", "\"doc0\"");
    test_emits_ints_(ti.ints, 5, "doc1", "\"doc1\"");
    test_emits_tree_(ti.tree.docref(1), "--- doc1", "\"doc1\""); // FIXME newline
    test_emits_ints_(ti.ints, 6, "--- doc1\n", "\"doc1\"");
    test_emits_ints_(ti.ints, 7, "doc1", "\"doc1\"");
    test_emits_ints_(ti.ints, 10, "doc2", "\"doc2\"");
    test_emits_tree_(ti.tree.docref(2), "--- doc2", "\"doc2\""); // FIXME newline
    test_emits_ints_(ti.ints, 11, "--- doc2\n", "\"doc2\"");
    test_emits_ints_(ti.ints, 12, "doc2", "\"doc2\"");
    {
        SCOPED_TRACE("error on stream");
        const EmitOptions opts = EmitOptions{}.json_err_on_stream(true);
        test_emit_error_json_ints(ti.ints, 0, opts);
    }
}

TEST(emit, stream_docval_2)
{
    SCOPED_TRACE("stream_docval2");
    std::string yaml = R"(--- doc0
---
doc1
---
doc2
)";
    std::string expected_json = R"([
  "doc0",
  "doc1",
  "doc2"
]
)";
    TreeAndInts ti(to_csubstr(yaml));
    test_emits(ti, "--- doc0\n--- doc1\n--- doc2\n", expected_json);
    test_emits_tree_(ti.tree.docref(0), "--- doc0", "\"doc0\""); // FIXME newline
    test_emits_ints_(ti.ints, 1, "--- doc0\n", "\"doc0\"");
    test_emits_ints_(ti.ints, 2, "doc0", "\"doc0\"");
    test_emits_ints_(ti.ints, 5, "doc1", "\"doc1\"");
    test_emits_tree_(ti.tree.docref(1), "--- doc1", "\"doc1\""); // FIXME newline
    test_emits_ints_(ti.ints, 6, "--- doc1\n", "\"doc1\"");
    test_emits_ints_(ti.ints, 7, "doc1", "\"doc1\"");
    test_emits_ints_(ti.ints, 10, "doc2", "\"doc2\"");
    test_emits_tree_(ti.tree.docref(2), "--- doc2", "\"doc2\""); // FIXME newline
    test_emits_ints_(ti.ints, 11, "--- doc2\n", "\"doc2\"");
    test_emits_ints_(ti.ints, 12, "doc2", "\"doc2\"");
}

TEST(emit, stream_seq)
{
    SCOPED_TRACE("stream_seq");
    std::string yaml = "--- [a, b]\n--- [c, d]\n--- [e, f]\n";
    std::string exp = "--- [a,b]\n--- [c,d]\n--- [e,f]\n";
    std::string expjson = "[\n  [\"a\",\"b\"],\n  [\"c\",\"d\"],\n  [\"e\",\"f\"]\n]\n";
    TreeAndInts ti(to_csubstr(yaml));
    {
        SCOPED_TRACE("orig");
        test_emits_(ti, exp, expjson);
        test_emits_tree_(ti.tree.docref(0), "--- [a,b]", "[\"a\",\"b\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 1, "--- [a,b]\n", "[\"a\",\"b\"]");
        test_emits_ints_(ti.ints, 2, "[a,b]", "[\"a\",\"b\"]");
        test_emits_tree_(ti.tree.docref(1), "--- [c,d]", "[\"c\",\"d\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 11, "--- [c,d]\n", "[\"c\",\"d\"]");
        test_emits_ints_(ti.ints, 12, "[c,d]", "[\"c\",\"d\"]");
        test_emits_tree_(ti.tree.docref(2), "--- [e,f]", "[\"e\",\"f\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 21, "--- [e,f]\n", "[\"e\",\"f\"]");
        test_emits_ints_(ti.ints, 22, "[e,f]", "[\"e\",\"f\"]");
    }
    {
        SCOPED_TRACE("flow spc");
        test_emits_(ti, exp, expjson);
        test_emits_tree_(ti.tree.docref(0), "--- [a,b]", "[\"a\",\"b\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 1, "--- [a,b]\n", "[\"a\",\"b\"]");
        test_emits_ints_(ti.ints, 2, "[a,b]", "[\"a\",\"b\"]");
        test_emits_tree_(ti.tree.docref(1), "--- [c,d]", "[\"c\",\"d\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 11, "--- [c,d]\n", "[\"c\",\"d\"]");
        test_emits_ints_(ti.ints, 12, "[c,d]", "[\"c\",\"d\"]");
        test_emits_tree_(ti.tree.docref(2), "--- [e,f]", "[\"e\",\"f\"]"); // FIXME newline
        test_emits_ints_(ti.ints, 21, "--- [e,f]\n", "[\"e\",\"f\"]");
        test_emits_ints_(ti.ints, 22, "[e,f]", "[\"e\",\"f\"]");
    }
    {
        SCOPED_TRACE("block");
        TMPSTY(blk, ti, ti.tree.docref(0), 2);
        TMPSTY(blk, ti, ti.tree.docref(1), 12);
        TMPSTY(blk, ti, ti.tree.docref(2), 22);
        test_emits_(ti, "---\n- a\n- b\n---\n- c\n- d\n---\n- e\n- f\n",
                    "[\n  [\n    \"a\",\n    \"b\"\n  ],\n  [\n    \"c\",\n    \"d\"\n  ],\n  [\n    \"e\",\n    \"f\"\n  ]\n]\n");
        test_emits_tree_(ti.tree.docref(0), "---\n- a\n- b\n", "[\n  \"a\",\n  \"b\"\n]\n");
        test_emits_ints_(ti.ints, 1, "---\n- a\n- b\n", "[\n  \"a\",\n  \"b\"\n]"); // FIXME newline
        test_emits_ints_(ti.ints, 2, "- a\n- b\n", "[\n  \"a\",\n  \"b\"\n]\n");
        test_emits_tree_(ti.tree.docref(1), "---\n- c\n- d\n", "[\n  \"c\",\n  \"d\"\n]\n");
        test_emits_ints_(ti.ints, 11, "---\n- c\n- d\n", "[\n  \"c\",\n  \"d\"\n]"); // FIXME newline
        test_emits_ints_(ti.ints, 12, "- c\n- d\n", "[\n  \"c\",\n  \"d\"\n]\n");
        test_emits_tree_(ti.tree.docref(2), "---\n- e\n- f\n", "[\n  \"e\",\n  \"f\"\n]\n");
        test_emits_ints_(ti.ints, 21, "---\n- e\n- f\n", "[\n  \"e\",\n  \"f\"\n]"); // FIXME newline
        test_emits_ints_(ti.ints, 22, "- e\n- f\n", "[\n  \"e\",\n  \"f\"\n]\n");
    }
}

TEST(emit, stream_map)
{
    SCOPED_TRACE("stream_map");
    std::string yaml = "--- {a: b}\n--- {c: d}\n--- {e: f}\n";
    std::string expjson = "[\n  {\"a\": \"b\"},\n  {\"c\": \"d\"},\n  {\"e\": \"f\"}\n]\n";
    TreeAndInts ti(to_csubstr(yaml));
    test_emits_(ti, yaml, expjson);
    {
        SCOPED_TRACE("orig");
        test_emits_tree_(ti.tree.docref(0), "--- {a: b}", "{\"a\": \"b\"}"); // FIXME newline
        test_emits_ints_(ti.ints, 1, "--- {a: b}\n", "{\"a\": \"b\"}");
        test_emits_ints_(ti.ints, 2, "{a: b}", "{\"a\": \"b\"}");
        test_emits_tree_(ti.tree.docref(1), "--- {c: d}", "{\"c\": \"d\"}"); // FIXME newline
        test_emits_ints_(ti.ints, 11, "--- {c: d}\n", "{\"c\": \"d\"}");
        test_emits_ints_(ti.ints, 12, "{c: d}", "{\"c\": \"d\"}");
        test_emits_tree_(ti.tree.docref(2), "--- {e: f}", "{\"e\": \"f\"}"); // FIXME newline
        test_emits_ints_(ti.ints, 21, "--- {e: f}\n", "{\"e\": \"f\"}");
        test_emits_ints_(ti.ints, 22, "{e: f}", "{\"e\": \"f\"}");
    }
    {
        SCOPED_TRACE("block");
        TMPSTY(blk, ti, ti.tree.docref(0), 2);
        TMPSTY(blk, ti, ti.tree.docref(1), 12);
        TMPSTY(blk, ti, ti.tree.docref(2), 22);
        test_emits_(ti, "---\na: b\n---\nc: d\n---\ne: f\n",
                    "[\n  {\n    \"a\": \"b\"\n  },\n  {\n    \"c\": \"d\"\n  },\n  {\n    \"e\": \"f\"\n  }\n]\n");
        test_emits_tree_(ti.tree.docref(0), "---\na: b\n", "{\n  \"a\": \"b\"\n}\n");
        test_emits_ints_(ti.ints, 1, "---\na: b\n", "{\n  \"a\": \"b\"\n}"); // FIXME newline
        test_emits_ints_(ti.ints, 2, "a: b\n", "{\n  \"a\": \"b\"\n}\n");
        test_emits_tree_(ti.tree.docref(1), "---\nc: d\n", "{\n  \"c\": \"d\"\n}\n");
        test_emits_ints_(ti.ints, 11, "---\nc: d\n", "{\n  \"c\": \"d\"\n}"); // FIXME newline
        test_emits_ints_(ti.ints, 12, "c: d\n", "{\n  \"c\": \"d\"\n}\n");
        test_emits_tree_(ti.tree.docref(2), "---\ne: f\n", "{\n  \"e\": \"f\"\n}\n");
        test_emits_ints_(ti.ints, 21, "---\ne: f\n", "{\n  \"e\": \"f\"\n}"); // FIXME newline
        test_emits_ints_(ti.ints, 22, "e: f\n", "{\n  \"e\": \"f\"\n}\n");
    }
}


TEST(emit, nested_map_sl)
{
    SCOPED_TRACE("nested_map_sl");
    std::string yaml = "{a: {b: {c: {d: e}}}}";
    std::string json = "{\"a\": {\"b\": {\"c\": {\"d\": \"e\"}}}}";
    TreeAndInts ti(to_csubstr(yaml));
    test_emits_(ti, yaml, json);
}


TEST(emit, anchor_ref)
{
    SCOPED_TRACE("anchor_ref");
    TreeAndInts ti("[&a a,*a]");
    auto dotest = [&](std::string const& expected, std::string const& expected_json){
        test_emits_(ti, ti.tree, 0, expected, expected_json);
        test_emits_(ti, ti.tree, 1, expected, expected_json);
        test_emits_(ti, ti.tree, 2, expected, expected_json);
        test_emits_(ti, ti.tree[0], 3, "&a a", "\"a\"");
        test_emits_ints_(ti.ints, 6, "a", "\"a\"");
        test_emits_(ti, ti.tree[1], 9, "*a", "\"*a\"");
    };
    {
        TMPSTY(flowml1, ti, ti.tree, 2);
        dotest("[\n  &a a,\n  *a\n]\n", "[\n  \"a\",\n  \"*a\"\n]\n");
    }
    {
        TMPSTY(flowsl, ti, ti.tree, 2);
        dotest("[&a a,*a]", "[\"a\",\"*a\"]");
    }
}


TEST(emit, anchor_ref_err_json)
{
    SCOPED_TRACE("anchor_ref_err_json");
    const EmitOptions opts = EmitOptions{}.json_err_on_anchor(true);
    {
        SCOPED_TRACE("seq");
        TreeAndInts ti("[*a, *b]");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("flowsl");
            TMPSTY(flowsl, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("flowml1");
            TMPSTY(flowml1, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("nested a");
            SCOPED_CALL(test_emit_error_json, (ti.tree[0], ti.ints, 2, opts));
        }
        {
            SCOPED_TRACE("nested b");
            SCOPED_CALL(test_emit_error_json, (ti.tree[1], ti.ints, 5, opts));
        }
    }
    {
        SCOPED_TRACE("map");
        TreeAndInts ti("{*a : *b}");
        {
            SCOPED_TRACE("block");
            TMPSTY(blk, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("flowsl");
            TMPSTY(flowsl, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("flowml1");
            TMPSTY(flowml1, ti, ti.tree, 2);
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("nested a");
            SCOPED_CALL(test_emit_error_json, (ti.tree[0], ti.ints, 2, opts));
        }
        {
            SCOPED_TRACE("nested b");
            SCOPED_CALL(test_emit_error_json, (ti.tree[1], ti.ints, 5, opts));
        }
    }
    {
        TreeAndInts ti("*a");
        {
            SCOPED_TRACE("docval");
            SCOPED_CALL(test_emit_error_json, (ti, opts));
        }
        {
            SCOPED_TRACE("nested");
            SCOPED_CALL(test_emit_error_json, (ti.tree.rootref(), ti.ints, 2, opts));
        }
    }
}


TEST(emit, tag_anchor)
{
    SCOPED_TRACE("tag_anchor_ml");
    TreeAndInts ti(R"({
  !kt a: !vt 8,
  &ka b: &va 9,
  &ka !kt c: &va !vt 10,
  !kt &ka d: !vt &va 11,
})");
    auto dotest = [&](std::string const& expected, std::string const& expected_json){
        test_emits_(ti, ti.tree, 0, expected, expected_json);
        test_emits_(ti, ti.tree, 1, expected, expected_json);
        test_emits_(ti, ti.tree, 2, expected, expected_json);
        //
        test_emits_(ti, ti.tree[0], 3, "!kt a: !vt 8\n", "\"a\": 8\n");
        test_emits_(ti.tree[0], "!vt 8", "8", without_key);
        test_emits_ints_(ti.ints, 3, "!kt a", "\"a\"", without_key);
        test_emits_ints_(ti.ints, 6, "a: !vt 8\n", "\"a\": 8\n");
        test_emits_ints_(ti.ints, 6, "a", "\"a\"", without_key);
        test_emits_ints_(ti.ints, 9, "!vt 8", "8");
        test_emits_ints_(ti.ints, 9, "!vt 8", "8", without_key);
        test_emits_ints_(ti.ints, 12, "8", "8");
        test_emits_ints_(ti.ints, 12, "8", "8", without_key);
        //
        test_emits_(ti, ti.tree[1], 15, "&ka b: &va 9\n", "\"b\": 9\n");
        test_emits_(ti.tree[1], "&va 9", "9", without_key);
        test_emits_ints_(ti.ints, 15, "&ka b", "\"b\"", without_key);
        test_emits_ints_(ti.ints, 18, "b: &va 9\n", "\"b\": 9\n");
        test_emits_ints_(ti.ints, 18, "b", "\"b\"", without_key);
        test_emits_ints_(ti.ints, 21, "&va 9", "9");
        test_emits_ints_(ti.ints, 21, "&va 9", "9", without_key);
        test_emits_ints_(ti.ints, 24, "9", "9");
        test_emits_ints_(ti.ints, 24, "9", "9", without_key);
        //
        test_emits_(ti, ti.tree[2], 27, "&ka !kt c: &va !vt 10\n", "\"c\": 10\n");
        test_emits_(ti.tree[2], "&va !vt 10", "10", without_key);
        test_emits_ints_(ti.ints, 27, "&ka !kt c", "\"c\"", without_key);
        test_emits_ints_(ti.ints, 30, "!kt c: &va !vt 10\n", "\"c\": 10\n");
        test_emits_ints_(ti.ints, 30, "!kt c", "\"c\"", without_key);
        test_emits_ints_(ti.ints, 33, "c: &va !vt 10\n", "\"c\": 10\n");
        test_emits_ints_(ti.ints, 33, "c", "\"c\"", without_key);
        test_emits_ints_(ti.ints, 36, "&va !vt 10", "10");
        test_emits_ints_(ti.ints, 36, "&va !vt 10", "10", without_key);
        test_emits_ints_(ti.ints, 39, "!vt 10", "10");
        test_emits_ints_(ti.ints, 39, "!vt 10", "10", without_key);
        test_emits_ints_(ti.ints, 42, "10", "10");
        test_emits_ints_(ti.ints, 42, "10", "10", without_key);
        //
        test_emits_(ti, ti.tree[3], 45, "&ka !kt d: &va !vt 11\n", "\"d\": 11\n");
        test_emits_(ti.tree[3], "&va !vt 11", "11", without_key);
        test_emits_ints_(ti.ints, 45, "&ka !kt d", "\"d\"", without_key);
        test_emits_ints_(ti.ints, 48, "!kt d: &va !vt 11\n", "\"d\": 11\n");
        test_emits_ints_(ti.ints, 48, "!kt d", "\"d\"", without_key);
        test_emits_ints_(ti.ints, 51, "d: &va !vt 11\n", "\"d\": 11\n");
        test_emits_ints_(ti.ints, 51, "d", "\"d\"", without_key);
        test_emits_ints_(ti.ints, 54, "&va !vt 11", "11");
        test_emits_ints_(ti.ints, 54, "&va !vt 11", "11", without_key);
        test_emits_ints_(ti.ints, 57, "!vt 11", "11");
        test_emits_ints_(ti.ints, 57, "!vt 11", "11", without_key);
        test_emits_ints_(ti.ints, 60, "11", "11");
        test_emits_ints_(ti.ints, 60, "11", "11", without_key);
    };
    {
        TMPSTY(flowml1, ti, ti.tree, 2);
        dotest("{\n  !kt a: !vt 8,\n  &ka b: &va 9,\n  &ka !kt c: &va !vt 10,\n  &ka !kt d: &va !vt 11\n}\n",
               "{\n  \"a\": 8,\n  \"b\": 9,\n  \"c\": 10,\n  \"d\": 11\n}\n");
    }
    {
        TMPSTY(flowsl, ti, ti.tree, 2);
        dotest(R"({!kt a: !vt 8,&ka b: &va 9,&ka !kt c: &va !vt 10,&ka !kt d: &va !vt 11})",
               R"({"a": 8,"b": 9,"c": 10,"d": 11})");
    }
    {
        SCOPED_TRACE("json err on tag");
        const EmitOptions opts = EmitOptions{}.json_err_on_tag(true);
        {
            SCOPED_TRACE("flowsl");
            TMPSTY(flowsl, ti, ti.tree, 2);
            test_emit_error_json_(ti, opts);
        }
        {
            SCOPED_TRACE("flowml1");
            TMPSTY(flowml1, ti, ti.tree, 2);
            test_emit_error_json_(ti, opts);
        }
    }
    {
        SCOPED_TRACE("json err on anchor");
        const EmitOptions opts = EmitOptions{}.json_err_on_anchor(true);
        {
            SCOPED_TRACE("flowsl");
            TMPSTY(flowsl, ti, ti.tree, 2);
            test_emit_error_json_(ti, opts);
        }
        {
            SCOPED_TRACE("flowml1");
            TMPSTY(flowml1, ti, ti.tree, 2);
            test_emit_error_json_(ti, opts);
        }
    }
}


TEST(emit, container_key_tag_anchor)
{
    SCOPED_TRACE("container_key_tag_anchor");
    IntBufs ints;
    std::string yaml = R"({
  !kt []: !vt 0,
  &ka []: &va 1,
  &ka !kt []: &va !vt 2,
  !kt &ka []: !vt &va 3,
  !kt {}: !vt 4,
  &ka {}: &va 5,
  &ka !kt {}: &va !vt 6,
  !kt &ka {}: !vt &va 7,
  !kt a: !vt 8,
  &ka b: &va 9,
  &ka !kt c: &va !vt 10,
  !kt &ka d: !vt &va 11,
})";
    parse_ints(to_substr(yaml), &ints);
    {
        SCOPED_TRACE("orig");
        std::string expected = R"({
  ? !kt []: !vt 0,
  ? &ka []: &va 1,
  ? &ka !kt []: &va !vt 2,
  ? &ka !kt []: &va !vt 3,
  ? !kt {}: !vt 4,
  ? &ka {}: &va 5,
  ? &ka !kt {}: &va !vt 6,
  ? &ka !kt {}: &va !vt 7,
  !kt a: !vt 8,
  &ka b: &va 9,
  &ka !kt c: &va !vt 10,
  &ka !kt d: &va !vt 11
}
)";
        test_emits_ints_nojson_(ints, 0, expected);
        test_emits_ints_nojson_(ints, 1, expected);
        test_emits_ints_nojson_(ints, 2, expected);
        test_emits_ints_nojson_(ints, 3, "? !kt []\n: !vt 0\n");
        test_emits_ints_nojson_(ints, 3, "!kt []", without_key);
        test_emits_ints_nojson_(ints, 6, "? []\n: !vt 0\n");
        test_emits_ints_nojson_(ints, 6, "[]", without_key);
        test_emits_ints_nojson_(ints, 8, "!vt 0");
        test_emits_ints_nojson_(ints, 8, "!vt 0", without_key);
        test_emits_ints_nojson_(ints, 11, "0");
        test_emits_ints_nojson_(ints, 11, "0", without_key);
        {
            SCOPED_TRACE("nosty");
            TMPSTY(nosty, ints, 6);
            test_emits_ints_nojson_(ints, 3, "? !kt\n  []\n: !vt 0\n");
            test_emits_ints_nojson_(ints, 3, "!kt\n[]", without_key);
            test_emits_ints_nojson_(ints, 6, "?\n  []\n: !vt 0\n");
            test_emits_ints_nojson_(ints, 6, "[]", without_key);
        }
        //
        test_emits_ints_nojson_(ints, 14, "? &ka []\n: &va 1\n");
        test_emits_ints_nojson_(ints, 14, "&ka []", without_key);
        test_emits_ints_nojson_(ints, 17, "? []\n: &va 1\n");
        test_emits_ints_nojson_(ints, 17, "[]", without_key);
        test_emits_ints_nojson_(ints, 19, "&va 1");
        test_emits_ints_nojson_(ints, 19, "&va 1", without_key);
        test_emits_ints_nojson_(ints, 22, "1");
        test_emits_ints_nojson_(ints, 22, "1", without_key);
        {
            SCOPED_TRACE("nosty");
            TMPSTY(nosty, ints, 17);
            test_emits_ints_nojson_(ints, 14, "? &ka\n  []\n: &va 1\n");
            test_emits_ints_nojson_(ints, 14, "&ka\n[]", without_key);
            test_emits_ints_nojson_(ints, 17, "?\n  []\n: &va 1\n");
            test_emits_ints_nojson_(ints, 17, "[]", without_key);
        }
        //
        test_emits_ints_nojson_(ints, 25, "? &ka !kt []\n: &va !vt 2\n");
        test_emits_ints_nojson_(ints, 25, "&ka !kt []", without_key);
        test_emits_ints_nojson_(ints, 28, "? !kt []\n: &va !vt 2\n");
        test_emits_ints_nojson_(ints, 28, "!kt []", without_key);
        test_emits_ints_nojson_(ints, 31, "? []\n: &va !vt 2\n");
        test_emits_ints_nojson_(ints, 31, "[]", without_key);
        test_emits_ints_nojson_(ints, 33, "&va !vt 2");
        test_emits_ints_nojson_(ints, 33, "&va !vt 2", without_key);
        test_emits_ints_nojson_(ints, 36, "!vt 2");
        test_emits_ints_nojson_(ints, 36, "!vt 2", without_key);
        test_emits_ints_nojson_(ints, 39, "2");
        test_emits_ints_nojson_(ints, 39, "2", without_key);
        //
        test_emits_ints_nojson_(ints, 42, "? &ka !kt []\n: &va !vt 3\n");
        test_emits_ints_nojson_(ints, 42, "&ka !kt []", without_key);
        test_emits_ints_nojson_(ints, 45, "? !kt []\n: &va !vt 3\n");
        test_emits_ints_nojson_(ints, 45, "!kt []", without_key);
        test_emits_ints_nojson_(ints, 48, "? []\n: &va !vt 3\n");
        test_emits_ints_nojson_(ints, 48, "[]", without_key);
        test_emits_ints_nojson_(ints, 50, "&va !vt 3");
        test_emits_ints_nojson_(ints, 50, "&va !vt 3", without_key);
        test_emits_ints_nojson_(ints, 53, "!vt 3");
        test_emits_ints_nojson_(ints, 53, "!vt 3", without_key);
        test_emits_ints_nojson_(ints, 56, "3");
        test_emits_ints_nojson_(ints, 56, "3", without_key);
        //
        //
        test_emits_ints_nojson_(ints, 56+3, "? !kt {}\n: !vt 4\n");
        test_emits_ints_nojson_(ints, 56+3, "!kt {}", without_key);
        test_emits_ints_nojson_(ints, 56+6, "? {}\n: !vt 4\n");
        test_emits_ints_nojson_(ints, 56+6, "{}", without_key);
        test_emits_ints_nojson_(ints, 56+8, "!vt 4");
        test_emits_ints_nojson_(ints, 56+8, "!vt 4", without_key);
        test_emits_ints_nojson_(ints, 56+11, "4");
        test_emits_ints_nojson_(ints, 56+11, "4", without_key);
        //
        test_emits_ints_nojson_(ints, 56+14, "? &ka {}\n: &va 5\n");
        test_emits_ints_nojson_(ints, 56+14, "&ka {}", without_key);
        test_emits_ints_nojson_(ints, 56+17, "? {}\n: &va 5\n");
        test_emits_ints_nojson_(ints, 56+17, "{}", without_key);
        test_emits_ints_nojson_(ints, 56+19, "&va 5");
        test_emits_ints_nojson_(ints, 56+19, "&va 5", without_key);
        test_emits_ints_nojson_(ints, 56+22, "5");
        test_emits_ints_nojson_(ints, 56+22, "5", without_key);
        //
        test_emits_ints_nojson_(ints, 56+25, "? &ka !kt {}\n: &va !vt 6\n");
        test_emits_ints_nojson_(ints, 56+25, "&ka !kt {}", without_key);
        test_emits_ints_nojson_(ints, 56+28, "? !kt {}\n: &va !vt 6\n");
        test_emits_ints_nojson_(ints, 56+28, "!kt {}", without_key);
        test_emits_ints_nojson_(ints, 56+31, "? {}\n: &va !vt 6\n");
        test_emits_ints_nojson_(ints, 56+31, "{}", without_key);
        test_emits_ints_nojson_(ints, 56+33, "&va !vt 6");
        test_emits_ints_nojson_(ints, 56+33, "&va !vt 6", without_key);
        test_emits_ints_nojson_(ints, 56+36, "!vt 6");
        test_emits_ints_nojson_(ints, 56+36, "!vt 6", without_key);
        test_emits_ints_nojson_(ints, 56+39, "6");
        test_emits_ints_nojson_(ints, 56+39, "6", without_key);
        //
        test_emits_ints_nojson_(ints, 56+42, "? &ka !kt {}\n: &va !vt 7\n");
        test_emits_ints_nojson_(ints, 56+42, "&ka !kt {}", without_key);
        test_emits_ints_nojson_(ints, 56+45, "? !kt {}\n: &va !vt 7\n");
        test_emits_ints_nojson_(ints, 56+45, "!kt {}", without_key);
        test_emits_ints_nojson_(ints, 56+48, "? {}\n: &va !vt 7\n");
        test_emits_ints_nojson_(ints, 56+48, "{}", without_key);
        test_emits_ints_nojson_(ints, 56+50, "&va !vt 7");
        test_emits_ints_nojson_(ints, 56+50, "&va !vt 7", without_key);
        test_emits_ints_nojson_(ints, 56+53, "!vt 7");
        test_emits_ints_nojson_(ints, 56+53, "!vt 7", without_key);
        test_emits_ints_nojson_(ints, 56+56, "7");
        test_emits_ints_nojson_(ints, 56+56, "7", without_key);
        //
        //
        test_emits_ints_nojson_(ints, 112+3, "!kt a: !vt 8\n");
        test_emits_ints_nojson_(ints, 112+3, "!kt a", without_key);
        test_emits_ints_nojson_(ints, 112+6, "a: !vt 8\n");
        test_emits_ints_nojson_(ints, 112+6, "a", without_key);
        test_emits_ints_nojson_(ints, 112+9, "!vt 8");
        test_emits_ints_nojson_(ints, 112+9, "!vt 8", without_key);
        test_emits_ints_nojson_(ints, 112+12, "8");
        test_emits_ints_nojson_(ints, 112+12, "8", without_key);
        //
        test_emits_ints_nojson_(ints, 112+15, "&ka b: &va 9\n");
        test_emits_ints_nojson_(ints, 112+15, "&ka b", without_key);
        test_emits_ints_nojson_(ints, 112+18, "b: &va 9\n");
        test_emits_ints_nojson_(ints, 112+18, "b", without_key);
        test_emits_ints_nojson_(ints, 112+21, "&va 9");
        test_emits_ints_nojson_(ints, 112+21, "&va 9", without_key);
        test_emits_ints_nojson_(ints, 112+24, "9");
        test_emits_ints_nojson_(ints, 112+24, "9", without_key);
        //
        test_emits_ints_nojson_(ints, 112+27, "&ka !kt c: &va !vt 10\n");
        test_emits_ints_nojson_(ints, 112+27, "&ka !kt c", without_key);
        test_emits_ints_nojson_(ints, 112+30, "!kt c: &va !vt 10\n");
        test_emits_ints_nojson_(ints, 112+30, "!kt c", without_key);
        test_emits_ints_nojson_(ints, 112+33, "c: &va !vt 10\n");
        test_emits_ints_nojson_(ints, 112+33, "c", without_key);
        test_emits_ints_nojson_(ints, 112+36, "&va !vt 10");
        test_emits_ints_nojson_(ints, 112+36, "&va !vt 10", without_key);
        test_emits_ints_nojson_(ints, 112+39, "!vt 10");
        test_emits_ints_nojson_(ints, 112+39, "!vt 10", without_key);
        test_emits_ints_nojson_(ints, 112+42, "10");
        test_emits_ints_nojson_(ints, 112+42, "10", without_key);
        //
        test_emits_ints_nojson_(ints, 112+45, "&ka !kt d: &va !vt 11\n");
        test_emits_ints_nojson_(ints, 112+45, "&ka !kt d", without_key);
        test_emits_ints_nojson_(ints, 112+48, "!kt d: &va !vt 11\n");
        test_emits_ints_nojson_(ints, 112+48, "!kt d", without_key);
        test_emits_ints_nojson_(ints, 112+51, "d: &va !vt 11\n");
        test_emits_ints_nojson_(ints, 112+51, "d", without_key);
        test_emits_ints_nojson_(ints, 112+54, "&va !vt 11");
        test_emits_ints_nojson_(ints, 112+54, "&va !vt 11", without_key);
        test_emits_ints_nojson_(ints, 112+57, "!vt 11");
        test_emits_ints_nojson_(ints, 112+57, "!vt 11", without_key);
        test_emits_ints_nojson_(ints, 112+60, "11");
        test_emits_ints_nojson_(ints, 112+60, "11", without_key);
    }
}

TEST(emit, blck_seq_nosty)
{
    SCOPED_TRACE("blck_seq_nosty");
    std::string yaml = R"(- !str scalar
- &str scalar
- !map {a: b}
- &map {c: d}
- !seq [e,f]
- &seq [g,h]
)";
    TreeAndInts ti(to_csubstr(yaml));
    test_emit_yaml_tree_(ti.tree, yaml);
    test_emit_yaml_ints_(ti.ints, 0, yaml);
    test_emit_yaml_ints_(ti.ints, 1, yaml);
    test_emit_yaml_ints_(ti.ints, 2, yaml);
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[0], 6);
        TMPSTY(nosty, ti, ti.tree[1], 12);
        TMPSTY(nosty, ti, ti.tree[2], 18);
        TMPSTY(nosty, ti, ti.tree[3], 29);
        TMPSTY(nosty, ti, ti.tree[4], 40);
        TMPSTY(nosty, ti, ti.tree[5], 51);
        std::string expected = R"(- !str scalar
- &str scalar
- !map
  a: b
- &map
  c: d
- !seq
  - e
  - f
- &seq
  - g
  - h
)";
        test_emit_yaml_tree_(ti.tree, expected);
        test_emit_yaml_ints_(ti.ints, 0, expected);
        test_emit_yaml_ints_(ti.ints, 1, expected);
        test_emit_yaml_ints_(ti.ints, 2, expected);
    }

    test_emit_yaml_tree_(ti.tree[0], without_dash, "!str scalar");
    test_emit_yaml_tree_(ti.tree[0], with_dash, "- !str scalar\n");
    test_emit_yaml_ints_(ti.ints, 3, without_dash, "!str scalar");
    test_emit_yaml_ints_(ti.ints, 3, with_dash, "- !str scalar\n");
    test_emit_yaml_ints_(ti.ints, 6, without_dash, "scalar");
    test_emit_yaml_ints_(ti.ints, 6, with_dash, "- scalar\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[0], 6);
        test_emit_yaml_tree_(ti.tree[0], without_dash, "!str scalar");
        test_emit_yaml_tree_(ti.tree[0], with_dash, "- !str scalar\n");
        test_emit_yaml_ints_(ti.ints, 3, without_dash, "!str scalar");
        test_emit_yaml_ints_(ti.ints, 3, with_dash, "- !str scalar\n");
        test_emit_yaml_ints_(ti.ints, 6, without_dash, "scalar");
        test_emit_yaml_ints_(ti.ints, 6, with_dash, "- scalar\n");
    }

    test_emit_yaml_tree_(ti.tree[1], without_dash, "&str scalar");
    test_emit_yaml_tree_(ti.tree[1], with_dash, "- &str scalar\n");
    test_emit_yaml_ints_(ti.ints, 9, without_dash, "&str scalar");
    test_emit_yaml_ints_(ti.ints, 9, with_dash, "- &str scalar\n");
    test_emit_yaml_ints_(ti.ints, 12, without_dash, "scalar");
    test_emit_yaml_ints_(ti.ints, 12, with_dash, "- scalar\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[1], 12);
        test_emit_yaml_tree_(ti.tree[1], without_dash, "&str scalar");
        test_emit_yaml_tree_(ti.tree[1], with_dash, "- &str scalar\n");
        test_emit_yaml_ints_(ti.ints, 9, without_dash, "&str scalar");
        test_emit_yaml_ints_(ti.ints, 9, with_dash, "- &str scalar\n");
        test_emit_yaml_ints_(ti.ints, 12, without_dash, "scalar");
        test_emit_yaml_ints_(ti.ints, 12, with_dash, "- scalar\n");
    }

    test_emit_yaml_tree_(ti.tree[2], without_dash, "!map {a: b}");
    test_emit_yaml_tree_(ti.tree[2], with_dash, "- !map {a: b}\n");
    test_emit_yaml_ints_(ti.ints, 15, without_dash, "!map {a: b}");
    test_emit_yaml_ints_(ti.ints, 15, with_dash, "- !map {a: b}\n");
    test_emit_yaml_ints_(ti.ints, 18, without_dash, "{a: b}");
    test_emit_yaml_ints_(ti.ints, 18, with_dash, "- {a: b}\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[2], 18);
        test_emit_yaml_tree_(ti.tree[2], without_dash, "!map\na: b\n");
        test_emit_yaml_tree_(ti.tree[2], with_dash, "- !map\n  a: b\n");
        test_emit_yaml_ints_(ti.ints, 15, without_dash, "!map\na: b\n");
        test_emit_yaml_ints_(ti.ints, 15, with_dash, "- !map\n  a: b\n");
        test_emit_yaml_ints_(ti.ints, 18, without_dash, "a: b\n");
        test_emit_yaml_ints_(ti.ints, 18, with_dash, "- a: b\n");
    }

    test_emit_yaml_tree_(ti.tree[3], without_dash, "&map {c: d}");
    test_emit_yaml_tree_(ti.tree[3], with_dash, "- &map {c: d}\n");
    test_emit_yaml_ints_(ti.ints, 26, without_dash, "&map {c: d}");
    test_emit_yaml_ints_(ti.ints, 26, with_dash, "- &map {c: d}\n");
    test_emit_yaml_ints_(ti.ints, 29, without_dash, "{c: d}");
    test_emit_yaml_ints_(ti.ints, 29, with_dash, "- {c: d}\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[3], 29);
        test_emit_yaml_tree_(ti.tree[3], without_dash, "&map\nc: d\n");
        test_emit_yaml_tree_(ti.tree[3], with_dash, "- &map\n  c: d\n");
        test_emit_yaml_ints_(ti.ints, 26, without_dash, "&map\nc: d\n");
        test_emit_yaml_ints_(ti.ints, 26, with_dash, "- &map\n  c: d\n");
        test_emit_yaml_ints_(ti.ints, 29, without_dash, "c: d\n");
        test_emit_yaml_ints_(ti.ints, 29, with_dash, "- c: d\n");
    }

    test_emit_yaml_tree_(ti.tree[4], without_dash, "!seq [e,f]");
    test_emit_yaml_tree_(ti.tree[4], with_dash, "- !seq [e,f]\n");
    test_emit_yaml_ints_(ti.ints, 37, without_dash, "!seq [e,f]");
    test_emit_yaml_ints_(ti.ints, 37, with_dash, "- !seq [e,f]\n");
    test_emit_yaml_ints_(ti.ints, 40, without_dash, "[e,f]");
    test_emit_yaml_ints_(ti.ints, 40, with_dash, "- [e,f]\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[4], 40);
        test_emit_yaml_tree_(ti.tree[4], without_dash, "!seq\n- e\n- f\n");
        test_emit_yaml_tree_(ti.tree[4], with_dash, "- !seq\n  - e\n  - f\n");
        test_emit_yaml_ints_(ti.ints, 37, without_dash, "!seq\n- e\n- f\n");
        test_emit_yaml_ints_(ti.ints, 37, with_dash, "- !seq\n  - e\n  - f\n");
        test_emit_yaml_ints_(ti.ints, 40, without_dash, "- e\n- f\n");
        test_emit_yaml_ints_(ti.ints, 40, with_dash, "- - e\n  - f\n");
    }

    test_emit_yaml_tree_(ti.tree[5], without_dash, "&seq [g,h]");
    test_emit_yaml_tree_(ti.tree[5], with_dash, "- &seq [g,h]\n");
    test_emit_yaml_ints_(ti.ints, 48, without_dash, "&seq [g,h]");
    test_emit_yaml_ints_(ti.ints, 48, with_dash, "- &seq [g,h]\n");
    test_emit_yaml_ints_(ti.ints, 51, without_dash, "[g,h]");
    test_emit_yaml_ints_(ti.ints, 51, with_dash, "- [g,h]\n");
    {
        SCOPED_TRACE("nosty");
        TMPSTY(nosty, ti, ti.tree[5], 51);
        test_emit_yaml_tree_(ti.tree[5], without_dash, "&seq\n- g\n- h\n");
        test_emit_yaml_tree_(ti.tree[5], with_dash, "- &seq\n  - g\n  - h\n");
        test_emit_yaml_ints_(ti.ints, 48, without_dash, "&seq\n- g\n- h\n");
        test_emit_yaml_ints_(ti.ints, 48, with_dash, "- &seq\n  - g\n  - h\n");
        test_emit_yaml_ints_(ti.ints, 51, without_dash, "- g\n- h\n");
        test_emit_yaml_ints_(ti.ints, 51, with_dash, "- - g\n  - h\n");
    }
}


//-----------------------------------------------------------------------------

TEST(emit, percent_is_quoted)
{
    Tree ti = parse_in_arena("{}");
    ASSERT_TRUE(ti.rootref().is_map());
    ti["%ROOT"].set_val("%VAL");
    ti["%ROOT2"].set_seq();
    ti["%ROOT2"][0].set_val("%VAL");
    ti["%ROOT2"][1].set_val("%VAL");
    std::string yaml = emitrs_yaml<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("%ROOT"));
        ASSERT_TRUE(t.rootref().has_child("%ROOT2"));
        ASSERT_EQ(t["%ROOT2"].num_children(), 2u);
        EXPECT_TRUE(t["%ROOT"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT"].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"].is_key_quoted());
        EXPECT_TRUE(t["%ROOT2"][0].is_val_quoted());
        EXPECT_TRUE(t["%ROOT2"][1].is_val_quoted());
    });
}

TEST(emit, at_is_quoted__issue_309)
{
    auto check = [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("at"));
        ASSERT_TRUE(t.rootref().has_child("backtick"));
        ASSERT_EQ(t["at"].num_children(), 4u);
        ASSERT_EQ(t["backtick"].num_children(), 4u);
        EXPECT_EQ(t["at"][0].val(), "@test");
        EXPECT_EQ(t["at"][1].val(), "@test2");
        EXPECT_EQ(t["at"][2].val(), "@");
        EXPECT_EQ(t["at"][3].val(), "@");
        EXPECT_TRUE(t["at"][0].is_val_quoted());
        EXPECT_TRUE(t["at"][1].is_val_quoted());
        EXPECT_TRUE(t["at"][2].is_val_quoted());
        EXPECT_TRUE(t["at"][3].is_val_quoted());
        EXPECT_EQ(t["backtick"][0].val(), "`test");
        EXPECT_EQ(t["backtick"][1].val(), "`test2");
        EXPECT_EQ(t["backtick"][2].val(), "`");
        EXPECT_EQ(t["backtick"][3].val(), "`");
        EXPECT_TRUE(t["backtick"][0].is_val_quoted());
        EXPECT_TRUE(t["backtick"][1].is_val_quoted());
        EXPECT_TRUE(t["backtick"][2].is_val_quoted());
        EXPECT_TRUE(t["backtick"][3].is_val_quoted());
    };
    {
        Tree ti = parse_in_arena("{at: [], backtick: []}");
        ti["at"][0].save("@test");
        ti["at"][1].set_val("@test2");
        ti["at"][2].save("@");
        ti["at"][3].set_val("@");
        ti["backtick"][0].save("`test");
        ti["backtick"][1].set_val("`test2");
        ti["backtick"][2].save("`");
        ti["backtick"][3].set_val("`");
        std::string yaml = emitrs_yaml<std::string>(ti);
        test_check_emit_check(to_csubstr(yaml), check);
    }
    {
        Tree ti = parse_in_arena("{at: [], backtick: []}");
        ti.set_serialized(ti["at"].append_child().id(),  "@test");
        ti.set_val       (ti["at"].append_child().id(), "@test2");
        ti.set_serialized(ti["at"].append_child().id(),  "@");
        ti.set_val       (ti["at"].append_child().id(), "@");
        ti.set_serialized(ti["backtick"].append_child().id(),  "`test");
        ti.set_val       (ti["backtick"].append_child().id(), "`test2");
        ti.set_serialized(ti["backtick"].append_child().id(),  "`");
        ti.set_val       (ti["backtick"].append_child().id(), "`");
        std::string yaml = emitrs_yaml<std::string>(ti);
        test_check_emit_check(to_csubstr(yaml), check);
    }
}

TEST(emit, at_is_quoted_only_in_the_beggining__issue_320)
{
    Tree ti = parse_in_arena("{at: [], backtick: []}");
    ti["at"].append_child().save("@test");
    ti["at"].append_child().save("t@est");
    ti["at"].append_child().save("test@");
    ti["at"].append_child().set_val("@test2");
    ti["at"].append_child().set_val("t@est2");
    ti["at"].append_child().set_val("test2@");
    ti["backtick"].append_child().save("`test");
    ti["backtick"].append_child().save("t`est");
    ti["backtick"].append_child().save("test`");
    ti["backtick"].append_child().set_val("`test2");
    ti["backtick"].append_child().set_val("t`est2");
    ti["backtick"].append_child().set_val("test2`");
    std::string yaml = emitrs_yaml<std::string>(ti);
    test_check_emit_check(to_csubstr(yaml), [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("at"));
        ASSERT_TRUE(t.rootref().has_child("backtick"));
        ASSERT_EQ(t["at"].num_children(), 6u);
        ASSERT_EQ(t["backtick"].num_children(), 6u);
        EXPECT_EQ(t["at"][0].val(), "@test");
        EXPECT_EQ(t["at"][1].val(), "t@est");
        EXPECT_EQ(t["at"][2].val(), "test@");
        EXPECT_EQ(t["at"][3].val(), "@test2");
        EXPECT_EQ(t["at"][4].val(), "t@est2");
        EXPECT_EQ(t["at"][5].val(), "test2@");
        EXPECT_TRUE(   t["at"][0].is_val_quoted());
        EXPECT_TRUE( ! t["at"][1].is_val_quoted());
        EXPECT_TRUE( ! t["at"][2].is_val_quoted());
        EXPECT_TRUE(   t["at"][3].is_val_quoted());
        EXPECT_TRUE( ! t["at"][4].is_val_quoted());
        EXPECT_TRUE( ! t["at"][5].is_val_quoted());
        EXPECT_EQ(t["backtick"][0].val(), "`test");
        EXPECT_EQ(t["backtick"][1].val(), "t`est");
        EXPECT_EQ(t["backtick"][2].val(), "test`");
        EXPECT_EQ(t["backtick"][3].val(), "`test2");
        EXPECT_EQ(t["backtick"][4].val(), "t`est2");
        EXPECT_EQ(t["backtick"][5].val(), "test2`");
        EXPECT_TRUE(   t["backtick"][0].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][1].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][2].is_val_quoted());
        EXPECT_TRUE(   t["backtick"][3].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][4].is_val_quoted());
        EXPECT_TRUE( ! t["backtick"][5].is_val_quoted());
    });
}


TEST(emit, error_on_emit_yaml_to_short_buffer)
{
    csubstr yaml = "this is: not empty\n";
    const Tree tree = parse_in_arena(yaml);
    {
        char too_small[2];
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, too_small);
        });
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, too_small, /*error_on_excess*/true);
        });
        substr required = emit_yaml(tree, too_small, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, yaml.len);
    }
    {
        substr nothing;
        EXPECT_EQ(nothing.str, nullptr);
        EXPECT_EQ(nothing.len, 0u);
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, nothing);
        });
        ExpectError::check_error_basic([&]{
            emit_yaml(tree, nothing, /*error_on_excess*/true);
        });
        substr required = emit_yaml(tree, nothing, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, yaml.len);
    }
}

TEST(emit, error_on_emit_json_to_short_buffer)
{
    csubstr json = "{\"this is\": \"not empty\"}";
    const Tree tree = parse_in_arena(json);
    {
        char too_small[2];
        ExpectError::check_error_basic([&]{
            emit_json(tree, too_small);
        });
        ExpectError::check_error_basic([&]{
            emit_json(tree, too_small, /*error_on_excess*/true);
        });
        substr required = emit_json(tree, too_small, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, json.len);
    }
    {
        substr nothing;
        EXPECT_EQ(nothing.str, nullptr);
        EXPECT_EQ(nothing.len, 0u);
        ExpectError::check_error_basic([&]{
            emit_json(tree, nothing);
        });
        ExpectError::check_error_basic([&]{
            emit_json(tree, nothing, /*error_on_excess*/true);
        });
        substr required = emit_json(tree, nothing, /*error_on_excess*/false);
        EXPECT_EQ(required.str, nullptr);
        EXPECT_EQ(required.len, json.len);
    }
}

// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
