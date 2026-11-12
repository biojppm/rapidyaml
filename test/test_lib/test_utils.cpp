#include "test_lib/test_utils.hpp"

#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/yml/error.def.hpp"
#include "c4/yml/std/std.hpp"
#include "c4/yml/detail/print.hpp"
#include "c4/yml/detail/dbgprint.hpp"
#include "c4/yml/detail/checks.hpp"
#endif

#include <gtest/gtest.h>
#ifdef C4_EXCEPTIONS
#include <exception>
#else
#include <csetjmp>
#endif

namespace c4 {
namespace yml {

const bomspec bomspecs_[] = {
    //                        bare string causes problems in gcc5 and earlier
    {"NOBOM"       , UTF8   , true , csubstr("", size_t(0))},
    {"UTF8"        , UTF8   , true , csubstr("\xef\xbb\xbf", 3)},
    {"UTF16BE"     , UTF16BE, false, csubstr("\xfe\xff", 2)},
    {"!UTF16BE-a"  , UTF16BE, false, csubstr("\x00""a", 2)},
    {"!UTF16BE-b"  , UTF16BE, false, csubstr("\x00""b", 2)},
    {"!UTF16BE-0"  , UTF16BE, false, csubstr("\x00""0", 2)},
    {"UTF16LE"     , UTF16LE, false, csubstr("\xff\xfe", 2)},
    {"!UTF16LE-a"  , UTF16LE, false, csubstr("a""\x00" , 2)},
    {"!UTF16LE-b"  , UTF16LE, false, csubstr("b""\x00" , 2)},
    {"!UTF16LE-0"  , UTF16LE, false, csubstr("0""\x00" , 2)},
    {"UTF32BE"     , UTF32BE, false, csubstr("\x00\x00\xfe\xff", 4)},
    {"!UTF32BE-a"  , UTF32BE, false, csubstr("\x00\x00\x00""a" , 4)},
    {"!UTF32BE-b"  , UTF32BE, false, csubstr("\x00\x00\x00""b" , 4)},
    {"!UTF32BE-0"  , UTF32BE, false, csubstr("\x00\x00\x00""0" , 4)},
    {"UTF32LE"     , UTF32LE, false, csubstr("\xff\xfe\x00\x00", 4)},
    {"!UTF32LE-a"  , UTF32LE, false, csubstr("a""\x00\x00\x00" , 4)},
    {"!UTF32LE-b"  , UTF32LE, false, csubstr("b""\x00\x00\x00" , 4)},
    {"!UTF32LE-0"  , UTF32LE, false, csubstr("0""\x00\x00\x00" , 4)},
};
const cspan<bomspec> bomspecs = bomspecs_;


// ensure coverage of the default callback report
#ifndef RYML_NO_DEFAULT_CALLBACKS
extern void report_error_parse_impl(csubstr msg, Location loc, FILE *file);
#endif

namespace {
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
} // namespace


//-----------------------------------------------------------------------------
namespace {

C4_IF_EXCEPTIONS_(
    ,
    std::jmp_buf s_jmp_env_expect_error;
    ExpectedErrorBasic s_jmp_err_basic = {};
    ExpectedErrorParse s_jmp_err_parse = {};
    ExpectedErrorVisit s_jmp_err_visit = {};
    )

[[noreturn]] void experror_callback_basic(csubstr msg, ErrorDataBasic const& errdata, void *this_)
{
    _c4dbgp("called basic error callback! from here:");
    #ifdef RYML_DBG
    dbg_printf_("{}:{}: cpploc\n", errdata.location.name, errdata.location.line);
    dbg_printf_("{}:{}: {}\n", errdata.location.name, errdata.location.line, msg);
    #endif
    ((ExpectError*)this_)->m_error = ExpectedErrorType::err_basic; // assign in here to ensure the exception was thrown here
    C4_IF_EXCEPTIONS(
        throw ExpectedErrorBasic(msg, errdata);
        ,
        s_jmp_err_basic = ExpectedErrorBasic(msg, errdata);
        std::longjmp(s_jmp_env_expect_error, 1);
        );
    C4_UNREACHABLE_AFTER_ERR();
}

[[noreturn]] void experror_callback_parse(csubstr msg, ErrorDataParse const& errdata, void *this_)
{
    _c4dbgpf("called parse error callback! (withlocation={})", bool(errdata.ymlloc));
    #ifdef RYML_DBG
    dbg_printf_("{}:{}: cpploc\n", errdata.cpploc.name, errdata.cpploc.line);
    dbg_printf_("{}:{}: {}\n", errdata.ymlloc.name, errdata.ymlloc.line, msg);
    #endif
    ((ExpectError*)this_)->m_error = ExpectedErrorType::err_parse; // assign in here to ensure the exception was thrown here
    C4_IF_EXCEPTIONS(
        throw ExpectedErrorParse(msg, errdata);
        ,
        s_jmp_err_parse = ExpectedErrorParse(msg, errdata);
        std::longjmp(s_jmp_env_expect_error, 1);
    );
    C4_UNREACHABLE_AFTER_ERR();
}

[[noreturn]] void experror_callback_visit(csubstr msg, ErrorDataVisit const& errdata, void *this_)
{
    _c4dbgp("called visit error callback!");
    #ifdef RYML_DBG
    dbg_printf_("{}:{}: cpploc\n", errdata.cpploc.name, errdata.cpploc.line);
    dbg_printf_("{}:{}: {}\n", errdata.cpploc.name, errdata.cpploc.line, msg);
    #endif
    ((ExpectError*)this_)->m_error = ExpectedErrorType::err_visit; // assign in here to ensure the exception was thrown here
    C4_IF_EXCEPTIONS(
        throw ExpectedErrorVisit(msg, errdata);
        ,
        s_jmp_err_visit = ExpectedErrorVisit(msg, errdata);
        std::longjmp(s_jmp_env_expect_error, 1);
    );
    C4_UNREACHABLE_AFTER_ERR();
}
} // namespace

ExpectError::ExpectError(ExpectedErrorType errtype, Tree *tree, Location loc)
    : m_error(ExpectedErrorType::err_none)
    , m_expected_error(errtype)
    , m_tree(tree)
    , m_glob_prev(get_callbacks())
    , m_tree_prev(tree ? tree->callbacks() : m_glob_prev)
    , expected_location(loc)
{
    EXPECT_NE(m_glob_prev.m_allocate, nullptr);
    EXPECT_NE(m_tree_prev.m_allocate, nullptr);
    c4::yml::Callbacks tcb = m_tree_prev;
    c4::yml::Callbacks gcb = m_glob_prev;
    tcb.set_user_data((void*)this)
        .set_error_basic(experror_callback_basic)
        .set_error_parse(experror_callback_parse)
        .set_error_visit(experror_callback_visit);
    gcb.set_user_data((void*)this)
        .set_error_basic(experror_callback_basic)
        .set_error_parse(experror_callback_parse)
        .set_error_visit(experror_callback_visit);
    if(tree)
    {
        _c4dbgpf("setting error callback: tree={} err={}", c4::fmt::hex((void const*)m_tree), c4::fmt::hex((void const*)tcb.m_error_parse));
        tree->callbacks(tcb);
        EXPECT_EQ(tree->callbacks().m_error_basic, experror_callback_basic);
        EXPECT_EQ(tree->callbacks().m_error_parse, experror_callback_parse);
        EXPECT_EQ(tree->callbacks().m_error_visit, experror_callback_visit);
    }
    _c4dbgpf("setting error callback: global err={}", c4::fmt::hex((void const*)gcb.m_error_parse));
    set_callbacks(gcb);
    EXPECT_EQ(get_callbacks().m_error_basic, experror_callback_basic);
    EXPECT_EQ(get_callbacks().m_error_parse, experror_callback_parse);
    EXPECT_EQ(get_callbacks().m_error_visit, experror_callback_visit);
}

ExpectError::~ExpectError()
{
    if(m_tree)
    {
        _c4dbgp("resetting error callback: tree");
        m_tree->callbacks(m_tree_prev);
    }
    _c4dbgp("resetting error callback: global");
    set_callbacks(m_glob_prev);
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
}

void ExpectError::check_error(ExpectedErrorType errtype, Tree *tree, fntestref fn, Location const& loc)
{
    switch(errtype)
    {
    case ExpectedErrorType::err_none:
        check_success(tree, std::forward<fntestref>(fn));
        break;
    case ExpectedErrorType::err_any:
        check_error_basic(tree, std::forward<fntestref>(fn), /*only_basic*/false);
        break;
    case ExpectedErrorType::err_basic:
        check_error_basic(tree, std::forward<fntestref>(fn));
        break;
    case ExpectedErrorType::err_parse:
        check_error_parse(tree, std::forward<fntestref>(fn), loc);
        break;
    case ExpectedErrorType::err_visit:
        check_error_visit(tree, std::forward<fntestref>(fn));
        break;
    }
}

void ExpectError::check_success(Tree *tree, fntestref fn)
{
    Location expected_location = {};
    auto context = ExpectError(ExpectedErrorType::err_none, tree, expected_location);
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _c4dbgp("check expected success");
        fn();
        _c4dbgp("check expected success: success!");
    }
    C4_IF_EXCEPTIONS_(catch(ExpectedErrorBasic const& e), else)
    {
        C4_IF_EXCEPTIONS_( , ExpectedErrorBasic const& e = s_jmp_err_basic);
        (void)e;
        #if defined(RYML_WITH_EXCEPTIONS_)
        FAIL() << "exception:\n" << e.what();
        #else
        FAIL() << "exception:\n" << e.smsg;
        #endif
    }
    EXPECT_EQ(context.m_error, ExpectedErrorType::err_none);
}

void ExpectError::check_error_basic(Tree const* tree, fntestref fn, bool only_basic)
{
    check_error_basic(const_cast<Tree*>(tree), fn, only_basic);
}

void ExpectError::check_error_parse(Tree const* tree, fntestref fn, Location const& expected_location)
{
    check_error_parse(const_cast<Tree*>(tree), fn, expected_location);
}

void ExpectError::check_error_visit(Tree const* tree, fntestref fn, id_type expected_id)
{
    check_error_visit(const_cast<Tree*>(tree), fn, expected_id);
}

void ExpectError::check_assert_basic(Tree const* tree, fntestref fn, bool only_basic)
{
    check_assert_basic(const_cast<Tree*>(tree), fn, only_basic);
}

void ExpectError::check_assert_parse(Tree const* tree, fntestref fn, Location const& expected_location)
{
    check_assert_parse(const_cast<Tree*>(tree), fn, expected_location);
}

void ExpectError::check_assert_visit(Tree const* tree, fntestref fn, id_type expected_id)
{
    check_assert_visit(const_cast<Tree*>(tree), fn, expected_id);
}

void ExpectError::check_error_basic(Tree *tree, fntestref fn, bool only_basic)
{
    RYML_SAVE_TEST_EXPFAIL_();
    auto context = ExpectError(ExpectedErrorType::err_basic, tree);
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
        #if defined(RYML_WITH_EXCEPTIONS_)
        _c4dbgpf("---------------------\n""got an expected exception: {}""---------------------\n", e.what());
        #endif
    }
    C4_IF_EXCEPTIONS_(catch(...)
    {
        FAIL() << "got an unexpected exception";
    }, )
    if(context.m_error == ExpectedErrorType::err_none)
    {
        FAIL() << "no error occurred";
    }
    else
    {
        EXPECT_NE(context.m_error, ExpectedErrorType::err_none);
        if(only_basic)
        {
            EXPECT_EQ(context.m_error, ExpectedErrorType::err_basic);
        }
    }
}

void ExpectError::check_error_parse(Tree *tree, fntestref fn, Location const& expected_location)
{
    RYML_SAVE_TEST_EXPFAIL_();
    ExpectError context(ExpectedErrorType::err_parse, tree, expected_location);
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
        #if defined(RYML_DBG) && defined(RYML_WITH_EXCEPTIONS_)
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
    C4_IF_EXCEPTIONS_(catch(std::exception const& exc)
    {
        FAIL() << "got an unexpected exception: " << exc.what() << "\n";
    }, )
    C4_IF_EXCEPTIONS_(catch(...)
    {
        FAIL() << "got an unexpected exception";
    }, )
    if(context.m_error != ExpectedErrorType::err_none)
    {
        EXPECT_EQ(context.m_error, ExpectedErrorType::err_parse);
    }
    else
    {
        FAIL() << "no error occurred";
    }
}

void ExpectError::check_error_visit(Tree *tree, fntestref fn, id_type id)
{
    RYML_SAVE_TEST_EXPFAIL_();
    auto context = ExpectError(ExpectedErrorType::err_visit, tree);
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
        #if defined(RYML_DBG) && defined(RYML_WITH_EXCEPTIONS_)
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
        FAIL() << "got an unexpected exception:\n" << exc.what() << "\n";
    }, )
    C4_IF_EXCEPTIONS_(catch(...)
    {
        FAIL() << "got an unexpected exception";
    }, )
    if(context.m_error != ExpectedErrorType::err_none)
    {
        EXPECT_EQ(context.m_error, ExpectedErrorType::err_visit);
    }
    else
    {
        FAIL() << "no error occurred";
    }
}

void ExpectError::check_assert(ExpectedErrorType errtype, Tree *tree, fntestref fn, Location const& loc)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error(errtype, tree, fn, loc);
    #else
    C4_UNUSED(errtype);
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(loc);
    #endif
}

void ExpectError::check_assert_basic(Tree *tree, fntestref fn, bool only_basic)
{
    #if RYML_USE_ASSERT
    ExpectError::check_error_basic(tree, fn, only_basic);
    #else
    C4_UNUSED(tree);
    C4_UNUSED(fn);
    C4_UNUSED(only_basic);
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

} // namespace yml
} // namespace c4
