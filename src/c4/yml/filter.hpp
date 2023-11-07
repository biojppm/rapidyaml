#ifndef _C4_YML_FILTER_HPP_
#define _C4_YML_FILTER_HPP_

#include "c4/yml/common.hpp"

namespace c4 {
namespace yml {

struct ScalarFilter
{
    using LocCRef = Location const& C4_RESTRICT;

public:

    Callbacks const* m_callbacks; // FIXME use CRTP for this

    ScalarFilter() : m_callbacks(&get_callbacks()) {}
    ScalarFilter(Callbacks const& cb) : m_callbacks(&cb) {}

public:

    /** filter a plain scalar */
    csubstr filter_plain(csubstr scalar, substr dst, size_t indentation);
    /** filter a plain scalar in place */
    csubstr filter_plain(substr scalar, size_t cap, size_t indentation);

    /** filter a single-quoted scalar */
    csubstr filter_squoted(csubstr scalar, substr dst);
    /** filter a single-quoted scalar in place */
    csubstr filter_squoted(substr scalar, size_t cap);

    /** filter a double-quoted scalar */
    csubstr filter_dquoted(csubstr scalar, substr dst, LocCRef loc);
    /** filter a double-quoted scalar in place */
    csubstr filter_dquoted(substr scalar, size_t cap, LocCRef loc);

    /** filter a block literal */
    csubstr filter_block_literal(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp);
    /** filter a block literal in place */
    csubstr filter_block_literal(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp);

    /** filter a block folded scalar */
    csubstr filter_block_folded(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp);
    /** filter a block folded scalar in place */
    csubstr filter_block_folded(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp);

public:

    template<class FilterProcessor> csubstr filter_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation);
    template<class FilterProcessor> csubstr filter_squoted(FilterProcessor &C4_RESTRICT proc);
    template<class FilterProcessor> csubstr filter_dquoted(FilterProcessor &C4_RESTRICT proc, LocCRef loc);
    template<class FilterProcessor> csubstr filter_block_literal(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp);
    template<class FilterProcessor> csubstr filter_block_folded(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp);

public:

    template<class FilterProcessor> void _filter_nl_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation);
    template<class FilterProcessor> void _filter_nl_squoted(FilterProcessor &C4_RESTRICT proc);
    template<class FilterProcessor> void _filter_nl_dquoted(FilterProcessor &C4_RESTRICT proc);

    template<class FilterProcessor> bool _filter_ws_handle_to_first_non_space(FilterProcessor &C4_RESTRICT proc);
    template<class FilterProcessor> void _filter_ws_copy_trailing(FilterProcessor &C4_RESTRICT proc);
    template<class FilterProcessor> void _filter_ws_skip_trailing(FilterProcessor &C4_RESTRICT proc);

    template<class FilterProcessor> void _filter_dquoted_backslash(FilterProcessor &C4_RESTRICT proc, LocCRef loc);

    template<class FilterProcessor> void   _chomp(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp, size_t indentation);
    template<class FilterProcessor> size_t _handle_all_whitespace(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp);
    template<class FilterProcessor> size_t _extend_to_chomp(FilterProcessor &C4_RESTRICT proc, size_t contents_len);
    template<class FilterProcessor> void   _filter_block_indentation(FilterProcessor &C4_RESTRICT proc, size_t indentation);
    template<class FilterProcessor> size_t _filter_block_folded_newlines(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len);
    template<class FilterProcessor> size_t _filter_block_folded_indented(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len, size_t curr_indentation);
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_HPP_ */
