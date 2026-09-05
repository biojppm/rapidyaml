#ifndef TEST_TREE_AND_INTS_HPP_
#define TEST_TREE_AND_INTS_HPP_

#include <c4/yml/parse.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/emit.hpp>
#include <test_lib/test_events_ints_helpers.hpp>
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

inline void parse_ints(substr src, extra::ievt::Buffers *ints, ParserOptions const& opts={})
{
    using Handler = extra::ievt::EventHandlerInts<true>;
    Handler handler;
    ParseEngine<Handler> parser(&handler, opts);
    handler.reset(src);
    parser.parse_in_place_ev("(testyaml)", src);
    *ints = handler.get_buffers(true);
}
inline TreeAndInts parse_tree_and_ints(csubstr src, ParserOptions const& opts={})
{
    TreeAndInts ret;
    parse_in_arena(src, &ret.tree, opts);
    ret.src_ints.assign(src.str, src.len);
    parse_ints(to_substr(ret.src_ints), &ret.ints, opts);
    return ret;
}

inline void test_emit_yaml(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        EXPECT_EQ(emitrs_yaml<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
inline void test_emit_yaml(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos={}", pos);
        EXPECT_EQ(ints.emit_yaml<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
inline void test_emit_yaml(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(n, expected, opts);
}
inline void test_emit_yaml(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(ints, pos, expected, opts);
}
inline void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    test_emit_yaml(n, expected, opts);
    test_emit_yaml(ints, pos, expected, opts);
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


inline void test_emit_json(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id={}", n.id());
        EXPECT_EQ(emitrs_json<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
inline void test_emit_json(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos={}", pos);
        EXPECT_EQ(ints.emit_json<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
inline void test_emit_json(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(n, expected, opts);
}
inline void test_emit_json(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(ints, pos, expected, opts);
}
inline void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={})
{
    test_emit_json(n, expected, opts);
    test_emit_json(ints, pos, expected, opts);
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


} // namespace yml
} // namespace c4

#endif /* TEST_TREE_AND_INTS_HPP_ */
