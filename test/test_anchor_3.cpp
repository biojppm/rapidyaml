#include "./test_lib/tree_and_ints.hpp"
#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#include "c4/yml/reference_resolver.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


static const ResolveOptions opts_pri_orig = {};
static const ResolveOptions opts_keep = ResolveOptions{}.keep_repeated_keys(true);
static const ResolveOptions opts_pri_merg = ResolveOptions{}.priority_original(false).priority_mergedin(true);
static const ResolveOptions opts_pri_none = ResolveOptions{}.priority_original(false);
static const ResolveOptions opts_pri_both = ResolveOptions{}.priority_mergedin(true);
static const ResolveOptions opts_pri_orig_all = ResolveOptions{opts_pri_orig}.merge_always(true);
static const ResolveOptions opts_pri_merg_all = ResolveOptions{opts_pri_merg}.merge_always(true);
static const ResolveOptions opts_pri_none_all = ResolveOptions{opts_pri_none}.merge_always(true);
static const ResolveOptions opts_pri_both_all = ResolveOptions{opts_pri_both}.merge_always(true);

static const ResolveOptions opts_pri_orig_last = ResolveOptions{opts_pri_orig}.pick_last(true);
static const ResolveOptions opts_pri_merg_last = ResolveOptions{opts_pri_merg}.pick_last(true);
static const ResolveOptions opts_pri_none_last = ResolveOptions{opts_pri_none}.pick_last(true);
static const ResolveOptions opts_pri_both_last = ResolveOptions{opts_pri_both}.pick_last(true);
static const ResolveOptions opts_pri_orig_all_last = ResolveOptions{opts_pri_orig_all}.pick_last(true);
static const ResolveOptions opts_pri_merg_all_last = ResolveOptions{opts_pri_merg_all}.pick_last(true);
static const ResolveOptions opts_pri_none_all_last = ResolveOptions{opts_pri_none_all}.pick_last(true);
static const ResolveOptions opts_pri_both_all_last = ResolveOptions{opts_pri_both_all}.pick_last(true);


TEST(anchors, resolve_options)
{
    #define TEST_RESOLVE_OPTIONS(opts, priority_original_, priority_mergedin_, pick_last_, keep_repeated_keys_, merge_always_) \
        EXPECT_##priority_original_((opts).priority_original());        \
        EXPECT_##priority_mergedin_((opts).priority_mergedin());        \
        EXPECT_##pick_last_((opts).pick_last());                        \
        EXPECT_##keep_repeated_keys_((opts).keep_repeated_keys());      \
        EXPECT_##merge_always_((opts).merge_always())

    TEST_RESOLVE_OPTIONS(opts_keep, TRUE,  FALSE, FALSE, TRUE, FALSE);

    TEST_RESOLVE_OPTIONS(opts_pri_orig, TRUE,  FALSE, FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg, FALSE, TRUE,  FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_none, FALSE, FALSE, FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_both, TRUE,  TRUE,  FALSE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_orig_all, TRUE,  FALSE, FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_all, FALSE, TRUE,  FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_all, FALSE, FALSE, FALSE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_all, TRUE,  TRUE,  FALSE, FALSE, TRUE);

    TEST_RESOLVE_OPTIONS(opts_pri_orig_last, TRUE,  FALSE, TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_last, FALSE, TRUE,  TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_last, FALSE, FALSE, TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_last, TRUE,  TRUE,  TRUE, FALSE, FALSE);
    TEST_RESOLVE_OPTIONS(opts_pri_orig_all_last, TRUE,  FALSE, TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_merg_all_last, FALSE, TRUE,  TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_none_all_last, FALSE, FALSE, TRUE, FALSE, TRUE);
    TEST_RESOLVE_OPTIONS(opts_pri_both_all_last, TRUE,  TRUE,  TRUE, FALSE, TRUE);


    #undef TEST_RESOLVE_OPTIONS
}

#define test_resolve_with_options0(yaml, resolved)              \
    {                                                           \
        TreeAndInts ti(to_csubstr(yaml));                       \
        test_resolve_(ti);                                      \
        test_emit_yaml_(ti, resolved);                          \
    }
#define test_resolve_with_options_(yaml, optsname, resolved)    \
    {                                                           \
        TreeAndInts ti(to_csubstr(yaml));                       \
        test_resolve_(ti, optsname);                            \
        test_emit_yaml_(ti, resolved);                          \
    }
#define test_resolve_with_options2(yaml, optsname1, optsname2, resolved) \
    {                                                           \
        test_resolve_with_options_(yaml, optsname1, resolved);  \
        test_resolve_with_options_(yaml, optsname2, resolved);  \
    }
#define test_resolve_with_options4(yaml, optsname1, optsname2, optsname3, optsname4, resolved) \
    {                                                           \
        test_resolve_with_options_(yaml, optsname1, resolved);  \
        test_resolve_with_options_(yaml, optsname2, resolved);  \
        test_resolve_with_options_(yaml, optsname3, resolved);  \
        test_resolve_with_options_(yaml, optsname4, resolved);  \
    }

TEST(anchors, cyclic2)
{
    test_resolve_err_("- &x [a,b]\n- &y [*x,*y]\n");
}
TEST(anchors, cyclic3)
{
    test_resolve_err_("- &x [a,b]\n- &y [*y,*y]\n");
}
TEST(anchors, cyclic4)
{
    test_resolve_with_options0("- &x [a,b]\n- &y [*x,*x]\n",
                               "- [a,b]\n- [[a,b],[a,b]]\n");
}
TEST(anchors, cyclic5)
{
    test_resolve_with_options0("{&z x: *z}",
                               "{x: x}");
}
TEST(anchors, cyclic6)
{
    test_resolve_err_("&z {&x x: a, *x : *z}\n");
}
TEST(anchors, cyclic7)
{
    test_resolve_err_("&z {&x x: a, *z : *x}\n");
}
TEST(anchors, cyclic8)
{
    test_resolve_with_options0("- &a0\n- &a1\n  - *a0\n",
                               "- \n- - \n");
}
TEST(anchors, cyclic9)
{
    test_resolve_err("- &a0\n- &a1\n  - *a1\n");
}
TEST(anchors, cyclic10)
{
    test_resolve_with_options0("- &a0\n"
                               "  - - - - &a1\n"
                               "        - *a1\n"
                               ,
                               "- - - - - \n"
                               "        - \n"
                               "");
}
TEST(anchors, cyclic11)
{
    test_resolve_err("- &a0\n"
                     "  - - - - &a1\n"
                     "        - *a0\n");
}

TEST(anchors, repeated_doc)
{
    std::string yaml = ""
        "--- &merg1\nrep: m11\nrep: m12\n"
        "--- &merg2\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\n<<: *merg1\nrep: o1\nbar: a00\nrep: o2\nrep: o3\n<<: *merg2\n"
        "---\nrep: o1\n<<: *merg1\nbar: a01\nrep: o2\n<<: *merg2\nrep: o3\n"
        "---\nrep: o1\nbar: a02\n<<: *merg1\n<<: *merg2\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a03\n<<: *merg2\nrep: o2\n<<: *merg1\nrep: o3\n"
        "---\nrep: o1\n<<: *merg2\nbar: a04\nrep: o2\nrep: o3\n<<: *merg1\n"
        "---\n<<: [*merg1,*merg2]\nrep: o1\nbar: a05\nrep: o2\nrep: o3\n"
        "---\n<<: [*merg2,*merg1]\nrep: o1\nbar: a06\nrep: o2\nrep: o3\n"
        "---\n<<: *merg1\n<<: *merg2\nrep: o1\nbar: a07\nrep: o2\nrep: o3\n"
        "---\n<<: *merg2\n<<: *merg1\nrep: o1\nbar: a08\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: [*merg1,*merg2]\nbar: a09\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: [*merg2,*merg1]\nbar: a10\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: *merg1\n<<: *merg2\nbar: a11\nrep: o2\nrep: o3\n"
        "---\nrep: o1\n<<: *merg2\n<<: *merg1\nbar: a12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a13\n<<: [*merg1,*merg2]\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a14\n<<: [*merg2,*merg1]\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a15\n<<: *merg1\n<<: *merg2\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a16\n<<: *merg2\n<<: *merg1\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a17\nrep: o2\n<<: [*merg1,*merg2]\nrep: o3\n"
        "---\nrep: o1\nbar: a18\nrep: o2\n<<: [*merg2,*merg1]\nrep: o3\n"
        "---\nrep: o1\nbar: a19\nrep: o2\n<<: *merg1\n<<: *merg2\nrep: o3\n"
        "---\nrep: o1\nbar: a20\nrep: o2\n<<: *merg2\n<<: *merg1\nrep: o3\n"
        "---\nrep: o1\nbar: a21\nrep: o2\nrep: o3\n<<: [*merg1,*merg2]\n"
        "---\nrep: o1\nbar: a22\nrep: o2\nrep: o3\n<<: [*merg2,*merg1]\n"
        "---\nrep: o1\nbar: a23\nrep: o2\nrep: o3\n<<: *merg1\n<<: *merg2\n"
        "---\nrep: o1\nbar: a24\nrep: o2\nrep: o3\n<<: *merg2\n<<: *merg1\n"
        "";
    test_resolve_with_options_(yaml, opts_keep,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nrep: m12\nrep: o1\nbar: a00\nrep: o2\nrep: o3\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nbar: a01\nrep: o2\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a02\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a03\nrep: m21\nrep: m22\nrep: o2\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nbar: a04\nrep: o2\nrep: o3\nrep: m11\nrep: m12\n"
        "---\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o1\nbar: a05\nrep: o2\nrep: o3\n"
        "---\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o1\nbar: a06\nrep: o2\nrep: o3\n"
        "---\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o1\nbar: a07\nrep: o2\nrep: o3\n"
        "---\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o1\nbar: a08\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nbar: a09\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nbar: a10\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nbar: a11\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nbar: a12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a13\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a14\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a15\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a16\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o2\nrep: o3\n"
        "---\nrep: o1\nbar: a17\nrep: o2\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a18\nrep: o2\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nbar: a19\nrep: o2\nrep: m11\nrep: m12\nrep: m21\nrep: m22\nrep: o3\n"
        "---\nrep: o1\nbar: a20\nrep: o2\nrep: m21\nrep: m22\nrep: m11\nrep: m12\nrep: o3\n"
        "---\nrep: o1\nbar: a21\nrep: o2\nrep: o3\nrep: m11\nrep: m12\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nbar: a22\nrep: o2\nrep: o3\nrep: m21\nrep: m22\nrep: m11\nrep: m12\n"
        "---\nrep: o1\nbar: a23\nrep: o2\nrep: o3\nrep: m11\nrep: m12\nrep: m21\nrep: m22\n"
        "---\nrep: o1\nbar: a24\nrep: o2\nrep: o3\nrep: m21\nrep: m22\nrep: m11\nrep: m12\n"
        "");

    {
        std::string expected =
            ""
            "---\nrep: m11\nrep: m12\n"
            "---\nrep: m21\nrep: m22\n"
            "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
            "---\nrep: o1\nbar: a00\n"
            "---\nrep: o1\nbar: a01\n"
            "---\nrep: o1\nbar: a02\n"
            "---\nrep: o1\nbar: a03\n"
            "---\nrep: o1\nbar: a04\n"
            "---\nrep: o1\nbar: a05\n"
            "---\nrep: o1\nbar: a06\n"
            "---\nrep: o1\nbar: a07\n"
            "---\nrep: o1\nbar: a08\n"
            "---\nrep: o1\nbar: a09\n"
            "---\nrep: o1\nbar: a10\n"
            "---\nrep: o1\nbar: a11\n"
            "---\nrep: o1\nbar: a12\n"
            "---\nrep: o1\nbar: a13\n"
            "---\nrep: o1\nbar: a14\n"
            "---\nrep: o1\nbar: a15\n"
            "---\nrep: o1\nbar: a16\n"
            "---\nrep: o1\nbar: a17\n"
            "---\nrep: o1\nbar: a18\n"
            "---\nrep: o1\nbar: a19\n"
            "---\nrep: o1\nbar: a20\n"
            "---\nrep: o1\nbar: a21\n"
            "---\nrep: o1\nbar: a22\n"
            "---\nrep: o1\nbar: a23\n"
            "---\nrep: o1\nbar: a24\n"
            "";
        test_resolve_with_options0(yaml, expected);
        test_resolve_with_options_(yaml, opts_pri_orig, expected);
    }

    test_resolve_with_options_(yaml, opts_pri_merg,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: m11\nbar: a01\n"
        "---\nrep: m11\nbar: a02\n"
        "---\nrep: m21\nbar: a03\n"
        "---\nrep: m21\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: m11\nbar: a09\n"
        "---\nrep: m21\nbar: a10\n"
        "---\nrep: m11\nbar: a11\n"
        "---\nrep: m21\nbar: a12\n"
        "---\nrep: m11\nbar: a13\n"
        "---\nrep: m21\nbar: a14\n"
        "---\nrep: m11\nbar: a15\n"
        "---\nrep: m21\nbar: a16\n"
        "---\nrep: m11\nbar: a17\n"
        "---\nrep: m21\nbar: a18\n"
        "---\nrep: m11\nbar: a19\n"
        "---\nrep: m21\nbar: a20\n"
        "---\nrep: m11\nbar: a21\n"
        "---\nrep: m21\nbar: a22\n"
        "---\nrep: m11\nbar: a23\n"
        "---\nrep: m21\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none, opts_pri_both,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: o1\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: o1\nbar: a05\n"
        "---\nrep: o1\nbar: a06\n"
        "---\nrep: o1\nbar: a07\n"
        "---\nrep: o1\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: m11\nbar: a01\n"
        "---\nrep: m11\nbar: a02\n"
        "---\nrep: m21\nbar: a03\n"
        "---\nrep: m21\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: m11\nbar: a09\n"
        "---\nrep: m21\nbar: a10\n"
        "---\nrep: m11\nbar: a11\n"
        "---\nrep: m21\nbar: a12\n"
        "---\nrep: m11\nbar: a13\n"
        "---\nrep: m21\nbar: a14\n"
        "---\nrep: m11\nbar: a15\n"
        "---\nrep: m21\nbar: a16\n"
        "---\nrep: m11\nbar: a17\n"
        "---\nrep: m21\nbar: a18\n"
        "---\nrep: m11\nbar: a19\n"
        "---\nrep: m21\nbar: a20\n"
        "---\nrep: m11\nbar: a21\n"
        "---\nrep: m21\nbar: a22\n"
        "---\nrep: m11\nbar: a23\n"
        "---\nrep: m21\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_all, opts_pri_both_all,
        "---\nrep: m11\n"
        "---\nrep: m21\n"
        "---\nrep: O1\nbar: a\n"
        "---\nrep: m11\nbar: a00\n"
        "---\nrep: o1\nbar: a01\n"
        "---\nrep: o1\nbar: a02\n"
        "---\nrep: o1\nbar: a03\n"
        "---\nrep: o1\nbar: a04\n"
        "---\nrep: m11\nbar: a05\n"
        "---\nrep: m21\nbar: a06\n"
        "---\nrep: m11\nbar: a07\n"
        "---\nrep: m21\nbar: a08\n"
        "---\nrep: o1\nbar: a09\n"
        "---\nrep: o1\nbar: a10\n"
        "---\nrep: o1\nbar: a11\n"
        "---\nrep: o1\nbar: a12\n"
        "---\nrep: o1\nbar: a13\n"
        "---\nrep: o1\nbar: a14\n"
        "---\nrep: o1\nbar: a15\n"
        "---\nrep: o1\nbar: a16\n"
        "---\nrep: o1\nbar: a17\n"
        "---\nrep: o1\nbar: a18\n"
        "---\nrep: o1\nbar: a19\n"
        "---\nrep: o1\nbar: a20\n"
        "---\nrep: o1\nbar: a21\n"
        "---\nrep: o1\nbar: a22\n"
        "---\nrep: o1\nbar: a23\n"
        "---\nrep: o1\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: o3\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: o3\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: o3\nbar: a21\n"
        "---\nrep: o3\nbar: a22\n"
        "---\nrep: o3\nbar: a23\n"
        "---\nrep: o3\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: m22\nbar: a01\n"
        "---\nrep: m22\nbar: a02\n"
        "---\nrep: m12\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: m22\nbar: a05\n"
        "---\nrep: m12\nbar: a06\n"
        "---\nrep: m22\nbar: a07\n"
        "---\nrep: m12\nbar: a08\n"
        "---\nrep: m22\nbar: a09\n"
        "---\nrep: m12\nbar: a10\n"
        "---\nrep: m22\nbar: a11\n"
        "---\nrep: m12\nbar: a12\n"
        "---\nrep: m22\nbar: a13\n"
        "---\nrep: m12\nbar: a14\n"
        "---\nrep: m22\nbar: a15\n"
        "---\nrep: m12\nbar: a16\n"
        "---\nrep: m22\nbar: a17\n"
        "---\nrep: m12\nbar: a18\n"
        "---\nrep: m22\nbar: a19\n"
        "---\nrep: m12\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_last, opts_pri_both_last,
        "---\nrep: m11\nrep: m12\n"
        "---\nrep: m21\nrep: m22\n"
        "---\nrep: O1\nbar: a\nrep: O2\nrep: O3\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_orig_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: o3\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: o3\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: o3\nbar: a21\n"
        "---\nrep: o3\nbar: a22\n"
        "---\nrep: o3\nbar: a23\n"
        "---\nrep: o3\nbar: a24\n"
        "");

    test_resolve_with_options_(
        yaml, opts_pri_merg_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: m22\nbar: a01\n"
        "---\nrep: m22\nbar: a02\n"
        "---\nrep: m12\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: m22\nbar: a05\n"
        "---\nrep: m12\nbar: a06\n"
        "---\nrep: m22\nbar: a07\n"
        "---\nrep: m12\nbar: a08\n"
        "---\nrep: m22\nbar: a09\n"
        "---\nrep: m12\nbar: a10\n"
        "---\nrep: m22\nbar: a11\n"
        "---\nrep: m12\nbar: a12\n"
        "---\nrep: m22\nbar: a13\n"
        "---\nrep: m12\nbar: a14\n"
        "---\nrep: m22\nbar: a15\n"
        "---\nrep: m12\nbar: a16\n"
        "---\nrep: m22\nbar: a17\n"
        "---\nrep: m12\nbar: a18\n"
        "---\nrep: m22\nbar: a19\n"
        "---\nrep: m12\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");

    test_resolve_with_options2(
        yaml, opts_pri_none_all_last, opts_pri_both_all_last,
        "---\nrep: m12\n"
        "---\nrep: m22\n"
        "---\nrep: O3\nbar: a\n"
        "---\nrep: m22\nbar: a00\n"
        "---\nrep: o3\nbar: a01\n"
        "---\nrep: o3\nbar: a02\n"
        "---\nrep: o3\nbar: a03\n"
        "---\nrep: m12\nbar: a04\n"
        "---\nrep: o3\nbar: a05\n"
        "---\nrep: o3\nbar: a06\n"
        "---\nrep: o3\nbar: a07\n"
        "---\nrep: o3\nbar: a08\n"
        "---\nrep: o3\nbar: a09\n"
        "---\nrep: o3\nbar: a10\n"
        "---\nrep: o3\nbar: a11\n"
        "---\nrep: o3\nbar: a12\n"
        "---\nrep: o3\nbar: a13\n"
        "---\nrep: o3\nbar: a14\n"
        "---\nrep: o3\nbar: a15\n"
        "---\nrep: o3\nbar: a16\n"
        "---\nrep: o3\nbar: a17\n"
        "---\nrep: o3\nbar: a18\n"
        "---\nrep: o3\nbar: a19\n"
        "---\nrep: o3\nbar: a20\n"
        "---\nrep: m22\nbar: a21\n"
        "---\nrep: m12\nbar: a22\n"
        "---\nrep: m22\nbar: a23\n"
        "---\nrep: m12\nbar: a24\n"
        "");
}

TEST(anchors, repeated_map)
{
    std::string yaml = R"(map:
  foo: 0
  foo: 1
)";
    test_resolve_with_options0(yaml,                "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_orig, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_merg, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_none, "map:\n  foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_both, "map:\n  foo: 0\n  foo: 1\n");
}

TEST(anchors, repeated_seq)
{
    std::string yaml = R"(-
  foo: 0
  foo: 1
)";
    test_resolve_with_options0(yaml,                "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_orig, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_merg, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_none, "- foo: 0\n  foo: 1\n");
    test_resolve_with_options_(yaml, opts_pri_both, "- foo: 0\n  foo: 1\n");
}


//-----------------------------------------------------------------------------

TEST(anchors, issue_484_0)
{
    csubstr cases[] = {
        csubstr("base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  <<: [*base_1,*base_2]\n"
                "k2:\n  <<: [*base_2,*base_1]\n"
                "k3:\n  a: 100\n  <<: [*base_1,*base_2]\n"
                "k4:\n  <<: [*base_1,*base_2]\n  a: 100\n"
                "k5:\n  a: 100\n  <<: [*base_2,*base_1]\n"
                "k6:\n  <<: [*base_2,*base_1]\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\nk1:\n"
                "  <<: *base_1\n  <<: *base_2\n"
                "k2:\n  <<: *base_2\n  <<: *base_1\n"
                "k3:\n  a: 100\n  <<: *base_1\n  <<: *base_2\n"
                "k4:\n  <<: *base_1\n  <<: *base_2\n  a: 100\n"
                "k5:\n  a: 100\n  <<: *base_2\n  <<: *base_1\n"
                "k6:\n  <<: *base_2\n  <<: *base_1\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  !!merge <<: [*base_1,*base_2]\n"
                "k2:\n  !!merge <<: [*base_2,*base_1]\n"
                "k3:\n  a: 100\n  !!merge <<: [*base_1,*base_2]\n"
                "k4:\n  !!merge <<: [*base_1,*base_2]\n  a: 100\n"
                "k5:\n  a: 100\n  !!merge <<: [*base_2,*base_1]\n"
                "k6:\n  !!merge <<: [*base_2,*base_1]\n  a: 100\n"
                ""),
        csubstr(""
                "base_1: &base_1\n  a: 10\n  b: 10\n"
                "base_2: &base_2\n  b: 20\n"
                "k1:\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n"
                "k2:\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n"
                "k3:\n  a: 100\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n"
                "k4:\n  !!merge <<: *base_1\n  !!merge <<: *base_2\n  a: 100\n"
                "k5:\n  a: 100\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n"
                "k6:\n  !!merge <<: *base_2\n  !!merge <<: *base_1\n  a: 100\n"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        "base_1:\n  a: 10\n  b: 10\n"
                        "base_2:\n  b: 20\n"
                        "k1:\n  a: 10\n  b: 10\n"
                        "k2:\n  b: 20\n  a: 10\n"
                        "k3:\n  a: 100\n  b: 10\n"
                        "k4:\n  a: 100\n  b: 10\n"
                        "k5:\n  a: 100\n  b: 20\n"
                        "k6:\n  b: 20\n  a: 100\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}

TEST(anchors, issue_484_0_flow_sl)
{
    csubstr cases[] = {
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {<<: [*base_1,*base_2]}},\n"
                "  {k2: {<<: [*base_2,*base_1]}},\n"
                "  {k3: {a: 100,<<: [*base_1,*base_2]}},\n"
                "  {k4: {<<: [*base_1,*base_2],a: 100}},\n"
                "  {k5: {a: 100,<<: [*base_2,*base_1]}},\n"
                "  {k6: {<<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {<<: *base_1,<<: *base_2}},\n"
                "  {k2: {<<: *base_2,<<: *base_1}},\n"
                "  {k3: {a: 100,<<: *base_1,<<: *base_2}},\n"
                "  {k4: {<<: *base_1,<<: *base_2,a: 100}},\n"
                "  {k5: {a: 100,<<: *base_2,<<: *base_1}},\n"
                "  {k6: {<<: *base_2,<<: *base_1,a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {!!merge <<: [*base_1,*base_2]}},\n"
                "  {k2: {!!merge <<: [*base_2,*base_1]}},\n"
                "  {k3: {a: 100,!!merge <<: [*base_1,*base_2]}},\n"
                "  {k4: {!!merge <<: [*base_1,*base_2],a: 100}},\n"
                "  {k5: {a: 100,!!merge <<: [*base_2,*base_1]}},\n"
                "  {k6: {!!merge <<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {base_1: &base_1 {a: 10,b: 10}},\n"
                "  {base_2: &base_2 {b: 20}},\n"
                "  {k1: {!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {k2: {!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {k3: {a: 100,!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {k4: {!!merge <<: *base_1,!!merge <<: *base_2,a: 100}},\n"
                "  {k5: {a: 100,!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {k6: {!!merge <<: *base_2,!!merge <<: *base_1,a: 100}}\n"
                "]"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        ""
                        "[\n"
                        "  {base_1: {a: 10,b: 10}},\n"
                        "  {base_2: {b: 20}},\n"
                        "  {k1: {a: 10,b: 10}},\n"
                        "  {k2: {b: 20,a: 10}},\n"
                        "  {k3: {a: 100,b: 10}},\n"
                        "  {k4: {a: 100,b: 10}},\n"
                        "  {k5: {a: 100,b: 20}},\n"
                        "  {k6: {b: 20,a: 100}}\n"
                        "]\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}

TEST(anchors, issue_484_0_flow_ml)
{
    csubstr cases[] = {
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n<<: [*base_1,*base_2]}},\n"
                "  {\nk2: {\n<<: [*base_2,*base_1]}},\n"
                "  {\nk3: {\na: 100,<<: [*base_1,*base_2]}},\n"
                "  {\nk4: {\n<<: [*base_1,*base_2],a: 100}},\n"
                "  {\nk5: {\na: 100,<<: [*base_2,*base_1]}},\n"
                "  {\nk6: {\n<<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
                "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
                "  {\nk3: {\na: 100,<<: *base_1,<<: *base_2}},\n"
                "  {\nk4: {\n<<: *base_1,<<: *base_2,a: 100}},\n"
                "  {\nk5: {\na: 100,<<: *base_2,<<: *base_1}},\n"
                "  {\nk6: {\n<<: *base_2,<<: *base_1,a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n!!merge <<: [*base_1,*base_2]}},\n"
                "  {\nk2: {\n!!merge <<: [*base_2,*base_1]}},\n"
                "  {\nk3: {\na: 100,!!merge <<: [*base_1,*base_2]}},\n"
                "  {\nk4: {\n!!merge <<: [*base_1,*base_2],a: 100}},\n"
                "  {\nk5: {\na: 100,!!merge <<: [*base_2,*base_1]}},\n"
                "  {\nk6: {\n!!merge <<: [*base_2,*base_1],a: 100}}\n"
                "]"
                ""),
        csubstr(""
                "[\n"
                "  {\nbase_1: &base_1 {\na: 10,b: 10}},\n"
                "  {\nbase_2: &base_2 {\nb: 20}},\n"
                "  {\nk1: {\n!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {\nk2: {\n!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {\nk3: {\na: 100,!!merge <<: *base_1,!!merge <<: *base_2}},\n"
                "  {\nk4: {\n!!merge <<: *base_1,!!merge <<: *base_2,a: 100}},\n"
                "  {\nk5: {\na: 100,!!merge <<: *base_2,!!merge <<: *base_1}},\n"
                "  {\nk6: {\n!!merge <<: *base_2,!!merge <<: *base_1,a: 100}}\n"
                "]"
                ""),
    };
    size_t count = 0;
    for(csubstr yaml : cases)
    {
        RYML_TRACE_FMT("case #{}", count++);
        TreeAndInts ti(to_csubstr(yaml));
        test_resolve_(ti);
        if(testing::Test::HasFailure()) break;
        test_emit_yaml_(ti,
                        ""
                        "[\n"
                        "  {\n    base_1: {\n      a: 10,\n      b: 10\n    }\n  },\n"
                        "  {\n    base_2: {\n      b: 20\n    }\n  },\n"
                        "  {\n    k1: {\n      a: 10,\n      b: 10\n    }\n  },\n"
                        "  {\n    k2: {\n      b: 20,\n      a: 10\n    }\n  },\n"
                        "  {\n    k3: {\n      a: 100,\n      b: 10\n    }\n  },\n"
                        "  {\n    k4: {\n      a: 100,\n      b: 10\n    }\n  },\n"
                        "  {\n    k5: {\n      a: 100,\n      b: 20\n    }\n  },\n"
                        "  {\n    k6: {\n      b: 20,\n      a: 100\n    }\n  }\n"
                        "]\n"
                        "");
        if(testing::Test::HasFailure()) break;
    }
}


TEST(anchors, issue_484_1)
{
    std::string yaml = ""
        "base_1: &base_1\n  a: a1\n"
        "base_2: &base_2\n  b: b2\n"
        "k1:\n  <<: *base_1\n  <<: *base_2\n"
        "k2:\n  <<: *base_2\n  <<: *base_1\n"
        "k3:\n  a: ak3\n  <<: *base_1\n  <<: *base_2\n"
        "k4:\n  <<: *base_1\n  <<: *base_2\n  a: ak4\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: a1\n  b: b2\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n"
                               "base_2:\n  b: b2\n"
                               "k1:\n  a: a1\n  b: b2\n"
                               "k2:\n  b: b2\n  a: a1\n"
                               "k3:\n  a: a1\n  b: b2\n"
                               "k4:\n  a: ak4\n  b: b2\n"
                               "");
}

TEST(anchors, issue_484_1_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1}},\n"
        "  {base_2: &base_2 {b: b2}},\n"
        "  {k1: {<<: *base_1,<<: *base_2}},\n"
        "  {k2: {<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {k4: {<<: *base_1,<<: *base_2,a: ak4}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: ak3,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: a1,b: b2}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1}},\n"
                               "  {base_2: {b: b2}},\n"
                               "  {k1: {a: a1,b: b2}},\n"
                               "  {k2: {b: b2,a: a1}},\n"
                               "  {k3: {a: a1,b: b2}},\n"
                               "  {k4: {a: ak4,b: b2}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_1_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\nbase_1: &base_1 {\na: a1}},\n"
        "  {\nbase_2: &base_2 {\nb: b2}},\n"
        "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
        "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
        "  {\nk3: {\na: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {\nk4: {\n<<: *base_1,<<: *base_2,a: ak4}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1\n    }\n  },\n"
                               "  {\n    base_2: {\n      b: b2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b2,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b2\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b2\n    }\n  }\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_2)
{
    std::string yaml = ""
        "base_1: &base_1\n  a: a1\n  b: b1\n"
        "base_2: &base_2\n  a: a2\n"
        "k1:\n  <<: *base_1\n  <<: *base_2\n"
        "k2:\n  <<: *base_2\n  <<: *base_1\n"
        "k3:\n  a: ak3\n  <<: *base_1\n  <<: *base_2\n"
        "k4:\n  <<: *base_1\n  <<: *base_2\n  a: ak4\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: a1\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  a: a2\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: a2\n  b: b1\n"
                               "k4:\n  a: a2\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "k1:\n  a: a2\n  b: b1\n"
                               "k2:\n  a: a1\n  b: b1\n"
                               "k3:\n  a: a2\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "");
}

TEST(anchors, issue_484_2_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1,b: b1}},\n"
        "  {base_2: &base_2 {a: a2}},\n"
        "  {k1: {<<: *base_1,<<: *base_2}},\n"
        "  {k2: {<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {k4: {<<: *base_1,<<: *base_2,a: ak4}},\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: a1,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {a: a2,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: a2,b: b1}},\n"
                               "  {k4: {a: a2,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {k1: {a: a2,b: b1}},\n"
                               "  {k2: {a: a1,b: b1}},\n"
                               "  {k3: {a: a2,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_2_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\nbase_1: &base_1 {\na: a1,b: b1}},\n"
        "  {\nbase_2: &base_2 {\na: a2}},\n"
        "  {\nk1: {\n<<: *base_1,<<: *base_2}},\n"
        "  {\nk2: {\n<<: *base_2,<<: *base_1}},\n"
        "  {\nk3: {\na: ak3,<<: *base_1,<<: *base_2}},\n"
        "  {\nk4: {\n<<: *base_1,<<: *base_2,a: ak4}},\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_3)
{
    std::string yaml = ""
        "base_1: &base_1\n" "  a: a1\n" "  b: b1\n"
        "base_2: &base_2\n" "  a: a2\n"
        "base_3: &base_3\n" "  b: b3\n"
        "k1:\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k2:\n" "  <<: *base_3\n" "  <<: *base_2\n" "  <<: *base_1\n"
        "k3:\n" "  a: ak3\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k4:\n" "  <<: *base_1\n" "  a: ak4\n" "  <<: *base_2\n" "  <<: *base_3\n"
        "k5:\n" "  <<: *base_1\n" "  <<: *base_2\n" "  <<: *base_3\n" "  a: ak5\n"
        "";
    test_resolve_with_options0(yaml,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "k5:\n  a: ak5\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: ak4\n  b: b1\n"
                               "k5:\n  a: ak5\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: a1\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "k5:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a1\n  b: b1\n"
                               "k2:\n  b: b3\n  a: a2\n"
                               "k3:\n  a: ak3\n  b: b1\n"
                               "k4:\n  a: a1\n  b: b1\n"
                               "k5:\n  a: a1\n  b: b1\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: ak3\n  b: b3\n"
                               "k4:\n  a: ak4\n  b: b3\n"
                               "k5:\n  a: ak5\n  b: b3\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: a2\n  b: b3\n"
                               "k4:\n  a: a2\n  b: b3\n"
                               "k5:\n  a: a2\n  b: b3\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "base_1:\n  a: a1\n  b: b1\n"
                               "base_2:\n  a: a2\n"
                               "base_3:\n  b: b3\n"
                               "k1:\n  a: a2\n  b: b3\n"
                               "k2:\n  b: b1\n  a: a1\n"
                               "k3:\n  a: a2\n  b: b3\n"
                               "k4:\n  a: a2\n  b: b3\n"
                               "k5:\n  a: ak5\n  b: b3\n"
                               "");
}

TEST(anchors, issue_484_3_flow_sl)
{
    std::string yaml = ""
        "[\n"
        "  {base_1: &base_1 {a: a1,b: b1}},\n"
        "  {base_2: &base_2 {a: a2}},\n"
        "  {base_3: &base_3 {b: b3}},\n"
        "  {k1: {<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {k2: {<<: *base_3,<<: *base_2,<<: *base_1}},\n"
        "  {k3: {a: ak3,<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {k4: {<<: *base_1,a: ak4,<<: *base_2,<<: *base_3}},\n"
        "  {k5: {<<: *base_1,<<: *base_2,<<: *base_3,a: ak5}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}},\n"
                               "  {k5: {a: ak5,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: ak4,b: b1}},\n"
                               "  {k5: {a: ak5,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: a1,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}},\n"
                               "  {k5: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a1,b: b1}},\n"
                               "  {k2: {b: b3,a: a2}},\n"
                               "  {k3: {a: ak3,b: b1}},\n"
                               "  {k4: {a: a1,b: b1}},\n"
                               "  {k5: {a: a1,b: b1}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: ak3,b: b3}},\n"
                               "  {k4: {a: ak4,b: b3}},\n"
                               "  {k5: {a: ak5,b: b3}}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: a2,b: b3}},\n"
                               "  {k4: {a: a2,b: b3}},\n"
                               "  {k5: {a: a2,b: b3}}\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {base_1: {a: a1,b: b1}},\n"
                               "  {base_2: {a: a2}},\n"
                               "  {base_3: {b: b3}},\n"
                               "  {k1: {a: a2,b: b3}},\n"
                               "  {k2: {b: b1,a: a1}},\n"
                               "  {k3: {a: a2,b: b3}},\n"
                               "  {k4: {a: a2,b: b3}},\n"
                               "  {k5: {a: ak5,b: b3}}\n"
                               "]\n"
                               "");
}

TEST(anchors, issue_484_3_flow_ml)
{
    std::string yaml = ""
        "[\n"
        "  {\n    base_1: &base_1 {\na: a1,b: b1}},\n"
        "  {\n    base_2: &base_2 {\na: a2}},\n"
        "  {\n    base_3: &base_3 {\nb: b3}},\n"
        "  {\n    k1: {\n<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k2: {\n<<: *base_3,<<: *base_2,<<: *base_1}},\n"
        "  {\n    k3: {\na: ak3,<<: *base_1,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k4: {\n<<: *base_1,a: ak4,<<: *base_2,<<: *base_3}},\n"
        "  {\n    k5: {\n<<: *base_1,<<: *base_2,<<: *base_3,a: ak5}}\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both, opts_pri_none,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b3,\n      a: a2\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b1\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a1,\n      b: b1\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_orig_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: ak3,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: ak4,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_pri_merg_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: a2,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
    test_resolve_with_options2(yaml, opts_pri_both_last, opts_pri_none_last,
                               "[\n"
                               "  {\n    base_1: {\n      a: a1,\n      b: b1\n    }\n  },\n"
                               "  {\n    base_2: {\n      a: a2\n    }\n  },\n"
                               "  {\n    base_3: {\n      b: b3\n    }\n  },\n"
                               "  {\n    k1: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k2: {\n      b: b1,\n      a: a1\n    }\n  },\n"
                               "  {\n    k3: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k4: {\n      a: a2,\n      b: b3\n    }\n  },\n"
                               "  {\n    k5: {\n      a: ak5,\n      b: b3\n    }\n  }\n"
                               "]\n"
                               "");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(anchors, keep_repeated_0)
{
    std::string yaml = ""
        "{\n"
        "  m0: &m0 {k0: 00,k1: 01,k2: 02,k3: 03},\n"
        "  m1: &m1 {k0: 10,k1: 11,k2: 12,k3: 13},\n"
        "  m2: &m2 {k0: 20,k1: 21,k2: 22,k3: 23},\n"
        "  m3: &m3 {k0: 30,k1: 31,k2: 32,k3: 33},\n"
        "  r0: {first: y,<<: *m0,foo: b,<<: *m1,bar: b,<<: *m2,baz: b,<<: *m3,last: y},\n"
        "  r1: {first: y,<<: *m3,foo: b,<<: *m2,bar: b,<<: *m1,baz: b,<<: *m0,last: y},\n"
        "  r2: {first: y,<<: [*m0,*m1,*m2,*m3],last: y},\n"
        "  r3: {first: y,<<: [*m3,*m2,*m1,*m0],last: y},\n"
        "}\n"
        "";
    test_resolve_with_options0(yaml,
                               "{\n"
                               "  m0: {k0: 00,k1: 01,k2: 02,k3: 03},\n"
                               "  m1: {k0: 10,k1: 11,k2: 12,k3: 13},\n"
                               "  m2: {k0: 20,k1: 21,k2: 22,k3: 23},\n"
                               "  m3: {k0: 30,k1: 31,k2: 32,k3: 33},\n"
                               "  r0: {first: y,k0: 00,k1: 01,k2: 02,k3: 03,foo: b,bar: b,baz: b,last: y},\n"
                               "  r1: {first: y,k0: 30,k1: 31,k2: 32,k3: 33,foo: b,bar: b,baz: b,last: y},\n"
                               "  r2: {first: y,k0: 00,k1: 01,k2: 02,k3: 03,last: y},\n"
                               "  r3: {first: y,k0: 30,k1: 31,k2: 32,k3: 33,last: y}\n"
                               "}\n"
                               "");
    test_resolve_with_options_(yaml, opts_keep,
                               "{\n"
                               "  m0: {k0: 00,k1: 01,k2: 02,k3: 03},\n"
                               "  m1: {k0: 10,k1: 11,k2: 12,k3: 13},\n"
                               "  m2: {k0: 20,k1: 21,k2: 22,k3: 23},\n"
                               "  m3: {k0: 30,k1: 31,k2: 32,k3: 33},\n"
                               "  r0: {first: y,k0: 00,k1: 01,k2: 02,k3: 03,foo: b,k0: 10,k1: 11,k2: 12,k3: 13,bar: b,k0: 20,k1: 21,k2: 22,k3: 23,baz: b,k0: 30,k1: 31,k2: 32,k3: 33,last: y},\n"
                               "  r1: {first: y,k0: 30,k1: 31,k2: 32,k3: 33,foo: b,k0: 20,k1: 21,k2: 22,k3: 23,bar: b,k0: 10,k1: 11,k2: 12,k3: 13,baz: b,k0: 00,k1: 01,k2: 02,k3: 03,last: y},\n"
                               "  r2: {first: y,k0: 00,k1: 01,k2: 02,k3: 03,k0: 10,k1: 11,k2: 12,k3: 13,k0: 20,k1: 21,k2: 22,k3: 23,k0: 30,k1: 31,k2: 32,k3: 33,last: y},\n"
                               "  r3: {first: y,k0: 30,k1: 31,k2: 32,k3: 33,k0: 20,k1: 21,k2: 22,k3: 23,k0: 10,k1: 11,k2: 12,k3: 13,k0: 00,k1: 01,k2: 02,k3: 03,last: y}\n"
                               "}\n"
                               "");
}

TEST(anchors, keep_repeated_1_anch_tag)
{
    std::string yaml = ""
        "[\n"
        "  &m0 {k0: 0,k1: 1,k2: 2,k3: 3},\n"
        "  {k0: y,&anc <<: *m0},\n"
        "  {k0: y,!tag <<: *m0},\n"
        "  {k0: y,&a2 !tag <<: *m0},\n"
        "]\n"
        "";
    test_resolve_with_options0(yaml,
                               "[\n"
                               "  {k0: 0,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k1: 1,k2: 2,k3: 3}\n"
                               "]\n"
                               "");
    test_resolve_with_options_(yaml, opts_keep,
                               "[\n"
                               "  {k0: 0,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k0: 0,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k0: 0,k1: 1,k2: 2,k3: 3},\n"
                               "  {k0: y,k0: 0,k1: 1,k2: 2,k3: 3}\n"
                               "]\n"
                               "");
}

} // namespace yml
} // namespace c4
