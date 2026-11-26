#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse_engine.def.hpp"
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_helpers_tree.hpp"
#include "./test_lib/callbacks_tester.hpp"

#include <unordered_map>
#include <vector>

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated-declarations")
C4_SUPPRESS_WARNING_MSVC(4996) // deprecated
C4_SUPPRESS_WARNING_MSVC(4389) // signed/unsigned mismatch
C4_SUPPRESS_WARNING_CLANG("-Wdollar-in-identifier-extension")

RYML_DEFINE_TEST_MAIN()

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

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_POP
