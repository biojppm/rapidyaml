#pragma once
#ifndef TEST_FUZZ_INTS_HPP_
#define TEST_FUZZ_INTS_HPP_

#include "./test_fuzz_common.hpp"

namespace c4 {
namespace yml {

void destroy_ints(extra::ievt::Buffers *ints)
{
    auto cb = get_callbacks();
    if(ints->evts.ptr)
        RYML_CB_FREE_(cb, ints->evts.ptr, extra::ievt::evt_size, static_cast<size_t>(ints->evts.len));
    if(ints->arena.str)
        RYML_CB_FREE_(cb, ints->arena.str, char, ints->arena.len);
    ints->evts = {};
    ints->src = {};
    ints->arena = {};
}
size_t emit_ints(EmitType_e is_yaml, extra::ievt::Buffers const& evts, substr buf, bool error_on_excess)
{
    extra::ievt::EmitterInts<WriterBuf> em(EmitOptions{}, buf);
    em.emit_as(is_yaml, evts.evts.ptr, evts.evts.len, 0, evts.src, evts.arena);
    return em.get_result(error_on_excess).len;
}
void emit_ints(EmitType_e is_yaml, extra::ievt::Buffers const& evts, std::string *dst)
{
    dst->resize(evts.src.len);
    size_t sz = emit_ints(is_yaml, evts, to_substr(*dst), /*err_on_excess*/false);
    if(sz > dst->size())
    {
        dst->resize(sz);
        size_t sz2 = emit_ints(is_yaml, evts, to_substr(*dst), /*err_on_excess*/true);
        ASSERT_FUZZ(sz2 == sz);
    }
    dst->resize(sz);
}
C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC("-Wclobbered")
template<bool resize_buffers> using HandlerInts = extra::ievt::EventHandlerInts<resize_buffers>;
template<bool resize_buffers> using ParserInts = ParseEngine<HandlerInts<resize_buffers>>;
template<bool resize_buffers, class FnParse>
inline int fuzztest_ints(uint32_t case_number, csubstr src, FnParse const& fn_parse, EmitType_e is_yaml)
{
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    using I = typename HandlerInts<resize_buffers>::value_type;
    HandlerInts<resize_buffers> handler{};
    ParserInts<resize_buffers> parser(&handler);
    std::string str(src.begin(), src.end());
    std::string emitted;
    _if_dbg(dbg_printf_("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
    if C4_IF_CONSTEXPR (resize_buffers)
    {
        extra::ievt::Buffers ints{};
        handler.reset(to_substr(str));
        C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
        {
            fn_parse(parser, c4::to_substr(str));
            ASSERT_FUZZ(handler.fits_buffers());
            handler.get_buffers(&ints, /*transfer_ownership*/true);
            emit_ints(is_yaml, ints, &emitted);
            destroy_ints(&ints);
        }
        C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
        {
            // if an exception leaks from here, it is likely because of a greedy noexcept
            _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
            destroy_ints(&ints);
            return 1;
        }
    }
    else
    {
        std::vector<char> arena(str.size());
        std::vector<I> evts;
        evts.reserve(256);
        handler.reset(to_substr(str), to_substr(arena),
                      evts.data(), static_cast<I>(evts.size()));
        C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
        {
            fn_parse(parser, c4::to_substr(str));
            if(!handler.fits_buffers())
            {
                evts.resize(static_cast<size_t>(handler.required_size_events()));
                arena.resize(handler.required_size_arena());
                str.assign(src.begin(), src.end());
                handler.reset(to_substr(str), to_substr(arena),
                              evts.data(), static_cast<I>(evts.size()));
                fn_parse(parser, c4::to_substr(str));
            }
            ASSERT_FUZZ(handler.fits_buffers());
            extra::ievt::Buffers ints{};
            handler.get_buffers(&ints, /*transfer_ownership*/false);
            emit_ints(is_yaml, ints, &emitted);
        }
        C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
        {
            // if an exception leaks from here, it is likely because of a greedy noexcept
            _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
            return 1;
        }
    }
    _if_dbg(dbg_printf_("emitted[{}]: [{}]~~~\n{}\n~~~\n", case_number, emitted.size(), to_csubstr(emitted)); fflush(NULL));
    C4_DONT_OPTIMIZE(emitted);
    return 0;
}
C4_SUPPRESS_WARNING_POP


template<bool resize_buffers>
inline int fuzztest_yaml_ints(uint32_t case_number, csubstr src)
{
    return fuzztest_ints<resize_buffers>(
        case_number,
        src,
        [](ParserInts<resize_buffers> &parser, c4::substr str){
            parser.parse_in_place_ev("input.yaml", c4::to_substr(str));
        },
        EMIT_YAML);
}
template<bool resize_buffers>
inline int fuzztest_json_ints(uint32_t case_number, csubstr src)
{
    return fuzztest_ints<resize_buffers>(
        case_number,
        src,
        [](ParserInts<resize_buffers> &parser, c4::substr str){
            parser.parse_json_in_place_ev("input.json", c4::to_substr(str));
        },
        EMIT_JSON);
}
inline int fuzztest_yaml_ints_resize(uint32_t case_number, csubstr src)
{
    return fuzztest_yaml_ints<true>(case_number, src);
}
inline int fuzztest_yaml_ints_noresize(uint32_t case_number, csubstr src)
{
    return fuzztest_yaml_ints<false>(case_number, src);
}
inline int fuzztest_json_ints_resize(uint32_t case_number, csubstr src)
{
    return fuzztest_json_ints<true>(case_number, src);
}
inline int fuzztest_json_ints_noresize(uint32_t case_number, csubstr src)
{
    return fuzztest_json_ints<false>(case_number, src);
}

} // namespace yml
} // namespace c4

#endif /* TEST_FUZZ_INTS_HPP_ */
