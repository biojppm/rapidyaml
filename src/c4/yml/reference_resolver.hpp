#ifndef _C4_YML_REFERENCE_RESOLVER_HPP_
#define _C4_YML_REFERENCE_RESOLVER_HPP_

#include "c4/yml/tree.hpp"
#include "c4/yml/detail/stack.hpp"


namespace c4 {
namespace yml {

struct RYML_EXPORT ReferenceResolver
{
    ReferenceResolver() = default;

    void resolve(Tree *t_);
    void reset(Tree *t_);

public:

    struct RefData;

    void gather_anchors_and_refs_();
    void gather_anchors_and_refs__(size_t n);
    size_t count_anchors_and_refs_(size_t n);

    size_t lookup_(RefData *C4_RESTRICT ra);

public:

    struct RefData
    {
        NodeType type;
        size_t node;
        size_t prev_anchor;
        size_t target;
        size_t parent_ref;
        size_t parent_ref_sibling;
    };

    Tree *C4_RESTRICT m_tree;
    /** We're using this stack purely as an array. */
    detail::stack<RefData> m_refs;

};


} // namespace ryml
} // namespace c4


#endif // _C4_YML_REFERENCE_RESOLVER_HPP_
