#ifndef TEST_TREE_AND_INTS_HPP_
#define TEST_TREE_AND_INTS_HPP_

#include <test_lib/test_events_ints_helpers.hpp>
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
    std::string src_ints;
};

inline TreeAndInts parse_tree_and_ints(csubstr src, ParserOptions const& opts={})
{
    TreeAndInts ret;
    parse_in_arena(src, &ret.tree, opts);
    ret.src_ints.assign(src.str, src.len);
    parse_ints(to_substr(ret.src_ints), &ret.ints, opts);
    return ret;
}


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
        EXPECT_EQ(ints.emit_yaml<std::string>(opts, pos), expected);
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
    test_emit_yaml_tree(n, expected, opts);
    test_emit_yaml_ints(ints, pos, expected, opts);
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
        EXPECT_EQ(ints.emit_json<std::string>(opts, pos), expected);
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
        ints.emit_yaml<std::string>(opts, pos);
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
        ints.emit_json<std::string>(opts, pos);
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


} // namespace yml
} // namespace c4

#endif /* TEST_TREE_AND_INTS_HPP_ */
