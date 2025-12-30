#include "./test_lib/test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/yml/error.def.hpp"
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


namespace { // anon
C4_NO_INLINE void dump2file(csubstr s, FILE *f)
{
    // using fwrite() is more portable than using fprintf("%.*s") which
    // is not available in some embedded platforms
    if(s.len)
        fwrite(s.str, 1, s.len, f); // NOLINT
}
} // namespace anon

void location_print(Location const& loc, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    location_format([f](csubstr s){ dump2file(s, f); }, loc);
}

void err_basic_print(csubstr msg, ErrorDataBasic const& errdata, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    err_basic_format([f](csubstr s){ dump2file(s, f); }, msg, errdata);
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}

void err_parse_print(csubstr msg, ErrorDataParse const& errdata, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    err_parse_format([f](csubstr s){ dump2file(s, f); }, msg, errdata);
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}

void err_visit_print(csubstr msg, ErrorDataVisit const& errdata, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    err_visit_format([f](csubstr s){ dump2file(s, f); }, msg, errdata);
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}

static void test_loc_print(Location const& cpploc)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    location_print(cpploc, file);
    fprintf(file, "\n"); // NOLINT
    fflush(file); // NOLINT
    if(file)
        fclose(file); // NOLINT
}

static void test_error_basic_print(csubstr msg, ErrorDataBasic const& errdata)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_basic_print(msg, errdata, file);
    if(file)
        fclose(file); // NOLINT
}

static void test_error_parse_print(csubstr msg, ErrorDataParse const& errdata)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_parse_print(msg, errdata, file);
    if(file)
        fclose(file); // NOLINT
}

static void test_error_visit_print(csubstr msg, ErrorDataVisit const& errdata)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_visit_print(msg, errdata, file);
    if(file)
        fclose(file); // NOLINT
}

C4_NORETURN void test_error_basic_impl(csubstr msg, ErrorDataBasic const& errdata, void * /*user_data*/)
{
    std::string pmsg;
    test_error_basic_print(msg, errdata);
    err_basic_format([&](csubstr s){
        pmsg.append(s.str, s.len);
    }, msg, errdata);
    stored_msg_full = pmsg;
    stored_msg.assign(msg.str, msg.len);
    stored_cpploc = errdata.location;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionBasic(msg, errdata);
    #else
    throw std::runtime_error(stored_msg);
    #endif
    #endif
}

C4_NORETURN void test_error_parse_impl(csubstr msg, ErrorDataParse const& errdata, void * /*user_data*/)
{
    std::string pmsg;
    test_error_parse_print(msg, errdata);
    err_parse_format([&](csubstr s){
        pmsg.append(s.str, s.len);
    }, msg, errdata);
    stored_msg_full = pmsg;
    stored_msg.assign(msg.str, msg.len);
    stored_cpploc = errdata.cpploc;
    stored_ymlloc = errdata.ymlloc;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionParse(msg, errdata);
    #else
    throw std::runtime_error(stored_msg);
    #endif
    #endif
}

C4_NORETURN void test_error_visit_impl(csubstr msg, ErrorDataVisit const& errdata, void * /*user_data*/)
{
    std::string pmsg;
    test_error_visit_print(msg, errdata);
    err_visit_format([&](csubstr s){
        pmsg.append(s.str, s.len);
    }, msg, errdata);
    stored_msg_full = pmsg;
    stored_msg.assign(msg.str, msg.len);
    stored_cpploc = errdata.cpploc;
    stored_tree = errdata.tree;
    stored_id = errdata.node;
    #ifndef C4_EXCEPTIONS
    std::longjmp(s_jmp_env_expect_error, 1);
    #else
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ExceptionVisit(msg, errdata);
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
        test_error_basic_impl("could not allocate memory", RYML_LOC_HERE(), nullptr);
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
    int data = 1;
    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wdeprecated-declarations");
    C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated");
    Callbacks cb2((void*)&data,
                  &test_allocate_impl,
                  &test_free_impl,
                  &test_error_basic_impl);
    C4_SUPPRESS_WARNING_GCC_CLANG_POP
#ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb2.m_user_data, &data);
    EXPECT_EQ(cb2.m_allocate, &test_allocate_impl);
    EXPECT_EQ(cb2.m_free, &test_free_impl);
    EXPECT_EQ(cb2.m_error_basic, &test_error_basic_impl);
    EXPECT_NE(cb.m_error_parse, nullptr);
    EXPECT_NE(cb.m_error_visit, nullptr);
#else
    EXPECT_EQ(cb2.m_user_data, nullptr);
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
    test_loc_print(actual); // ensure coverage
    EXPECT_EQ(actual.name, expected.name);
    EXPECT_EQ(actual.offset, expected.offset);
    EXPECT_EQ(actual.line, expected.line);
    EXPECT_EQ(actual.col, expected.col);
}
void test_full_msg(Location const& loc, csubstr msg, csubstr *payload)
{
    SCOPED_TRACE(std::string(msg.str, msg.len));
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
    if(loc.col != npos)
    {
        ASSERT_TRUE(rem.begins_with(" col="));
        rem = rem.sub(5);
        if(loc.offset == npos)
        {
            pos = rem.find(':');
            ASSERT_NE(pos, csubstr::npos);
            str = rem.first(pos);
            EXPECT_EQ(str.len, pos);
            rem = rem.sub(pos);
            ASSERT_TRUE(rem.begins_with(':'));
            rem = rem.sub(1);
        }
        else
        {
            pos = rem.find(" (");
            ASSERT_NE(pos, csubstr::npos);
            str = rem.first(pos);
            EXPECT_EQ(str.len, pos);
            rem = rem.sub(pos);
        }
        size_t col_num = csubstr::npos;
        ASSERT_TRUE(from_chars(str, &col_num)) << str;
        EXPECT_EQ(loc.col, col_num);
    }
    ASSERT_TRUE(rem.begins_with(" "));
    rem = rem.sub(1);
    if(loc.offset != npos)
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
        ASSERT_TRUE(rem.begins_with("B): ")) << rem;
        rem = rem.sub(4);
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
    auto reset_stored = []{
        stored_msg_full = {};
        stored_msg = {};
        stored_cpploc = {};
    };
    SCOPED_TRACE("test_error_basic");
    reset_stored();
    {
        SCOPED_TRACE("custom handler");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_basic(cpploc, errmsg, args...);
        }
        RYML_IF_EXC(catch(ExceptionBasic const& exc){
            exc_msg = exc.what();
            exc_ok = true;
            exc_msg_full = format_exc<std::string>(exc);
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    reset_stored();
    {
        SCOPED_TRACE("custom handler, no format");
        std::string expected = errmsg;
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_basic(cpploc, errmsg);
        }
        RYML_IF_EXC(catch(ExceptionBasic const& exc){
            exc_msg = exc.what();
            exc_ok = true;
            exc_msg_full = format_exc<std::string>(exc);
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = c4::formatrs<std::string>(errmsg, args...);
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_basic(cpploc, errmsg, args...);
        }
        catch(ExceptionBasic const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_basic.location, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions");
        std::string expected = errmsg;
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_basic(cpploc, errmsg);
        }
        catch(ExceptionBasic const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_basic.location, cpploc);
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

template<class Fn>
void test_check_basic(Fn &&fn, csubstr expected_msg)
{
    set_callbacks(mk_test_callbacks());
    {
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            std::forward<Fn>(fn)();
        }
        RYML_IF_EXC(catch(ExceptionBasic const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else { exc_msg = stored_msg; })
        std::cout << exc_msg << "\n";
        std::cout << stored_msg_full << "\n";
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    reset_callbacks();
}
TEST(check, basic)
{
    bool disregard_this_message = false;
    csubstr msg = "disregard_this_message";
    csubstr fmsg = "disregard_this_message: extra args: 1,2";
    test_check_basic([&]{ _RYML_CHECK_BASIC(disregard_this_message); }, msg);
    test_check_basic([&]{ _RYML_CHECK_BASIC_(get_callbacks(), disregard_this_message); }, msg);
    test_check_basic([&]{ _RYML_CHECK_BASIC_MSG(disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    test_check_basic([&]{ _RYML_CHECK_BASIC_MSG_(get_callbacks(), disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    #if RYML_USE_ASSERT
    test_check_basic([&]{ _RYML_ASSERT_BASIC(disregard_this_message); }, msg);
    test_check_basic([&]{ _RYML_ASSERT_BASIC_(get_callbacks(), disregard_this_message); }, msg);
    test_check_basic([&]{ _RYML_ASSERT_BASIC_MSG(disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    test_check_basic([&]{ _RYML_ASSERT_BASIC_MSG_(get_callbacks(), disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    #endif
}

#ifdef _RYML_WITH_EXCEPTIONS
template<class T, class ...Args>
void testmsg(Args const& ...args)
{
    std::string msg(RYML_ERRMSG_SIZE + 10, '=');
    T exc(to_csubstr(msg), args...);
    ASSERT_EQ(sizeof(exc.msg), RYML_ERRMSG_SIZE);
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 1], '\0');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 2], ']');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 3], '.');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 4], '.');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 5], '.');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 6], '[');
    EXPECT_EQ(exc.msg[RYML_ERRMSG_SIZE - 7], '=');
    EXPECT_EQ(to_csubstr(exc.msg).first_not_of('='), RYML_ERRMSG_SIZE - 6);
}
TEST(error, basic_message_is_truncated)
{
    testmsg<ExceptionBasic>(ErrorDataBasic(Location{}));
}
TEST(error, parse_message_is_truncated)
{
    testmsg<ExceptionParse>(ErrorDataParse(Location{}, Location{}));
}
TEST(error, visit_message_is_truncated)
{
    Tree tree;
    id_type node = {};
    testmsg<ExceptionVisit>(ErrorDataVisit(Location{}, &tree, node));
}
#endif

TEST(error, loc_print_accepts_null_file)
{
    test_loc_print(Location{});
    test_loc_print(RYML_LOC_HERE());
    location_print(Location{});
    location_print(Location{}, nullptr);
    location_print(RYML_LOC_HERE());
    fprintf(stderr, "\n"); // NOLINT
    location_print(RYML_LOC_HERE(), nullptr);
    fprintf(stderr, "\n"); // NOLINT
}

TEST(error, basic_print_accepts_null_file)
{
    csubstr msg = "(disregard this message)";
    err_basic_print(msg, Location{});
    err_basic_print(msg, Location{}, nullptr);
    err_basic_print(msg, RYML_LOC_HERE());
    err_basic_print(msg, RYML_LOC_HERE(), nullptr);
}

TEST(error, parse_print_accepts_null_file)
{
    csubstr msg = "(disregard this message)";
    err_parse_print(msg, {Location{}, Location{}});
    err_parse_print(msg, {Location{}, Location{}}, nullptr);
    err_parse_print(msg, {RYML_LOC_HERE(), Location{}});
    err_parse_print(msg, {RYML_LOC_HERE(), Location{}}, nullptr);
    err_parse_print(msg, {Location{}, RYML_LOC_HERE()});
    err_parse_print(msg, {Location{}, RYML_LOC_HERE()}, nullptr);
    err_parse_print(msg, {RYML_LOC_HERE(), RYML_LOC_HERE()});
    err_parse_print(msg, {RYML_LOC_HERE(), RYML_LOC_HERE()}, nullptr);
}

TEST(error, visit_print_accepts_null_file)
{
    csubstr msg = "(disregard this message)";
    Tree tree;
    id_type node = {};
    err_visit_print(msg, {Location{}, &tree, node});
    err_visit_print(msg, {Location{}, &tree, node}, nullptr);
    err_visit_print(msg, {RYML_LOC_HERE(), &tree, node});
    err_visit_print(msg, {RYML_LOC_HERE(), &tree, node}, nullptr);
}


//-----------------------------------------------------------------------------

template<class... Args>
void test_error_parse(Location const& cpploc, Location const& ymlloc, const char* errmsg, Args const& ...args)
{
    SCOPED_TRACE("test_error_parse");
    const std::string expected = errmsg;
    const std::string expected_fmt = c4::formatrs<std::string>(errmsg, args...);
    auto reset_stored = []{
        stored_msg_full = {};
        stored_msg = {};
        stored_cpploc = {};
        stored_ymlloc = {};
    };
    reset_stored();
    {
        SCOPED_TRACE("custom handler");
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg, args...);
        }
        RYML_IF_EXC(catch(ExceptionParse const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_ymlloc, ymlloc);
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(ymlloc, to_csubstr(stored_msg_full), expected_fmt);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected_fmt));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    reset_stored();
    {
        SCOPED_TRACE("custom handler, no format");
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg);
        }
        RYML_IF_EXC(catch(ExceptionParse const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_ymlloc, ymlloc);
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(ymlloc, to_csubstr(stored_msg_full), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    reset_stored();
    {
        SCOPED_TRACE("custom handler, parse falls back to basic");
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        Callbacks cb2 = mk_test_callbacks();
        cb2.m_error_parse = nullptr;
        set_callbacks(cb2);
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg);
        }
        RYML_IF_EXC(catch(ExceptionParse const& exc){
            exc_ok = false;
        }
        catch(ExceptionBasic const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, ymlloc);
        EXPECT_EQ(to_csubstr(stored_msg), expected);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    reset_stored();
    {
        SCOPED_TRACE("custom handler, parse falls back to basic");
        Callbacks cb2 = mk_test_callbacks();
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
        cb2.m_error_parse = nullptr;
        set_callbacks(cb2);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg, args...);
        }
        RYML_IF_EXC(catch(ExceptionParse const& exc){
            exc_ok = false;
        }
        catch(ExceptionBasic const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, ymlloc);
        EXPECT_EQ(to_csubstr(stored_msg), expected_fmt);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected_fmt));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions");
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg, args...);
        }
        catch(ExceptionParse const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_parse.ymlloc, ymlloc);
            testloc(exc.errdata_parse.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected_fmt);
    }
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions, no format");
        bool exc_ok = false;
        std::string exc_msg;
        std::string exc_msg_full;
        try
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg);
        }
        catch(ExceptionParse const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            exc_msg_full = format_exc<std::string>(exc);
            testloc(exc.errdata_parse.ymlloc, ymlloc);
            testloc(exc.errdata_parse.cpploc, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    reset_stored();
    {
        SCOPED_TRACE("parse err can be caught as basic");
        bool exc_ok = false;
        std::string exc_msg;
        std::string exc_msg_full;
        try
        {
            c4::yml::err_parse({cpploc, ymlloc}, errmsg, args...);
        }
        catch(ExceptionBasic const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            exc_msg_full = format_exc<std::string>(exc);
            testloc(exc.errdata_basic.location, ymlloc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected_fmt);
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

template<class Fn>
void test_check_parse(Fn &&fn, csubstr expected_msg)
{
    set_callbacks(mk_test_callbacks());
    {
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            std::forward<Fn>(fn)();
        }
        RYML_IF_EXC(catch(ExceptionParse const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else { exc_msg = stored_msg; })
        std::cout << stored_msg_full << "\n";
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    reset_callbacks();
}
TEST(check, parse)
{
    bool disregard_this_message = false;
    csubstr msg = "disregard_this_message";
    csubstr fmsg = "disregard_this_message: extra args: 1,2";
    Location ymlloc = {};
    {
        SCOPED_TRACE("here 0");
        test_check_parse([&]{ _RYML_CHECK_PARSE(disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 1");
        test_check_parse([&]{ _RYML_CHECK_PARSE_(get_callbacks(), disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 2");
        test_check_parse([&]{ _RYML_CHECK_PARSE_MSG(disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 3");
        test_check_parse([&]{ _RYML_CHECK_PARSE_MSG_(get_callbacks(), disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #if RYML_USE_ASSERT
    {
        SCOPED_TRACE("here 4");
        test_check_parse([&]{ _RYML_ASSERT_PARSE(disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 5");
        test_check_parse([&]{ _RYML_ASSERT_PARSE_(get_callbacks(), disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 6");
        test_check_parse([&]{ _RYML_ASSERT_PARSE_MSG(disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 7");
        test_check_parse([&]{ _RYML_ASSERT_PARSE_MSG_(get_callbacks(), disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #endif
}


//-----------------------------------------------------------------------------

template<class... Args>
void test_error_visit(Location const& cpploc, Tree const* t, id_type id, const char* errmsg, Args const& ...args)
{
    const std::string expected = errmsg;
    const std::string expected_fmt = c4::formatrs<std::string>(errmsg, args...);
    auto reset_stored = []{
        stored_msg_full = {};
        stored_msg = {};
        stored_cpploc = {};
        stored_id = NONE;
        stored_tree = nullptr;
    };
    SCOPED_TRACE("test_error_visit");
    reset_stored();
    {
        SCOPED_TRACE("custom handler");
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg, args...);
        }
        RYML_IF_EXC(catch(ExceptionVisit const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected_fmt);
        EXPECT_EQ(stored_tree, t);
        EXPECT_EQ(stored_id, id);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected_fmt));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    reset_stored();
    {
        SCOPED_TRACE("custom handler, no fmt");
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        set_callbacks(mk_test_callbacks());
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg);
        }
        RYML_IF_EXC(catch(ExceptionVisit const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected);
        EXPECT_EQ(stored_tree, t);
        EXPECT_EQ(stored_id, id);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    reset_stored();
    {
        SCOPED_TRACE("custom handler, null visit falls back to basic, args");
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        Callbacks cb = mk_test_callbacks();
        cb.m_error_visit = nullptr;
        set_callbacks(cb);
        EXPECT_EQ(get_callbacks().m_error_visit, nullptr);
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg, args...);
        }
        RYML_IF_EXC(catch(ExceptionVisit const& exc){
            exc_ok = false;
        }
        catch(ExceptionBasic const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        test_full_msg_args(cpploc, to_csubstr(stored_msg_full), expected_fmt);
        EXPECT_EQ(stored_tree, nullptr);
        EXPECT_EQ(stored_id, NONE);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected_fmt));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    reset_stored();
    {
        SCOPED_TRACE("custom handler, null visit falls back to basic, args");
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
        Callbacks cb = mk_test_callbacks();
        cb.m_error_visit = nullptr;
        set_callbacks(cb);
        EXPECT_EQ(get_callbacks().m_error_visit, nullptr);
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg);
        }
        RYML_IF_EXC(catch(ExceptionVisit const& exc){
            exc_ok = false;
        }
        catch(ExceptionBasic const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else {;})
        testloc(stored_cpploc, cpploc);
        EXPECT_EQ(stored_msg, expected);
        EXPECT_EQ(stored_tree, nullptr);
        EXPECT_EQ(stored_id, NONE);
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg, expected));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        reset_callbacks();
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    #ifdef _RYML_WITH_EXCEPTIONS
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions");
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg, args...);
        }
        catch(ExceptionVisit const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_visit.cpploc, cpploc);
            EXPECT_EQ(exc.errdata_visit.tree, t);
            EXPECT_EQ(exc.errdata_visit.node, id);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected_fmt);
    }
    reset_stored();
    {
        SCOPED_TRACE("default handler exceptions, no fmt");
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg);
        }
        catch(ExceptionVisit const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_visit.cpploc, cpploc);
            EXPECT_EQ(exc.errdata_visit.tree, t);
            EXPECT_EQ(exc.errdata_visit.node, id);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected);
    }
    reset_stored();
    {
        SCOPED_TRACE("visit err can be caught as basic");
        bool exc_ok = false;
        std::string exc_msg;
        try
        {
            c4::yml::err_visit({cpploc, t, id}, errmsg, args...);
        }
        catch(ExceptionBasic const& exc)
        {
            exc_ok = true;
            exc_msg = exc.what();
            testloc(exc.errdata_basic.location, cpploc);
        }
        catch(std::exception const& exc)
        {
            exc_msg = exc.what();
        }
        EXPECT_TRUE(exc_ok);
        EXPECT_EQ(exc_msg, expected_fmt);
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

template<class Fn>
void test_check_visit(Fn &&fn, csubstr expected_msg)
{
    set_callbacks(mk_test_callbacks());
    {
        std::string exc_msg;(void)exc_msg;
        std::string exc_msg_full;(void)exc_msg_full;
        RYML_IF_EXC(bool exc_ok = false);
        C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
        {
            std::forward<Fn>(fn)();
        }
        RYML_IF_EXC(catch(ExceptionVisit const& exc){
            exc_msg_full = format_exc<std::string>(exc);
            exc_msg = exc.what();
            exc_ok = true;
        })
        C4_IF_EXCEPTIONS_(catch(std::exception const& exc) { exc_msg = exc.what(); }, else { exc_msg = stored_msg; })
        std::cout << stored_msg_full << "\n";
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    reset_callbacks();
}
TEST(check, visit)
{
    bool disregard_this_message = false;
    csubstr msg = "disregard_this_message";
    csubstr fmsg = "disregard_this_message: extra args: 1,2";
    Tree tree = parse_in_arena("foo: bar");
    id_type id = tree.root_id();
    {
        SCOPED_TRACE("here 0");
        test_check_visit([&]{ _RYML_CHECK_VISIT(disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 1");
        test_check_visit([&]{ _RYML_CHECK_VISIT_(get_callbacks(), disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 2");
        test_check_visit([&]{ _RYML_CHECK_VISIT_MSG(disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 3");
        test_check_visit([&]{ _RYML_CHECK_VISIT_MSG_(get_callbacks(), disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #if RYML_USE_ASSERT
    {
        SCOPED_TRACE("here 4");
        test_check_visit([&]{ _RYML_ASSERT_VISIT(disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 5");
        test_check_visit([&]{ _RYML_ASSERT_VISIT_(get_callbacks(), disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 6");
        test_check_visit([&]{ _RYML_ASSERT_VISIT_MSG(disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 7");
        test_check_visit([&]{ _RYML_ASSERT_VISIT_MSG_(get_callbacks(), disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #endif
}

TEST(RYML_CHECK, basic)
{
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    ASSERT_EQ(get_callbacks(), mk_test_callbacks());
    const size_t the_line = (size_t)(__LINE__ + 3); // careful
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        _RYML_CHECK_BASIC(false);  // keep both statements in the same line
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        ;
    }
    EXPECT_TRUE(to_csubstr(stored_msg).begins_with("check failed"));
    EXPECT_EQ(stored_cpploc.name, __FILE__);
    EXPECT_EQ(stored_cpploc.offset, npos);
    EXPECT_EQ(stored_cpploc.line, the_line);
    EXPECT_EQ(stored_cpploc.col, npos);
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
        _RYML_ASSERT_BASIC(false);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        ;
    }
    #if RYML_USE_ASSERT
    EXPECT_TRUE(to_csubstr(stored_msg).begins_with("check failed"));
    EXPECT_EQ(stored_cpploc.name, __FILE__);
    EXPECT_EQ(stored_cpploc.offset, npos);
    EXPECT_EQ(stored_cpploc.line, the_line);
    EXPECT_EQ(stored_cpploc.col, npos);
    #else
    (void)the_line;
    EXPECT_EQ(stored_msg, "");
    EXPECT_EQ(stored_cpploc.name, nullptr);
    EXPECT_EQ(stored_cpploc.offset, npos);
    EXPECT_EQ(stored_cpploc.line, npos);
    EXPECT_EQ(stored_cpploc.col, npos);
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


TEST(_get_text_region, basic)
{
    #define check_same(a, b)                                        \
        {                                                           \
            csubstr a_ = (a);                                       \
            csubstr b_ = (b);                                       \
            if(a_.str != b_.str || a_.len != b_.len)                \
            {                                                       \
                std::cout << __FILE__ << ":" << __LINE__ << ": "    \
                          << "[" << a_.len << "]~~~";               \
                std::cout.write(a_.str, (std::streamsize)a_.len);   \
                std::cout << "~~~=" #a "\n";                        \
                std::cout << __FILE__ << ":" << __LINE__ << ": "    \
                          << "[" << b_.len << "]~~~";               \
                std::cout.write(b_.str, (std::streamsize)b_.len);   \
                std::cout << "~~~=" #b "\n";                        \
            }                                                       \
            EXPECT_EQ(a_.str, b_.str);                              \
            EXPECT_EQ(a_.len, b_.len);                              \
        }
    using detail::_get_text_region;
    {
        csubstr txt = "";
        check_same(txt, _get_text_region(txt, 0, 0, 0));
        check_same(txt, _get_text_region(txt, 0, 2, 2));
        check_same(txt, _get_text_region(txt, 1, 2, 2));
        check_same(txt, _get_text_region(txt, 2, 2, 2));
    }
    {
        csubstr txt = "a\nb\nc\n";
        //
        check_same(txt.first(1), _get_text_region(txt, 0, 0, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 0, 0));
        check_same(txt.range(2, 3), _get_text_region(txt, 2, 0, 0));
        check_same(txt.range(2, 3), _get_text_region(txt, 3, 0, 0));
        check_same(txt.range(4, 5), _get_text_region(txt, 4, 0, 0));
        check_same(txt.range(4, 5), _get_text_region(txt, 5, 0, 0));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 0));
        // 1 line before
        check_same(txt.first(1), _get_text_region(txt, 0, 1, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 1, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 2, 1, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 3, 1, 0));
        check_same(txt.range(2, 5), _get_text_region(txt, 4, 1, 0));
        check_same(txt.range(2, 5), _get_text_region(txt, 5, 1, 0));
        check_same(txt.range(4, 6), _get_text_region(txt, 6, 1, 0));
        // 2 lines before
        check_same(txt.first(1), _get_text_region(txt, 0, 2, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 2, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 2, 2, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 3, 2, 0));
        check_same(txt.range(0, 5), _get_text_region(txt, 4, 2, 0));
        check_same(txt.range(0, 5), _get_text_region(txt, 5, 2, 0));
        check_same(txt.range(2, 6), _get_text_region(txt, 6, 2, 0));
        // 1 line after
        check_same(txt.first(3), _get_text_region(txt, 0, 0, 1));
        check_same(txt.first(3), _get_text_region(txt, 1, 0, 1));
        check_same(txt.range(2, 5), _get_text_region(txt, 2, 0, 1));
        check_same(txt.range(2, 5), _get_text_region(txt, 3, 0, 1));
        check_same(txt.range(4, 6), _get_text_region(txt, 4, 0, 1));
        check_same(txt.range(4, 6), _get_text_region(txt, 5, 0, 1));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 1));
        // 2 lines after
        check_same(txt.first(5), _get_text_region(txt, 0, 0, 2));
        check_same(txt.first(5), _get_text_region(txt, 1, 0, 2));
        check_same(txt.range(2, 6), _get_text_region(txt, 2, 0, 2));
        check_same(txt.range(2, 6), _get_text_region(txt, 3, 0, 2));
        check_same(txt.range(4, 6), _get_text_region(txt, 4, 0, 2));
        check_same(txt.range(4, 6), _get_text_region(txt, 5, 0, 2));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 2));
    }
    #undef check_same
}


struct msgs { std::string nocall, withcall; };
msgs mkmsg_(csubstr txt, const char* file, size_t line, size_t col, size_t offset, size_t firstcol=0, size_t lastcol=0, size_t num_lines_before=0, size_t num_lines_after = 0, size_t maxlen=80u)
{
    Location loc;
    loc.name = to_csubstr(file);
    loc.line = line;
    loc.col = col;
    loc.offset = offset;
    msgs ret;
    auto dumpfn1 = [&ret](csubstr s) { ret.nocall.append(s.str, s.len); };
    location_format_with_context(dumpfn1, loc, txt, csubstr{}, firstcol, lastcol, num_lines_before, num_lines_after, maxlen);
    #if defined(RYML_DBG) || 1
    std::cout << "----\n";
    std::cout << ret.nocall;
    #endif
    auto dumpfn2 = [&ret](csubstr s) { ret.withcall.append(s.str, s.len); };
    location_format_with_context(dumpfn2, loc, txt, "error", firstcol, lastcol, num_lines_before, num_lines_after, maxlen);
    #if defined(RYML_DBG) || 1
    std::cout << "....\n";
    std::cout << ret.withcall;
    #endif
    return ret;
}

TEST(format_region, basic)
{
#define mkmsg(args, nocall_, withcall_)                 \
    do {                                                \
        SCOPED_TRACE(#args);                            \
        msgs actual = mkmsg_ args;                      \
        msgs expected{nocall_, withcall_};              \
        EXPECT_EQ(actual.nocall, expected.nocall);      \
        EXPECT_EQ(actual.withcall, expected.withcall);  \
    } while(0)

    csubstr txt = "aaa\nbbb\nccc\nddd\neee\n";
    //     src  file     line  col   offset
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 0), "(0B): aaa\n", "(0B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 1), "(1B): aaa\n", "(1B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 2), "(2B): aaa\n", "(2B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 3), "(3B): aaa\n", "(3B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 0), "col=0 (0B): aaa\n            |\n            (here)\n", "col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 1), "col=1 (1B): aaa\n             |\n             (here)\n", "col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 2), "col=2 (2B): aaa\n              |\n              (here)\n", "col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 3), "col=3 (3B): aaa\n               |\n               (here)\n", "col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0), "(4B): bbb\n", "(4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1), "(5B): bbb\n", "(5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2), "(6B): bbb\n", "(6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3), "(7B): bbb\n", "(7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 4+0), "col=0 (4B): bbb\n            |\n            (here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1), "col=1 (5B): bbb\n             |\n             (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2), "col=2 (6B): bbb\n              |\n              (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3), "col=3 (7B): bbb\n               |\n               (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 1), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0 , 1), "(4B): bbb\n(4B): see region:\naaa\nbbb\n", "(4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1 , 1), "(5B): bbb\n(5B): see region:\naaa\nbbb\n", "(5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2 , 1), "(6B): bbb\n(6B): see region:\naaa\nbbb\n", "(6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3 , 1), "(7B): bbb\n(7B): see region:\naaa\nbbb\n", "(7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 1), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 4+0 , 1), "col=0 (4B): bbb\n            |\n            (here)\ncol=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1 , 1), "col=1 (5B): bbb\n             |\n             (here)\ncol=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2 , 1), "col=2 (6B): bbb\n              |\n              (here)\ncol=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3 , 1), "col=3 (7B): bbb\n               |\n               (here)\ncol=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 2), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0 , 2), "(4B): bbb\n(4B): see region:\naaa\nbbb\n", "(4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1 , 2), "(5B): bbb\n(5B): see region:\naaa\nbbb\n", "(5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2 , 2), "(6B): bbb\n(6B): see region:\naaa\nbbb\n", "(6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3 , 2), "(7B): bbb\n(7B): see region:\naaa\nbbb\n", "(7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 2), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 4+0 , 2), "col=0 (4B): bbb\n            |\n            (here)\ncol=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1 , 2), "col=1 (5B): bbb\n             |\n             (here)\ncol=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2 , 2), "col=2 (6B): bbb\n              |\n              (here)\ncol=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3 , 2), "col=3 (7B): bbb\n               |\n               (here)\ncol=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0), "(8B): ccc\n", "(8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1), "(9B): ccc\n", "(9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2), "(10B): ccc\n", "(10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3), "(11B): ccc\n", "(11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 8+0), "col=0 (8B): ccc\n            |\n            (here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1), "col=1 (9B): ccc\n             |\n             (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2), "col=2 (10B): ccc\n               |\n               (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3), "col=3 (11B): ccc\n                |\n                (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 1), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 1), "(8B): ccc\n(8B): see region:\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 1), "(9B): ccc\n(9B): see region:\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 1), "(10B): ccc\n(10B): see region:\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 1), "(11B): ccc\n(11B): see region:\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 1), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 1), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 1), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 1), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 1), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 2), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 2), "(8B): ccc\n(8B): see region:\naaa\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 2), "(9B): ccc\n(9B): see region:\naaa\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 2), "(10B): ccc\n(10B): see region:\naaa\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 2), "(11B): ccc\n(11B): see region:\naaa\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 2), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 2), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 2), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 2), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 2), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 3), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 3), "(8B): ccc\n(8B): see region:\naaa\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 3), "(9B): ccc\n(9B): see region:\naaa\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 3), "(10B): ccc\n(10B): see region:\naaa\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 3), "(11B): ccc\n(11B): see region:\naaa\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 3), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 3), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 3), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 3), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 3), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file     line col  offset
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 0), "line=11: (0B): aaa\n", "line=11: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 12, npos, 1), "line=12: (1B): aaa\n", "line=12: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 13, npos, 2), "line=13: (2B): aaa\n", "line=13: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 14, npos, 3), "line=14: (3B): aaa\n", "line=14: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 10, 0   , npos), "line=10 col=0: \n", "line=10 col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, 0   , 0), "line=11 col=0 (0B): aaa\n                    |\n                    (here)\n", "line=11 col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 1), "line=12 col=1 (1B): aaa\n                     |\n                     (here)\n", "line=12 col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 2), "line=13 col=2 (2B): aaa\n                      |\n                      (here)\n", "line=13 col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 3), "line=14 col=3 (3B): aaa\n                       |\n                       (here)\n", "line=14 col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 4+0), "line=11: (4B): bbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1), "line=12: (5B): bbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2), "line=13: (6B): bbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3), "line=14: (7B): bbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , 4+0), "line=10 col=0 (4B): bbb\n                    |\n                    (here)\n", "line=10 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 0   , 4+1), "line=12 col=0 (5B): bbb\n                    |\n                    (here)\n", "line=12 col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 1), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 4+0 , 1), "line=11: (4B): bbb\nline=11: (4B): see region:\naaa\nbbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1 , 1), "line=12: (5B): bbb\nline=12: (5B): see region:\naaa\nbbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2 , 1), "line=13: (6B): bbb\nline=13: (6B): see region:\naaa\nbbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3 , 1), "line=14: (7B): bbb\nline=14: (7B): see region:\naaa\nbbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 1), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 4+0 , 1), "line=11 col=0 (4B): bbb\n                    |\n                    (here)\nline=11 col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "line=11 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1 , 1), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\nline=12 col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2 , 1), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\nline=13 col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3 , 1), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\nline=14 col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 2), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 4+0 , 2), "line=11: (4B): bbb\nline=11: (4B): see region:\naaa\nbbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1 , 2), "line=12: (5B): bbb\nline=12: (5B): see region:\naaa\nbbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2 , 2), "line=13: (6B): bbb\nline=13: (6B): see region:\naaa\nbbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3 , 2), "line=14: (7B): bbb\nline=14: (7B): see region:\naaa\nbbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 2), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 4+0 , 2), "line=11 col=0 (4B): bbb\n                    |\n                    (here)\nline=11 col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "line=11 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1 , 2), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\nline=12 col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2 , 2), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\nline=13 col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3 , 2), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\nline=14 col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 8+0), "line=11: (8B): ccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1), "line=12: (9B): ccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2), "line=13: (10B): ccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3), "line=14: (11B): ccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos), "line=10 col=0: \n", "line=10 col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, 0   , 8+0), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 1), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 1), "line=11: (8B): ccc\nline=11: (8B): see region:\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 1), "line=12: (9B): ccc\nline=12: (9B): see region:\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 1), "line=13: (10B): ccc\nline=13: (10B): see region:\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 1), "line=14: (11B): ccc\nline=14: (11B): see region:\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 1), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 1), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 1), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 1), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 1), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 2), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 2), "line=11: (8B): ccc\nline=11: (8B): see region:\naaa\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 2), "line=12: (9B): ccc\nline=12: (9B): see region:\naaa\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 2), "line=13: (10B): ccc\nline=13: (10B): see region:\naaa\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 2), "line=14: (11B): ccc\nline=14: (11B): see region:\naaa\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 2), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 2), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 2), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 2), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 2), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 3), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 3), "line=11: (8B): ccc\nline=11: (8B): see region:\naaa\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 3), "line=12: (9B): ccc\nline=12: (9B): see region:\naaa\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 3), "line=13: (10B): ccc\nline=13: (10B): see region:\naaa\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 3), "line=14: (11B): ccc\nline=14: (11B): see region:\naaa\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 3), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 3), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 3), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 3), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 3), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file      line     col   offset
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 0), "file.yml: (0B): aaa\n", "file.yml: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 1), "file.yml: (1B): aaa\n", "file.yml: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 2), "file.yml: (2B): aaa\n", "file.yml: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 3), "file.yml: (3B): aaa\n", "file.yml: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, 0   , npos), "file.yml: col=0: \n", "file.yml: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, 0   , 0), "file.yml: col=0 (0B): aaa\n                      |\n                      (here)\n", "file.yml: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 1), "file.yml: col=1 (1B): aaa\n                       |\n                       (here)\n", "file.yml: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 2), "file.yml: col=2 (2B): aaa\n                        |\n                        (here)\n", "file.yml: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 3), "file.yml: col=3 (3B): aaa\n                         |\n                         (here)\n", "file.yml: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 4+0), "file.yml: (4B): bbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1), "file.yml: (5B): bbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2), "file.yml: (6B): bbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3), "file.yml: (7B): bbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+1), "file.yml: col=0 (5B): bbb\n                      |\n                      (here)\n", "file.yml: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 1), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 4+0 , 1), "file.yml: (4B): bbb\nfile.yml: (4B): see region:\naaa\nbbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1 , 1), "file.yml: (5B): bbb\nfile.yml: (5B): see region:\naaa\nbbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2 , 1), "file.yml: (6B): bbb\nfile.yml: (6B): see region:\naaa\nbbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3 , 1), "file.yml: (7B): bbb\nfile.yml: (7B): see region:\naaa\nbbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 1), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0 , 1), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\nfile.yml: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1 , 1), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\nfile.yml: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2 , 1), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\nfile.yml: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3 , 1), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\nfile.yml: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 2), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 4+0 , 2), "file.yml: (4B): bbb\nfile.yml: (4B): see region:\naaa\nbbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1 , 2), "file.yml: (5B): bbb\nfile.yml: (5B): see region:\naaa\nbbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2 , 2), "file.yml: (6B): bbb\nfile.yml: (6B): see region:\naaa\nbbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3 , 2), "file.yml: (7B): bbb\nfile.yml: (7B): see region:\naaa\nbbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 2), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0 , 2), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\nfile.yml: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1 , 2), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\nfile.yml: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2 , 2), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\nfile.yml: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3 , 2), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\nfile.yml: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 8+0), "file.yml: (8B): ccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1), "file.yml: (9B): ccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2), "file.yml: (10B): ccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3), "file.yml: (11B): ccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos), "file.yml: col=0: \n", "file.yml: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 1), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 1), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 1), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 1), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 1), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 1), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 1), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 1), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 1), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 1), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 2), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 2), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\naaa\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 2), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\naaa\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 2), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\naaa\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 2), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\naaa\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 2), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 2), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 2), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 2), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 2), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 3), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 3), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\naaa\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 3), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\naaa\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 3), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\naaa\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 3), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\naaa\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 3), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 3), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 3), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 3), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 3), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file      line  col   offset
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 0), "file.yml:11: (0B): aaa\n", "file.yml:11: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 12, npos, 1), "file.yml:12: (1B): aaa\n", "file.yml:12: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 13, npos, 2), "file.yml:13: (2B): aaa\n", "file.yml:13: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 14, npos, 3), "file.yml:14: (3B): aaa\n", "file.yml:14: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 10, 0   , npos), "file.yml:10: col=0: \n", "file.yml:10: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, 0   , 0), "file.yml:11: col=0 (0B): aaa\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1), "file.yml:12: col=1 (1B): aaa\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2), "file.yml:13: col=2 (2B): aaa\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3), "file.yml:14: col=3 (3B): aaa\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 0, 2), "file.yml:11: col=0 (0B): aaa\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 0, 2), "file.yml:12: col=1 (1B): aaa\n                         ~~  (cols 0-2/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 0, 2), "file.yml:13: col=2 (2B): aaa\n                         ~~  (cols 0-2/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 0, 2), "file.yml:14: col=3 (3B): aaa\n                         ~~  (cols 0-2/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 1, 3), "file.yml:11: col=0 (0B): aaa\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 1, 3), "file.yml:12: col=1 (1B): aaa\n                          ~~  (cols 1-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 1, 3), "file.yml:13: col=2 (2B): aaa\n                          ~~  (cols 1-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 1, 3), "file.yml:14: col=3 (3B): aaa\n                          ~~  (cols 1-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 0, 3), "file.yml:11: col=0 (0B): aaa\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 0, 3), "file.yml:12: col=1 (1B): aaa\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 0, 3), "file.yml:13: col=2 (2B): aaa\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 0, 3), "file.yml:14: col=3 (3B): aaa\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 4+0), "file.yml:11: (4B): bbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1), "file.yml:12: (5B): bbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2), "file.yml:13: (6B): bbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3), "file.yml:14: (7B): bbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0), "file.yml:10: col=0 (4B): bbb\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1), "file.yml:12: col=0 (5B): bbb\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 0, 2), "file.yml:10: col=0 (4B): bbb\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 0, 2), "file.yml:12: col=0 (5B): bbb\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 0, 2), "file.yml:12: col=1 (5B): bbb\n                         ~~  (cols 0-2/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 0, 2), "file.yml:13: col=2 (6B): bbb\n                         ~~  (cols 0-2/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 0, 2), "file.yml:14: col=3 (7B): bbb\n                         ~~  (cols 0-2/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 1, 3), "file.yml:10: col=0 (4B): bbb\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 1, 3), "file.yml:12: col=0 (5B): bbb\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 1, 3), "file.yml:12: col=1 (5B): bbb\n                          ~~  (cols 1-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 1, 3), "file.yml:13: col=2 (6B): bbb\n                          ~~  (cols 1-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 1, 3), "file.yml:14: col=3 (7B): bbb\n                          ~~  (cols 1-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 0, 3), "file.yml:10: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 0, 3), "file.yml:12: col=0 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 1), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 4+0 , 1), "file.yml:11: (4B): bbb\nfile.yml:11: (4B): see region:\naaa\nbbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1 , 1), "file.yml:12: (5B): bbb\nfile.yml:12: (5B): see region:\naaa\nbbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2 , 1), "file.yml:13: (6B): bbb\nfile.yml:13: (6B): see region:\naaa\nbbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3 , 1), "file.yml:14: (7B): bbb\nfile.yml:14: (7B): see region:\naaa\nbbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 1), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 1), "file.yml:11: col=0 (4B): bbb\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 1), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 1), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 1), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 1, 0, 0, 3), "file.yml:11: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 1, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 1, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 1, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 2), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 4+0 , 2), "file.yml:11: (4B): bbb\nfile.yml:11: (4B): see region:\naaa\nbbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1 , 2), "file.yml:12: (5B): bbb\nfile.yml:12: (5B): see region:\naaa\nbbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2 , 2), "file.yml:13: (6B): bbb\nfile.yml:13: (6B): see region:\naaa\nbbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3 , 2), "file.yml:14: (7B): bbb\nfile.yml:14: (7B): see region:\naaa\nbbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 2), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 2), "file.yml:11: col=0 (4B): bbb\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 2), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 2), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 2), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 2, 0, 0, 3), "file.yml:11: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 2, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 2, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 2, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 8+0), "file.yml:11: (8B): ccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1), "file.yml:12: (9B): ccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2), "file.yml:13: (10B): ccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3), "file.yml:14: (11B): ccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos), "file.yml:10: col=0: \n", "file.yml:10: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 1), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 1), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 1), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 1), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 1), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 1), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 1), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 1), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 1), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 1), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 1, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 1, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 1, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 1, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 2), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 2), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\naaa\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 2), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\naaa\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 2), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\naaa\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 2), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\naaa\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 2), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 2), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 2), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 2), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 2), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 2, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 2, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 2, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 2, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 3), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 3), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\naaa\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 3), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\naaa\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 3), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\naaa\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 3), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\naaa\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 3), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 3), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 3), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 3), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 3), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 3, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 3, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 3, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 3, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");

    txt = R"(
0123456789112345678921234567893123456789412345678951234567896123456789
0a234567891a234567892a234567893a234567894a234567895a234567896a23456789
)";
    //                      line   offset
    //                      |  col |   linesbefore
    //                      |  |   |   |  linesafter
    //                      |  |   |   |  |  hlfirstcol
    //                      |  |   |   |  |  |  hllastcol
    //                      |  |   |   |  |  |  |  maxlen
    //                      |  |   |   |  |  |  |  |
    mkmsg((txt, "file.yml", 2,  3, 73, 0, 0, 0, 0, 20), "file.yml:2: col=3 (73B): 0a234567891a23456789[...] (showing columns 0-20/70)\n                            |\n                            (here)\n", "file.yml:2: col=3 (73B): error:\nerror:\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 30), "file.yml:2: col=23 (93B): 0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                                                 |\n                                                 (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 20), "file.yml:2: col=23 (93B): [...]4567891a234567892a23[...] (showing columns 4-24/70)\n                                                  |\n                                                  (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 15), "file.yml:2: col=23 (93B): [...]91a234567892a23[...] (showing columns 9-24/70)\n                                             |\n                                             (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 10), "file.yml:2: col=23 (93B): [...]4567892a23[...] (showing columns 14-24/70)\n                                        |\n                                        (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\n");

    txt = R"(
..........++++++++++0123456789
..........0123456789
0123456789
0a234567891a234567892a234567893a234567894a234567895a234567896a23456789
)";
    //                      line   offset
    //                      |  col |   linesbefore
    //                      |  |   |   |  linesafter
    //                      |  |   |   |  |  hlfirstcol
    //                      |  |   |   |  |  |  hllastcol
    //                      |  |   |   |  |  |  |  maxlen
    //                      |  |   |   |  |  |  |  |
    mkmsg((txt, "file.yml", 4,  3, 67, 3, 0, 0, 0, 20), "file.yml:4: col=3 (67B): 0a234567891a23456789[...] (showing columns 0-20/70)\n                            |\n                            (here)\nfile.yml:4: col=3 (67B): see region:\n..........++++++++++[...] (showing columns 0-20/30)\n..........0123456789\n0123456789\n0a234567891a23456789[...] (showing columns 0-20/70)\n   |\n   (here)\n", "file.yml:4: col=3 (67B): error:\nerror:\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     ..........++++++++++[...] (showing columns 0-20/30)\nerror:     ..........0123456789\nerror:     0123456789\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 30), "file.yml:4: col=23 (87B): 0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                                                 |\n                                                 (here)\nfile.yml:4: col=23 (87B): see region:\n..........++++++++++0123456789\n..........0123456789\n0123456789\n0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                       |\n                       (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\nerror:\nerror: see region:\nerror:\nerror:     ..........++++++++++0123456789\nerror:     ..........0123456789\nerror:     0123456789\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 20), "file.yml:4: col=23 (87B): [...]4567891a234567892a23[...] (showing columns 4-24/70)\n                                                  |\n                                                  (here)\nfile.yml:4: col=23 (87B): see region:\n[...]......++++++++++0123[...] (showing columns 4-24/30)\n[...]......0123456789          (showing columns 4-20/20)\n[...]456789                    (showing columns 4-10/10)\n[...]4567891a234567892a23[...] (showing columns 4-24/70)\n                        |\n                        (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]......++++++++++0123[...] (showing columns 4-24/30)\nerror:     [...]......0123456789          (showing columns 4-20/20)\nerror:     [...]456789                    (showing columns 4-10/10)\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 17), "file.yml:4: col=23 (87B): [...]7891a234567892a23[...] (showing columns 7-24/70)\n                                               |\n                                               (here)\nfile.yml:4: col=23 (87B): see region:\n[...]...++++++++++0123[...] (showing columns 7-24/30)\n[...]...0123456789          (showing columns 7-20/20)\n[...]789                    (showing columns 7-10/10)\n[...]7891a234567892a23[...] (showing columns 7-24/70)\n                     |\n                     (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]7891a234567892a23[...] (showing columns 7-24/70)\nerror:                          |\nerror:                          (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]...++++++++++0123[...] (showing columns 7-24/30)\nerror:     [...]...0123456789          (showing columns 7-20/20)\nerror:     [...]789                    (showing columns 7-10/10)\nerror:     [...]7891a234567892a23[...] (showing columns 7-24/70)\nerror:                          |\nerror:                          (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 16), "file.yml:4: col=23 (87B): [...]891a234567892a23[...] (showing columns 8-24/70)\n                                              |\n                                              (here)\nfile.yml:4: col=23 (87B): see region:\n[...]..++++++++++0123[...] (showing columns 8-24/30)\n[...]..0123456789          (showing columns 8-20/20)\n[...]89                    (showing columns 8-10/10)\n[...]891a234567892a23[...] (showing columns 8-24/70)\n                    |\n                    (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]891a234567892a23[...] (showing columns 8-24/70)\nerror:                         |\nerror:                         (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]..++++++++++0123[...] (showing columns 8-24/30)\nerror:     [...]..0123456789          (showing columns 8-20/20)\nerror:     [...]89                    (showing columns 8-10/10)\nerror:     [...]891a234567892a23[...] (showing columns 8-24/70)\nerror:                         |\nerror:                         (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 15), "file.yml:4: col=23 (87B): [...]91a234567892a23[...] (showing columns 9-24/70)\n                                             |\n                                             (here)\nfile.yml:4: col=23 (87B): see region:\n[...].++++++++++0123[...] (showing columns 9-24/30)\n[...].0123456789          (showing columns 9-20/20)\n[...]9                    (showing columns 9-10/10)\n[...]91a234567892a23[...] (showing columns 9-24/70)\n                   |\n                   (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\nerror:\nerror: see region:\nerror:\nerror:     [...].++++++++++0123[...] (showing columns 9-24/30)\nerror:     [...].0123456789          (showing columns 9-20/20)\nerror:     [...]9                    (showing columns 9-10/10)\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 10), "file.yml:4: col=23 (87B): [...]4567892a23[...] (showing columns 14-24/70)\n                                        |\n                                        (here)\nfile.yml:4: col=23 (87B): see region:\n[...]++++++0123[...] (showing columns 14-24/30)\n[...]456789          (showing columns 14-20/20)\n[...]                (not showing, columns=10)\n[...]4567892a23[...] (showing columns 14-24/70)\n              |\n              (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]++++++0123[...] (showing columns 14-24/30)\nerror:     [...]456789          (showing columns 14-20/20)\nerror:     [...]                (not showing, columns=10)\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0,  5), "file.yml:4: col=23 (87B): [...]92a23[...] (showing columns 19-24/70)\n                                   |\n                                   (here)\nfile.yml:4: col=23 (87B): see region:\n[...]+0123[...] (showing columns 19-24/30)\n[...]9          (showing columns 19-20/20)\n[...]           (not showing, columns=10)\n[...]92a23[...] (showing columns 19-24/70)\n         |\n         (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]92a23[...] (showing columns 19-24/70)\nerror:              |\nerror:              (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]+0123[...] (showing columns 19-24/30)\nerror:     [...]9          (showing columns 19-20/20)\nerror:     [...]           (not showing, columns=10)\nerror:     [...]92a23[...] (showing columns 19-24/70)\nerror:              |\nerror:              (here)\n");
}

// FIXME this is here merely to avoid a linker error
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
