#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include "./test_lib/callbacks_tester.hpp"

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389) // signed/unsigned mismatch
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void node_scalar_test_empty(NodeScalar const& s)
{
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.tag, "");
    EXPECT_EQ(s.tag.len, 0u);
    EXPECT_TRUE(s.tag.empty());
    EXPECT_EQ(s.scalar, "");
    EXPECT_EQ(s.scalar.len, 0u);
    EXPECT_TRUE(s.scalar.empty());
}

void node_scalar_test_foo(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str");
        EXPECT_EQ(s.tag.len, 5u);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0u);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo");
    EXPECT_EQ(s.scalar.len, 3u);
    EXPECT_FALSE(s.scalar.empty());
}

void node_scalar_test_foo3(NodeScalar const& s, bool with_tag=false)
{
    EXPECT_FALSE(s.empty());
    if(with_tag)
    {
        EXPECT_EQ(s.tag, "!!str+++");
        EXPECT_EQ(s.tag.len, 8u);
        EXPECT_FALSE(s.tag.empty());
    }
    else
    {
        EXPECT_EQ(s.tag, "");
        EXPECT_EQ(s.tag.len, 0u);
        EXPECT_TRUE(s.tag.empty());
    }
    EXPECT_EQ(s.scalar, "foo3");
    EXPECT_EQ(s.scalar.len, 4u);
    EXPECT_FALSE(s.scalar.empty());
}

TEST(NodeScalar, ctor_empty)
{
    NodeScalar s;
    node_scalar_test_empty(s);
}

TEST(NodeScalar, ctor__untagged)
{
    {
        const char sarr[] = "foo";
        const char *sptr = "foo";
        csubstr ssp = "foo";

        for(auto s : {NodeScalar(sarr), NodeScalar(to_csubstr(sptr)), NodeScalar(ssp)})
        {
            node_scalar_test_foo(s);
        }

        NodeScalar s;
        s = {sarr};
        node_scalar_test_foo(s);
        s = to_csubstr(sptr);
        node_scalar_test_foo(s);
        s = {ssp};
        node_scalar_test_foo(s);
    }

    {
        const char sarr[] = "foo3";
        const char *sptr = "foo3";
        csubstr ssp = "foo3";

        for(auto s : {NodeScalar(sarr), NodeScalar(to_csubstr(sptr)), NodeScalar(ssp)})
        {
            node_scalar_test_foo3(s);
        }

        NodeScalar s;
        {
            SCOPED_TRACE("here 1");
            s = {sarr};
            node_scalar_test_foo3(s);
        }
        {
            SCOPED_TRACE("here 2");
            s = to_csubstr(sptr);
            node_scalar_test_foo3(s);
        }
        {
            SCOPED_TRACE("here 3");
            s = ssp;
            node_scalar_test_foo3(s);
        }
    }
}

TEST(NodeScalar, ctor__tagged)
{
    {
        const char sarr[] = "foo", tarr[] = "!!str";
        const char *sptr = "foo";
        const char *tptr = "!!str";
        csubstr ssp = "foo", tsp = "!!str";

        for(NodeScalar s : {
                NodeScalar(tsp, ssp),
                    NodeScalar(tsp, to_csubstr(sptr)),
                    NodeScalar(tsp, sarr),
                NodeScalar(to_csubstr(tptr), ssp),
                    NodeScalar(to_csubstr(tptr), to_csubstr(sptr)),
                    NodeScalar(to_csubstr(tptr), sarr),
                NodeScalar(tarr, ssp),
                    NodeScalar(tarr, to_csubstr(sptr)),
                    NodeScalar(tarr, sarr),
        })
        {
            node_scalar_test_foo(s, true);
        }

        NodeScalar s;

        {
            SCOPED_TRACE("here 0.0");
            s = {tsp, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 0.1");
            s = {tsp, to_csubstr(sptr)};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 0.2");
            s = {tsp, sarr};
            node_scalar_test_foo(s, true);
        }

        {
            SCOPED_TRACE("here 1.0");
            s = {to_csubstr(tptr), ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 1.1");
            s = {to_csubstr(tptr), to_csubstr(sptr)};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 1.3");
            s = {to_csubstr(tptr), sarr};
            node_scalar_test_foo(s, true);
        }

        {
            SCOPED_TRACE("here 3.0");
            s = {tarr, ssp};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 3.1");
            s = {tarr, to_csubstr(sptr)};
            node_scalar_test_foo(s, true);
        }
        {
            SCOPED_TRACE("here 3.3");
            s = {tarr, sarr};
            node_scalar_test_foo(s, true);
        }

    }

    {
        const char sarr[] = "foo3", tarr[] = "!!str+++";
        const char *sptr = "foo3";
        const char *tptr = "!!str+++";
        csubstr ssp = "foo3", tsp = "!!str+++";

        NodeScalar wtf = {tsp, ssp};
        EXPECT_EQ(wtf.tag, tsp);
        EXPECT_EQ(wtf.scalar, ssp);
        for(auto s : {
                NodeScalar(tsp, ssp),
                    NodeScalar(tsp, to_csubstr(sptr)),
                    NodeScalar(tsp, sarr),
                NodeScalar(to_csubstr(tptr), ssp),
                    NodeScalar(to_csubstr(tptr), to_csubstr(sptr)),
                    NodeScalar(to_csubstr(tptr), sarr),
                NodeScalar(tarr, ssp),
                    NodeScalar(tarr, to_csubstr(sptr)),
                    NodeScalar(tarr, sarr),
        })
        {
            node_scalar_test_foo3(s, true);
        }

        NodeScalar s;

        {
            SCOPED_TRACE("here 0.0");
            s = {tsp, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 0.1");
            s = {tsp, to_csubstr(sptr)};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 0.3");
            s = {tsp, sarr};
            node_scalar_test_foo3(s, true);
        }

        {
            SCOPED_TRACE("here 1.0");
            s = {to_csubstr(tptr), ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 1.1");
            s = {to_csubstr(tptr), to_csubstr(sptr)};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 1.3");
            s = {to_csubstr(tptr), sarr};
            node_scalar_test_foo3(s, true);
        }

        {
            SCOPED_TRACE("here 3.0");
            s = {tarr, ssp};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 3.1");
            s = {tarr, to_csubstr(sptr)};
            node_scalar_test_foo3(s, true);
        }
        {
            SCOPED_TRACE("here 3.3");
            s = {tarr, sarr};
            node_scalar_test_foo3(s, true);
        }

    }

}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(NodeInit, ctor__empty)
{
    NodeInit n;
    EXPECT_EQ((type_bits)n.type, (type_bits)NOTYPE);
    EXPECT_EQ(n.key.scalar, "");
    EXPECT_EQ(n.key.tag, "");
    EXPECT_EQ(n.val.scalar, "");
    EXPECT_EQ(n.val.tag, "");
}

TEST(NodeInit, ctor__type_only)
{
    for(auto k : {KEY, KEYVAL, MAP, KEYMAP, SEQ, KEYSEQ})
    {
        SCOPED_TRACE(NodeType::type_str(k));
        NodeInit n(k);
        EXPECT_EQ((type_bits)n.type, (type_bits)k);
        EXPECT_EQ(n.key.scalar, "");
        EXPECT_EQ(n.key.tag, "");
        EXPECT_EQ(n.val.scalar, "");
        EXPECT_EQ(n.val.tag, "");
    }
}

TEST(NodeInit, ctor__val_only)
{
    {
        const char sarr[] = "foo";
        const char *sptr = "foo"; size_t sptrlen = 3;
        csubstr ssp = "foo";

        {
            SCOPED_TRACE("here 0");
            {
                NodeInit s(sarr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s{to_csubstr(sptr)};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s{sarr};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 1");
            {
                NodeInit s(sarr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s(to_csubstr(sptr));
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
            {
                NodeInit s(sarr);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 2");
            NodeInit s;
            s = {sarr};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
            s = {to_csubstr(sptr)};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
            //s = {sptr, sptrlen}; // fails to compile
            //node_scalar_test_foo(s.val);
            //node_scalar_test_empty(s.key);
            //s.clear();
            s = {ssp};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            s.clear();
        }

        for(auto s : {
            NodeInit(sarr),
            NodeInit(to_csubstr(sptr)),
            NodeInit(csubstr{sptr, sptrlen}),
            NodeInit(ssp)})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
        }
    }

    {
        const char sarr[] = "foo3";
        const char *sptr = "foo3"; size_t sptrlen = 4;
        csubstr ssp = "foo3";

        {
            SCOPED_TRACE("here 0");
            NodeInit s = {sarr};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
        {   // FAILS
            SCOPED_TRACE("here 1");
            //NodeInit s = sarr;
            //node_scalar_test_foo3(s.val);
            //node_scalar_test_empty(s.key);
        }
        {
            SCOPED_TRACE("here 2");
            NodeInit s{sarr};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
        {
            SCOPED_TRACE("here 3");
            NodeInit s(sarr);
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }

        for(auto s : {
            NodeInit(sarr),
            NodeInit(to_csubstr(sptr)),
            NodeInit(csubstr{sptr, sptrlen}),
            NodeInit(ssp)})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
        }
    }
}

TEST(NodeInit, ctor__val_type_only)
{
    {
        const char sarr[] = "foo";
        const char *sptr = "foo"; size_t sptrlen = 3;
        csubstr ssp = "foo";

        {
            SCOPED_TRACE("here 0");
            {
                NodeInit s(sarr, VAL_PLAIN);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
            {
                NodeInit s{to_csubstr(sptr), VAL_PLAIN};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
            {
                NodeInit s{sarr, VAL_PLAIN};
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 1");
            {
                NodeInit s(sarr, VAL_PLAIN);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
            {
                NodeInit s(to_csubstr(sptr), VAL_PLAIN);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
            {
                NodeInit s(sarr, VAL_PLAIN);
                node_scalar_test_foo(s.val);
                node_scalar_test_empty(s.key);
                EXPECT_TRUE(s.type & VAL_PLAIN);
                s.clear();
            }
        }

        {
            SCOPED_TRACE("here 2");
            NodeInit s;
            s = {sarr, VAL_PLAIN};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
            s.clear();
            s = {to_csubstr(sptr), VAL_PLAIN};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
            s.clear();
            s = {ssp, VAL_PLAIN};
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
            s.clear();
        }

        for(auto s : {
            NodeInit(sarr, VAL_PLAIN),
            NodeInit(to_csubstr(sptr), VAL_PLAIN),
            NodeInit(csubstr{sptr, sptrlen}, VAL_PLAIN),
            NodeInit(ssp, VAL_PLAIN)})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
        }
    }

    {
        const char sarr[] = "foo3";
        const char *sptr = "foo3"; size_t sptrlen = 4;
        csubstr ssp = "foo3";

        {
            SCOPED_TRACE("here 0");
            NodeInit s = {sarr, VAL_PLAIN};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
        }
        {   // FAILS
            //SCOPED_TRACE("here 1");
            //NodeInit s = sarr;
            //node_scalar_test_foo3(s.val);
            //node_scalar_test_empty(s.key);
        }
        {
            SCOPED_TRACE("here 2");
            NodeInit s{sarr, VAL_PLAIN};
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
        }
        {
            SCOPED_TRACE("here 3");
            NodeInit s(sarr, VAL_PLAIN);
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
        }

        for(auto s : {
            NodeInit(sarr, VAL_PLAIN),
            NodeInit(to_csubstr(sptr), VAL_PLAIN),
            NodeInit(csubstr{sptr, sptrlen}, VAL_PLAIN),
            NodeInit(ssp, VAL_PLAIN)})
        {
            SCOPED_TRACE("here LOOP");
            node_scalar_test_foo3(s.val);
            node_scalar_test_empty(s.key);
            EXPECT_TRUE(s.type & VAL_PLAIN);
        }
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Tree, empty_ctor)
{
    Tree tree;
    EXPECT_EQ(tree.callbacks(), get_callbacks());
    EXPECT_EQ(tree.empty(), true);
    EXPECT_EQ(tree.capacity(), 0u);
    EXPECT_EQ(tree.arena_capacity(), 0u);
    EXPECT_EQ(tree.arena_slack(), 0u);
    EXPECT_EQ(tree.size(), 0u);
    EXPECT_EQ(tree.slack(), 0u);
    EXPECT_EQ(tree.arena().empty(), true);
}

TEST(Tree, node_cap_ctor)
{
    {
        Tree tree(10u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), false); // we have the root
        EXPECT_EQ(tree.capacity(), 10u);
        EXPECT_EQ(tree.arena_capacity(), 0u);
        EXPECT_EQ(tree.arena_slack(), 0u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 1u); // we have the root
        EXPECT_EQ(tree.slack(), 9u);
    }
    {
        Tree tree(10u, 20u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), false); // we have the root
        EXPECT_EQ(tree.capacity(), 10u);
        EXPECT_EQ(tree.arena_capacity(), 20u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 1u); // we have the root
        EXPECT_EQ(tree.slack(), 9u);
    }
    {
        Tree tree(0u, 20u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), true);
        EXPECT_EQ(tree.capacity(), 0u);
        EXPECT_EQ(tree.arena_capacity(), 20u);
        EXPECT_EQ(tree.arena_slack(), 20u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 0u);
        EXPECT_EQ(tree.slack(), 0u);
    }
}

Tree get_test_tree(CallbacksTester *cbt=nullptr)
{
    Parser::handler_type evt_handler(cbt ? cbt->callbacks() : get_callbacks());
    Parser parser(&evt_handler);
    Tree t = parse_in_arena(&parser, "", "{a: b, c: d, e: [0, 1, 2, 3]}");
    // make sure the tree has strings in its arena
    NodeRef n = t.rootref();
    NodeRef ch = n.append_child();
    ch << key("serialized_key");
    ch << 89;
    return t;
}

TEST(Tree, test_tree_has_arena)
{
    {
        Tree t = get_test_tree();
        ASSERT_GT(t.arena().size(), 0u);
    }
    {
        CallbacksTester cbt;
        Tree t = get_test_tree(&cbt);
        ASSERT_GT(t.arena().size(), 0u);
    }
}

//-------------------------------------------

TEST(Tree, copy_ctor_empty)
{
    CallbacksTester cbt;
    {
        Tree src(cbt.callbacks());
        test_invariants(src);
        {
            Tree dst(src);
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
    {
        Tree src(cbt.callbacks());
        test_invariants(src);
        {
            Tree dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

TEST(Tree, copy_ctor)
{
    CallbacksTester cbt;
    {
        Tree src = get_test_tree(&cbt);
        test_invariants(src);
        {
            Tree dst(src);
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

//-------------------------------------------
TEST(Tree, move_ctor)
{
    CallbacksTester cbt;
    Tree src = get_test_tree(&cbt);
    test_invariants(src);
    Tree save(src);
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst(std::move(src));
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

//-------------------------------------------
TEST(Tree, copy_assign_same_callbacks)
{
    CallbacksTester cbt;
    {
        Tree src = get_test_tree(&cbt);
        test_invariants(src);
        {
            Tree dst(cbt.callbacks());
            EXPECT_EQ(dst.callbacks(), src.callbacks());
            test_invariants(dst);
            dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

TEST(Tree, copy_assign_diff_callbacks)
{
    CallbacksTester cbsrc("src");
    CallbacksTester cbdst("dst");
    {
        Tree src = get_test_tree(&cbsrc);
        EXPECT_EQ(src.callbacks(), cbsrc.callbacks());
        test_invariants(src);
        {
            Tree dst = get_test_tree(&cbdst);
            EXPECT_EQ(dst.callbacks(), cbdst.callbacks());
            test_invariants(dst);
            dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

//-------------------------------------------
TEST(Tree, move_assign_same_callbacks)
{
    CallbacksTester cbt;
    Tree src = get_test_tree(&cbt);
    test_invariants(src);
    Tree save(src);
    EXPECT_EQ(save.callbacks(), src.callbacks());
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst = get_test_tree(&cbt);
        EXPECT_NE(dst.empty(), true);
        EXPECT_NE(dst.size(), 0u);
        EXPECT_NE(dst.arena().empty(), true);
        dst = std::move(src);
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(src.m_callbacks, cbt.callbacks());
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        EXPECT_EQ(dst.callbacks(), save.callbacks());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

TEST(Tree, move_assign_diff_callbacks)
{
    CallbacksTester cbsrc("src");
    CallbacksTester cbdst("dst");
    Tree src = get_test_tree(&cbsrc);
    test_invariants(src);
    Tree save(src);
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst = get_test_tree(&cbdst);
        EXPECT_NE(dst.empty(), true);
        EXPECT_NE(dst.size(), 0u);
        EXPECT_NE(dst.arena().empty(), true);
        EXPECT_EQ(dst.callbacks(), cbdst.callbacks());
        dst = std::move(src);
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(src.m_callbacks, cbsrc.callbacks());
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        EXPECT_NE(dst.callbacks(), cbdst.callbacks());
        EXPECT_EQ(dst.callbacks(), save.callbacks());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

TEST(Tree, std_interop)
{
    CallbacksTester cbt;
    std::vector<Tree> forest;
    for(size_t i = 0; i < 3; ++i)
    {
        forest.emplace_back(cbt.callbacks());
        parse_in_arena("{foo: bar}", &forest.back());
    }
}


//-------------------------------------------
TEST(Tree, reserve)
{
    Tree t(16, 64);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    auto buf = t.m_buf;
    t.reserve(16);
    t.reserve_arena(64);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.reserve(32);
    t.reserve_arena(128);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    buf = t.m_buf;
    parse_in_arena("[a, b, c, d, e, f]", &t);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 25);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 110);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);

    t.reserve(64);
    t.reserve_arena(256);
    EXPECT_EQ(t.capacity(), 64);
    EXPECT_EQ(t.slack(), 57);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 256);
    EXPECT_EQ(t.arena_slack(), 238);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);
}

// https://github.com/biojppm/rapidyaml/issues/288
TEST(Tree, reserve_arena_issue288)
{
    Tree t;
    EXPECT_EQ(t.arena_slack(), 0u);
    EXPECT_EQ(t.arena_capacity(), 0u);
    EXPECT_EQ(t.arena_size(), 0u);
    t.reserve_arena(3u);
    EXPECT_EQ(t.arena_slack(), 3u);
    EXPECT_GE(t.arena_capacity(), 3u);
    EXPECT_EQ(t.arena_size(), 0u);
    // longer than the slack to cause another call to _grow_arena()
    std::string stars(2 * t.arena_slack(), '*');
    t.copy_to_arena(to_csubstr(stars));
    EXPECT_GE(t.arena_capacity(), stars.size());
    EXPECT_EQ(t.arena_size(), stars.size());
    EXPECT_EQ(t.arena(), to_csubstr(stars));
    // again
    std::string pluses(2 * t.arena_slack(), '+');
    t.copy_to_arena(to_csubstr(pluses));
    EXPECT_GE(t.arena_capacity(), stars.size() + pluses.size());
    EXPECT_EQ(t.arena_size(), stars.size() + pluses.size());
    EXPECT_EQ(t.arena().first(stars.size()), to_csubstr(stars));
    EXPECT_EQ(t.arena().last(pluses.size()), to_csubstr(pluses));
}

TEST(Tree, clear)
{
    Tree t(16, 64);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.clear();
    t.clear_arena();
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    auto buf = t.m_buf;
    t.reserve(16);
    t.reserve_arena(64);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.reserve(32);
    t.reserve_arena(128);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    buf = t.m_buf;
    parse_in_arena("[a, b, c, d, e, f]", &t);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 25);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 110);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);

    t.clear();
    t.clear_arena();
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);
}


//-------------------------------------------

template<class Function>
void verify_success_(Tree &tree, Function &&fn)
{
    ExpectError::check_success(&tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
void verify_success_(csubstr src, Function &&fn)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_success(&tree, [&]{
        (void)fn(tree);
    });
}

template<class Function>
void verify_assertion_(Tree &tree, Function &&fn)
{
    ExpectError::check_assertion(&tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
void verify_assertion_(csubstr src, Function &&fn)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_assertion(&tree, [&]{
        (void)fn(tree);
    });
}

template<class Function>
void verify_error_(Tree &tree, Function &&fn)
{
    ExpectError::check_error(&tree, [&]{
        (void)fn(tree);
    });
}
template<class Function>
void verify_error_(csubstr src, Function &&fn)
{
    Tree tree = parse_in_arena(src);
    ExpectError::check_error(&tree, [&]{
        (void)fn(tree);
    });
}

#define verify_success(...)                     \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_success_(__VA_ARGS__);           \
    }

#define verify_assertion(...)                   \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_assertion_(__VA_ARGS__);         \
    }

#define verify_error(...)                       \
    {                                           \
        SCOPED_TRACE("caller");                 \
        verify_error_(__VA_ARGS__);             \
    }


TEST(Tree, ref)
{
    Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.ref(0).id(), 0);
    EXPECT_EQ(t.ref(1).id(), 1);
    EXPECT_EQ(t.ref(2).id(), 2);
    EXPECT_EQ(t.ref(3).id(), 3);
    EXPECT_EQ(t.ref(4).id(), 4);
    EXPECT_TRUE(t.ref(0).is_seq());
    EXPECT_TRUE(t.ref(1).is_val());
    EXPECT_TRUE(t.ref(2).is_val());
    EXPECT_TRUE(t.ref(3).is_val());
    EXPECT_TRUE(t.ref(4).is_val());
    verify_assertion(t, [](Tree & tree){ return tree.ref(tree.capacity()); });
    verify_assertion(t, [](Tree & tree){ return tree.ref(NONE); });
}

TEST(Tree, cref)
{
    Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.cref(0).id(), 0);
    EXPECT_EQ(t.cref(1).id(), 1);
    EXPECT_EQ(t.cref(2).id(), 2);
    EXPECT_EQ(t.cref(3).id(), 3);
    EXPECT_EQ(t.cref(4).id(), 4);
    EXPECT_TRUE(t.cref(0).is_seq());
    EXPECT_TRUE(t.cref(1).is_val());
    EXPECT_TRUE(t.cref(2).is_val());
    EXPECT_TRUE(t.cref(3).is_val());
    EXPECT_TRUE(t.cref(4).is_val());
    verify_assertion(t, [](Tree & tree){ return tree.cref(tree.capacity()); });
    verify_assertion(t, [](Tree & tree){ return tree.cref(NONE); });
}

TEST(Tree, ref_const)
{
    const Tree t = parse_in_arena("[0, 1, 2, 3]");
    EXPECT_EQ(t.ref(0).id(), 0);
    EXPECT_EQ(t.ref(1).id(), 1);
    EXPECT_EQ(t.ref(2).id(), 2);
    EXPECT_EQ(t.ref(3).id(), 3);
    EXPECT_EQ(t.ref(4).id(), 4);
    EXPECT_TRUE(t.ref(0).is_seq());
    EXPECT_TRUE(t.ref(1).is_val());
    EXPECT_TRUE(t.ref(2).is_val());
    EXPECT_TRUE(t.ref(3).is_val());
    EXPECT_TRUE(t.ref(4).is_val());
    verify_assertion("[0, 1, 2, 3]", [](Tree const& tree){ return tree.cref(tree.capacity()); });
    verify_assertion("[0, 1, 2, 3]", [](Tree const& tree){ return tree.cref(NONE); });
}


TEST(Tree, operator_square_brackets_seq)
{
    Tree t = parse_in_arena("[0, 1, 2, 3, 4]");
    Tree &m = t;
    Tree const& cm = t;
    EXPECT_EQ(m[0].val(), "0");
    EXPECT_EQ(m[1].val(), "1");
    EXPECT_EQ(m[2].val(), "2");
    EXPECT_EQ(m[3].val(), "3");
    EXPECT_EQ(m[4].val(), "4");
    EXPECT_EQ(cm[0].val(), "0");
    EXPECT_EQ(cm[1].val(), "1");
    EXPECT_EQ(cm[2].val(), "2");
    EXPECT_EQ(cm[3].val(), "3");
    EXPECT_EQ(cm[4].val(), "4");
    //
    verify_assertion(t, [&](Tree const&){ return cm[m.capacity()]; });
    verify_assertion(t, [&](Tree const&){ return cm[NONE]; });
    verify_assertion(t, [&](Tree const&){ return cm[0][0]; });
    verify_assertion(t, [&](Tree const&){ return cm["a"]; });
}

TEST(Tree, operator_square_brackets_map)
{
    Tree t = parse_in_arena("{a: 0, b: 1, c: 2, d: 3, e: 4}");
    Tree &m = t;
    Tree const& cm = t;
    EXPECT_EQ(m["a"].val(), "0");
    EXPECT_EQ(m["b"].val(), "1");
    EXPECT_EQ(m["c"].val(), "2");
    EXPECT_EQ(m["d"].val(), "3");
    EXPECT_EQ(m["e"].val(), "4");
    EXPECT_EQ(cm["a"].val(), "0");
    EXPECT_EQ(cm["b"].val(), "1");
    EXPECT_EQ(cm["c"].val(), "2");
    EXPECT_EQ(cm["d"].val(), "3");
    EXPECT_EQ(cm["e"].val(), "4");
    //
    verify_assertion(t, [&](Tree const&){ return cm["f"]; });
    verify_assertion(t, [&](Tree const&){ return cm["g"]["h"]; });
}

TEST(Tree, noderef_at_seq)
{
    Tree t = parse_in_arena("[0, 1, 2, 3, 4]");
    NodeRef m = t.rootref();
    ConstNodeRef const cm = t.rootref();
    EXPECT_EQ(m.at(0).val(), "0");
    EXPECT_EQ(m.at(1).val(), "1");
    EXPECT_EQ(m.at(2).val(), "2");
    EXPECT_EQ(m.at(3).val(), "3");
    EXPECT_EQ(m.at(4).val(), "4");
    EXPECT_EQ(cm.at(0).val(), "0");
    EXPECT_EQ(cm.at(1).val(), "1");
    EXPECT_EQ(cm.at(2).val(), "2");
    EXPECT_EQ(cm.at(3).val(), "3");
    EXPECT_EQ(cm.at(4).val(), "4");
    //
    EXPECT_EQ(cm.num_children(), 5);
    EXPECT_EQ(cm.num_children(), m.num_children());
    //
    verify_error(t, [&](Tree const&){ return cm.at(NONE); });
    verify_error(t, [&](Tree const&){ return cm.at(t.capacity()); });
    verify_error(t, [&](Tree const&){ return cm.at(5); });
    verify_error(t, [&](Tree const&){ return cm.at(6); });
    verify_error(t, [&](Tree const&){ return cm.at(7); });
    verify_error(t, [&](Tree const&){ return cm.at(10); });
    verify_error(t, [&](Tree const&){ return cm.at(0).at(0); });
    verify_error(t, [&](Tree const&){ return cm.at("a"); });
    //
    verify_error(t, [&](Tree const&){ return m.at(NONE); });
    verify_error(t, [&](Tree const&){ return m.at(t.capacity()); });
    verify_success(t, [&](Tree const&){ return m.at(5); });
    verify_success(t, [&](Tree const&){ return m.at(6); });
    verify_error(t, [&](Tree const&){ return m.at(0).at(0); });
    verify_error(t, [&](Tree const&){ return m.at("a"); });
    EXPECT_TRUE(m.at(5).is_seed());
    EXPECT_TRUE(m.at(6).is_seed());
    //
    NodeRef to_be_removed_orig = m.at(4);
    NodeRef to_be_removed = to_be_removed_orig;
    EXPECT_EQ(to_be_removed.id(), 5);
    EXPECT_EQ(to_be_removed_orig.id(), 5);
    m.remove_child(to_be_removed);
    EXPECT_EQ(to_be_removed.id(), 5); // it is stale now
    EXPECT_EQ(to_be_removed_orig.id(), 5); // it is stale now
    EXPECT_EQ(m.num_children(), 4);
    EXPECT_TRUE(m.at(4).is_seed());
    verify_success(t, [&](Tree const&){ return m.at(4); });
    verify_error(t, [&](Tree const&){ return cm.at(4); });
}

TEST(Tree, noderef_at_map)
{
    Tree t = parse_in_arena("{a: 0, b: 1, c: 2, d: 3, e: 4}");
    NodeRef m = t.rootref();
    ConstNodeRef const cm = t.rootref();
    EXPECT_EQ(m.at("a").val(), "0");
    EXPECT_EQ(m.at("b").val(), "1");
    EXPECT_EQ(m.at("c").val(), "2");
    EXPECT_EQ(m.at("d").val(), "3");
    EXPECT_EQ(m.at("e").val(), "4");
    EXPECT_EQ(cm.at("a").val(), "0");
    EXPECT_EQ(cm.at("b").val(), "1");
    EXPECT_EQ(cm.at("c").val(), "2");
    EXPECT_EQ(cm.at("d").val(), "3");
    EXPECT_EQ(cm.at("e").val(), "4");
    //
    verify_error(t, [&](Tree const&){ return cm.at(t.capacity()); });
    verify_error(t, [&](Tree const&){ return cm.at(NONE); });
    verify_error(t, [&](Tree const&){ return cm.at(cm.num_children()); });
    verify_error(t, [&](Tree const&){ return cm.at(5); });
    verify_error(t, [&](Tree const&){ return cm.at(6); });
    verify_error(t, [&](Tree const&){ return cm.at("f"); });
    verify_error(t, [&](Tree const&){ return cm.at("g").at("h"); });
    //
    verify_error(t, [&](Tree const&){ return m.at(t.capacity()); });
    verify_error(t, [&](Tree const&){ return m.at(NONE); });
    verify_success(t, [&](Tree const&){ return m.at(cm.num_children()); });
    verify_success(t, [&](Tree const&){ return m.at(5); });
    verify_success(t, [&](Tree const&){ return m.at(6); });
    verify_success(t, [&](Tree const&){ return m.at("f"); });
    verify_error(t, [&](Tree const&){ return m.at("g").at("h"); });
    EXPECT_TRUE(m.at(cm.num_children()).is_seed());
    EXPECT_TRUE(m.at(5).is_seed());
    EXPECT_TRUE(m.at(6).is_seed());
    EXPECT_TRUE(m.at("f").is_seed());
}

TEST(Tree, relocate)
{
    // create a tree with anchors and refs, and copy it to ensure the
    // relocation also applies to the anchors and refs. Ensure to put
    // the source in the arena so that it gets relocated.
    Tree tree = parse_in_arena(R"(&keyanchor key: val
key2: &valanchor val2
keyref: *keyanchor
*valanchor : was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
    Tree copy = tree;
    EXPECT_EQ(copy.size(), tree.size());
    EXPECT_EQ(emitrs_yaml<std::string>(copy), R"(&keyanchor key: val
key2: &valanchor val2
keyref: *keyanchor
*valanchor : was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
    //
    Tree copy2 = copy;
    EXPECT_EQ(copy.size(), tree.size());
    copy2.resolve();
    EXPECT_EQ(emitrs_yaml<std::string>(copy2), R"(key: val
key2: val2
keyref: key
val2: was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(NodeType, type_str)
{
    // avoid coverage misses
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL).type_str()), "KEYVAL");
    EXPECT_EQ(to_csubstr(NodeType(KEY).type_str()), "KEY");
    EXPECT_EQ(to_csubstr(NodeType(VAL).type_str()), "VAL");
    EXPECT_EQ(to_csubstr(NodeType(MAP).type_str()), "MAP");
    EXPECT_EQ(to_csubstr(NodeType(SEQ).type_str()), "SEQ");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP).type_str()), "KEYMAP");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ).type_str()), "KEYSEQ");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ).type_str()), "DOCSEQ");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP).type_str()), "DOCMAP");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL).type_str()), "DOCVAL");
    EXPECT_EQ(to_csubstr(NodeType(DOC).type_str()), "DOC");
    EXPECT_EQ(to_csubstr(NodeType(STREAM).type_str()), "STREAM");
    EXPECT_EQ(to_csubstr(NodeType(NOTYPE).type_str()), "NOTYPE");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|VALREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|VALANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYREF|VALANCH).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYVAL|KEYANCH|VALREF).type_str()), "KEYVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|VALREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|VALANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYREF|VALANCH).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYMAP|KEYANCH|VALREF).type_str()), "KEYMAP***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|VALREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|VALANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYREF|VALANCH).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(KEYSEQ|KEYANCH|VALREF).type_str()), "KEYSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ|VALANCH).type_str()), "DOCSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCSEQ|VALREF).type_str()), "DOCSEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP|VALANCH).type_str()), "DOCMAP***");
    EXPECT_EQ(to_csubstr(NodeType(DOCMAP|VALREF).type_str()), "DOCMAP***");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL|VALANCH).type_str()), "DOCVAL***");
    EXPECT_EQ(to_csubstr(NodeType(DOCVAL|VALREF).type_str()), "DOCVAL***");
    EXPECT_EQ(to_csubstr(NodeType(KEY|KEYREF).type_str()), "KEY***");
    EXPECT_EQ(to_csubstr(NodeType(KEY|KEYANCH).type_str()), "KEY***");
    EXPECT_EQ(to_csubstr(NodeType(VAL|VALREF).type_str()), "VAL***");
    EXPECT_EQ(to_csubstr(NodeType(VAL|VALANCH).type_str()), "VAL***");
    EXPECT_EQ(to_csubstr(NodeType(MAP|VALREF).type_str()), "MAP***");
    EXPECT_EQ(to_csubstr(NodeType(MAP|VALANCH).type_str()), "MAP***");
    EXPECT_EQ(to_csubstr(NodeType(SEQ|VALREF).type_str()), "SEQ***");
    EXPECT_EQ(to_csubstr(NodeType(SEQ|VALANCH).type_str()), "SEQ***");
    EXPECT_EQ(to_csubstr(NodeType(DOC|VALREF).type_str()), "DOC***");
    EXPECT_EQ(to_csubstr(NodeType(DOC|VALANCH).type_str()), "DOC***");
    EXPECT_EQ(to_csubstr(NodeType(KEYREF).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(VALREF).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(KEYANCH).type_str()), "(unk)");
    EXPECT_EQ(to_csubstr(NodeType(VALANCH).type_str()), "(unk)");
}



TEST(NodeType, is_stream)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_stream());
    EXPECT_TRUE(NodeType(STREAM).is_stream());
}

TEST(Tree, is_stream)
{
    Tree t = parse_in_arena(R"(---
foo: bar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type keyval_id = t.first_child(doc_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    EXPECT_TRUE(t.is_stream(stream_id));
    EXPECT_FALSE(t.is_stream(doc_id));
    EXPECT_FALSE(t.is_stream(keyval_id));
    EXPECT_TRUE(stream.is_stream());
    EXPECT_FALSE(doc.is_stream());
    EXPECT_FALSE(keyval.is_stream());
    EXPECT_EQ(t.is_stream(stream_id), t._p(stream_id)->m_type.is_stream());
    EXPECT_EQ(t.is_stream(doc_id), t._p(doc_id)->m_type.is_stream());
    EXPECT_EQ(t.is_stream(keyval_id), t._p(keyval_id)->m_type.is_stream());
    EXPECT_EQ(stream.is_stream(), stream.get()->m_type.is_stream());
    EXPECT_EQ(doc.is_stream(), doc.get()->m_type.is_stream());
    EXPECT_EQ(keyval.is_stream(), keyval.get()->m_type.is_stream());
    //
    ASSERT_TRUE(!t.docref(0)["none"].invalid());
    ASSERT_TRUE(t.docref(0)["none"].is_seed());
    ASSERT_FALSE(t.docref(0)["none"].readable());
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_stream(); });
    verify_assertion(t, [&](Tree const&){ return t.is_stream(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_stream(NONE); });
}

TEST(NodeType, is_doc)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_doc());
    EXPECT_TRUE(NodeType(DOC).is_doc());
}

TEST(Tree, is_doc)
{
    Tree t = parse_in_arena(R"(---
foo: bar
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type keyval_id = t.first_child(doc_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_doc(stream_id));
    EXPECT_TRUE(t.is_doc(doc_id));
    EXPECT_FALSE(t.is_doc(keyval_id));
    EXPECT_TRUE(t.is_doc(docval_id));
    EXPECT_FALSE(stream.is_doc());
    EXPECT_TRUE(doc.is_doc());
    EXPECT_FALSE(keyval.is_doc());
    EXPECT_TRUE(docval.is_doc());
    EXPECT_FALSE(mstream.is_doc());
    EXPECT_TRUE(mdoc.is_doc());
    EXPECT_FALSE(mkeyval.is_doc());
    EXPECT_TRUE(mdocval.is_doc());
    EXPECT_EQ(t.is_doc(stream_id), t._p(stream_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(doc_id), t._p(doc_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(keyval_id), t._p(keyval_id)->m_type.is_doc());
    EXPECT_EQ(t.is_doc(docval_id), t._p(docval_id)->m_type.is_doc());
    EXPECT_EQ(stream.is_doc(), stream.get()->m_type.is_doc());
    EXPECT_EQ(doc.is_doc(), doc.get()->m_type.is_doc());
    EXPECT_EQ(keyval.is_doc(), keyval.get()->m_type.is_doc());
    EXPECT_EQ(docval.is_doc(), docval.get()->m_type.is_doc());
    EXPECT_EQ(mstream.is_doc(), mstream.get()->m_type.is_doc());
    EXPECT_EQ(mdoc.is_doc(), mdoc.get()->m_type.is_doc());
    EXPECT_EQ(mkeyval.is_doc(), mkeyval.get()->m_type.is_doc());
    EXPECT_EQ(mdocval.is_doc(), mdocval.get()->m_type.is_doc());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_doc(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_doc(); });
    verify_assertion(t, [&](Tree const&){ return t.is_doc(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_doc(NONE); });
}

TEST(Tree, docref)
{
    Tree tree = parse_in_arena(R"(--- doc0
--- doc1
--- doc2
--- doc3
)");
    EXPECT_EQ(tree.doc(0), 1u);
    EXPECT_EQ(tree.doc(1), 2u);
    EXPECT_EQ(tree.doc(2), 3u);
    EXPECT_EQ(tree.doc(3), 4u);
    EXPECT_EQ(tree.docref(0).val(), "doc0");
    EXPECT_EQ(tree.docref(1).val(), "doc1");
    EXPECT_EQ(tree.docref(2).val(), "doc2");
    EXPECT_EQ(tree.docref(3).val(), "doc3");
    EXPECT_EQ(tree.cdocref(0).val(), "doc0");
    EXPECT_EQ(tree.cdocref(1).val(), "doc1");
    EXPECT_EQ(tree.cdocref(2).val(), "doc2");
    EXPECT_EQ(tree.cdocref(3).val(), "doc3");
}

TEST(NodeType, is_container)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_container());
    EXPECT_FALSE(NodeType(VAL).is_container());
    EXPECT_FALSE(NodeType(KEY).is_container());
    EXPECT_FALSE(NodeType(KEYVAL).is_container());
    EXPECT_TRUE(NodeType(MAP).is_container());
    EXPECT_TRUE(NodeType(SEQ).is_container());
    EXPECT_TRUE(NodeType(KEYMAP).is_container());
    EXPECT_TRUE(NodeType(KEYSEQ).is_container());
    EXPECT_TRUE(NodeType(DOCMAP).is_container());
    EXPECT_TRUE(NodeType(DOCSEQ).is_container());
}

TEST(Tree, is_container)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_TRUE(t.is_container(stream_id));
    EXPECT_TRUE(t.is_container(doc_id));
    EXPECT_TRUE(t.is_container(map_id));
    EXPECT_FALSE(t.is_container(keyval_id));
    EXPECT_TRUE(t.is_container(seq_id));
    EXPECT_FALSE(t.is_container(val_id));
    EXPECT_FALSE(t.is_container(docval_id));
    EXPECT_TRUE(stream.is_container());
    EXPECT_TRUE(doc.is_container());
    EXPECT_TRUE(map.is_container());
    EXPECT_FALSE(keyval.is_container());
    EXPECT_TRUE(seq.is_container());
    EXPECT_FALSE(val.is_container());
    EXPECT_FALSE(docval.is_container());
    EXPECT_TRUE(mstream.is_container());
    EXPECT_TRUE(mdoc.is_container());
    EXPECT_TRUE(mmap.is_container());
    EXPECT_FALSE(mkeyval.is_container());
    EXPECT_TRUE(mseq.is_container());
    EXPECT_FALSE(mval.is_container());
    EXPECT_FALSE(mdocval.is_container());
    EXPECT_EQ(t.is_container(stream_id), t._p(stream_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(doc_id), t._p(doc_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(map_id), t._p(map_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(keyval_id), t._p(keyval_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(seq_id), t._p(seq_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(val_id), t._p(val_id)->m_type.is_container());
    EXPECT_EQ(t.is_container(docval_id), t._p(docval_id)->m_type.is_container());
    EXPECT_EQ(stream.is_container(), stream.get()->m_type.is_container());
    EXPECT_EQ(doc.is_container(), doc.get()->m_type.is_container());
    EXPECT_EQ(map.is_container(), map.get()->m_type.is_container());
    EXPECT_EQ(keyval.is_container(), keyval.get()->m_type.is_container());
    EXPECT_EQ(seq.is_container(), seq.get()->m_type.is_container());
    EXPECT_EQ(val.is_container(), val.get()->m_type.is_container());
    EXPECT_EQ(docval.is_container(), docval.get()->m_type.is_container());
    EXPECT_EQ(mstream.is_container(), mstream.get()->m_type.is_container());
    EXPECT_EQ(mdoc.is_container(), mdoc.get()->m_type.is_container());
    EXPECT_EQ(mmap.is_container(), mmap.get()->m_type.is_container());
    EXPECT_EQ(mkeyval.is_container(), mkeyval.get()->m_type.is_container());
    EXPECT_EQ(mseq.is_container(), mseq.get()->m_type.is_container());
    EXPECT_EQ(mval.is_container(), mval.get()->m_type.is_container());
    EXPECT_EQ(mdocval.is_container(), mdocval.get()->m_type.is_container());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_container(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_container(); });
    verify_assertion(t, [&](Tree const&){ return t.is_container(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_container(NONE); });
}

TEST(NodeType, is_map)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_map());
    EXPECT_FALSE(NodeType(VAL).is_map());
    EXPECT_FALSE(NodeType(KEY).is_map());
    EXPECT_TRUE(NodeType(MAP).is_map());
    EXPECT_TRUE(NodeType(KEYMAP).is_map());
    EXPECT_FALSE(NodeType(SEQ).is_map());
    EXPECT_FALSE(NodeType(KEYSEQ).is_map());
}

TEST(Tree, is_map)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_map(stream_id));
    EXPECT_TRUE(t.is_map(doc_id));
    EXPECT_TRUE(t.is_map(map_id));
    EXPECT_FALSE(t.is_map(keyval_id));
    EXPECT_FALSE(t.is_map(seq_id));
    EXPECT_FALSE(t.is_map(val_id));
    EXPECT_FALSE(t.is_map(docval_id));
    EXPECT_FALSE(stream.is_map());
    EXPECT_TRUE(doc.is_map());
    EXPECT_TRUE(map.is_map());
    EXPECT_FALSE(keyval.is_map());
    EXPECT_FALSE(seq.is_map());
    EXPECT_FALSE(val.is_map());
    EXPECT_FALSE(docval.is_map());
    EXPECT_FALSE(mstream.is_map());
    EXPECT_TRUE(mdoc.is_map());
    EXPECT_TRUE(mmap.is_map());
    EXPECT_FALSE(mkeyval.is_map());
    EXPECT_FALSE(mseq.is_map());
    EXPECT_FALSE(mval.is_map());
    EXPECT_FALSE(mdocval.is_map());
    EXPECT_EQ(t.is_map(stream_id), t._p(stream_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(doc_id), t._p(doc_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(map_id), t._p(map_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(keyval_id), t._p(keyval_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(seq_id), t._p(seq_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(val_id), t._p(val_id)->m_type.is_map());
    EXPECT_EQ(t.is_map(docval_id), t._p(docval_id)->m_type.is_map());
    EXPECT_EQ(stream.is_map(), stream.get()->m_type.is_map());
    EXPECT_EQ(doc.is_map(), doc.get()->m_type.is_map());
    EXPECT_EQ(map.is_map(), map.get()->m_type.is_map());
    EXPECT_EQ(keyval.is_map(), keyval.get()->m_type.is_map());
    EXPECT_EQ(seq.is_map(), seq.get()->m_type.is_map());
    EXPECT_EQ(val.is_map(), val.get()->m_type.is_map());
    EXPECT_EQ(docval.is_map(), docval.get()->m_type.is_map());
    EXPECT_EQ(mstream.is_map(), mstream.get()->m_type.is_map());
    EXPECT_EQ(mdoc.is_map(), mdoc.get()->m_type.is_map());
    EXPECT_EQ(mmap.is_map(), mmap.get()->m_type.is_map());
    EXPECT_EQ(mkeyval.is_map(), mkeyval.get()->m_type.is_map());
    EXPECT_EQ(mseq.is_map(), mseq.get()->m_type.is_map());
    EXPECT_EQ(mval.is_map(), mval.get()->m_type.is_map());
    EXPECT_EQ(mdocval.is_map(), mdocval.get()->m_type.is_map());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_map(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_map(); });
    verify_assertion(t, [&](Tree const&){ return t.is_map(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_map(NONE); });
}

TEST(NodeType, is_seq)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_seq());
    EXPECT_FALSE(NodeType(VAL).is_seq());
    EXPECT_FALSE(NodeType(KEY).is_seq());
    EXPECT_FALSE(NodeType(MAP).is_seq());
    EXPECT_FALSE(NodeType(KEYMAP).is_seq());
    EXPECT_TRUE(NodeType(SEQ).is_seq());
    EXPECT_TRUE(NodeType(KEYSEQ).is_seq());
}

TEST(Tree, is_seq)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_TRUE(t.is_seq(stream_id));
    EXPECT_FALSE(t.is_seq(doc_id));
    EXPECT_FALSE(t.is_seq(map_id));
    EXPECT_FALSE(t.is_seq(keyval_id));
    EXPECT_TRUE(t.is_seq(seq_id));
    EXPECT_FALSE(t.is_seq(val_id));
    EXPECT_FALSE(t.is_seq(docval_id));
    EXPECT_TRUE(stream.is_seq());
    EXPECT_FALSE(doc.is_seq());
    EXPECT_FALSE(map.is_seq());
    EXPECT_FALSE(keyval.is_seq());
    EXPECT_TRUE(seq.is_seq());
    EXPECT_FALSE(val.is_seq());
    EXPECT_FALSE(docval.is_seq());
    EXPECT_TRUE(mstream.is_seq());
    EXPECT_FALSE(mdoc.is_seq());
    EXPECT_FALSE(mmap.is_seq());
    EXPECT_FALSE(mkeyval.is_seq());
    EXPECT_TRUE(mseq.is_seq());
    EXPECT_FALSE(mval.is_seq());
    EXPECT_FALSE(mdocval.is_seq());
    EXPECT_EQ(t.is_seq(stream_id), t._p(stream_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(doc_id), t._p(doc_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(map_id), t._p(map_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(keyval_id), t._p(keyval_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(seq_id), t._p(seq_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(val_id), t._p(val_id)->m_type.is_seq());
    EXPECT_EQ(t.is_seq(docval_id), t._p(docval_id)->m_type.is_seq());
    EXPECT_EQ(stream.is_seq(), stream.get()->m_type.is_seq());
    EXPECT_EQ(doc.is_seq(), doc.get()->m_type.is_seq());
    EXPECT_EQ(map.is_seq(), map.get()->m_type.is_seq());
    EXPECT_EQ(keyval.is_seq(), keyval.get()->m_type.is_seq());
    EXPECT_EQ(seq.is_seq(), seq.get()->m_type.is_seq());
    EXPECT_EQ(val.is_seq(), val.get()->m_type.is_seq());
    EXPECT_EQ(docval.is_seq(), docval.get()->m_type.is_seq());
    EXPECT_EQ(mstream.is_seq(), mstream.get()->m_type.is_seq());
    EXPECT_EQ(mdoc.is_seq(), mdoc.get()->m_type.is_seq());
    EXPECT_EQ(mmap.is_seq(), mmap.get()->m_type.is_seq());
    EXPECT_EQ(mkeyval.is_seq(), mkeyval.get()->m_type.is_seq());
    EXPECT_EQ(mseq.is_seq(), mseq.get()->m_type.is_seq());
    EXPECT_EQ(mval.is_seq(), mval.get()->m_type.is_seq());
    EXPECT_EQ(mdocval.is_seq(), mdocval.get()->m_type.is_seq());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_seq(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_seq(); });
    verify_assertion(t, [&](Tree const&){ return t.is_seq(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_seq(NONE); });
}

TEST(NodeType, has_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_val());
    EXPECT_FALSE(NodeType(KEY).has_val());
    EXPECT_TRUE(NodeType(VAL).has_val());
    EXPECT_TRUE(NodeType(DOCVAL).has_val());
    EXPECT_TRUE(NodeType(KEYVAL).has_val());
    EXPECT_FALSE(NodeType(KEYMAP).has_val());
    EXPECT_FALSE(NodeType(KEYSEQ).has_val());
}

TEST(Tree, has_val)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_val(stream_id));
    EXPECT_FALSE(t.has_val(doc_id));
    EXPECT_FALSE(t.has_val(map_id));
    EXPECT_TRUE(t.has_val(keyval_id));
    EXPECT_FALSE(t.has_val(seq_id));
    EXPECT_TRUE(t.has_val(val_id));
    EXPECT_TRUE(t.has_val(docval_id));
    EXPECT_FALSE(stream.has_val());
    EXPECT_FALSE(doc.has_val());
    EXPECT_FALSE(map.has_val());
    EXPECT_TRUE(keyval.has_val());
    EXPECT_FALSE(seq.has_val());
    EXPECT_TRUE(val.has_val());
    EXPECT_TRUE(docval.has_val());
    EXPECT_FALSE(mstream.has_val());
    EXPECT_FALSE(mdoc.has_val());
    EXPECT_FALSE(mmap.has_val());
    EXPECT_TRUE(mkeyval.has_val());
    EXPECT_FALSE(mseq.has_val());
    EXPECT_TRUE(mval.has_val());
    EXPECT_TRUE(mdocval.has_val());
    EXPECT_EQ(t.has_val(stream_id), t._p(stream_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(doc_id), t._p(doc_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(map_id), t._p(map_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(keyval_id), t._p(keyval_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(seq_id), t._p(seq_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(val_id), t._p(val_id)->m_type.has_val());
    EXPECT_EQ(t.has_val(docval_id), t._p(docval_id)->m_type.has_val());
    EXPECT_EQ(stream.has_val(), stream.get()->m_type.has_val());
    EXPECT_EQ(doc.has_val(), doc.get()->m_type.has_val());
    EXPECT_EQ(map.has_val(), map.get()->m_type.has_val());
    EXPECT_EQ(keyval.has_val(), keyval.get()->m_type.has_val());
    EXPECT_EQ(seq.has_val(), seq.get()->m_type.has_val());
    EXPECT_EQ(val.has_val(), val.get()->m_type.has_val());
    EXPECT_EQ(docval.has_val(), docval.get()->m_type.has_val());
    EXPECT_EQ(mstream.has_val(), mstream.get()->m_type.has_val());
    EXPECT_EQ(mdoc.has_val(), mdoc.get()->m_type.has_val());
    EXPECT_EQ(mmap.has_val(), mmap.get()->m_type.has_val());
    EXPECT_EQ(mkeyval.has_val(), mkeyval.get()->m_type.has_val());
    EXPECT_EQ(mseq.has_val(), mseq.get()->m_type.has_val());
    EXPECT_EQ(mval.has_val(), mval.get()->m_type.has_val());
    EXPECT_EQ(mdocval.has_val(), mdocval.get()->m_type.has_val());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_val(); });
    verify_assertion(t, [&](Tree const&){ return t.has_val(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_val(NONE); });
}

TEST(NodeType, is_val)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_val());
    EXPECT_FALSE(NodeType(KEY).is_val());
    EXPECT_TRUE(NodeType(VAL).is_val());
    EXPECT_TRUE(NodeType(DOCVAL).is_val());
    EXPECT_FALSE(NodeType(KEYVAL).is_val());
    EXPECT_FALSE(NodeType(KEYMAP).is_val());
    EXPECT_FALSE(NodeType(KEYSEQ).is_val());
}

TEST(Tree, is_val)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_val(stream_id));
    EXPECT_FALSE(t.is_val(doc_id));
    EXPECT_FALSE(t.is_val(map_id));
    EXPECT_FALSE(t.is_val(keyval_id));
    EXPECT_FALSE(t.is_val(seq_id));
    EXPECT_TRUE(t.is_val(val_id));
    EXPECT_TRUE(t.is_val(docval_id));
    EXPECT_FALSE(stream.is_val());
    EXPECT_FALSE(doc.is_val());
    EXPECT_FALSE(map.is_val());
    EXPECT_FALSE(keyval.is_val());
    EXPECT_FALSE(seq.is_val());
    EXPECT_TRUE(val.is_val());
    EXPECT_TRUE(docval.is_val());
    EXPECT_FALSE(mstream.is_val());
    EXPECT_FALSE(mdoc.is_val());
    EXPECT_FALSE(mmap.is_val());
    EXPECT_FALSE(mkeyval.is_val());
    EXPECT_FALSE(mseq.is_val());
    EXPECT_TRUE(mval.is_val());
    EXPECT_TRUE(mdocval.is_val());
    EXPECT_EQ(t.is_val(stream_id), t._p(stream_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(doc_id), t._p(doc_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(map_id), t._p(map_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(keyval_id), t._p(keyval_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(seq_id), t._p(seq_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(val_id), t._p(val_id)->m_type.is_val());
    EXPECT_EQ(t.is_val(docval_id), t._p(docval_id)->m_type.is_val());
    EXPECT_EQ(stream.is_val(), stream.get()->m_type.is_val());
    EXPECT_EQ(doc.is_val(), doc.get()->m_type.is_val());
    EXPECT_EQ(map.is_val(), map.get()->m_type.is_val());
    EXPECT_EQ(keyval.is_val(), keyval.get()->m_type.is_val());
    EXPECT_EQ(seq.is_val(), seq.get()->m_type.is_val());
    EXPECT_EQ(val.is_val(), val.get()->m_type.is_val());
    EXPECT_EQ(docval.is_val(), docval.get()->m_type.is_val());
    EXPECT_EQ(mstream.is_val(), mstream.get()->m_type.is_val());
    EXPECT_EQ(mdoc.is_val(), mdoc.get()->m_type.is_val());
    EXPECT_EQ(mmap.is_val(), mmap.get()->m_type.is_val());
    EXPECT_EQ(mkeyval.is_val(), mkeyval.get()->m_type.is_val());
    EXPECT_EQ(mseq.is_val(), mseq.get()->m_type.is_val());
    EXPECT_EQ(mval.is_val(), mval.get()->m_type.is_val());
    EXPECT_EQ(mdocval.is_val(), mdocval.get()->m_type.is_val());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(1)[1].is_val(); });
    verify_assertion(t, [&](Tree const&){ return t.is_val(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_val(NONE); });
}

TEST(NodeType, has_key)
{
    EXPECT_FALSE(NodeType(NOTYPE).has_key());
    EXPECT_TRUE(NodeType(KEY).has_key());
    EXPECT_FALSE(NodeType(VAL).has_key());
    EXPECT_TRUE(NodeType(KEYVAL).has_key());
    EXPECT_TRUE(NodeType(KEYMAP).has_key());
    EXPECT_TRUE(NodeType(KEYSEQ).has_key());
}

TEST(Tree, key_val_is_null)
{
    Tree t = parse_in_arena(R"(
map: {foo: bar, : knil, vnil: }
seq: [foo, bar]
: knil
vnil:
squoted: ''
dquoted: ""
literal: |
folded: >
)");
    const NodeRef mroot = t.rootref();
    const NodeRef mmap = t["map"];
    const NodeRef mfoo = t["map"]["foo"];
    const NodeRef mfooknil = t["map"][1];
    const NodeRef mfoovnil = t["map"]["vnil"];
    const NodeRef mseq = t["seq"];
    const NodeRef mknil = t[2];
    const NodeRef mvnil = t["vnil"];
    const NodeRef msquo = t["squoted"];
    const NodeRef mdquo = t["dquoted"];
    const NodeRef mliteral = t["literal"];
    const NodeRef mfolded = t["folded"];
    const ConstNodeRef root = mroot;
    const ConstNodeRef map = mmap;
    const ConstNodeRef foo = mfoo;
    const ConstNodeRef fooknil = mfooknil;
    const ConstNodeRef foovnil = mfoovnil;
    const ConstNodeRef seq = mseq;
    const ConstNodeRef knil = mknil;
    const ConstNodeRef vnil = mvnil;
    const ConstNodeRef squo = msquo;
    const ConstNodeRef dquo = mdquo;
    const ConstNodeRef literal = mliteral;
    const ConstNodeRef folded = mfolded;
    const size_t root_id = root.id();
    const size_t map_id = map.id();
    const size_t foo_id = foo.id();
    const size_t fooknil_id = fooknil.id();
    const size_t foovnil_id = foovnil.id();
    const size_t seq_id = seq.id();
    const size_t knil_id = knil.id();
    const size_t vnil_id = vnil.id();
    const size_t squo_id = squo.id();
    const size_t dquo_id = dquo.id();
    const size_t literal_id = literal.id();
    const size_t folded_id = folded.id();
    //
    verify_assertion(t, [&](Tree const&){ return t.key_is_null(root_id); });
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(root_id); });
    EXPECT_FALSE(t.key_is_null(map_id));
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(map_id); });
    EXPECT_FALSE(t.key_is_null(foo_id));
    EXPECT_FALSE(t.val_is_null(foo_id));
    EXPECT_TRUE (t.key_is_null(fooknil_id));
    EXPECT_FALSE(t.val_is_null(fooknil_id));
    EXPECT_FALSE(t.key_is_null(foovnil_id));
    EXPECT_TRUE (t.val_is_null(foovnil_id));
    EXPECT_FALSE(t.key_is_null(seq_id));
    verify_assertion(t, [&](Tree const&){ return t.val_is_null(seq_id); });
    EXPECT_TRUE (t.key_is_null(knil_id));
    EXPECT_FALSE(t.val_is_null(knil_id));
    EXPECT_FALSE(t.key_is_null(vnil_id));
    EXPECT_TRUE (t.val_is_null(vnil_id));
    EXPECT_FALSE(t.key_is_null(squo_id));
    EXPECT_FALSE(t.val_is_null(squo_id));
    EXPECT_FALSE(t.key_is_null(dquo_id));
    EXPECT_FALSE(t.val_is_null(dquo_id));
    EXPECT_FALSE(t.key_is_null(literal_id));
    EXPECT_FALSE(t.val_is_null(literal_id));
    EXPECT_FALSE(t.key_is_null(folded_id));
    EXPECT_FALSE(t.val_is_null(folded_id));
    //
    verify_assertion(t, [&](Tree const&){ return root.key_is_null(); });
    verify_assertion(t, [&](Tree const&){ return root.val_is_null(); });
    EXPECT_FALSE(map.key_is_null());
    verify_assertion(t, [&](Tree const&){ return map.val_is_null(); });
    EXPECT_FALSE(foo.key_is_null());
    EXPECT_FALSE(foo.val_is_null());
    EXPECT_TRUE (fooknil.key_is_null());
    EXPECT_FALSE(fooknil.val_is_null());
    EXPECT_FALSE(foovnil.key_is_null());
    EXPECT_TRUE (foovnil.val_is_null());
    EXPECT_FALSE(seq.key_is_null());
    verify_assertion(t, [&](Tree const&){ seq.val_is_null(); });
    EXPECT_TRUE (knil.key_is_null());
    EXPECT_FALSE(knil.val_is_null());
    EXPECT_FALSE(vnil.key_is_null());
    EXPECT_TRUE (vnil.val_is_null());
    EXPECT_FALSE(squo.key_is_null());
    EXPECT_FALSE(squo.val_is_null());
    EXPECT_FALSE(dquo.key_is_null());
    EXPECT_FALSE(dquo.val_is_null());
    EXPECT_FALSE(literal.key_is_null());
    EXPECT_FALSE(literal.val_is_null());
    EXPECT_FALSE(folded.key_is_null());
    EXPECT_FALSE(folded.val_is_null());
    //
    verify_assertion(t, [&](Tree const&){ return mroot.key_is_null(); });
    verify_assertion(t, [&](Tree const&){ return mroot.val_is_null(); });
    EXPECT_FALSE(mmap.key_is_null());
    verify_assertion(t, [&](Tree const&){ return mmap.val_is_null(); });
    EXPECT_FALSE(mfoo.key_is_null());
    EXPECT_FALSE(mfoo.val_is_null());
    EXPECT_TRUE (mfooknil.key_is_null());
    EXPECT_FALSE(mfooknil.val_is_null());
    EXPECT_FALSE(mfoovnil.key_is_null());
    EXPECT_TRUE (mfoovnil.val_is_null());
    EXPECT_FALSE(mseq.key_is_null());
    verify_assertion(t, [&](Tree const&){ mseq.val_is_null(); });
    EXPECT_TRUE (mknil.key_is_null());
    EXPECT_FALSE(mknil.val_is_null());
    EXPECT_FALSE(mvnil.key_is_null());
    EXPECT_TRUE (mvnil.val_is_null());
    EXPECT_FALSE(msquo.key_is_null());
    EXPECT_FALSE(msquo.val_is_null());
    EXPECT_FALSE(mdquo.key_is_null());
    EXPECT_FALSE(mdquo.val_is_null());
    EXPECT_FALSE(mliteral.key_is_null());
    EXPECT_FALSE(mliteral.val_is_null());
    EXPECT_FALSE(mfolded.key_is_null());
    EXPECT_FALSE(mfolded.val_is_null());
}

TEST(Tree, has_key)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_key(stream_id));
    EXPECT_FALSE(t.has_key(doc_id));
    EXPECT_TRUE(t.has_key(map_id));
    EXPECT_TRUE(t.has_key(keyval_id));
    EXPECT_TRUE(t.has_key(seq_id));
    EXPECT_FALSE(t.has_key(val_id));
    EXPECT_FALSE(t.has_key(docval_id));
    EXPECT_FALSE(stream.has_key());
    EXPECT_FALSE(doc.has_key());
    EXPECT_TRUE(map.has_key());
    EXPECT_TRUE(keyval.has_key());
    EXPECT_TRUE(seq.has_key());
    EXPECT_FALSE(val.has_key());
    EXPECT_FALSE(docval.has_key());
    EXPECT_FALSE(mstream.has_key());
    EXPECT_FALSE(mdoc.has_key());
    EXPECT_TRUE(mmap.has_key());
    EXPECT_TRUE(mkeyval.has_key());
    EXPECT_TRUE(mseq.has_key());
    EXPECT_FALSE(mval.has_key());
    EXPECT_FALSE(mdocval.has_key());
    EXPECT_EQ(t.has_key(stream_id), t._p(stream_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(doc_id), t._p(doc_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(map_id), t._p(map_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(keyval_id), t._p(keyval_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(seq_id), t._p(seq_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(val_id), t._p(val_id)->m_type.has_key());
    EXPECT_EQ(t.has_key(docval_id), t._p(docval_id)->m_type.has_key());
    EXPECT_EQ(stream.has_key(), stream.get()->m_type.has_key());
    EXPECT_EQ(doc.has_key(), doc.get()->m_type.has_key());
    EXPECT_EQ(map.has_key(), map.get()->m_type.has_key());
    EXPECT_EQ(keyval.has_key(), keyval.get()->m_type.has_key());
    EXPECT_EQ(seq.has_key(), seq.get()->m_type.has_key());
    EXPECT_EQ(val.has_key(), val.get()->m_type.has_key());
    EXPECT_EQ(docval.has_key(), docval.get()->m_type.has_key());
    EXPECT_EQ(mstream.has_key(), mstream.get()->m_type.has_key());
    EXPECT_EQ(mdoc.has_key(), mdoc.get()->m_type.has_key());
    EXPECT_EQ(mmap.has_key(), mmap.get()->m_type.has_key());
    EXPECT_EQ(mkeyval.has_key(), mkeyval.get()->m_type.has_key());
    EXPECT_EQ(mseq.has_key(), mseq.get()->m_type.has_key());
    EXPECT_EQ(mval.has_key(), mval.get()->m_type.has_key());
    EXPECT_EQ(mdocval.has_key(), mdocval.get()->m_type.has_key());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_key(); });
    verify_assertion(t, [&](Tree const&){ return t.has_key(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_key(NONE); });
}

TEST(NodeType, is_keyval)
{
    EXPECT_FALSE(NodeType(NOTYPE).is_keyval());
    EXPECT_FALSE(NodeType(KEY).is_keyval());
    EXPECT_FALSE(NodeType(VAL).is_keyval());
    EXPECT_TRUE(NodeType(KEYVAL).is_keyval());
    EXPECT_FALSE(NodeType(DOCVAL).is_keyval());
    EXPECT_FALSE(NodeType(KEYMAP).is_keyval());
    EXPECT_FALSE(NodeType(KEYSEQ).is_keyval());
}

TEST(Tree, is_keyval)
{
    Tree t = parse_in_arena(R"(---
map: {foo: bar}
seq: [foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.is_keyval(stream_id));
    EXPECT_FALSE(t.is_keyval(doc_id));
    EXPECT_FALSE(t.is_keyval(map_id));
    EXPECT_TRUE(t.is_keyval(keyval_id));
    EXPECT_FALSE(t.is_keyval(seq_id));
    EXPECT_FALSE(t.is_keyval(val_id));
    EXPECT_FALSE(t.is_keyval(docval_id));
    EXPECT_FALSE(stream.is_keyval());
    EXPECT_FALSE(doc.is_keyval());
    EXPECT_FALSE(map.is_keyval());
    EXPECT_TRUE(keyval.is_keyval());
    EXPECT_FALSE(seq.is_keyval());
    EXPECT_FALSE(val.is_keyval());
    EXPECT_FALSE(docval.is_keyval());
    EXPECT_FALSE(mstream.is_keyval());
    EXPECT_FALSE(mdoc.is_keyval());
    EXPECT_FALSE(mmap.is_keyval());
    EXPECT_TRUE(mkeyval.is_keyval());
    EXPECT_FALSE(mseq.is_keyval());
    EXPECT_FALSE(mval.is_keyval());
    EXPECT_FALSE(mdocval.is_keyval());
    EXPECT_EQ(t.is_keyval(stream_id), t._p(stream_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(doc_id), t._p(doc_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(map_id), t._p(map_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(keyval_id), t._p(keyval_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(seq_id), t._p(seq_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(val_id), t._p(val_id)->m_type.is_keyval());
    EXPECT_EQ(t.is_keyval(docval_id), t._p(docval_id)->m_type.is_keyval());
    EXPECT_EQ(stream.is_keyval(), stream.get()->m_type.is_keyval());
    EXPECT_EQ(doc.is_keyval(), doc.get()->m_type.is_keyval());
    EXPECT_EQ(map.is_keyval(), map.get()->m_type.is_keyval());
    EXPECT_EQ(keyval.is_keyval(), keyval.get()->m_type.is_keyval());
    EXPECT_EQ(seq.is_keyval(), seq.get()->m_type.is_keyval());
    EXPECT_EQ(val.is_keyval(), val.get()->m_type.is_keyval());
    EXPECT_EQ(docval.is_keyval(), docval.get()->m_type.is_keyval());
    EXPECT_EQ(mstream.is_keyval(), mstream.get()->m_type.is_keyval());
    EXPECT_EQ(mdoc.is_keyval(), mdoc.get()->m_type.is_keyval());
    EXPECT_EQ(mmap.is_keyval(), mmap.get()->m_type.is_keyval());
    EXPECT_EQ(mkeyval.is_keyval(), mkeyval.get()->m_type.is_keyval());
    EXPECT_EQ(mseq.is_keyval(), mseq.get()->m_type.is_keyval());
    EXPECT_EQ(mval.is_keyval(), mval.get()->m_type.is_keyval());
    EXPECT_EQ(mdocval.is_keyval(), mdocval.get()->m_type.is_keyval());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_keyval(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_keyval(); });
    verify_assertion(t, [&](Tree const&){ return t.is_keyval(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_keyval(NONE); });
}

TEST(NodeType, has_key_tag)
{
    EXPECT_FALSE(NodeType().has_key_tag());
    EXPECT_TRUE(NodeType(KEYTAG).has_key_tag());
    EXPECT_TRUE(NodeType(KEY|KEYTAG).has_key_tag());
}

TEST(Tree, has_key_tag)
{
    Tree t = parse_in_arena(R"(--- !docmaptag
!maptag map: {!footag foo: bar, notag: none}
!seqtag seq: [!footag foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnotag_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnotag_id = t.last_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnotag = t.ref(keyvalnotag_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnotag = t.ref(val_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnotag = t.ref(keyvalnotag_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnotag = t.ref(val_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_key_tag(stream_id));
    EXPECT_FALSE(t.has_key_tag(doc_id));
    EXPECT_TRUE(t.has_key_tag(map_id));
    EXPECT_TRUE(t.has_key_tag(keyval_id));
    EXPECT_FALSE(t.has_key_tag(keyvalnotag_id));
    EXPECT_TRUE(t.has_key_tag(seq_id));
    EXPECT_FALSE(t.has_key_tag(val_id));
    EXPECT_FALSE(t.has_key_tag(valnotag_id));
    EXPECT_FALSE(t.has_key_tag(docval_id));
    EXPECT_FALSE(stream.has_key_tag());
    EXPECT_FALSE(doc.has_key_tag());
    EXPECT_TRUE(map.has_key_tag());
    EXPECT_TRUE(keyval.has_key_tag());
    EXPECT_FALSE(keyvalnotag.has_key_tag());
    EXPECT_TRUE(seq.has_key_tag());
    EXPECT_FALSE(val.has_key_tag());
    EXPECT_FALSE(valnotag.has_key_tag());
    EXPECT_FALSE(docval.has_key_tag());
    EXPECT_FALSE(mstream.has_key_tag());
    EXPECT_FALSE(mdoc.has_key_tag());
    EXPECT_TRUE(mmap.has_key_tag());
    EXPECT_TRUE(mkeyval.has_key_tag());
    EXPECT_FALSE(mkeyvalnotag.has_key_tag());
    EXPECT_TRUE(mseq.has_key_tag());
    EXPECT_FALSE(mval.has_key_tag());
    EXPECT_FALSE(mvalnotag.has_key_tag());
    EXPECT_FALSE(mdocval.has_key_tag());
    EXPECT_EQ(t.has_key_tag(stream_id), t._p(stream_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(doc_id), t._p(doc_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(map_id), t._p(map_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(keyval_id), t._p(keyval_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(keyvalnotag_id), t._p(keyvalnotag_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(seq_id), t._p(seq_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(val_id), t._p(val_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(valnotag_id), t._p(valnotag_id)->m_type.has_key_tag());
    EXPECT_EQ(t.has_key_tag(docval_id), t._p(docval_id)->m_type.has_key_tag());
    EXPECT_EQ(stream.has_key_tag(), stream.get()->m_type.has_key_tag());
    EXPECT_EQ(doc.has_key_tag(), doc.get()->m_type.has_key_tag());
    EXPECT_EQ(map.has_key_tag(), map.get()->m_type.has_key_tag());
    EXPECT_EQ(keyval.has_key_tag(), keyval.get()->m_type.has_key_tag());
    EXPECT_EQ(keyvalnotag.has_key_tag(), keyvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(seq.has_key_tag(), seq.get()->m_type.has_key_tag());
    EXPECT_EQ(val.has_key_tag(), val.get()->m_type.has_key_tag());
    EXPECT_EQ(valnotag.has_key_tag(), valnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(docval.has_key_tag(), docval.get()->m_type.has_key_tag());
    EXPECT_EQ(mstream.has_key_tag(), mstream.get()->m_type.has_key_tag());
    EXPECT_EQ(mdoc.has_key_tag(), mdoc.get()->m_type.has_key_tag());
    EXPECT_EQ(mmap.has_key_tag(), mmap.get()->m_type.has_key_tag());
    EXPECT_EQ(mkeyval.has_key_tag(), mkeyval.get()->m_type.has_key_tag());
    EXPECT_EQ(mkeyvalnotag.has_key_tag(), mkeyvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(mseq.has_key_tag(), mseq.get()->m_type.has_key_tag());
    EXPECT_EQ(mval.has_key_tag(), mval.get()->m_type.has_key_tag());
    EXPECT_EQ(mvalnotag.has_key_tag(), mvalnotag.get()->m_type.has_key_tag());
    EXPECT_EQ(mdocval.has_key_tag(), mdocval.get()->m_type.has_key_tag());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key_tag(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_key_tag(); });
    verify_assertion(t, [&](Tree const&){ return t.has_key_tag(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_key_tag(NONE); });
}

TEST(NodeType, has_val_tag)
{
    EXPECT_FALSE(NodeType().has_val_tag());
    EXPECT_TRUE(NodeType(VALTAG).has_val_tag());
    EXPECT_TRUE(NodeType(VAL|VALTAG).has_val_tag());
}

TEST(Tree, has_val_tag)
{
    Tree t = parse_in_arena(R"(--- !docmaptag
map: !maptag {foo: !bartag bar, notag: none}
seq: !seqtag [!footag foo, bar]
---
a scalar
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnotag_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnotag_id = t.last_child(seq_id);
    const id_type docval_id = t.last_child(stream_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnotag = t.ref(keyvalnotag_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnotag = t.ref(valnotag_id);
    ConstNodeRef docval = t.ref(docval_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnotag = t.ref(keyvalnotag_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnotag = t.ref(valnotag_id);
    NodeRef mdocval = t.ref(docval_id);
    EXPECT_FALSE(t.has_val_tag(stream_id));
    EXPECT_TRUE(t.has_val_tag(doc_id));
    EXPECT_TRUE(t.has_val_tag(map_id));
    EXPECT_TRUE(t.has_val_tag(keyval_id));
    EXPECT_FALSE(t.has_val_tag(keyvalnotag_id));
    EXPECT_TRUE(t.has_val_tag(seq_id));
    EXPECT_TRUE(t.has_val_tag(val_id));
    EXPECT_FALSE(t.has_val_tag(valnotag_id));
    EXPECT_FALSE(t.has_val_tag(docval_id));
    EXPECT_FALSE(stream.has_val_tag());
    EXPECT_TRUE(doc.has_val_tag());
    EXPECT_TRUE(map.has_val_tag());
    EXPECT_TRUE(keyval.has_val_tag());
    EXPECT_FALSE(keyvalnotag.has_val_tag());
    EXPECT_TRUE(seq.has_val_tag());
    EXPECT_TRUE(val.has_val_tag());
    EXPECT_FALSE(valnotag.has_val_tag());
    EXPECT_FALSE(docval.has_val_tag());
    EXPECT_FALSE(mstream.has_val_tag());
    EXPECT_TRUE(mdoc.has_val_tag());
    EXPECT_TRUE(mmap.has_val_tag());
    EXPECT_TRUE(mkeyval.has_val_tag());
    EXPECT_FALSE(mkeyvalnotag.has_val_tag());
    EXPECT_TRUE(mseq.has_val_tag());
    EXPECT_TRUE(mval.has_val_tag());
    EXPECT_FALSE(mvalnotag.has_val_tag());
    EXPECT_FALSE(mdocval.has_val_tag());
    EXPECT_EQ(t.has_val_tag(stream_id), t._p(stream_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(doc_id), t._p(doc_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(map_id), t._p(map_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(keyval_id), t._p(keyval_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(keyvalnotag_id), t._p(keyvalnotag_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(seq_id), t._p(seq_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(val_id), t._p(val_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(valnotag_id), t._p(valnotag_id)->m_type.has_val_tag());
    EXPECT_EQ(t.has_val_tag(docval_id), t._p(docval_id)->m_type.has_val_tag());
    EXPECT_EQ(stream.has_val_tag(), stream.get()->m_type.has_val_tag());
    EXPECT_EQ(doc.has_val_tag(), doc.get()->m_type.has_val_tag());
    EXPECT_EQ(map.has_val_tag(), map.get()->m_type.has_val_tag());
    EXPECT_EQ(keyval.has_val_tag(), keyval.get()->m_type.has_val_tag());
    EXPECT_EQ(keyvalnotag.has_val_tag(), keyvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(seq.has_val_tag(), seq.get()->m_type.has_val_tag());
    EXPECT_EQ(val.has_val_tag(), val.get()->m_type.has_val_tag());
    EXPECT_EQ(valnotag.has_val_tag(), valnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(docval.has_val_tag(), docval.get()->m_type.has_val_tag());
    EXPECT_EQ(mstream.has_val_tag(), mstream.get()->m_type.has_val_tag());
    EXPECT_EQ(mdoc.has_val_tag(), mdoc.get()->m_type.has_val_tag());
    EXPECT_EQ(mmap.has_val_tag(), mmap.get()->m_type.has_val_tag());
    EXPECT_EQ(mkeyval.has_val_tag(), mkeyval.get()->m_type.has_val_tag());
    EXPECT_EQ(mkeyvalnotag.has_val_tag(), mkeyvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(mseq.has_val_tag(), mseq.get()->m_type.has_val_tag());
    EXPECT_EQ(mval.has_val_tag(), mval.get()->m_type.has_val_tag());
    EXPECT_EQ(mvalnotag.has_val_tag(), mvalnotag.get()->m_type.has_val_tag());
    EXPECT_EQ(mdocval.has_val_tag(), mdocval.get()->m_type.has_val_tag());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val_tag(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_val_tag(); });
    verify_assertion(t, [&](Tree const&){ return t.has_val_tag(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_val_tag(NONE); });
}

TEST(NodeType, has_key_anchor)
{
    EXPECT_FALSE(NodeType().has_key_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_key_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_key_anchor());
}

TEST(Tree, has_key_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
&mapanchor map: {&keyvalanchor foo: bar, anchor: none}
&seqanchor seq: [&valanchor foo, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.ref(keyvalnoanchor_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnoanchor = t.ref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_key_anchor(stream_id));
    EXPECT_FALSE(t.has_key_anchor(doc_id));
    EXPECT_TRUE(t.has_key_anchor(map_id));
    EXPECT_TRUE(t.has_key_anchor(keyval_id));
    EXPECT_FALSE(t.has_key_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_key_anchor(seq_id));
    EXPECT_FALSE(t.has_key_anchor(val_id));
    EXPECT_FALSE(t.has_key_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_key_anchor());
    EXPECT_FALSE(doc.has_key_anchor());
    EXPECT_TRUE(map.has_key_anchor());
    EXPECT_TRUE(keyval.has_key_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_key_anchor());
    EXPECT_TRUE(seq.has_key_anchor());
    EXPECT_FALSE(val.has_key_anchor());
    EXPECT_FALSE(valnoanchor.has_key_anchor());
    EXPECT_FALSE(mstream.has_key_anchor());
    EXPECT_FALSE(mdoc.has_key_anchor());
    EXPECT_TRUE(mmap.has_key_anchor());
    EXPECT_TRUE(mkeyval.has_key_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_key_anchor());
    EXPECT_TRUE(mseq.has_key_anchor());
    EXPECT_FALSE(mval.has_key_anchor());
    EXPECT_FALSE(mvalnoanchor.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(stream_id), t._p(stream_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(doc_id), t._p(doc_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(map_id), t._p(map_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(keyval_id), t._p(keyval_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(seq_id), t._p(seq_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(val_id), t._p(val_id)->m_type.has_key_anchor());
    EXPECT_EQ(t.has_key_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_key_anchor());
    EXPECT_EQ(stream.has_key_anchor(), stream.get()->m_type.has_key_anchor());
    EXPECT_EQ(doc.has_key_anchor(), doc.get()->m_type.has_key_anchor());
    EXPECT_EQ(map.has_key_anchor(), map.get()->m_type.has_key_anchor());
    EXPECT_EQ(keyval.has_key_anchor(), keyval.get()->m_type.has_key_anchor());
    EXPECT_EQ(keyvalnoanchor.has_key_anchor(), keyvalnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(seq.has_key_anchor(), seq.get()->m_type.has_key_anchor());
    EXPECT_EQ(val.has_key_anchor(), val.get()->m_type.has_key_anchor());
    EXPECT_EQ(valnoanchor.has_key_anchor(), valnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(mstream.has_key_anchor(), mstream.get()->m_type.has_key_anchor());
    EXPECT_EQ(mdoc.has_key_anchor(), mdoc.get()->m_type.has_key_anchor());
    EXPECT_EQ(mmap.has_key_anchor(), mmap.get()->m_type.has_key_anchor());
    EXPECT_EQ(mkeyval.has_key_anchor(), mkeyval.get()->m_type.has_key_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_key_anchor(), mkeyvalnoanchor.get()->m_type.has_key_anchor());
    EXPECT_EQ(mseq.has_key_anchor(), mseq.get()->m_type.has_key_anchor());
    EXPECT_EQ(mval.has_key_anchor(), mval.get()->m_type.has_key_anchor());
    EXPECT_EQ(mvalnoanchor.has_key_anchor(), mvalnoanchor.get()->m_type.has_key_anchor());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_key(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(1).has_key_anchor(); });
    verify_assertion(t, [&](Tree const&){ return t.has_key_anchor(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_key_anchor(NONE); });
}

TEST(NodeType, has_val_anchor)
{
    EXPECT_FALSE(NodeType().has_val_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_val_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_val_anchor());
}

TEST(Tree, has_val_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
map: &mapanchor {foo: &keyvalanchor bar, anchor: none}
seq: &seqanchor [&valanchor foo, bar]
...)");
    _c4dbg_tree(t);
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef keyval = t.cref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.cref(keyvalnoanchor_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef val = t.cref(val_id);
    ConstNodeRef valnoanchor = t.cref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_val_anchor(stream_id));
    EXPECT_TRUE(t.has_val_anchor(doc_id));
    EXPECT_TRUE(t.has_val_anchor(map_id));
    EXPECT_TRUE(t.has_val_anchor(keyval_id));
    EXPECT_FALSE(t.has_val_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_val_anchor(seq_id));
    EXPECT_TRUE(t.has_val_anchor(val_id));
    EXPECT_FALSE(t.has_val_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_val_anchor());
    EXPECT_TRUE(doc.has_val_anchor());
    EXPECT_TRUE(map.has_val_anchor());
    EXPECT_TRUE(keyval.has_val_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_val_anchor());
    EXPECT_TRUE(seq.has_val_anchor());
    EXPECT_TRUE(val.has_val_anchor());
    EXPECT_FALSE(valnoanchor.has_val_anchor());
    EXPECT_FALSE(mstream.has_val_anchor());
    EXPECT_TRUE(mdoc.has_val_anchor());
    EXPECT_TRUE(mmap.has_val_anchor());
    EXPECT_TRUE(mkeyval.has_val_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_val_anchor());
    EXPECT_TRUE(mseq.has_val_anchor());
    EXPECT_TRUE(mval.has_val_anchor());
    EXPECT_FALSE(mvalnoanchor.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(stream_id), t._p(stream_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(doc_id), t._p(doc_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(map_id), t._p(map_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(keyval_id), t._p(keyval_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(seq_id), t._p(seq_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(val_id), t._p(val_id)->m_type.has_val_anchor());
    EXPECT_EQ(t.has_val_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_val_anchor());
    EXPECT_EQ(stream.has_val_anchor(), stream.get()->m_type.has_val_anchor());
    EXPECT_EQ(doc.has_val_anchor(), doc.get()->m_type.has_val_anchor());
    EXPECT_EQ(map.has_val_anchor(), map.get()->m_type.has_val_anchor());
    EXPECT_EQ(keyval.has_val_anchor(), keyval.get()->m_type.has_val_anchor());
    EXPECT_EQ(keyvalnoanchor.has_val_anchor(), keyvalnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(seq.has_val_anchor(), seq.get()->m_type.has_val_anchor());
    EXPECT_EQ(val.has_val_anchor(), val.get()->m_type.has_val_anchor());
    EXPECT_EQ(valnoanchor.has_val_anchor(), valnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(mstream.has_val_anchor(), mstream.get()->m_type.has_val_anchor());
    EXPECT_EQ(mdoc.has_val_anchor(), mdoc.get()->m_type.has_val_anchor());
    EXPECT_EQ(mmap.has_val_anchor(), mmap.get()->m_type.has_val_anchor());
    EXPECT_EQ(mkeyval.has_val_anchor(), mkeyval.get()->m_type.has_val_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_val_anchor(), mkeyvalnoanchor.get()->m_type.has_val_anchor());
    EXPECT_EQ(mseq.has_val_anchor(), mseq.get()->m_type.has_val_anchor());
    EXPECT_EQ(mval.has_val_anchor(), mval.get()->m_type.has_val_anchor());
    EXPECT_EQ(mvalnoanchor.has_val_anchor(), mvalnoanchor.get()->m_type.has_val_anchor());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_val_anchor(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(1).has_val_anchor(); });
    verify_assertion(t, [&](Tree const&){ return t.has_val_anchor(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_val_anchor(NONE); });
}

TEST(NodeType, has_anchor)
{
    EXPECT_FALSE(NodeType().has_anchor());
    EXPECT_TRUE(NodeType(VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEYANCH|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|VALANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(KEY|KEYANCH).has_anchor());
    EXPECT_TRUE(NodeType(VAL|VALANCH).has_anchor());
}

TEST(Tree, has_anchor)
{
    Tree t = parse_in_arena(R"(--- &docanchor
map: &mapanchor {foo: &keyvalanchor bar, anchor: none}
&seqanchor seq: [&valanchor foo, bar]
...)");
    _c4dbg_tree(t);
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type keyvalnoanchor_id = t.last_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    const id_type valnoanchor_id = t.last_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef keyvalnoanchor = t.ref(keyvalnoanchor_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    ConstNodeRef valnoanchor = t.ref(valnoanchor_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mkeyvalnoanchor = t.ref(keyvalnoanchor_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    NodeRef mvalnoanchor = t.ref(valnoanchor_id);
    EXPECT_FALSE(t.has_anchor(stream_id));
    EXPECT_TRUE(t.has_anchor(doc_id));
    EXPECT_TRUE(t.has_anchor(map_id));
    EXPECT_TRUE(t.has_anchor(keyval_id));
    EXPECT_FALSE(t.has_anchor(keyvalnoanchor_id));
    EXPECT_TRUE(t.has_anchor(seq_id));
    EXPECT_TRUE(t.has_anchor(val_id));
    EXPECT_FALSE(t.has_anchor(valnoanchor_id));
    EXPECT_FALSE(stream.has_anchor());
    EXPECT_TRUE(doc.has_anchor());
    EXPECT_TRUE(map.has_anchor());
    EXPECT_TRUE(keyval.has_anchor());
    EXPECT_FALSE(keyvalnoanchor.has_anchor());
    EXPECT_TRUE(seq.has_anchor());
    EXPECT_TRUE(val.has_anchor());
    EXPECT_FALSE(valnoanchor.has_anchor());
    EXPECT_FALSE(mstream.has_anchor());
    EXPECT_TRUE(mdoc.has_anchor());
    EXPECT_TRUE(mmap.has_anchor());
    EXPECT_TRUE(mkeyval.has_anchor());
    EXPECT_FALSE(mkeyvalnoanchor.has_anchor());
    EXPECT_TRUE(mseq.has_anchor());
    EXPECT_TRUE(mval.has_anchor());
    EXPECT_FALSE(mvalnoanchor.has_anchor());
    EXPECT_EQ(t.has_anchor(stream_id), t._p(stream_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(doc_id), t._p(doc_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(map_id), t._p(map_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(keyval_id), t._p(keyval_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(keyvalnoanchor_id), t._p(keyvalnoanchor_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(seq_id), t._p(seq_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(val_id), t._p(val_id)->m_type.has_anchor());
    EXPECT_EQ(t.has_anchor(valnoanchor_id), t._p(valnoanchor_id)->m_type.has_anchor());
    EXPECT_EQ(stream.has_anchor(), stream.get()->m_type.has_anchor());
    EXPECT_EQ(doc.has_anchor(), doc.get()->m_type.has_anchor());
    EXPECT_EQ(map.has_anchor(), map.get()->m_type.has_anchor());
    EXPECT_EQ(keyval.has_anchor(), keyval.get()->m_type.has_anchor());
    EXPECT_EQ(keyvalnoanchor.has_anchor(), keyvalnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(seq.has_anchor(), seq.get()->m_type.has_anchor());
    EXPECT_EQ(val.has_anchor(), val.get()->m_type.has_anchor());
    EXPECT_EQ(valnoanchor.has_anchor(), valnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(mstream.has_anchor(), mstream.get()->m_type.has_anchor());
    EXPECT_EQ(mdoc.has_anchor(), mdoc.get()->m_type.has_anchor());
    EXPECT_EQ(mmap.has_anchor(), mmap.get()->m_type.has_anchor());
    EXPECT_EQ(mkeyval.has_anchor(), mkeyval.get()->m_type.has_anchor());
    EXPECT_EQ(mkeyvalnoanchor.has_anchor(), mkeyvalnoanchor.get()->m_type.has_anchor());
    EXPECT_EQ(mseq.has_anchor(), mseq.get()->m_type.has_anchor());
    EXPECT_EQ(mval.has_anchor(), mval.get()->m_type.has_anchor());
    EXPECT_EQ(mvalnoanchor.has_anchor(), mvalnoanchor.get()->m_type.has_anchor());
    verify_assertion(t, [&](Tree const&){ return t.has_anchor(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_anchor(NONE); });
}

TEST(NodeType, is_key_ref)
{
    EXPECT_FALSE(NodeType().is_key_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_key_ref());
    EXPECT_TRUE(NodeType(KEY|KEYREF).is_key_ref());
}

TEST(Tree, is_key_ref)
{
    Tree t = parse_in_arena(R"(---
*mapref : {foo: bar, notag: none}
*seqref : [foo, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef keyval = t.cref(keyval_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef val = t.cref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.is_key_ref(stream_id));
    EXPECT_FALSE(t.is_key_ref(doc_id));
    EXPECT_TRUE(t.is_key_ref(map_id));
    EXPECT_FALSE(t.is_key_ref(keyval_id));
    EXPECT_TRUE(t.is_key_ref(seq_id));
    EXPECT_FALSE(t.is_key_ref(val_id));
    EXPECT_FALSE(stream.is_key_ref());
    EXPECT_FALSE(doc.is_key_ref());
    EXPECT_TRUE(map.is_key_ref());
    EXPECT_FALSE(keyval.is_key_ref());
    EXPECT_TRUE(seq.is_key_ref());
    EXPECT_FALSE(val.is_key_ref());
    EXPECT_FALSE(mstream.is_key_ref());
    EXPECT_FALSE(mdoc.is_key_ref());
    EXPECT_TRUE(mmap.is_key_ref());
    EXPECT_FALSE(mkeyval.is_key_ref());
    EXPECT_TRUE(mseq.is_key_ref());
    EXPECT_FALSE(mval.is_key_ref());
    EXPECT_EQ(t.is_key_ref(stream_id), t._p(stream_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(doc_id), t._p(doc_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(map_id), t._p(map_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(keyval_id), t._p(keyval_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(seq_id), t._p(seq_id)->m_type.is_key_ref());
    EXPECT_EQ(t.is_key_ref(val_id), t._p(val_id)->m_type.is_key_ref());
    EXPECT_EQ(stream.is_key_ref(), stream.get()->m_type.is_key_ref());
    EXPECT_EQ(doc.is_key_ref(), doc.get()->m_type.is_key_ref());
    EXPECT_EQ(map.is_key_ref(), map.get()->m_type.is_key_ref());
    EXPECT_EQ(keyval.is_key_ref(), keyval.get()->m_type.is_key_ref());
    EXPECT_EQ(seq.is_key_ref(), seq.get()->m_type.is_key_ref());
    EXPECT_EQ(val.is_key_ref(), val.get()->m_type.is_key_ref());
    EXPECT_EQ(mstream.is_key_ref(), mstream.get()->m_type.is_key_ref());
    EXPECT_EQ(mdoc.is_key_ref(), mdoc.get()->m_type.is_key_ref());
    EXPECT_EQ(mmap.is_key_ref(), mmap.get()->m_type.is_key_ref());
    EXPECT_EQ(mkeyval.is_key_ref(), mkeyval.get()->m_type.is_key_ref());
    EXPECT_EQ(mseq.is_key_ref(), mseq.get()->m_type.is_key_ref());
    EXPECT_EQ(mval.is_key_ref(), mval.get()->m_type.is_key_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_key_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_key_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.is_key_ref(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_key_ref(NONE); });
}

TEST(NodeType, is_val_ref)
{
    EXPECT_FALSE(NodeType().is_val_ref());
    EXPECT_TRUE(NodeType(VALREF).is_val_ref());
    EXPECT_TRUE(NodeType(VAL|VALREF).is_val_ref());
}

TEST(Tree, is_val_ref)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.is_val_ref(stream_id));
    EXPECT_FALSE(t.is_val_ref(doc_id));
    EXPECT_FALSE(t.is_val_ref(map_id));
    EXPECT_TRUE(t.is_val_ref(keyval_id));
    EXPECT_FALSE(t.is_val_ref(seq_id));
    EXPECT_TRUE(t.is_val_ref(val_id));
    EXPECT_FALSE(stream.is_val_ref());
    EXPECT_FALSE(doc.is_val_ref());
    EXPECT_FALSE(map.is_val_ref());
    EXPECT_TRUE(keyval.is_val_ref());
    EXPECT_FALSE(seq.is_val_ref());
    EXPECT_TRUE(val.is_val_ref());
    EXPECT_FALSE(mstream.is_val_ref());
    EXPECT_FALSE(mdoc.is_val_ref());
    EXPECT_FALSE(mmap.is_val_ref());
    EXPECT_TRUE(mkeyval.is_val_ref());
    EXPECT_FALSE(mseq.is_val_ref());
    EXPECT_TRUE(mval.is_val_ref());
    EXPECT_EQ(t.is_val_ref(stream_id), t._p(stream_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(doc_id), t._p(doc_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(map_id), t._p(map_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(keyval_id), t._p(keyval_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(seq_id), t._p(seq_id)->m_type.is_val_ref());
    EXPECT_EQ(t.is_val_ref(val_id), t._p(val_id)->m_type.is_val_ref());
    EXPECT_EQ(stream.is_val_ref(), stream.get()->m_type.is_val_ref());
    EXPECT_EQ(doc.is_val_ref(), doc.get()->m_type.is_val_ref());
    EXPECT_EQ(map.is_val_ref(), map.get()->m_type.is_val_ref());
    EXPECT_EQ(keyval.is_val_ref(), keyval.get()->m_type.is_val_ref());
    EXPECT_EQ(seq.is_val_ref(), seq.get()->m_type.is_val_ref());
    EXPECT_EQ(val.is_val_ref(), val.get()->m_type.is_val_ref());
    EXPECT_EQ(mstream.is_val_ref(), mstream.get()->m_type.is_val_ref());
    EXPECT_EQ(mdoc.is_val_ref(), mdoc.get()->m_type.is_val_ref());
    EXPECT_EQ(mmap.is_val_ref(), mmap.get()->m_type.is_val_ref());
    EXPECT_EQ(mkeyval.is_val_ref(), mkeyval.get()->m_type.is_val_ref());
    EXPECT_EQ(mseq.is_val_ref(), mseq.get()->m_type.is_val_ref());
    EXPECT_EQ(mval.is_val_ref(), mval.get()->m_type.is_val_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_val_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.is_val_ref(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_val_ref(NONE); });
}

TEST(NodeType, is_ref)
{
    EXPECT_FALSE(NodeType().is_ref());
    EXPECT_FALSE(NodeType(KEYVAL).is_ref());
    EXPECT_TRUE(NodeType(KEYREF).is_ref());
    EXPECT_TRUE(NodeType(VALREF).is_ref());
    EXPECT_TRUE(NodeType(KEY|VALREF).is_ref());
    EXPECT_TRUE(NodeType(VAL|KEYREF).is_ref());
    EXPECT_TRUE(NodeType(KEYREF|VALREF).is_ref());
}

TEST(Tree, is_ref)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.is_ref(stream_id));
    EXPECT_FALSE(t.is_ref(doc_id));
    EXPECT_FALSE(t.is_ref(map_id));
    EXPECT_TRUE(t.is_ref(keyval_id));
    EXPECT_FALSE(t.is_ref(seq_id));
    EXPECT_TRUE(t.is_ref(val_id));
    EXPECT_FALSE(stream.is_ref());
    EXPECT_FALSE(doc.is_ref());
    EXPECT_FALSE(map.is_ref());
    EXPECT_TRUE(keyval.is_ref());
    EXPECT_FALSE(seq.is_ref());
    EXPECT_TRUE(val.is_ref());
    EXPECT_FALSE(mstream.is_ref());
    EXPECT_FALSE(mdoc.is_ref());
    EXPECT_FALSE(mmap.is_ref());
    EXPECT_TRUE(mkeyval.is_ref());
    EXPECT_FALSE(mseq.is_ref());
    EXPECT_TRUE(mval.is_ref());
    EXPECT_EQ(t.is_ref(stream_id), t._p(stream_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(doc_id), t._p(doc_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(map_id), t._p(map_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(keyval_id), t._p(keyval_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(seq_id), t._p(seq_id)->m_type.is_ref());
    EXPECT_EQ(t.is_ref(val_id), t._p(val_id)->m_type.is_ref());
    EXPECT_EQ(stream.is_ref(), stream.get()->m_type.is_ref());
    EXPECT_EQ(doc.is_ref(), doc.get()->m_type.is_ref());
    EXPECT_EQ(map.is_ref(), map.get()->m_type.is_ref());
    EXPECT_EQ(keyval.is_ref(), keyval.get()->m_type.is_ref());
    EXPECT_EQ(seq.is_ref(), seq.get()->m_type.is_ref());
    EXPECT_EQ(val.is_ref(), val.get()->m_type.is_ref());
    EXPECT_EQ(mstream.is_ref(), mstream.get()->m_type.is_ref());
    EXPECT_EQ(mdoc.is_ref(), mdoc.get()->m_type.is_ref());
    EXPECT_EQ(mmap.is_ref(), mmap.get()->m_type.is_ref());
    EXPECT_EQ(mkeyval.is_ref(), mkeyval.get()->m_type.is_ref());
    EXPECT_EQ(mseq.is_ref(), mseq.get()->m_type.is_ref());
    EXPECT_EQ(mval.is_ref(), mval.get()->m_type.is_ref());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_ref(); });
    verify_assertion(t, [&](Tree const&){ return t.is_ref(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_ref(NONE); });
}

TEST(NodeType, is_key_quoted)
{
    EXPECT_FALSE(NodeType().is_key_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_key_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_key_quoted());
}

TEST(Tree, is_key_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted": foo
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted_id = t.first_child(map_id);
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted = t.ref(quoted_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted = t.ref(quoted_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_key_quoted(map_id));
    EXPECT_TRUE(t.is_key_quoted(quoted_id));
    EXPECT_FALSE(t.is_key_quoted(notquoted_id));
    EXPECT_FALSE(map.is_key_quoted());
    EXPECT_TRUE(quoted.is_key_quoted());
    EXPECT_FALSE(notquoted.is_key_quoted());
    EXPECT_FALSE(mmap.is_key_quoted());
    EXPECT_TRUE(mquoted.is_key_quoted());
    EXPECT_FALSE(mnotquoted.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(map_id), t._p(map_id)->m_type.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(quoted_id), t._p(quoted_id)->m_type.is_key_quoted());
    EXPECT_EQ(t.is_key_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_key_quoted());
    EXPECT_EQ(map.is_key_quoted(), map.get()->m_type.is_key_quoted());
    EXPECT_EQ(quoted.is_key_quoted(), quoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(notquoted.is_key_quoted(), notquoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(mmap.is_key_quoted(), mmap.get()->m_type.is_key_quoted());
    EXPECT_EQ(mquoted.is_key_quoted(), mquoted.get()->m_type.is_key_quoted());
    EXPECT_EQ(mnotquoted.is_key_quoted(), mnotquoted.get()->m_type.is_key_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_key_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_key_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.is_key_quoted(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_key_quoted(NONE); });
}

TEST(NodeType, is_val_quoted)
{
    EXPECT_FALSE(NodeType().is_val_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_val_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_val_quoted());
}

TEST(Tree, is_val_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted": "foo"
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted_id = t.first_child(map_id);
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted = t.ref(quoted_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted = t.ref(quoted_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_val_quoted(map_id));
    EXPECT_TRUE(t.is_val_quoted(quoted_id));
    EXPECT_FALSE(t.is_val_quoted(notquoted_id));
    EXPECT_FALSE(map.is_val_quoted());
    EXPECT_TRUE(quoted.is_val_quoted());
    EXPECT_FALSE(notquoted.is_val_quoted());
    EXPECT_FALSE(mmap.is_val_quoted());
    EXPECT_TRUE(mquoted.is_val_quoted());
    EXPECT_FALSE(mnotquoted.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(map_id), t._p(map_id)->m_type.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(quoted_id), t._p(quoted_id)->m_type.is_val_quoted());
    EXPECT_EQ(t.is_val_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_val_quoted());
    EXPECT_EQ(map.is_val_quoted(), map.get()->m_type.is_val_quoted());
    EXPECT_EQ(quoted.is_val_quoted(), quoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(notquoted.is_val_quoted(), notquoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(mmap.is_val_quoted(), mmap.get()->m_type.is_val_quoted());
    EXPECT_EQ(mquoted.is_val_quoted(), mquoted.get()->m_type.is_val_quoted());
    EXPECT_EQ(mnotquoted.is_val_quoted(), mnotquoted.get()->m_type.is_val_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_val_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_val_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.is_val_quoted(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_val_quoted(NONE); });
}

TEST(NodeType, is_quoted)
{
    EXPECT_FALSE(NodeType().is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEYQUO|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|KEYQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(KEY|VALQUO).is_quoted());
    EXPECT_TRUE(NodeType(VAL|KEYQUO).is_quoted());
}

TEST(Tree, is_quoted)
{
    Tree t = parse_in_arena(R"(---
"quoted1": foo
quoted2: "foo"
"quoted3": "foo"
'quoted4': foo
quoted5: 'foo'
'quoted6': 'foo'
notquoted: bar
...)");
    const id_type map_id = t.first_child(t.root_id());
    const id_type quoted1_id = t.find_child(map_id, "quoted1");
    const id_type quoted2_id = t.find_child(map_id, "quoted2");
    const id_type quoted3_id = t.find_child(map_id, "quoted3");
    const id_type quoted4_id = t.find_child(map_id, "quoted4");
    const id_type quoted5_id = t.find_child(map_id, "quoted5");
    const id_type quoted6_id = t.find_child(map_id, "quoted6");
    const id_type notquoted_id = t.last_child(map_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef quoted1 = t.ref(quoted1_id);
    ConstNodeRef quoted2 = t.ref(quoted2_id);
    ConstNodeRef quoted3 = t.ref(quoted3_id);
    ConstNodeRef quoted4 = t.ref(quoted4_id);
    ConstNodeRef quoted5 = t.ref(quoted5_id);
    ConstNodeRef quoted6 = t.ref(quoted6_id);
    ConstNodeRef notquoted = t.ref(notquoted_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mquoted1 = t.ref(quoted1_id);
    NodeRef mquoted2 = t.ref(quoted2_id);
    NodeRef mquoted3 = t.ref(quoted3_id);
    NodeRef mquoted4 = t.ref(quoted4_id);
    NodeRef mquoted5 = t.ref(quoted5_id);
    NodeRef mquoted6 = t.ref(quoted6_id);
    NodeRef mnotquoted = t.ref(notquoted_id);
    EXPECT_FALSE(t.is_quoted(map_id));
    EXPECT_TRUE(t.is_quoted(quoted1_id));
    EXPECT_TRUE(t.is_quoted(quoted2_id));
    EXPECT_TRUE(t.is_quoted(quoted3_id));
    EXPECT_TRUE(t.is_quoted(quoted4_id));
    EXPECT_TRUE(t.is_quoted(quoted5_id));
    EXPECT_TRUE(t.is_quoted(quoted6_id));
    EXPECT_FALSE(t.is_quoted(notquoted_id));
    EXPECT_FALSE(map.is_quoted());
    EXPECT_TRUE(quoted1.is_quoted());
    EXPECT_TRUE(quoted2.is_quoted());
    EXPECT_TRUE(quoted3.is_quoted());
    EXPECT_TRUE(quoted4.is_quoted());
    EXPECT_TRUE(quoted5.is_quoted());
    EXPECT_TRUE(quoted6.is_quoted());
    EXPECT_FALSE(notquoted.is_quoted());
    EXPECT_FALSE(mmap.is_quoted());
    EXPECT_TRUE(mquoted1.is_quoted());
    EXPECT_TRUE(mquoted2.is_quoted());
    EXPECT_TRUE(mquoted3.is_quoted());
    EXPECT_TRUE(mquoted4.is_quoted());
    EXPECT_TRUE(mquoted5.is_quoted());
    EXPECT_TRUE(mquoted6.is_quoted());
    EXPECT_FALSE(mnotquoted.is_quoted());
    EXPECT_EQ(t.is_quoted(map_id), t._p(map_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted1_id), t._p(quoted1_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted2_id), t._p(quoted2_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted3_id), t._p(quoted3_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted4_id), t._p(quoted4_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted5_id), t._p(quoted5_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(quoted6_id), t._p(quoted6_id)->m_type.is_quoted());
    EXPECT_EQ(t.is_quoted(notquoted_id), t._p(notquoted_id)->m_type.is_quoted());
    EXPECT_EQ(map.is_quoted(), map.get()->m_type.is_quoted());
    EXPECT_EQ(quoted1.is_quoted(), quoted1.get()->m_type.is_quoted());
    EXPECT_EQ(quoted2.is_quoted(), quoted2.get()->m_type.is_quoted());
    EXPECT_EQ(quoted3.is_quoted(), quoted3.get()->m_type.is_quoted());
    EXPECT_EQ(quoted4.is_quoted(), quoted4.get()->m_type.is_quoted());
    EXPECT_EQ(quoted5.is_quoted(), quoted5.get()->m_type.is_quoted());
    EXPECT_EQ(quoted6.is_quoted(), quoted6.get()->m_type.is_quoted());
    EXPECT_EQ(notquoted.is_quoted(), notquoted.get()->m_type.is_quoted());
    EXPECT_EQ(mmap.is_quoted(), mmap.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted1.is_quoted(), mquoted1.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted2.is_quoted(), mquoted2.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted3.is_quoted(), mquoted3.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted4.is_quoted(), mquoted4.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted5.is_quoted(), mquoted5.get()->m_type.is_quoted());
    EXPECT_EQ(mquoted6.is_quoted(), mquoted6.get()->m_type.is_quoted());
    EXPECT_EQ(mnotquoted.is_quoted(), mnotquoted.get()->m_type.is_quoted());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].is_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).is_quoted(); });
    verify_assertion(t, [&](Tree const&){ return t.is_quoted(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.is_quoted(NONE); });
}


TEST(Tree, parent_is_seq)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    //EXPECT_FALSE(t.parent_is_seq(stream_id));
    EXPECT_TRUE(t.parent_is_seq(doc_id));
    EXPECT_FALSE(t.parent_is_seq(map_id));
    EXPECT_FALSE(t.parent_is_seq(keyval_id));
    EXPECT_FALSE(t.parent_is_seq(seq_id));
    EXPECT_TRUE(t.parent_is_seq(val_id));
    //EXPECT_FALSE(stream.parent_is_seq());
    EXPECT_TRUE(doc.parent_is_seq());
    EXPECT_FALSE(map.parent_is_seq());
    EXPECT_FALSE(keyval.parent_is_seq());
    EXPECT_FALSE(seq.parent_is_seq());
    EXPECT_TRUE(val.parent_is_seq());
    //EXPECT_FALSE(mstream.parent_is_seq());
    EXPECT_TRUE(mdoc.parent_is_seq());
    EXPECT_FALSE(mmap.parent_is_seq());
    EXPECT_FALSE(mkeyval.parent_is_seq());
    EXPECT_FALSE(mseq.parent_is_seq());
    EXPECT_TRUE(mval.parent_is_seq());
    //EXPECT_EQ(t.parent_is_seq(stream_id), stream.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(doc_id), doc.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(map_id), map.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(keyval_id), keyval.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(seq_id), seq.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(val_id), val.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(doc_id), mdoc.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(map_id), mmap.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(keyval_id), mkeyval.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(seq_id), mseq.parent_is_seq());
    EXPECT_EQ(t.parent_is_seq(val_id), mval.parent_is_seq());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].parent_is_seq(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).parent_is_seq(); });
    verify_assertion(t, [&](Tree const&){ return t.parent_is_seq(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.parent_is_seq(NONE); });
}

TEST(Tree, parent_is_map)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    //EXPECT_FALSE(t.parent_is_map(stream_id));
    EXPECT_FALSE(t.parent_is_map(doc_id));
    EXPECT_TRUE(t.parent_is_map(map_id));
    EXPECT_TRUE(t.parent_is_map(keyval_id));
    EXPECT_TRUE(t.parent_is_map(seq_id));
    EXPECT_FALSE(t.parent_is_map(val_id));
    //EXPECT_FALSE(stream.parent_is_map());
    EXPECT_FALSE(doc.parent_is_map());
    EXPECT_TRUE(map.parent_is_map());
    EXPECT_TRUE(keyval.parent_is_map());
    EXPECT_TRUE(seq.parent_is_map());
    EXPECT_FALSE(val.parent_is_map());
    //EXPECT_FALSE(mstream.parent_is_map());
    EXPECT_FALSE(mdoc.parent_is_map());
    EXPECT_TRUE(mmap.parent_is_map());
    EXPECT_TRUE(mkeyval.parent_is_map());
    EXPECT_TRUE(mseq.parent_is_map());
    EXPECT_FALSE(mval.parent_is_map());
    //EXPECT_EQ(t.parent_is_map(stream_id), stream.parent_is_map());
    EXPECT_EQ(t.parent_is_map(doc_id), doc.parent_is_map());
    EXPECT_EQ(t.parent_is_map(map_id), map.parent_is_map());
    EXPECT_EQ(t.parent_is_map(keyval_id), keyval.parent_is_map());
    EXPECT_EQ(t.parent_is_map(seq_id), seq.parent_is_map());
    EXPECT_EQ(t.parent_is_map(val_id), val.parent_is_map());
    //EXPECT_EQ(t.parent_is_map(stream_id), mstream.parent_is_map());
    EXPECT_EQ(t.parent_is_map(doc_id), mdoc.parent_is_map());
    EXPECT_EQ(t.parent_is_map(map_id), mmap.parent_is_map());
    EXPECT_EQ(t.parent_is_map(keyval_id), mkeyval.parent_is_map());
    EXPECT_EQ(t.parent_is_map(seq_id), mseq.parent_is_map());
    EXPECT_EQ(t.parent_is_map(val_id), mval.parent_is_map());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].parent_is_map(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).parent_is_map(); });
    verify_assertion(t, [&](Tree const&){ return t.parent_is_map(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.parent_is_map(NONE); });
}

TEST(Tree, has_parent)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_FALSE(t.has_parent(stream_id));
    EXPECT_TRUE(t.has_parent(doc_id));
    EXPECT_TRUE(t.has_parent(map_id));
    EXPECT_TRUE(t.has_parent(keyval_id));
    EXPECT_TRUE(t.has_parent(seq_id));
    EXPECT_TRUE(t.has_parent(val_id));
    EXPECT_FALSE(stream.has_parent());
    EXPECT_TRUE(doc.has_parent());
    EXPECT_TRUE(map.has_parent());
    EXPECT_TRUE(keyval.has_parent());
    EXPECT_TRUE(seq.has_parent());
    EXPECT_TRUE(val.has_parent());
    EXPECT_FALSE(mstream.has_parent());
    EXPECT_TRUE(mdoc.has_parent());
    EXPECT_TRUE(mmap.has_parent());
    EXPECT_TRUE(mkeyval.has_parent());
    EXPECT_TRUE(mseq.has_parent());
    EXPECT_TRUE(mval.has_parent());
    EXPECT_EQ(t.has_parent(stream_id), stream.has_parent());
    EXPECT_EQ(t.has_parent(doc_id), doc.has_parent());
    EXPECT_EQ(t.has_parent(map_id), map.has_parent());
    EXPECT_EQ(t.has_parent(keyval_id), keyval.has_parent());
    EXPECT_EQ(t.has_parent(seq_id), seq.has_parent());
    EXPECT_EQ(t.has_parent(val_id), val.has_parent());
    EXPECT_EQ(t.has_parent(stream_id), mstream.has_parent());
    EXPECT_EQ(t.has_parent(doc_id), mdoc.has_parent());
    EXPECT_EQ(t.has_parent(map_id), mmap.has_parent());
    EXPECT_EQ(t.has_parent(keyval_id), mkeyval.has_parent());
    EXPECT_EQ(t.has_parent(seq_id), mseq.has_parent());
    EXPECT_EQ(t.has_parent(val_id), mval.has_parent());
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].has_parent(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).has_parent(); });
    verify_assertion(t, [&](Tree const&){ return t.has_parent(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.has_parent(NONE); });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(Tree, num_children)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_EQ(t.num_children(stream_id), 1u);
    EXPECT_EQ(t.num_children(doc_id), 2u);
    EXPECT_EQ(t.num_children(map_id), 2u);
    EXPECT_EQ(t.num_children(keyval_id), 0u);
    EXPECT_EQ(t.num_children(seq_id), 2u);
    EXPECT_EQ(t.num_children(val_id), 0u);
    EXPECT_EQ(stream.num_children(), t.num_children(stream_id));
    EXPECT_EQ(doc.num_children(), t.num_children(doc_id));
    EXPECT_EQ(map.num_children(), t.num_children(map_id));
    EXPECT_EQ(keyval.num_children(), t.num_children(keyval_id));
    EXPECT_EQ(seq.num_children(), t.num_children(seq_id));
    EXPECT_EQ(val.num_children(), t.num_children(val_id));
    EXPECT_EQ(mstream.num_children(), t.num_children(stream_id));
    EXPECT_EQ(mdoc.num_children(), t.num_children(doc_id));
    EXPECT_EQ(mmap.num_children(), t.num_children(map_id));
    EXPECT_EQ(mkeyval.num_children(), t.num_children(keyval_id));
    EXPECT_EQ(mseq.num_children(), t.num_children(seq_id));
    EXPECT_EQ(mval.num_children(), t.num_children(val_id));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].num_children(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).num_children(); });
    verify_assertion(t, [&](Tree const&){ return t.num_children(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.num_children(NONE); });
}

TEST(Tree, child)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    const id_type val_id = t.first_child(seq_id);
    ConstNodeRef stream = t.ref(stream_id);
    ConstNodeRef doc = t.ref(doc_id);
    ConstNodeRef map = t.ref(map_id);
    ConstNodeRef keyval = t.ref(keyval_id);
    ConstNodeRef seq = t.ref(seq_id);
    ConstNodeRef val = t.ref(val_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mkeyval = t.ref(keyval_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mval = t.ref(val_id);
    EXPECT_EQ(t.child(stream_id, 0), doc_id);
    EXPECT_EQ(t.child(doc_id, 0), map_id);
    EXPECT_EQ(t.child(map_id, 0), keyval_id);
    EXPECT_EQ(t.child(keyval_id, 0), (id_type)NONE);
    EXPECT_EQ(t.child(seq_id, 0), val_id);
    EXPECT_EQ(t.child(val_id, 0), (id_type)NONE);
    EXPECT_EQ(stream.child(0).id(), t.child(stream_id, 0));
    EXPECT_EQ(doc.child(0).id(), t.child(doc_id, 0));
    EXPECT_EQ(map.child(0).id(), t.child(map_id, 0));
    EXPECT_EQ(keyval.child(0).id(), t.child(keyval_id, 0));
    EXPECT_EQ(seq.child(0).id(), t.child(seq_id, 0));
    EXPECT_EQ(val.child(0).id(), t.child(val_id, 0));
    EXPECT_EQ(mstream.child(0).id(), t.child(stream_id, 0));
    EXPECT_EQ(mdoc.child(0).id(), t.child(doc_id, 0));
    EXPECT_EQ(mmap.child(0).id(), t.child(map_id, 0));
    EXPECT_EQ(mkeyval.child(0).id(), t.child(keyval_id, 0));
    EXPECT_EQ(mseq.child(0).id(), t.child(seq_id, 0));
    EXPECT_EQ(mval.child(0).id(), t.child(val_id, 0));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].child(0); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).child(0); });
    verify_assertion(t, [&](Tree const&){ return t.child(t.capacity(), 0); });
    verify_assertion(t, [&](Tree const&){ return t.child(NONE, 0); });
}

TEST(Tree, find_child_by_name)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type keyval_id = t.first_child(map_id);
    const id_type seq_id = t.last_child(doc_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    EXPECT_EQ(t.find_child(doc_id, "map"), map_id);
    EXPECT_EQ(t.find_child(doc_id, "seq"), seq_id);
    EXPECT_EQ(t.find_child(doc_id, "..."), (id_type)NONE);
    EXPECT_EQ(t.find_child(map_id, "foo"), keyval_id);
    EXPECT_EQ(t.find_child(map_id, "bar"), (id_type)NONE);
    EXPECT_EQ(doc.find_child("map").id(), t.find_child(doc_id, "map"));
    EXPECT_EQ(doc.find_child("seq").id(), t.find_child(doc_id, "seq"));
    EXPECT_EQ(doc.find_child("...").id(), t.find_child(doc_id, "..."));
    EXPECT_EQ(map.find_child("foo").id(), t.find_child(map_id, "foo"));
    EXPECT_EQ(map.find_child("bar").id(), t.find_child(map_id, "bar"));
    EXPECT_EQ(mdoc.find_child("map").id(), t.find_child(doc_id, "map"));
    EXPECT_EQ(mdoc.find_child("seq").id(), t.find_child(doc_id, "seq"));
    EXPECT_EQ(mdoc.find_child("...").id(), t.find_child(doc_id, "..."));
    EXPECT_EQ(mmap.find_child("foo").id(), t.find_child(map_id, "foo"));
    EXPECT_EQ(mmap.find_child("bar").id(), t.find_child(map_id, "bar"));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].find_child("foo"); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).find_child("foo"); });
    verify_assertion(t, [&](Tree const&){ return t.find_child(t.capacity(), "foo"); });
    verify_assertion(t, [&](Tree const&){ return t.find_child(NONE, "foo"); });
}

TEST(Tree, find_sibling_by_name)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const id_type stream_id = t.root_id();
    const id_type doc_id = t.first_child(stream_id);
    const id_type map_id = t.first_child(doc_id);
    const id_type seq_id = t.last_child(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef seq = t.cref(seq_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mseq = t.ref(seq_id);
    //
    EXPECT_EQ(t.find_sibling(map_id, "map"), map_id);
    EXPECT_EQ(t.find_sibling(map_id, "seq"), seq_id);
    EXPECT_EQ(t.find_sibling(map_id, "..."), (id_type)NONE);
    EXPECT_EQ(t.find_sibling(seq_id, "map"), map_id);
    EXPECT_EQ(t.find_sibling(seq_id, "seq"), seq_id);
    EXPECT_EQ(t.find_sibling(seq_id, "..."), (id_type)NONE);
    //
    EXPECT_TRUE(t.has_sibling(map_id, "map"));
    EXPECT_TRUE(t.has_sibling(map_id, "seq"));
    EXPECT_FALSE(t.has_sibling(map_id, "..."));
    EXPECT_TRUE(t.has_sibling(seq_id, "map"));
    EXPECT_TRUE(t.has_sibling(seq_id, "seq"));
    EXPECT_FALSE(t.has_sibling(seq_id, "..."));
    //
    EXPECT_EQ(t.find_sibling(map_id, "map"), t.find_child(doc_id, "map"));
    EXPECT_EQ(t.find_sibling(map_id, "seq"), t.find_child(doc_id, "seq"));
    EXPECT_EQ(t.find_sibling(map_id, "..."), t.find_child(doc_id, "..."));
    EXPECT_EQ(t.find_sibling(seq_id, "map"), t.find_child(doc_id, "map"));
    EXPECT_EQ(t.find_sibling(seq_id, "seq"), t.find_child(doc_id, "seq"));
    EXPECT_EQ(t.find_sibling(seq_id, "..."), t.find_child(doc_id, "..."));
    //
    EXPECT_TRUE(t.has_sibling(map_id, "map"));
    EXPECT_TRUE(t.has_sibling(map_id, "seq"));
    EXPECT_FALSE(t.has_sibling(map_id, "..."));
    EXPECT_TRUE(t.has_sibling(seq_id, "map"));
    EXPECT_TRUE(t.has_sibling(seq_id, "seq"));
    EXPECT_FALSE(t.has_sibling(seq_id, "..."));
    //
    EXPECT_EQ(map.find_sibling("map").id(), t.find_sibling(map_id, "map"));
    EXPECT_EQ(map.find_sibling("seq").id(), t.find_sibling(map_id, "seq"));
    EXPECT_EQ(map.find_sibling("...").id(), t.find_sibling(map_id, "..."));
    EXPECT_EQ(seq.find_sibling("map").id(), t.find_sibling(seq_id, "map"));
    EXPECT_EQ(seq.find_sibling("seq").id(), t.find_sibling(seq_id, "seq"));
    EXPECT_EQ(seq.find_sibling("...").id(), t.find_sibling(seq_id, "..."));
    //
    EXPECT_TRUE(map.has_sibling("map"));
    EXPECT_TRUE(map.has_sibling("seq"));
    EXPECT_FALSE(map.has_sibling("..."));
    EXPECT_TRUE(seq.has_sibling("map"));
    EXPECT_TRUE(seq.has_sibling("seq"));
    EXPECT_FALSE(seq.has_sibling("..."));
    //
    EXPECT_EQ(mmap.find_sibling("map").id(), t.find_sibling(map_id, "map"));
    EXPECT_EQ(mmap.find_sibling("seq").id(), t.find_sibling(map_id, "seq"));
    EXPECT_EQ(mmap.find_sibling("...").id(), t.find_sibling(map_id, "..."));
    EXPECT_EQ(mseq.find_sibling("map").id(), t.find_sibling(seq_id, "map"));
    EXPECT_EQ(mseq.find_sibling("seq").id(), t.find_sibling(seq_id, "seq"));
    EXPECT_EQ(mseq.find_sibling("...").id(), t.find_sibling(seq_id, "..."));
    //
    EXPECT_TRUE(mmap.has_sibling("map"));
    EXPECT_TRUE(mmap.has_sibling("seq"));
    EXPECT_FALSE(mmap.has_sibling("..."));
    EXPECT_TRUE(mseq.has_sibling("map"));
    EXPECT_TRUE(mseq.has_sibling("seq"));
    EXPECT_FALSE(mseq.has_sibling("..."));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].find_sibling("foo"); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).find_sibling("foo"); });
    verify_assertion(t, [&](Tree const&){ return t.find_sibling(t.capacity(), "foo"); });
    verify_assertion(t, [&](Tree const&){ return t.find_sibling(NONE, "foo"); });
}

TEST(Tree, depth_asc_desc)
{
    Tree t = parse_in_arena(R"(---
map: {foo: *keyvalref, notag: none}
seq: &seq [*valref, bar]
...)");
    const size_t stream_id = t.root_id();
    const size_t doc_id = t.first_child(stream_id);
    const size_t map_id = t.first_child(doc_id);
    const size_t seq_id = t.last_child(doc_id);
    const size_t map_child_id = t.first_child(map_id);
    const size_t seq_child_id = t.first_child(seq_id);
    ConstNodeRef stream = t.cref(stream_id);
    ConstNodeRef doc = t.cref(doc_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef seq = t.cref(seq_id);
    ConstNodeRef map_child = t.cref(map_child_id);
    ConstNodeRef seq_child = t.cref(seq_child_id);
    NodeRef mstream = t.ref(stream_id);
    NodeRef mdoc = t.ref(doc_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef mseq = t.ref(seq_id);
    NodeRef mmap_child = t.ref(map_child_id);
    NodeRef mseq_child = t.ref(seq_child_id);
    //
    EXPECT_EQ(t.depth_asc(stream_id), id_type(0));
    EXPECT_EQ(t.depth_asc(doc_id), id_type(1));
    EXPECT_EQ(t.depth_asc(map_id), id_type(2));
    EXPECT_EQ(t.depth_asc(seq_id), id_type(2));
    EXPECT_EQ(t.depth_asc(map_child_id), id_type(3));
    EXPECT_EQ(t.depth_asc(seq_child_id), id_type(3));
    //
    EXPECT_EQ(stream.depth_asc(), id_type(0));
    EXPECT_EQ(doc.depth_asc(), id_type(1));
    EXPECT_EQ(map.depth_asc(), id_type(2));
    EXPECT_EQ(seq.depth_asc(), id_type(2));
    EXPECT_EQ(map_child.depth_asc(), id_type(3));
    EXPECT_EQ(seq_child.depth_asc(), id_type(3));
    //
    EXPECT_EQ(mstream.depth_asc(), id_type(0));
    EXPECT_EQ(mdoc.depth_asc(), id_type(1));
    EXPECT_EQ(mmap.depth_asc(), id_type(2));
    EXPECT_EQ(mseq.depth_asc(), id_type(2));
    EXPECT_EQ(mmap_child.depth_asc(), id_type(3));
    EXPECT_EQ(mseq_child.depth_asc(), id_type(3));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].depth_asc(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).depth_asc(); });
    verify_assertion(t, [&](Tree const&){ return t.depth_asc(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.depth_asc(NONE); });
    //
    EXPECT_EQ(t.depth_desc(stream_id), id_type(3));
    EXPECT_EQ(t.depth_desc(doc_id), id_type(2));
    EXPECT_EQ(t.depth_desc(map_id), id_type(1));
    EXPECT_EQ(t.depth_desc(seq_id), id_type(1));
    EXPECT_EQ(t.depth_desc(map_child_id), id_type(0));
    EXPECT_EQ(t.depth_desc(seq_child_id), id_type(0));
    //
    EXPECT_EQ(stream.depth_desc(), id_type(3));
    EXPECT_EQ(doc.depth_desc(), id_type(2));
    EXPECT_EQ(map.depth_desc(), id_type(1));
    EXPECT_EQ(seq.depth_desc(), id_type(1));
    EXPECT_EQ(map_child.depth_desc(), id_type(0));
    EXPECT_EQ(seq_child.depth_desc(), id_type(0));
    //
    EXPECT_EQ(mstream.depth_desc(), id_type(3));
    EXPECT_EQ(mdoc.depth_desc(), id_type(2));
    EXPECT_EQ(mmap.depth_desc(), id_type(1));
    EXPECT_EQ(mseq.depth_desc(), id_type(1));
    EXPECT_EQ(mmap_child.depth_desc(), id_type(0));
    EXPECT_EQ(mseq_child.depth_desc(), id_type(0));
    //
    verify_assertion(t, [&](Tree const&){ return t.docref(0)["none"].depth_desc(); });
    verify_assertion(t, [&](Tree const&){ return t.docref(2).depth_desc(); });
    verify_assertion(t, [&](Tree const&){ return t.depth_desc(t.capacity()); });
    verify_assertion(t, [&](Tree const&){ return t.depth_desc(NONE); });
}

TEST(Tree, is_ancestor)
{
    Tree t = parse_in_arena(R"(a0:
  a1:
    a2:
      a3: a
b0:
  b1:
    b2:
      b3: b
)");
    const size_t map_id = t.root_id();
    const size_t a0_id = t.first_child(map_id);
    const size_t a1_id = t.first_child(a0_id);
    const size_t a2_id = t.first_child(a1_id);
    const size_t a3_id = t.first_child(a2_id);
    const size_t b0_id = t.first_child(map_id);
    const size_t b1_id = t.first_child(b0_id);
    const size_t b2_id = t.first_child(b1_id);
    const size_t b3_id = t.first_child(b2_id);
    ConstNodeRef map = t.cref(map_id);
    ConstNodeRef a0 = t.cref(a0_id);
    ConstNodeRef a1 = t.cref(a1_id);
    ConstNodeRef a2 = t.cref(a2_id);
    ConstNodeRef a3 = t.cref(a3_id);
    ConstNodeRef b0 = t.cref(b0_id);
    ConstNodeRef b1 = t.cref(b1_id);
    ConstNodeRef b2 = t.cref(b2_id);
    ConstNodeRef b3 = t.cref(b3_id);
    NodeRef mmap = t.ref(map_id);
    NodeRef ma0 = t.ref(a0_id);
    NodeRef ma1 = t.ref(a1_id);
    NodeRef ma2 = t.ref(a2_id);
    NodeRef ma3 = t.ref(a3_id);
    NodeRef mb0 = t.ref(b0_id);
    NodeRef mb1 = t.ref(b1_id);
    NodeRef mb2 = t.ref(b2_id);
    NodeRef mb3 = t.ref(b3_id);
    //
    EXPECT_TRUE(t.is_ancestor(a0_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a1_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, map_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, map_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a1_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, a0_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a1_id));
    EXPECT_TRUE(t.is_ancestor(a2_id, a1_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a2_id));
    EXPECT_TRUE(t.is_ancestor(a3_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a3_id, a3_id));
    EXPECT_TRUE(t.is_ancestor(b0_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b1_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, map_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, map_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b1_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, b0_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b1_id));
    EXPECT_TRUE(t.is_ancestor(b2_id, b1_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b2_id));
    EXPECT_TRUE(t.is_ancestor(b3_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b3_id, b3_id));
    //
    EXPECT_FALSE(t.is_ancestor(map_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(map_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a0_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a0_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a1_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a1_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a2_id));
    EXPECT_FALSE(t.is_ancestor(a2_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(a3_id, a3_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(map_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b0_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b0_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b1_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b1_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b2_id));
    EXPECT_FALSE(t.is_ancestor(b2_id, b3_id));
    EXPECT_FALSE(t.is_ancestor(b3_id, b3_id));
    //
    verify_assertion(t, [&](Tree const&){ return t.is_ancestor(NONE, map_id); });
    //
    EXPECT_TRUE(a0.is_ancestor(map));
    EXPECT_TRUE(a1.is_ancestor(map));
    EXPECT_TRUE(a2.is_ancestor(map));
    EXPECT_TRUE(a3.is_ancestor(map));
    EXPECT_FALSE(a0.is_ancestor(a0));
    EXPECT_TRUE(a1.is_ancestor(a0));
    EXPECT_TRUE(a2.is_ancestor(a0));
    EXPECT_TRUE(a3.is_ancestor(a0));
    EXPECT_FALSE(a1.is_ancestor(a1));
    EXPECT_TRUE(a2.is_ancestor(a1));
    EXPECT_TRUE(a3.is_ancestor(a1));
    EXPECT_FALSE(a2.is_ancestor(a2));
    EXPECT_TRUE(a3.is_ancestor(a2));
    EXPECT_FALSE(a3.is_ancestor(a3));
    EXPECT_TRUE(b0.is_ancestor(map));
    EXPECT_TRUE(b1.is_ancestor(map));
    EXPECT_TRUE(b2.is_ancestor(map));
    EXPECT_TRUE(b3.is_ancestor(map));
    EXPECT_FALSE(b0.is_ancestor(b0));
    EXPECT_TRUE(b1.is_ancestor(b0));
    EXPECT_TRUE(b2.is_ancestor(b0));
    EXPECT_TRUE(b3.is_ancestor(b0));
    EXPECT_FALSE(b1.is_ancestor(b1));
    EXPECT_TRUE(b2.is_ancestor(b1));
    EXPECT_TRUE(b3.is_ancestor(b1));
    EXPECT_FALSE(b2.is_ancestor(b2));
    EXPECT_TRUE(b3.is_ancestor(b2));
    EXPECT_FALSE(b3.is_ancestor(b3));
    //
    EXPECT_FALSE(map.is_ancestor(a0));
    EXPECT_FALSE(map.is_ancestor(a1));
    EXPECT_FALSE(map.is_ancestor(a2));
    EXPECT_FALSE(map.is_ancestor(a3));
    EXPECT_FALSE(a0.is_ancestor(a0));
    EXPECT_FALSE(a0.is_ancestor(a1));
    EXPECT_FALSE(a0.is_ancestor(a2));
    EXPECT_FALSE(a0.is_ancestor(a3));
    EXPECT_FALSE(a1.is_ancestor(a1));
    EXPECT_FALSE(a1.is_ancestor(a2));
    EXPECT_FALSE(a1.is_ancestor(a3));
    EXPECT_FALSE(a2.is_ancestor(a2));
    EXPECT_FALSE(a2.is_ancestor(a3));
    EXPECT_FALSE(a3.is_ancestor(a3));
    EXPECT_FALSE(map.is_ancestor(b0));
    EXPECT_FALSE(map.is_ancestor(b1));
    EXPECT_FALSE(map.is_ancestor(b2));
    EXPECT_FALSE(map.is_ancestor(b3));
    EXPECT_FALSE(b0.is_ancestor(b0));
    EXPECT_FALSE(b0.is_ancestor(b1));
    EXPECT_FALSE(b0.is_ancestor(b2));
    EXPECT_FALSE(b0.is_ancestor(b3));
    EXPECT_FALSE(b1.is_ancestor(b1));
    EXPECT_FALSE(b1.is_ancestor(b2));
    EXPECT_FALSE(b1.is_ancestor(b3));
    EXPECT_FALSE(b2.is_ancestor(b2));
    EXPECT_FALSE(b2.is_ancestor(b3));
    EXPECT_FALSE(b3.is_ancestor(b3));
    //
    EXPECT_TRUE(ma0.is_ancestor(mmap));
    EXPECT_TRUE(ma1.is_ancestor(mmap));
    EXPECT_TRUE(ma2.is_ancestor(mmap));
    EXPECT_TRUE(ma3.is_ancestor(mmap));
    EXPECT_FALSE(ma0.is_ancestor(ma0));
    EXPECT_TRUE(ma1.is_ancestor(ma0));
    EXPECT_TRUE(ma2.is_ancestor(ma0));
    EXPECT_TRUE(ma3.is_ancestor(ma0));
    EXPECT_FALSE(ma1.is_ancestor(ma1));
    EXPECT_TRUE(ma2.is_ancestor(ma1));
    EXPECT_TRUE(ma3.is_ancestor(ma1));
    EXPECT_FALSE(ma2.is_ancestor(ma2));
    EXPECT_TRUE(ma3.is_ancestor(ma2));
    EXPECT_FALSE(ma3.is_ancestor(ma3));
    EXPECT_TRUE(mb0.is_ancestor(mmap));
    EXPECT_TRUE(mb1.is_ancestor(mmap));
    EXPECT_TRUE(mb2.is_ancestor(mmap));
    EXPECT_TRUE(mb3.is_ancestor(mmap));
    EXPECT_FALSE(mb0.is_ancestor(mb0));
    EXPECT_TRUE(mb1.is_ancestor(mb0));
    EXPECT_TRUE(mb2.is_ancestor(mb0));
    EXPECT_TRUE(mb3.is_ancestor(mb0));
    EXPECT_FALSE(mb1.is_ancestor(mb1));
    EXPECT_TRUE(mb2.is_ancestor(mb1));
    EXPECT_TRUE(mb3.is_ancestor(mb1));
    EXPECT_FALSE(mb2.is_ancestor(mb2));
    EXPECT_TRUE(mb3.is_ancestor(mb2));
    EXPECT_FALSE(mb3.is_ancestor(mb3));
    //
    EXPECT_FALSE(mmap.is_ancestor(ma0));
    EXPECT_FALSE(mmap.is_ancestor(ma1));
    EXPECT_FALSE(mmap.is_ancestor(ma2));
    EXPECT_FALSE(mmap.is_ancestor(ma3));
    EXPECT_FALSE(ma0.is_ancestor(ma0));
    EXPECT_FALSE(ma0.is_ancestor(ma1));
    EXPECT_FALSE(ma0.is_ancestor(ma2));
    EXPECT_FALSE(ma0.is_ancestor(ma3));
    EXPECT_FALSE(ma1.is_ancestor(ma1));
    EXPECT_FALSE(ma1.is_ancestor(ma2));
    EXPECT_FALSE(ma1.is_ancestor(ma3));
    EXPECT_FALSE(ma2.is_ancestor(ma2));
    EXPECT_FALSE(ma2.is_ancestor(ma3));
    EXPECT_FALSE(ma3.is_ancestor(ma3));
    EXPECT_FALSE(mmap.is_ancestor(mb0));
    EXPECT_FALSE(mmap.is_ancestor(mb1));
    EXPECT_FALSE(mmap.is_ancestor(mb2));
    EXPECT_FALSE(mmap.is_ancestor(mb3));
    EXPECT_FALSE(mb0.is_ancestor(mb0));
    EXPECT_FALSE(mb0.is_ancestor(mb1));
    EXPECT_FALSE(mb0.is_ancestor(mb2));
    EXPECT_FALSE(mb0.is_ancestor(mb3));
    EXPECT_FALSE(mb1.is_ancestor(mb1));
    EXPECT_FALSE(mb1.is_ancestor(mb2));
    EXPECT_FALSE(mb1.is_ancestor(mb3));
    EXPECT_FALSE(mb2.is_ancestor(mb2));
    EXPECT_FALSE(mb2.is_ancestor(mb3));
    EXPECT_FALSE(mb3.is_ancestor(mb3));
    //
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(change_type, from_val)
{
    Tree t = parse_in_arena("[val0, val1, val2]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    Tree expected = parse_in_arena("[val0, {}, []]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keyval)
{
    Tree t = parse_in_arena("{keyval0: val0, keyval1: val1, keyval2: val2}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    Tree expected = parse_in_arena("{keyval0: val0, keyval1: {}, keyval2: []}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}

TEST(change_type, from_map)
{
    Tree t = parse_in_arena("[{map0: val0}, {map1: {map1key0: a, map1key1: b}}, {map2: [map2val0, map2val1]}]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("['', {map1: {map1key0: a, map1key1: b}}, []]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keymap)
{
    Tree t = parse_in_arena("{map0: {map0: val0}, map1: {map1: {map1key0: a, map1key1: b}}, map2: {map2: [map2val0, map2val1]}}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("{map0: '', map1: {map1: {map1key0: a, map1key1: b}}, map2: []}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}

TEST(change_type, from_seq)
{
    Tree t = parse_in_arena("[[seq00, seq01], [seq10, seq11], [seq20, seq21]]");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("['', {}, [seq20, seq21]]");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}
TEST(change_type, from_keyseq)
{
    Tree t = parse_in_arena("{map0: [seq00, seq01], map1: [seq10, seq11], map2: [seq20, seq21]}");
    t[0].change_type(VAL);
    t[1].change_type(MAP);
    t[2].change_type(SEQ);
    verify_assertion(t, [&](Tree const&){ return t[3].change_type(VAL); });
    EXPECT_FALSE(t[0].val_is_null());
    EXPECT_NE(t[0].val(), nullptr);
    Tree expected = parse_in_arena("{map0: '', map1: {}, map2: [seq20, seq21]}");
    EXPECT_EQ(emitrs_yaml<std::string>(t), emitrs_yaml<std::string>(expected));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Tree, lookup_path)
{
    const char yaml[] = R"(
a:
  b: bval
  c:
    d:
      - e
      - d
      - f: fval
        g: gval
        h:
          -
            x: a
            y: b
          -
            z: c
            u:
)";
    Tree t = parse_in_arena(yaml);
    print_tree(t);

    EXPECT_EQ(t.lookup_path("a").target, 1);
    EXPECT_EQ(t.lookup_path("a.b").target, 2);
    EXPECT_EQ(t.lookup_path("a.c").target, 3);
    EXPECT_EQ(t.lookup_path("a.c.d").target, 4);
    EXPECT_EQ(t.lookup_path("a.c.d[0]").target, 5);
    EXPECT_EQ(t.lookup_path("a.c.d[1]").target, 6);
    EXPECT_EQ(t.lookup_path("a.c.d[2]").target, 7);
    EXPECT_EQ(t.lookup_path("a.c.d[2].f").target, 8);
    EXPECT_EQ(t.lookup_path("a.c.d[2].g").target, 9);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h").target, 10);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0]").target, 11);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0].x").target, 12);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[0].y").target, 13);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1]").target, 14);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1].z").target, 15);
    EXPECT_EQ(t.lookup_path("a.c.d[2].h[1].u").target, 16);
    EXPECT_EQ(t.lookup_path("d", 3).target, 4);
    EXPECT_EQ(t.lookup_path("d[0]", 3).target, 5);
    EXPECT_EQ(t.lookup_path("d[1]", 3).target, 6);
    EXPECT_EQ(t.lookup_path("d[2]", 3).target, 7);
    EXPECT_EQ(t.lookup_path("d[2].f", 3).target, 8);
    EXPECT_EQ(t.lookup_path("d[2].g", 3).target, 9);
    EXPECT_EQ(t.lookup_path("d[2].h", 3).target, 10);
    EXPECT_EQ(t.lookup_path("d[2].h[0]", 3).target, 11);
    EXPECT_EQ(t.lookup_path("d[2].h[0].x", 3).target, 12);
    EXPECT_EQ(t.lookup_path("d[2].h[0].y", 3).target, 13);
    EXPECT_EQ(t.lookup_path("d[2].h[1]", 3).target, 14);
    EXPECT_EQ(t.lookup_path("d[2].h[1].z", 3).target, 15);
    EXPECT_EQ(t.lookup_path("d[2].h[1].u", 3).target, 16);

    auto lp = t.lookup_path("x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, (id_type)NONE);
    EXPECT_EQ(lp.resolved(), "");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 1);
    EXPECT_EQ(lp.resolved(), "a");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.b.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 2);
    EXPECT_EQ(lp.resolved(), "a.b");
    EXPECT_EQ(lp.unresolved(), "x");
    lp = t.lookup_path("a.c.x");
    EXPECT_FALSE(lp);
    EXPECT_EQ(lp.target, (id_type)NONE);
    EXPECT_EQ(lp.closest, 3);
    EXPECT_EQ(lp.resolved(), "a.c");
    EXPECT_EQ(lp.unresolved(), "x");

    id_type sz = t.size();
    EXPECT_EQ(t.lookup_path("x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "x"), sz);
    EXPECT_EQ(t.lookup_path("x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "x"), sz);
    EXPECT_EQ(t.val(sz), "z");

    sz = t.size();
    EXPECT_EQ(t.lookup_path("a.x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "a.x"), sz);
    EXPECT_EQ(t.lookup_path("a.x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "a.x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "a.x"), sz);
    EXPECT_EQ(t.val(sz), "z");

    sz = t.size();
    EXPECT_EQ(t.lookup_path("a.c.x").target, (id_type)NONE);
    EXPECT_EQ(t.lookup_path_or_modify("x", "a.c.x"), sz);
    EXPECT_EQ(t.lookup_path("a.c.x").target, sz);
    EXPECT_EQ(t.val(sz), "x");
    EXPECT_EQ(t.lookup_path_or_modify("y", "a.c.x"), sz);
    EXPECT_EQ(t.val(sz), "y");
    EXPECT_EQ(t.lookup_path_or_modify("z", "a.c.x"), sz);
    EXPECT_EQ(t.val(sz), "z");
}

TEST(Tree, lookup_path_or_modify)
{
    {
        Tree dst = parse_in_arena("{}");
        Tree const src = parse_in_arena("{d: [x, y, z]}");
        dst.lookup_path_or_modify("ok", "a.b.c");
        EXPECT_EQ(dst["a"]["b"]["c"].val(), "ok");
        dst.lookup_path_or_modify(&src, src["d"].id(), "a.b.d");
        EXPECT_EQ(dst["a"]["b"]["d"][0].val(), "x");
        EXPECT_EQ(dst["a"]["b"]["d"][1].val(), "y");
        EXPECT_EQ(dst["a"]["b"]["d"][2].val(), "z");
    }

    {
        Tree t;
        t.rootref() |= MAP;
        csubstr bigpath = "newmap.newseq[0].newmap.newseq[0].first";
        auto result = t.lookup_path(bigpath);
        EXPECT_EQ(result.target, (id_type)NONE);
        EXPECT_EQ(result.closest, (id_type)NONE);
        EXPECT_EQ(result.resolved(), "");
        EXPECT_EQ(result.unresolved(), bigpath);
        id_type sz = t.lookup_path_or_modify("x", bigpath);
        EXPECT_EQ(t.lookup_path(bigpath).target, sz);
        EXPECT_EQ(t.val(sz), "x");
        EXPECT_EQ(t["newmap"]["newseq"].num_children(), 1u);
        EXPECT_EQ(t["newmap"]["newseq"][0].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"].num_children(), 1u);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0]["first"].val(), "x");
        id_type sz2 = t.lookup_path_or_modify("y", bigpath);
        EXPECT_EQ(t["newmap"]["newseq"][0]["newmap"]["newseq"][0]["first"].val(), "y");
        EXPECT_EQ(sz2, sz);
        EXPECT_EQ(t.lookup_path(bigpath).target, sz);
        EXPECT_EQ(t.val(sz2), "y");

        sz2 = t.lookup_path_or_modify("y", "newmap2.newseq2[2].newmap2.newseq2[3].first2");
        EXPECT_EQ(t.lookup_path("newmap2.newseq2[2].newmap2.newseq2[3].first2").target, sz2);
        EXPECT_EQ(t.val(sz2), "y");
        EXPECT_EQ(t["newmap2"]["newseq2"].num_children(), 3u);
        EXPECT_EQ(t["newmap2"]["newseq2"][0].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][1].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"].is_seq(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"].num_children(), 4u);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][0].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][1].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][2].val(), nullptr);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3].is_map(), true);
        EXPECT_EQ(t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3]["first2"].val(), "y");
        (void)t.lookup_path_or_modify("z", "newmap2.newseq2[2].newmap2.newseq2[3].second2");
        EXPECT_EQ  (t["newmap2"]["newseq2"][2]["newmap2"]["newseq2"][3]["second2"].val(), "z");

        (void)t.lookup_path_or_modify("foo", "newmap.newseq1[1]");
        EXPECT_EQ(t["newmap"].is_map(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 2u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        (void)t.lookup_path_or_modify("bar", "newmap.newseq1[2][1]");
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        EXPECT_EQ(t["newmap"]["newseq1"][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2].num_children(), 2u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][0].val(), nullptr);
        EXPECT_EQ(t["newmap"]["newseq1"][2][1].val(), "bar");
        (void)t.lookup_path_or_modify("Foo?"   , "newmap.newseq1[0]");
        (void)t.lookup_path_or_modify("Bar?"   , "newmap.newseq1[2][0]");
        (void)t.lookup_path_or_modify("happy"  , "newmap.newseq1[2][2][3]");
        (void)t.lookup_path_or_modify("trigger", "newmap.newseq1[2][2][2]");
        (void)t.lookup_path_or_modify("Arnold" , "newmap.newseq1[2][2][0]");
        (void)t.lookup_path_or_modify("is"     , "newmap.newseq1[2][2][1]");
        EXPECT_EQ(t["newmap"]["newseq1"].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][0].val(), "Foo?");
        EXPECT_EQ(t["newmap"]["newseq1"][1].val(), "foo");
        EXPECT_EQ(t["newmap"]["newseq1"][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2].num_children(), 3u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][0].val(), "Bar?");
        EXPECT_EQ(t["newmap"]["newseq1"][2][1].val(), "bar");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2].is_seq(), true);
        EXPECT_EQ(t["newmap"]["newseq1"][2][2].num_children(), 4u);
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][0].val(), "Arnold");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][1].val(), "is");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][2].val(), "trigger");
        EXPECT_EQ(t["newmap"]["newseq1"][2][2][3].val(), "happy");

        EXPECT_EQ(emitrs_yaml<std::string>(t), R"(newmap:
  newseq:
    - newmap:
        newseq:
          - first: y
  newseq1:
    - Foo?
    - foo
    - - Bar?
      - bar
      - - Arnold
        - is
        - trigger
        - happy
newmap2:
  newseq2:
    - 
    - 
    - newmap2:
        newseq2:
          - 
          - 
          - 
          - first2: y
            second2: z
)");
    }
}



//-----------------------------------------------------------------------------

TEST(set_root_as_stream, empty_tree)
{
    Tree t;
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.num_children(), 0u);
    t.set_root_as_stream();
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.num_children(), 0u);
}

TEST(set_root_as_stream, already_with_stream)
{
    Tree t;
    t.to_stream(t.root_id());
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.num_children(), 0u);
    t.set_root_as_stream();
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.num_children(), 0u);
}


TEST(set_root_as_stream, root_is_map)
{
    Tree t = parse_in_arena(R"({a: b, c: d})");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 2u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 2u);
    EXPECT_EQ(r[0]["a"].is_keyval(), true);
    EXPECT_EQ(r[0]["c"].is_keyval(), true);
    EXPECT_EQ(r[0]["a"].val(), "b");
    EXPECT_EQ(r[0]["c"].val(), "d");
}

TEST(set_root_as_stream, root_is_docmap)
{
    Tree t = parse_in_arena(R"({a: b, c: d})");
    t._p(t.root_id())->m_type.add(DOC);
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 2u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 2u);
    EXPECT_EQ(r[0]["a"].is_keyval(), true);
    EXPECT_EQ(r[0]["c"].is_keyval(), true);
    EXPECT_EQ(r[0]["a"].val(), "b");
    EXPECT_EQ(r[0]["c"].val(), "d");
}


TEST(set_root_as_stream, root_is_seq)
{
    Tree t = parse_in_arena(R"([a, b, c, d])");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].val(), "a");
    EXPECT_EQ(r[0][1].val(), "b");
    EXPECT_EQ(r[0][2].val(), "c");
    EXPECT_EQ(r[0][3].val(), "d");
}

TEST(set_root_as_stream, root_is_docseq)
{
    Tree t = parse_in_arena(R"([a, b, c, d])");
    t._p(t.root_id())->m_type.add(DOC);
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].val(), "a");
    EXPECT_EQ(r[0][1].val(), "b");
    EXPECT_EQ(r[0][2].val(), "c");
    EXPECT_EQ(r[0][3].val(), "d");
}

TEST(set_root_as_stream, root_is_seqmap)
{
    Tree t = parse_in_arena(R"([{a: b, c: d}, {e: e, f: f}, {g: g, h: h}, {i: i, j: j}])");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 4u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), true);
    EXPECT_EQ(r[0].num_children(), 4u);
    EXPECT_EQ(r[0][0].is_map(), true);
    EXPECT_EQ(r[0][1].is_map(), true);
    EXPECT_EQ(r[0][2].is_map(), true);
    EXPECT_EQ(r[0][3].is_map(), true);
    EXPECT_EQ(r[0][0]["a"].val(), "b");
    EXPECT_EQ(r[0][0]["c"].val(), "d");
    EXPECT_EQ(r[0][1]["e"].val(), "e");
    EXPECT_EQ(r[0][1]["f"].val(), "f");
    EXPECT_EQ(r[0][2]["g"].val(), "g");
    EXPECT_EQ(r[0][2]["h"].val(), "h");
    EXPECT_EQ(r[0][3]["i"].val(), "i");
    EXPECT_EQ(r[0][3]["j"].val(), "j");
}

TEST(set_root_as_stream, root_is_mapseq)
{
    Tree t = parse_in_arena(R"({a: [0, 1, 2], b: [3, 4, 5], c: [6, 7, 8]})");
    NodeRef r = t.rootref();
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), true);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.num_children(), 3u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), true);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].num_children(), 3u);
    EXPECT_EQ(r[0]["a"].is_seq(), true);
    EXPECT_EQ(r[0]["b"].is_seq(), true);
    EXPECT_EQ(r[0]["c"].is_seq(), true);
    EXPECT_EQ(r[0]["a"][0].val(), "0");
    EXPECT_EQ(r[0]["a"][1].val(), "1");
    EXPECT_EQ(r[0]["a"][2].val(), "2");
    EXPECT_EQ(r[0]["b"][0].val(), "3");
    EXPECT_EQ(r[0]["b"][1].val(), "4");
    EXPECT_EQ(r[0]["b"][2].val(), "5");
    EXPECT_EQ(r[0]["c"][0].val(), "6");
    EXPECT_EQ(r[0]["c"][1].val(), "7");
    EXPECT_EQ(r[0]["c"][2].val(), "8");
}

TEST(set_root_as_stream, root_is_docval)
{
    Tree t;
    NodeRef r = t.rootref();
    r.set_type(DOCVAL);
    r.set_val("bar");
    r.set_val_tag("<!foo>");
    EXPECT_EQ(r.is_stream(), false);
    EXPECT_EQ(r.is_doc(), true);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), false);
    EXPECT_EQ(r.is_val(), true);
    EXPECT_EQ(r.has_val_tag(), true);
    EXPECT_EQ(r.val_tag(), "<!foo>");
    EXPECT_EQ(r.num_children(), 0u);
    print_tree(t);
    std::cout << t;
    t.set_root_as_stream();
    print_tree(t);
    std::cout << t;
    EXPECT_EQ(r.is_stream(), true);
    EXPECT_EQ(r.is_doc(), false);
    EXPECT_EQ(r.is_map(), false);
    EXPECT_EQ(r.is_seq(), true);
    EXPECT_EQ(r.is_val(), false);
    ASSERT_EQ(r.num_children(), 1u);
    EXPECT_EQ(r[0].is_stream(), false);
    EXPECT_EQ(r[0].is_doc(), true);
    EXPECT_EQ(r[0].is_map(), false);
    EXPECT_EQ(r[0].is_seq(), false);
    EXPECT_EQ(r[0].is_val(), true);
    EXPECT_EQ(r[0].has_val_tag(), true);
    EXPECT_EQ(r[0].val_tag(), "<!foo>");
    EXPECT_EQ(r[0].num_children(), 0u);
}


//-------------------------------------------
//-------------------------------------------
//-------------------------------------------

TEST(Tree, doc)
{
    Tree t = parse_in_arena(R"(---
doc0
---
doc1
---
doc2
---
doc3
---
doc4
)");
    id_type ir = t.root_id();
    ASSERT_EQ(t.num_children(ir), 5u);
    ASSERT_TRUE(t.is_stream(ir));
    EXPECT_EQ(t.child(ir, 0), t.doc(0));
    EXPECT_EQ(t.child(ir, 1), t.doc(1));
    EXPECT_EQ(t.child(ir, 2), t.doc(2));
    EXPECT_EQ(t.child(ir, 3), t.doc(3));
    EXPECT_EQ(t.child(ir, 4), t.doc(4));
    {
        NodeRef r = t.rootref();
        EXPECT_EQ(r.id(), ir);
        EXPECT_EQ(r.child(0), r.doc(0));
        EXPECT_EQ(r.child(1), r.doc(1));
        EXPECT_EQ(r.child(2), r.doc(2));
        EXPECT_EQ(r.child(3), r.doc(3));
        EXPECT_EQ(r.child(4), r.doc(4));
        EXPECT_EQ(r.child(0).id(), t.doc(0));
        EXPECT_EQ(r.child(1).id(), t.doc(1));
        EXPECT_EQ(r.child(2).id(), t.doc(2));
        EXPECT_EQ(r.child(3).id(), t.doc(3));
        EXPECT_EQ(r.child(4).id(), t.doc(4));
        EXPECT_EQ(r.child(0).id(), t.docref(0).id());
        EXPECT_EQ(r.child(1).id(), t.docref(1).id());
        EXPECT_EQ(r.child(2).id(), t.docref(2).id());
        EXPECT_EQ(r.child(3).id(), t.docref(3).id());
        EXPECT_EQ(r.child(4).id(), t.docref(4).id());
    }
    {
        const Tree &ct = t;
        const ConstNodeRef r = ct.rootref();
        EXPECT_EQ(r.id(), ir);
        EXPECT_EQ(r.child(0), r.doc(0));
        EXPECT_EQ(r.child(1), r.doc(1));
        EXPECT_EQ(r.child(2), r.doc(2));
        EXPECT_EQ(r.child(3), r.doc(3));
        EXPECT_EQ(r.child(4), r.doc(4));
        EXPECT_EQ(r.child(0).id(), t.doc(0));
        EXPECT_EQ(r.child(1).id(), t.doc(1));
        EXPECT_EQ(r.child(2).id(), t.doc(2));
        EXPECT_EQ(r.child(3).id(), t.doc(3));
        EXPECT_EQ(r.child(4).id(), t.doc(4));
        EXPECT_EQ(r.child(0).id(), t.docref(0).id());
        EXPECT_EQ(r.child(1).id(), t.docref(1).id());
        EXPECT_EQ(r.child(2).id(), t.docref(2).id());
        EXPECT_EQ(r.child(3).id(), t.docref(3).id());
        EXPECT_EQ(r.child(4).id(), t.docref(4).id());
    }
}


//-------------------------------------------
//-------------------------------------------
//-------------------------------------------

TEST(Tree, add_tag_directives)
{
    #if RYML_MAX_TAG_DIRECTIVES != 4
    #error this test assumes RYML_MAX_TAG_DIRECTIVES == 4
    #endif
    const TagDirective td[RYML_MAX_TAG_DIRECTIVES + 1] = {
        TagDirective{csubstr("!a!"), csubstr("!ay-"), 0u},
        TagDirective{csubstr("!b!"), csubstr("!by-"), 0u},
        TagDirective{csubstr("!c!"), csubstr("!cy-"), 0u},
        TagDirective{csubstr("!d!"), csubstr("!dy-"), 0u},
        TagDirective{csubstr("!e!"), csubstr("!ey-"), 0u},
    };
    Tree t;
    auto check_up_to = [&](id_type num)
    {
        id_type pos = 0;
        EXPECT_EQ(t.num_tag_directives(), num);
        for(TagDirective const& d : t.tag_directives())
        {
            EXPECT_EQ(d.handle.str, td[pos].handle.str);
            EXPECT_EQ(d.handle.len, td[pos].handle.len);
            EXPECT_EQ(d.prefix.str, td[pos].prefix.str);
            EXPECT_EQ(d.prefix.str, td[pos].prefix.str);
            EXPECT_EQ(d.next_node_id, td[pos].next_node_id);
            ++pos;
        }
        EXPECT_EQ(pos, num);
    };
    check_up_to(0);
    t.add_tag_directive(td[0]);
    check_up_to(1);
    t.add_tag_directive(td[1]);
    check_up_to(2);
    t.add_tag_directive(td[2]);
    check_up_to(3);
    t.add_tag_directive(td[3]);
    check_up_to(4);
    ExpectError::check_error(&t, [&]{ // number exceeded
        t.add_tag_directive(td[4]);
    });
    t.clear_tag_directives();
    check_up_to(0);
}

TEST(Tree, resolve_tag)
{
    csubstr yaml = R"(
#%TAG !m! !my-
--- # Bulb here
!m!light fluorescent
...
#%TAG !m! !meta-
--- # Color here
!m!light green
)";
    // we're not testing the parser here, just the tag mechanics.
    // So we'll add the tag directives by hand.
    Tree t = parse_in_arena(yaml);
    ASSERT_EQ(t.rootref().num_children(), 2);
    EXPECT_EQ(t[0].val_tag(), "!m!light");
    EXPECT_EQ(t[1].val_tag(), "!m!light");
    EXPECT_EQ(t.num_tag_directives(), 0u);
    t.add_tag_directive(TagDirective{csubstr("!m!"), csubstr("!my-"), 1});
    t.add_tag_directive(TagDirective{csubstr("!m!"), csubstr("!meta-"), 2});
    EXPECT_EQ(t.num_tag_directives(), 2u);
    char buf_[100];
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 1u), csubstr("<!my-light>"));
    EXPECT_EQ(t.resolve_tag_sub(buf_, "!m!light", 2u), csubstr("<!meta-light>"));
}


//-----------------------------------------------------------------------------

void test_key_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_key_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_val_quoted(n.id()));
    EXPECT_FALSE(n.tree()->type_has_any(n.id(), KEY_STYLE));
    EXPECT_FALSE(n.tree()->type_has_all(n.id(), KEY_STYLE));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE));
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_FALSE(n.is_key_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_val_quoted());
    EXPECT_FALSE(n.type_has_any(KEY_STYLE));
    EXPECT_FALSE(n.type_has_all(KEY_STYLE));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE));
}
void test_val_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_val_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_key_quoted(n.id()));
    EXPECT_FALSE(n.tree()->type_has_any(n.id(), VAL_STYLE));
    EXPECT_FALSE(n.tree()->type_has_all(n.id(), VAL_STYLE));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE));
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_FALSE(n.is_val_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_key_quoted());
    EXPECT_FALSE(n.type_has_any(VAL_STYLE));
    EXPECT_FALSE(n.type_has_all(VAL_STYLE));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE));
}

void test_key_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_key_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_PLAIN));
    EXPECT_TRUE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_FALSE(n.is_key_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_val_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_PLAIN));
    EXPECT_TRUE(n.type_has_all(KEY_PLAIN));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_PLAIN));
}
void test_key_squo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_TRUE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_SQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_SQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_SQUO));
    EXPECT_TRUE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_SQUO));
    EXPECT_TRUE(n.type_has_all(KEY_SQUO));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_SQUO));
}
void test_key_dquo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_TRUE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_DQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_DQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_DQUO));
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_TRUE(n.is_key_dquo());
    EXPECT_FALSE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_DQUO));
    EXPECT_TRUE(n.type_has_all(KEY_DQUO));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_DQUO));
}
void test_key_literal(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_TRUE(n.tree()->is_key_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_LITERAL));
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_TRUE(n.is_key_literal());
    EXPECT_FALSE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_LITERAL));
    EXPECT_TRUE(n.type_has_all(KEY_LITERAL));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_LITERAL));
}
void test_key_folded(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_key_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_key_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_key_literal(n.id()));
    EXPECT_TRUE(n.tree()->is_key_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), KEY_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), KEY_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), KEY_STYLE & ~KEY_FOLDED));
    EXPECT_FALSE(n.is_key_plain());
    EXPECT_FALSE(n.is_key_squo());
    EXPECT_FALSE(n.is_key_dquo());
    EXPECT_TRUE(n.is_key_folded());
    EXPECT_TRUE(n.is_key_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(KEY_FOLDED));
    EXPECT_TRUE(n.type_has_all(KEY_FOLDED));
    EXPECT_TRUE(n.type_has_none(KEY_STYLE & ~KEY_FOLDED));
}

void test_val_plain(ConstNodeRef n)
{
    EXPECT_TRUE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_FALSE(n.tree()->is_val_quoted(n.id()));
    EXPECT_EQ(n.tree()->is_quoted(n.id()), n.tree()->is_key_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_PLAIN));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_PLAIN));
    EXPECT_TRUE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_FALSE(n.is_val_quoted());
    EXPECT_EQ(n.is_quoted(), n.is_key_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_PLAIN));
    EXPECT_TRUE(n.type_has_all(VAL_PLAIN));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_PLAIN));
}
void test_val_squo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_TRUE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_SQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_SQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_SQUO));
    EXPECT_TRUE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_SQUO));
    EXPECT_TRUE(n.type_has_all(VAL_SQUO));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_SQUO));
}
void test_val_dquo(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_TRUE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_DQUO));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_DQUO));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_DQUO));
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_TRUE(n.is_val_dquo());
    EXPECT_FALSE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_DQUO));
    EXPECT_TRUE(n.type_has_all(VAL_DQUO));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_DQUO));
}
void test_val_literal(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_TRUE(n.tree()->is_val_literal(n.id()));
    EXPECT_FALSE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_LITERAL));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_LITERAL));
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_TRUE(n.is_val_literal());
    EXPECT_FALSE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_LITERAL));
    EXPECT_TRUE(n.type_has_all(VAL_LITERAL));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_LITERAL));
}
void test_val_folded(ConstNodeRef n)
{
    EXPECT_FALSE(n.tree()->is_val_plain(n.id()));
    EXPECT_FALSE(n.tree()->is_val_squo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_dquo(n.id()));
    EXPECT_FALSE(n.tree()->is_val_literal(n.id()));
    EXPECT_TRUE(n.tree()->is_val_folded(n.id()));
    EXPECT_TRUE(n.tree()->is_val_quoted(n.id()));
    EXPECT_TRUE(n.tree()->is_quoted(n.id()));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), STYLE));
    EXPECT_TRUE(n.tree()->type_has_any(n.id(), VAL_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_all(n.id(), VAL_FOLDED));
    EXPECT_TRUE(n.tree()->type_has_none(n.id(), VAL_STYLE & ~VAL_FOLDED));
    EXPECT_FALSE(n.is_val_plain());
    EXPECT_FALSE(n.is_val_squo());
    EXPECT_FALSE(n.is_val_dquo());
    EXPECT_TRUE(n.is_val_folded());
    EXPECT_TRUE(n.is_val_quoted());
    EXPECT_TRUE(n.is_quoted());
    EXPECT_TRUE(n.type_has_any(STYLE));
    EXPECT_TRUE(n.type_has_any(VAL_FOLDED));
    EXPECT_TRUE(n.type_has_all(VAL_FOLDED));
    EXPECT_TRUE(n.type_has_none(VAL_STYLE & ~VAL_FOLDED));
}

void test_key_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_key_style({});
    test_key_nostyle(n);
    n.set_key_style(KEY_PLAIN);
    test_key_plain(n);
    n.set_key_style(KEY_SQUO);
    test_key_squo(n);
    n.set_key_style(KEY_DQUO);
    n.set_key_style(KEY_DQUO);
    test_key_dquo(n);
    n.set_key_style(KEY_LITERAL);
    test_key_literal(n);
    n.set_key_style(KEY_FOLDED);
    test_key_folded(n);
    n.set_key_style(orig);
}
void test_val_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_val_style({});
    test_val_nostyle(n);
    n.set_val_style(VAL_PLAIN);
    test_val_plain(n);
    n.set_val_style(VAL_SQUO);
    test_val_squo(n);
    n.set_val_style(VAL_DQUO);
    test_val_dquo(n);
    n.set_val_style(VAL_LITERAL);
    test_val_literal(n);
    n.set_val_style(VAL_FOLDED);
    test_val_folded(n);
    n.set_val_style(orig);
}

void test_nostyle(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_ml());
    EXPECT_FALSE(n.is_flow());
}
void test_block(ConstNodeRef n)
{
    EXPECT_TRUE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_ml());
    EXPECT_FALSE(n.is_flow());
}
void test_flow_sl(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_TRUE(n.is_flow_sl());
    EXPECT_FALSE(n.is_flow_ml());
    EXPECT_TRUE(n.is_flow());
}
void test_flow_ml(ConstNodeRef n)
{
    EXPECT_FALSE(n.is_block());
    EXPECT_FALSE(n.is_flow_sl());
    EXPECT_TRUE(n.is_flow_ml());
    EXPECT_TRUE(n.is_flow());
}

void test_container_styles(NodeRef n)
{
    NodeType orig = n.type();
    n.set_container_style({});
    test_nostyle(n);
    n.set_container_style(BLOCK);
    test_block(n);
    n.set_container_style(FLOW_SL);
    test_flow_sl(n);
    n.set_container_style(FLOW_ML);
    test_flow_ml(n);
    n.set_container_style(orig);
}

csubstr style_yaml = R"(plain key: plain val
'squoted key': 'squoted val'
"dquoted key": "dquoted val"
? |-
    literal key
: |-
    literal val
? >-
    folded key
: >-
    folded val
block map:
  this is: block
block seq:
  - this is
  - block
flow map: {this is: flow}
flow seq: [this is, flow]
)";
TEST(Tree, style)
{
    Tree tree = parse_in_arena(style_yaml);

    test_key_plain(tree["plain key"]);
    test_key_styles(tree["plain key"]);
    test_key_plain(tree["plain key"]);

    test_val_plain(tree["plain key"]);
    test_val_styles(tree["plain key"]);
    test_val_plain(tree["plain key"]);

    test_key_squo(tree["squoted key"]);
    test_key_styles(tree["squoted key"]);
    test_key_squo(tree["squoted key"]);

    test_val_squo(tree["squoted key"]);
    test_val_styles(tree["squoted key"]);
    test_val_squo(tree["squoted key"]);

    test_key_dquo(tree["dquoted key"]);
    test_key_styles(tree["dquoted key"]);
    test_key_dquo(tree["dquoted key"]);

    test_val_dquo(tree["dquoted key"]);
    test_val_styles(tree["squoted key"]);
    test_val_dquo(tree["dquoted key"]);

    test_key_literal(tree["literal key"]);
    test_key_styles(tree["literal key"]);
    test_key_literal(tree["literal key"]);

    test_val_literal(tree["literal key"]);
    test_val_styles(tree["literal key"]);
    test_val_literal(tree["literal key"]);

    test_key_folded(tree["folded key"]);
    test_key_styles(tree["folded key"]);
    test_key_folded(tree["folded key"]);

    test_val_folded(tree["folded key"]);
    test_val_styles(tree["folded key"]);
    test_val_folded(tree["folded key"]);

    test_block(tree["block map"]);
    test_container_styles(tree["block map"]);
    test_block(tree["block map"]);

    test_block(tree["block seq"]);
    test_container_styles(tree["block seq"]);
    test_block(tree["block seq"]);

    test_flow_sl(tree["flow map"]);
    test_container_styles(tree["flow map"]);
    test_flow_sl(tree["flow map"]);

    test_flow_sl(tree["flow seq"]);
    test_container_styles(tree["flow seq"]);
    test_flow_sl(tree["flow seq"]);
}

TEST(Tree, unfiltered)
{
    const Tree tree = parse_in_arena(style_yaml);
    EXPECT_FALSE(tree.is_key_unfiltered(tree[0].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[0].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[1].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[1].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[2].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[2].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[3].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[3].id()));
    EXPECT_FALSE(tree.is_key_unfiltered(tree[4].id()));
    EXPECT_FALSE(tree.is_val_unfiltered(tree[4].id()));
    EXPECT_FALSE(tree[0].is_key_unfiltered());
    EXPECT_FALSE(tree[0].is_val_unfiltered());
    EXPECT_FALSE(tree[1].is_key_unfiltered());
    EXPECT_FALSE(tree[1].is_val_unfiltered());
    EXPECT_FALSE(tree[2].is_key_unfiltered());
    EXPECT_FALSE(tree[2].is_val_unfiltered());
    EXPECT_FALSE(tree[3].is_key_unfiltered());
    EXPECT_FALSE(tree[3].is_val_unfiltered());
    EXPECT_FALSE(tree[4].is_key_unfiltered());
    EXPECT_FALSE(tree[4].is_val_unfiltered());
    EXPECT_EQ(tree[3].key(), "literal key");
    EXPECT_EQ(tree[3].val(), "literal val");
    EXPECT_EQ(tree[4].key(), "folded key");
    EXPECT_EQ(tree[4].val(), "folded val");
    EventHandlerTree evt_handler = {};
    Parser parser(&evt_handler, ParserOptions().scalar_filtering(false));
    const Tree tree2 = parse_in_arena(&parser, style_yaml);
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[0].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[0].id()));
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[1].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[1].id()));
    EXPECT_FALSE(tree2.is_key_unfiltered(tree2[2].id()));
    EXPECT_FALSE(tree2.is_val_unfiltered(tree2[2].id()));
    EXPECT_TRUE(tree2.is_key_unfiltered(tree2[3].id()));
    EXPECT_TRUE(tree2.is_val_unfiltered(tree2[3].id()));
    EXPECT_TRUE(tree2.is_key_unfiltered(tree2[4].id()));
    EXPECT_TRUE(tree2.is_val_unfiltered(tree2[4].id()));
    EXPECT_FALSE(tree2[0].is_key_unfiltered());
    EXPECT_FALSE(tree2[0].is_val_unfiltered());
    EXPECT_FALSE(tree2[1].is_key_unfiltered());
    EXPECT_FALSE(tree2[1].is_val_unfiltered());
    EXPECT_FALSE(tree2[2].is_key_unfiltered());
    EXPECT_FALSE(tree2[2].is_val_unfiltered());
    EXPECT_TRUE(tree2[3].is_key_unfiltered());
    EXPECT_TRUE(tree2[3].is_val_unfiltered());
    EXPECT_TRUE(tree2[4].is_key_unfiltered());
    EXPECT_TRUE(tree2[4].is_val_unfiltered());
    EXPECT_NE(tree2[3].key(), "literal key");
    EXPECT_NE(tree2[3].val(), "literal val");
    EXPECT_NE(tree2[4].key(), "folded key");
    EXPECT_NE(tree2[4].val(), "folded val");
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
