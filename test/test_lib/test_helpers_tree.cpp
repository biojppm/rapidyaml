#include "test_lib/test_helpers_tree.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/yml/tree.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse_engine.def.hpp"
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/emit.hpp"
#include "c4/yml/detail/print.hpp"
#include "c4/yml/detail/dbgprint.hpp"
#include "c4/yml/detail/checks.hpp"
#include "c4/yml/parse_options.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
#if defined(__GNUC__) && (__GNUC__ >= 6)
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif

namespace c4 {
namespace yml {

void test_check_emit_check_with_parser(Tree const& t, Parser &parser, fn_test_check_with_parser check_fn)
{
    {
        SCOPED_TRACE("original yaml");
        test_invariants(t);
        check_fn(t, parser);
        if(testing::Test::HasFailure())
        {
            print_tree(t);
        }
    }
    auto emit_and_parse = [&](Tree const& tp, Tree *out, const char* identifier){
        SCOPED_TRACE(identifier);
        std::string emitted = emitrs_yaml<std::string>(tp);
        parse_in_arena(&parser, to_csubstr(emitted), out);
        test_invariants(*out);
        check_fn(*out, parser);
        if(testing::Test::HasFailure())
        {
            printf("~~~%s~~~[%zu]\n%.*s", identifier, emitted.size(), (int)emitted.size(), emitted.data());
            print_tree(*out);
        }
    };
    if(!testing::Test::HasFailure())
    {
        Tree cp1;
        SCOPED_TRACE("level 1");
        emit_and_parse(t, &cp1, "level 1");
        if(!testing::Test::HasFailure())
        {
            Tree cp2;
            SCOPED_TRACE("level 2");
            emit_and_parse(cp1, &cp2, "level 2");
            if(!testing::Test::HasFailure())
            {
                Tree cp3;
                SCOPED_TRACE("level 3");
                emit_and_parse(cp2, &cp3, "level 3");
            }
        }
    }
}
void test_check_emit_check(Tree const& t, Parser &parser, fn_test_check check_fn)
{
    test_check_emit_check_with_parser(t, parser, [&check_fn](Tree const& t_, Parser const&){
        check_fn(t_);
    });
}


void test_check_emit_check_with_parser(Tree const& t, fn_test_check_with_parser check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, ParserOptions());
    test_check_emit_check_with_parser(t, parser, check_fn);
}
void test_check_emit_check(Tree const& t, fn_test_check check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, ParserOptions());
    test_check_emit_check(t, parser, check_fn);
}

void test_check_emit_check_with_parser(csubstr yaml, ParserOptions opts, fn_test_check_with_parser check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    const Tree t = parse_in_arena(&parser, yaml);
    test_check_emit_check_with_parser(t, parser, check_fn);
}
void test_check_emit_check(csubstr yaml, ParserOptions opts, fn_test_check check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    const Tree t = parse_in_arena(&parser, yaml);
    test_check_emit_check(t, parser, check_fn);
}


void test_check_emit_check_with_parser(csubstr yaml, fn_test_check_with_parser check_fn)
{
    test_check_emit_check_with_parser(yaml, ParserOptions(), check_fn);
}
void test_check_emit_check(csubstr yaml, fn_test_check check_fn)
{
    test_check_emit_check(yaml, ParserOptions(), check_fn);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

id_type _num_leaves(Tree const& t, id_type node)
{
    id_type count = 0;
    for(id_type ch = t.first_child(node); ch != NONE; ch = t.next_sibling(ch))
        count += _num_leaves(t, ch);
    return count;
}


void test_compare(ConstNodeRef const& actual, ConstNodeRef const& expected,
                  const char *actual_name, const char *expected_name,
                  type_bits cmp_mask)
{
    if(actual.is_root() && expected.is_root())
        test_compare(*actual.tree(), *expected.tree(),
                     actual_name, expected_name,
                     cmp_mask);
    else
        test_compare(*actual.tree(), actual.id(),
                     *expected.tree(), expected.id(),
                     0,
                     actual_name, expected_name,
                     cmp_mask);
}
void test_compare(Tree const& actual, Tree const& expected,
                  const char *actual_name, const char *expected_name,
                  type_bits cmp_mask)
{
    ASSERT_EQ(actual.empty(), expected.empty());
    if(actual.empty() || expected.empty())
        return;
    EXPECT_EQ(actual.size(), expected.size());
    EXPECT_EQ(_num_leaves(actual, actual.root_id()), _num_leaves(expected, expected.root_id()));
    test_compare(actual, actual.root_id(),
                 expected, expected.root_id(),
                 0,
                 actual_name, expected_name,
                 cmp_mask);
}


void test_compare(Tree const& actual, id_type node_actual,
                  Tree const& expected, id_type node_expected,
                  id_type depth, const char *actual_name, const char *expected_name,
                  type_bits cmp_mask)
{
    RYML_TRACE_FMT("actual={}[{}] vs expected={}[{}]. depth={}", actual_name, node_actual, expected_name, node_expected, depth);

    ASSERT_NE(node_actual, (id_type)NONE);
    ASSERT_NE(node_expected, (id_type)NONE);
    ASSERT_LT(node_actual, actual.capacity());
    ASSERT_LT(node_expected, expected.capacity());

    NodeType type_actual = actual.type(node_actual) & cmp_mask;
    NodeType type_expected = expected.type(node_expected) & cmp_mask;
    RYML_COMPARE_NODE_TYPE(type_actual, type_expected, ==, EQ);

    EXPECT_EQ(actual.has_key(node_actual), expected.has_key(node_expected));
    if(actual.has_key(node_actual) && expected.has_key(node_expected))
    {
        EXPECT_EQ(actual.key(node_actual), expected.key(node_expected));
    }

    EXPECT_EQ(actual.has_val(node_actual), expected.has_val(node_expected));
    if(actual.has_val(node_actual) && expected.has_val(node_expected))
    {
        EXPECT_EQ(actual.val(node_actual), expected.val(node_expected));
    }

    EXPECT_EQ(actual.has_key_tag(node_actual), expected.has_key_tag(node_expected));
    if(actual.has_key_tag(node_actual) && expected.has_key_tag(node_expected))
    {
        EXPECT_EQ(actual.key_tag(node_actual), expected.key_tag(node_expected));
    }

    EXPECT_EQ(actual.has_val_tag(node_actual), expected.has_val_tag(node_expected));
    if(actual.has_val_tag(node_actual) && expected.has_val_tag(node_expected))
    {
        EXPECT_EQ(actual.val_tag(node_actual), expected.val_tag(node_expected));
    }

    EXPECT_EQ(actual.has_key_anchor(node_actual), expected.has_key_anchor(node_expected));
    if(actual.has_key_anchor(node_actual) && expected.has_key_anchor(node_expected))
    {
        EXPECT_EQ(actual.key_anchor(node_actual), expected.key_anchor(node_expected));
    }

    EXPECT_EQ(actual.has_val_anchor(node_actual), expected.has_val_anchor(node_expected));
    if(actual.has_val_anchor(node_actual) && expected.has_val_anchor(node_expected))
    {
        EXPECT_EQ(actual.val_anchor(node_actual), expected.val_anchor(node_expected));
    }

    EXPECT_EQ(actual.num_children(node_actual), expected.num_children(node_expected));
    for(id_type ia = actual.first_child(node_actual), ib = expected.first_child(node_expected);
        ia != NONE && ib != NONE;
        ia = actual.next_sibling(ia), ib = expected.next_sibling(ib))
    {
        test_compare(actual, ia, expected, ib, depth+1, actual_name, expected_name);
    }

}

void test_arena_not_shared(Tree const& a, Tree const& b)
{
    for(NodeData const* n = a.m_buf, *e = a.m_buf + a.m_cap; n != e; ++n)
    {
        EXPECT_FALSE(b.in_arena(n->m_key.scalar)) << n - a.m_buf;
        EXPECT_FALSE(b.in_arena(n->m_key.tag   )) << n - a.m_buf;
        EXPECT_FALSE(b.in_arena(n->m_key.anchor)) << n - a.m_buf;
        EXPECT_FALSE(b.in_arena(n->m_val.scalar)) << n - a.m_buf;
        EXPECT_FALSE(b.in_arena(n->m_val.tag   )) << n - a.m_buf;
        EXPECT_FALSE(b.in_arena(n->m_val.anchor)) << n - a.m_buf;
    }
    for(NodeData const* n = b.m_buf, *e = b.m_buf + b.m_cap; n != e; ++n)
    {
        EXPECT_FALSE(a.in_arena(n->m_key.scalar)) << n - b.m_buf;
        EXPECT_FALSE(a.in_arena(n->m_key.tag   )) << n - b.m_buf;
        EXPECT_FALSE(a.in_arena(n->m_key.anchor)) << n - b.m_buf;
        EXPECT_FALSE(a.in_arena(n->m_val.scalar)) << n - b.m_buf;
        EXPECT_FALSE(a.in_arena(n->m_val.tag   )) << n - b.m_buf;
        EXPECT_FALSE(a.in_arena(n->m_val.anchor)) << n - b.m_buf;
    }
    for(TagDirective const& td : a.m_tag_directives)
    {
        EXPECT_FALSE(b.in_arena(td.handle));
        EXPECT_FALSE(b.in_arena(td.prefix));
    }
    for(TagDirective const& td : b.m_tag_directives)
    {
        EXPECT_FALSE(a.in_arena(td.handle));
        EXPECT_FALSE(a.in_arena(td.prefix));
    }
}


void test_invariants(NodeType ty)
{
    #define EXPECT_ALL(v, flags) EXPECT_EQ(((v) & (flags)), flags)
    #define EXPECT_ONE(v, flags, one) EXPECT_EQ(((v) & (flags)), one)
    #define EXPECT_ANY(v, flags) EXPECT_NE(((v) & (flags)), 0)
    #define EXPECT_NONE(v, flags) EXPECT_EQ(((v) & (flags)), 0)
    type_bits STREAMONLY = (STREAM & ~SEQ);
    if(ty & (KEYNIL|KEYREF|KEY_STYLE))
    {
        EXPECT_ALL(ty, KEY);
    }
    if(ty & (VALNIL|VALREF|VAL_STYLE))
    {
        EXPECT_ALL(ty, VAL);
    }
    if(ty & (KEYTAG|KEYANCH))
    {
        EXPECT_ALL(ty, KEY);
    }
    if(ty & (VALTAG|VALANCH))
    {
        EXPECT_ANY(ty, VAL|SEQ|MAP);
    }
    if(ty & KEYREF)
    {
        EXPECT_NONE(ty, KEYTAG|KEYANCH|KEY_STYLE);
    }
    if(ty & VALREF)
    {
        EXPECT_NONE(ty, VALTAG|VALANCH|VAL_STYLE|CONTAINER_STYLE);
    }
    if(ty & VAL)
    {
        EXPECT_NONE(ty, MAP|SEQ|STREAM|CONTAINER_STYLE);
    }
    if(ty & MAP)
    {
        EXPECT_NONE(ty, VAL|SEQ|STREAM|VAL_STYLE);
    }
    if(ty & SEQ)
    {
        EXPECT_NONE(ty, VAL|MAP|VAL_STYLE);
    }
    if(ty & DOC)
    {
        EXPECT_NONE(ty, STREAMONLY|KEY);
    }
    if(ty & STREAMONLY)
    {
        EXPECT_NONE(ty, DOC|MAP|KEY);
    }
    if(ty & (MAP|SEQ|VAL))
    {
        if(ty & MAP)
        {
            EXPECT_ONE(ty, MAP|SEQ|VAL, MAP);
        }
        if(ty & SEQ)
        {
            EXPECT_ONE(ty, MAP|SEQ|VAL, SEQ);
        }
        if(ty & VAL)
        {
            EXPECT_ONE(ty, MAP|SEQ|VAL, VAL);
        }
    }
    if(ty & CONTAINER_STYLE)
    {
        EXPECT_NONE(ty, VAL);
        if(ty & FLOW_SL)
        {
            EXPECT_ONE(ty, CONTAINER_STYLE & ~FLOW_SPC, FLOW_SL);
        }
        if(ty & FLOW_ML1)
        {
            EXPECT_ONE(ty, CONTAINER_STYLE & ~FLOW_SPC, FLOW_ML1);
        }
        if(ty & FLOW_MLN)
        {
            EXPECT_ONE(ty, CONTAINER_STYLE & ~FLOW_SPC, FLOW_MLN);
        }
        if(ty & FLOW_SPC)
        {
            EXPECT_ANY(ty, FLOW_MLX|FLOW_SL);
        }
        if(ty & BLOCK)
        {
            EXPECT_ONE(ty, CONTAINER_STYLE, BLOCK);
        }
    }
    if(ty & KEY_STYLE)
    {
        if(ty & KEY_PLAIN)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_PLAIN);
        }
        if(ty & KEY_SQUO)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_SQUO);
        }
        if(ty & KEY_DQUO)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_DQUO);
        }
        if(ty & KEY_LITERAL)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_LITERAL);
        }
        if(ty & KEY_FOLDED)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_FOLDED);
        }
    }
    if(ty & VAL_STYLE)
    {
        if(ty & KEY_PLAIN)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_PLAIN);
        }
        if(ty & KEY_SQUO)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_SQUO);
        }
        if(ty & KEY_DQUO)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_DQUO);
        }
        if(ty & KEY_LITERAL)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_LITERAL);
        }
        if(ty & KEY_FOLDED)
        {
            EXPECT_ONE(ty, KEY_STYLE, KEY_FOLDED);
        }
    }
}

void test_invariants(ConstNodeRef const& n)
{
    SCOPED_TRACE(n.id());
    test_invariants(n.type());
    if(n.is_root())
    {
        EXPECT_FALSE(n.has_other_siblings());
    }
    NodeType ty = n.type();
    // vals cannot be containers
    if( ! n.empty() && ! ty.is_doc())
    {
        EXPECT_NE(ty.has_val(), ty.is_container());
    }
    if(n.has_children())
    {
        EXPECT_TRUE(ty.is_container());
        EXPECT_FALSE(ty.is_val());
    }
    if(ty.has_key())
    {
        EXPECT_TRUE(n.parent().readable());
        if(n.parent().readable())
        {
            EXPECT_TRUE(n.parent().type().is_map());
        }
    }
    if(n.has_key())
    {
        EXPECT_TRUE(n.parent().readable());
        if(n.parent().readable())
        {
            EXPECT_TRUE(n.parent().is_map());
        }
    }
    // check sibling reciprocity
    for(ConstNodeRef s : n.siblings())
    {
        EXPECT_TRUE(n.has_sibling(s));
        EXPECT_TRUE(s.has_sibling(n));
        if(ty.has_key() && ty.has_key())
        {
            EXPECT_TRUE(n.has_sibling(s.key()));
            EXPECT_TRUE(s.has_sibling(n.key()));
        }
        EXPECT_EQ(s.parent().get(), n.parent().get());
    }
    // check parent/child reciprocity
    if(n.parent().readable())
    {
        EXPECT_EQ(n.parent().num_children() > 1, n.has_other_siblings());
        EXPECT_TRUE(n.parent().has_child(n));
        EXPECT_EQ(n.parent().num_children(), n.num_siblings());
        EXPECT_EQ(n.parent().num_children(), n.num_other_siblings()+id_type(1));
        // doc parent must be a seq and a stream
        if(ty.is_doc())
        {
            EXPECT_TRUE(n.parent().is_seq());
            EXPECT_TRUE(n.parent().is_stream());
        }
        if(n.parent().type().is_map())
        {
            EXPECT_TRUE(ty.has_key());
        }
        if(n.parent().is_map())
        {
            EXPECT_TRUE(n.has_key());
        }
    }
    else
    {
        EXPECT_TRUE(n.is_root());
    }
    if(ty.is_seq())
    {
        EXPECT_TRUE(ty.is_container());
        EXPECT_FALSE(ty.is_map());
        for(ConstNodeRef ch : n.children())
        {
            EXPECT_FALSE(ch.is_keyval());
            EXPECT_FALSE(ch.has_key());
        }
    }
    if(ty.is_map())
    {
        EXPECT_TRUE(ty.is_container());
        EXPECT_FALSE(ty.is_seq());
        for(ConstNodeRef ch : n.children())
        {
            if(ch.type() != NOTYPE)
            {
                EXPECT_TRUE(ch.has_key());
            }
        }
    }
    if(n.is_stream() && !n.is_seq())
    {
        for(ConstNodeRef ch : n.children())
        {
            if(ch.type() != NOTYPE)
            {
                EXPECT_TRUE(ch.is_doc());
            }
        }
    }
    if(n.has_key_anchor())
    {
        EXPECT_FALSE(n.key_anchor().empty());
        EXPECT_FALSE(ty.is_key_ref());
    }
    if(ty.has_val_anchor())
    {
        EXPECT_FALSE(n.val_anchor().empty());
        EXPECT_FALSE(ty.is_val_ref());
    }
    if(ty.is_key_ref())
    {
        EXPECT_FALSE(n.key_ref().empty());
        EXPECT_FALSE(ty.has_key_anchor());
    }
    if(ty.is_val_ref())
    {
        EXPECT_FALSE(n.val_ref().empty());
        EXPECT_FALSE(ty.has_val_anchor());
    }
    if(ty.has_key())
    {
        if(ty.is_key_quoted())
        {
            EXPECT_FALSE(ty.key_is_null());
        }
        if(n.key_is_null())
        {
            EXPECT_FALSE(ty.is_key_quoted());
        }
    }
    if(ty.has_val() && ty.is_val_quoted())
    {
        if(ty.is_val_quoted())
        {
            EXPECT_FALSE(n.val_is_null());
        }
        if(ty.val_is_null())
        {
            EXPECT_FALSE(ty.is_val_quoted());
        }
    }
    // ... add more tests here

    // now recurse into the children
    for(ConstNodeRef ch : n.children())
    {
        test_invariants(ch);
    }
}


static size_t test_tree_invariants(ConstNodeRef const& n)
{
    ConstNodeRef parent = n.parent();

    if(n.get()->m_prev_sibling == NONE)
    {
        if(parent.readable())
        {
            EXPECT_EQ(parent.first_child().get(), n.get());
            EXPECT_EQ(parent.first_child().id(), n.id());
        }
    }

    if(n.get()->m_next_sibling == NONE)
    {
        if(parent.readable())
        {
            EXPECT_EQ(parent.last_child().get(), n.get());
            EXPECT_EQ(parent.last_child().id(), n.id());
        }
    }

    if(!parent.readable())
    {
        EXPECT_TRUE(n.is_root());
        EXPECT_EQ(n.prev_sibling().get(), nullptr);
        EXPECT_EQ(n.next_sibling().get(), nullptr);
    }

    size_t count = 1, num = 0;
    for(ConstNodeRef ch : n.children())
    {
        EXPECT_NE(ch.id(), n.id());
        count += test_tree_invariants(ch);
        ++num;
    }

    EXPECT_EQ(num, n.num_children());

    return count;
}

void test_invariants(Tree const& t)
{
    SCOPED_TRACE("tree invariants");

    ASSERT_LE(t.size(), t.capacity());
    EXPECT_EQ(t.size() + t.slack(), t.capacity());

    ASSERT_LE(t.arena_size(), t.arena_capacity());
    ASSERT_LE(t.arena_slack(), t.arena_capacity());
    EXPECT_EQ(t.arena_size() + t.arena_slack(), t.arena_capacity());

    if(t.empty())
        return;

    size_t count = test_tree_invariants(t.rootref());
    EXPECT_EQ(count, t.size());

    test_invariants(t.rootref());
    check_invariants(t);

    if(testing::Test::HasFailure())
    {
        print_tree(t);
    }

    return;
#if 0 == 1  // TODO
    for(size_t i = 0; i < t.m_size; ++i)
    {
        auto n = t.get(i);
        if(n->m_prev_sibling == NONE)
        {
            EXPECT_TRUE(i == t.m_head || i == t.m_free_head);
        }
        if(n->m_next_sibling == NONE)
        {
            EXPECT_TRUE(i == t.m_tail || i == t.m_free_tail);
        }
    }

    std::vector<bool> touched(t.capacity());

    for(size_t i = t.m_head; i != NONE; i = t.get(i)->m_next_sibling)
        touched[i] = true;

    size_t size = 0;
    for(bool v : touched)
        size += v;

    EXPECT_EQ(size, t.size());

    touched.clear();
    touched.resize(t.capacity());

    for(size_t i = t.m_free_head; i != NONE; i = t.get(i)->m_next_sibling)
    {
        touched[i] = true;
    }

    size_t slack = 0;
    for(auto v : touched)
    {
        slack += v;
    }

    EXPECT_EQ(slack, t.slack());
    EXPECT_EQ(size+slack, t.capacity());

    // there are more checks to be done
#endif
}

} // namespace yml
} // namespace c4
