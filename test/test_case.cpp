#include "./test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/format.hpp"
#include "c4/span.hpp"
#include "c4/yml/std/std.hpp"
#include "c4/yml/detail/print.hpp"
#include "c4/yml/detail/checks.hpp"
#endif

#include <gtest/gtest.h>
#ifndef C4_EXCEPTIONS
#include <csetjmp>
#endif

#if defined(_MSC_VER)
#   pragma warning(push)
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wuseless-cast"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   if __GNUC__ >= 6
#       pragma GCC diagnostic ignored "-Wnull-dereference"
#   endif
#endif

namespace c4 {
namespace yml {


size_t _num_leaves(Tree const& t, size_t node)
{
    size_t count = 0;
    for(size_t ch = t.first_child(node); ch != NONE; ch = t.next_sibling(ch))
        count += _num_leaves(t, ch);
    return count;
}


void test_compare(Tree const& actual, Tree const& expected)
{
    ASSERT_EQ(actual.empty(), expected.empty());
    if(actual.empty() || expected.empty())
        return;
    EXPECT_EQ(actual.size(), expected.size());
    EXPECT_EQ(_num_leaves(actual, actual.root_id()), _num_leaves(expected, expected.root_id()));
    test_compare(actual, actual.root_id(), expected, expected.root_id(), 0);
}


void test_compare(Tree const& actual, size_t node_actual,
     Tree const& expected, size_t node_expected,
     size_t level)
{
    #define _MORE_INFO "actual=" << node_actual << " vs expected=" << node_expected

    ASSERT_NE(node_actual, (size_t)NONE);
    ASSERT_NE(node_expected, (size_t)NONE);
    ASSERT_LT(node_actual, actual.capacity());
    ASSERT_LT(node_expected, expected.capacity());

    EXPECT_EQ((type_bits)(actual.type(node_actual)&_TYMASK), (type_bits)(expected.type(node_expected)&_TYMASK)) << _MORE_INFO;

    EXPECT_EQ(actual.has_key(node_actual), expected.has_key(node_expected)) << _MORE_INFO;
    if(actual.has_key(node_actual) && expected.has_key(node_expected))
    {
        EXPECT_EQ(actual.key(node_actual), expected.key(node_expected)) << _MORE_INFO;
    }

    EXPECT_EQ(actual.has_val(node_actual), expected.has_val(node_expected)) << _MORE_INFO;
    if(actual.has_val(node_actual) && expected.has_val(node_expected))
    {
        EXPECT_EQ(actual.val(node_actual), expected.val(node_expected)) << _MORE_INFO;
    }

    EXPECT_EQ(actual.has_key_tag(node_actual), expected.has_key_tag(node_expected)) << _MORE_INFO;
    if(actual.has_key_tag(node_actual) && expected.has_key_tag(node_expected))
    {
        EXPECT_EQ(actual.key_tag(node_actual), expected.key_tag(node_expected)) << _MORE_INFO;
    }

    EXPECT_EQ(actual.has_val_tag(node_actual), expected.has_val_tag(node_expected)) << _MORE_INFO;
    if(actual.has_val_tag(node_actual) && expected.has_val_tag(node_expected))
    {
        auto filtered = [](csubstr tag) {
            if(tag.begins_with("!<!") && tag.ends_with('>'))
                return tag.offs(3, 1);
            return tag;
        };
        csubstr actual_tag = filtered(actual.val_tag(node_actual));
        csubstr expected_tag = filtered(actual.val_tag(node_expected));
        EXPECT_EQ(actual_tag, expected_tag) << _MORE_INFO;
    }

    EXPECT_EQ(actual.has_key_anchor(node_actual), expected.has_key_anchor(node_expected)) << _MORE_INFO;
    if(actual.has_key_anchor(node_actual) && expected.has_key_anchor(node_expected))
    {
        EXPECT_EQ(actual.key_anchor(node_actual), expected.key_anchor(node_expected)) << _MORE_INFO;
    }

    EXPECT_EQ(actual.has_val_anchor(node_actual), expected.has_val_anchor(node_expected)) << _MORE_INFO;
    if(actual.has_val_anchor(node_actual) && expected.has_val_anchor(node_expected))
    {
        EXPECT_EQ(actual.val_anchor(node_actual), expected.val_anchor(node_expected)) << _MORE_INFO;
    }

    EXPECT_EQ(actual.num_children(node_actual), expected.num_children(node_expected)) << _MORE_INFO;
    for(size_t ia = actual.first_child(node_actual), ib = expected.first_child(node_expected);
        ia != NONE && ib != NONE;
        ia = actual.next_sibling(ia), ib = expected.next_sibling(ib))
    {
        test_compare(actual, ia, expected, ib, level+1);
    }

    #undef _MORE_INFO
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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ensure coverage of the default callback report
#ifndef RYML_NO_DEFAULT_CALLBACKS
extern void report_error_impl(const char* msg, size_t len, Location loc, FILE *file);
#endif

std::string format_error(const char* msg, size_t len, Location loc)
{
    // ensure coverage of the default callback report
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    report_error_impl(msg, len, loc, nullptr);
    #endif
    if(!loc) return msg;
    std::string out;
    if(!loc.name.empty()) c4::formatrs_append(&out, "{}:", loc.name);
    c4::formatrs_append(&out, "{}:{}:", loc.line, loc.col);
    if(loc.offset) c4::formatrs_append(&out, " (@{}B):", loc.offset);
    c4::formatrs_append(&out, "{}:", csubstr(msg, len));
    return out;
}

struct ExpectedError C4_IF_EXCEPTIONS_( : public std::runtime_error, )
{
    C4_IF_EXCEPTIONS_( ,
        std::string smsg;
        const char *what() const { return smsg.c_str(); }
        ExpectedError() = default;
    )
    Location error_location;
    ExpectedError(const char* msg, size_t len, Location loc)
        : C4_IF_EXCEPTIONS_(std::runtime_error(format_error(msg, len, loc)),
                            smsg(format_error(msg, len, loc)))
        , error_location(loc)
    {
    }
};


//-----------------------------------------------------------------------------
C4_IF_EXCEPTIONS_(
    ,
    std::jmp_buf s_jmp_env_expect_error = {};
    ExpectedError s_jmp_err = {};
    )

ExpectError::ExpectError(Tree *tree, Location loc)
    : m_got_an_error(false)
    , m_tree(tree)
    , m_glob_prev(get_callbacks())
    , m_tree_prev(tree ? tree->callbacks() : Callbacks{})
    , expected_location(loc)
{
    auto err = [](const char* msg, size_t len, Location errloc, void *this_) {
        ((ExpectError*)this_)->m_got_an_error = true;
        C4_IF_EXCEPTIONS(
            throw ExpectedError(msg, len, errloc);
            ,
            s_jmp_err = ExpectedError(msg, len, errloc);
            std::longjmp(s_jmp_env_expect_error, 1);
        );
    };
    #ifdef RYML_NO_DEFAULT_CALLBACKS
    c4::yml::Callbacks tcb((void*)this, nullptr, nullptr, err);
    c4::yml::Callbacks gcb((void*)this, nullptr, nullptr, err);
    #else
    c4::yml::Callbacks tcb((void*)this, tree ? m_tree_prev.m_allocate : nullptr, tree ? m_tree_prev.m_free : nullptr, err);
    c4::yml::Callbacks gcb((void*)this, m_glob_prev.m_allocate, m_glob_prev.m_free, err);
    #endif
    if(tree)
        tree->callbacks(tcb);
    set_callbacks(gcb);
}

ExpectError::~ExpectError()
{
    if(m_tree)
        m_tree->callbacks(m_tree_prev);
    set_callbacks(m_tree_prev);
}

void ExpectError::do_check(Tree *tree, std::function<void()> fn, Location expected_location)
{
    auto context = ExpectError(tree, expected_location);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        fn();
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedError const& e), else)
    {
        C4_IF_EXCEPTIONS_( , ExpectedError const& e = s_jmp_err);
        #if defined(RYML_DBG)
        std::cout << "---------------\n";
        std::cout << "got an expected error:\n" << e.what() << "\n";
        std::cout << "---------------\n";
        #endif
        if(context.expected_location)
        {
            EXPECT_EQ(static_cast<bool>(context.expected_location),
                      static_cast<bool>(e.error_location));
            EXPECT_EQ(e.error_location.line, context.expected_location.line);
            EXPECT_EQ(e.error_location.col, context.expected_location.col);
            if(context.expected_location.offset)
            {
                EXPECT_EQ(e.error_location.offset, context.expected_location.offset);
            }
        }
    };
    EXPECT_TRUE(context.m_got_an_error);
}

void ExpectError::check_assertion(Tree *tree, std::function<void()> fn, Location expected_location)
{
    #if RYML_USE_ASSERT
    ExpectError::do_check(tree, fn, expected_location);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(expected_location);
    #endif
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

using N = CaseNode;
using L = CaseNode::iseqmap;

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
NodeType_e CaseNode::_guess() const
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
void CaseNode::compare_child(yml::ConstNodeRef const& n, size_t pos) const
{
    EXPECT_TRUE(pos < n.num_children());
    EXPECT_TRUE(pos < children.size());

    if(pos >= n.num_children() || pos >= children.size()) return;

    ASSERT_GT(n.num_children(), pos);
    auto const& expectedch = children[pos];

    if(type & MAP)
    {
        auto actualch = n.find_child(expectedch.key);
        if(actualch != nullptr)
        {
            // there may be duplicate keys.
            if(actualch.id() != n[pos].id())
                actualch = n[pos];
            //EXPECT_EQ(fch, n[ch.key]);
            EXPECT_EQ(actualch.get(), n[pos].get());
            //EXPECT_EQ(n[pos], n[ch.key]);
            EXPECT_EQ(n[expectedch.key].key(), expectedch.key);
        }
        else
        {
            printf("error: node should have child %.*s: ", (int)expectedch.key.len, expectedch.key.str);
            print_path(n);
            printf("\n");
            print_node(n);
            GTEST_FAIL();
        }
    }

    if(type & SEQ)
    {
        EXPECT_FALSE(n[pos].has_key());
        EXPECT_EQ(n[pos].get()->m_key.scalar, children[pos].key);
        auto actualch = n.child(pos);
        EXPECT_EQ(actualch.get(), n[pos].get());
    }

    if(expectedch.type & KEY)
    {
        auto actualfch = n[pos];
        EXPECT_TRUE(actualfch.has_key()) << "id=" << actualfch.id();
        if(actualfch.has_key())
        {
            EXPECT_EQ(actualfch.key(), expectedch.key) << "id=" << actualfch.id();
        }

        if( ! expectedch.key_tag.empty())
        {
            EXPECT_TRUE(actualfch.has_key_tag()) << "id=" << actualfch.id();
            if(actualfch.has_key_tag())
            {
                EXPECT_EQ(actualfch.key_tag(), expectedch.key_tag) << "id=" << actualfch.id();
            }
        }
    }

    if(expectedch.type & VAL)
    {
        auto actualch = n[pos];
        EXPECT_TRUE(actualch.has_val()) << "id=" << actualch.id();
        if(actualch.has_val())
        {
            EXPECT_EQ(actualch.val(), expectedch.val) << "id=" << actualch.id();
        }

        if( ! expectedch.val_tag.empty())
        {
            EXPECT_TRUE(actualch.has_val_tag()) << "id=" << actualch.id();
            if(actualch.has_val_tag())
            {
                EXPECT_EQ(actualch.val_tag(), expectedch.val_tag) << "id=" << actualch.id();
            }
        }
    }
}

void CaseNode::compare(yml::ConstNodeRef const& actual, bool ignore_quote) const
{
    if(ignore_quote)
    {
        const auto actual_type   = actual.get()->m_type & ~(VALQUO | KEYQUO);
        const auto expected_type = type & ~(VALQUO | KEYQUO);
        EXPECT_EQ(expected_type, actual_type) << "id=" << actual.id();
    }
    else
    {
        EXPECT_EQ((int)actual.get()->m_type, (int)type) << "id=" << actual.id(); // the type() method masks the type, and thus tag flags are omitted on its return value
    }

    EXPECT_EQ(actual.num_children(), children.size()) << "id=" << actual.id();

    if(actual.has_key())
    {
        EXPECT_EQ(actual.key(), key) << "id=" << actual.id();
    }

    if(actual.has_val())
    {
        EXPECT_EQ(actual.val(), val) << "id=" << actual.id();
    }

    // check that the children are in the same order
    {
        EXPECT_EQ(children.size(), actual.num_children()) << "id=" << actual.id();

        size_t ic = 0;
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

        if(actual.first_child() != nullptr)
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

    for(size_t i = 0, ei = actual.num_children(), j = 0, ej = children.size(); i < ei && j < ej; ++i, ++j)
    {
        children[j].compare(actual[i], ignore_quote);
    }
}

void CaseNode::recreate(yml::NodeRef *n) const
{
    C4_ASSERT( ! n->has_children());
    auto *nd = n->get();
    nd->m_type = type|key_anchor.type|val_anchor.type;
    nd->m_key.scalar = key;
    nd->m_key.tag = (key_tag);
    nd->m_key.anchor = key_anchor.str;
    nd->m_val.scalar = val;
    nd->m_val.tag = (val_tag);
    nd->m_val.anchor = val_anchor.str;
    auto &tree = *n->tree();
    size_t nid = n->id(); // don't use node from now on
    for(auto const& ch : children)
    {
        size_t id = tree.append_child(nid);
        NodeRef chn(n->tree(), id);
        ch.recreate(&chn);
    }
}


//-----------------------------------------------------------------------------

void print_path(ConstNodeRef const& n)
{
    size_t len = 0;
    char buf[1024];
    ConstNodeRef p = n;
    while(p != nullptr)
    {
        if(p.has_key())
        {
            len += 1 + p.key().len;
        }
        else
        {
            int ret = snprintf(buf, sizeof(buf), "/%zd", p.has_parent() ? p.parent().child_pos(p) : 0);
            RYML_ASSERT(ret >= 0);
            len += static_cast<size_t>(ret);
        }
        p = p.parent();
    };
    C4_ASSERT(len < sizeof(buf));
    size_t pos = len;
    p = n;
    while(p.valid() && p != nullptr)
    {
        if(p.has_key())
        {
            size_t tl = p.key().len;
            int ret = snprintf(buf + pos - tl, tl, "%.*s", (int)tl, p.key().str);
            RYML_ASSERT(ret >= 0);
            pos -= static_cast<size_t>(ret);
        }
        else if(p.has_parent())
        {
            pos = p.parent().child_pos(p);
            int ret = snprintf(buf, 0, "/%zd", pos);
            RYML_ASSERT(ret >= 0);
            size_t tl = static_cast<size_t>(ret);
            RYML_ASSERT(pos >= tl);
            ret = snprintf(buf + static_cast<size_t>(pos - tl), tl, "/%zd", pos);
            RYML_ASSERT(ret >= 0);
            pos -= static_cast<size_t>(ret);
        }
        p = p.parent();
    };
    printf("%.*s", (int)len, buf);
}



void print_node(CaseNode const& p, int level)
{
    printf("%*s%p", (2*level), "", (void const*)&p);
    if( ! p.parent)
    {
        printf(" [ROOT]");
    }
    printf(" %s:", NodeType::type_str(p.type));
    if(p.has_key())
    {
        if(p.has_key_anchor())
        {
            csubstr ka = p.key_anchor.str;
            printf(" &%.*s", (int)ka.len, ka.str);
        }
        if(p.key_tag.empty())
        {
            csubstr v  = p.key;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            csubstr vt = p.key_tag;
            csubstr v  = p.key;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    if(p.has_val())
    {
        if(p.val_tag.empty())
        {
            csubstr v  = p.val;
            printf(" '%.*s'", (int)v.len, v.str);
        }
        else
        {
            csubstr vt = p.val_tag;
            csubstr v  = p.val;
            printf(" '%.*s %.*s'", (int)vt.len, vt.str, (int)v.len, v.str);
        }
    }
    else
    {
        if( ! p.val_tag.empty())
        {
            csubstr vt = p.val_tag;
            printf(" %.*s", (int)vt.len, vt.str);
        }
    }
    if(p.has_val_anchor())
    {
        auto &a = p.val_anchor.str;
        printf(" valanchor='&%.*s'", (int)a.len, a.str);
    }
    printf(" (%zd sibs)", p.parent ? p.parent->children.size() : 0);
    if(p.is_container())
    {
        printf(" %zd children:", p.children.size());
    }
    printf("\n");
}


void print_tree(ConstNodeRef const& p, int level)
{
    print_node(p, level);
    for(ConstNodeRef ch : p.children())
    {
        print_tree(ch, level+1);
    }
}

void print_tree(CaseNode const& p, int level)
{
    print_node(p, level);
    for(auto const& ch : p.children)
        print_tree(ch, level+1);
}

void print_tree(CaseNode const& t)
{
    printf("--------------------------------------\n");
    print_tree(t, 0);
    printf("#nodes: %zd\n", t.reccount());
    printf("--------------------------------------\n");
}

void test_invariants(ConstNodeRef const& n)
{
    #define _MORE_INFO << "id=" << n.id()

    if(n.is_root())
    {
        EXPECT_FALSE(n.has_other_siblings()) _MORE_INFO;
    }
    // keys or vals cannot be root
    if(n.has_key() || n.is_val() || n.is_keyval())
    {
        EXPECT_TRUE(!n.is_root() || (n.is_doc() && !n.has_key())) _MORE_INFO;
    }
    // vals cannot be containers
    if( ! n.empty() && ! n.is_doc())
    {
        EXPECT_NE(n.has_val(), n.is_container()) _MORE_INFO;
    }
    if(n.has_children())
    {
        EXPECT_TRUE(n.is_container()) _MORE_INFO;
        EXPECT_FALSE(n.is_val()) _MORE_INFO;
    }
    // check parent & sibling reciprocity
    for(ConstNodeRef s : n.siblings())
    {
        EXPECT_TRUE(n.has_sibling(s)) _MORE_INFO;
        EXPECT_TRUE(s.has_sibling(n)) _MORE_INFO;
        EXPECT_EQ(s.parent().get(), n.parent().get()) _MORE_INFO;
    }
    if(n.parent() != nullptr)
    {
        EXPECT_EQ(n.parent().num_children() > 1, n.has_other_siblings()) _MORE_INFO;
        EXPECT_TRUE(n.parent().has_child(n)) _MORE_INFO;
        EXPECT_EQ(n.parent().num_children(), n.num_siblings()) _MORE_INFO;
        // doc parent must be a seq and a stream
        if(n.is_doc())
        {
            EXPECT_TRUE(n.parent().is_seq()) _MORE_INFO;
            EXPECT_TRUE(n.parent().is_stream()) _MORE_INFO;
        }
    }
    else
    {
        EXPECT_TRUE(n.is_root()) _MORE_INFO;
    }
    if(n.is_seq())
    {
        EXPECT_TRUE(n.is_container()) _MORE_INFO;
        EXPECT_FALSE(n.is_map()) _MORE_INFO;
        for(ConstNodeRef ch : n.children())
        {
            EXPECT_FALSE(ch.is_keyval()) _MORE_INFO;
            EXPECT_FALSE(ch.has_key()) _MORE_INFO;
        }
    }
    if(n.is_map())
    {
        EXPECT_TRUE(n.is_container()) _MORE_INFO;
        EXPECT_FALSE(n.is_seq()) _MORE_INFO;
        for(ConstNodeRef ch : n.children())
        {
            EXPECT_TRUE(ch.has_key()) _MORE_INFO;
        }
    }
    if(n.has_key_anchor())
    {
        EXPECT_FALSE(n.key_anchor().empty()) _MORE_INFO;
        EXPECT_FALSE(n.is_key_ref()) _MORE_INFO;
    }
    if(n.has_val_anchor())
    {
        EXPECT_FALSE(n.val_anchor().empty()) _MORE_INFO;
        EXPECT_FALSE(n.is_val_ref()) _MORE_INFO;
    }
    if(n.is_key_ref())
    {
        EXPECT_FALSE(n.key_ref().empty()) _MORE_INFO;
        EXPECT_FALSE(n.has_key_anchor()) _MORE_INFO;
    }
    if(n.is_val_ref())
    {
        EXPECT_FALSE(n.val_ref().empty()) _MORE_INFO;
        EXPECT_FALSE(n.has_val_anchor()) _MORE_INFO;
    }
    // ... add more tests here

    // now recurse into the children
    for(ConstNodeRef ch : n.children())
    {
        test_invariants(ch);
    }

    #undef _MORE_INFO
}

size_t test_tree_invariants(ConstNodeRef const& n)
{
    auto parent = n.parent();

    if(n.get()->m_prev_sibling == NONE)
    {
        if(parent != nullptr)
        {
            EXPECT_EQ(parent.first_child().get(), n.get());
            EXPECT_EQ(parent.first_child().id(), n.id());
        }
    }

    if(n.get()->m_next_sibling == NONE)
    {
        if(parent != nullptr)
        {
            EXPECT_EQ(parent.last_child().get(), n.get());
            EXPECT_EQ(parent.last_child().id(), n.id());
        }
    }

    if(parent == nullptr)
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

    ASSERT_LE(t.size(), t.capacity());
    EXPECT_EQ(t.size() + t.slack(), t.capacity());

    ASSERT_LE(t.arena_size(), t.arena_capacity());
    ASSERT_LE(t.arena_slack(), t.arena_capacity());
    EXPECT_EQ(t.arena_size() + t.arena_slack(), t.arena_capacity());

    if(t.empty())
        return;

    size_t count = test_tree_invariants(t.rootref());
    EXPECT_EQ(count, t.size());

    check_invariants(t);
    test_invariants(t.rootref());

    if(!testing::UnitTest::GetInstance()->current_test_info()->result()->Passed())
    {
        print_tree(t);
    }

    return;
#if 0 == 1
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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CaseData* get_data(csubstr name)
{
    static std::map<csubstr, CaseData> m;

    auto it = m.find(name);
    CaseData *cd;
    if(it == m.end())
    {
        cd = &m[name];
        Case const* c = get_case(name);
        RYML_CHECK(c->src.find("\n\r") == csubstr::npos);
        {
            std::string tmp;
            replace_all("\r", "", c->src, &tmp);
            cd->unix_style.src_buf.assign(tmp.begin(), tmp.end());
            cd->unix_style.src = to_substr(cd->unix_style.src_buf);
            cd->unix_style_json.src_buf.assign(tmp.begin(), tmp.end());
            cd->unix_style_json.src = to_substr(cd->unix_style.src_buf);
        }
        {
            std::string tmp;
            replace_all("\n", "\r\n", cd->unix_style.src, &tmp);
            cd->windows_style.src_buf.assign(tmp.begin(), tmp.end());
            cd->windows_style.src = to_substr(cd->windows_style.src_buf);
            cd->windows_style_json.src_buf.assign(tmp.begin(), tmp.end());
            cd->windows_style_json.src = to_substr(cd->windows_style.src_buf);
        }
    }
    else
    {
        cd = &it->second;
    }
    return cd;
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
