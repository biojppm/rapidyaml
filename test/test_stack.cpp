#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/detail/stack.hpp"
#endif
#include <gtest/gtest.h>
#include "./test_lib/callbacks_tester.hpp"


//-------------------------------------------

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace detail {

template<id_type N>
using istack = stack<int, N>;
using ip = int const*;

template<id_type N>
void to_large(istack<N> *s)
{
    id_type sz = 3u * N;
    s->reserve(sz);
    EXPECT_NE(s->m_stack, s->m_buf);
}

template<id_type N>
void fill_to_large(istack<N> *s)
{
    id_type sz = 3u * N;
    s->reserve(sz);
    for(int i = 0, e = (int)sz; i < e; ++i)
        s->push(i);
    EXPECT_NE(s->m_stack, s->m_buf);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<id_type N>
void test_stack_small_vs_large()
{
    istack<N> s;
    for(id_type i = 0; i < N; ++i)
    {
        s.push(static_cast<int>(i));
        EXPECT_EQ(s.size(), i+1);
    }
    EXPECT_EQ(s.size(), N);
    EXPECT_EQ(s.m_stack, s.m_buf);
    for(id_type i = 0; i < N; ++i)
    {
        EXPECT_EQ(s.top(N-1-i), static_cast<int>(i));
    }
    s.push(N);
    EXPECT_NE(s.m_stack, s.m_buf);
    EXPECT_EQ(s.top(), static_cast<int>(N));
    EXPECT_EQ(s.pop(), static_cast<int>(N));
    EXPECT_NE(s.m_stack, s.m_buf);
    for(id_type i = 0; i < N; ++i)
    {
        EXPECT_EQ(s.top(N-1-i), static_cast<int>(i));
    }
}

TEST(stack, small_vs_large)
{
    test_stack_small_vs_large<8>();
    test_stack_small_vs_large<16>();
    test_stack_small_vs_large<32>();
    test_stack_small_vs_large<128>();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<id_type N>
void test_copy_ctor()
{
    istack<N> src;

    // small
    for(id_type i = 0; i < N; ++i)
    {
        src.push((int)i);
    }
    EXPECT_EQ(src.m_stack, src.m_buf);
    ip b = src.begin();
    {
        istack<N> dst(src);
        EXPECT_EQ(dst.size(), src.size());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ((ip)src.begin(), b);
        EXPECT_NE((ip)dst.begin(), (ip)src.begin());
    }

    // large
    for(id_type i = 0; i < 2*N; ++i)
    {
        src.push((int)i); // large
    }
    EXPECT_NE(src.m_stack, src.m_buf);
    b = src.begin();
    {
        istack<N> dst(src);
        EXPECT_EQ(dst.size(), src.size());
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ((ip)src.begin(), b);
        EXPECT_NE((ip)dst.begin(), (ip)src.begin());
    }
}

TEST(stack, copy_ctor)
{
    test_copy_ctor<4>();
    test_copy_ctor<8>();
    test_copy_ctor<64>();
    test_copy_ctor<128>();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<id_type N>
void test_move_ctor()
{
    istack<N> src;

    // small
    for(id_type i = 0; i < N; ++i)
    {
        src.push((int)i);
    }
    EXPECT_EQ(src.m_stack, src.m_buf);
    ip b = src.begin();
    id_type sz = src.size();
    {
        istack<N> dst(std::move(src));
        EXPECT_EQ(dst.size(), sz);
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_NE(dst.m_stack, b);
        EXPECT_EQ(src.size(), id_type(0));
        EXPECT_EQ((ip)src.begin(), src.m_buf);
        EXPECT_NE((ip)dst.begin(), b);
    }
    EXPECT_EQ(src.size(), id_type(0));
    EXPECT_EQ(src.capacity(), N);
    EXPECT_EQ(src.m_stack, src.m_buf);

    // redo
    for(id_type i = 0; i < N; ++i)
    {
        src.push((int)i);
    }
    EXPECT_EQ(src.size(), N);
    EXPECT_EQ(src.capacity(), N);
    EXPECT_EQ(src.m_stack, src.m_buf);
    // large
    for(id_type i = 0; i < 2*N; ++i)
    {
        src.push((int)i); // large
    }
    EXPECT_EQ(src.size(), 3*N);
    EXPECT_NE(src.m_stack, src.m_buf);
    b = src.begin();
    sz = src.size();
    {
        istack<N> dst(std::move(src));
        EXPECT_EQ(dst.size(), sz);
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(dst.m_stack, b);
        EXPECT_EQ(src.capacity(), N);
        EXPECT_EQ(src.size(), id_type(0));
        EXPECT_EQ((ip)src.begin(), src.m_buf);
        EXPECT_EQ((ip)dst.begin(), b);
    }
}

TEST(stack, move_ctor)
{
    test_move_ctor<4>();
    test_move_ctor<8>();
    test_move_ctor<64>();
    test_move_ctor<128>();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<id_type N>
void test_copy_assign()
{
    istack<N> dst;
    istack<N> srcs; // small
    istack<N> srcl; // large

    for(id_type i = 0; i < N; ++i)
    {
        srcs.push((int)i); // small
        srcl.push((int)i); // large
    }
    for(id_type i = 0; i < 2*N; ++i)
    {
        srcl.push((int)i); // large
    }
    EXPECT_EQ(srcs.m_stack, srcs.m_buf);
    EXPECT_NE(srcl.m_stack, srcl.m_buf);

    ip bs = srcs.begin(), bl = srcl.begin();

    {
        dst = srcs;
        EXPECT_EQ(dst.size(), srcs.size());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ((ip)srcs.begin(), bs);
        EXPECT_NE((ip)dst.begin(), (ip)srcs.begin());
    }

    {
        dst = srcl;
        EXPECT_EQ(dst.size(), srcl.size());
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ((ip)srcl.begin(), bl);
        EXPECT_NE((ip)dst.begin(), (ip)srcl.begin());
    }

    {
        dst = srcs;
        EXPECT_EQ(dst.size(), srcs.size());
        EXPECT_NE(dst.m_stack, dst.m_buf); // it stays in long mode (it's not trimmed when assigned from a short-mode stack)
        EXPECT_EQ((ip)srcs.begin(), bs);
        EXPECT_NE((ip)dst.begin(), (ip)srcs.begin());
    }
}

TEST(stack, copy_assign)
{
    test_copy_assign<4>();
    test_copy_assign<8>();
    test_copy_assign<64>();
    test_copy_assign<128>();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<id_type N>
void test_move_assign()
{
    istack<N> srcs, srcl, dst;

    for(id_type i = 0; i < N; ++i)
    {
        srcs.push((int)i); // small
        srcl.push((int)i); // large
    }
    for(id_type i = 0; i < 2*N; ++i)
    {
        srcl.push((int)i); // large
    }
    EXPECT_EQ(srcs.m_stack, srcs.m_buf);
    EXPECT_NE(srcl.m_stack, srcl.m_buf);

    ip bs = srcs.begin()/*, bl = srcl.begin()*/;
    id_type szs = srcs.size(), szl = srcl.size();

    for(int i = 0; i < 10; ++i)
    {
        EXPECT_FALSE(srcs.empty());
        EXPECT_TRUE(dst.empty());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ(srcs.m_stack, srcs.m_buf);

        dst = std::move(srcs);
        EXPECT_TRUE(srcs.empty());
        EXPECT_FALSE(dst.empty());
        EXPECT_EQ(srcs.size(), id_type(0));
        EXPECT_EQ(srcs.capacity(), N);
        EXPECT_EQ(dst.size(), szs);
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ(srcs.m_stack, srcs.m_buf);
        EXPECT_EQ((ip)srcs.begin(), bs);
        EXPECT_NE((ip)dst.begin(), (ip)srcs.begin());

        srcs = std::move(dst);
    }

    for(int i = 0; i < 10; ++i)
    {
        EXPECT_EQ(srcl.size(), 3*N);
        EXPECT_FALSE(srcl.empty());
        EXPECT_TRUE(dst.empty());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_NE(srcl.m_stack, srcl.m_buf);

        dst = std::move(srcl);
        EXPECT_TRUE(srcl.empty());
        EXPECT_FALSE(dst.empty());
        EXPECT_EQ(srcl.size(), id_type(0));
        EXPECT_EQ(srcl.capacity(), N);
        EXPECT_EQ(dst.size(), szl);
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(srcl.m_stack, srcl.m_buf);
        EXPECT_EQ((ip)srcl.begin(), srcl.m_buf);
        EXPECT_NE((ip)dst.begin(), (ip)srcl.begin());

        srcl = std::move(dst);
    }
}

TEST(stack, move_assign)
{
    test_move_assign<4>();
    test_move_assign<8>();
    test_move_assign<64>();
    test_move_assign<128>();
}


//-----------------------------------------------------------------------------

template<id_type N>
void test_callbacks_default_ctor()
{
    CallbacksTester td;
    CallbacksTester ts;
    istack<N> dst;
    EXPECT_EQ(dst.m_callbacks, get_callbacks());
}

TEST(stack, callbacks_default_ctor)
{
    test_callbacks_default_ctor<4>();
    test_callbacks_default_ctor<8>();
    test_callbacks_default_ctor<64>();
    test_callbacks_default_ctor<128>();
}

template<id_type N>
void test_callbacks_ctor()
{
    CallbacksTester td;
    CallbacksTester ts;
    istack<N> dst(td.callbacks());
    ASSERT_EQ(dst.m_callbacks, td.callbacks());
}

TEST(stack, callbacks_ctor)
{
    test_callbacks_ctor<4>();
    test_callbacks_ctor<8>();
    test_callbacks_ctor<64>();
    test_callbacks_ctor<128>();
}


//-----------------------------------------------------------------------------
// copy ctor

template<id_type N>
void test_callbacks_copy_ctor_small()
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        istack<N> src(ts.callbacks());
        EXPECT_EQ(src.size(), 0u);
        EXPECT_EQ(src.capacity(), N);
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        size_t nbefore = ts.num_allocs;
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        istack<N> dst(src);
        EXPECT_EQ(dst.size(), 0u);
        EXPECT_EQ(dst.capacity(), N);
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, ts.callbacks());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, nbefore);
        EXPECT_EQ(td.num_allocs, 0u);
    }
}

template<id_type N>
void test_callbacks_copy_ctor_large_unfilled()
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        istack<N> src(ts.callbacks());
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        to_large(&src);
        ASSERT_GT(src.capacity(), N);
        size_t nbefore = ts.num_allocs;
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_NE(ts.num_allocs, 0u);
        istack<N> dst(src);
        ASSERT_EQ(dst.m_callbacks, ts.callbacks());
        ASSERT_NE(dst.m_callbacks, td.callbacks());
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, nbefore);
        EXPECT_EQ(td.num_allocs, 0u);
    }
}

template<id_type N>
void test_callbacks_copy_ctor_large_filled()
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        istack<N> src(ts.callbacks());
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        fill_to_large(&src);
        ASSERT_GT(src.capacity(), N);
        size_t nbefore = ts.num_allocs;
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_NE(ts.num_allocs, 0u);
        istack<N> dst(src);
        ASSERT_EQ(dst.m_callbacks, ts.callbacks());
        ASSERT_NE(dst.m_callbacks, td.callbacks());
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_GT(ts.num_allocs, nbefore);
        EXPECT_EQ(td.num_allocs, 0u);
    }
}

TEST(stack, callbacks_copy_ctor_small)
{
    test_callbacks_copy_ctor_small<4>();
    test_callbacks_copy_ctor_small<8>();
    test_callbacks_copy_ctor_small<64>();
    test_callbacks_copy_ctor_small<128>();
}

TEST(stack, callbacks_copy_ctor_large_unfilled)
{
    test_callbacks_copy_ctor_large_unfilled<4>();
    test_callbacks_copy_ctor_large_unfilled<8>();
    test_callbacks_copy_ctor_large_unfilled<64>();
    test_callbacks_copy_ctor_large_unfilled<128>();
}

TEST(stack, callbacks_copy_ctor_large_filled)
{
    test_callbacks_copy_ctor_large_filled<4>();
    test_callbacks_copy_ctor_large_filled<8>();
    test_callbacks_copy_ctor_large_filled<64>();
    test_callbacks_copy_ctor_large_filled<128>();
}


//-----------------------------------------------------------------------------
// copy ctor

template<id_type N>
void test_callbacks_move_ctor_small()
{
    CallbacksTester ts;
    istack<N> src(ts.callbacks());
    ASSERT_EQ(src.m_callbacks, ts.callbacks());
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, 0u);
    size_t nbefore = ts.num_allocs;
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, 0u);
    istack<N> dst(std::move(src));
    ASSERT_EQ(dst.m_callbacks, ts.callbacks());
    EXPECT_EQ(dst.m_stack, dst.m_buf);
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, nbefore);
}

template<id_type N>
void test_callbacks_move_ctor_large_unfilled()
{
    CallbacksTester ts;
    istack<N> src(ts.callbacks());
    ASSERT_EQ(src.m_callbacks, ts.callbacks());
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, 0u);
    to_large(&src);
    size_t nbefore = ts.num_allocs;
    EXPECT_NE(src.m_stack, src.m_buf);
    EXPECT_NE(ts.num_allocs, 0u);
    istack<N> dst(std::move(src));
    ASSERT_EQ(dst.m_callbacks, ts.callbacks());
    EXPECT_NE(dst.m_stack, dst.m_buf);
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, nbefore);
}

template<id_type N>
void test_callbacks_move_ctor_large_filled()
{
    CallbacksTester ts;
    istack<N> src(ts.callbacks());
    ASSERT_EQ(src.m_callbacks, ts.callbacks());
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, 0u);
    fill_to_large(&src);
    size_t nbefore = ts.num_allocs;
    EXPECT_NE(src.m_stack, src.m_buf);
    EXPECT_NE(ts.num_allocs, 0u);
    istack<N> dst(std::move(src));
    ASSERT_EQ(dst.m_callbacks, ts.callbacks());
    EXPECT_NE(dst.m_stack, dst.m_buf);
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, nbefore);
}

TEST(stack, callbacks_move_ctor_small)
{
    test_callbacks_move_ctor_small<4>();
    test_callbacks_move_ctor_small<8>();
    test_callbacks_move_ctor_small<64>();
    test_callbacks_move_ctor_small<128>();
}

TEST(stack, callbacks_move_ctor_large_unfilled)
{
    test_callbacks_move_ctor_large_unfilled<4>();
    test_callbacks_move_ctor_large_unfilled<8>();
    test_callbacks_move_ctor_large_unfilled<64>();
    test_callbacks_move_ctor_large_unfilled<128>();
}

TEST(stack, callbacks_move_ctor_large_filled)
{
    test_callbacks_move_ctor_large_filled<4>();
    test_callbacks_move_ctor_large_filled<8>();
    test_callbacks_move_ctor_large_filled<64>();
    test_callbacks_move_ctor_large_filled<128>();
}


//-----------------------------------------------------------------------------
// copy assign

template<id_type N>
void test_callbacks_copy_assign_to_empty()
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    istack<N> src(ts.callbacks());
    istack<N> dst(td.callbacks());
    ASSERT_EQ(src.m_callbacks, ts.callbacks());
    EXPECT_EQ(src.m_stack, src.m_buf);
    EXPECT_EQ(ts.num_allocs, 0u);
    fill_to_large(&src);
    size_t nbefore = ts.num_allocs;
    EXPECT_NE(src.m_stack, src.m_buf);
    EXPECT_NE(ts.num_allocs, 0u);
    dst = src;
    ASSERT_EQ(dst.m_callbacks, ts.callbacks());
    ASSERT_NE(dst.m_callbacks, td.callbacks());
    EXPECT_NE(dst.m_stack, dst.m_buf);
    EXPECT_NE(src.m_stack, src.m_buf);
    EXPECT_GT(ts.num_allocs, nbefore);
    EXPECT_EQ(td.num_allocs, 0u);
}

TEST(stack, callbacks_copy_assign_to_empty)
{
    test_callbacks_copy_assign_to_empty<4>();
    test_callbacks_copy_assign_to_empty<8>();
    test_callbacks_copy_assign_to_empty<64>();
    test_callbacks_copy_assign_to_empty<128>();
}

template<id_type N>
void test_callbacks_copy_assign_to_nonempty()
{
    CallbacksTester ts("src");
    {
        CallbacksTester td("dst");
        istack<N> src(ts.callbacks());
        istack<N> dst(td.callbacks());
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, td.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(td.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        fill_to_large(&src);
        fill_to_large(&dst);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(td.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        dst = src;
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, ts.callbacks()); // changed to ts
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 2u);
        EXPECT_EQ(td.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 1u);
        td.check();
    }
    ts.check();
}

TEST(stack, callbacks_copy_assign_to_nonempty)
{
    test_callbacks_copy_assign_to_nonempty<4>();
    test_callbacks_copy_assign_to_nonempty<8>();
    test_callbacks_copy_assign_to_nonempty<64>();
    test_callbacks_copy_assign_to_nonempty<128>();
}

template<size_t N>
void test_callbacks_move_assign_to_empty()
{
    CallbacksTester ts("src");
    {
        CallbacksTester td("dst");
        istack<N> src(ts.callbacks());
        istack<N> dst(td.callbacks());
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, td.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(td.num_allocs, 0u);
        fill_to_large(&src);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_EQ(dst.m_stack, dst.m_buf);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(td.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        dst = std::move(src);
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, ts.callbacks()); // changed to ts
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(td.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        td.check();
    }
    EXPECT_EQ(ts.num_allocs, 1u);
    EXPECT_EQ(ts.num_deallocs, 1u);
    ts.check();
}

TEST(stack, callbacks_move_assign_to_empty)
{
    test_callbacks_move_assign_to_empty<4>();
    test_callbacks_move_assign_to_empty<8>();
    test_callbacks_move_assign_to_empty<64>();
    test_callbacks_move_assign_to_empty<128>();
}

template<id_type N>
void test_callbacks_move_assign_to_nonempty()
{
    CallbacksTester ts("src");
    {
        CallbacksTester td("dst");
        istack<N> src(ts.callbacks());
        istack<N> dst(td.callbacks());
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, td.callbacks());
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(td.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        fill_to_large(&src);
        fill_to_large(&dst);
        EXPECT_NE(src.m_stack, src.m_buf);
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(td.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 0u);
        dst = std::move(src);
        ASSERT_EQ(src.m_callbacks, ts.callbacks());
        ASSERT_EQ(dst.m_callbacks, ts.callbacks()); // changed to ts
        EXPECT_NE(dst.m_stack, dst.m_buf);
        EXPECT_EQ(src.m_stack, src.m_buf);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(td.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(td.num_deallocs, 1u);
        td.check();
    }
    EXPECT_EQ(ts.num_allocs, 1u);
    EXPECT_EQ(ts.num_deallocs, 1u);
    ts.check();
}

TEST(stack, callbacks_move_assign_to_nonempty)
{
    test_callbacks_move_assign_to_nonempty<4>();
    test_callbacks_move_assign_to_nonempty<8>();
    test_callbacks_move_assign_to_nonempty<64>();
    test_callbacks_move_assign_to_nonempty<128>();
}


//-----------------------------------------------------------------------------

template<id_type N>
void test_reserve()
{
    {
        CallbacksTester ts;
        {
            istack<N> s(ts.callbacks());
            EXPECT_EQ(ts.num_allocs, 0u);
            EXPECT_EQ(ts.num_deallocs, 0u);
            EXPECT_EQ(s.capacity(), N);
            s.reserve(4*N);
            EXPECT_EQ(ts.num_allocs, 1u);
            EXPECT_EQ(ts.num_deallocs, 0u);
            EXPECT_EQ(s.capacity(), 4*N);
        }
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 1u);
        ts.check();
    }
    {
        CallbacksTester ts;
        {
            istack<N> s(ts.callbacks());
            EXPECT_EQ(ts.num_allocs, 0u);
            EXPECT_EQ(ts.num_deallocs, 0u);
            EXPECT_EQ(s.capacity(), N);
            s.reserve(4*N);
            EXPECT_EQ(ts.num_allocs, 1u);
            EXPECT_EQ(ts.num_deallocs, 0u);
            EXPECT_EQ(s.capacity(), 4*N);
            s._free();
        }
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 1u);
        ts.check();
    }
}

TEST(stack, reserve_capacity)
{
    test_reserve<10>();
    test_reserve<20>();
}


template<id_type N, int NumTimes>
void grow_to_large__push()
{
    istack<N> s;
    int ni = (int)N;
    for(int i = 0; i < NumTimes * ni; ++i)
    {
        s.push(i);
        if(i < ni)
            EXPECT_EQ(s.m_stack, s.m_buf) << i;
        else
            EXPECT_NE(s.m_stack, s.m_buf) << i;
    }
    for(int i = 0; i < NumTimes * ni; ++i)
    {
        EXPECT_EQ(s.bottom((id_type)i), i);
    }
}

TEST(stack, push_to_large_twice)
{
    grow_to_large__push<10, 8>();
    grow_to_large__push<20, 8>();
    grow_to_large__push<32, 8>();
}

template<id_type N, int NumTimes>
void grow_to_large__push_top()
{
    istack<N> s;
    int ni = (int)N;
    s.push(0);
    for(int i = 1; i < NumTimes * ni; ++i)
    {
        s.push_top();
        EXPECT_EQ(s.top(), i-1) << i;
        s.top() = i;
        if(i < ni)
            EXPECT_EQ(s.m_stack, s.m_buf) << i;
        else
            EXPECT_NE(s.m_stack, s.m_buf) << i;
    }
    for(int i = 0; i < NumTimes * ni; ++i)
    {
        EXPECT_EQ(s.bottom((id_type)i), i);
    }
}

TEST(stack, push_top_to_large_twice)
{
    grow_to_large__push_top<10, 8>();
    grow_to_large__push_top<20, 8>();
    grow_to_large__push_top<32, 8>();
}

} // namespace detail
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
