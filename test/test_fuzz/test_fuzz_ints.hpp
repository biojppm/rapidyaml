#pragma once
#ifndef TEST_FUZZ_INTS_HPP_
#define TEST_FUZZ_INTS_HPP_

#include "./test_fuzz_common.hpp"

namespace c4 {
namespace yml {

template<bool resize_buffers> using HandlerInts = extra::ievt::EventHandlerInts<resize_buffers>;
template<bool resize_buffers> using ParserInts = ParseEngine<HandlerInts<resize_buffers>>;

namespace detail {
inline void init_src(extra::ievt::Buffers *ints, std::string *srcbuf, csubstr src)
{
    srcbuf->assign(src.begin(), src.end());
    ints->src = to_substr(*srcbuf);
}
template<bool resize_buffers>
inline void resize_bufs(extra::ievt::Buffers *ints, HandlerInts<resize_buffers> const& handler)
{
    if C4_IF_CONSTEXPR (!resize_buffers)
    {
        ints->arena = detail::resize(ints->arena, handler.required_size_arena(), get_callbacks());
        ints->evts = extra::ievt::detail::resize(ints->evts, handler.required_size_events(), get_callbacks());
    }
}
inline void destroy_ints(extra::ievt::Buffers *ints)
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
inline size_t emit_ints(EmitType_e is_yaml, extra::ievt::Buffers const& ints, substr buf, bool error_on_excess)
{
    extra::ievt::EmitterInts<WriterBuf> em(EmitOptions{}, buf);
    em.emit_as(is_yaml, ints.evts.ptr, ints.evts.len, 0, ints.src, ints.arena);
    return em.get_result(error_on_excess).len;
}
inline void emit_ints(EmitType_e is_yaml, extra::ievt::Buffers const& ints, std::string *dst)
{
    dst->resize(ints.src.len);
    size_t sz = emit_ints(is_yaml, ints, to_substr(*dst), /*err_on_excess*/false);
    if(sz > dst->size())
    {
        dst->resize(sz);
        size_t sz2 = emit_ints(is_yaml, ints, to_substr(*dst), /*err_on_excess*/true);
        ASSERT_FUZZ(sz2 == sz);
    }
    dst->resize(sz);
}
template<bool resize_buffers, class FnParse>
inline int fuzztest_ints(uint32_t case_number, csubstr src, FnParse const& fn_parse, EmitType_e is_yaml)
{
    _if_dbg(dbg_printf_("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
    C4_UNUSED(case_number);
    set_callbacks(create_custom_callbacks());
    HandlerInts<resize_buffers> handler{};
    ParserInts<resize_buffers> parser(&handler);
    extra::ievt::Buffers ints{};
    std::string srcbuf;
    init_src(&ints, &srcbuf, src);
    std::string emitted;
    auto doemit = [&]{
        emit_ints(is_yaml, ints, &emitted);
        _if_dbg(dbg_printf_("emitted[{}]: [{}]~~~\n{}\n~~~\n", case_number, emitted.size(), to_csubstr(emitted)); fflush(NULL));
        C4_DONT_OPTIMIZE(emitted);
    };
    int status = 0;
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        handler.reset(ints); // start with empty buffers
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            fn_parse(parser, ints.src);
            ASSERT_FUZZ(handler.fits_buffers());
            handler.get_buffers(&ints, /*transfer_ownership*/true);
            doemit();
        }
        else
        {
            fn_parse(parser, ints.src);
            if(!handler.fits_buffers())
            {
                init_src(&ints, &srcbuf, src);
                resize_bufs(&ints, handler);
                handler.reset(ints);
                fn_parse(parser, ints.src);
                ASSERT_FUZZ(handler.fits_buffers());
            }
            doemit();
        }
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)  // if an exception leaks from here, it is likely because of a greedy noexcept
    {
        _if_dbg(fprintf(stdout, "err\n"); fflush(NULL));
        status = 1;
    }
    destroy_ints(&ints);
    return status;
}
} // namespace detail


template<bool resize_buffers>
inline int fuzztest_yaml_ints(uint32_t case_number, csubstr src)
{
    return detail::fuzztest_ints<resize_buffers>(
        case_number,
        src,
        [](ParserInts<resize_buffers> &parser, substr str){
            parser.parse_in_place_ev("input.yaml", str);
        },
        EMIT_YAML);
}
template<bool resize_buffers>
inline int fuzztest_json_ints(uint32_t case_number, csubstr src)
{
    return detail::fuzztest_ints<resize_buffers>(
        case_number,
        src,
        [](ParserInts<resize_buffers> &parser, substr str){
            parser.parse_json_in_place_ev("input.json", str);
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
