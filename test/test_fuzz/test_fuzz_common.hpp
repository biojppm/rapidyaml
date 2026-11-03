#pragma once
#ifndef TEST_FUZZ_COMMON_H
#define TEST_FUZZ_COMMON_H

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/error.def.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/event_handler_tree.hpp>
#include <c4/yml/parse_engine.def.hpp>
#include <c4/yml/writer_buf.hpp>
#include <c4/yml/resolve_options.hpp>
#endif
#include <testsuite/testsuite_events.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/emitter_ints.hpp>
#include <c4/yml/extra/emitter_ints.def.hpp>
#include <c4/yml/extra/ints_reference_resolver.hpp>
#include <cstdio>

#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif


#ifdef RYML_DBG
#define _if_dbg(...) __VA_ARGS__
bool report_errors = true;
#else
#define _if_dbg(...)
bool report_errors = false;
#endif

#define ASSERT_FUZZ(cond)                       \
    if(!(cond))                                 \
    {                                           \
        dump2stderr("panic: " #cond "\n");      \
        abort();                                \
    }

// watchout: VS2022 requires C4_NORETURN to come before inline
[[noreturn]] void throwerr(c4::csubstr msg)
{
    C4_IF_EXCEPTIONS(
        throw std::runtime_error({msg.str, msg.len});
        ,
        jmp_msg.assign(msg.str, msg.len);
        std::longjmp(jmp_env, 1);
        );
    C4_UNREACHABLE_AFTER_ERR();
}

void dump2stderr(c4::csubstr s)
{
    if(s.len)
    {
        (void)fwrite(s.str, 1, s.len, stderr);
        (void)fflush(stderr);
    }
}

inline c4::yml::Callbacks create_custom_callbacks()
{
    c4::set_error_flags(c4::ON_ERROR_CALLBACK);
    c4::set_error_callback([](const char *msg, size_t msg_len){
        throwerr(c4::csubstr{msg, msg_len});
    });
    return c4::yml::Callbacks{}
        .set_allocate([](size_t len, void* , void *){
            return malloc(len);
        })
        .set_free([](void* mem, size_t, void *){
            free(mem);
        })
        .set_error_basic([](c4::csubstr msg, c4::yml::ErrorDataBasic const& errdata, void *){
            if(report_errors)
                c4::yml::err_basic_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })
        .set_error_parse([](c4::csubstr msg, c4::yml::ErrorDataParse const& errdata, void *){
            if(report_errors)
                c4::yml::err_parse_format(dump2stderr, msg, errdata);
            throwerr(msg);
        })
        .set_error_visit([](c4::csubstr msg, c4::yml::ErrorDataVisit const& errdata, void *){
            if(report_errors)
                c4::yml::err_visit_format(dump2stderr, msg, errdata);
            throwerr(msg);
        });
}

namespace c4 {
namespace yml {

} // namespace yml
} // namespace c4

#endif /* TEST_FUZZ_COMMON_H */
