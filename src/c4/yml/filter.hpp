#ifndef _C4_YML_FILTER_HPP_
#define _C4_YML_FILTER_HPP_

#include "c4/yml/common.hpp"

namespace c4 {
namespace yml {

struct ScalarFilterProcessor
{
    Callbacks const* m_callbacks;
    ScalarFilterProcessor() : m_callbacks(&get_callbacks()) {}
    ScalarFilterProcessor(Callbacks const& cb) : m_callbacks(&cb) {}

    substr filter_squoted(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_dquoted(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_block_literal(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_block_folded(substr scalar, size_t indentation, Location const* loc=nullptr);
    substr filter_plain(substr scalar, size_t indentation, Location const* loc=nullptr);
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_HPP_ */
