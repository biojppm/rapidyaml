#ifndef TEST_TREE_AND_INTS_HPP_
#define TEST_TREE_AND_INTS_HPP_

#include <c4/yml/parse.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/emit.hpp>
#include <test_lib/test_events_ints_helpers.hpp>
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
inline void test_emit(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={})
{
    {
        SCOPED_TRACE("tree");
        EXPECT_EQ(emitrs_yaml<std::string>(ti.tree, opts), expected);
    }
    {
        SCOPED_TRACE("ints");
        EXPECT_EQ(ti.ints.emit_yaml<std::string>(opts), expected);
    }
}
inline void test_emit_json(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={})
{
    {
        SCOPED_TRACE("tree");
        EXPECT_EQ(emitrs_json<std::string>(ti.tree, opts), expected);
    }
    {
        SCOPED_TRACE("ints");
        EXPECT_EQ(ti.ints.emit_json<std::string>(opts), expected);
    }
}


} // namespace yml
} // namespace c4

#endif /* TEST_TREE_AND_INTS_HPP_ */
