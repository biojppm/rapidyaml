#include "c4/yml/common.hpp"

#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS
#ifdef _RYML_EXCEPTIONS
#       include <stdexcept>
#endif


namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702/*unreachable code*/) // on the call to the unreachable macro

namespace {
Callbacks s_default_callbacks;

#ifndef RYML_NO_DEFAULT_CALLBACKS

[[noreturn]] void error_basic_impl(const char* msg, size_t length, Location const& ryml_location, void * /*user_data*/)
{
    err_basic_print(msg, length, ryml_location, nullptr);
    #ifdef _RYML_WITH_EXCEPTIONS
    throw BasicException(msg, length, ryml_location);
    #else
    abort(); // LCOV_EXCL_LINE
    #endif
}

[[noreturn]] void error_parse_impl(const char* msg, size_t length, Location const& ryml_location, Location const& yaml_location, void * /*user_data*/)
{
    err_parse_print(msg, length, ryml_location, yaml_location, nullptr);
    #ifdef _RYML_WITH_EXCEPTIONS
    throw ParseException(msg, length, ryml_location, yaml_location);
    #else
    abort(); // LCOV_EXCL_LINE
    #endif
}

[[noreturn]] void error_visit_impl(const char* msg, size_t length, Location const& ryml_location, Tree const* tree, id_type id, void * /*user_data*/)
{
    err_visit_print(msg, length, ryml_location, tree, id, nullptr);
    #ifdef _RYML_WITH_EXCEPTIONS
    throw VisitException(msg, length, ryml_location, tree, id);
    #else
    abort(); // LCOV_EXCL_LINE
    #endif
}

void* allocate_impl(size_t length, void * /*hint*/, void * /*user_data*/)
{
    void *mem = ::malloc(length);
    if(mem == nullptr)
    {
        const char msg[] = "could not allocate memory";
        error_basic_impl(msg, sizeof(msg)-1, RYML_LOC_HERE(), nullptr);
    }
    return mem;
}

void free_impl(void *mem, size_t /*length*/, void * /*user_data*/)
{
    ::free(mem);
}

#endif // RYML_NO_DEFAULT_CALLBACKS

} // anon namespace


void set_callbacks(Callbacks const& c)
{
    s_default_callbacks = c;
}

Callbacks const& get_callbacks()
{
    return s_default_callbacks;
}

void reset_callbacks()
{
    set_callbacks(Callbacks());
}


Callbacks::Callbacks() noexcept
    :
    m_user_data(nullptr),
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_allocate(allocate_impl),
    m_free(free_impl),
    m_error_basic(error_basic_impl),
    m_error_parse(error_parse_impl),
    m_error_visit(error_visit_impl)
    #else
    m_allocate(nullptr),
    m_free(nullptr),
    m_error_basic(nullptr),
    m_error_parse(nullptr),
    m_error_visit(nullptr)
    #endif
{
}

Callbacks::Callbacks(void *user_data, pfn_allocate alloc_, pfn_free free_, pfn_error_basic error_basic_)
    :
    m_user_data(user_data),
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_allocate(alloc_ ? alloc_ : allocate_impl),
    m_free(free_ ? free_ : free_impl),
    m_error_basic(error_basic_ ? error_basic_ : error_basic_impl),
    m_error_parse(error_parse_impl),
    m_error_visit(error_visit_impl)
    #else
    m_allocate(alloc_),
    m_free(free_),
    m_error_basic(error_)
    m_error_parse(nullptr)
    m_error_visit(nullptr)
    #endif
{
    RYML_CHECK_BASIC(m_allocate);
    RYML_CHECK_BASIC(m_free);
    RYML_CHECK_BASIC(m_error_basic);
}


Callbacks& Callbacks::set_user_data(void* user_data)
{
    m_user_data = user_data;
    return *this;
}

Callbacks& Callbacks::set_allocate(pfn_allocate allocate)
{
    m_allocate = allocate;
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_allocate = m_allocate ? m_allocate : allocate_impl;
    #endif
    return *this;
}

Callbacks& Callbacks::set_free(pfn_free free)
{
    m_free = free;
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_free = m_free ? m_free : free_impl;
    #endif
    return *this;
}

Callbacks& Callbacks::set_error_basic(pfn_error_basic error_basic)
{
    m_error_basic = error_basic;
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_error_basic = m_error_basic ? m_error_basic : error_basic_impl;
    #endif
    return *this;
}

Callbacks& Callbacks::set_error_parse(pfn_error_parse error_parse)
{
    m_error_parse = error_parse;
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_error_parse = m_error_parse ? m_error_parse : error_parse_impl;
    #endif
    return *this;
}

Callbacks& Callbacks::set_error_visit(pfn_error_visit error_visit)
{
    m_error_visit = error_visit;
    #ifndef RYML_NO_DEFAULT_CALLBACKS
    m_error_visit = m_error_visit ? m_error_visit : error_visit_impl;
    #endif
    return *this;
}


C4_NORETURN C4_NO_INLINE void err_basic(Callbacks const& callbacks, Location const& cpploc, const char* msg_)
{
    csubstr msg = to_csubstr(msg_);
    callbacks.m_error_basic(msg.str, msg.len, cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}

C4_NORETURN C4_NO_INLINE void err_basic(Location const& cpploc, const char* msg)
{
    err_basic(get_callbacks(), cpploc, msg);
    C4_UNREACHABLE_AFTER_ERR();
}


C4_NORETURN C4_NO_INLINE void err_parse(Callbacks const& callbacks, Location const& cpploc, Location const& ymlloc, const char *msg_)
{
    csubstr msg = to_csubstr(msg_);
    if(callbacks.m_error_parse)
        callbacks.m_error_parse(msg.str, msg.len, cpploc, ymlloc, callbacks.m_user_data);
    // fall to basic error if there is no parse handler set
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg.str, msg.len, cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}

C4_NORETURN C4_NO_INLINE void err_parse(Location const& cpploc, Location const& ymlloc, const char *msg)
{
    err_parse(get_callbacks(), cpploc, ymlloc, msg);
    C4_UNREACHABLE_AFTER_ERR();
}


C4_NORETURN C4_NO_INLINE void err_visit(Callbacks const& callbacks, Location const& cpploc, Tree const* tree, id_type node, const char *msg_)
{
    csubstr msg = to_csubstr(msg_);
    if(callbacks.m_error_visit)
        callbacks.m_error_visit(msg.str, msg.len, cpploc, tree, node, callbacks.m_user_data);
    // fall to basic error if there is no visit handler set
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg.str, msg.len, cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}

C4_NORETURN C4_NO_INLINE void err_visit(Location const& cpploc, Tree const* tree, id_type node, const char *msg)
{
    err_visit(get_callbacks(), cpploc, tree, node, msg);
    C4_UNREACHABLE_AFTER_ERR();
}


// using fwrite() is more portable than using fprintf("%.*s") which
// is not available in some embedded platforms

C4_NO_INLINE static void dump2file(csubstr s, FILE *f)
{
    if(s.len)
        fwrite(s.str, 1, s.len, f); // NOLINT
}

void location_print(Location const& loc, FILE *f)
{
    if(!f)
        f = stderr;
    location_msg(loc, [f](csubstr s){ dump2file(s, f); });
}

void err_basic_print(const char* msg, size_t length, Location const& cpploc, FILE *f)
{
    if(!f)
        f = stderr;
    err_basic_fmt(msg, length, cpploc, [f](csubstr s){ dump2file(s, f); });
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}

void err_parse_print(const char* msg, size_t length, Location const& cpploc, Location const& ymlloc, FILE *f)
{
    if(!f)
        f = stderr;
    err_parse_fmt(msg, length, cpploc, ymlloc, [f](csubstr s){ dump2file(s, f); });
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}

void err_visit_print(const char* msg, size_t length, Location const& cpploc, Tree const* tree, id_type id, FILE *f)
{
    if(!f)
        f = stderr;
    err_visit_fmt(msg, length, cpploc, tree, id, [f](csubstr s){ dump2file(s, f); });
    fputc('\n', f); // NOLINT
    fflush(f); // NOLINT
}


#ifdef _RYML_WITH_EXCEPTIONS
BasicException::BasicException(const char *msg_, size_t len_, Location const& loc) noexcept
    : cpploc(loc)
    , msg()
{
    msg[0] = '\0';
    if(len_)
    {
        if(len_ >= sizeof(msg))
        {
            static_assert(sizeof(msg) > 4u, "message buffer too small");
            len_ = sizeof(msg) - 4u;
            msg[sizeof(msg) - 4u] = '.';
            msg[sizeof(msg) - 3u] = '.';
            msg[sizeof(msg) - 2u] = '.';
            msg[sizeof(msg) - 1u] = '\0';
        }
        memcpy(msg, msg_, len_);
    }
}
ParseException::ParseException(const char *msg_, size_t len_, Location const& cpploc_, Location const& ymlloc_) noexcept
    : BasicException(msg_, len_, cpploc_)
    , ymlloc(ymlloc_)
{
}
VisitException::VisitException(const char *msg_, size_t len_, Location const& cpploc_, Tree const* tree_, id_type node_) noexcept
    : BasicException(msg_, len_, cpploc_)
    , tree(tree_)
    , node(node_)
{
}
#endif // _RYML_WITH_EXCEPTIONS


C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4
