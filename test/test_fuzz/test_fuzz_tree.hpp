#pragma once
#ifndef TEST_FUZZ_TREE_HPP_
#define TEST_FUZZ_TREE_HPP_

#include "./test_fuzz_common.hpp"

namespace c4 {
namespace yml {

template<class FnParse, class FnEmit>
inline int fuzztest_tree(uint32_t case_number, csubstr src, FnParse fn_parse, FnEmit fn_emit, bool resolve)
{
    C4_UNUSED(case_number);
    Callbacks cb = create_custom_callbacks();
    set_callbacks(cb);
    Tree tree(cb);
    bool parse_success = false;
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        _if_dbg(dbg_printf_("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        fn_parse(src, &tree, ParserOptions{});
        parse_success = true;
        _if_dbg(print_tree("parsed tree", tree));
        if(resolve)
        {
            tree.resolve(ResolveOptions{}.max_size_multiplier(2u));
            _if_dbg(print_tree("resolved tree", tree));
        }
        _if_dbg(dbg_printf_("in[{}]: [{}]~~~\n{}\n~~~\n", case_number, src.len, src); fflush(NULL));
        std::string dst = fn_emit(tree, EmitOptions{});
        _if_dbg(dbg_printf_("emitted[{}]: [{}]~~~\n{}\n~~~\n", case_number, dst.size(), to_csubstr(dst)); fflush(NULL));
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
    return fuzztest_tree(case_number, src, fn_parse, fn_emit, /*resolve*/true);
}
inline int fuzztest_json_tree(uint32_t case_number, csubstr src)
{
    FnTreeParse fn_parse = &parse_json_in_arena;
    FnTreeEmit fn_emit = &emitrs_json<std::string>;
    return fuzztest_tree(case_number, src, fn_parse, fn_emit, /*resolve*/false);
}

} // namespace yml
} // namespace c4

#endif /* TEST_FUZZ_TREE_HPP_ */
