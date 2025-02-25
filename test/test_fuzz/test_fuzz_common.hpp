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


#ifdef RYML_DBG
#define _if_dbg(...) __VA_ARGS__
bool report_errors = true;
#else
#define _if_dbg(...)
bool report_errors = false;
#endif

// watchout: VS2022 requires C4_NORETURN to come before inline
C4_NORETURN inline void errcallback(const char *msg, size_t msg_len, void *)
{
    C4_IF_EXCEPTIONS(
        throw std::runtime_error({msg, msg_len});
        ,
        jmp_msg.assign(msg, msg_len);
        std::longjmp(jmp_env, 1);
    );
}

inline c4::yml::Callbacks create_custom_callbacks()
{
    c4::set_error_flags(c4::ON_ERROR_CALLBACK);
    c4::set_error_callback([](const char *msg, size_t msg_len){
        errcallback(msg, msg_len, nullptr);
    });
    return c4::yml::Callbacks{}
        .set_error_basic([](const char *msg, size_t msg_len, c4::yml::Location const& cpploc, void *){
            if(report_errors)
                c4::yml::err_basic_print(msg, msg_len, cpploc, stderr);
            errcallback(msg, msg_len, nullptr);
        })
        .set_error_parse([](const char *msg, size_t msg_len, c4::yml::Location const& cpploc, c4::yml::Location const& ymlloc, void *){
            if(report_errors)
                c4::yml::err_parse_print(msg, msg_len, cpploc, ymlloc, stderr);
            errcallback(msg, msg_len, nullptr);
        })
        .set_error_visit([](const char *msg, size_t msg_len, c4::yml::Location const& cpploc, c4::yml::Tree const* tree, c4::yml::id_type id, void *){
            if(report_errors)
                c4::yml::err_visit_print(msg, msg_len, cpploc, tree, id, stderr);
            errcallback(msg, msg_len, nullptr);
        });
}

namespace c4 {
namespace yml {

inline int fuzztest_parse_emit(uint32_t case_number, csubstr src)
{
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    Tree tree(create_custom_callbacks());
    bool parse_success = false;
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        RYML_ASSERT_BASIC(tree.empty());
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        parse_in_arena(src, &tree);
        parse_success = true;
        _if_dbg(print_tree("parsed tree", tree));
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        std::string dst = emitrs_yaml<std::string>(tree);
        _if_dbg(_dbg_printf("emitted[{}]: [{}]~~~\n{}\n~~~\n", case_number, dst.size(), to_csubstr(dst)); fflush(NULL));
        C4_DONT_OPTIMIZE(dst);
        C4_DONT_OPTIMIZE(parse_success);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        // if an exception leaks from here, it is likely because of a greedy noexcept
        _if_dbg(if(parse_success) print_tree("parsed tree", tree));
        return 1;
    }
    return 0;
}

inline int fuzztest_yaml_events(uint32_t case_number, csubstr src)
{
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    EventHandlerYamlStd::EventSink sink = {};
    EventHandlerYamlStd handler(&sink, create_custom_callbacks());
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    std::string str(src.begin(), src.end());
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        parser.parse_in_place_ev("input", c4::to_substr(str));
        _if_dbg(_dbg_printf("evts[{}]: ~~~\n{}\n~~~\n", case_number, sink); fflush(NULL));
        C4_DONT_OPTIMIZE(sink);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        // if an exception leaks from here, it is likely because of a greedy noexcept
        _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
        return 1;
    }
    return 0;
}

} // namespace yml
} // namespace c4

#endif /* TEST_FUZZ_COMMON_H */
