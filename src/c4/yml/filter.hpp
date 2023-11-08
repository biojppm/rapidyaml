#ifndef _C4_YML_FILTER_HPP_
#define _C4_YML_FILTER_HPP_

#include "c4/yml/common.hpp"

namespace c4 {
namespace yml {

/** A CRTP for filtering scalars.
 * @see ScalarFilter for filtering in standalone mode */
template<class Parser>
struct ScalarFilterCRTP
{
    using LocCRef = Location const& C4_RESTRICT;

public:

    /** @name scalar filtering */
    /** @{*/

    /** filter a plain scalar */
    FilterResult filter_plain(csubstr scalar, substr dst, size_t indentation) noexcept;
    /** filter a plain scalar in place */
    FilterResult filter_plain_inplace(substr scalar, size_t cap, size_t indentation) noexcept;

    /** filter a single-quoted scalar */
    FilterResult filter_squoted(csubstr scalar, substr dst) noexcept;
    /** filter a single-quoted scalar in place */
    FilterResult filter_squoted_inplace(substr scalar, size_t cap) noexcept;

    /** filter a double-quoted scalar */
    FilterResult filter_dquoted(csubstr scalar, substr dst, LocCRef loc) noexcept;
    /** filter a double-quoted scalar in place */
    FilterResult filter_dquoted_inplace(substr scalar, size_t cap, LocCRef loc) noexcept;

    /** filter a block-literal scalar */
    FilterResult filter_block_literal(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept;
    /** filter a block-literal scalar in place */
    FilterResult filter_block_literal_inplace(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept;

    /** filter a block-folded scalar */
    FilterResult filter_block_folded(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept;
    /** filter a block-folded scalar in place */
    FilterResult filter_block_folded_inplace(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept;

    /** @} */

public:

    template<class FilterProcessor> FilterResult _filter_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept;
    template<class FilterProcessor> FilterResult _filter_squoted(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> FilterResult _filter_dquoted(FilterProcessor &C4_RESTRICT proc, LocCRef loc) noexcept;
    template<class FilterProcessor> FilterResult _filter_block_literal(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept;
    template<class FilterProcessor> FilterResult _filter_block_folded(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept;

public:

    template<class FilterProcessor> void   _filter_nl_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept;
    template<class FilterProcessor> void   _filter_nl_squoted(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_nl_dquoted(FilterProcessor &C4_RESTRICT proc) noexcept;

    template<class FilterProcessor> bool   _filter_ws_handle_to_first_non_space(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_ws_copy_trailing(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_ws_skip_trailing(FilterProcessor &C4_RESTRICT proc) noexcept;

    template<class FilterProcessor> void   _filter_dquoted_backslash(FilterProcessor &C4_RESTRICT proc, LocCRef loc) noexcept;

    template<class FilterProcessor> void   _filter_chomp(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp, size_t indentation) noexcept;
    template<class FilterProcessor> size_t _handle_all_whitespace(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp) noexcept;
    template<class FilterProcessor> size_t _extend_to_chomp(FilterProcessor &C4_RESTRICT proc, size_t contents_len) noexcept;
    template<class FilterProcessor> void   _filter_block_indentation(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept;
    template<class FilterProcessor> size_t _filter_block_folded_newlines(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len) noexcept;
    template<class FilterProcessor> size_t _filter_block_folded_indented(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len, size_t curr_indentation) noexcept;
};


/** A convenience class to enable filtering scalars in standalone
 * mode, ie without having a parser instance. */
struct ScalarFilter : public ScalarFilterCRTP<ScalarFilter>
{
    ScalarFilter() noexcept : m_callbacks(&get_callbacks()) {}
    ScalarFilter(Callbacks const& cb) noexcept : m_callbacks(&cb) {}
    Callbacks const& callbacks() const noexcept { return *m_callbacks; }
    Callbacks const* m_callbacks;
};


} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_HPP_ */
