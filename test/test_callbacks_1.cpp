#include "./test_lib/test_utils.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
#include "c4/yml/error.def.hpp"
#include "c4/dump.hpp"
#endif
#include <stdexcept>
#include <csetjmp>
#include <cstring>
#include "c4/yml/extra/event_handler_ints.hpp"


C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen: This function or variable may be unsafe
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated-declarations") // fopen is deprecated

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static_assert(std::is_same<std::underlying_type<decltype(c4::yml::npos)>::type, size_t>::value, "invalid type");
static_assert(std::is_same<std::underlying_type<decltype(c4::yml::NONE)>::type, id_type>::value, "invalid type");
static_assert(size_t(c4::yml::npos) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...
static_assert(size_t(c4::yml::NONE) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...

template<class T>
constexpr bool check_remove_cvref()
{
    static_assert(std::is_same<T, typename detail::_remove_cvref<T>::type>::value, "remove cvref");
    static_assert(std::is_same<T, typename detail::_remove_cvref<T&>::type>::value, "remove cvref");
    static_assert(std::is_same<T, typename detail::_remove_cvref<T&&>::type>::value, "remove cvref");
    static_assert(std::is_same<T, typename detail::_remove_cvref<const T>::type>::value, "remove cvref");
    static_assert(std::is_same<T, typename detail::_remove_cvref<const T&>::type>::value, "remove cvref");
    static_assert(std::is_same<T, typename detail::_remove_cvref<const T&&>::type>::value, "remove cvref");
    return true;
}
static_assert(check_remove_cvref<substr>(), "remove cvref");
static_assert(check_remove_cvref<csubstr>(), "remove cvref");
static_assert(check_remove_cvref<char *>(), "remove cvref");
static_assert(check_remove_cvref<const char *>(), "remove cvref");

static_assert(c4::is_string<char[2]>::value, "dump directly");
static_assert(c4::is_string<char(&)[2]>::value, "dump directly");
static_assert(c4::is_string<char(&&)[2]>::value, "dump directly");
static_assert(c4::is_string<const char[2]>::value, "dump directly");
static_assert(c4::is_string<const char(&&)[2]>::value, "dump directly");
static_assert(c4::is_string<substr>::value, "dump directly");
static_assert(c4::is_string<csubstr>::value, "dump directly");
static_assert(c4::is_string<std::string>::value, "dump directly");

template<class T>
constexpr bool check_dump_directly()
{
    static_assert(detail::_dump_directly<T>::value, "dump directly");
    static_assert(detail::_dump_directly<T&>::value, "dump directly");
    static_assert(detail::_dump_directly<T&&>::value, "dump directly");
    static_assert(detail::_dump_directly<T const>::value, "dump directly");
    static_assert(detail::_dump_directly<T const&>::value, "dump directly");
    static_assert(detail::_dump_directly<T const&&>::value, "dump directly");
    return true;
}
static_assert(check_dump_directly<substr>(), "dump directly");
static_assert(check_dump_directly<csubstr>(), "dump directly");
static_assert(check_dump_directly<char*>(), "dump directly");
static_assert(check_dump_directly<const char*>(), "dump directly");
static_assert(check_dump_directly<char[2]>(), "dump directly");
static_assert(check_dump_directly<char(&)[2]>(), "dump directly");
static_assert(check_dump_directly<char(&&)[2]>(), "dump directly");
static_assert(check_dump_directly<std::string>(), "dump directly");


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(cb, grow_buf)
{
    {
        substr buf;
        ASSERT_EQ(buf.len, 0);
        ASSERT_EQ(buf.str, nullptr);
    }
    Callbacks cb = get_callbacks();
    substr buf = {};
    auto isall = [&buf](size_t first, size_t last, char c){
        return buf.range(first, last).first_not_of(c) == csubstr::npos;
    };
    ASSERT_EQ(buf.len, 0);
    ASSERT_EQ(buf.str, nullptr);
    buf = detail::resize(buf, 10, cb);
    ASSERT_EQ(buf.len, 10);
    ASSERT_NE(buf.str, nullptr);
    buf.fill('0');
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_EQ(buf, "0000000000");

    char *prev = buf.str;
    buf = detail::resize(buf, 9, cb);
    ASSERT_EQ(buf.str, prev);
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_EQ(buf, "0000000000");

    buf = detail::resize(buf, 10, cb);
    ASSERT_EQ(buf.str, prev);
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_EQ(buf, "0000000000");

    buf = detail::resize(buf, 11, cb);
    ASSERT_NE(buf.str, prev);
    ASSERT_TRUE(isall(0, 10, '0'));
    buf.sub(10).fill('1');
    ASSERT_TRUE(isall(10, 11, '1'));
    ASSERT_EQ(buf, "00000000001");
    prev = buf.str;

    buf = detail::resize(buf, 16, cb);
    ASSERT_NE(buf.str, prev);
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_TRUE(isall(10, 11, '1'));
    buf.sub(11).fill('2');
    ASSERT_TRUE(isall(11, 16, '2'));
    ASSERT_EQ(buf, "0000000000122222");

    buf = detail::destroy(buf, cb);
    ASSERT_EQ(buf.len, 0);
    ASSERT_EQ(buf.str, nullptr);
}

TEST(cb, grow_buf_cap)
{
    Callbacks cb = get_callbacks();
    substr buf = {};
    ASSERT_EQ(buf.len, 0);
    ASSERT_EQ(buf.str, nullptr);
    auto isall = [&buf](size_t first, size_t last, char c){
        return buf.range(first, last).first_not_of(c) == csubstr::npos;
    };
    auto growcap = [&](size_t sz, size_t cap){
        ASSERT_GT(cap, buf.len);
        ASSERT_LE(sz, buf.len);
        buf.str = detail::grow_buf(buf.str, sz, buf.len, cap, cb);
        buf.len = cap;
        buf.sub(sz).fill('-');
    };
    growcap(0, 10);
    ASSERT_EQ(buf.len, 10);
    ASSERT_NE(buf.str, nullptr);
    ASSERT_EQ(buf, "----------");
    buf.fill('0');
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_EQ(buf, "0000000000");
    growcap(10, 20);
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_TRUE(isall(10, 20, '-'));
    ASSERT_EQ(buf, "0000000000----------");
    growcap(10, 24);
    ASSERT_TRUE(isall(0, 10, '0'));
    ASSERT_TRUE(isall(10, 24, '-'));
    ASSERT_EQ(buf, "0000000000--------------");

    buf = detail::destroy(buf, cb);
    ASSERT_EQ(buf.len, 0);
    ASSERT_EQ(buf.str, nullptr);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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

bool print_msg = false;

namespace { // anon
C4_NO_INLINE void dump2file(csubstr s, FILE *f)
{
    // using fwrite() is more portable than using fprintf("%.*s") which
    // is not available in some embedded platforms
    if(print_msg && s.len)
        fwrite(s.str, 1, s.len, f); // NOLINT
}

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
    if(print_msg)
    {
        fputc('\n', f); // NOLINT
        fflush(f); // NOLINT
    }
}

void err_parse_print(csubstr msg, ErrorDataParse const& errdata, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    err_parse_format([f](csubstr s){ dump2file(s, f); }, msg, errdata);
    if(print_msg)
    {
        fputc('\n', f); // NOLINT
        fflush(f); // NOLINT
    }
}

void err_visit_print(csubstr msg, ErrorDataVisit const& errdata, FILE *f=nullptr)
{
    if(!f)
        f = stderr;
    err_visit_format([f](csubstr s){ dump2file(s, f); }, msg, errdata);
    if(print_msg)
    {
        fputc('\n', f); // NOLINT
        fflush(f); // NOLINT
    }
}

void test_loc_print(Location const& cpploc)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    location_print(cpploc, file);
    if(print_msg)
    {
        fprintf(file, "\n"); // NOLINT
        fflush(file); // NOLINT
    }
    if(file)
        fclose(file); // NOLINT
}

void test_error_basic_print(csubstr msg, ErrorDataBasic const& errdata)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_basic_print(msg, errdata, file);
    if(file)
        fclose(file); // NOLINT
}

void test_error_parse_print(csubstr msg, ErrorDataParse const& errdata)
{
    FILE *file = fopen(null_name, "wb"); // NOLINT
    err_parse_print(msg, errdata, file);
    if(file)
        fclose(file); // NOLINT
}

void test_error_visit_print(csubstr msg, ErrorDataVisit const& errdata)
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
    #ifdef RYML_WITH_EXCEPTIONS_
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
    #ifdef RYML_WITH_EXCEPTIONS_
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
    #ifdef RYML_WITH_EXCEPTIONS_
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

void * stored_resident_mem;
size_t stored_resident_length;
void * stored_resident_user_data;
int num_make_resident_calls;
void test_make_resident_impl(void *mem, size_t length, void *user_data)
{
    stored_resident_mem = mem;
    stored_resident_length = length;
    stored_resident_user_data = user_data;
    ++num_make_resident_calls;
}

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
    EXPECT_EQ(cb.m_user_data, nullptr);
#ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_NE(cb.m_allocate, nullptr);
    EXPECT_NE(cb.m_free, nullptr);
    EXPECT_NE(cb.m_error_basic, nullptr);
    EXPECT_NE(cb.m_error_parse, nullptr);
    EXPECT_NE(cb.m_error_visit, nullptr);
#endif
}

} // namespace anon


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Callbacks, ctor)
{
    Callbacks cb;
    EXPECT_NE(cb.m_allocate, &test_allocate_impl);
    EXPECT_NE(cb.m_free, &test_free_impl);
    EXPECT_NE(cb.m_error_basic, &test_error_basic_impl);
    EXPECT_NE(cb.m_error_parse, &test_error_parse_impl);
    EXPECT_NE(cb.m_error_visit, &test_error_visit_impl);
    EXPECT_EQ(cb.m_make_resident, nullptr);
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
    EXPECT_EQ(cb2.m_user_data, &data);
    EXPECT_EQ(cb2.m_allocate, &test_allocate_impl);
    EXPECT_EQ(cb2.m_free, &test_free_impl);
    EXPECT_EQ(cb2.m_error_basic, &test_error_basic_impl);
    EXPECT_EQ(cb2.m_make_resident, nullptr);
#ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_NE(cb2.m_error_parse, nullptr);
    EXPECT_NE(cb2.m_error_visit, nullptr);
#else
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

TEST(Callbacks, set_make_resident)
{
    Callbacks cb;
    EXPECT_EQ(cb.m_make_resident, nullptr);
    cb.set_make_resident(test_make_resident_impl);
    EXPECT_EQ(cb.m_make_resident, test_make_resident_impl);
    cb.set_make_resident(nullptr);
    EXPECT_EQ(cb.m_make_resident, nullptr); // no default
}


TEST(Callbacks, get)
{
    Callbacks cb = get_callbacks();
    check_callbacks_default(cb);
}

TEST(Callbacks, set)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
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
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
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
    set_callbacks(before); // DON'T FORGET!
}

TEST(Callbacks, eq)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
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

TEST(Callbacks, cmp_make_resident)
{
    Callbacks before = get_callbacks();
    Callbacks cb = get_callbacks();
    EXPECT_EQ(cb, before);
    EXPECT_TRUE(cb == before);
    EXPECT_FALSE(!(cb == before));
    cb.set_make_resident(test_make_resident_impl);
    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_FALSE(!(cb != before));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(allocate_and_free, basic)
{
    Callbacks before = get_callbacks();
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
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
    set_callbacks(before); // DON'T FORGET!
}

TEST(allocate_and_make_resident, without_make_resident_only_allocates)
{
    Callbacks cb = mk_test_callbacks();
    EXPECT_EQ(cb.m_make_resident, nullptr);
    num_make_resident_calls = 0;
    void *mem = cb.allocate_and_make_resident(32, /*hint*/nullptr);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
    EXPECT_EQ(num_make_resident_calls, 0);
    cb.m_free(mem, 32u, cb.m_user_data);
}

TEST(allocate_and_make_resident, with_make_resident)
{
    int data = 1;
    Callbacks cb = mk_test_callbacks()
        .set_user_data(&data)
        .set_make_resident(&test_make_resident_impl);
    num_make_resident_calls = 0;
    void *mem = cb.allocate_and_make_resident(32, /*hint*/nullptr);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
    EXPECT_EQ(num_make_resident_calls, 1);
    EXPECT_EQ(stored_resident_mem, mem);
    EXPECT_EQ(stored_resident_length, 32u);
    EXPECT_EQ(stored_resident_user_data, &data);
    cb.m_free(mem, 32u, cb.m_user_data);
}

TEST(make_resident, writes_one_zero_per_page)
{
    const size_t len = size_t(1) << 18; // 256 KiB
    char *mem = static_cast<char*>(::malloc(len));
    ASSERT_NE(mem, nullptr);
    memset(mem, 'a', len);
    make_resident(mem, len, nullptr);
    size_t num_zeros = 0;
    size_t first = len, second = len;
    for(size_t i = 0; i < len; ++i)
    {
        if(mem[i] == '\0')
        {
            if(num_zeros == 0)
                first = i;
            else if(num_zeros == 1)
                second = i;
            ++num_zeros;
        }
        else
        {
            EXPECT_EQ(mem[i], 'a');
        }
    }
    #if defined(C4_WIN) || defined(C4_UNIX) || defined(C4_LINUX) || defined(C4_MACOS) || defined(C4_IOS)
    EXPECT_EQ(first, 0u);
    ASSERT_GT(num_zeros, 1u);
    const size_t page = second - first;
    EXPECT_GE(page, 512u);
    EXPECT_EQ(page & (page - 1u), 0u); // a power of two
    EXPECT_EQ(num_zeros, (len + page - 1u) / page);
    for(size_t i = 0; i < len; i += page)
        EXPECT_EQ(mem[i], '\0');
    #else
    EXPECT_EQ(num_zeros, 0u);
    #endif
    ::free(mem);
}

TEST(make_resident, empty_or_null)
{
    char mem[4] = {'a', 'a', 'a', 'a'};
    make_resident(mem, 0, nullptr);
    EXPECT_EQ(mem[0], 'a');
    make_resident(nullptr, 0, nullptr);
    make_resident(nullptr, 100, nullptr);
}

TEST(make_resident, can_be_set)
{
    Callbacks cb = mk_test_callbacks().set_make_resident(&make_resident);
    EXPECT_EQ(cb.m_make_resident, &make_resident);
    void *mem = cb.allocate_and_make_resident(64, /*hint*/nullptr);
    ASSERT_NE(mem, nullptr);
    EXPECT_EQ(static_cast<char*>(mem)[0], '\0');
    cb.m_free(mem, 64u, cb.m_user_data);
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

#ifdef RYML_WITH_EXCEPTIONS_
#define RYML_IF_EXC(...) __VA_ARGS__
#else
#define RYML_IF_EXC(...)
#endif

template<class... Args>
void test_error_basic(Location const& cpploc, const char* errmsg, Args const& ...args)
{
    Callbacks before = get_callbacks();
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
        set_callbacks(before);
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
        set_callbacks(before);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef RYML_WITH_EXCEPTIONS_
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
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
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
    Callbacks before = get_callbacks();
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
        if(print_msg)
        {
            std::cout << exc_msg << "\n";
            std::cout << stored_msg_full << "\n";
        }
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    set_callbacks(before);
}
TEST(check, basic)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    bool disregard_this_message = false;
    csubstr msg = "disregard_this_message";
    csubstr fmsg = "disregard_this_message: extra args: 1,2";
    test_check_basic([&]{ RYML_CHECK_BASIC_(disregard_this_message); }, msg);
    test_check_basic([&]{ RYML_CHECK_BASIC_CB_(get_callbacks(), disregard_this_message); }, msg);
    test_check_basic([&]{ RYML_CHECK_BASIC_CB_MSG_(disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    test_check_basic([&]{ RYML_CHECK_BASIC_MSG_CB_(get_callbacks(), disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    #if RYML_USE_ASSERT && !RYML_ABORT_ON_ASSERT && !C4_ABORT_ON_ASSERT
    test_check_basic([&]{ RYML_ASSERT_BASIC_(disregard_this_message); }, msg);
    test_check_basic([&]{ RYML_ASSERT_BASIC_CB_(get_callbacks(), disregard_this_message); }, msg);
    test_check_basic([&]{ RYML_ASSERT_BASIC_MSG_(disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    test_check_basic([&]{ RYML_ASSERT_BASIC_MSG_CB_(get_callbacks(), disregard_this_message, "extra args: {},{}", 1, 2); }, fmsg);
    #endif
}

#ifdef RYML_WITH_EXCEPTIONS_
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
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    testmsg<ExceptionBasic>(ErrorDataBasic(Location{}));
}
TEST(error, parse_message_is_truncated)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    testmsg<ExceptionParse>(ErrorDataParse(Location{}, Location{}));
}
TEST(error, visit_message_is_truncated)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    Tree tree;
    id_type node = {};
    testmsg<ExceptionVisit>(ErrorDataVisit(Location{}, &tree, node));
}
#endif

TEST(error, loc_print_accepts_null_file)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    test_loc_print(Location{});
    test_loc_print(RYML_LOC_HERE());
    location_print(Location{});
    location_print(Location{}, nullptr);
    location_print(RYML_LOC_HERE());
    if(print_msg)
        fprintf(stderr, "\n"); // NOLINT
    location_print(RYML_LOC_HERE(), nullptr);
    if(print_msg)
        fprintf(stderr, "\n"); // NOLINT
}

TEST(error, basic_print_accepts_null_file)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    csubstr msg = "(disregard this message)";
    err_basic_print(msg, Location{});
    err_basic_print(msg, Location{}, nullptr);
    err_basic_print(msg, RYML_LOC_HERE());
    err_basic_print(msg, RYML_LOC_HERE(), nullptr);
}

TEST(error, parse_print_accepts_null_file)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    csubstr msg = "(disregard this message)";
    err_parse_print(msg, {Location{}, Location{}});
    err_parse_print(msg, {Location{}, Location{}}, nullptr);
    err_parse_print(msg, {RYML_LOC_HERE(), Location{}});
    err_parse_print(msg, {RYML_LOC_HERE(), Location{}}, nullptr);
    err_parse_print(msg, {Location{}, RYML_LOC_HERE()});
    err_parse_print(msg, {Location{}, RYML_LOC_HERE()}, nullptr);
    err_parse_print(msg, {RYML_LOC_HERE(), RYML_LOC_HERE()});
    err_parse_print(msg, {RYML_LOC_HERE(), RYML_LOC_HERE()}, nullptr);
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
}

TEST(error, visit_print_accepts_null_file)
{
    EXPECT_NE(get_callbacks().m_allocate, nullptr);
    csubstr msg = "(disregard this message)";
    Tree tree;
    id_type node = {};
    EXPECT_NE(tree.callbacks().m_allocate, nullptr);
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
    Callbacks before = get_callbacks();
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
        set_callbacks(before);
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
        set_callbacks(before);
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
        set_callbacks(before);
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
        set_callbacks(before);
        EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    }
    //
    #ifdef RYML_WITH_EXCEPTIONS_
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
    Callbacks before = get_callbacks();
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
        if(print_msg)
            std::cout << stored_msg_full << "\n";
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    set_callbacks(before);
}
TEST(check, parse)
{
    bool disregard_this_message = false;
    csubstr msg = "disregard_this_message";
    csubstr fmsg = "disregard_this_message: extra args: 1,2";
    Location ymlloc = {};
    {
        SCOPED_TRACE("here 0");
        test_check_parse([&]{ RYML_CHECK_PARSE_(disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 1");
        test_check_parse([&]{ RYML_CHECK_PARSE_CB_(get_callbacks(), disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 2");
        test_check_parse([&]{ RYML_CHECK_PARSE_CB_MSG_(disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 3");
        test_check_parse([&]{ RYML_CHECK_PARSE_MSG_CB_(get_callbacks(), disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #if RYML_USE_ASSERT && !RYML_ABORT_ON_ASSERT && !C4_ABORT_ON_ASSERT
    {
        SCOPED_TRACE("here 4");
        test_check_parse([&]{ RYML_ASSERT_PARSE_(disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 5");
        test_check_parse([&]{ RYML_ASSERT_PARSE_CB_(get_callbacks(), disregard_this_message, ymlloc); }, msg);
    }
    {
        SCOPED_TRACE("here 6");
        test_check_parse([&]{ RYML_ASSERT_PARSE_MSG_(disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 7");
        test_check_parse([&]{ RYML_ASSERT_PARSE_MSG_CB_(get_callbacks(), disregard_this_message, ymlloc, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #endif
}


//-----------------------------------------------------------------------------

template<class... Args>
void test_error_visit(Location const& cpploc, Tree const* t, id_type id, const char* errmsg, Args const& ...args)
{
    Callbacks before = get_callbacks();
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
        set_callbacks(before);
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
        set_callbacks(before);
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
        set_callbacks(before);
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
        set_callbacks(before);
        EXPECT_NE(get_callbacks().m_error_visit, &test_error_visit_impl);
    }
    //
    #ifdef RYML_WITH_EXCEPTIONS_
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
    Callbacks before = get_callbacks();
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
        if(print_msg)
            std::cout << stored_msg_full << "\n";
        RYML_IF_EXC(EXPECT_TRUE(exc_ok));
        RYML_IF_EXC(EXPECT_EQ(exc_msg_full, stored_msg_full));
        EXPECT_NE(csubstr::npos, to_csubstr(exc_msg).find(expected_msg));
    }
    set_callbacks(before);
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
        test_check_visit([&]{ RYML_CHECK_VISIT_(disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 1");
        test_check_visit([&]{ RYML_CHECK_VISIT_CB_(get_callbacks(), disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 2");
        test_check_visit([&]{ RYML_CHECK_VISIT_CB_MSG_(disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 3");
        test_check_visit([&]{ RYML_CHECK_VISIT_MSG_CB_(get_callbacks(), disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #if RYML_USE_ASSERT && !RYML_ABORT_ON_ASSERT && !C4_ABORT_ON_ASSERT
    {
        SCOPED_TRACE("here 4");
        test_check_visit([&]{ RYML_ASSERT_VISIT_(disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 5");
        test_check_visit([&]{ RYML_ASSERT_VISIT_CB_(get_callbacks(), disregard_this_message, &tree, id); }, msg);
    }
    {
        SCOPED_TRACE("here 6");
        test_check_visit([&]{ RYML_ASSERT_VISIT_MSG_(disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    {
        SCOPED_TRACE("here 7");
        test_check_visit([&]{ RYML_ASSERT_VISIT_MSG_CB_(get_callbacks(), disregard_this_message, &tree, id, "extra args: {},{}", 1, 2); }, fmsg);
    }
    #endif
}

TEST(RYML_CHECK, basic)
{
    Callbacks before = get_callbacks();
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    ASSERT_EQ(get_callbacks(), mk_test_callbacks());
    const size_t the_line = (size_t)(__LINE__ + 3); // careful
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        RYML_CHECK_BASIC_(false);  // keep both statements in the same line
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
    set_callbacks(before);
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
}


TEST(RYML_ASSERT, basic)
{
    Callbacks before = get_callbacks();
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
    set_callbacks(mk_test_callbacks());
    stored_msg = "";
    stored_cpploc = {};
    const size_t the_line = (size_t)(__LINE__ + 4); // careful
    C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env_expect_error) == 0))
    {
        #if !RYML_ABORT_ON_ASSERT && !C4_ABORT_ON_ASSERT
        RYML_ASSERT_BASIC_(false);
        #endif
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        ;
    }
    #if RYML_USE_ASSERT && !RYML_ABORT_ON_ASSERT && !C4_ABORT_ON_ASSERT
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
    set_callbacks(before);
    EXPECT_NE(get_callbacks().m_error_parse, &test_error_parse_impl);
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_CLANG_POP
