#include "test_lib/test_case_node.hpp"
#include "test_lib/test_case.hpp"
#include <gtest/gtest.h>

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace c4 {
namespace yml {

using N = TestCaseNode;
using L = TestCaseNode::iseqmap;

TEST(CaseNode, setting_up)
{
    L tl1 = {DOC, DOC};
    L tl2 = {(DOC), (DOC)};

    ASSERT_EQ(tl1.size(), tl2.size());
    N const& d1 = *tl1.begin();
    N const& d2 = *(tl1.begin() + 1);
    ASSERT_EQ(d1.reccount(), d2.reccount());
    ASSERT_EQ((type_bits)d1.type, (type_bits)DOC);
    ASSERT_EQ((type_bits)d2.type, (type_bits)DOC);

    N n1(tl1);
    N n2(tl2);
    ASSERT_EQ(n1.reccount(), n2.reccount());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NodeType_e TestCaseNode::_guess() const
{
    NodeType t;
    C4_ASSERT(!val.empty() != !children.empty() || (val.empty() && children.empty()));
    if(children.empty())
    {
        C4_ASSERT(parent);
        if(key.empty())
        {
            t = VAL;
        }
        else
        {
            t = KEYVAL;
        }
    }
    else
    {
        NodeType_e has_key = key.empty() ? NOTYPE : KEY;
        auto const& ch = children.front();
        if(ch.key.empty())
        {
            t = (has_key|SEQ);
        }
        else
        {
            t = (has_key|MAP);
        }
    }
    if( ! key_tag.empty())
    {
        C4_ASSERT( ! key.empty());
        t.add(KEYTAG);
    }
    if( ! val_tag.empty())
    {
        C4_ASSERT( ! val.empty() || ! children.empty());
        t.add(VALTAG);
    }
    if( ! key_anchor.str.empty())
    {
        t.add(key_anchor.type);
    }
    if( ! val_anchor.str.empty())
    {
        t.add(val_anchor.type);
    }
    return t;
}


//-----------------------------------------------------------------------------
void TestCaseNode::compare_child(yml::ConstNodeRef const& n, id_type pos) const
{
    SCOPED_TRACE(pos);
    SCOPED_TRACE(n.id());
    EXPECT_TRUE(pos < n.num_children());
    EXPECT_TRUE((size_t)pos < children.size());

    if(pos >= n.num_children() || (size_t)pos >= children.size()) return;

    ASSERT_GT(n.num_children(), pos);
    auto const& expectedch = children[(size_t)pos];

    if(type & MAP)
    {
        ConstNodeRef actualch = n.find_child(expectedch.key);
        SCOPED_TRACE(actualch.id());
        if(!actualch.invalid())
        {
            // there may be duplicate keys.
            if(actualch.id() != n[pos].id())
                actualch = n[pos];
            SCOPED_TRACE(actualch.id());
            //EXPECT_EQ(fch, n[ch.key]);
            EXPECT_EQ(actualch.get(), n[pos].get());
            //EXPECT_EQ(n[pos], n[ch.key]);
            EXPECT_EQ(n[expectedch.key].key(), expectedch.key);
        }
        else
        {
            printf("error: node should have child %.*s: ", (int)expectedch.key.len, expectedch.key.str);
            fflush(stdout);
            print_path(n);
            fflush(stdout);
            printf("\n");
            print_node(n);
            GTEST_FAIL();
        }
    }

    if(type & SEQ)
    {
        EXPECT_FALSE(n[pos].has_key());
        EXPECT_EQ(n[pos].get()->m_key.scalar, children[(size_t)pos].key);
        ConstNodeRef actualch = n.child(pos);
        SCOPED_TRACE(actualch.id());
        EXPECT_EQ(actualch.get(), n[pos].get());
    }

    if(expectedch.type & KEY)
    {
        ConstNodeRef actualfch = n[pos];
        SCOPED_TRACE(actualfch.id());
        EXPECT_TRUE(actualfch.has_key());
        if(actualfch.has_key())
        {
            EXPECT_EQ(actualfch.key(), expectedch.key);
        }

        if( ! expectedch.key_tag.empty())
        {
            EXPECT_TRUE(actualfch.has_key_tag());
            if(actualfch.has_key_tag())
            {
                EXPECT_EQ(actualfch.key_tag(), expectedch.key_tag);
            }
        }
    }

    if(expectedch.type & VAL)
    {
        ConstNodeRef actualch = n[pos];
        SCOPED_TRACE(actualch.id());
        EXPECT_TRUE(actualch.has_val());
        if(actualch.has_val())
        {
            EXPECT_EQ(actualch.val(), expectedch.val);
        }

        if( ! expectedch.val_tag.empty())
        {
            EXPECT_TRUE(actualch.has_val_tag());
            if(actualch.has_val_tag())
            {
                EXPECT_EQ(actualch.val_tag(), expectedch.val_tag);
            }
        }
    }
}


void TestCaseNode::compare(yml::ConstNodeRef const& actual, bool ignore_quote) const
{
    SCOPED_TRACE(actual.id());
    if(ignore_quote)
    {
        const type_bits mask = VALQUO | KEYQUO | KEY_STYLE | VAL_STYLE | CONTAINER_STYLE;
        const type_bits actual_type   = actual.get()->m_type & ~mask;
        const type_bits expected_type = type & ~mask;
        EXPECT_EQ(expected_type, actual_type);
        RYML_COMPARE_NODE_TYPE(expected_type, actual_type, ==, EQ);
    }
    else
    {
        // the type() method masks the type, and thus tag flags are omitted on its return value
        RYML_COMPARE_NODE_TYPE(actual.get()->m_type, this->type, ==, EQ);
    }

    EXPECT_EQ(actual.num_children(), children.size());

    if(actual.has_key())
    {
        EXPECT_EQ(actual.key(), key);
    }

    if(actual.has_val())
    {
        EXPECT_EQ(actual.val(), val);
    }

    // check that the children are in the same order
    {
        EXPECT_EQ(children.size(), actual.num_children());
        id_type ic = 0;
        for(auto const &expectedch : children)
        {
            SCOPED_TRACE("comparing: iteration based on the ref children");
            (void)expectedch; // unused
            compare_child(actual, ic++);
        }
        ic = 0;
        for(auto const actualch : actual.children())
        {
            SCOPED_TRACE("comparing: iteration based on the yml::Node children");
            (void)actualch; // unused
            compare_child(actual, ic++);
        }
        if(!actual.first_child().invalid())
        {
            ic = 0;
            for(auto const ch : actual.first_child().siblings())
            {
                SCOPED_TRACE("comparing: iteration based on the yml::Node siblings");
                (void)ch; // unused
                compare_child(actual, ic++);
            }
        }
    }

    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
    for(id_type i = 0, ei = actual.num_children(), j = 0, ej = (id_type)children.size(); i < ei && j < ej; ++i, ++j)
    {
        children[(size_t)j].compare(actual[i], ignore_quote);
    }
    C4_SUPPRESS_WARNING_GCC_POP
}

void TestCaseNode::recreate(yml::NodeRef *n) const
{
    C4_ASSERT( ! n->has_children());
    NodeData *nd = n->get();
    nd->m_type = type|key_anchor.type|val_anchor.type;
    nd->m_key.scalar = key;
    nd->m_key.tag = (key_tag);
    nd->m_key.anchor = key_anchor.str;
    nd->m_val.scalar = val;
    nd->m_val.tag = (val_tag);
    nd->m_val.anchor = val_anchor.str;
    Tree &tree = *n->tree();
    id_type nid = n->id(); // don't use node from now on
    for(TestCaseNode const& ch : children)
    {
        id_type id = tree.append_child(nid);
        NodeRef chn(n->tree(), id);
        ch.recreate(&chn);
    }
}

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH

} // namespace yml
} // namespace c4
