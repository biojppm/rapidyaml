#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse_engine.def.hpp"
#include "c4/yml/event_handler_tree.hpp"
#endif
#include "./test_lib/test_helpers_tree.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
#if defined(__GNUC__) && (__GNUC__ == 14) && (C4_CPP >= 20)
C4_SUPPRESS_WARNING_GCC("-Warray-bounds")  // gcc has false positives here
C4_SUPPRESS_WARNING_GCC("-Wstringop-overflow")  // gcc has false positives here
#endif


struct plain_scalar_case
{
    size_t indentation;
    csubstr input, expected;
};

// double quoted filtering can result in an output larger than the input.
// so we ensure adequate test covering by using different sizes.
// test also cases where the destination string is not large
// enough to accomodate the filtered string.

/** when filtering from src to dst, specifying the dst sz is enough to
 * cover the different cases */
void test_filter_src_dst(csubstr input, csubstr expected, size_t indentation, size_t dst_sz)
{
    RYML_TRACE_FMT("\nstr=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~\nsz={}", input.len, input, expected.len, expected, dst_sz);
    // fill the dst buffer with a ref char to ensure there is no
    // write overflow.
    const size_t actual_sz = size_t(30) + (dst_sz > expected.len ? dst_sz : expected.len);
    std::string subject_;
    subject_.resize(actual_sz);
    const substr full = to_substr(subject_);
    // fill the canary region
    const char refchar = '`';
    full.sub(dst_sz).fill(refchar);
    // filter now
    const substr dst = full.first(dst_sz);
    Parser::handler_type evt_handler = {};
    Parser proc(&evt_handler);
    FilterResult result = proc.filter_scalar_plain(input, dst, indentation);
    // check the result
    EXPECT_EQ(result.required_len(), expected.len);
    if(result.valid())
    {
        const csubstr out = result.get();
        RYML_TRACE_FMT("\nout=[{}]~~~{}~~~", out.len, out);
        RYML_TRACE_FMT("\nout.str={}\ndst.str={}", (void const*)out.str, (void const*)dst.str);
        EXPECT_TRUE(out.is_sub(dst));
        EXPECT_EQ(out, expected);
        // check the fill character in the canary region
        EXPECT_GT(full.sub(dst_sz).len, 0u);
        EXPECT_EQ(full.sub(dst_sz).first_not_of(refchar), csubstr::npos);
    }
}


void test_filter_inplace(csubstr input, csubstr expected, size_t indentation)
{
    EXPECT_LE(expected.len, input.len);
    // fill the dst buffer with a ref char to ensure there is no
    // write overflow.
    const size_t max_sz = (input.len > expected.len ? input.len : expected.len);
    const size_t full_sz = max_sz + size_t(30);
    RYML_TRACE_FMT("\ninp=[{}]~~~{}~~~\nexp=[{}]~~~{}~~~\nmax_sz={}", input.len, input, expected.len, expected, max_sz);
    auto run = [&](size_t cap){
        // create the string
        std::string subject_(input.str, input.len);
        std::string subject_2 = subject_;
        subject_.resize(full_sz);
        // fill the canary region
        const char refchar = '`';
        const substr full = to_substr(subject_);
        full.sub(max_sz).fill(refchar);
        substr dst = full.first(input.len);
        // filter now
        Parser::handler_type evt_handler1 = {};
        Parser parser1(&evt_handler1);
        FilterResult result = parser1.filter_scalar_plain_in_place(dst, cap, indentation);
        EXPECT_EQ(result.get().len, expected.len);
        EXPECT_EQ(result.required_len(), expected.len);
        Parser::handler_type evt_handler2 = {};
        Parser parser2(&evt_handler2);
        Tree tree = parse_in_arena(&parser2, "file", "# set the tree in the parser");
        csubstr sresult = parser2._filter_scalar_plain(to_substr(subject_2), indentation);
        EXPECT_GE(result.required_len(), expected.len);
        EXPECT_EQ(sresult.len, result.str.len);
        if(result.valid())
        {
        const csubstr out = result.get();
            EXPECT_EQ(out, expected);
            // check the fill character in the canary region.
            EXPECT_GT(full.sub(max_sz).len, 0u);
            EXPECT_EQ(full.first_not_of(refchar, max_sz), csubstr::npos);
        }
    };
    if(input.len >= expected.len)
    {
        RYML_TRACE_FMT("all good: input.len={} >= expected.len={}", input.len, expected.len);
        run(input.len);
    }
    else // input.len < expected.len
    {
        RYML_TRACE_FMT("expanding: input.len={} < expected.len={}", input.len, expected.len);
        {
            RYML_TRACE_FMT("expanding.1: up to larger expected.len={}", expected.len);
            run(expected.len);
        }
        // there is no room to filter if we pass input.len as the capacity.
        {
            RYML_TRACE_FMT("expanding.2: up to smaller input.len={}", input.len);
            run(input.len);
        }
    }
}


// declare test cases
plain_scalar_case test_cases_filter[] = {
#define psc(indentation, input, expected) plain_scalar_case{indentation, csubstr(input), csubstr(expected)}
    // 0
    psc(0, "A", "A"),
    psc(0, "A B", "A B"),
    psc(0, "A\nB", "A B"),
    psc(1, "A\nB", "A B"),
    psc(2, "A\nB", "A B"),
    // 5
    psc(0, "A\n\nB", "A\nB"),
    psc(1, "A\n\nB", "A\nB"),
    psc(2, "A\n\nB", "A\nB"),
    psc(0, "A\n\n\nB", "A\n\nB"),
    psc(1, "A\n\n\nB", "A\n\nB"),
    // 10
    psc(2, "A\n\n\nB", "A\n\nB"),
    psc(0, "A\n\n\n\nB", "A\n\n\nB"),
    psc(1, "A\n\n\n\nB", "A\n\n\nB"),
    psc(2, "A\n\n\n\nB", "A\n\n\nB"),
    psc(0, "A\n\n\n\n\nB", "A\n\n\n\nB"),
    // 15
    psc(1, "A\n\n\n\n\nB", "A\n\n\n\nB"),
    psc(2, "A\n\n\n\n\nB", "A\n\n\n\nB"),
    psc(0, "a\nb  \n  c\nd\n\ne", "a b c d\ne"),
    psc(1, "a\nb  \n  c\nd\n\ne", "a b c d\ne"),
    psc(2, "a\nb  \n  c\nd\n\ne", "a b c d\ne"),
    //psc(0, "A\n \n", "A"),
    //psc(1, "A\n \n", "A"),
    //psc(2, "A\n \n", "A"),
    // 20
    psc(0, "1st non-empty\n\n 2nd non-empty \n   	3rd non-empty\n", "1st non-empty\n2nd non-empty 3rd non-empty"),
    psc(1, "1st non-empty\n\n 2nd non-empty \n   	3rd non-empty\n", "1st non-empty\n2nd non-empty 3rd non-empty"),
    psc(2, "1st non-empty\n\n 2nd non-empty \n   	3rd non-empty\n", "1st non-empty\n2nd non-empty 3rd non-empty"),
    psc(0, "---word1\nword2\n", "---word1 word2"),
    psc(0, "---word1\nword2", "---word1 word2"),
    // 25
    psc(0, "---word1\n\nword2\n", "---word1\nword2"),
    psc(0, "---word1\n\nword2", "---word1\nword2"),
    psc(0, "---word1\n\n\nword2", "---word1\n\nword2"),
    psc(0, "---word1\n\n\n\nword2", "---word1\n\n\nword2"),
    psc(0, "---word1\n\n\n\n\nword2", "---word1\n\n\n\nword2"),
    // 30
    psc(0, "value\n"
        "with\n"
        "  \n"
        "tabs\n"
        "tabs\n"
        "  \n"
        "  foo\n"
        "  \n"
        "    bar\n"
        "      baz\n"
        "     \n"
        ,
        "value with\ntabs tabs\nfoo\nbar baz\n"), // !!! not sure the final \n is right
    psc(2, "value\n"
        "with\n"
        "\n"
        "tabs\n"
        "tabs\n"
        "\n"
        "  foo\n"
        "\n"
        "    bar\n"
        "      baz\n"
        "\n", "value with\ntabs tabs\nfoo\nbar baz\n"), // !!! not sure the final \n is right
    psc(2, "value\n"
        "  with\n"
        "   	\n"
        "  tabs\n"
        "  tabs\n"
        "   	\n"
        "    foo\n"
        "   	\n"
        "      bar\n"
        "        baz\n"
        "   	   \n", "value with\ntabs tabs\nfoo\nbar baz\n"), // !!! not sure the final \n is right
    // 33
    psc(2, "value\n"
        "  with\n"
        "\n"
        "  tabs\n"
        "  tabs\n"
        "\n"
        "    foo\n"
        "\n"
        "      bar\n"
        "        baz\n"
        "\n", "value with\ntabs tabs\nfoo\nbar baz\n"), // !!! not sure the final \n is right
    psc(0, ". \"dquo\" plain 'squo'", ". \"dquo\" plain 'squo'"),
    // 35
    // 40
    // 45
    // 50
    // 55
    // 60
    // 65
    // 70
    // 75
    #undef psc
};


struct PlainScalarFilterSrcDstTest : public ::testing::TestWithParam<plain_scalar_case>
{
};

TEST_P(PlainScalarFilterSrcDstTest, dst_is_same_size)
{
    plain_scalar_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.expected, dqc.indentation, /*dst_sz*/dqc.expected.len);
}

TEST_P(PlainScalarFilterSrcDstTest, dst_is_larger_size)
{
    plain_scalar_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.expected, dqc.indentation, /*sz*/dqc.expected.len + 2u);
    test_filter_src_dst(dqc.input, dqc.expected, dqc.indentation, /*sz*/dqc.expected.len + 100u);
}

TEST_P(PlainScalarFilterSrcDstTest, dst_is_smaller_size)
{
    plain_scalar_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.expected, dqc.indentation, /*sz*/dqc.expected.len / 2u);
}

TEST_P(PlainScalarFilterSrcDstTest, dst_is_zero_size)
{
    plain_scalar_case dqc = GetParam();
    test_filter_src_dst(dqc.input, dqc.expected, dqc.indentation, /*sz*/0u);
}

struct PlainScalarFilterInplaceTest : public ::testing::TestWithParam<plain_scalar_case>
{
};

TEST_P(PlainScalarFilterInplaceTest, dst_is_same_size)
{
    plain_scalar_case dqc = GetParam();
    test_filter_inplace(dqc.input, dqc.expected, dqc.indentation);
}



INSTANTIATE_TEST_SUITE_P(plain_scalar_filter,
                         PlainScalarFilterSrcDstTest,
                         testing::ValuesIn(test_cases_filter));

INSTANTIATE_TEST_SUITE_P(plain_scalar_filter,
                         PlainScalarFilterInplaceTest,
                         testing::ValuesIn(test_cases_filter));


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(plain_scalar, multiline_1_4)
{
    csubstr val = "Several lines of text,\n"
        "\n"
        "but the second line is empty, and _unindented_. "
        "There are more lines that follow. "
        "And the last line terminates at the end of the file.";
    std::string yaml = R"(Several lines of text,


  but the second line is empty, and _unindented_.
  There are more lines that follow. And the last line
  terminates at the end of the file.
)";
    std::string expected = R"(Several lines of text,


  but the second line is empty, and _unindented_. There are more lines that follow. And the last line terminates at the end of the file.
)";
    test_check_emit_check(to_csubstr(yaml), [&](Tree const &t){
        ASSERT_EQ(t.rootref().val(), val);
        ASSERT_EQ(emitrs_yaml<std::string>(t), expected);
    });
}

TEST(plain_scalar, issue153_seq)
{
    Tree t = parse_in_arena("- A\n \n");
    EXPECT_EQ(t[0].val(), "A");
}

TEST(plain_scalar, issue153_map)
{
    Tree t = parse_in_arena("foo: A\n \n");
    EXPECT_EQ(t["foo"].val(), "A");
}

TEST(plain_scalar, test_suite_BS4K)
{
    Tree t;
    ExpectError::check_error_parse(&t, [&]{
        t = parse_in_arena(R"(word1  # comment
word2
word3
)");
    });
    ExpectError::check_error_parse(&t, [&]{
        t = parse_in_arena(R"(word1  # comment
word2
)");
    });
}

TEST(plain_scalar, test_suite_7TMG)
{
    csubstr yaml = R"(---
word1
# comment
---
# first value is NOT a multiline plain scalar
[ word1
# comment
, word2]
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_stream());
        ConstNodeRef doc = t.rootref().first_child();
        ASSERT_TRUE(doc.is_doc());
        ASSERT_TRUE(doc.is_val());
        EXPECT_EQ(doc.val(), "word1");
        doc = t.rootref().child(1);
        ASSERT_TRUE(doc.is_doc());
        ASSERT_TRUE(doc.is_seq());
        EXPECT_EQ(doc[0].val(), "word1");
        EXPECT_EQ(doc[1].val(), "word2");
    });
}


TEST(plain_scalar, test_suite_82AN)
{
    csubstr yaml = R"(
---word1
word2
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("---word1 word2"));
    });
}

TEST(plain_scalar, test_suite_EXG3)
{
    csubstr yaml = R"(
---
---word1
word2
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        ASSERT_TRUE(t.rootref().first_child().is_val());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("---word1 word2"));
    });
}


TEST(plain_scalar, test_suite_9YRD)
{
    csubstr yaml = R"(
a
b  
  c
d

e
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("a b c d\ne"));
    });
}

TEST(plain_scalar, test_suite_EX5H)
{
    csubstr yaml = R"(
---
a
b  
  c
d

e
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().child(0).is_doc());
        ASSERT_TRUE(t.rootref().child(0).is_val());
        EXPECT_EQ(t.rootref().child(0).val(), csubstr("a b c d\ne"));
    });
}


TEST(plain_scalar, test_suite_M7A3)
{
    csubstr yaml = R"(
Bare
document
...
# No document
...
|
%!PS-Adobe-2.0 # Not the first line
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_EQ(t.rootref().num_children(), 2u);
        EXPECT_EQ(t.rootref().child(0).val(), csubstr("Bare document"));
        EXPECT_EQ(t.rootref().child(1).val(), csubstr("%!PS-Adobe-2.0 # Not the first line\n"));
    });
}


TEST(plain_scalar, test_suite_HS5T)
{
    csubstr yaml = R"(
1st non-empty

 2nd non-empty 
   	3rd non-empty
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("1st non-empty\n2nd non-empty 3rd non-empty"));
    });
}

TEST(plain_scalar, test_suite_NB6Z)
{
    csubstr yaml = R"(
key0:
  value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
   	   
key1:
  value
  with

  tabs
  tabs

    foo

      bar
        baz

key2: something
      else
key3: something
  else
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_map());
        ASSERT_TRUE(t.rootref().has_child("key0"));
        ASSERT_TRUE(t.rootref().has_child("key1"));
        ASSERT_TRUE(t.rootref().has_child("key2"));
        ASSERT_TRUE(t.rootref().has_child("key3"));
        EXPECT_EQ(t["key0"].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t["key1"].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t["key2"].val(), csubstr("something else"));
        EXPECT_EQ(t["key3"].val(), csubstr("something else"));
    });
}

TEST(plain_scalar, test_suite_NB6Z_seq)
{
    csubstr yaml = R"(
- value
  with
   	
  tabs
  tabs
   	
    foo
   	
      bar
        baz
   	
- value
  with

  tabs
  tabs

    foo

      bar
        baz

- more
  value
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 3u);
        EXPECT_EQ(t[0].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t[1].val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
        EXPECT_EQ(t[2].val(), csubstr("more value"));
    });
}

TEST(plain_scalar, test_suite_NB6Z_docval)
{
    auto check = [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), csubstr("value with\ntabs tabs\nfoo\nbar baz"));
    };
    {
        SCOPED_TRACE("case 0");
        test_check_emit_check(R"(
value
with
   	
tabs
tabs
   	
  foo
   	
    bar
      baz
   	
)", check);
    }
    {
        SCOPED_TRACE("case 1");
        test_check_emit_check(R"(
value
with

tabs
tabs

  foo

    bar
      baz

)", check);
    }
}


} // namespace yml
} // namespace c4
