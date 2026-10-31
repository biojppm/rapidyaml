#ifndef TEST_TREE_AND_INTS_HPP_
#define TEST_TREE_AND_INTS_HPP_

#include <test_lib/test_helpers_ints.hpp>
#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/parse.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/emit.hpp>
#endif

#include <test_lib/test_case.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {

using IntBufs = extra::ievt::TestBuffers;
using IntBufsCR = IntBufs const&;

struct TreeAndInts
{
    Tree    tree;
    IntBufs ints;
    txtbuf  src_ints;
    TreeAndInts(csubstr src, ParserOptions const& opts={}) : tree(), ints(), src_ints()
    {
        parse(src, opts);
    }
    void parse(csubstr src, ParserOptions const& opts={})
    {
        parse_in_arena(src, &tree, opts);
        src_ints.assign(src.str, src.len);
        parse_ints(src_ints, &ints, opts);
    }
    TreeAndInts() noexcept : tree(), ints(), src_ints() {}
    TreeAndInts(TreeAndInts &&that) noexcept
        : tree(std::move(that.tree))
        , ints(std::move(that.ints))
        , src_ints(std::move(that.src_ints))
    {
        ints.src = src_ints;
    }
    TreeAndInts(TreeAndInts const& that) = delete;
    TreeAndInts& operator= (TreeAndInts &&that) noexcept = delete;
    TreeAndInts& operator= (TreeAndInts const& that) noexcept = delete;
};


#define test_resolve_tree_(...) SCOPED_CALL(test_resolve_tree, (__VA_ARGS__))
#define test_resolve_ints_(...) SCOPED_CALL(test_resolve_ints, (__VA_ARGS__))
#define test_resolve_(...) SCOPED_CALL(test_resolve, (__VA_ARGS__))
#define test_resolve_err_tree_(...) SCOPED_CALL(test_resolve_err_tree, (__VA_ARGS__))
#define test_resolve_err_ints_(...) SCOPED_CALL(test_resolve_err_ints, (__VA_ARGS__))
#define test_resolve_err_(...) SCOPED_CALL(test_resolve_err, (__VA_ARGS__))


#define test_emit_yaml_(...) SCOPED_CALL(test_emit_yaml, (__VA_ARGS__))
#define test_emit_yaml_tree_(...) SCOPED_CALL(test_emit_yaml_tree, (__VA_ARGS__))
#define test_emit_yaml_ints_(...) SCOPED_CALL(test_emit_yaml_ints, (__VA_ARGS__))
#define test_emit_yaml_same_ints_(...) SCOPED_CALL(test_emit_yaml_same_ints, (__VA_ARGS__))
#define test_emit_error_yaml_(...) SCOPED_CALL(test_emit_error_yaml, (__VA_ARGS__))
#define test_emit_error_yaml_tree_(...) SCOPED_CALL(test_emit_error_yaml_tree, (__VA_ARGS__))
#define test_emit_error_yaml_ints_(...) SCOPED_CALL(test_emit_error_yaml_ints, (__VA_ARGS__))


#define test_emit_json_(...) SCOPED_CALL(test_emit_json, (__VA_ARGS__))
#define test_emit_json_tree_(...) SCOPED_CALL(test_emit_json_tree, (__VA_ARGS__))
#define test_emit_json_ints_(...) SCOPED_CALL(test_emit_json_ints, (__VA_ARGS__))
#define test_emit_json_same_ints_(...) SCOPED_CALL(test_emit_json_same_ints, (__VA_ARGS__))
#define test_emit_error_json_(...) SCOPED_CALL(test_emit_error_json, (__VA_ARGS__))
#define test_emit_error_json_tree_(...) SCOPED_CALL(test_emit_error_json_tree, (__VA_ARGS__))
#define test_emit_error_json_ints_(...) SCOPED_CALL(test_emit_error_json_ints, (__VA_ARGS__))



//-----------------------------------------------------------------------------


inline void test_resolve_tree(Tree &tree, ResolveOptions const& opts={})
{
    SCOPED_TRACE("test_resolve_tree");
    {
        SCOPED_TRACE("invariants before");
        test_invariants(tree);
    }
    RYML_EXPECT_ERROR(check_success(&tree, [&]{
        tree.resolve(opts);
    }));
    {
        SCOPED_TRACE("invariants after");
        test_invariants(tree);
    }
    if(testing::Test::HasFailure())
        print_tree(tree);
}
inline void test_resolve_ints(IntBufs &ints, ResolveOptions const& opts={})
{
    SCOPED_TRACE("test_resolve_ints");
    {
        SCOPED_TRACE("invariants before");
        ints.test_invariants();
    }
    if(testing::Test::HasFailure())
    {
        ints.print();
        return;
    }
    RYML_EXPECT_ERROR(check_success([&]{
        ints.resolve(opts);
    }));
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("invariants after");
        ints.test_invariants();
    }
    if(testing::Test::HasFailure())
        ints.print();
}
inline void test_resolve(TreeAndInts &ti, ResolveOptions const& opts={})
{
    test_resolve_tree(ti.tree, opts);
    test_resolve_ints(ti.ints, opts);
}


inline void test_resolve_err_tree(Tree &tree, ResolveOptions const& opts={})
{
    SCOPED_TRACE("test_resolve_err_tree");
    bool failed = false;
    RYML_EXPECT_ERROR(check_error_visit(&tree, [&]{
        tree.resolve(opts);
        // if we reach this, it's a failure
        failed = true;
    }));
    if(failed)
        print_tree(tree);
}
inline void test_resolve_err_ints(IntBufs &ints, ResolveOptions const& opts={})
{
    SCOPED_TRACE("test_resolve_err_ints");
    bool failed = false;
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.resolve(opts);
        // if we reach this, it's a failure
        failed = true;
    }));
    if(failed)
        ints.print();
}
inline void test_resolve_err(TreeAndInts &ti, ResolveOptions const& opts={})
{
    test_resolve_err_tree(ti.tree, opts);
    test_resolve_err_ints(ti.ints, opts);
}


//-----------------------------------------------------------------------------

inline void test_emit_yaml_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        EXPECT_EQ(emitrs_yaml<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
inline void test_emit_yaml_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml_tree(n, expected, opts);
}


inline void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos={}", pos);
        EXPECT_EQ(ints.emitrs_yaml<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
inline void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml_ints(ints, pos, expected, opts);
}


inline void test_emit_yaml_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        std::string emitted_tree = emitrs_yaml<std::string>(n, opts);
        EXPECT_EQ(emitted_tree, expected);
        if(testing::Test::HasFailure())
        {
            print_tree(*n.tree());
            return;
        }
        {
            SCOPED_TRACE("here");
            IntBufs ints;
            parse_ints(to_substr(emitted_tree), &ints);
            test_emit_yaml_ints(ints, 0, expected, opts);
        }
    }
}

inline void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    SCOPED_CALL(test_emit_yaml_ints, (ints, pos, expected, opts));
    SCOPED_CALL(test_emit_yaml_tree, (n, expected, opts));
}
inline void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(n, ints, pos, expected, opts);
}
inline void test_emit_yaml(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={})
{
    test_emit_yaml(ti.tree.rootref(), ti.ints, 0, expected, opts);
}
inline void test_emit_yaml(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(ti.tree.rootref(), ti.ints, 0, expected, opts);
}


inline void test_emit_json_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        EXPECT_EQ(emitrs_json<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
inline void test_emit_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos={}", pos);
        EXPECT_EQ(ints.emitrs_json<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
inline void test_emit_json_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        std::string emitted_tree = emitrs_json<std::string>(n, opts);
        EXPECT_EQ(emitted_tree, expected);
        if(testing::Test::HasFailure())
        {
            print_tree(*n.tree());
            return;
        }
        {
            SCOPED_TRACE("here");
            IntBufs ints;
            parse_ints(to_substr(emitted_tree), &ints);
            test_emit_json_ints(ints, 0, expected, opts);
        }
    }
}
inline void test_emit_json_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json_tree(n, expected, opts);
}
inline void test_emit_json(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json_ints(ints, pos, expected, opts);
}
inline void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    test_emit_json_tree(n, expected, opts);
    test_emit_json_ints(ints, pos, expected, opts);
}
inline void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(n, ints, pos, expected, opts);
}
inline void test_emit_json(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={})
{
    test_emit_json(ti.tree.rootref(), ti.ints, 0, expected, opts);
}
inline void test_emit_json(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(ti.tree.rootref(), ti.ints, 0, opts, expected);
}



inline void test_emit_error_yaml_tree(NodeRef n, EmitOptions const& opts={})
{
    RYML_TRACE_FMT("tree: id={}", n.id());
    RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
        emitrs_yaml<std::string>(n, opts);
        // if we reach this, it's a failure
        GTEST_FAIL();
        print_tree(*n.tree());
    }));
}
inline void test_emit_error_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={})
{
    RYML_TRACE_FMT("ints: pos={}", pos);
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.emitrs_yaml<std::string>(opts, pos);
        // if we reach this, it's a failure
        GTEST_FAIL();
        ints.print();
    }));
}
inline void test_emit_error_yaml_ints(IntBufsCR ints, EmitOptions const& opts={})
{
    test_emit_error_yaml_ints(ints, 0, opts);
}

inline void test_emit_error_yaml(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={})
{
    test_emit_error_yaml_tree(n, opts);
    test_emit_error_yaml_ints(ints, pos, opts);
}
inline void test_emit_error_yaml(NodeRef n, IntBufsCR ints, EmitOptions const& opts={})
{
    test_emit_error_yaml(n, ints, 0, opts);
}
inline void test_emit_error_yaml(TreeAndInts & ti, EmitOptions const& opts={})
{
    test_emit_error_yaml(ti.tree.rootref(), ti.ints, 0, opts);
}

inline void test_emit_error_json_tree(NodeRef n, EmitOptions const& opts={})
{
    RYML_TRACE_FMT("tree: id={}", n.id());
    RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
        emitrs_json<std::string>(n, opts);
        // if we reach this, it's a failure
        print_tree(*n.tree());
    }));
}
inline void test_emit_error_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={})
{
    RYML_TRACE_FMT("ints: pos={}", pos);
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.emitrs_json<std::string>(opts, pos);
        // if we reach this, it's a failure
        ints.print();
    }));
}
inline void test_emit_error_json_ints(IntBufsCR ints, EmitOptions const& opts={})
{
    test_emit_error_json_ints(ints, 0, opts);
}

inline void test_emit_error_json(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={})
{
    test_emit_error_json_tree(n, opts);
    test_emit_error_json_ints(ints, pos, opts);
}
inline void test_emit_error_json(NodeRef n, IntBufsCR ints, EmitOptions const& opts={})
{
    test_emit_error_json(n, ints, 0, opts);
}
inline void test_emit_error_json(TreeAndInts & ti, EmitOptions const& opts={})
{
    test_emit_error_json(ti.tree.rootref(), ti.ints, 0, opts);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline void test_resolve(ResolveOptions opts, std::string const& yaml, std::string const& resolved={})
{
    TreeAndInts ti(to_csubstr(yaml));
    SCOPED_CALL(test_emit_yaml, (ti, yaml));
    SCOPED_CALL(test_resolve, (ti, opts));
    if(!resolved.empty())
        SCOPED_CALL(test_emit_yaml, (ti, resolved));
    else
        SCOPED_CALL(test_emit_yaml, (ti, yaml));
}
inline void test_resolve(std::string const& yaml, std::string const& resolved={})
{
    test_resolve(ResolveOptions{}, yaml, resolved);
}
inline void test_resolve_err(std::string const& yaml, ResolveOptions opts={})
{
    TreeAndInts ti(to_csubstr(yaml));
    SCOPED_CALL(test_resolve_err, (ti, opts));
}
inline void test_emit_resolve_err(std::string const& yaml, std::string const& json, ResolveOptions opts={})
{
    TreeAndInts ti(to_csubstr(yaml));
    SCOPED_CALL(test_emit_yaml, (ti, yaml));
    SCOPED_CALL(test_emit_json, (ti, json));
    SCOPED_CALL(test_resolve_err, (ti, opts));
}
inline void test_emit_resolve(std::string const& src, std::string const& yaml, std::string const& json, ResolveOptions opts={})
{
    TreeAndInts ti(to_csubstr(src));
    SCOPED_CALL(test_resolve, (ti, opts));
    SCOPED_CALL(test_emit_yaml, (ti, yaml));
    SCOPED_CALL(test_emit_json, (ti, json));
}

} // namespace yml
} // namespace c4

#endif /* TEST_TREE_AND_INTS_HPP_ */
