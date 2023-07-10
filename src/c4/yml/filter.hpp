#ifndef _C4_YML_FILTER_HPP_
#define _C4_YML_FILTER_HPP_

#include "c4/yml/common.hpp"

namespace c4 {
namespace yml {

struct ScalarFilterProcessor
{
    using LocCRef = Location const& C4_RESTRICT;

    csubstr filter_plain(substr scalar, size_t indentation, LocCRef loc);
    csubstr filter_plain(csubstr scalar, substr dst, size_t indentation, LocCRef loc);

    csubstr filter_squoted(substr scalar, LocCRef loc);
    csubstr filter_squoted(csubstr scalar, substr dst, LocCRef loc);

    csubstr filter_dquoted(substr scalar, LocCRef loc);
    csubstr filter_dquoted(csubstr scalar, substr dst, LocCRef loc);

    csubstr filter_block_literal(substr scalar, size_t indentation, BlockChomp_e chomp, LocCRef loc);
    csubstr filter_block_literal(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp, LocCRef loc);

    csubstr filter_block_folded(substr scalar, size_t indentation, BlockChomp_e chomp, LocCRef loc);
    csubstr filter_block_folded(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp, LocCRef loc);

public:

    Callbacks const* m_callbacks; // FIXME use CRTP for this

    ScalarFilterProcessor() : m_callbacks(&get_callbacks()) {}
    ScalarFilterProcessor(Callbacks const& cb) : m_callbacks(&cb) {}

public:

    template<bool backslash_is_escape, bool keep_trailing_whitespace>
    bool _filter_nl(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos, size_t indentation);

    template<bool keep_trailing_whitespace>
    void _filter_ws(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos);

    bool _apply_chomp(csubstr buf, substr dst, size_t *C4_RESTRICT pos, BlockChomp_e chomp, LocCRef loc);

};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_HPP_ */
