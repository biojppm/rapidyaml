#include "./test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#endif
#include <stdexcept>


namespace c4 {
namespace yml {

static_assert(std::is_same<std::underlying_type<decltype(c4::yml::npos)>::type, size_t>::value, "invalid type");
static_assert(std::is_same<std::underlying_type<decltype(c4::yml::NONE)>::type, size_t>::value, "invalid type");
static_assert(size_t(c4::yml::npos) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...
static_assert(size_t(c4::yml::NONE) == ((size_t)-1), "invalid value"); // some debuggers show the wrong value...

std::string stored_msg;
Location stored_location;
void * stored_mem;
size_t stored_length;

void test_error_impl(const char* msg, size_t length, Location loc, void * /*user_data*/)
{
    stored_msg = std::string(msg, length);
    stored_location = loc;
}

void* test_allocate_impl(size_t length, void * /*hint*/, void * /*user_data*/)
{
    void *mem = ::malloc(length);
    stored_length = length;
    stored_mem = mem;
    if(mem == nullptr)
    {
        const char msg[] = "could not allocate memory";
        test_error_impl(msg, sizeof(msg)-1, {}, nullptr);
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


TEST(Callbacks, ctor)
{
    {
        Callbacks cb;
        EXPECT_NE(cb.m_allocate, &test_allocate_impl);
        EXPECT_NE(cb.m_free, &test_free_impl);
        EXPECT_NE(cb.m_error, &test_error_impl);
        #ifndef RYML_NO_DEFAULT_CALLBACKS
        EXPECT_EQ(cb.m_user_data, nullptr);
        EXPECT_NE(cb.m_allocate, nullptr);
        EXPECT_NE(cb.m_free, nullptr);
        EXPECT_NE(cb.m_error, nullptr);
        #else
        EXPECT_EQ(cb.m_user_data, nullptr);
        EXPECT_EQ(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, nullptr);
        EXPECT_EQ(cb.m_error, nullptr);
        #endif
    }
    {
        Callbacks cb((void*)0xff, nullptr, nullptr, nullptr);
        EXPECT_NE(cb.m_allocate, &test_allocate_impl);
        EXPECT_NE(cb.m_free, &test_free_impl);
        EXPECT_NE(cb.m_error, &test_error_impl);
        #ifndef RYML_NO_DEFAULT_CALLBACKS
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_NE(cb.m_allocate, nullptr);
        EXPECT_NE(cb.m_free, nullptr);
        EXPECT_NE(cb.m_error, nullptr);
        #else
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_EQ(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, nullptr);
        EXPECT_EQ(cb.m_error, nullptr);
        #endif
    }
    {
        Callbacks cb((void*)0xff, &test_allocate_impl, nullptr, nullptr);
        #ifndef RYML_NO_DEFAULT_CALLBACKS
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_EQ(cb.m_allocate, &test_allocate_impl);
        EXPECT_NE(cb.m_free, nullptr);
        EXPECT_NE(cb.m_error, nullptr);
        #else
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_EQ(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, nullptr);
        EXPECT_EQ(cb.m_error, nullptr);
        #endif
    }
    {
        Callbacks cb((void*)0xff, nullptr, &test_free_impl, nullptr);
        #ifndef RYML_NO_DEFAULT_CALLBACKS
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_NE(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, &test_free_impl);
        EXPECT_NE(cb.m_error, nullptr);
        #else
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_EQ(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, &test_free_impl);
        EXPECT_EQ(cb.m_error, nullptr);
        #endif
    }
    {
        Callbacks cb((void*)0xff, nullptr, nullptr, &test_error_impl);
        #ifndef RYML_NO_DEFAULT_CALLBACKS
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_NE(cb.m_allocate, nullptr);
        EXPECT_NE(cb.m_free, nullptr);
        EXPECT_EQ(cb.m_error, &test_error_impl);
        #else
        EXPECT_EQ(cb.m_user_data, (void*)0xff);
        EXPECT_EQ(cb.m_allocate, nullptr);
        EXPECT_EQ(cb.m_free, nullptr);
        EXPECT_EQ(cb.m_error, test_error_impl);
        #endif
    }
}

TEST(Callbacks, get)
{
    Callbacks cb = get_callbacks();
    EXPECT_NE(cb.m_allocate, &test_allocate_impl);
    EXPECT_NE(cb.m_free, &test_free_impl);
    EXPECT_NE(cb.m_error, &test_error_impl);
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_NE(cb.m_allocate, nullptr);
    EXPECT_NE(cb.m_free, nullptr);
    EXPECT_NE(cb.m_error, nullptr);
    #else
    EXPECT_EQ(cb.m_user_data, nullptr);
    EXPECT_EQ(cb.m_allocate, nullptr);
    EXPECT_EQ(cb.m_free, nullptr);
    EXPECT_EQ(cb.m_error, nullptr);
    #endif
}

TEST(Callbacks, set)
{
    Callbacks before = get_callbacks();
    Callbacks cb((void*)0xff, &test_allocate_impl, &test_free_impl, &test_error_impl);

    set_callbacks(cb);
    Callbacks after = get_callbacks();
    EXPECT_EQ(cb.m_user_data, after.m_user_data);
    EXPECT_EQ(cb.m_allocate, after.m_allocate);
    EXPECT_EQ(cb.m_free, after.m_free);
    EXPECT_EQ(cb.m_error, after.m_error);

    set_callbacks(before);
    after = get_callbacks();
    EXPECT_EQ(before.m_user_data, after.m_user_data);
    EXPECT_EQ(before.m_allocate, after.m_allocate);
    EXPECT_EQ(before.m_free, after.m_free);
    EXPECT_EQ(before.m_error, after.m_error);

    set_callbacks(cb);
    after = get_callbacks();
    EXPECT_EQ(cb.m_user_data, after.m_user_data);
    EXPECT_EQ(cb.m_allocate, after.m_allocate);
    EXPECT_EQ(cb.m_free, after.m_free);
    EXPECT_EQ(cb.m_error, after.m_error);
}

TEST(Callbacks, reset)
{
    Callbacks before = get_callbacks();
    Callbacks cb((void*)0xff, &test_allocate_impl, &test_free_impl, &test_error_impl);

    set_callbacks(cb);
    Callbacks after = get_callbacks();
    EXPECT_EQ(cb.m_user_data, after.m_user_data);
    EXPECT_EQ(cb.m_allocate, after.m_allocate);
    EXPECT_EQ(cb.m_free, after.m_free);
    EXPECT_EQ(cb.m_error, after.m_error);

    reset_callbacks();
    EXPECT_EQ(before.m_user_data, after.m_user_data);
    EXPECT_EQ(before.m_allocate, after.m_allocate);
    EXPECT_EQ(before.m_free, after.m_free);
    EXPECT_EQ(before.m_error, after.m_error);
}

TEST(Callbacks, eq)
{
    Callbacks before = get_callbacks();
    Callbacks bf2 = get_callbacks();
    Callbacks cb((void*)0xff, &test_allocate_impl, &test_free_impl, &test_error_impl);

    EXPECT_EQ(bf2, before);
    EXPECT_TRUE(bf2 == before);
    EXPECT_FALSE(!(bf2 == before));
    EXPECT_TRUE(!(cb == before));
}

TEST(Callbacks, ne)
{
    Callbacks before = get_callbacks();
    Callbacks bf2 = get_callbacks();
    Callbacks cb((void*)0xff, &test_allocate_impl, &test_free_impl, &test_error_impl);

    EXPECT_NE(cb, before);
    EXPECT_TRUE(cb != before);
    EXPECT_TRUE(!(bf2 != before));
    EXPECT_FALSE(!(cb != before));
}

TEST(Callbacks, cmp_user_data)
{
    Callbacks before = get_callbacks();
    before.m_user_data = (void*)1u;
    Callbacks cp = before;
    EXPECT_EQ(cp, before);
    cp.m_user_data = (void*)(((char*)before.m_user_data) + 100u);
    EXPECT_NE(cp, before);
}

TEST(Callbacks, cmp_allocate)
{
    Callbacks before = get_callbacks();
    Callbacks cp = before;
    EXPECT_NE(cp.m_allocate, nullptr);
    EXPECT_EQ(cp, before);
    cp.m_allocate = nullptr;
    EXPECT_NE(cp, before);
}

TEST(Callbacks, cmp_free)
{
    Callbacks before = get_callbacks();
    Callbacks cp = before;
    EXPECT_NE(cp.m_free, nullptr);
    EXPECT_EQ(cp, before);
    cp.m_free = nullptr;
    EXPECT_NE(cp, before);
}

TEST(Callbacks, cmp_error)
{
    Callbacks before = get_callbacks();
    Callbacks cp = before;
    EXPECT_NE(cp.m_error, nullptr);
    EXPECT_EQ(cp, before);
    cp.m_error = nullptr;
    EXPECT_NE(cp, before);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(allocate_and_free, basic)
{
    EXPECT_NE(get_callbacks().m_allocate, &test_allocate_impl);
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
    Callbacks cb(nullptr, &test_allocate_impl, &test_free_impl, nullptr);
    set_callbacks(cb);
    void *mem = get_callbacks().m_allocate(32, /*hint*/0, get_callbacks().m_user_data);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
    stored_mem = nullptr;
    stored_length = 0;
    get_callbacks().m_free(mem, 32u, get_callbacks().m_user_data);
    EXPECT_EQ(stored_mem, mem);
    EXPECT_EQ(stored_length, 32u);
}

TEST(error, basic)
{
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
    Callbacks cb(nullptr, nullptr, nullptr, &test_error_impl);
    set_callbacks(cb);
    // message
    EXPECT_EQ(get_callbacks().m_error, &test_error_impl);
    c4::yml::error("some message 123"); // calls test_error_impl, which sets stored_msg and stored_location
    EXPECT_EQ(stored_msg, "some message 123");
    EXPECT_EQ(stored_location.name, "");
    EXPECT_EQ(stored_location.offset, 0u);
    EXPECT_EQ(stored_location.line, 0u);
    EXPECT_EQ(stored_location.col, 0u);
    // location
    c4::yml::error("some message 456", Location("file.yml", 433u, 123u, 4u));
    EXPECT_EQ(stored_msg, "some message 456");
    EXPECT_EQ(stored_location.name, "file.yml");
    EXPECT_EQ(stored_location.offset, 433u);
    EXPECT_EQ(stored_location.line, 123u);
    EXPECT_EQ(stored_location.col, 4u);
    reset_callbacks();
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
}

TEST(RYML_CHECK, basic)
{
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
    Callbacks cb(nullptr, nullptr, nullptr, &test_error_impl);
    set_callbacks(cb);
    size_t the_line = __LINE__; RYML_CHECK(false);  // keep both statements in the same line
    EXPECT_EQ(stored_msg, "check failed: false");
    EXPECT_EQ(stored_location.name, __FILE__);
    EXPECT_EQ(stored_location.offset, 0u);
    EXPECT_EQ(stored_location.line, the_line);
    EXPECT_EQ(stored_location.col, 0u);
    reset_callbacks();
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
}


TEST(RYML_ASSERT, basic)
{
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
    Callbacks cb(nullptr, nullptr, nullptr, &test_error_impl);
    set_callbacks(cb);
    stored_msg = "";
    stored_location = {};
    size_t the_line = __LINE__; RYML_ASSERT(false);  // keep both statements in the same line
    #if RYML_USE_ASSERT
    EXPECT_EQ(stored_msg, "check failed: false");
    EXPECT_EQ(stored_location.name, __FILE__);
    EXPECT_EQ(stored_location.offset, 0u);
    EXPECT_EQ(stored_location.line, the_line);
    EXPECT_EQ(stored_location.col, 0u);
    #else
    C4_UNUSED(the_line);
    EXPECT_EQ(stored_msg, "");
    EXPECT_EQ(stored_location.name, nullptr);
    EXPECT_EQ(stored_location.offset, 0u);
    EXPECT_EQ(stored_location.line, 0u);
    EXPECT_EQ(stored_location.col, 0u);
    #endif
    reset_callbacks();
    EXPECT_NE(get_callbacks().m_error, &test_error_impl);
}


// FIXME this is here merely to avoid a linker error
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
