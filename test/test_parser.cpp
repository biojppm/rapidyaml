#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/parse.hpp"
#endif
#include <gtest/gtest.h>
#include "./callbacks_tester.hpp"


namespace c4 {
namespace yml {

// TODO: add this as a method to csubstr
bool is_same(csubstr lhs, csubstr rhs)
{
    return lhs.str == rhs.str && lhs.len == rhs.len;
}

void mklarge(Parser *p, Callbacks const& cb)
{
    p->~Parser();
    new ((void*)p) Parser(cb);
    p->reserve_stack(20); // cause an allocation
    p->reserve_locations(128); // cause an allocation
    p->reserve_filter_arena(128); // cause an allocation
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Parser, empty_ctor)
{
    Parser parser;
    EXPECT_EQ(parser.callbacks(), get_callbacks());
}

TEST(Parser, callbacks_ctor)
{
    CallbacksTester cbt;
    {
        Parser parser(cbt.callbacks());
        EXPECT_EQ(parser.callbacks(), cbt.callbacks());
    }
    EXPECT_EQ(cbt.num_allocs, 0u);
    EXPECT_EQ(cbt.num_deallocs, 0u);
}

TEST(Parser, reserve_capacity)
{
    CallbacksTester cbt("test", 20000/*Bytes*/);
    {
        Parser parser(cbt.callbacks());
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
        Parser parser(ts.callbacks());
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

TEST(Parser, reserve_filter_arena)
{
    size_t cap = 256u;
    CallbacksTester ts;
    {
        Parser parser(ts.callbacks());
        EXPECT_EQ(parser.filter_arena_capacity(), 0u);
        EXPECT_EQ(parser.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        parser.reserve_filter_arena(cap);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(ts.alloc_size, cap);
        EXPECT_EQ(ts.dealloc_size, 0u);
    }
    EXPECT_EQ(ts.num_allocs, 1u);
    EXPECT_EQ(ts.num_deallocs, 1u);
    EXPECT_EQ(ts.alloc_size, cap);
    EXPECT_EQ(ts.dealloc_size, cap);
}

TEST(Parser, copy_ctor)
{
    {
        Parser src;
        mklarge(&src, get_callbacks());
        EXPECT_EQ(src.callbacks(), get_callbacks());
        Parser dst(src);
        EXPECT_EQ(src.callbacks(), get_callbacks());
        EXPECT_EQ(dst.callbacks(), get_callbacks());
    }
    {
        CallbacksTester ts;
        {
            Parser src;
            mklarge(&src, ts.callbacks());
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
}

TEST(Parser, move_ctor)
{
    {
        Parser src;
        mklarge(&src, get_callbacks());
        EXPECT_EQ(src.callbacks(), get_callbacks());
        Parser dst(std::move(src));
        EXPECT_EQ(src.callbacks(), get_callbacks());
        EXPECT_EQ(dst.callbacks(), get_callbacks());
    }
    {
        CallbacksTester ts;
        {
            Parser src;
            mklarge(&src, ts.callbacks());
            ASSERT_EQ(src.callbacks(), ts.callbacks());
            size_t nbefore = ts.num_allocs;
            EXPECT_GT(ts.num_allocs, 0u);
            Parser dst(std::move(src));
            ASSERT_EQ(src.callbacks(), ts.callbacks());
            ASSERT_EQ(dst.callbacks(), ts.callbacks());
            EXPECT_EQ(ts.num_allocs, nbefore);
        }
        EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
        EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
    }
}

TEST(Parser, copy_assign_same_callbacks)
{
    CallbacksTester ts;
    {
        Parser src;
        Parser dst;
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
        Parser src;
        Parser dst;
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
        Parser src;
        Parser dst;
        mklarge(&src, ts.callbacks());
        mklarge(&dst, ts.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(ts.num_allocs, 0u);
        dst = std::move(src);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
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
        Parser src;
        Parser dst;
        mklarge(&src, ts.callbacks());
        mklarge(&dst, td.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), td.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(td.num_allocs, 0u);
        dst = std::move(src);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(td.num_allocs, nbefore); // dst frees with td
        EXPECT_EQ(ts.num_allocs, nbefore); // dst moves from ts
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
            Parser parser;
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parser.parse_in_arena("file0", csrc);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser parser(cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parser.parse_in_arena("file1", csrc);
            EXPECT_EQ(t.callbacks(), cbt.callbacks());
        }
        cbt.check();
    }
    {
        {
            Parser parser;
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parser.parse_in_place("file", src);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser parser(cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parser.parse_in_place("file", src);
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
            Parser parser(cbp.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parser.parse_in_arena("file", csrc, &tree);
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
            Parser parser(cbp.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parser.parse_in_place("file", src, &tree);
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
    Parser parser;
    EXPECT_EQ(parser.filename(), csubstr{});
    {
        Tree tree = parser.parse_in_place("file0", src);
        EXPECT_EQ(parser.filename(), "file0");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parser.parse_in_arena("file1", csrc);
        EXPECT_EQ(parser.filename(), "file1");
        EXPECT_TRUE(!is_same(parser.source(), src));
    }
    {
        Tree tree = parser.parse_in_place("file2", src);
        EXPECT_EQ(parser.filename(), "file2");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parser.parse_in_arena({}, csrc);
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
} // namespace yml
} // namespace c4
