#ifndef _C4_YML_FILTER_HPP_
#define _C4_YML_FILTER_HPP_

#include "c4/yml/common.hpp"

namespace c4 {
namespace yml {

struct ScalarFilterProcessor
{
    substr filter_plain(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_plain(csubstr scalar, substr dst, size_t indentation, Location const* loc=nullptr);

    substr filter_squoted(substr scalar, Location const* loc=nullptr);
    substr filter_squoted(csubstr scalar, substr dst, Location const* loc=nullptr);

    substr filter_dquoted(substr scalar, Location const* loc=nullptr);
    substr filter_dquoted(csubstr scalar, substr dst, Location const* loc=nullptr);

    substr filter_block_literal(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_block_literal(csubstr scalar, substr dst, size_t indentation, Location const* loc=nullptr);

    substr filter_block_folded(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_block_folded(csubstr scalar, substr dst, size_t indentation, Location const* loc=nullptr);

public:

    Callbacks const* m_callbacks; // FIXME use CRTP for this

    ScalarFilterProcessor() : m_callbacks(&get_callbacks()) {}
    ScalarFilterProcessor(Callbacks const& cb) : m_callbacks(&cb) {}

public:

    template<bool backslash_is_escape, bool keep_trailing_whitespace>
    bool _filter_nl(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos, size_t indentation);

    template<bool keep_trailing_whitespace>
    void _filter_ws(csubstr r, substr dst, size_t *C4_RESTRICT i, size_t *C4_RESTRICT pos)

};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_HPP_ */
