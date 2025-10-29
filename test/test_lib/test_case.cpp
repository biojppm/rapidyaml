#include "./test_lib/test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/yml/error.def.hpp"
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
extern void report_error_parse_impl(csubstr msg, Location loc, FILE *file);
#endif

std::string format_error_basic(csubstr msg, ErrorDataBasic const& errdata)
{
    std::string out;
    err_basic_format([&](csubstr s){
        out.append(s.str, s.len);
    }, msg, errdata);
    return out;
}

std::string format_error_parse(csubstr msg, ErrorDataParse const& errdata)
{
    std::string out;
    err_parse_format([&](csubstr s){
        out.append(s.str, s.len);
    }, msg, errdata);
    return out;
}

std::string format_error_visit(csubstr msg, ErrorDataVisit const& errdata)
{
    std::string out;
    err_visit_format([&](csubstr s){
        out.append(s.str, s.len);
    }, msg, errdata);
    return out;
}

struct ExpectedErrorBasic C4_IF_EXCEPTIONS_( : public std::exception, )
{
    std::string smsg;
    C4_IF_EXCEPTIONS_(const char *what() const noexcept override { return smsg.c_str(); }, )
    ExpectedErrorBasic() = default;
    ExpectedErrorBasic(csubstr msg, ErrorDataBasic const& errdata_)
        :
        C4_IF_EXCEPTIONS_(std::exception() C4_COMMA,)
        smsg(format_error_basic(msg, errdata_))
    {
    }
    ExpectedErrorBasic(std::string s)
        :
        C4_IF_EXCEPTIONS_(std::exception() C4_COMMA,)
        smsg(std::move(s))
    {
    }
};

struct ExpectedErrorParse : public ExpectedErrorBasic
{
    ErrorDataParse errdata;
    ExpectedErrorParse() = default;
    ExpectedErrorParse(csubstr msg, ErrorDataParse const& errdata_)
        : ExpectedErrorBasic(format_error_parse(msg, errdata_))
        , errdata(errdata_)
    {
    }
};

struct ExpectedErrorVisit : public ExpectedErrorBasic
{
    ErrorDataVisit errdata;
    ExpectedErrorVisit() = default;
    ExpectedErrorVisit(csubstr msg, ErrorDataVisit const& errdata_)
        : ExpectedErrorBasic(format_error_visit(msg, errdata_))
        , errdata(errdata_)
    {
    }
};


//-----------------------------------------------------------------------------
C4_IF_EXCEPTIONS_(
    ,
    std::jmp_buf s_jmp_env_expect_error;
    ExpectedErrorBasic s_jmp_err_basic = {};
    ExpectedErrorParse s_jmp_err_parse = {};
    ExpectedErrorVisit s_jmp_err_visit = {};
    )

ExpectError::ExpectError(Tree *tree, Location loc)
    : m_got_an_error(false)
    , m_tree(tree)
    , m_glob_prev(get_callbacks())
    , m_tree_prev(tree ? tree->callbacks() : m_glob_prev)
    , expected_location(loc)
{
    auto errb = [](csubstr msg, ErrorDataBasic const& errdata, void *this_) {
        _c4dbgp("called basic error callback!");
        ((ExpectError*)this_)->m_got_an_error = true; // assign in here to ensure the exception was thrown here
        C4_IF_EXCEPTIONS(
            throw ExpectedErrorBasic(msg, errdata);
            ,
            s_jmp_err_basic = ExpectedErrorBasic(msg, errdata);
            std::longjmp(s_jmp_env_expect_error, 1);
        );
        C4_UNREACHABLE_AFTER_ERR();
    };
    auto errp = [](csubstr msg, ErrorDataParse const& errdata, void *this_) {
        _c4dbgpf("called parse error callback! (withlocation={})", bool(errdata.ymlloc));
        ((ExpectError*)this_)->m_got_an_error = true; // assign in here to ensure the exception was thrown here
        C4_IF_EXCEPTIONS(
            throw ExpectedErrorParse(msg, errdata);
            ,
            s_jmp_err_parse = ExpectedErrorParse(msg, errdata);
            std::longjmp(s_jmp_env_expect_error, 1);
        );
        C4_UNREACHABLE_AFTER_ERR();
    };
    auto errv = [](csubstr msg, ErrorDataVisit const& errdata, void *this_) {
        _c4dbgp("called visit error callback!");
        ((ExpectError*)this_)->m_got_an_error = true; // assign in here to ensure the exception was thrown here
        C4_IF_EXCEPTIONS(
            throw ExpectedErrorVisit(msg, errdata);
            ,
            s_jmp_err_visit = ExpectedErrorVisit(msg, errdata);
            std::longjmp(s_jmp_env_expect_error, 1);
        );
        C4_UNREACHABLE_AFTER_ERR();
    };
    c4::yml::Callbacks tcb;
    c4::yml::Callbacks gcb;
    tcb.set_user_data((void*)this).set_error_basic(errb).set_error_parse(errp).set_error_visit(errv);
    gcb.set_user_data((void*)this).set_error_basic(errb).set_error_parse(errp).set_error_visit(errv);
    if(tree)
    {
        _c4dbgpf("setting error callback: tree err={}", c4::fmt::hex((void const*)&errp));
        tree->callbacks(tcb);
        EXPECT_EQ(tree->callbacks().m_error_basic, errb);
        EXPECT_EQ(tree->callbacks().m_error_parse, errp);
        EXPECT_EQ(tree->callbacks().m_error_visit, errv);
    }
    _c4dbgpf("setting error callback: global err={}", c4::fmt::hex((void const*)&errp));
    set_callbacks(gcb);
    EXPECT_EQ(get_callbacks().m_error_basic, errb);
    EXPECT_EQ(get_callbacks().m_error_parse, errp);
    EXPECT_EQ(get_callbacks().m_error_visit, errv);
}

ExpectError::~ExpectError()
{
    if(m_tree)
    {
        _c4dbgp("resetting error callback: tree");
        m_tree->callbacks(m_tree_prev);
    }
    _c4dbgp("resetting error callback: global");
    set_callbacks(m_tree_prev);
}

void ExpectError::check_success(Tree *tree, fntestref fn)
{
    Location expected_location = {};
    auto context = ExpectError(tree, expected_location);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected success");
        fn();
        _c4dbgp("check expected success: success!");
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedErrorParse const&), else)
    {
        FAIL() << "check expected success: failed!";
    }
    ASSERT_FALSE(context.m_got_an_error);
}

void ExpectError::check_error_basic(Tree const* tree, fntestref fn)
{
    check_error_basic(const_cast<Tree*>(tree), fn);
}

void ExpectError::check_error_parse(Tree const* tree, fntestref fn, Location const& expected_location)
{
    check_error_parse(const_cast<Tree*>(tree), fn, expected_location);
}

void ExpectError::check_error_visit(Tree const* tree, fntestref fn, id_type expected_id)
{
    check_error_visit(const_cast<Tree*>(tree), fn, expected_id);
}

void ExpectError::check_error_basic(Tree *tree, fntestref fn)
{
    auto context = ExpectError(tree);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected basic error");
        fn();
        _c4dbgp("check expected basic error: failed!");
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedErrorBasic const& e), else)
    {
        C4_IF_EXCEPTIONS_( , ExpectedErrorBasic const& e = s_jmp_err_basic);
        (void)e;
        #if defined(RYML_DBG)
        std::cout << "---------------\n";
        std::cout << "got an expected parse error:\n" << e.what() << "\n";
        std::cout << "---------------\n";
        #endif
    }
    C4_IF_EXCEPTIONS_(catch(...)
    {
        _c4dbgp("---------------\n"
                "got an unexpected exception!\n"
                "---------------\n");
    }, )
    EXPECT_TRUE(context.m_got_an_error);
}

void ExpectError::check_error_parse(Tree *tree, fntestref fn, Location const& expected_location)
{
    ExpectError context(tree, expected_location);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected parse error");
        fn();
        _c4dbgp("check expected parse error: failed!");
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedErrorParse const& e), else)
    {
        C4_IF_EXCEPTIONS_( , ExpectedErrorParse const& e = s_jmp_err_parse);
        (void)e;
        #if defined(RYML_DBG)
        std::cout << "---------------\n";
        std::cout << "got an expected parse error:\n" << e.what() << "\n";
        std::cout << "---------------\n";
        #endif
        if(expected_location)
        {
            _c4dbgp("checking expected location...");
            EXPECT_EQ(static_cast<bool>(e.errdata.ymlloc), static_cast<bool>(context.expected_location));
            if(context.expected_location.line != npos)
            {
                EXPECT_EQ(e.errdata.ymlloc.line, context.expected_location.line);
            }
            if(context.expected_location.col != npos)
            {
                EXPECT_EQ(e.errdata.ymlloc.col, context.expected_location.col);
            }
            if(context.expected_location.offset != npos)
            {
                EXPECT_EQ(e.errdata.ymlloc.offset, context.expected_location.offset);
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

void ExpectError::check_error_visit(Tree *tree, fntestref fn, id_type id)
{
    auto context = ExpectError(tree);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected visit error");
        fn();
        _c4dbgp("check expected visit error: failed!");
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedErrorVisit const& e), else)
    {
        C4_IF_EXCEPTIONS_( , ExpectedErrorVisit const& e = s_jmp_err_visit);
        (void)e;
        #if defined(RYML_DBG)
        std::cout << "---------------\n";
        std::cout << "got an expected visit error:\n" << e.what() << "\n";
        std::cout << "---------------\n";
        #endif
        EXPECT_EQ(e.errdata.tree, tree);
        if(id != npos)
        {
            EXPECT_EQ(e.errdata.node, id);
        }
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const& exc)
    {
        std::cout << "---------------\n";
        std::cout << "got an unexpected exception!:\n" << exc.what() << "\n";
        std::cout << "---------------\n";
    }, )
    C4_IF_EXCEPTIONS_(catch(...)
    {
        std::cout << "---------------\n";
        std::cout << "got an unexpected exception!\n";
        std::cout << "---------------\n";
    }, )
    EXPECT_TRUE(context.m_got_an_error);
}

void ExpectError::check_assert_basic(Tree *tree, fntestref fn)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error_basic(tree, fn);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    #endif
}

void ExpectError::check_assert_parse(Tree *tree, fntestref fn, Location const& expected_location)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error_parse(tree, fn, expected_location);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(expected_location);
    #endif
}

void ExpectError::check_assert_visit(Tree *tree, fntestref fn, id_type id)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error_visit(tree, fn, id);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(id);
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
            _RYML_ASSERT_BASIC(ret >= 0);
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
            _RYML_ASSERT_BASIC(ret >= 0);
            pos -= static_cast<size_t>(ret);
        }
        else if(p.has_parent())
        {
            pos = (size_t)p.parent().child_pos(p);
            int ret = snprintf(buf, 0, "/%zu", pos);
            _RYML_ASSERT_BASIC(ret >= 0);
            size_t tl = static_cast<size_t>(ret);
            _RYML_ASSERT_BASIC(pos >= tl);
            ret = snprintf(buf + static_cast<size_t>(pos - tl), tl, "/%zu", pos);
            _RYML_ASSERT_BASIC(ret >= 0);
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


void test_comment_invariants(Tree const& t, id_type id)
{
    id = id != NONE ? id : t.root_id();
    (void)id;
#ifdef RYML_WITH_COMMENTS
    SCOPED_TRACE("comment invariants");
    RYML_TRACE_FMT("id={}", id);
    if(t.m_comments_cap == 0)
    {
        ASSERT_EQ(t.m_comments_buf, nullptr);
        ASSERT_EQ(t.m_comments_size, 0);
        ASSERT_EQ(t._p(id)->m_first_comment, NONE);
        ASSERT_EQ(t._p(id)->m_last_comment, NONE);
        return;
    }
    ASSERT_NE(t.m_comments_buf, nullptr);
    ASSERT_LE(t.m_comments_size, t.m_comments_cap);

    struct comm_nfo { CommentType_e type; csubstr name; };
    const comm_nfo all_comments[] = {
        #define _c4comm(comm_symbol, bit) {COMM_##comm_symbol, #comm_symbol},
        _RYML_DEFINE_COMMENTS(_c4comm)
        #undef _c4comm
    };
    auto get_nfo = [&](CommentType_e type) {
        for(comm_nfo nfo : all_comments)
            if(nfo.type == type)
                return nfo;
        return comm_nfo{};
    };

    id_type count_iter = 0;
    NodeData const* node = t._p(id);
    if(node->m_first_comment == NONE || node->m_last_comment == NONE)
    {
        EXPECT_EQ(node->m_first_comment, NONE);
        EXPECT_EQ(node->m_last_comment, NONE);
    }
    else
    {
        ASSERT_LT(node->m_first_comment, t.m_comments_size);
        ASSERT_LT(node->m_last_comment, t.m_comments_size);
        EXPECT_EQ(t.m_comments_buf[node->m_first_comment].m_prev, NONE);
        EXPECT_EQ(t.m_comments_buf[node->m_last_comment].m_next, NONE);
        for(id_type cid = node->m_first_comment; cid != NONE; cid = t.m_comments_buf[cid].m_next)
        {
            CommentData const* comm = &t.m_comments_buf[cid];
            RYML_TRACE_FMT("cid={} {}({})", cid, get_nfo(comm->m_type).name, c4::fmt::hex((comment_data_type)comm->m_type));
            ASSERT_LT(cid, t.m_comments_size);
            EXPECT_EQ(t.comment(id, comm->m_type), comm);
            if(comm->m_prev != NONE)
            {
                EXPECT_NE(cid, node->m_first_comment);
                ASSERT_LT(comm->m_prev, t.m_comments_size);
                CommentData const* prev = &t.m_comments_buf[comm->m_prev];
                EXPECT_LT(prev->m_type, comm->m_type);
                EXPECT_EQ(prev->m_next, cid);
            }
            else
            {
                EXPECT_EQ(cid, node->m_first_comment);
            }
            if(comm->m_next != NONE)
            {
                EXPECT_NE(cid, node->m_last_comment);
                ASSERT_LT(comm->m_next, t.m_comments_size);
                CommentData const* next = &t.m_comments_buf[comm->m_next];
                EXPECT_GT(next->m_type, comm->m_type);
                EXPECT_EQ(next->m_prev, cid);
            }
            else
            {
                EXPECT_EQ(cid, node->m_last_comment);
            }
            ++count_iter;
        }
    }

    id_type count_get = 0;
    {
        CommentData const* prev = nullptr;
        comm_nfo nfo_prev = {COMM_NONE, "NONE"};
        for(comm_nfo nfo : all_comments)
        {
            RYML_TRACE_FMT("comm={}({})", nfo.name, c4::fmt::hex((comment_data_type)nfo.type));
            CommentData const* comm = t.comment(id, nfo.type);
            if(nfo_prev.type != COMM_NONE)
            {
                RYML_TRACE_FMT("ctype_prev={}({})", nfo_prev.name, c4::fmt::hex((comment_data_type)nfo_prev.type));
                ASSERT_GE(nfo.type, nfo_prev.type);
                EXPECT_EQ(comm, t.comment(id, prev, nfo.type));
            }
            else
            {
                EXPECT_EQ(comm, t.comment(id, prev, nfo.type));
            }
            count_get += !!comm;
            nfo_prev = nfo;
            prev = comm;
        }
    }
    EXPECT_EQ(count_iter, count_get);

    size_t num_types = C4_COUNTOF(all_comments);
    for(size_t i = 0; i < num_types; ++i)
    {
        comm_nfo nfo = all_comments[i];
        RYML_TRACE_FMT("comm[{}]={}({})", i, nfo.name, c4::fmt::hex((comment_data_type)nfo.type));
        ASSERT_EQ(nfo.type & (nfo.type - 1u), 0u); // must be power of two (ie, single bit)
        CommentType_e mask_lower_without = CommentType_e(nfo.type - 1u); // all bits up to this comment (exclusive)
        CommentType_e mask_lower_with    = CommentType_e((((uint32_t)nfo.type) << 1u) - 1u); // all bits up to this comment (inclusive)
        CommentType_e mask_upper_without = CommentType_e(COMM_ANY & ~mask_lower_with); // all bits
        CommentData const* comm = t.comment(id, nfo.type);
        if(comm)
        {
            EXPECT_NE(t.comment(id, mask_lower_without), comm);
            EXPECT_NE(t.comment(id, mask_upper_without), comm);
        }
        for(size_t j = 0; j < i; ++j)
        {
            comm_nfo lo = all_comments[j];
            comm_nfo hi = nfo;
            RYML_TRACE_FMT("lo[{}]={}({})", j, lo.name, c4::fmt::hex((comment_data_type)lo.type));
            RYML_TRACE_FMT("hi[{}]={}({})", i, hi.name, c4::fmt::hex((comment_data_type)hi.type));
            CommentData const* comm_lo = t.comment(id, lo.type);
            CommentData const* comm_hi = t.comment(id, hi.type);
            EXPECT_EQ(t.comment(id, comm_lo, hi.type), comm_hi);
        }
        for(size_t j = i+1; j < num_types; ++j)
        {
            comm_nfo lo = nfo;
            comm_nfo hi = all_comments[j];
            RYML_TRACE_FMT("lo[{}]={}({})", j, lo.name, c4::fmt::hex((comment_data_type)lo.type));
            RYML_TRACE_FMT("hi[{}]={}({})", i, hi.name, c4::fmt::hex((comment_data_type)hi.type));
            CommentData const* comm_lo = t.comment(id, lo.type);
            CommentData const* comm_hi = t.comment(id, hi.type);
            EXPECT_EQ(t.comment(id, comm_lo, hi.type), comm_hi);
        }
    }

    for(id_type child = t.first_child(id); child != NONE; child = t.next_sibling(id))
    {
        test_comment_invariants(t, child);
    }
#endif
}

void test_comment_invariants(ConstNodeRef const &n)
{
    ASSERT_TRUE(n.readable());
    test_comment_invariants(*n.tree(), n.id());
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
    // check sibling reciprocity
    for(ConstNodeRef s : n.siblings())
    {
        EXPECT_TRUE(n.has_sibling(s));
        EXPECT_TRUE(s.has_sibling(n));
        if(s.has_key() && !n.has_key()) { EXPECT_EQ(n.type(), NOTYPE); _RYML_WITH_COMMENTS(EXPECT_NE(n.comment(), nullptr)); }
        if(!s.has_key() && n.has_key()) { EXPECT_EQ(s.type(), NOTYPE); _RYML_WITH_COMMENTS(EXPECT_NE(s.comment(), nullptr)); }
        if(n.has_key() && s.has_key())
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
            if(ch.type() != NOTYPE)
            {
                EXPECT_TRUE(ch.has_key());
            }
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
    if(n.has_key())
    {
        if(n.is_key_quoted())
        {
            EXPECT_FALSE(n.key_is_null());
        }
        if(n.key_is_null())
        {
            EXPECT_FALSE(n.is_key_quoted());
        }
    }
    if(n.has_val() && n.is_val_quoted())
    {
        if(n.is_val_quoted())
        {
            EXPECT_FALSE(n.val_is_null());
        }
        if(n.val_is_null())
        {
            EXPECT_FALSE(n.is_val_quoted());
        }
    }
    // ... add more tests here

    // now recurse into the children
    for(ConstNodeRef ch : n.children())
    {
        test_invariants(ch);
    }

    #undef _MORE_INFO
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

    check_invariants(t);
    test_invariants(t.rootref());
    test_comment_invariants(t);

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
        _RYML_CHECK_BASIC(c->src.find("\n\r") == csubstr::npos);
        {
            std::string tmp;
            replace_all("\r", "", c->src, &tmp);
            cd->unix_style.assign(to_csubstr(tmp));
            cd->unix_style_json.assign(to_csubstr(tmp));
        }
        {
            std::string tmp;
            replace_all("\n", "\r\n", cd->unix_style.src, &tmp);
            cd->windows_style.assign(to_csubstr(tmp));
            cd->windows_style_json.assign(to_csubstr(tmp));
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
