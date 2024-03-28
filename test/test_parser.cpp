#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/node.hpp"
#endif
#include <gtest/gtest.h>
#include "./test_lib/callbacks_tester.hpp"


namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

// TODO: add this as a method to csubstr
bool is_same(csubstr lhs, csubstr rhs)
{
    return lhs.str == rhs.str && lhs.len == rhs.len;
}

void mklarge(Parser *p, Callbacks const& cb)
{
    ASSERT_TRUE(p->m_evt_handler);
    Parser::handler_type *evt_handler = p->m_evt_handler;
    p->~Parser();
    evt_handler->m_stack.m_callbacks = cb;
    new ((void*)p) Parser(evt_handler, p->options());
    p->reserve_stack(20); // cause an allocation
    p->reserve_locations(128); // cause an allocation
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Parser, empty_ctor)
{
    Parser::handler_type evt_handler = {};
    EXPECT_EQ(evt_handler.m_stack.m_callbacks, get_callbacks());
    Parser parser(&evt_handler);
    EXPECT_EQ(parser.callbacks(), get_callbacks());
}

TEST(Parser, callbacks_ctor)
{
    CallbacksTester cbt;
    {
        Parser::handler_type evt_handler = {cbt.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(parser.callbacks(), cbt.callbacks());
    }
    EXPECT_EQ(cbt.num_allocs, 0u);
    EXPECT_EQ(cbt.num_deallocs, 0u);
}

TEST(Parser, reserve_capacity)
{
    CallbacksTester cbt("test", 30000/*Bytes*/);
    {
        Parser::handler_type evt_handler = {cbt.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(cbt.num_allocs, 0u);
        EXPECT_EQ(cbt.num_deallocs, 0u);
        parser.reserve_stack(18);
        EXPECT_EQ(cbt.num_allocs, 1u);
        EXPECT_EQ(cbt.num_deallocs, 0u);
        parser.reserve_stack(24);
        EXPECT_EQ(cbt.num_allocs, 2u);
        EXPECT_EQ(cbt.num_deallocs, 1u);
        parser.reserve_stack(28);
        EXPECT_EQ(cbt.num_allocs, 3u);
        EXPECT_EQ(cbt.num_deallocs, 2u);
    }
    EXPECT_EQ(cbt.num_allocs, 3u);
    EXPECT_EQ(cbt.num_deallocs, 3u);
    cbt.check();
}

TEST(Parser, reserve_locations)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(parser.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        parser.reserve_locations(128);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(ts.alloc_size, 128u * sizeof(size_t));
        EXPECT_EQ(ts.dealloc_size, 0u);
    }
    EXPECT_EQ(ts.num_allocs, 1u);
    EXPECT_EQ(ts.num_deallocs, 1u);
    EXPECT_EQ(ts.alloc_size, 128u * sizeof(size_t));
    EXPECT_EQ(ts.dealloc_size, 128u * sizeof(size_t));
}

TEST(Parser, copy_ctor_0)
{
    Parser::handler_type evt_handler = {};
    EXPECT_EQ(evt_handler.m_stack.m_callbacks, get_callbacks());
    Parser src(&evt_handler);
    mklarge(&src, evt_handler.m_stack.m_callbacks);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    Parser dst(src);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    EXPECT_EQ(dst.callbacks(), get_callbacks());
}

TEST(Parser, copy_ctor_1)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser src(&evt_handler);
        mklarge(&src, evt_handler.m_stack.m_callbacks);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        Parser dst(src);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, move_ctor_0)
{
    Parser::handler_type evt_handler = {};
    Parser src(&evt_handler);
    mklarge(&src, evt_handler.m_stack.m_callbacks);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    Parser dst(std::move(src));
    //EXPECT_EQ(src.callbacks(), get_callbacks());
    EXPECT_EQ(dst.callbacks(), get_callbacks());
}
TEST(Parser, move_ctor_1)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser src(&evt_handler);
        mklarge(&src, evt_handler.m_stack.m_callbacks);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        Parser dst(std::move(src));
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, copy_assign_same_callbacks)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {ts.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, ts.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(ts.num_allocs, 0u);
        dst = src;
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, copy_assign_diff_callbacks)
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {td.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, td.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), td.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(td.num_allocs, 0u);
        dst = src;
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
        EXPECT_EQ(td.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
    EXPECT_EQ(td.num_allocs, td.num_deallocs);
    EXPECT_EQ(td.alloc_size, td.dealloc_size);
}

TEST(Parser, move_assign_same_callbacks)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {ts.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, ts.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(ts.num_allocs, 0u);
        dst = std::move(src);
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, move_assign_diff_callbacks)
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {td.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, td.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), td.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(td.num_allocs, 0u);
        dst = std::move(src);
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
    EXPECT_EQ(td.num_allocs, td.num_deallocs);
    EXPECT_EQ(td.alloc_size, td.dealloc_size);
}

TEST(Parser, new_tree_receives_callbacks)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    {
        {
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parse_in_arena(&parser, "file0", csrc);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser::handler_type evt_handler = {cbt.callbacks()};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parse_in_arena(&parser, "file1", csrc);
            EXPECT_EQ(t.callbacks(), cbt.callbacks());
        }
        cbt.check();
    }
    {
        {
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parse_in_place(&parser, "file", src);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser::handler_type evt_handler = {cbt.callbacks()};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parse_in_place(&parser, "file", src);
            EXPECT_EQ(t.callbacks(), cbt.callbacks());
        }
        cbt.check();
    }
}

TEST(Parser, existing_tree_overwrites_parser_callbacks)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    {
        CallbacksTester cbp("parser");
        CallbacksTester cbt("tree");
        {
            Tree tree(cbt.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            Parser::handler_type evt_handler(cbp.callbacks());
            EXPECT_EQ(evt_handler.m_stack.m_callbacks, cbp.callbacks());
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parse_in_arena(&parser, "file", csrc, &tree);
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
        }
        cbp.check();
        cbt.check();
    }
    {
        CallbacksTester cbp("parser");
        CallbacksTester cbt("tree");
        {
            Tree tree(cbt.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            Parser::handler_type evt_handler(cbp.callbacks());
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parse_in_place(&parser, "file", src, &tree);
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
        }
        cbp.check();
        cbt.check();
    }
}

TEST(Parser, filename_and_buffer_are_stored)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler);
    EXPECT_EQ(parser.filename(), csubstr{});
    {
        Tree tree = parse_in_place(&parser, "file0", src);
        EXPECT_EQ(parser.filename(), "file0");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_arena(&parser, "file1", csrc);
        EXPECT_EQ(parser.filename(), "file1");
        EXPECT_TRUE(!is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_place(&parser, "file2", src);
        EXPECT_EQ(parser.filename(), "file2");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_arena(&parser, csrc);
        EXPECT_EQ(parser.filename(), csubstr{});
        EXPECT_TRUE(!is_same(parser.source(), src));
    }
}

TEST(parse_in_place, overloads)
{
    char src1_[] = "{a: b}";
    char src2_[] = "{c: d, e: {}}";
    {
        Tree tree = parse_in_place(src1_);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src1", src1_);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree;
        parse_in_place(src1_, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree;
        parse_in_place("src1", src1_, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place(src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        size_t e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (size_t)NONE);
        parse_in_place(src1_, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src2", src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        size_t e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (size_t)NONE);
        parse_in_place("src1", src1_, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place(src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        parse_in_place(src1_, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src2", src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        parse_in_place("src1", src1_, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
}

TEST(parse_in_arena, overloads)
{
    csubstr src1 = "{a: b}";
    csubstr src2 = "{c: d, e: {}}";
    {
        Tree tree = parse_in_arena(src1);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
    }
    {
        Tree tree = parse_in_arena("src1", src1);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
    }
    {
        Tree tree;
        parse_in_arena(src1, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
    }
    {
        Tree tree;
        parse_in_arena("src1", src1, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
    }
    {
        Tree tree = parse_in_arena(src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
        size_t e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (size_t)NONE);
        parse_in_arena(src1, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
    }
    {
        Tree tree = parse_in_arena("src2", src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
        size_t e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (size_t)NONE);
        parse_in_arena("src1", src1, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
    }
    {
        Tree tree = parse_in_arena(src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
        parse_in_arena(src1, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
    }
    {
        Tree tree = parse_in_arena("src2", src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
        parse_in_arena("src1", src1, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), (size_t)npos);
        EXPECT_NE(tree.arena().find(src2), (size_t)npos);
    }
}

TEST(parse_in_place, version_numbers)
{
    char src1_[] = "{a: 1.2.3}";
    {
        Tree tree = parse_in_place(src1_);
        EXPECT_EQ(tree["a"].val(), "1.2.3");
    }
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

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4
