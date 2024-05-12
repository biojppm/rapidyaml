#pragma once
#ifndef TEST_FUZZ_COMMON_H
#define TEST_FUZZ_COMMON_H

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/event_handler_tree.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif
#include <test_suite/test_suite_events.hpp>
#include <test_suite/test_suite_event_handler.hpp>
#include <cstdio>

#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif

inline bool report_errors = false;
inline void report_error(const char* msg, size_t length, c4::yml::Location loc, FILE *f)
{
    if(!report_errors)
        return;
    if(!loc.name.empty())
    {
        fwrite(loc.name.str, 1, loc.name.len, f);
        fputc(':', f);
    }
    fprintf(f, "%zu:", loc.line);
    if(loc.col)
        fprintf(f, "%zu:", loc.col);
    if(loc.offset)
        fprintf(f, " (%zuB):", loc.offset);
    fputc(' ', f);
    fprintf(f, "%.*s\n", static_cast<int>(length), msg);
    fflush(f);
}

inline C4_NORETURN void errcallback(const char *msg, size_t msg_len, c4::yml::Location location, void *)
{
printf("aqui 5: %.*s\n", (int)msg_len, msg);fflush(stdout);
    report_error(msg, msg_len, location, stderr);
    C4_IF_EXCEPTIONS(
printf("aqui 6: \n");fflush(stdout);
        throw std::runtime_error({msg, msg_len});
        ,
        jmp_msg.assign(msg, msg_len);
        std::longjmp(jmp_env, 1);
    );
};

inline c4::yml::Callbacks create_custom_callbacks()
{
    c4::set_error_flags(c4::ON_ERROR_CALLBACK);
    c4::set_error_callback([](const char *msg, size_t msg_len){
        errcallback(msg, msg_len, {}, nullptr);
    });
    c4::yml::Callbacks callbacks = {};
    callbacks.m_error = errcallback;
    return callbacks;
}

std::string fuzz_subject(c4::csubstr src);

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *str, size_t len)
{
    c4::yml::set_callbacks(create_custom_callbacks());
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        c4::csubstr src = {reinterpret_cast<const char*>(str), len};
        #ifdef RYML_DBG
        fprintf(stdout, "in: ~~~\n%.*s\n~~~\n", static_cast<int>(src.len), src.str); fflush(stdout);
        #else
        static int count = 0;
        fprintf(stdout, "%d: ", count++);
        #endif
        std::string out = fuzz_subject(src);
        #ifdef RYML_DBG
        fprintf(stdout, "out: ~~~\n%s\n~~~\n", out.c_str()); fflush(stdout);
        #else
        fprintf(stdout, "ok!\n"); fflush(stdout);
        #endif
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        fprintf(stdout, "err\n"); fflush(stdout);
printf("aqui 7: \n");fflush(stdout);
        return 1;
    }
    return 0;
}

#endif /* TEST_FUZZ_COMMON_H */
