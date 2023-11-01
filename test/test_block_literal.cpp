#include "./test_group.hpp"

namespace c4 {
namespace yml {


struct blocklit_case
{
    size_t indentation;
    BlockChomp_e chomp;
    csubstr input, expected;
};

void test_filter_src_dst(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    std::string subject_;
    subject_.resize(2 * blcase.input.size());
    c4::substr dst = to_substr(subject_);
    ScalarFilter proc = {};
    csubstr out = proc.filter_block_literal(blcase.input, dst, blcase.indentation, blcase.chomp, Location{});
    if(blcase.chomp != CHOMP_CLIP)
    {
        EXPECT_EQ(out.len, blcase.expected.len);
    }
    ASSERT_TRUE(out.is_sub(dst));
    RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
    EXPECT_EQ(out, blcase.expected);
}

void test_filter_inplace(blocklit_case const& blcase)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~", blcase.input.len, blcase.input, blcase.expected.len, blcase.expected);
    if(blcase.input.len >= blcase.expected.len)
    {
        std::string subject_(blcase.input.str, blcase.input.len);
        c4::substr dst = to_substr(subject_);
        ScalarFilter proc = {};
        csubstr out = proc.filter_block_literal(dst, subject_.size(), blcase.indentation, blcase.chomp, Location{});
        if(blcase.chomp != CHOMP_CLIP)
        {
            EXPECT_EQ(out.len, blcase.expected.len);
        }
        ASSERT_TRUE(out.str);
        EXPECT_TRUE(out.is_sub(dst));
        RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
        EXPECT_EQ(out, blcase.expected);
    }
    else
    {
        {
            SCOPED_TRACE("spare size");
            std::string subject_(blcase.input.str, blcase.input.len);
            subject_.resize(blcase.expected.len + 30);
            c4::substr dst = to_substr(subject_).first(blcase.input.len);
            c4::substr rem = to_substr(subject_).sub(blcase.expected.len);
            rem.fill('^');
            ScalarFilter proc = {};
            csubstr out = proc.filter_block_literal(dst, subject_.size(), blcase.indentation, blcase.chomp, Location{});
            if(blcase.chomp != CHOMP_CLIP)
            {
                EXPECT_EQ(out.len, blcase.expected.len);
            }
            ASSERT_TRUE(out.str);
            EXPECT_TRUE(out.is_super(dst));
            RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
            EXPECT_EQ(out, blcase.expected);
            EXPECT_EQ(rem.first_not_of('^'), npos);
        }
        {
            SCOPED_TRACE("trimmed size");
            std::string subject_(blcase.input.str, blcase.input.len);
            subject_.resize(blcase.expected.len);
            c4::substr dst = to_substr(subject_).first(blcase.input.len);
            ScalarFilter proc = {};
            csubstr out = proc.filter_block_literal(dst, subject_.size(), blcase.indentation, blcase.chomp, Location{});
            if(blcase.chomp != CHOMP_CLIP)
            {
                EXPECT_EQ(out.len, blcase.expected.len);
            }
            ASSERT_TRUE(out.str);
            EXPECT_TRUE(out.is_super(dst));
            RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
            EXPECT_EQ(out, blcase.expected);
        }
        {
            SCOPED_TRACE("insufficient size");
            std::string subject_(blcase.input.str, blcase.input.len);
            c4::substr dst = to_substr(subject_);
            ScalarFilter proc = {};
            csubstr out = proc.filter_block_literal(dst, subject_.size(), blcase.indentation, blcase.chomp, Location{});
            if(blcase.chomp != CHOMP_CLIP)
            {
                EXPECT_EQ(out.len, blcase.expected.len);
            }
            ASSERT_FALSE(out.str);
        }
    }
}

struct BlockLitFilterTest : public ::testing::TestWithParam<blocklit_case>
{
};

std::string add_carriage_returns(csubstr input)
{
    std::string result;
    result.reserve(input.len + input.count('\n'));
    for(const char c : input)
    {
        if(c == '\n')
            result += '\r';
        result += c;
    }
    return result;
}

TEST_P(BlockLitFilterTest, filter_src_dst)
{
    test_filter_src_dst(GetParam());
}
TEST_P(BlockLitFilterTest, filter_src_dst_carriage_return)
{
    ParamType p = GetParam();
    std::string subject = add_carriage_returns(p.input);
    p.input = to_csubstr(subject);
    test_filter_src_dst(p);
}
TEST_P(BlockLitFilterTest, filter_inplace)
{
    test_filter_inplace(GetParam());
}
TEST_P(BlockLitFilterTest, filter_inplace_carriage_return)
{
    ParamType p = GetParam();
    std::string subject = add_carriage_returns(p.input);
    p.input = to_csubstr(subject);
    test_filter_inplace(p);
}


blocklit_case test_cases_filter[] = {
    #define blc(indentation, chomp, input, output) blocklit_case{indentation, chomp, csubstr(input), csubstr(output)}
    // 0
    blc(2, CHOMP_STRIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    blc(2, CHOMP_CLIP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    blc(2, CHOMP_KEEP,
        "Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n"),
    // 3
    blc(2, CHOMP_STRIP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    blc(2, CHOMP_KEEP,
        "  Several lines of text,\n  with some \"quotes\" of various 'types',\n  and also a blank line:\n\n  plus another line at the end.\n  \n",
        "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n"),
    // 6
    blc(1, CHOMP_STRIP, "", ""),
    blc(1, CHOMP_CLIP, "", ""),
    blc(1, CHOMP_KEEP, "", ""),
    // 9
    blc(1, CHOMP_STRIP, "\n", ""),
    blc(1, CHOMP_CLIP, "\n", ""),
    blc(1, CHOMP_KEEP, "\n", "\n"),
    // 12
    blc(1, CHOMP_STRIP, "\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 15
    blc(1, CHOMP_STRIP, "\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n", "\n\n"),
    // 18
    blc(1, CHOMP_STRIP, "\n\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n\n", "\n\n\n"),
    // 21
    blc(1, CHOMP_STRIP, "\n\n\n\n", ""),
    blc(1, CHOMP_CLIP, "\n\n\n\n", ""),
    blc(1, CHOMP_KEEP, "\n\n\n\n", "\n\n\n\n"),
    // 24
    blc(1, CHOMP_STRIP, "a", "a"),
    blc(1, CHOMP_CLIP, "a", "a\n"),
    blc(1, CHOMP_KEEP, "a", "a"),
    // 27
    blc(1, CHOMP_STRIP, "a\n", "a"),
    blc(1, CHOMP_CLIP, "a\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n", "a\n"),
    // 30
    blc(1, CHOMP_STRIP, "a\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 33
    blc(0, CHOMP_STRIP, "a\n\n", "a"),
    blc(0, CHOMP_CLIP, "a\n\n", "a\n"),
    blc(0, CHOMP_KEEP, "a\n\n", "a\n\n"),
    // 36
    blc(1, CHOMP_STRIP, "a\n\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n\n", "a\n\n\n"),
    // 39
    blc(1, CHOMP_STRIP, "a\n\n\n\n", "a"),
    blc(1, CHOMP_CLIP, "a\n\n\n\n", "a\n"),
    blc(1, CHOMP_KEEP, "a\n\n\n\n", "a\n\n\n\n"),
    // 42
    blc(1, CHOMP_STRIP, " ab\n \n \n", "ab"),
    blc(1, CHOMP_CLIP, " ab\n \n \n", "ab\n"),
    blc(1, CHOMP_KEEP, " ab\n \n \n", "ab\n\n\n"),
    // 45
    blc(1, CHOMP_STRIP, " ab\n \n  \n", "ab\n\n "),
    blc(1, CHOMP_CLIP, " ab\n \n  \n", "ab\n\n \n"),
    blc(1, CHOMP_KEEP, " ab\n \n  \n", "ab\n\n \n"),
    // 48
    blc(0, CHOMP_STRIP, "ab\n\n \n", "ab\n\n "),
    blc(0, CHOMP_CLIP, "ab\n\n \n", "ab\n\n \n"),
    blc(0, CHOMP_KEEP, "ab\n\n \n", "ab\n\n \n"),
    // 51
    blc(1, CHOMP_STRIP, "hello\nthere\n", "hello\nthere"),
    blc(1, CHOMP_CLIP, "hello\nthere\n", "hello\nthere\n"),
    blc(1, CHOMP_KEEP, "hello\nthere\n", "hello\nthere\n"),
    // 54
    blc(0, CHOMP_STRIP, "hello\nthere\n", "hello\nthere"),
    blc(0, CHOMP_CLIP, "hello\nthere\n", "hello\nthere\n"),
    blc(0, CHOMP_KEEP, "hello\nthere\n", "hello\nthere\n"),
    // 57
    blc(3, CHOMP_CLIP,
        "   There once was a short man from Ealing\n   Who got on a bus to Darjeeling\n       It said on the door\n       \"Please don't spit on the floor\"\n   So he carefully spat on the ceiling.\n",
        "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling.\n"),
    blc(8, CHOMP_CLIP,
        "        <blockquote style=\"font: italic 12pt Times\">\n"
        "        <p>\"Three is always greater than two,\n"
        "           even for large values of two\"</p>\n"
        "        <p>--Author Unknown</p>\n"
        "        </blockquote>",
        "<blockquote style=\"font: italic 12pt Times\">\n"
        "<p>\"Three is always greater than two,\n"
        "   even for large values of two\"</p>\n"
        "<p>--Author Unknown</p>\n"
        "</blockquote>\n"),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "  \n"
        "  plus another line at the end.\n",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        "\n"
        "plus another line at the end.\n"),
    // 60
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "   \n"
        "  plus another line at the end.",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        " \n"
        "plus another line at the end.\n"),
    blc(2, CHOMP_CLIP,
        "  Several lines of text,\n"
        "  with some \"quotes\" of various 'types',\n"
        "  and also a blank line:\n"
        "    \n"
        "  plus another line at the end.",
        "Several lines of text,\n"
        "with some \"quotes\" of various 'types',\n"
        "and also a blank line:\n"
        "  \n"
        "plus another line at the end.\n"),
    blc(4, CHOMP_CLIP,
        "    #include \"{{hdr.filename}}\"\n  \n    {{src.gencode}}",
        "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n"),
    // 63
    blc(1, CHOMP_STRIP, " \n  \n   \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n", "\n\n\n"),
    // 66
    blc(1, CHOMP_STRIP, " \n  \n   \n    ", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n    ", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n    ", "\n\n\n"),
    // 69
    blc(1, CHOMP_STRIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n    \n     \n      \n    \n   \n \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n    \n     \n      \n    \n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 72
    blc(1, CHOMP_STRIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    blc(1, CHOMP_CLIP, " \n  \n   \n\n     \n      \n\n   \n \n", ""),
    blc(1, CHOMP_KEEP, " \n  \n   \n\n     \n      \n\n   \n \n", "\n\n\n\n\n\n\n\n\n"),
    // 75
    blc(7, CHOMP_STRIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd"),
    blc(7, CHOMP_CLIP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n"),
    blc(7, CHOMP_KEEP,
        "       asd\n"
        "     \n"
        "   \n"
        "       \n"
        "  \n"
        " \n"
        "  ",
        "asd\n\n\n\n\n\n"),
    // 78
    blc(5, CHOMP_STRIP, "     asd\n     	 ", "asd\n\t "),
    blc(5, CHOMP_CLIP, "     asd\n     	 ", "asd\n\t \n"),
    blc(5, CHOMP_KEEP, "     asd\n     	 ", "asd\n\t "),
    // 81
    blc(5, CHOMP_STRIP, "     asd\n      	 ", "asd\n \t "),
    blc(5, CHOMP_CLIP, "     asd\n      	 ", "asd\n \t \n"),
    blc(5, CHOMP_KEEP, "     asd\n      	 ", "asd\n \t "),


    #undef blc
};

INSTANTIATE_TEST_SUITE_P(block_literal_filter,
                         BlockLitFilterTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(block_literal, empty_block)
{
    {
        Tree t = parse_in_arena(R"(- |
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- |-
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- |+
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(# no indentation: fails!
- |

- |-

- |+

)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- |
  
- |-
  
- |+
  
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- |
- |-
- |+
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    }
}


TEST(block_literal, empty_block_as_container_member)
{
    // this was ok
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
  e:
)", [](Tree const &t){
        EXPECT_TRUE(t["map"].has_key());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_EQ(t["map"].num_children(), 5u);
        for(const auto &child : t["map"].children())
        {
            EXPECT_EQ(child.val(), "");
            if(child.key() != "e")
            {
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
    // this was ok
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
)", [](Tree const &t){
        EXPECT_TRUE(t["map"].has_key());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_TRUE(t["map"].is_map());
        EXPECT_EQ(t["map"].num_children(), 4u);
        for(const auto &child : t["map"].children())
        {
            EXPECT_EQ(child.val(), "");
            EXPECT_TRUE(child.type().is_val_quoted());
            EXPECT_FALSE(child.val_is_null());
        }
    });
    // this was not ok! the block literal before next is extended: to
    // include the YAML for next!
    test_check_emit_check(R"(
map:
  a: ""
  b: ''
  c: >
  d: |
next:
  a: ""
  b: ''
  c: >
  d: |
)", [](Tree const &t){
        for(const char *name : {"map", "next"})
        {
            ASSERT_TRUE(t.rootref().has_child(to_csubstr(name))) << "name=" << name;
            ConstNodeRef node = t[to_csubstr(name)];
            EXPECT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_map());
            ASSERT_EQ(node.num_children(), 4u);
            for(const auto &child : node.children())
            {
                EXPECT_EQ(child.val(), "");
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
    test_check_emit_check(R"(
seq:
  - ""
  - ''
  - >
  - |
next:
  - ""
  - ''
  - >
  - |
)", [](Tree const &t){
        for(const char *name : {"seq", "next"})
        {
            ASSERT_TRUE(t.rootref().has_child(to_csubstr(name))) << "name=" << name;
            ConstNodeRef node = t[to_csubstr(name)];
            EXPECT_TRUE(node.has_key());
            EXPECT_TRUE(node.is_seq());
            ASSERT_EQ(node.num_children(), 4u);
            for(const auto &child : node.children())
            {
                EXPECT_EQ(child.val(), "");
                EXPECT_TRUE(child.type().is_val_quoted());
                EXPECT_FALSE(child.val_is_null());
            }
        }
    });
}


TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_1)
{
    Tree t = parse_in_arena("[]");
    NodeRef r = t.rootref();
    r.append_child() = "\n\n";
    r.append_child() = "\n\n";
    r.append_child() = "last";
    std::string out = emitrs_yaml<std::string>(t);
    t.clear();
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    out = emitrs_yaml<std::string>(t);
    t.clear();
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    out = emitrs_yaml<std::string>(t);
    t.clear();
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    EXPECT_EQ(csubstr("ab\n\n \n").trimr(" \t\n"), csubstr("ab"));
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2)
{
    Tree t = parse_in_arena(R"(--- |+
 ab
 
  
)");
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    std::string expected = R"(--- |
  ab
  
   

)";
    std::string out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_3)
{
    std::string yaml = R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.



- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

- last
)";
    std::string expected = R"(- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.

- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
- last
)";
    Tree t = parse_in_arena(to_csubstr(yaml));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    std::string out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
}

TEST(block_literal, carriage_return)
{
    std::string yaml = "with: |\r\n"
"  text\r\n"
"   	lines\r\n"
"without: |\n"
"  text\n"
"   	lines\n";
    Tree t = parse_in_arena(to_csubstr(yaml));
    EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
    EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
    auto emitted = emitrs_yaml<std::string>(t);
    #ifdef RYML_DBG
    __c4presc(emitted.data(), emitted.size());
    #endif
    Tree r = parse_in_arena(to_csubstr(emitted));
    EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
    EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
}

#ifdef JAVAI
TEST(block_literal, errors_on_tab_indents)
{
    Tree tree;
    ExpectError::do_check(&tree, [&]{
        parse_in_arena("foo: |4\n    this is foo\n    now with tab-\n \t \tmust not work\n", &tree);
    });
}
#endif

TEST(block_literal, test_suite_L24T_00)
{
    // this is double quoted, but will be emitted as a block literal
    csubstr yaml = R"(foo: "x\n \n"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["foo"].val(), csubstr("x\n \n"));
    });
}

TEST(block_literal, error_on_bad_spec)
{
    Tree t;
    ExpectError::do_check(&t, [&t]{
        t = parse_in_arena("- |012abc\n  must have errors above\n");
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_LITERAL)
{
//
ADD_CASE_TO_GROUP("indentation requirements",
R"(---
|
hello
there
---
|
 hello
 there
---
|
  hello
  there
---
|
ciao
qua
---
|
    ciao
    qua
---
|
      ciao
      qua
---
- |
 hello
 there
- |
 ciao
 qua
---
foo: |
 hello
 there
bar: |
 ciao
 qua
)",
N(STREAM, L{
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(SEQ|DOC, L{N(QV, "hello\nthere\n"), N(QV, "ciao\nqua\n")}),
        N(MAP|DOC, L{N(QV, "foo", "hello\nthere\n"), N(QV, "bar", "ciao\nqua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- |
hello
there
- |
ciao
qua
)",
N(L{N(QV, "hello\nthere\n"), N(QV, "ciao\nqua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: |
hello
there
bar: |
ciao
qua
)",
N(L{N(QV, "foo", "hello\nthere\n"), N(QV, "bar" "ciao\nqua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- |2
 hello
 there
)",
N(NOTYPE));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02",
"--- |1-",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03",
"--- |1+",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx",
"--- |+",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02_1",
"|1-",
N(DOCVAL|VALQUO, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03_1",
"|1+",
N(DOCVAL|VALQUO, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx_1",
"|+",
N(DOCVAL|VALQUO, {}));

ADD_CASE_TO_GROUP("block literal as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       "Please don't spit on the floor"
   So he carefully spat on the ceiling
)",
  N(MAP, {
     N(KEYVAL|VALQUO, "data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n")
      })
);

ADD_CASE_TO_GROUP("block literal and two scalars",
R"(
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style="font: italic 12pt Times">
        <p>"Three is always greater than two,
           even for large values of two"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     N(MAP, L{
          N(KEYVAL|VALQUO, "example", "HTML goes into YAML without modification\n"),
          N(KEYVAL|VALQUO, "message", R"(<blockquote style="font: italic 12pt Times">
<p>"Three is always greater than two,
   even for large values of two"</p>
<p>--Author Unknown</p>
</blockquote>
)"),
          N("date", "2007-06-01"),
              })
);

ADD_CASE_TO_GROUP("block literal no chomp, no indentation",
R"(example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     N(MAP, L{
      N(KEYVAL|VALQUO, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
      N("another", "text"),
          })
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another val")
  }
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=keep",
R"(
- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another val")
  }
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=strip",
R"(
- |-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another val")
  }
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 4",
R"(
- |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2",
R"(
example: |2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=keep",
R"(
example: |+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=strip",
R"(
example: |-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 3",
R"(
example: |
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 3",
R"(
example: |3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 4",
R"(
example: |4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 9",
R"(
example: |
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 9",
R"(
example: |9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, without quotes",
    R"(tpl:
  src: |
    #include <{{hdr.filename}}>
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include <{{hdr.filename}}>\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with double quotes",
    R"(tpl:
  src: |
    #include "{{hdr.filename}}"
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with single quotes",
    R"(tpl:
  src: |
    #include '{{hdr.filename}}'
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include '{{hdr.filename}}'\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with same indentation level 0",
R"(
aaa: |2
  xxx
bbb: |
  yyy
)",
  L{N(QV, "aaa", "xxx\n"), N(QV, "bbb", "yyy\n")}
    );

ADD_CASE_TO_GROUP("block literal with same indentation level 1",
R"(
- aaa: |2
    xxx
  bbb: |
    yyy
)",
  L{N(L{N(QV, "aaa", "xxx\n"), N(QV, "bbb", "yyy\n")})}
    );

ADD_CASE_TO_GROUP("block literal with tab and spaces",
R"(|
       	  )",
  N(DOCVAL|VALQUO, "\t  \n")
    );


ADD_CASE_TO_GROUP("block literal with empty docval 1",
R"(|)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 2",
R"(|
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 3",
R"(|
  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 4",
R"(|
  
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 5",
R"(|
    
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 8",
R"(|


)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 9",
R"(|



)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 10",
R"(|




)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 11",
R"(|
 
  
   
    )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 12",
R"(|
 
  
   
    
     
      
    
   
 
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 13",
R"(|
 
  

   

    

)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0",
R"(- |+
)",
  N(SEQ, L{N(VALQUO, "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.1",
R"(- |+
 )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.2",
R"(- |+
   )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1",
R"(foo: |+
)",
  N(MAP, L{N(VALQUO, "foo", "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.1",
R"(foo: |+
 )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.2",
R"(foo: |+
  )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2",
R"(|+
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.1",
R"(|+
 )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.2",
R"(|+
   )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0",
R"(- |+

)",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0.1",
R"(- |+

  )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1",
R"(foo: |+

)",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1.1",
R"(foo: |+

  )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2",
R"(|+

)",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2.1",
R"(|+

  )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16",
R"(|+


)",
  N(DOCVAL|VALQUO, "\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.1",
R"(foo: |+


)",
  N(MAP, L{N(VALQUO, "foo", "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.2",
R"(- |+


)",
  N(SEQ, L{N(VALQUO, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17",
R"(|+



)",
  N(DOCVAL|VALQUO, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.1",
R"(foo: |+



)",
  N(MAP, L{N(VALQUO, "foo", "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.2",
R"(- |+



)",
  N(SEQ, L{N(VALQUO, "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 0",
R"(|
  asd)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1",
R"(|
  asd
)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.1",
R"(|
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.2",
R"(|+
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 2",
R"(|
  asd

)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 3",
R"(|
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 4",
R"(|
  asd
  
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5",
R"(|
     asd
   
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.1",
R"(|
       asd
     
   
     
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.2",
R"(|
       asd
     
   
       
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.3",
R"(|
       asd
     
   
        
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 6",
R"(|
     asd
      )",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 7",
R"(|
     asd
      
)",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 8",
R"(|
     asd
       )",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 9",
R"(|
     asd
       
)",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 10",
R"(|
     asd
     	 )",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 11",
R"(|
     asd
      	 )",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 12",
R"(|
     asd
     	 
)",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 13",
R"(|
     asd
      	 
)",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 0",
R"(- |+
  
- |+
  )",
N(L{N(QV, "\n"), N(QV, "\n"),}));

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 1",
R"(- |+
  
- |+
  
)",
N(L{N(QV, "\n"), N(QV, "\n"),}));

}


} // namespace yml
} // namespace c4
