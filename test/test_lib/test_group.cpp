#ifndef RYML_SINGLE_HEADER
#include "c4/yml/detail/print.hpp"
#endif
#include "test_lib/test_group.hpp"
#include "test_lib/test_case.hpp"
#include "test_lib/test_events_ints_helpers.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <fstream>


namespace c4 {
namespace yml {

void CaseData::ensure_reftree(Case const *c)
{
    SCOPED_TRACE("reftree");
    if(reftree.ensure([&]{ c->root.recreate(reftree.result); }))
    {
        SCOPED_TRACE("checking tree invariants of recreated tree");
        test_invariants(reftree.result);
    }
}

void CaseDataLineEndings::ensure_tree_parse(Case const *c)
{
    SCOPED_TRACE("tree parse");
    auto &dst = tree.val;
    dst.ensure([&]{
        dst.result.clear();
        if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        {
            RYML_EXPECT_ERROR(check_error_parse(&dst.result, [&]{
                parse_in_place(c->fileline, tree.src.result, &dst.result);
            }, c->expected_location));
            if(testing::Test::HasFailure())
            {
                printf("---------------\n%.*s\n---------------\n", (int)c->src.len, c->src.str);
                print_tree("PARSED TREE", dst.result);
                return;
            }
        }
        else
        {
            bool parseok = false;
            RYML_EXPECT_ERROR(check_success(&dst.result, [&]{
                parse_in_place(c->fileline, tree.src.result, &dst.result);
                parseok = true;
            }));
            if(parseok && !(c->flags & EXPECT_RESOLVE_ERROR))
            {
                SCOPED_TRACE("test invariants");
                test_invariants(dst.result);
            }
            if(testing::Test::HasFailure())
            {
                printf("---------------\n%.*s\n---------------\n", (int)c->src.len, c->src.str);
                print_tree("PARSED TREE", dst.result);
                return;
            }
            if(c->flags & RESOLVE_REFS)
            {
                SCOPED_TRACE("resolve");
                if(c->flags & EXPECT_RESOLVE_ERROR)
                {
                    RYML_EXPECT_ERROR(check_error(ExpectedErrorType::err_any, &dst.result, [&]{
                        dst.result.resolve();
                    }, c->expected_location));
                }
                else
                {
                    RYML_EXPECT_ERROR(check_success(&dst.result, [&]{
                        dst.result.resolve();
                    }));
                    {
                        SCOPED_TRACE("test invariants");
                        test_invariants(dst.result);
                    }
                    {
                        SCOPED_TRACE("reorder");
                        dst.result.reorder();
                        test_invariants(dst.result);
                    }
                }
                if(testing::Test::HasFailure())
                {
                    printf("~~~\n%.*s\n~~~\n", (int)c->src.len, c->src.str);
                    print_tree("PARSED TREE", dst.result);
                    return;
                }
            }
        }
    });
}

void CaseDataLineEndings::ensure_tree_emit_yaml(Case const *c)
{
    SCOPED_TRACE("tree emit yaml");
    ensure_tree_parse(c);
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    auto &dst = tree_roundtrip_yaml;
    if(dst.src.ensure([&]{
        RYML_EXPECT_ERROR(check_success(&tree.val.result, [&]{
            emitrs_yaml(tree.val.result, &dst.src.result);
        }));
    }))
    {
        dst.orig = dst.src;
    }
}

void CaseDataLineEndings::ensure_tree_emit_json(Case const *c)
{
    SCOPED_TRACE("tree emit json");
    ensure_tree_parse(c);
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    auto &dst = tree_roundtrip_json;
    if(dst.src.ensure([&]{
        RYML_EXPECT_ERROR(check_success(&tree.val.result, [&]{
            emitrs_json(tree.val.result, &dst.src.result);
        }));
    }))
    {
        dst.orig = dst.src;
    }
}


void CaseDataLineEndings::ensure_tree_roundtrip_yaml(Case const *c)
{
    SCOPED_TRACE("tree roundtrip yaml");
    ensure_tree_emit_yaml(c);
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    auto &dst = tree_roundtrip_yaml.val;
    dst.ensure([&]{
        RYML_EXPECT_ERROR(check_success(&tree.val.result, [&]{
            parse_in_place(c->fileline, tree_roundtrip_yaml.src.result, &dst.result);
        }));
        if(!(c->flags & EXPECT_RESOLVE_ERROR)) // NOLINT
        {
            SCOPED_TRACE("test invariants: roundtrip");
            test_invariants(dst.result);
        }
    });
}

void CaseDataLineEndings::ensure_tree_roundtrip_json(Case const *c)
{
    SCOPED_TRACE("tree roundtrip json");
    ensure_tree_emit_json(c);
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    auto &dst = tree_roundtrip_json.val;
    dst.ensure([&]{
        RYML_EXPECT_ERROR(check_success(&tree.val.result, [&]{
            parse_json_in_place(c->fileline, tree_roundtrip_json.src.result, &dst.result);
        }));
        {
            SCOPED_TRACE("test invariants: roundtrip");
            test_invariants(tree_roundtrip_json.val.result);
        }
    });
}


//-----------------------------------------------------------------------------

using LangType = EmitType_e;
const LangType as_yaml = EMIT_YAML;
const LangType as_json = EMIT_JSON;

template<bool resize_buffers>
static void _parse_ints(csubstr name, substr src, extra::ievt::TestBuffers *ints, LangType type)
{
    SCOPED_TRACE("parse_ints");
    using Handler = extra::ievt::EventHandlerInts<resize_buffers>;
    Handler handler;
    ParseEngine<Handler> parser(&handler);
    _c4dbgpf("parsing source:\n{}", prs_(src));
    if C4_IF_CONSTEXPR (resize_buffers)
    {
        ints->prepare_parse<resize_buffers>(handler, src);
        if(type == as_yaml)
            parser.parse_in_place_ev(name, src);
        else
            parser.parse_json_in_place_ev(name, src);
    }
    else
    {
        ints->prepare_parse<resize_buffers>(handler, src,
                                /*estimate*/-1, /*arena*/2 * src.len);
        if(type == as_yaml)
            parser.parse_in_place_ev(name, src);
        else
            parser.parse_json_in_place_ev(name, src);
    }
    ASSERT_GT(handler.required_size_events(), 0);
    ASSERT_TRUE(handler.fits_buffers());
    handler.get_buffers(ints, true);
}


template<bool resize_buffers>
static void _test_parse_to_ints(Case const* c, substr src, extra::ievt::TestBuffers *ints, LangType type)
{
    if(c->flags & EXPECT_PARSE_ERROR)
    {
        SCOPED_TRACE("expect error");
        RYML_EXPECT_ERROR(check_error_parse([&]{
            _parse_ints<resize_buffers>(c->fileline, src, ints, type);
            ints->print(); // error failed to occur. So print debugging info.
        }, c->expected_location));
        if C4_IF_CONSTEXPR (!resize_buffers)
            ints->owned = true;
    }
    else
    {
        SCOPED_TRACE("parse to ints");
        bool parseok = false;
        RYML_EXPECT_ERROR(check_success([&]{
            _parse_ints<resize_buffers>(c->fileline, src, ints, type);
            parseok = true;
        }));
        if(parseok)
            ints->test_invariants();
    }
    if(testing::Test::HasFailure())
    {
        if(src != c->src)
            printf("~~~[%zu]\n%.*s~~~\n", c->src.len, (int)c->src.len, c->src.str);
        printf("~~~[%zu]\n%.*s~~~\n", src.len, (int)src.len, src.str);
        ints->print();
    }
}

template<bool resize_buffers>
static void _ensure_ints_parse(Case const *c, SrcInts &ints, LangType type)
{
    SCOPED_TRACE("ensure_ints_parse");
    ASSERT_TRUE(ints.orig.done);
    ASSERT_TRUE(ints.src.done);
    auto &dst = ints.val;
    dst.ensure([&]{
        _test_parse_to_ints<resize_buffers>(c, ints.src.result, &dst.result, type);
    });
}

template<bool resize_buffers>
static void _ensure_ints_emit(Case const *c, SrcInts &from, SrcInts &to, LangType type)
{
    SCOPED_TRACE("ensure_ints_parse");
    _ensure_ints_parse<resize_buffers>(c, from, as_yaml);// first level parsed as yaml
    ASSERT_TRUE(from.val.done);
    if(c->flags & EXPECT_PARSE_ERROR) // NOLINT
        return;
    bool executed = to.src.ensure([&]{
        if(type == as_yaml)
            from.val.result.emitrs_yaml(&to.src.result);
        else
            from.val.result.emitrs_json(&to.src.result);
    });
    if(executed)
    {
        to.orig = to.src;
    }
}

template<bool resize_buffers>
static void _ensure_ints_parse_roundtrip(Case const *c, SrcInts &from, SrcInts &to, LangType type)
{
    SCOPED_TRACE("ensure ints roundtrip");
    _ensure_ints_emit<resize_buffers>(c, from, to, type);
    {
        SCOPED_TRACE("roundtrip parse");
        _ensure_ints_parse<resize_buffers>(c, to, type);
    }
}


void CaseDataLineEndings::ensure_ints_resize_emit_yaml(Case const *c)
{
    SCOPED_TRACE("ensure_ints_resize_emit_yaml");
    _ensure_ints_emit<true>(c, ints_resize, ints_resize_roundtrip_yaml, as_yaml);
}
void CaseDataLineEndings::ensure_ints_resize_emit_json(Case const *c)
{
    SCOPED_TRACE("ensure_ints_resize_emit_json");
    _ensure_ints_emit<true>(c, ints_resize, ints_resize_roundtrip_json, as_json);
}

void CaseDataLineEndings::ensure_ints_noresize_emit_yaml(Case const *c)
{
    SCOPED_TRACE("ensure_ints_noresize_emit_yaml");
    _ensure_ints_emit<false>(c, ints_noresize, ints_noresize_roundtrip_yaml, as_yaml);
}
void CaseDataLineEndings::ensure_ints_noresize_emit_json(Case const *c)
{
    SCOPED_TRACE("ensure_ints_noresize_emit_json");
    _ensure_ints_emit<false>(c, ints_noresize, ints_noresize_roundtrip_json, as_json);
}


void CaseDataLineEndings::ensure_ints_resize_roundtrip_yaml(Case const *c)
{
    SCOPED_TRACE("ensure_ints_resize_roundtrip_yaml");
    _ensure_ints_parse_roundtrip<true>(c, ints_resize, ints_resize_roundtrip_yaml, as_yaml);
}
void CaseDataLineEndings::ensure_ints_resize_roundtrip_json(Case const *c)
{
    SCOPED_TRACE("ensure_ints_resize_roundtrip_json");
    _ensure_ints_parse_roundtrip<true>(c, ints_resize, ints_resize_roundtrip_json, as_json);
}

void CaseDataLineEndings::ensure_ints_noresize_roundtrip_yaml(Case const *c)
{
    SCOPED_TRACE("ensure_ints_noresize_roundtrip_yaml");
    _ensure_ints_parse_roundtrip<false>(c, ints_noresize, ints_noresize_roundtrip_yaml, as_yaml);
}
void CaseDataLineEndings::ensure_ints_noresize_roundtrip_json(Case const *c)
{
    SCOPED_TRACE("ensure_ints_noresize_roundtrip_json");
    _ensure_ints_parse_roundtrip<false>(c, ints_noresize, ints_noresize_roundtrip_json, as_json);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

using S = std::string const&;

void YmlTestCase::_test_parse_yaml_to_tree(CaseDataLineEndings *cd)
{
    cd->ensure_tree_parse(c);
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS|EXPECT_RESOLVE_ERROR)) // NOLINT
        return;
    d->ensure_reftree(c);
    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        ASSERT_GE(cd->tree.val.result.capacity(), c->root.reccount());
        ASSERT_EQ(cd->tree.val.result.size(), c->root.reccount());
        c->root.compare(cd->tree.val.result.rootref());
    }
    {
        SCOPED_TRACE("comparing to recreated tree");
        test_compare(cd->tree.val.result, d->reftree.result);
    }
}

void YmlTestCase::_test_roundtrip_yaml_tree(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS|EXPECT_RESOLVE_ERROR)) // NOLINT
        return;
    SCOPED_TRACE("test_roundtrip_yaml_tree");
    cd->ensure_tree_roundtrip_yaml(c);
    {
        SCOPED_TRACE("comparing roundtrip tree to ref tree");
        d->ensure_reftree(c);
        EXPECT_GE(cd->tree_roundtrip_yaml.val.result.capacity(), c->root.reccount());
        EXPECT_EQ(cd->tree_roundtrip_yaml.val.result.size(), c->root.reccount());
        c->root.compare(cd->tree_roundtrip_yaml.val.result.rootref());
    }
    {
        SCOPED_TRACE("comparing roundtrip tree to recreated tree");
        test_compare(cd->tree_roundtrip_yaml.val.result, d->reftree.result);
    }
}

void YmlTestCase::_test_roundtrip_json_tree(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    SCOPED_TRACE("test_roundtrip_json_tree");
    cd->ensure_tree_roundtrip_json(c);
    std::string json = emitrs_json<std::string>(cd->tree_roundtrip_json.val.result);
    EXPECT_EQ(json, (S)cd->tree_roundtrip_json.orig.result);
}


//-----------------------------------------------------------------------------

#define cmp_src_(ref, str)                                      \
    {                                                           \
        SCOPED_TRACE("cmp_src_");                               \
        ASSERT_TRUE((ref).orig.done);                           \
        ASSERT_TRUE((str).orig.done);                           \
        EXPECT_EQ((S)(ref).orig.result, (S)(str).orig.result);  \
    }
#define cmp_src_str_(ref, str)                      \
    {                                               \
        SCOPED_TRACE("cmp_src_str_");               \
        ASSERT_TRUE((ref).orig.done);               \
        EXPECT_EQ((S)(ref).orig.result, (S)(str));  \
    }

static void show_parse_info_(CaseDataLineEndings const* cd,
                             SrcTree const* tree, csubstr tree_emitted,
                             SrcInts const* ints, csubstr ints_emitted)
{
    csubstr src = cd->src;
    printf("------\nsrc\n------\n%.*s\n", (int)src.len, src.str);
    printf("------\nints\n------\n");
    ints->val.result.print();
    printf("------\nints emitted\n------\n%.*s\n", (int)ints_emitted.len, ints_emitted.str);
    print_tree("tree", tree->val.result);
    printf("------\ntree emitted\n------\n%.*s\n", (int)tree_emitted.len, tree_emitted.str);
}
void YmlTestCase::_test_parse_yaml_to_ints_resize(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_parse_ints_resize");
    _ensure_ints_parse<true>(c, cd->ints_resize, as_yaml);
    if(c->flags & EXPECT_PARSE_ERROR) // NOLINT
        return;
    cd->ensure_ints_resize_emit_yaml(c);
    if(c->flags & (HAS_CONTAINER_KEYS|RESOLVE_REFS)) // NOLINT
        return;
    cd->ensure_tree_emit_yaml(c);
    if(c->flags & NO_COMPARE_EMITTED_INTS) // NOLINT
        return;
    extra::ievt::test_compare_emitted_yaml_ints(cd->ints_resize_roundtrip_yaml.orig.result,
                                                cd->tree_roundtrip_yaml.orig.result);
    if(testing::Test::HasFailure())
    {
        show_parse_info_(cd,
                         &cd->tree, cd->tree_roundtrip_yaml.orig.result,
                         &cd->ints_resize, cd->ints_resize_roundtrip_yaml.orig.result);
    }
}
void YmlTestCase::_test_parse_yaml_to_ints_noresize(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_parse_ints_noresize");
    _ensure_ints_parse<false>(c, cd->ints_noresize, as_yaml);
    if(c->flags & EXPECT_PARSE_ERROR) // NOLINT
        return;
    cd->ensure_ints_noresize_emit_yaml(c);
    if(c->flags & (HAS_CONTAINER_KEYS|RESOLVE_REFS)) // NOLINT
        return;
    cd->ensure_tree_emit_yaml(c);
    if(c->flags & NO_COMPARE_EMITTED_INTS) // NOLINT
        return;
    extra::ievt::test_compare_emitted_yaml_ints(cd->ints_noresize_roundtrip_yaml.orig.result,
                                                cd->tree_roundtrip_yaml.orig.result);
    if(testing::Test::HasFailure())
    {
        show_parse_info_(cd,
                         &cd->tree, cd->tree_roundtrip_yaml.orig.result,
                         &cd->ints_noresize, cd->ints_noresize_roundtrip_yaml.orig.result);
    }
}


static void show_roundtrip_info_(CaseDataLineEndings const* cd,
                                 SrcTree const* tree,
                                 SrcInts const* level0, SrcInts const* level1,
                                 csubstr tree_emitted, csubstr actual)
{
    csubstr src = cd->src;
    csubstr ints_emitted = level1->orig.result;
    csubstr roundtrip_ints_emitted = to_csubstr(actual);
    printf("------\nsrc\n------\n%.*s\n", (int)src.len, src.str);
    printf("------\nints\n------\n");
    level0->val.result.print();
    printf("------\nints emitted\n------\n%.*s\n", (int)ints_emitted.len, ints_emitted.str);
    printf("------\nroundtrip ints\n------\n");
    level1->val.result.print();
    printf("------\nints roundtrip emitted\n------\n%.*s\n", (int)roundtrip_ints_emitted.len, roundtrip_ints_emitted.str);
    print_tree("roundtrip tree", tree->val.result);
    printf("------\ntree emitted\n------\n%.*s\n", (int)tree_emitted.len, tree_emitted.str);
}
static void _test_roundtrip_yaml_ints(Case const *c, CaseDataLineEndings *cd,
                                      SrcInts *level0, SrcInts *level1)
{
    if(testing::Test::HasFailure())
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    SCOPED_TRACE("test_roundtrip_yaml_ints");
    cd->ensure_tree_roundtrip_yaml(c);
    ASSERT_TRUE(level0->val.done);
    ASSERT_TRUE(level1->val.done);
    ASSERT_TRUE(cd->tree_roundtrip_yaml.val.done);
    level1->val.result.test_compare(level0->val.result);
    txtbuf actual = level1->val.result.emitrs_yaml<txtbuf>();
    if(!(c->flags & (NO_COMPARE_EMITTED_INTS|RESOLVE_REFS))) // NOLINT
    {
        {
            SCOPED_TRACE("vs ints");
            cmp_src_str_(*level1, actual);
        }
        {
            SCOPED_TRACE("vs tree");
            extra::ievt::test_compare_emitted_yaml_ints(actual, cd->tree_roundtrip_yaml.orig.result);
        }
    }
    if(testing::Test::HasFailure())
    {
        show_roundtrip_info_(cd, &cd->tree_roundtrip_yaml,
                             level0, level1, cd->tree_roundtrip_yaml.orig.result, actual);
    }
}
void YmlTestCase::_test_roundtrip_yaml_ints_resize(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_roundtrip_yaml_ints_resize");
    if(c->flags & (EXPECT_PARSE_ERROR)) // NOLINT
        return;
    cd->ensure_ints_resize_roundtrip_yaml(c);
    if(testing::Test::HasFailure())
        return;
    _test_roundtrip_yaml_ints(c, cd, &cd->ints_resize, &cd->ints_resize_roundtrip_yaml);
}
void YmlTestCase::_test_roundtrip_yaml_ints_noresize(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR)) // NOLINT
        return;
    SCOPED_TRACE("test_roundtrip_yaml_ints_noresize");
    cd->ensure_ints_noresize_roundtrip_yaml(c);
    _test_roundtrip_yaml_ints(c, cd, &cd->ints_noresize, &cd->ints_noresize_roundtrip_yaml);
}


void _test_roundtrip_json_ints(Case const *c, CaseDataLineEndings *cd,
                               SrcInts *level0, SrcInts *level1)
{
    if(testing::Test::HasFailure())
        return;
    SCOPED_TRACE("test_roundtrip_json_ints");
    cd->ensure_tree_emit_json(c);
    txtbuf actual = level1->val.result.emitrs_json<txtbuf>();
    if(!(c->flags & (NO_COMPARE_EMITTED_INTS|NO_COMPARE_EMITTED_INTS_JSON|RESOLVE_REFS))) // NOLINT
    {
        {
            SCOPED_TRACE("vs ints");
            cmp_src_str_(*level1, actual);
        }
        {
            SCOPED_TRACE("vs tree");
            cmp_src_str_(cd->tree_roundtrip_json, actual);
        }
    }
    if(testing::Test::HasFailure())
    {
        show_roundtrip_info_(cd, &cd->tree_roundtrip_json,
                             level0, level1, cd->tree_roundtrip_json.orig.result, actual);
    }
}
void YmlTestCase::_test_roundtrip_json_ints_resize(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_roundtrip_json_ints_resize");
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    cd->ensure_ints_resize_roundtrip_json(c);
    _test_roundtrip_json_ints(c, cd, &cd->ints_resize, &cd->ints_resize_roundtrip_json);
}
void YmlTestCase::_test_roundtrip_json_ints_noresize(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_roundtrip_json_ints_noresize");
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS)) // NOLINT
        return;
    cd->ensure_ints_noresize_roundtrip_json(c);
    _test_roundtrip_json_ints(c, cd, &cd->ints_noresize, &cd->ints_noresize_roundtrip_json);
}

} // namespace yml
} // namespace c4
