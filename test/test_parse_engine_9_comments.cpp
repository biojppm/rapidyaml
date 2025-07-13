#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

ENGINE_TEST(CommentSingle,
            ("# single comment\n"
             ),
            "+STR\n"
            "+DOC\n"
            "=COM |single comment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("single comment"));
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
            "=COM |single\\ncomment\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("single\ncomment"));
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
            "=COM |\\nsingle\\ncomment\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("\nsingle\ncomment\n"));
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
            "=COM |\\n\\nsingle\\n\\n\\ncomment\\n\\n\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("\n\nsingle\n\n\ncomment\n\n"));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(CommentMapFlowTrailing,
            (
             "{ # trailing comment\n"
             "}"
             ),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
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
            "=COM |leading comment\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("leading comment"));
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
            "=COM |leading comment\n"
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
    ___(ps.add_comment_leading("leading comment"));
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
            "=COM |leading\\ncomment\n"
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
    ___(ps.add_comment_leading("leading\ncomment"));
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
            "=COM |leading\\ncomment\n"
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
    ___(ps.add_comment_leading("leading\ncomment"));
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
            "=COM |leading\\ncomment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing\\ncomment\n"
            "=COM |leading2\\ncomment2\n"
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
    ___(ps.add_comment_leading("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\ncomment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading2\ncomment2"));
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
            "=COM |leading\\ncomment\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "=COM <trailing\\ncomment\n"
            "=COM |leading2\\ncomment2\n"
            "=VAL :foo2\n"
            "=VAL :bar2\n"
            "=COM <trailing2\\ncomment2\n"
            "=COM |leading3\\ncomment3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading("leading\ncomment"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("bar"));
    ___(ps.add_comment_trailing_val("trailing\ncomment"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading2\ncomment2"));
    ___(ps.set_key_scalar_plain("foo2"));
    ___(ps.set_val_scalar_plain("bar2"));
    ___(ps.add_comment_trailing_val("trailing2\ncomment2"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading3\ncomment3"));
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
            "=COM |leading comment 1\n"
            "+MAP {}\n"
            "=COM <trailing comment\n"
            "-MAP\n"
            "=COM |leading comment 2\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.add_comment_leading("leading comment 1"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_trailing_val("trailing comment"));
    ___(ps.end_map());
    ___(ps.add_comment_leading("leading comment 2"));
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
            "=COM |leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=COM <trailing comment for foo\n"
            "=COM |leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=COM <trailing comment for bar\n"
            "=COM |leading comment for map\n"
            "=VAL :map\n"
            "=COM <trailing comment for map\n"
            "+MAP {}\n"
            "=COM |leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=COM <trailing comment for mapchild\n"
            "-MAP\n"
            "=COM |leading comment for seq\n"
            "=VAL :seq\n"
            "=COM <trailing comment for seq\n"
            "+SEQ []\n"
            "=COM |leading comment for seqchild\n"
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
    ___(ps.add_comment_leading("leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val("trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val("trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_val("trailing comment for map"));
    ___(ps.begin_map_val_flow());
    ___(ps.add_comment_leading("leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val("trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_val("trailing comment for seq"));
    ___(ps.begin_seq_val_flow());
    ___(ps.add_comment_leading("leading comment for seqchild"));
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
            "=COM |leading comment for foo\n"
            "=VAL :foo\n"
            "=VAL :0\n"
            "=COM <trailing comment for foo\n"
            "=COM |leading comment for bar\n"
            "=VAL :bar\n"
            "=VAL :1\n"
            "=COM <trailing comment for bar\n"
            "=COM |leading comment for map\n"
            "=VAL :map\n"
            "=COM <trailing comment for map\n"
            "+MAP\n"
            "=COM |leading comment for mapchild\n"
            "=VAL :mapchild\n"
            "=VAL :yes\n"
            "=COM <trailing comment for mapchild\n"
            "-MAP\n"
            "=COM |leading comment for seq\n"
            "=VAL :seq\n"
            "=COM <trailing comment for seq\n"
            "+SEQ\n"
            "=COM |leading comment for seqchild\n"
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
    ___(ps.add_comment_leading("leading comment for foo"));
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain("0"));
    ___(ps.add_comment_trailing_val("trailing comment for foo"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for bar"));
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_comment_trailing_val("trailing comment for bar"));
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for map"));
    ___(ps.set_key_scalar_plain("map"));
    ___(ps.add_comment_trailing_val("trailing comment for map"));
    ___(ps.begin_map_val_block());
    ___(ps.add_comment_leading("leading comment for mapchild"));
    ___(ps.set_key_scalar_plain("mapchild"));
    ___(ps.set_val_scalar_plain("yes"));
    ___(ps.add_comment_trailing_val("trailing comment for mapchild"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.add_comment_leading("leading comment for seq"));
    ___(ps.set_key_scalar_plain("seq"));
    ___(ps.add_comment_trailing_val("trailing comment for seq"));
    ___(ps.begin_seq_val_block());
    ___(ps.add_comment_leading("leading comment for seqchild"));
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
