#include "./test_lib/test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/dump.hpp"
#endif
#include <stdexcept>
#include <csetjmp>


C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen: This function or variable may be unsafe

namespace c4 {
namespace yml {

static_assert(std::is_same<std::underlying_type<decltype(c4::yml::npos)>::type, size_t>::value, "invalid type");
static_assert(std::is_same<std::underlying_type<decltype(c4::yml::NONE)>::type, id_type>::value, "invalid type");
static_assert(size_t(c4::yml::npos) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...
static_assert(size_t(c4::yml::NONE) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...

std::string stored_msg;
std::string stored_msg_full;
Location stored_cpploc;
Location stored_ymlloc;
void * stored_mem;
size_t stored_length;
Tree const* stored_tree;
id_type stored_id;

C4_IF_EXCEPTIONS_( ,
std::jmp_buf s_jmp_env_expect_error = {};
)

#ifdef C4_WIN
const char * const null_name = "NUL:";
#else // assume unix or posix
const char * const null_name = "/dev/null";
#endif

static void test_error_basic_print(const char* msg, size_t length, Location const& cpploc)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_basic_print(msg, length, cpploc, file);
    if(file)
        fclose(file); // NOLINT
}

static void test_error_parse_print(const char* msg, size_t length, Location const& cpploc, Location const& ymlloc)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_parse_print(msg, length, cpploc, ymlloc, file);
    if(file)
        fclose(file); // NOLINT
}

static void test_error_visit_print(const char* msg, size_t length, Location const& cpploc, Tree const* tree, id_type id)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_visit_print(msg, length, cpploc, tree, id, file);
    if(file)
        fclose(file); // NOLINT
}

C4_NORETURN void test_error_basic_impl(const char* msg, size_t length, Location const& cpploc, void * /*user_data*/)
{
    std::string pmsg;
    test_error_basic_print(msg, length, cpploc);
    err_basic_fmt(msg, length, cpploc, [&](csubstr s){
        pmsg.append(s.str, s.len);
    });
    stored_msg_full = pmsg;
    stored_msg.assign(msg, length);
    stored_cpploc = cpploc;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw BasicException(msg, length, cpploc);
    #else
    throw std::runtime_error(stored_msg);
    #endif
    #endif
}

C4_NORETURN void test_error_parse_impl(const char* msg, size_t length, Location const& cpploc, Location const& ymlloc, void * /*user_data*/)
{
    std::string pmsg;
    test_error_parse_print(msg, length, cpploc, ymlloc);
    err_parse_fmt(msg, length, cpploc, ymlloc, [&](csubstr s){
        pmsg.append(s.str, s.len);
    });
    stored_msg_full = pmsg;
    stored_msg.assign(msg, length);
    stored_cpploc = cpploc;
    stored_ymlloc = ymlloc;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ParseException(msg, length, cpploc, ymlloc);
    #else
    throw std::runtime_error(stored_msg);
    #endif
    #endif
}

C4_NORETURN void test_error_visit_impl(const char* msg, size_t length, Location const& cpploc, Tree const* tree, id_type id, void * /*user_data*/)
{
    std::string pmsg;
    test_error_visit_print(msg, length, cpploc, tree, id);
    err_visit_fmt(msg, length, cpploc, tree, id, [&](csubstr s){
        pmsg.append(s.str, s.len);
    });
    stored_msg_full = pmsg;
    stored_msg.assign(msg, length);
    stored_cpploc = cpploc;
    stored_tree = tree;
    stored_id = id;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw VisitException(msg, length, cpploc, tree, id);
    #else
    throw std::runtime_error(stored_msg);
    #endif
    #endif
}

void* test_allocate_impl(size_t length, void * /*hint*/, void * /*user_data*/)
{
    void *mem = ::malloc(length);
    stored_length = length;
    stored_mem = mem;
    if(mem == nullptr)
    {
        const char msg[] = "could not allocate memory";
        test_error_basic_impl(msg, sizeof(msg)-1, {}, nullptr);
    }
    return mem;
}

void test_free_impl(void *mem, size_t length, void * /*user_data*/)
{
    stored_mem = mem;
    stored_length = length;
    ::free(mem);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Callbacks mk_test_callbacks()
{
    return Callbacks{}
        .set_user_data(nullptr)
        .set_allocate(&test_allocate_impl)
        .set_free(&test_free_impl)
        .set_error_basic(&test_error_basic_impl)
        .set_error_parse(&test_error_parse_impl)
        .set_error_visit(&test_error_visit_impl);
}

void check_callbacks_test(Callbacks const& cb)
{
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_EQ(cb.m_allocate, &test_allocate_impl);
    EXPECT_EQ(cb.m_free, &test_free_impl);
    EXPECT_EQ(cb.m_error_basic, &test_error_basic_impl);
    EXPECT_EQ(cb.m_error_parse, &test_error_parse_impl);
    EXPECT_EQ(cb.m_error_visit, &test_error_visit_impl);
}

void check_callbacks_default(Callbacks const& cb)
{
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_NE(cb.m_allocate, &test_allocate_impl);
    EXPECT_NE(cb.m_free, &test_free_impl);
    EXPECT_NE(cb.m_error_basic, &test_error_basic_impl);
    EXPECT_NE(cb.m_error_parse, &test_error_parse_impl);
    EXPECT_NE(cb.m_error_visit, &test_error_visit_impl);
#ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_NE(cb.m_allocate, nullptr);
    EXPECT_NE(cb.m_free, nullptr);
    EXPECT_NE(cb.m_error_basic, nullptr);
    EXPECT_NE(cb.m_error_parse, nullptr);
    EXPECT_NE(cb.m_error_visit, nullptr);
#else
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_EQ(cb.m_allocate, nullptr);
    EXPECT_EQ(cb.m_free, nullptr);
    EXPECT_EQ(cb.m_error_basic, nullptr);
    EXPECT_EQ(cb.m_error_parse, nullptr);
    EXPECT_EQ(cb.m_error_visit, nullptr);
#endif
}

TEST(Callbacks, ctor)
{
    Callbacks cb;
    EXPECT_NE(cb.m_allocate, &test_allocate_impl);
    EXPECT_NE(cb.m_free, &test_free_impl);
    EXPECT_NE(cb.m_error_basic, &test_error_basic_impl);
    EXPECT_NE(cb.m_error_parse, &test_error_parse_impl);
    EXPECT_NE(cb.m_error_visit, &test_error_visit_impl);
#ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_NE(cb.m_allocate, nullptr);
    EXPECT_NE(cb.m_free, nullptr);
    EXPECT_NE(cb.m_error_basic, nullptr);
    EXPECT_NE(cb.m_error_parse, nullptr);
    EXPECT_NE(cb.m_error_visit, nullptr);
#else
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_EQ(cb.m_allocate, nullptr);
    EXPECT_EQ(cb.m_free, nullptr);
    EXPECT_EQ(cb.m_error_basic, nullptr);
    EXPECT_EQ(cb.m_error_parse, nullptr);
    EXPECT_EQ(cb.m_error_visit, nullptr);
#endif
}

TEST(Callbacks, set_user_data)
{
    Callbacks cb;
    EXPECT_EQ(cb.m_user_data, nullptr);
    cb.set_user_data((void*)0xff);
    EXPECT_EQ(cb.m_user_data, (void*)0xff);
    cb.set_user_data(nullptr);
    EXPECT_EQ(cb.m_user_data, nullptr);
}

TEST(Callbacks, set_allocate)
{
    Callbacks cb;
    cb.set_allocate(test_allocate_impl);
    EXPECT_EQ(cb.m_allocate, test_allocate_impl);
    cb.set_allocate(test_allocate_impl);
    cb.set_allocate(nullptr);
#ifdef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_allocate, nullptr);
#else
    EXPECT_NE(cb.m_allocate, nullptr);
#endif
}

TEST(Callbacks, set_free)
{
    Callbacks cb;
    cb.set_free(test_free_impl);
    EXPECT_EQ(cb.m_free, test_free_impl);
    cb.set_free(test_free_impl);
    cb.set_free(nullptr);
#ifdef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_free, nullptr);
#else
    EXPECT_NE(cb.m_free, nullptr);
#endif
}

TEST(Callbacks, set_error_basic)
{
    Callbacks cb;
    cb.set_error_basic(test_error_basic_impl);
    EXPECT_EQ(cb.m_error_basic, test_error_basic_impl);
    cb.set_error_basic(test_error_basic_impl);
    cb.set_error_basic(nullptr);
#ifdef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_error_basic, nullptr);
#else
    EXPECT_NE(cb.m_error_basic, nullptr);
#endif
}

TEST(Callbacks, set_error_parse)
{
    Callbacks cb;
    cb.set_error_parse(test_error_parse_impl);
    EXPECT_EQ(cb.m_error_parse, test_error_parse_impl);
    cb.set_error_parse(test_error_parse_impl);
    cb.set_error_parse(nullptr);
#ifdef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_error_parse, nullptr);
#else
    EXPECT_NE(cb.m_error_parse, nullptr);
#endif
}

TEST(Callbacks, set_error_visit)
{
    Callbacks cb;
    cb.set_error_visit(test_error_visit_impl);
    EXPECT_EQ(cb.m_error_visit, test_error_visit_impl);
    cb.set_error_visit(test_error_visit_impl);
    cb.set_error_visit(nullptr);
#ifdef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_error_visit, nullptr);
#else
    EXPECT_NE(cb.m_error_visit, nullptr);
#endif
}


TEST(Callbacks, get)
{
    Callbacks cb = get_callbacks();
    check_callbacks_default(cb);
}

TEST(Callbacks, set)
{
    Callbacks cb = mk_test_callbacks();
    Callbacks before = get_callbacks();
    check_callbacks_default(before);
    set_callbacks(cb);
    Callbacks curr = get_callbacks();
    check_callbacks_test(curr);
    set_callbacks(before);
    curr = get_callbacks();
    check_callbacks_default(curr);
}

TEST(Callbacks, reset)
{
    Callbacks before = get_callbacks();
    {
        SCOPED_TRACE("default");
        check_callbacks_default(before);
    }
    Callbacks cb = mk_test_callbacks();
    set_callbacks(cb);
    {
        SCOPED_TRACE("test");
        check_callbacks_test(cb);
    }
    reset_callbacks();
    {
        SCOPED_TRACE("before");
        check_callbacks_default(before);
        check_callbacks_default(get_callbacks());
    }
}

TEST(Callbacks, eq)
{
    Callbacks before = get_callbacks();
    Callbacks bf2 = get_callbacks();
    Callbacks cb = mk_test_callbacks();

    EXPECT_EQ(bf2, before);
    EXPECT_TRUE(bf2 == before);
    EXPECT_FALSE(!(bf2 == before));
    EXPECT_TRUE(!(cb == before));
}

TEST(Callbacks, ne)
{
    Callbacks before = get_callbacks();
    Callbacks bf2 = get_callbacks();
    Callbacks cb = mk_test_callbacks().set_user_data((void*)0xff);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_TRUE(!(bf2 != before));
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_user_data)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    cb.set_user_data((void*)0xff);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_allocate)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_allocate(test_allocate_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_free)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_free(test_free_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_error_basic)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_error_basic(test_error_basic_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_error_parse)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_error_parse(test_error_parse_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_error_visit)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_error_visit(test_error_visit_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(allocate_and_free, basic)
{
    EXPECT_NE(get_callbacks().m_allocate, &test_allocate_impl);
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    void *mem = get_callbacks().m_allocate(32, /*hint*/0, get_callbacks().m_user_data);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
    stored_mem = nullptr;
    stored_length = 0;
    get_callbacks().m_free(mem, 32u, get_callbacks().m_user_data);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
    reset_callbacks();
}

void testloc(Location const& actual, Location const& expected)
{
    location_print(actual, stdout); // ensure coverage
    printf("\n");
    fflush(stdout);
    EXPECT_EQ(actual.name, expected.name);
    EXPECT_EQ(actual.offset, expected.offset);
    EXPECT_EQ(actual.line, expected.line);
    EXPECT_EQ(actual.col, expected.col);
}
void test_full_msg(Location const& loc, csubstr msg, csubstr *payload)
{
    csubstr rem = msg;
    ASSERT_TRUE(rem.begins_with(loc.name));
    rem = rem.sub(loc.name.len);
    ASSERT_TRUE(rem.begins_with(':'));
    rem = rem.sub(1);
    size_t pos = rem.find(':');
    ASSERT_NE(pos, csubstr::npos);
    csubstr str = rem.first(pos);
    EXPECT_EQ(str.len, pos);
    size_t line_num = csubstr::npos;
    ASSERT_TRUE(from_chars(str, &line_num));
    EXPECT_EQ(loc.line, line_num);
    rem = rem.sub(pos);
    ASSERT_TRUE(rem.begins_with(':'));
    rem = rem.sub(1);
    if(loc.col)
    {
        pos = rem.find(':');
        ASSERT_NE(pos, csubstr::npos);
        str = rem.first(pos);
        EXPECT_EQ(str.len, pos);
        size_t col_num = csubstr::npos;
        ASSERT_TRUE(from_chars(str, &col_num));
        EXPECT_EQ(loc.col, col_num);
        rem = rem.sub(pos);
        ASSERT_TRUE(rem.begins_with(':'));
        rem = rem.sub(1);
    }
    ASSERT_TRUE(rem.begins_with(" "));
    rem = rem.sub(1);
    if(loc.offset)
    {
        ASSERT_TRUE(rem.begins_with("("));
        rem = rem.sub(1);
        pos = rem.find("B)");
        ASSERT_NE(pos, csubstr::npos);
        str = rem.first(pos);
        EXPECT_EQ(str.len, pos);
        size_t offset = csubstr::npos;
        ASSERT_TRUE(from_chars(str, &offset));
        EXPECT_EQ(loc.offset, offset);
        rem = rem.sub(pos);
        ASSERT_TRUE(rem.begins_with("B) "));
        rem = rem.sub(3);
    }
    ASSERT_TRUE(rem.begins_with("ERROR: ")) << rem;
    rem = rem.sub(csubstr("ERROR: ").len);
    ASSERT_TRUE(rem.begins_with('[')) << rem;
    rem = rem.sub(1);
    ASSERT_TRUE(rem.begins_with("basic") || rem.begins_with("parse") || rem.begins_with("visit")) << rem;
    rem = rem.sub(5);
    ASSERT_TRUE(rem.begins_with("] ")) << rem;
    rem = rem.sub(2);
    *payload = rem;
}
template<class... Args>
void test_full_msg_args(Location const& loc, csubstr msg, std::string const& expected)
{
    csubstr payload;
    test_full_msg(loc, msg, &payload);
    ASSERT_TRUE(payload.is_sub(msg));
    EXPECT_TRUE(to_csubstr(payload).begins_with(to_csubstr(expected))) << "payload=~~~" << std::string(payload.str, payload.len) << "~~~  vs  expected=~~~" << expected << "~~~";
    EXPECT_TRUE(to_csubstr(stored_msg).begins_with(to_csubstr(expected))) << "payload=~~~" << std::string(payload.str, payload.len) << "~~~  vs  expected=~~~" << expected << "~~~";
}


//-----------------------------------------------------------------------------

#ifdef _RYML_WITH_EXCEPTIONS
#define RYML_IF_EXC(...) __VA_ARGS__
#else
#define RYML_IF_EXC(...)
#endif

template<class... Args>
void test_error_basic(Location const& cpploc, const char* errmsg, Args const& ...args)
{
    SCOPED_TRACE("test_error_basic");
    {
        SCOPED_TRACE("custom handler");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_basic_msg(cpploc, errmsg, args...);
        }
        RYML_IF_EXC(catch(BasicException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    {
        SCOPED_TRACE("custom handler, no format");
        std::string expected = errmsg;
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_basic(cpploc, errmsg);
        }
        RYML_IF_EXC(catch(BasicException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_basic_msg(cpploc, errmsg, args...);
        }
        catch(BasicException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = errmsg;
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_basic(cpploc, errmsg);
        }
        catch(BasicException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    #endif
}

TEST(error, basic)
{
    {
        SCOPED_TRACE("case 1: no col, no offset");
        test_error_basic(Location("file.yml", 0, 123u, 0), "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 2: no col, offset");
        test_error_basic(Location("file.yml", 433u, 123u, 0u), "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 3: col, no offset");
        test_error_basic(Location("file.yml", 0, 123u, 456u), "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 4: col, offset");
        test_error_basic(Location("file.yml", 433u, 123u, 456u), "some message int={} s={}", 1213, "abc");
    }
}


//-----------------------------------------------------------------------------

template<class... Args>
void test_error_parse(Location const& cpploc, Location const& ymlloc, const char* errmsg, Args const& ...args)
{
    SCOPED_TRACE("test_error_parse");
    {
        SCOPED_TRACE("custom handler");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse_msg(cpploc, ymlloc, errmsg, args...);
        }
        RYML_IF_EXC(catch(ParseException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_ymlloc, ymlloc);
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(ymlloc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    {
        SCOPED_TRACE("custom handler, no format");
        std::string expected = errmsg;
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse(cpploc, ymlloc, errmsg);
        }
        RYML_IF_EXC(catch(ParseException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_ymlloc, ymlloc);
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(ymlloc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_parse_msg(cpploc, ymlloc, errmsg, args...);
        }
        catch(ParseException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.ymlloc, ymlloc);
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    {
        SCOPED_TRACE("default handler exceptions, no format");
        std::string expected = errmsg;
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_parse(cpploc, ymlloc, errmsg);
        }
        catch(ParseException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.ymlloc, ymlloc);
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    {
        SCOPED_TRACE("parse err can be caught as basic");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_parse_msg(cpploc, ymlloc, errmsg, args...);
        }
        catch(BasicException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    #endif
}

TEST(error, parse)
{
    {
        SCOPED_TRACE("case 1: no col, no offset");
        test_error_parse(Location("file.cpp", 0, 123u, 0),
                         Location("file.yml", 0, 123u, 0),
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 2: no col, offset");
        test_error_parse(Location("file.cpp", 433u, 123u, 0u),
                         Location("file.yml", 433u, 123u, 0u),
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 2: col, no offset");
        test_error_parse(Location("file.cpp", 0, 123u, 456u),
                         Location("file.yml", 0, 123u, 456u),
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 4: no col, offset");
        test_error_parse(Location("file.cpp", 433u, 123u, 456u),
                         Location("file.yml", 433u, 123u, 456u),
                         "some message int={} s={}", 1213, "abc");
    }
}


//-----------------------------------------------------------------------------

template<class... Args>
void test_error_visit(Location const& cpploc, Tree const* t, id_type id, const char* errmsg, Args const& ...args)
{
    SCOPED_TRACE("test_error_visit");
    {
        SCOPED_TRACE("custom handler");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit_msg(cpploc, t, id, errmsg, args...);
        }
        RYML_IF_EXC(catch(VisitException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    {
        SCOPED_TRACE("custom handler, no fmt");
        std::string expected = errmsg;
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit(cpploc, t, id, errmsg);
        }
        RYML_IF_EXC(catch(VisitException const& exc){
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit_msg(cpploc, t, id, errmsg, args...);
        }
        catch(VisitException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
            EXPECT_EQ(exc.tree, t);
            EXPECT_EQ(exc.node, id);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    {
        SCOPED_TRACE("default handler exceptions, no fmt");
        std::string expected = errmsg;
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit(cpploc, t, id, errmsg);
        }
        catch(VisitException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
            EXPECT_EQ(exc.tree, t);
            EXPECT_EQ(exc.node, id);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    {
        SCOPED_TRACE("visit err can be caught as basic");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit_msg(cpploc, t, id, errmsg, args...);
        }
        catch(BasicException const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    #endif
}

TEST(error, visit)
{
    const Tree tree = parse_in_arena("foo: bar");
    const id_type id = tree["foo"].id();
    {
        SCOPED_TRACE("case 1: no col, no offset");
        test_error_visit(Location("file.cpp", 0, 123u, 0),
                         &tree, id,
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 2: no col, offset");
        test_error_visit(Location("file.cpp", 433u, 123u, 0u),
                         &tree, id,
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 2: col, no offset");
        test_error_visit(Location("file.cpp", 0, 123u, 456u),
                         &tree, id,
                         "some message int={} s={}", 1213, "abc");
    }
    {
        SCOPED_TRACE("case 4: no col, offset");
        test_error_visit(Location("file.cpp", 433u, 123u, 456u),
                         &tree, id,
                         "some message int={} s={}", 1213, "abc");
    }
}

TEST(RYML_CHECK, basic)
{
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    ASSERT_EQ(get_callbacks(), mk_test_callbacks());
    const size_t the_line = (size_t)(__LINE__ + 3); // careful
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        RYML_CHECK_BASIC(false);  // keep both statements in the same line
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        ;
    }
    EXPECT_EQ(stored_msg, "check failed: false");
    EXPECT_EQ(stored_cpploc.name, __FILE__);
    EXPECT_EQ(stored_cpploc.offset, 0u);
    EXPECT_EQ(stored_cpploc.line, the_line);
    EXPECT_EQ(stored_cpploc.col, 0u);
    reset_callbacks();
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
}


TEST(RYML_ASSERT, basic)
{
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    stored_msg = "";
    stored_cpploc = {};
    const size_t the_line = (size_t)(__LINE__ + 3); // careful
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        RYML_ASSERT_BASIC(false);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        ;
    }
    #if RYML_USE_ASSERT
    EXPECT_EQ(stored_msg, "check failed: false");
    EXPECT_EQ(stored_cpploc.name, __FILE__);
    EXPECT_EQ(stored_cpploc.offset, 0u);
    EXPECT_EQ(stored_cpploc.line, the_line);
    EXPECT_EQ(stored_cpploc.col, 0u);
    #else
    (void)the_line;
    EXPECT_EQ(stored_msg, "");
    EXPECT_EQ(stored_cpploc.name, nullptr);
    EXPECT_EQ(stored_cpploc.offset, 0u);
    EXPECT_EQ(stored_cpploc.line, 0u);
    EXPECT_EQ(stored_cpploc.col, 0u);
    #endif
    reset_callbacks();
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
}


//-----------------------------------------------------------------------------

struct CustomLengthArg { size_t len; };
size_t to_chars(substr buf, CustomLengthArg const& arg)
{
    size_t len = (arg.len < buf.len) ? arg.len : buf.len;
    for(size_t i = 0; i < len; ++i)
        buf.str[i] = (char)('0' + (i % 9));
    return arg.len;
}

struct Dumper
{
    char errmsg[RYML_ERRMSG_SIZE] = {0};
    detail::_SubstrWriter writer{errmsg};
    void operator()(csubstr s)
    {
        writer.append(s);
    }
};

TEST(_to_chars_limited, arg_not_requiring_argbuf)
{
    char argbuf_[RYML_LOGBUF_SIZE];
    substr argbuf(argbuf_);
    ASSERT_EQ(argbuf.len + 1, RYML_LOGBUF_SIZE);
    {
        SCOPED_TRACE("smaller size");
        std::string arg(argbuf.len - 1, '-');
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("same size");
        std::string arg(argbuf.len, '-');
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("larger size");
        std::string arg(argbuf.len + 1, '-');
        std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
}

TEST(_to_chars_limited, arg_requiring_argbuf)
{
    char argbuf_[RYML_LOGBUF_SIZE];
    substr argbuf(argbuf_);
    ASSERT_EQ(argbuf.len + 1, RYML_LOGBUF_SIZE);
    {
        SCOPED_TRACE("smaller size");
        CustomLengthArg arg{argbuf.len - 1};
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("same size");
        CustomLengthArg arg{argbuf.len};
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("larger size");
        CustomLengthArg arg{argbuf.len + 1};
        std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_NE(actual, expected);
        EXPECT_GT(arg.len, argbuf.len);
        EXPECT_GT(expected.size(), argbuf.len);
        EXPECT_EQ(actual.len, argbuf.len);
        EXPECT_TRUE(actual.ends_with("...")) << actual;
        csubstr expected_with_ellipsis = detail::_maybe_add_ellipsis(to_substr(expected).first(argbuf.len), arg.len);
        EXPECT_TRUE(expected_with_ellipsis.ends_with("...")) << expected;
        EXPECT_EQ(actual, expected_with_ellipsis);
    }
}

TEST(_mk_err_msg, smaller_size)
{
    size_t item_size = RYML_ERRMSG_SIZE / 10;
    std::string arg(item_size, '-');
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", to_csubstr(arg));
    EXPECT_EQ(expected.size(), item_size);
    csubstr actual = detail::_mk_err_msg(buf, "{}", to_csubstr(arg));
    EXPECT_LT(actual.len, buf.len);
    EXPECT_EQ(actual, expected);
}

TEST(_mk_err_msg, larger_size_not_requiring_argbuf)
{
    size_t item_size = RYML_ERRMSG_SIZE * 2;
    std::string arg_(item_size, '-');
    csubstr arg = to_csubstr(arg_);
    EXPECT_EQ(arg.size(), item_size);
    EXPECT_EQ(arg.len, item_size);
    char buf_[RYML_ERRMSG_SIZE] = {0};
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", arg);
    EXPECT_EQ(expected.size(), arg.len);
    csubstr actual = detail::_mk_err_msg(buf, "{}", arg);
    EXPECT_EQ(actual.len, buf.len);
    EXPECT_NE(actual, expected);
    EXPECT_TRUE(actual.ends_with("..."));
}

TEST(_mk_err_msg, larger_size_requiring_argbuf)
{
    size_t item_size = RYML_ERRMSG_SIZE * 2;
    CustomLengthArg arg{item_size};
    EXPECT_EQ(arg.len, item_size);
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", arg);
    EXPECT_EQ(expected.size(), arg.len);
    csubstr actual = detail::_mk_err_msg(buf, "{}", arg);
    EXPECT_EQ(actual.len, RYML_LOGBUF_SIZE-1);
    EXPECT_NE(actual, expected);
    EXPECT_TRUE(actual.ends_with("..."));
}

TEST(_mk_err_msg, trailing_fmt)
{
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{} trailing after fmt", 2);
    csubstr actual = detail::_mk_err_msg(buf, "{} trailing after fmt", 2);
    EXPECT_EQ(actual, expected);
}

TEST(_mk_err_msg, no_args)
{
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("look, no args!");
    csubstr actual = detail::_mk_err_msg(buf, "look, no args!");
    EXPECT_EQ(actual, expected);
}

// FIXME this is here merely to avoid a linker error
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
