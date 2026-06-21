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
#endif
#include <testsuite/testsuite_events.hpp>
#include <c4/yml/extra/event_handler_ints.hpp>
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
        fwrite(s.str, 1, s.len, stderr);
        fflush(stderr);
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

template<class FnParse, class FnEmit>
inline int fuzztest_tree(uint32_t case_number, csubstr src, FnParse fn_parse, FnEmit fn_emit)
{
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    Tree tree(create_custom_callbacks());
    bool parse_success = false;
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        fn_parse(src, &tree, ParserOptions{});
        parse_success = true;
        _if_dbg(print_tree("parsed tree", tree));
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        std::string dst = fn_emit(tree, EmitOptions{});
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

using FnTreeParse = void (*)(csubstr yaml, Tree *, ParserOptions const&);
using FnTreeEmit = std::string (*)(Tree const&, EmitOptions const&);
inline int fuzztest_yaml_tree(uint32_t case_number, csubstr src)
{
    FnTreeParse fn_parse = &parse_in_arena;
    FnTreeEmit fn_emit = &emitrs_yaml<std::string>;
    return fuzztest_tree(case_number, src, fn_parse, fn_emit);
}
inline int fuzztest_json_tree(uint32_t case_number, csubstr src)
{
    FnTreeParse fn_parse = &parse_json_in_arena;
    FnTreeEmit fn_emit = &emitrs_json<std::string>;
    return fuzztest_tree(case_number, src, fn_parse, fn_emit);
}


using HandlerInts = extra::EventHandlerInts;
using ParserInts = ParseEngine<HandlerInts>;
template<class FnParse>
inline int fuzztest_ints(uint32_t case_number, csubstr src, FnParse const& fn)
{
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    using I = HandlerInts::value_type;
    HandlerInts handler{};
    ParserInts parser(&handler);
    std::string str(src.begin(), src.end());
    std::vector<char> arena(str.size());
    std::vector<I> evts;
    evts.reserve(256);
    handler.reset(to_substr(str), to_substr(arena),
                  evts.data(), static_cast<I>(evts.size()));
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        fn(parser, c4::to_substr(str));
        C4_DONT_OPTIMIZE(evts);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        // if an exception leaks from here, it is likely because of a greedy noexcept
        _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
        return 1;
    }
    return 0;
}

inline int fuzztest_yaml_ints(uint32_t case_number, csubstr src)
{
    return fuzztest_ints(
        case_number,
        src,
        [](ParserInts &parser, c4::substr str){
            parser.parse_in_place_ev("input.yaml", c4::to_substr(str));
        });
}
inline int fuzztest_json_ints(uint32_t case_number, csubstr src)
{
    return fuzztest_ints(
        case_number,
        src,
        [](ParserInts &parser, c4::substr str){
            parser.parse_json_in_place_ev("input.json", c4::to_substr(str));
        });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace {

template<class T>
inline bool fuzztest_serialize_cleanup(csubstr src, csubstr *filtered)
{
    if C4_IF_CONSTEXPR (std::is_arithmetic<T>::value)
    {
        src = src.triml(" \t\n\r+");
        if C4_IF_CONSTEXPR (std::is_unsigned<T>::value)
        {
            src = src.triml("-");
        }
    }
    *filtered = src;
    return true;
}


template<class T>
inline int fuzztest_serialize(std::string *workspace, csubstr src, T val={})
{
    bool ok = fuzztest_serialize_cleanup<T>(src, &src);
    if(!ok)
        return false;
    ok = scalar_deserialize(src, &val);
    if(ok)
    {
        size_t sz = 0;
    again:
        scalar_serialize(c4::to_substr(*workspace), val);
        if(sz > workspace->size())
        {
            workspace->resize(sz);
            goto again; // NOLINT
        }
    }
    return 0;
}
}

inline int fuzztest_serialize(uint32_t case_number, csubstr src)
{
    std::string workspace;
    C4_UNUSED(case_number);
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(_dbg_printf("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        fuzztest_serialize<uint8_t>(&workspace, src);
        fuzztest_serialize<uint16_t>(&workspace, src);
        fuzztest_serialize<uint32_t>(&workspace, src);
        fuzztest_serialize<uint64_t>(&workspace, src);
        fuzztest_serialize<int8_t>(&workspace, src);
        fuzztest_serialize<int16_t>(&workspace, src);
        fuzztest_serialize<int32_t>(&workspace, src);
        fuzztest_serialize<int64_t>(&workspace, src);
        fuzztest_serialize<float>(&workspace, src);
        fuzztest_serialize<double>(&workspace, src);
        fuzztest_serialize<std::string>(&workspace, src);
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
