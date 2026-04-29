#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

#define INSTANTIATE_BOM_TESTS(name)                                 \
    using name = testing::TestWithParam<bomspec>;                   \
    INSTANTIATE_TEST_SUITE_P(BOM, name,                             \
                             testing::ValuesIn(bomspecs.begin(),    \
                                               bomspecs.end()),     \
                             namefor<bomspec>)


template<class T>
static std::string namefor(testing::TestParamInfo<T> const& pinfo) // NOLINT
{
    return namefor(pinfo.param);
}

static void test_bom_formatted_error_(bomspec spec, Location loc, csubstr fmt)
{
    RYML_TRACE_FMT("bom={}", spec.name);
    std::string src = formatrs<std::string>(fmt, spec.bom);
    Tree t;
    ExpectError::check_error_parse(&t, [&]{
        parse_in_place(to_substr(src), &t);
    }, loc);
    if(testing::Test::HasFailure())
        print_tree(t);
}

template<class Fn>
static void test_bom_formatted_success_(bomspec spec, Fn &&check, csubstr fmt)
{
    RYML_TRACE_FMT("bom={}", spec.name);
    std::string src = formatrs<std::string>(fmt, spec.bom);
    Tree t;
    ExpectError::check_success(&t, [&]{
        parse_in_place(to_substr(src), &t);
    });
    if(!testing::Test::HasFailure())
    {
        std::forward<Fn>(check)(t, spec);
    }
    if(testing::Test::HasFailure())
        print_tree(t);
}

#define test_bom_formatted_error(...)           \
    {                                           \
        SCOPED_TRACE("case");                   \
        test_bom_formatted_error_(__VA_ARGS__); \
    }

#define test_bom_formatted_success(...)             \
    {                                               \
        SCOPED_TRACE("case");                       \
        test_bom_formatted_success_(__VA_ARGS__);   \
    }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

INSTANTIATE_BOM_TESTS(SeqBlock);

namespace {
void check_seq_block(Tree const& t, bomspec const&)
{
    ConstNodeRef r = t.rootref();
    ASSERT_TRUE(r.is_seq());
    EXPECT_EQ(r[0].val(), "a");
    EXPECT_EQ(r[1].val(), "b");
}
} // namespace

TEST_P(SeqBlock, success_prevline)
{
    test_bom_formatted_success(GetParam(), check_seq_block,
                               "{}\n"
                               "- a\n"
                               "- b\n"
                               "");
}

TEST_P(SeqBlock, success_before_no_spaces)
{
    test_bom_formatted_success(GetParam(), check_seq_block,
                               "{}- a\n"
                               "- b\n"
                               "");
}

TEST_P(SeqBlock, success_before_interleaved_spaces)
{
    test_bom_formatted_success(GetParam(), check_seq_block,
                               "{}    - a\n"
                               "    - b\n"
                               "");
}

TEST_P(SeqBlock, err_after)
{
    if(GetParam().name == "NOBOM")
        return;
    size_t expected_col = GetParam().name == "UTF8" ? 7 : 1; // FIXME
    test_bom_formatted_error(GetParam(), Location(2, expected_col),
                             "   - a\n"
                             "{}   - b\n"
                             "");
}

TEST_P(SeqBlock, err_before_anchor)
{
    size_t expected_col = GetParam().bom.len + 4; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}&a - a\n"
                             "   - b\n"
                             "");
}

TEST_P(SeqBlock, err_before_tag)
{
    size_t expected_col = GetParam().bom.len + 4; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}!a - a\n"
                             "   - b\n"
                             "");
}

TEST_P(SeqBlock, err_doc_0)
{
    size_t expected_col = GetParam().bom.len + 5; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- - a\n"
                             "");
}

TEST_P(SeqBlock, err_doc_1)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        char buf[256];
        csubstr scalar = format_sub(buf, "---{} - a", b.bom);
        ASSERT_TRUE(t.rootref().is_val());
        EXPECT_EQ(t.rootref().val(), scalar);
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} - a\n"
                               "");
}

TEST_P(SeqBlock, err_doc_2)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        char buf[256];
        csubstr scalar = format_sub(buf, "{} - a", b.bom);
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.docref(0).is_val());
        EXPECT_EQ(t.docref(0).val(), scalar);
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "--- {} - a\n"
                               "");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

INSTANTIATE_BOM_TESTS(Qmrk);

namespace {
void check_qmrk(Tree const& t, bomspec const&)
{
    ConstNodeRef r = t.rootref();
    EXPECT_TRUE(r.is_map());
    EXPECT_EQ(r[0].key(), "a");
    EXPECT_EQ(r[0].val(), "b");
}
void check_qmrk_misleading(Tree const& t, bomspec const& spec, csubstr fmt)
{
    char buf[256];
    csubstr scalar = format_sub(buf, fmt, spec.bom);
    ASSERT_TRUE(t.rootref().is_val());
    ASSERT_TRUE(t.rootref().has_val());
    EXPECT_EQ(t.rootref().val(), scalar);
}
void check_qmrk_misleading2(Tree const& t, bomspec const& spec, csubstr fmt)
{
    char buf[256];
    csubstr scalar = format_sub(buf, fmt, spec.bom);
    ASSERT_TRUE(t.rootref().is_stream());
    ASSERT_TRUE(t[0].is_val());
    EXPECT_EQ(t[0].val(), scalar);
}
} // namespace

TEST_P(Qmrk, success_prevline)
{
    test_bom_formatted_success(GetParam(), check_qmrk,
                               "{}\n"
                               "? a\n"
                               ": b\n"
                               "");
}

TEST_P(Qmrk, success_before_no_spaces)
{
    test_bom_formatted_success(GetParam(), check_qmrk,
                               "{}? a\n"
                               ": b\n"
                               "");
}

TEST_P(Qmrk, success_before_interleaved_spaces)
{
    test_bom_formatted_success(GetParam(), check_qmrk,
                               "{}    ? a\n"
                               "    : b\n"
                               "");
}

TEST_P(Qmrk, err_after)
{
    if(GetParam().name == "NOBOM")
        return;
    test_bom_formatted_error(GetParam(), Location(2, 1),
                             "   ? a\n"
                             "{}   : b\n"
                             "");
}

TEST_P(Qmrk, err_before_anchor)
{
    size_t expected_col = GetParam().bom.len + 4; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}&a ? a\n"
                             "   : b\n"
                             "");
}

TEST_P(Qmrk, err_before_tag)
{
    size_t expected_col = GetParam().bom.len + 4; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}!a ? a\n"
                             "   : b\n"
                             "");
}

TEST_P(Qmrk, err_doc_0)
{
    size_t expected_col = GetParam().bom.len + 5; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- ? a\n"
                             "");
}

TEST_P(Qmrk, err_doc_1)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_qmrk_misleading(t, b, "---{} ? a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} ? a\n"
                               "");
}

TEST_P(Qmrk, err_doc_2)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_qmrk_misleading2(t, b, "{} ? a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "--- {} ? a\n"
                               "");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

INSTANTIATE_BOM_TESTS(MapBlock);

namespace {
void check_map_block(Tree const& t, bomspec const&)
{
    ConstNodeRef r = t.rootref();
    EXPECT_TRUE(r.is_map());
    EXPECT_EQ(r[0].key(), "a");
    EXPECT_EQ(r[0].val(), "b");
    EXPECT_EQ(r[1].key(), "c");
    EXPECT_EQ(r[1].val(), "d");
}
void check_map_block_misleading(Tree const& t, bomspec const& spec, csubstr fmt)
{
    char buf[256];
    csubstr scalar = format_sub(buf, fmt, spec.bom);
    ASSERT_TRUE(t.rootref().is_map());
    ASSERT_TRUE(t[0].has_val());
    ASSERT_TRUE(t[0].has_key());
    EXPECT_EQ(t[0].key(), scalar);
    EXPECT_EQ(t[0].val(), "b");
}
} // namespace

TEST_P(MapBlock, success_prevline)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}\n"
                               "a: b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlock, success_before_no_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}a: b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlock, success_before_interleaved_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    a: b\n"
                               "    c: d\n"
                               "");
}

TEST_P(MapBlock, err_after)
{
    if(GetParam().name == "NOBOM")
        return;
    size_t expected_col = 1;
    test_bom_formatted_error(GetParam(), Location(2, expected_col),
                             "   a: b\n"
                             "{}   c: d\n"
                             "");
}

TEST_P(MapBlock, success_before_anchor)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}&a a: b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlock, success_before_tag)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!t a: b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlock, multiple_annots_0_0)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}&key !ktag a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_0_0_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    &key !ktag a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_0_1)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!ktag &key a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_0_1_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    !ktag &key a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_1_0)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}&map !mtag\n"
                               "&key !ktag a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_1_0_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    &map !mtag\n"
                               "    &key !ktag a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_1_1)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!mtag &map\n"
                               "!ktag &key a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_1_1_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    !mtag &map\n"
                               "    !ktag &key a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_2_0)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!mtag &map\n"
                               "&key a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_2_0_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    !mtag &map\n"
                               "    &key a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_2_1)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!mtag &map\n"
                               "!ktag a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_2_1_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    !mtag &map\n"
                               "    !ktag a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_3_0)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}!mtag &map\n"
                               "a: b\n"
                               "c: d\n"
                               "");
}
TEST_P(MapBlock, multiple_annots_3_1_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block,
                               "{}    !mtag &map\n"
                               "    a: b\n"
                               "    c: d\n"
                               "");
}
TEST_P(MapBlock, err_multiple_anchors)
{
    size_t expected_col = GetParam().bom.len + 16; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}&ktag &another a: b\n"
                             "c: d\n"
                             "");
}
TEST_P(MapBlock, err_multiple_tags)
{
    size_t expected_col = GetParam().bom.len + 16; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}!ktag !another a: b\n"
                             "c: d\n"
                             "");
}

TEST_P(MapBlock, err_doc_0)
{
    size_t expected_col = GetParam().bom.len + 8; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- a: b\n"
                             "");
}

TEST_P(MapBlock, err_doc_0_tag)
{
    size_t expected_col = GetParam().bom.len + 11; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- !t a: b\n"
                             "");
}

TEST_P(MapBlock, err_doc_0_anchor)
{
    size_t expected_col = GetParam().bom.len + 11; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- &h a: b\n"
                             "");
}

TEST_P(MapBlock, err_doc_0_tag_anchor)
{
    size_t expected_col = GetParam().bom.len + 14; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- &h !t a: b\n"
                             "");
}


TEST_P(MapBlock, err_doc_1)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_misleading(t, b, "---{} a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} a: b\n"
                               "");
}

TEST_P(MapBlock, err_doc_1_tag)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_misleading(t, b, "---{} !t a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} !t a: b\n"
                               "");
}

TEST_P(MapBlock, err_doc_1_anchor)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_misleading(t, b, "---{} &h a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} &h a: b\n"
                               "");
}

TEST_P(MapBlock, err_doc_1_tag_anchor)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_misleading(t, b, "---{} &h !t a");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} &h !t a: b\n"
                               "");
}

TEST_P(MapBlock, err_doc_2)
{
    size_t expected_col = GetParam().bom.len + 9; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "--- {} a: b\n"
                             "");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

INSTANTIATE_BOM_TESTS(MapBlockNoKey);

namespace {
void check_map_block_nokey(Tree const& t, bomspec const&)
{
    ConstNodeRef r = t.rootref();
    EXPECT_TRUE(r.is_map());
    EXPECT_EQ(r[0].key(), "");
    EXPECT_EQ(r[0].val(), "b");
    EXPECT_EQ(r[1].key(), "c");
    EXPECT_EQ(r[1].val(), "d");
}
void check_map_block_nokey_misleading(Tree const& t, bomspec const& spec, csubstr fmt)
{
    char buf[256];
    csubstr scalar = format_sub(buf, fmt, spec.bom);
    ASSERT_TRUE(t.rootref().is_map());
    ASSERT_TRUE(t[0].has_val());
    ASSERT_TRUE(t[0].has_key());
    EXPECT_EQ(t[0].key(), scalar);
    EXPECT_EQ(t[0].val(), "b");
}
} // namespace

TEST_P(MapBlockNoKey, success_prevline)
{
    test_bom_formatted_success(GetParam(), check_map_block_nokey,
                               "{}\n"
                               ": b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlockNoKey, success_before_no_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block_nokey,
                               "{}: b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlockNoKey, success_before_interleaved_spaces)
{
    test_bom_formatted_success(GetParam(), check_map_block_nokey,
                               "{}    : b\n"
                               "    c: d\n"
                               "");
}

TEST_P(MapBlockNoKey, err_after)
{
    if(GetParam().name == "NOBOM")
        return;
    size_t expected_col = 1;
    test_bom_formatted_error(GetParam(), Location(2, expected_col),
                             "   : b\n"
                             "{}   c: d\n"
                             "");
}

TEST_P(MapBlockNoKey, success_before_anchor)
{
    test_bom_formatted_success(GetParam(), check_map_block_nokey,
                               "{}&a : b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlockNoKey, success_before_tag)
{
    test_bom_formatted_success(GetParam(), check_map_block_nokey,
                               "{}!t : b\n"
                               "c: d\n"
                               "");
}

TEST_P(MapBlockNoKey, err_doc_0)
{
    size_t expected_col = GetParam().bom.len + 5; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- : b\n"
                             "");
}

TEST_P(MapBlockNoKey, err_doc_0_tag)
{
    size_t expected_col = GetParam().bom.len + 8; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- !t : b\n"
                             "");
}

TEST_P(MapBlockNoKey, err_doc_0_anchor)
{
    size_t expected_col = GetParam().bom.len + 8; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- &h : b\n"
                             "");
}

TEST_P(MapBlockNoKey, err_doc_0_tag_anchor)
{
    size_t expected_col = GetParam().bom.len + 11; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "{}--- &h !t : b\n"
                             "");
}


TEST_P(MapBlockNoKey, err_doc_1)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_nokey_misleading(t, b, "---{}");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} : b\n"
                               "");
}

TEST_P(MapBlockNoKey, err_doc_1_tag)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_nokey_misleading(t, b, "---{} !t");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} !t : b\n"
                               "");
}

TEST_P(MapBlockNoKey, err_doc_1_anchor)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_nokey_misleading(t, b, "---{} &h");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} &h : b\n"
                               "");
}

TEST_P(MapBlockNoKey, err_doc_1_tag_anchor)
{
    if(GetParam().bom.empty())
        return;
    auto check_as_scalar = [&](Tree const& t, bomspec const& b){
        check_map_block_nokey_misleading(t, b, "---{} &h !t");
    };
    test_bom_formatted_success(GetParam(), check_as_scalar,
                               "---{} &h !t: b\n"
                               "");
}

TEST_P(MapBlockNoKey, err_doc_2)
{
    if(GetParam().bom.empty())
        return;
    size_t expected_col = GetParam().bom.len + 8; // FIXME
    test_bom_formatted_error(GetParam(), Location(1, expected_col),
                             "--- {} : b\n"
                             "");
}

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
