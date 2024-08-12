#include "./test_lib/test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/format.hpp"
#include "c4/span.hpp"
#include "c4/yml/std/std.hpp"
#include "c4/yml/detail/print.hpp"
#include "c4/yml/detail/checks.hpp"
#endif

#include <gtest/gtest.h>
#ifdef C4_EXCEPTIONS
#include <exception>
#else
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
#   if __GNUC__ >= 7
#       pragma GCC diagnostic ignored "-Wstringop-overflow"
#   endif
#endif


namespace c4 {
namespace yml {


id_type _num_leaves(Tree const& t, id_type node)
{
    id_type count = 0;
    for(id_type ch = t.first_child(node); ch != NONE; ch = t.next_sibling(ch))
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


void test_compare(Tree const& actual, id_type node_actual,
     Tree const& expected, id_type node_expected,
     id_type level)
{
    RYML_TRACE_FMT("actual={} vs expected={}", node_actual, node_expected);

    ASSERT_NE(node_actual, (id_type)NONE);
    ASSERT_NE(node_expected, (id_type)NONE);
    ASSERT_LT(node_actual, actual.capacity());
    ASSERT_LT(node_expected, expected.capacity());

    NodeType type_actual = actual.type(node_actual)&_TYMASK;
    NodeType type_expected = expected.type(node_expected)&_TYMASK;
    RYML_COMPARE_NODE_TYPE(type_actual, type_expected, ==, EQ);
    //EXPECT_EQ((type_bits)(actual.type(node_actual)&_TYMASK), (type_bits)(expected.type(node_expected)&_TYMASK));

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
        auto filtered = [](csubstr tag) {
            if(tag.begins_with("!<!") && tag.ends_with('>'))
                return tag.offs(3, 1);
            return tag;
        };
        csubstr actual_tag = filtered(actual.val_tag(node_actual));
        csubstr expected_tag = filtered(actual.val_tag(node_expected));
        EXPECT_EQ(actual_tag, expected_tag);
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
        test_compare(actual, ia, expected, ib, level+1);
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
    std::string out;
    if(!loc)
    {
        out.assign(msg, len);
        return out;
    }
    if(!loc.name.empty())
        c4::formatrs_append(&out, "{}:", loc.name);
    c4::formatrs_append(&out, "{}:{}:", loc.line, loc.col);
    if(loc.offset)
        c4::formatrs_append(&out, " (@{}B):", loc.offset);
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
    auto err = [](const char* msg, size_t len, Location errloc, void *this_)  {
        _c4dbgpf("called error callback! (withlocation={})", bool(errloc));
        ((ExpectError*)this_)->m_got_an_error = true; // assign in here to ensure the exception was thrown here
        C4_IF_EXCEPTIONS(
            throw ExpectedError(msg, len, errloc);
            ,
            s_jmp_err = ExpectedError(msg, len, errloc);
            std::longjmp(s_jmp_env_expect_error, 1);
        );
        C4_UNREACHABLE_AFTER_ERR();
    };
    #ifdef RYML_NO_DEFAULT_CALLBACKS
    c4::yml::Callbacks tcb((void*)this, nullptr, nullptr, err);
    c4::yml::Callbacks gcb((void*)this, nullptr, nullptr, err);
    #else
    c4::yml::Callbacks tcb((void*)this, tree ? m_tree_prev.m_allocate : nullptr, tree ? m_tree_prev.m_free : nullptr, err);
    c4::yml::Callbacks gcb((void*)this, m_glob_prev.m_allocate, m_glob_prev.m_free, err);
    #endif
    _c4dbgp("setting error callback");
    if(tree)
        tree->callbacks(tcb);
    set_callbacks(gcb);
}

ExpectError::~ExpectError()
{
    if(m_tree)
        m_tree->callbacks(m_tree_prev);
    set_callbacks(m_tree_prev);
    _c4dbgp("resetting error callback");
}

void ExpectError::check_success(Tree *tree, std::function<void()> fn)
{
    auto context = ExpectError(tree, {});
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        fn();
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedError const&), else)
    {
        ;
    }
    EXPECT_FALSE(context.m_got_an_error);
}

void ExpectError::check_error(Tree const* tree, std::function<void()> fn, Location expected_location)
{
    check_error(const_cast<Tree*>(tree), fn, expected_location);
}

void ExpectError::check_error(Tree *tree, std::function<void()> fn, Location expected_location)
{
    auto context = ExpectError(tree, expected_location);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected error");
        fn();
        _c4dbgp("check expected error: failed!");
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
            _c4dbgp("checking expected location...");
            EXPECT_EQ(static_cast<bool>(e.error_location), static_cast<bool>(context.expected_location));
            EXPECT_EQ(e.error_location.line, context.expected_location.line);
            EXPECT_EQ(e.error_location.col, context.expected_location.col);
            if(context.expected_location.offset)
            {
                EXPECT_EQ(e.error_location.offset, context.expected_location.offset);
            }
        }
    }
    C4_IF_EXCEPTIONS_(catch(...)
    {
        _c4dbgp("---------------\n"
                "got an unexpected exception!\n"
                "---------------\n");
    }, )
    EXPECT_TRUE(context.m_got_an_error);
}

void ExpectError::check_assertion(Tree *tree, std::function<void()> fn, Location expected_location)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error(tree, fn, expected_location);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(expected_location);
    #endif
}


//-----------------------------------------------------------------------------

void print_path(ConstNodeRef const& n)
{
    size_t len = 0;
    char buf[1024];
    ConstNodeRef p = n;
    while(p.readable())
    {
        if(p.has_key())
        {
            len += 1 + p.key().len;
        }
        else
        {
            int ret = snprintf(buf, sizeof(buf), "/%zu", p.has_parent() ? (size_t)p.parent().child_pos(p) : (size_t)0);
            RYML_ASSERT(ret >= 0);
            len += static_cast<size_t>(ret);
        }
        p = p.parent();
    };
    C4_ASSERT(len < sizeof(buf));
    size_t pos = len;
    p = n;
    while(p.readable())
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
            pos = (size_t)p.parent().child_pos(p);
            int ret = snprintf(buf, 0, "/%zu", pos);
            RYML_ASSERT(ret >= 0);
            size_t tl = static_cast<size_t>(ret);
            RYML_ASSERT(pos >= tl);
            ret = snprintf(buf + static_cast<size_t>(pos - tl), tl, "/%zu", pos);
            RYML_ASSERT(ret >= 0);
            pos -= static_cast<size_t>(ret);
        }
        p = p.parent();
    };
    printf("%.*s", (int)len, buf);
}



void print_test_node(TestCaseNode const& p, int level)
{
    printf("%*s%p", (2*level), "", (void const*)&p);
    if( ! p.parent)
    {
        printf(" [ROOT]");
    }
    printf(" %s:", NodeType::type_str(p.type));
    if(p.has_key())
    {
        const char code = _scalar_code_key(p.type);
        if(p.has_key_anchor())
        {
            csubstr ka = p.key_anchor.str;
            printf(" &%.*s", (int)ka.len, ka.str);
        }
        if(p.key_tag.empty())
        {
            csubstr v  = p.key;
            printf(" %c%.*s%c", code, (int)v.len, v.str, code);
        }
        else
        {
            csubstr vt = p.key_tag;
            csubstr v  = p.key;
            printf(" %.*s %c%.*s%c'", (int)vt.len, vt.str, code, (int)v.len, v.str, code);
        }
    }
    if(p.has_val())
    {
        const char code = _scalar_code_val(p.type);
        if(p.val_tag.empty())
        {
            csubstr v  = p.val;
            printf(" %c%.*s%c", code, (int)v.len, v.str, code);
        }
        else
        {
            csubstr vt = p.val_tag;
            csubstr v  = p.val;
            printf(" %.*s%c%.*s%c", (int)vt.len, vt.str, code, (int)v.len, v.str, code);
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


void print_test_tree(TestCaseNode const& p, int level)
{
    print_test_node(p, level);
    for(auto const& ch : p.children)
        print_test_tree(ch, level+1);
}

void print_test_tree(const char *message, TestCaseNode const& t)
{
    printf("--------------------------------------\n");
    if(message != nullptr)
        printf("%s:\n", message);
    print_test_tree(t, 0);
    printf("#nodes: %zu\n", (size_t)t.reccount());
    printf("--------------------------------------\n");
}

void test_invariants(ConstNodeRef const& n)
{
    SCOPED_TRACE(n.id());
    if(n.is_root())
    {
        EXPECT_FALSE(n.has_other_siblings());
    }
    // vals cannot be containers
    if( ! n.empty() && ! n.is_doc())
    {
        EXPECT_NE(n.has_val(), n.is_container());
    }
    if(n.has_children())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_val());
    }
    // check parent & sibling reciprocity
    for(ConstNodeRef s : n.siblings())
    {
        EXPECT_TRUE(n.has_sibling(s));
        EXPECT_TRUE(s.has_sibling(n));
        if(n.has_key())
        {
            EXPECT_TRUE(n.has_sibling(s.key()));
            EXPECT_TRUE(s.has_sibling(n.key()));
        }
        EXPECT_EQ(s.parent().get(), n.parent().get());
    }
    if(n.parent().readable())
    {
        EXPECT_EQ(n.parent().num_children() > 1, n.has_other_siblings());
        EXPECT_TRUE(n.parent().has_child(n));
        EXPECT_EQ(n.parent().num_children(), n.num_siblings());
        EXPECT_EQ(n.parent().num_children(), n.num_other_siblings()+id_type(1));
        // doc parent must be a seq and a stream
        if(n.is_doc())
        {
            EXPECT_TRUE(n.parent().is_seq());
            EXPECT_TRUE(n.parent().is_stream());
        }
    }
    else
    {
        EXPECT_TRUE(n.is_root());
    }
    if(n.is_seq())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_map());
        for(ConstNodeRef ch : n.children())
        {
            EXPECT_FALSE(ch.is_keyval());
            EXPECT_FALSE(ch.has_key());
        }
    }
    if(n.is_map())
    {
        EXPECT_TRUE(n.is_container());
        EXPECT_FALSE(n.is_seq());
        for(ConstNodeRef ch : n.children())
        {
            EXPECT_TRUE(ch.has_key());
        }
    }
    if(n.has_key_anchor())
    {
        EXPECT_FALSE(n.key_anchor().empty());
        EXPECT_FALSE(n.is_key_ref());
    }
    if(n.has_val_anchor())
    {
        EXPECT_FALSE(n.val_anchor().empty());
        EXPECT_FALSE(n.is_val_ref());
    }
    if(n.is_key_ref())
    {
        EXPECT_FALSE(n.key_ref().empty());
        EXPECT_FALSE(n.has_key_anchor());
    }
    if(n.is_val_ref())
    {
        EXPECT_FALSE(n.val_ref().empty());
        EXPECT_FALSE(n.has_val_anchor());
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
