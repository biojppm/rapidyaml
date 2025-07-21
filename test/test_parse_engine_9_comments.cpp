#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

/*
# CLK: Comment Leading to Key
key: # CTK Comment Trailing Key
  # CLV Leading to Val
  val # CTV Comment Trailing Val
  # CLV 2
--- # CTV 3
---
# CLV 4
---
# CLV 5
- val # CTV 6
# CLV 7
--- # CTV 8
# CLV 9
{ # CTV 10
  # continued 10

  # CLK 11
  key: # CTK 12
       # continued 12
    # CLV 13
    [ # CTV 14
      # continued 14

      # CLV 15
      a, # CTV 16
         # continued 16
      # CLV 17
      b  # CTV 18
         # continued 18
      # CLV 19
    ] # CFV: Comment Footer for Val 19
    # CLV 20
} # CFV 21
# CLV 22
*/

namespace c4 {
namespace yml {

ENGINE_TEST(CommentBasic,
            (
                "# CLK: Comment Leading to Key\n"
                "key: # CTK Comment Trailing Key\n"
                "  # CLV Leading to Val\n"
                "  val # CTV Comment Trailing Val\n"
                "  # CLV 2\n"
                "--- # CTV 3\n"
                "---\n"
                "# CLV 4\n"
                "---\n"
                "# CLV 5\n"
                "- val # CTV 6\n"
                "# CLV 7\n"
                "--- # CTV 8\n"
                "# CLV 9\n"
                "{ # CTV 10\n"
                "  # continued 10\n"
                "\n"
                "  # CLK 11\n"
                "  key: # CTK 12\n"
                "       # continued 12\n"
                "    # CLV 13\n"
                "    [ # CTV 14\n"
                "      # continued 14\n"
                "\n"
                "      # CLV 15\n"
                "      a, # CTV 16\n"
                "         # continued 16\n"
                "      # CLV 17\n"
                "      b  # CTV 18\n"
                "         # continued 18\n"
                "      # CLV 19\n"
                "    ] # CFV: Comment Footer for Val 20\n"
                "      # continued 20\n"
                "    # CLV 21\n"
                "} # CFV 22\n"
                "  # continued 22\n"
                "# CLV 23\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLK 'CLK: Comment Leading to Key\n"
            "=VAL :key\n"
            "=CTK 'CTK: Comment Trailing Key\n"
            "=CLV 'CLV: Comment Leading to Val\n"
            "=VAL :val\n"
            "=CTV 'CTV: Comment Trailing Val\n"
            "=CLV 'CLV 2\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CTV 'CTV 3\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV 'CLV 4\n"
            "-DOC\n"
            "+DOC ---\n"
            "+SEQ\n"
            "=CLV 'CLV 5\n"
            "=VAL :val\n"
            "=CTV 'CTV 6\n"
            "=CLV 'CLV 7\n"
            "-SEQ\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CTV 'CTV 8\n"
            "=CLV 'CLV 9\n"
            "+MAP {}\n"
            "=CTV 'CTV 10\\ncontinued 10\n"
            "=CLK 'CLK 11\n"
            "=VAL :key\n"
            "=CTK 'CTK 12\\ncontinued 12\n"
            "=CLV 'CLV 13\n"
            "+SEQ []\n"
            "=CTV 'CTV 14\\ncontinued 14\n"
            "=CLV 'CLV 15\n"
            "=VAL :a\n"
            "=CTV 'CTV 16\\ncontinued 16\n"
            "=CLV 'CLV 17\n"
            "=VAL :b\n"
            "=CTV 'CTV 18\\ncontinued 18\n"
            "=CLV 'CLV 19\n"
            "-SEQ\n"
            "=CFV 'CFV Comment Footer for Val 20\\ncontinued 20\n"
            "=CLV 'CLV 21\n"
            "-MAP\n"
            "=CFV 'CFV 22\\ncontinued 22\n"
            "=CLV 'CLV 23\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingle,
            ("# single comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleNoLeadSpace,
            ("#single comment\n"
             ,
             "# single comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleLeadSpace,
            ("#  single comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV # single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val(" single comment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleMultiline0,
            (
             "# single\n"
             "# comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV # single comment\n"
            "=CLV #single\\ncomment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("single\ncomment"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleMultiline1,
            (
             "#\n"
             "# single\n"
             "# comment\n"
             "#\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #\\nsingle\\ncomment\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\nsingle\ncomment\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleMultiline2,
            (
             "#\n"
             "#\n"
             "# single\n"
             "#\n"
             "#\n"
             "# comment\n"
             "#\n"
             "#\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #\\n\\nsingle\\n\\n\\ncomment\\n\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\n\nsingle\n\n\ncomment\n\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentSingleMultiline3,
            (
             "#\n"
             "#\n"
             "#  single  \n"
             "#  \n"
             "#  \n"
             "#  comment  \n"
             "#\n"
             "#\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #\\n\\n single  \\n \\n \\n comment  \\n\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("\n\n single  \n \n \n comment  \n\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(CommentDocLeading,
            (
             "# leading 1\n"
             "---\n"
             "# leading 2\n"
             "---\n"
             "# leading 3\n"
             ,
             "---\n"
             "# leading 1\n"
             "---\n"
             "# leading 2\n"
             "---\n"
             "# leading 3\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading 1\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading 2\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading 3\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading 1"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading 2"));
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading 3"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentFlowLeading,
            (
             "# leading map\n"
             "{\n"
             "}\n"
             "---\n"
             "# leading seq\n"
             "[\n"
             "]\n"
             ,
             "---\n"
             "# leading map\n"
             "{}\n"
             "---\n"
             "# leading seq\n"
             "[]\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading map\n"
            "+MAP {}\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading seq\n"
            "+SEQ []\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading map"));
    ___(ps.begin_map_val_flow());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentFlowLeading3Map,
            (
             "# leading val 1\n"
             "{ # leading key 2\n" // is NOT trailing for the map, but leading to first child
             "  foo: # trailing key 3\n"
             "    # leading val 4\n"
             "    bar # trailing val 5\n"
             "  # leading key 6\n" // comment-only node
             "} # trailing val 7\n" // targets the map
             "# leading val 8\n" // comment-only node
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading val1\n"
            "+MAP {}\n"
            "=CLV #leading map child\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading seq\n"
            "+SEQ []\n"
            "=CLV #leading seq child\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_val("leading map child"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_leading_val("leading seq child"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentFlowLeading2,
            (
             "# leading map\n"
             "{\n"
             "  # leading map child\n"
             "}\n"
             "---\n"
             "# leading seq\n"
             "[\n"
             "  # leading seq child\n"
             "]\n"
             ,
             "---\n"
             "# leading map\n"
             "{\n"
             "  # leading map child\n"
             "}\n"
             "---\n"
             "# leading seq\n"
             "[\n"
             "  # leading seq child\n"
             "]\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading map\n"
            "+MAP {}\n"
            "=CLV #leading map child\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "=CLV #leading seq\n"
            "+SEQ []\n"
            "=CLV #leading seq child\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_val("leading map child"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.add_comment_leading_val("leading seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_leading_val("leading seq child"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlowTrailingMultiline,
            (
             "{ # trailing\n"
             "  # comment\n"
             "  # with\n"
             "  # multiple\n"
             "  # lines\n"
             "}"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=COM <trailing\\ncomment\\nwith\\nmultiple\\nlines\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing\ncomment\nwith\nmultiple\nlines"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlowLeadingTrailing,
            (
             "# leading comment\n"
             "{ # trailing comment\n"
             "}\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading comment\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading comment"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlowLeadingLeading,
            (
             "# leading comment1\n"
             "{ # trailing comment\n"
             "  # trailing comment2\n"
             "}\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading comment\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading comment"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlockLeadingTrailing,
            (
             "# leading comment\n"
             "foo: bar # trailing comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading comment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading comment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlockLeadingTrailingMultiline,
            (
             "# leading\n"
             "# comment\n"
             "foo: bar # trailing\n"
             "         # comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading\\ncomment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing\\ncomment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\ncomment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlockLeadingTrailingMultiline1,
            (
             "# leading\n"
             "# comment\n"
             "foo: # trailing\n"
             "     # kcomment\n"
             "  bar # trailing\n"
             "      # vcomment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading\\ncomment\n"
            "=VAL :foo\n"
            "=COM <trailing\\nkcomment\n"
            "=VAL :bar\n"
            "=COM <trailing\\nvcomment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.add_comment_trailing_val("trailing\nkcomment"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\nvcomment"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlockLeadingTrailingMultiline2,
            (
             "# leading\n"
             "# comment\n"
             "foo: bar # trailing\n"
             "         # comment\n"
             "# leading2\n"
             "# comment2\n"
             "foo2: bar2 # trailing2\n"
             "           # comment2\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading\\ncomment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing\\ncomment\n"
            "=CLV #leading2\\ncomment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=COM <trailing2\\ncomment2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\ncomment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading2\ncomment2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment_trailing_val("trailing2\ncomment2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlockLeadingTrailingMultiline3,
            (
             "# leading\n"
             "# comment\n"
             "foo: bar # trailing\n"
             "         # comment\n"
             "# leading2\n"
             "# comment2\n"
             "foo2: bar2 # trailing2\n"
             "           # comment2\n"
             "# leading3\n"
             "# comment3\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading\\ncomment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing\\ncomment\n"
            "=CLV #leading2\\ncomment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=COM <trailing2\\ncomment2\n"
            "=CLV #leading3\\ncomment3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\ncomment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading2\ncomment2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment_trailing_val("trailing2\ncomment2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading3\ncomment3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlowSingleTrailingThenLeading,
            (
             "# leading comment 1\n"
             "{ # trailing comment\n"
             "}\n"
             "# leading comment 2\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=CLV #leading comment 1\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "=CLV #leading comment 2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading_val("leading comment 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
    ___(ps.add_comment_leading_val("leading comment 2"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlow0,
            ("{\n"
             "# leading comment for foo\n"
             "foo: 0, # trailing comment for foo\n"
             "# leading comment for bar\n"
             "bar: 1, # trailing comment for bar\n"
             "# leading comment for map\n"
             "map: { # trailing comment for map\n"
             "  # leading comment for mapchild\n"
             "  mapchild: yes # trailing comment for mapchild\n"
             "},\n"
             "# leading comment for seq\n"
             "seq: [ # trailing comment for seq\n"
             "  # leading comment for seqchild\n"
             "  - seqchild # trailing comment for seqchild\n"
             "]\n"
             "}\n"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=CLV #leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=COM <trailing comment for foo\n"
            "=CLV #leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=COM <trailing comment for bar\n"
            "=CLV #leading comment for map\n"
            "=VAL :map\n"
            "=COM <trailing comment for map\n"
            "+MAP {}\n"
            "=CLV #leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=COM <trailing comment for mapchild\n"
            "-MAP\n"
            "=CLV #leading comment for seq\n"
            "=VAL :seq\n"
            "=COM <trailing comment for seq\n"
            "+SEQ []\n"
            "=CLV #leading comment for seqchild\n"
            "=VAL :seqchild\n"
            "=COM <trailing comment for seqchild\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_val("leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val("trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val("trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_val("trailing comment for map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading_val("leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val("trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_val("trailing comment for seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_leading_val("leading comment for seqchild"));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment_trailing_val("trailing comment for seqchild"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapBlock0,
            ("# leading comment for foo\n"
             "foo: 0 # trailing comment for foo\n"
             "# leading comment for bar\n"
             "bar: 1 # trailing comment for bar\n"
             "# leading comment for map\n"
             "map: # trailing comment for map\n"
             "  # leading comment for mapchild\n"
             "  mapchild: yes # trailing comment for mapchild\n"
             "# leading comment for seq\n"
             "seq: # trailing comment for seq\n"
             "  # leading comment for seqchild\n"
             "  - seqchild # trailing comment for seqchild\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=CLV #leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=COM <trailing comment for foo\n"
            "=CLV #leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=COM <trailing comment for bar\n"
            "=CLV #leading comment for map\n"
            "=VAL :map\n"
            "=COM <trailing comment for map\n"
            "+MAP\n"
            "=CLV #leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=COM <trailing comment for mapchild\n"
            "-MAP\n"
            "=CLV #leading comment for seq\n"
            "=VAL :seq\n"
            "=COM <trailing comment for seq\n"
            "+SEQ\n"
            "=CLV #leading comment for seqchild\n"
            "=VAL :seqchild\n"
            "=COM <trailing comment for seqchild\n"
            "-SEQ\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val("trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val("trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_val("trailing comment for map"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading_val("leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val("trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading_val("leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_val("trailing comment for seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment_leading_val("leading comment for seqchild"));
    ___(ps.set_val_scalar_plain("seqchild"));
    ___(ps.add_comment_trailing_val("trailing comment for seqchild"));
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

#ifdef WIP
TEST(comments, leading_multi_line)
{
    csubstr yaml = R"(
# leading comment for foo
# with several lines
#
# including empty lines
  #
  #
  # not necessary aligned
#
# also not necessary aligned
foo: bar
)";
    test_check_emit_check(yaml, [](Tree const &t){
    });
}

TEST(comments, trailing_single_line)
{
    csubstr yaml = R"(
foo: 0 # trailing comment for foo
bar: 1 # trailing comment for bar
)";
    test_check_emit_check(yaml, [](Tree const &t){
    });
}

TEST(comments, trailing_multi_line)
{
    csubstr yaml = R"(
foo: 0 # trailing comment for foo
       # with several lines
       #
       # including empty lines
         #
         #
         # not necessary aligned
       #
       # also not necessary aligned
bar: 1 # trailing comment for bar
       # with several lines
       #
       # including empty lines
         #
         #
         # not necessary aligned
       #
       # also not necessary aligned
)";
    test_check_emit_check(yaml, [](Tree const &t){
    });
}
#endif // WIP

// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
